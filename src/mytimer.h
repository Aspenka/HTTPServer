#ifndef MYTIMER_H
#define MYTIMER_H

#include <QTimer>
#include "cronparser.h"
#include <QStringList>

class MyTimer : public QObject
{
    Q_OBJECT
public:
    explicit MyTimer(QObject *parent = 0);
    void    setId(const QString& id);
    const QString& getId();
    const QString& getCron();
    const QStringList& getTasks();
    const QString &getUrl();
    void  removeTask(const QString & taskUid);

signals:
    void timeout();

public slots:
    void start(const QString& cron, const QString& url);
    void start();
    void addTask(const QString& task_uid);

private:
    static quint64 current_id;
    QString m_sId;
    QString m_sCron;
    QString m_url;
    QTimer timer;
    QStringList tasks;
};

#endif // MYTIMER_H
