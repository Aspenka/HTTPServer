#include "datamanager.h"
#include <QJsonObject>
#include <QDebug>
#include <QDateTime>

DataManager::DataManager(QObject *parent) : QObject(parent)
{

}

DataManager::~DataManager()
{
    qDeleteAll(timer.begin(), timer.end());
    timer.clear();
    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString message = "Timer list deleted!";
    qDebug() << tr(" [ ][DataManager::202][%1] %2").arg(date, message);
}

void DataManager::setData(QJsonDocument data)
{
    qDebug() << "Received data: " << data.object();
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

                Timer *t = new Timer(cron, url, uid, timer.size());
                timer.append(t);
                start(timer.size() - 1);
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

void DataManager::start(int index)
{  
    connect (timer[index], SIGNAL(timeout(int)), this, SLOT(onTimeout(int)));
    timer[index]->start();
}

void DataManager::onTimeout(int index)
{
    QString date = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString message = "Calling: " + timer[index]->getCronjob();
    qDebug() << tr(" [ ][DataManager::205][%1] %2").arg(date, message);
    emit sentUrl(timer[index]->getUrl());
}

