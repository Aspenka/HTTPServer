#include "mytimer.h"

quint64 MyTimer::current_id = 0;

MyTimer::MyTimer(QObject *parent) :
    QObject(parent), timer(parent), tasks()
{
    m_sId = QString::number(current_id++);
    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), this, SIGNAL(timeout()));
}

void MyTimer::setId(const QString &id)
{
    m_sId = id;
}

const QString &MyTimer::getId()
{
    return m_sId;
}

const QString &MyTimer::getCron()
{
    return m_sCron;
}

const QStringList &MyTimer::getTasks()
{
    return tasks;
}

const QString &MyTimer::getUrl()
{
    return m_url;
}

void MyTimer::removeTask(const QString &taskUid)
{
    tasks.removeOne(taskUid);
}

void MyTimer::start(const QString &cron, const QString &url)
{
    m_sCron = cron;
    m_url = url;
    timer.start(CronParser::calcDiffTime(m_sCron));
}

void MyTimer::start()
{
    timer.start(CronParser::calcDiffTime(m_sCron));
}

void MyTimer::addTask(const QString &task_uid)
{
    this->tasks.append(task_uid);
}
