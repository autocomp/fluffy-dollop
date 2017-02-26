#ifndef DATETIMEWIDGET_H
#define DATETIMEWIDGET_H

#include <QLabel>
#include <QWidget>


class DateTimeWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DateTimeWidget(QWidget *parent = 0);

    /**
     * @brief setTime Установка зоны, времени и даты
     * @param timeZoneGmt - зона
     * @param hh - часы 0..23
     * @param min - минуты 0..59
     * @param dd - день
     * @param mm - месяц
     * @param yy - год
     */
    void setTime(int timeZoneGmt, int hh, int min, int dd, int mm, int yy);

private:

    QLabel *m_pDateLabel;
    QLabel *m_pTimeLabel;
    QLabel *m_pTimeZoneLabel;

};

#endif // DATETIMEWIDGET_H
