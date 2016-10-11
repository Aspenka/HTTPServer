#ifndef CRONPARSER_H
#define CRONPARSER_H

#include <QObject>
#include <QString>
#include <QPair>
#include <QVector>
#include <QDateTime>

 /*==========================================================================================
  Класс CronParser выполняет следующие задачи:
    - обработка строки, содержащей cron-выражение задачи;
    - вычисление ближайшей даты вызова текущей задачи.

  Атрибуты класса:
  cronJob - строка, содержащая cron-выражение;
  cronDate - дата вызова функции;
  minute, hour, dayOfMonth, month, year, dayOfWeek - пары значений, содержащие информацию об
                                                     отдельном хронологическом параметре.
                                                     Первый элемент пары - значение части даты
                                                     и времени, второе значение - флаг
                                                     переполнения разряда текущей части даты
                                                     и времени;
  isCalled - флаг, показывающий, что объект данного класса уже был вызван;
  current - значение даты и времени, от которой будет вестись расчет ближайшего
            времени вызова задачи.

  Методы класса:
  setCall - метод устанавливает флаг isCalled$
  getCronJob - метод возвращает текущее cron-выражение для задачи
  getDateTime - метод возвращает время ближайшего вызова функции;
  parse - метод обрабатывает cron-выражение и возвращает значение срабатывания для каждого
          хронологического параметра;
  calcTaskDate - метод обрабатывает cron-выражение и рассчитывает ближайшую дату
                 вызова функции;
  isSingle - сигнал, оповещающий о том, что данная задача является синглшотом
             (т.е. задача выполнится ровно один раз)
  ========================================================================================== */
class CronParser : public QObject
{
    Q_OBJECT

public:
    explicit            CronParser  (QObject *parent = 0);
                        CronParser  (CronParser & obj, QObject * parent = 0);
                        ~CronParser ();

    QString             getCronJob  ();
    QDateTime           getDateTime (QString cron, QDateTime date);
    void                setCall     (bool var);
    CronParser &        operator =  (CronParser const & obj);

private:
    QString             cronJob;
    QDateTime           cronDate;
    QPair<int, bool>    minute,
                        hour,
                        dayOfMonth,
                        month,
                        year,
                        dayOfWeek;
    bool                isCalled;
    QDateTime           current;

    QPair<int, bool>    parse       (QString cron, int var, int minLimit, int maxLimit);
    QDateTime           calcTaskDate();
signals:
    void                isSingle    (bool singleShot);

public slots:
};

#endif // CRONPARSER_H
