#include "datetimewidget.h"

#include <QHBoxLayout>


DateTimeWidget::DateTimeWidget(QWidget *parent) :
    QWidget(parent)
{

    QWidget * wdg = new QWidget(this);
    wdg->setObjectName("DateTimeWidget_Background");




    m_pDateLabel     = new QLabel(this);
    m_pTimeLabel     = new QLabel(this);
    m_pTimeZoneLabel = new QLabel(this);

    m_pDateLabel->setObjectName("DateTimeWidget_DateLabel");
    m_pTimeLabel->setObjectName("DateTimeWidget_TimeLabel");
    m_pTimeZoneLabel->setObjectName("DateTimeWidget_TimeZoneLabel");

    QHBoxLayout * lt = new QHBoxLayout(this);
    lt->setMargin(0);
    lt->setSpacing(0);
    lt->addWidget(wdg);

    QHBoxLayout * mainLayout = new QHBoxLayout(wdg);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    mainLayout->addWidget(m_pTimeZoneLabel);
    mainLayout->addWidget(m_pTimeLabel);
    mainLayout->addWidget(m_pDateLabel);
}


void DateTimeWidget::setTime(int timeZoneGmt, int hh, int min, int dd, int mm, int yy)
{

    QString timezoneStr = QString("(") + QString("GMT") ;
    if (timeZoneGmt > 0)
    {
        timezoneStr += QString("+") + QString::number(timeZoneGmt) + QString(")");
    }

    QString timeStr;
    if (hh < 10)
    {
        timeStr += "0";
    }

    timeStr += QString::number(hh) + QString(".") ;

    if (min < 10)
    {
        timeStr += "0";
    }
    timeStr += QString::number(min);


    QString dateStr;

    if (dd < 10)
    {
        dateStr += "0";
    }
    dateStr += QString::number(dd) + QString(".");

    if (mm < 10)
    {
        dateStr += "0";
    }
    dateStr += QString::number(mm) + QString(".");

    dateStr += QString::number(yy) ;

    m_pDateLabel->setText(dateStr);
    m_pTimeLabel->setText(timeStr);
    m_pTimeZoneLabel->setText(timezoneStr);
}
