#include "datamanager.h"
#include <QJsonObject>
#include <QDebug>
#include <QDateTime>

DataManager::DataManager(QObject *parent) : QObject(parent)
{

}

DataManager::~DataManager()
{
    clear();
}

void DataManager::setData(QJsonDocument data)
{
    QJsonArray array = data.array();
    parse(array);
}

void DataManager::parse(QJsonArray array)
{
    if(!array.isEmpty())
    {
        for(int i = 0; i < array.size(); i++)
        {
            QJsonObject object = array[i].toObject();
            QString uid = object.value("cs_schedule_uid").toString();
            QString cron = object.value("cron").toString();
            QString url = object.value("url").toString();

            if(uid != "" && cron != "" && url != "")
            {
                QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                QString message = "Timer have cron: " + cron;
                qDebug() << tr(" [ ][DataManager::203][%1] %2").arg(date, message);

                MyTimer *t = new MyTimer();
                connect(t, SIGNAL(timeout()), this, SLOT(onTimeout()));
                t->start(cron, url);
                timer.append(t);
            }
            else
            {
                QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
                QString message = "JSon-string is incorrect!";
                qDebug() << tr(" [!][DataManager::201][%1] %2").arg(date, message);
            }
        }
    }
    else
    {
        QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        QString message = "Incoming array is empty!";
        qDebug() << tr(" [!][DataManager::201][%1] %2").arg(date, message);
    }
}

void DataManager::onTimeout()
{
    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    MyTimer *t = qobject_cast <MyTimer *> (QObject::sender());
    QString message = "Calling: " + t->getCron();
    qDebug() << tr(" [ ][DataManager::205][%1] %2").arg(date, message);
    emit sentUrl(t->getUrl());
}

void DataManager::clear()
{
    qDeleteAll(timer.begin(), timer.end());
    timer.clear();
    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString message = "Timer list deleted!";
    qDebug() << tr(" [ ][DataManager::202][%1] %2").arg(date, message);
}

