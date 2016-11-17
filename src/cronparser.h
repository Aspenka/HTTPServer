#ifndef CRONPARSER_H
#define CRONPARSER_H

#include <QObject>
#include <QString>
#include <QPair>
#include <QVector>
#include <QDateTime>

/*==========================================================================================

========================================================================================== */
class CronParser : public QObject
{
    Q_OBJECT

public:
    explicit                   CronParser      (QObject *parent = 0);
    static time_t              calcDiffTime    (const QString &cron);
    static QString             getCurrentDate  ();

private:
    static QPair<int, bool>    parse           (QString cron, int var, int minLimit, int maxLimit);
    static QDateTime           calcTaskDate    (const QString &cronJob);
};

#endif // CRONPARSER_H
