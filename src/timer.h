#ifndef TIMER_H
#define TIMER_H

#include <QTimer>
#include <QObject>
#include <QPair>
#include <QString>
#include <QObject>
#include <QDateTime>
#include <QMap>
#include <mutex>
#include "cronparser.h"

/*=============================================================
 Класс Timer выполняет следующие функции:
 -обрабатывает cron-выражение задачи посредством объекта
  класса CronParser и получает время ближайшего выполнения
  задачи;
 -отслеживает время начала выполнения задачи

 Атрибуты класса:
 parser - обработчик cron-выражений;
 cronJob - cron-выражение;
 taskIndex - индекс задачи;
 timerStart - время срабатывания таймера;
 singShot - флаг синглшота задачи;
 nextExec - время следующего срабатывания;
 mtxNextExec - мьютекс для корректного срабатывания таймера;
 date - дата начала отсчета времени.

 Методы класса:
 timerEvent - метод по обработке событий таймера;
 calcDiffTime - метод осуществляет подсчет разницы времени;
 Сигналы класса:
 timeout - сигнал о том, что время таймера истекло;
 done - сигнал о том, что задача выполнена

 Слоты класса:
 start - метод осуществляет запуск таймера;
 stop - метод осуществляет остановку таймера;
 setSingleShot - метод устанавливает переменную singShot в
                 нужное состояние.
 ============================================================*/
class Timer : public QObject
{
    Q_OBJECT

public:
    explicit    Timer               (QString cron, QString urlString, QString uid, int index, QObject *parent = 0);
                Timer               (Timer const & obj, QObject *parent = 0);
                ~Timer              ();

    Timer &     operator =          (Timer const & obj);
    QString     getCronjob          ();
    QString     getUrl              ();

private:
    CronParser      parser;
    QString         cronJob;
    QString         url;
    QString         scheduleUid;
    int             timerIndex;
    int             timerStart = 0;
    bool            singShot = false;
    time_t          nextExec = -1;
    std::mutex      mtxNextExec;
    bool            pause = false;
    QDateTime       date;

    void        timerEvent          (QTimerEvent *event) override;
    time_t      calcDiffTime        ();

signals:
    void        timeout             (int index);
    void        done                (int index);

public slots:
    void        start               ();
    void        stop                ();
    void        setSingleShot       (bool singleShot);
};

#endif // TIMER_H
