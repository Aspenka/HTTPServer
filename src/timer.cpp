#include "timer.h"
#include <assert.h>
#include <limits>
#include <iostream>
#include <stdio.h>
#include <QTimerEvent>
#include <QDateTime>
#include <QDebug>
#include <stdlib.h>

/*======================================================
конструктор класса
======================================================*/
Timer::Timer(QString cron, QString urlString, QString uid, int index, QObject *parent) : QObject(parent)
{
    connect(&parser, SIGNAL(isSingle(bool)), this, SLOT(setSingleShot(bool)));
    cronJob = cron;
    timerIndex = index;
    scheduleUid = uid;
    url = urlString;
}

/*======================================================
конструктор копирования класса
======================================================*/
Timer::Timer(const Timer &obj, QObject *parent) : QObject(parent)
{
    parser = obj.parser;
    cronJob = obj.cronJob;
    timerIndex = obj.timerIndex;
    timerStart = obj.timerStart;
    singShot = obj.singShot;
    nextExec = obj.nextExec;
    pause = obj.pause;
    url = obj.url;
    scheduleUid = obj.scheduleUid;
}

/*======================================================
мтеод устанавливает состояние синглшота
Аргументы метода:
    singleShot - переменная, определяющая, вызовется
                 таймер один раз или несколько
======================================================*/
void Timer::setSingleShot(bool singleShot)
{
    if( (singleShot == true) && (pause != true))
    {
        singShot = singleShot;
    }
}

/*======================================================
метод запускает таймер
Аргументы метода:
    cron - строка-cron_выражение
    index - порядковый индекс задачи в списке
======================================================*/
void Timer::start()
{
    std::lock_guard<std::mutex> lock(mtxNextExec);

    if(pause != false)
    {
        time_t time = date.toTime_t();
        nextExec = parser.getDateTime(cronJob, date).toTime_t();
        if(nextExec > time)
        {
        while(time < QDateTime::currentDateTime().toTime_t())
        {
            nextExec = parser.getDateTime(cronJob, date).toTime_t();
            time_t temp;
            emit timeout(timerIndex);
            temp = calcDiffTime();
            time = time + temp;
        }
        }
        pause = false;
    }
    date = QDateTime::currentDateTime();
    nextExec = parser.getDateTime(cronJob, date).toTime_t();
    timerStart = startTimer(calcDiffTime());
    assert(timerStart);
}

/*======================================================
метод останавливает таймер
======================================================*/
void Timer::stop()
{
    pause = true;
    date = QDateTime::currentDateTime();
    std::lock_guard<std::mutex> lock(mtxNextExec);
    nextExec = -1;
    killTimer(timerStart);
}

/*======================================================
метод обрабатывает события таймера
Аргументы метода:
    event - объект события таймера
======================================================*/
void Timer::timerEvent(QTimerEvent *event)
{
    if(pause != true)
    {
        date = QDateTime::currentDateTime();
    }
    if(event->timerId() != timerStart)
        return;
    std::lock_guard<std::mutex> lock(mtxNextExec);
    killTimer(timerStart);
    timerStart = 0;
    if(nextExec > date.toTime_t())
    {
        timerStart = startTimer(calcDiffTime());
        assert(timerStart);
    }
    else
    {
        if((nextExec == date.toTime_t()) ||
           ((nextExec < date.toTime_t()) && (pause == true)))
        {
            emit timeout(timerIndex);
            if(!singShot)
            {
                nextExec = parser.getDateTime(cronJob, date).toTime_t();
                if(calcDiffTime() == 0)parser.setCall(true);
                timerStart = startTimer(calcDiffTime());
            }
            else
            {
                emit done(timerIndex);
            }
            pause = false;
        }
    }
}

/*======================================================
метод вычисляет время ожидания до следующего вызова
функции
======================================================*/
time_t Timer::calcDiffTime()
{
    time_t t = date.msecsTo(QDateTime::fromTime_t(0).addSecs(nextExec));
    if(t < 0)
    {
        return 0;
    }
    if(t > INT_MAX)
    {
        return INT_MAX;
    }
    return t;
}

/*======================================================
метод перегружает операцию присваивания
Аргументыметода:
    obj - объект, данные которого присваиваются
          данным текущего объекта
======================================================*/
Timer & Timer::operator = ( Timer const & obj )
{
    parser = obj.parser;
    cronJob = obj.cronJob;
    timerIndex = obj.timerIndex;
    timerStart = obj.timerStart;
    singShot = obj.singShot;
    nextExec = obj.nextExec;
    pause = obj.pause;
    url = obj.url;
    scheduleUid = obj.scheduleUid;
    return *this;
}

QString Timer::getCronjob()
{
    return cronJob;
}

QString Timer::getUrl()
{
    return url;
}

/*======================================================
деструктор класса
======================================================*/
Timer::~Timer()
{

}

