#include "cronparser.h"
#include <iostream>
#include <QStringList>
#include <QRegExp>
#include <QDebug>

/*======================================================
деструктор класса
======================================================*/
CronParser::CronParser(QObject *parent) : QObject(parent)
{
    cronJob = "";
    cronDate = QDateTime::currentDateTime();
    isCalled = false;
}

/*======================================================
деструктор копирования
======================================================*/
CronParser::CronParser(CronParser &obj, QObject *parent) : QObject(parent)
{
    cronJob = obj.cronJob;
    cronDate = obj.cronDate;
    minute = obj.minute;
    hour = obj.hour;
    dayOfMonth = obj.dayOfMonth;
    month = obj.month;
    year = obj.year;
    dayOfWeek = obj.dayOfWeek;
    isCalled = obj.isCalled;
    current = obj.current;
}

/*======================================================
метод перегружает операцию присваивания
======================================================*/
CronParser & CronParser::operator = ( CronParser const & obj )
{
    cronJob = obj.cronJob;
    cronDate = obj.cronDate;
    minute = obj.minute;
    hour = obj.hour;
    dayOfMonth = obj.dayOfMonth;
    month = obj.month;
    year = obj.year;
    dayOfWeek = obj.dayOfWeek;
    isCalled = obj.isCalled;
    current = obj.current;
    return *this;
}

/*======================================================
метод возвращает cron-выражение
======================================================*/
QString CronParser::getCronJob()
{
    return cronJob;
}

/*======================================================
метод возвращает ближайшую дату вызова функции.
Аргументы метода:
    cron - cron-выражение
    date - дата, от которой ноебходимо считать время
           ближайшего наступления события
======================================================*/
QDateTime CronParser::getDateTime(QString cron, QDateTime date)
{
    current = date;
    cronJob = cron;
    cronDate = calcTaskDate();
    return cronDate;
}

/*======================================================
метод обрабатывает cron-выражение и возвращает
ближайшую дату вызова функции
======================================================*/
QDateTime CronParser::calcTaskDate()
{
    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QDateTime optimalDate;
    QStringList crons = cronJob.split(" ");
    try
    {
        if(crons.size() != 6)
        {
            throw crons.size();
        }
        else
        {
            int value;
            value = current.time().minute();
            if(isCalled != true)
            {
                value += 1;
                setCall(false);
            }
            minute = parse(crons.at(0), value, 0, 59);

            value = current.time().hour();
            if(minute.second != false)
            {
                value +=1;
            }
            hour = parse(crons.at(1), value, 0, 23);

            value = current.date().day();
            if(hour.second != false)
            {
                value += 1;
            }
            dayOfMonth = parse(crons.at(2), value, 1, QDate::currentDate().daysInMonth());

            value = current.date().month();
            if(dayOfMonth.second != false)
            {
                value += 1;
            }
            month = parse(crons.at(3), value, 1, 12);

            value = current.date().year();
            if(month.second != false)
            {
                value += 1;
            }
            year = parse(crons.at(5), value, current.date().year() - 1, current.date().year() + 1);

            value = current.date().dayOfWeek();
            dayOfWeek = parse(crons.at(4), value, 1, 7);

            if(dayOfWeek.first < value)
            {
                dayOfWeek.first = value - dayOfWeek.first;
            }
            else
            {
                if(dayOfWeek.first == value)
                {
                    dayOfWeek.first = 0;
                }
                else
                {
                    dayOfWeek.first = 7 - dayOfWeek.first + value;
                }
            }

            value = current.date().day() + dayOfWeek.first;
            if(hour.second != false)
            {
                value ++;
            }
            try
            {
                if(crons.at(2) == "*")
                {
                    dayOfMonth.first = value;
                }
                else
                {
                    if(crons.at(4) == "*")
                    {
                        dayOfMonth.first = value;
                    }
                    else
                    {
                        if(dayOfMonth.first != value)
                        {
                            throw value;
                        }
                    }
                }
            }
            catch(int)
            {
                QString message = "Invalid cronjob! Day of week does not correspond with day of month! Invalid cronjob: ";
                qDebug() << tr(" [!][Scheduller::100][%1] %2 %3").arg(date, message, cronJob);
            }

            optimalDate.setDate(QDate(year.first, month.first, dayOfMonth.first));
            optimalDate.setTime(QTime(hour.first, minute.first, 0));
        }
    }
    catch(int)
    {
        QString message = "Invalid cronjob: ";
        qDebug() << tr(" [!][Scheduller::100][%1] %2 %3").arg(date, message, cronJob);
    }

    if(optimalDate <= QDateTime::currentDateTime())
    {
        emit isSingle(true);
    }

    return optimalDate;
}

/*======================================================
метод обрабатывает cron-выражение и возвращает пару
значений типа <int, bool>.
Первое значение пары - значение хронологического
                       параметра даты;
Второе значение пары - флаг переполнения разряда
                       хронологического параметра
Аргументы метода:
    cron - часть cron-выражения, характеризующая
           хронологический параметр даты;
    var - значение хронологического параметра даты;
    minLimit - минимально допустимое для данного
               хронологического параметра значение;
    maxLimit - максимально допустимое для данного
               хронологического параметра значение.
======================================================*/
QPair<int, bool> CronParser::parse(QString cron, int var, int minLimit, int maxLimit)
{
    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QPair <int, bool> res;
    res.first = var;
    res.second = false;

    int step = 1;
    int start = minLimit;
    int finish = maxLimit;

    QRegExp comma("((\\d+)(,))+(\\d+)");
    QRegExp value("(\\d+)");
    QRegExp star("\\*");
    QRegExp starStep("(\\*)(\\/)(\\d+)");
    QRegExp startStep("(\\d+)(\\/)(\\d+)");
    QRegExp startFinish("(\\d+)(\\-)(\\d+)");
    QRegExp startFinishStep("(\\d+)(\\-)(\\d+)(\\/)(\\d+)");

    if(comma.exactMatch(cron))
    {
        QStringList result;
        result = cron.split(",");
        int i=0;
        bool f = false;
        while(i < result.size() && result.at(i).toInt() < var)
        {
            i++;
        }
        if(i == result.size())
        {
            i = 0;
            f = true;
        }
        try
        {
            if(result.at(i).toInt() < minLimit && result.at(i).toInt() > maxLimit)
            {
                throw result.at(i);
            }
            else
            {
                res.first = result.at(i).toInt();
                res.second = f;
            }
        }
        catch(QString)
        {
            QString message = "Invalid value \"" + cron + "\" in a cronjob";
            qDebug() << tr(" [!][Scheduller::100][%1] %2 %3").arg(date, message, cronJob);
        }
    }
    else if(value.exactMatch(cron))
    {
        try
        {
            if(cron.toInt() < minLimit || cron.toInt() > maxLimit)
            {
                throw cron.toInt();
            }
            else
            {
                if(cron.toInt() < var)
                {
                    res.second = true;
                }
                res.first = cron.toInt();
            }
        }
        catch(int)
        {
            QString message = "Invalid value \"" + cron + "\" in a cronjob";
            qDebug() << tr(" [!][Scheduller::100][%1] %2 %3").arg(date, message, cronJob);
        }
    }
    else
    {
        if(star.exactMatch(cron))
        {
            step = 1;
        }
        else
        {
            if(starStep.exactMatch(cron))
            {
                step = cron.section("/", 1, 1).toInt();
            }
            else
            {
                if(startStep.exactMatch(cron))
                {
                    start = cron.section("/", 0, 0).toInt();
                    step = cron.section("/", 1, 1).toInt();
                }
                else
                {
                    if(startFinish.exactMatch(cron))
                    {
                        start = cron.section("-", 0, 0).toInt();
                        finish = cron.section("-", 1, 1).toInt();
                    }
                    else
                    {
                        if(startFinishStep.exactMatch(cron))
                        {
                            step = cron.section("/", 1, 1).toInt();
                            start = cron.section("-", 0, 0).toInt();
                            QString tmp = cron.section("-", 1, 1);
                            tmp = tmp.section("/", 0, 0);
                            finish = tmp.toInt();
                        }
                        else
                        {
                            start = -1;
                        }
                    }
                }
            }
        }
        try
        {
            if(start < minLimit || start > maxLimit)
            {
                throw start;
            }
            else
            {
                if(finish < minLimit || finish > maxLimit)
                {
                    throw finish;
                }
                else
                {
                    if(step < minLimit || step > maxLimit)
                    {                 
                        if((var != QDate::currentDate().year())&&(var != minLimit)&&(var!=maxLimit))
                        {
                            throw step;
                        }
                    }
                    else
                    {
                        int i = start;
                        while(i < var && i <= finish)
                        {
                            i += step;
                        }
                        if(i > finish)
                        {
                            if(i == (finish + 1))
                            {
                                res.first = start;
                            }
                            else
                            {
                                res.first = i%finish;
                            }
                            res.second = true;
                        }
                        else
                        {
                            res.first = i;
                        }
                    }
                }
            }
        }
        catch(int)
        {
            QString message = "Invalid value \"" + cron + "\" in a cronjob";
            qDebug() << tr(" [!][Scheduller::100][%1] %2 %3").arg(date, message, cronJob);
        }
    }
    return res;
}

/*======================================================
метод устианавливает флаг isCalled
Аргументы метода:
    var - новое значение флага isCalled
======================================================*/
void CronParser::setCall(bool var)
{
    isCalled = var;
}

/*======================================================
деструктор класса
======================================================*/
CronParser::~CronParser()
{

}

