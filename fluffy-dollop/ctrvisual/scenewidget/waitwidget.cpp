#include "waitwidget.h"
#include <QString>

WaitWidget::WaitWidget(QWidget *parent) :
    QWidget(parent)
{

    QVBoxLayout * lt = new QVBoxLayout(this);
    m_pInfoLabel = new QLabel(this);
    m_pInfoLabel->show();
    m_pPushButton = new QPushButton(tr("OK"),this);
    connect(m_pPushButton, SIGNAL(clicked()), this, SIGNAL(signalOkClicked()));
    m_pPushButton->setMaximumWidth(50);

    m_pBusyIndicator = new BusyIndicator(this);
    lt->setMargin(0);
    lt->setSpacing(10);

    m_pBusyIndicator->setColor(Qt::green);
    m_pBusyIndicator->setIndicatorStyle(BusyIndicator::StyleArc);
    m_pBusyIndicator->setText("0%");
    m_pBusyIndicator->setMinimumHeight(50);

    lt->addWidget(m_pBusyIndicator,10);
    lt->addWidget(m_pInfoLabel,0,Qt::AlignCenter);
    lt->addWidget(m_pPushButton,0, Qt::AlignCenter);

    m_pPushButton->hide();
    // setWindowFlags(Qt::FramelessWindowHint);

}


void WaitWidget::setProgress(QString progressString)
{
    m_pBusyIndicator->setText(progressString);
}

void WaitWidget::setInfoText(QString str, bool isCloseButtonVisisble)
{
    if(!str.isEmpty())
    {
        m_pInfoLabel->setText(str);
        m_pInfoLabel->show();
    }
    else
    {
        hide();
    }

    if(isCloseButtonVisisble)
    {
        m_pPushButton->show();
        m_pBusyIndicator->hide();
    }
    else
    {
        m_pBusyIndicator->show();
        m_pPushButton->hide();
    }

}
