#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QList>
#include <QJsonDocument>
#include <QJsonArray>
#include "mytimer.h"

class DataManager : public QObject
{
    Q_OBJECT
public:
    explicit        DataManager (QObject *parent = 0);
                    ~DataManager();
    void            setData     (QJsonDocument data);

private:
    QList
    <MyTimer *>     timer;

    void            parse       (QJsonArray array);
signals:
    void            timeout     ();
    void            sentUrl     (QString);

public slots:
    void            onTimeout   ();
    void            clear       ();

};

#endif // DATAMANAGER_H
