#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QList>
#include <QJsonDocument>
#include <QJsonArray>
#include "timer.h"

class DataManager : public QObject
{
    Q_OBJECT
public:
    explicit        DataManager (QObject *parent = 0);
                    ~DataManager();
    void            setData     (QJsonDocument data);

private:
    QList <Timer *> timer;

    void            parse       (QJsonArray array);
signals:
    void            timeout     ();
    void            sentUrl     (QString);

public slots:
    void            start       (int index);
    void            onTimeout   (int index);
    void            clear       ();

};

#endif // DATAMANAGER_H
