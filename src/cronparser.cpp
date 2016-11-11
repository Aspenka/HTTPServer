#include "CronParser.h"
#include <iostream>
#include <QStringList>
#include <QRegExp>
#include <QDebug>

/*======================================================
деструктор класса
======================================================*/
CronParser::CronParser(QObject *parent) : QObject(parent)
{
}

/*======================================================
метод обрабатывает cron-выражение и возвращает
ближайшую дату вызова функции
======================================================*/
QDateTime CronParser::calcTaskDate(const QString& cronJob)
{
    QDateTime optimalDate;
    QDateTime current = QDateTime::currentDateTime();
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
            value = current.time().minute() + 1;
            QPair<int, bool> minute = parse(crons.at(0), value, 0, 59);

            value = current.time().hour();
            if(minute.second != false)
            {
                value +=1;
            }
            QPair<int, bool> hour = parse(crons.at(1), value, 0, 23);

            value = current.date().day();
            if(hour.second != false)
            {
                value += 1;
            }
            QPair<int, bool> dayOfMonth = parse(crons.at(2), value, 1, QDate::currentDate().daysInMonth());

            value = current.date().month();
            if(dayOfMonth.second != false)
            {
                value += 1;
            }
            QPair<int, bool> month = parse(crons.at(3), value, 1, 12);

            value = current.date().year();
            if(month.second != false)
            {
                value += 1;
            }
            QPair<int, bool> year = parse(crons.at(5), value, current.date().year() - 1, current.date().year() + 5);

            value = current.date().dayOfWeek();
            QPair<int, bool> dayOfWeek = parse(crons.at(4), value, 1, 7);

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
                if(value > QDate::currentDate().daysInMonth())
                {
                    value = dayOfMonth.first;
                }
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
                qDebug() << tr(" [!][CronParser][100][%1] Invalid cronjob: %2").
                arg(getCurrentDate(), cronJob);
            }

            optimalDate.setDate(QDate(year.first, month.first, dayOfMonth.first));
            optimalDate.setTime(QTime(hour.first, minute.first, 0));
        }
    }
    catch(int)
    {
        qDebug() << tr(" [!][CronParser][100][%1] Invalid cronjob: %2").
        arg(getCurrentDate(), cronJob);
    }

    return optimalDate;
}

QString CronParser::getCurrentDate()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
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
            qDebug() << tr(" [!][CronParser][100][%1] Invalid cronjob: %2").
            arg(getCurrentDate(), "may be later...");
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
            qDebug() << tr(" [!][CronParser][100][%1] Invalid cronjob: %2").
            arg(getCurrentDate(), "may be later...");
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
            qDebug() << tr(" [!][CronParser][100][%1] Invalid cronjob: %2").
            arg(getCurrentDate(), "may be later...");
        }
    }
    return res;
}

time_t CronParser::calcDiffTime(const QString& cron)
{
    QDateTime cronDate = calcTaskDate(cron);
    time_t t = QDateTime::currentDateTime().msecsTo(QDateTime::fromTime_t(0).addSecs(cronDate.toTime_t()));
    if(t < 0)
    {
        return 0;
    }
    if(t > INT_MAX)
    {
        return INT_MAX;
    }
    return t + 1000;
}
