#include "minimapcontainerwidget.h"

#include <QDebug>
#include <QPainter>

MiniMapContainerWidget::MiniMapContainerWidget(QWidget *parent) :
    QWidget(parent),
    m_boundWidth(2)
{
    this->setMinimumSize(150,150);
    this->setFixedSize(300,150);
    m_minimapWidget = 0;
    m_pMainLayout = new QVBoxLayout(this);
    m_pInnerLayout = new QVBoxLayout();
    m_pMainLayout->setMargin(m_boundWidth);
    m_pMainLayout->setSpacing(m_boundWidth);
    m_pMainLayout->addLayout(m_pInnerLayout,1);
}


void MiniMapContainerWidget::resizeEvent(QResizeEvent *)
{
    /*if(0 != m_minimapWidget)
    {
        m_minimapWidget->resize(width()+2, height()+2);

    }*/
}


QWidget *MiniMapContainerWidget::setMiniMap(QWidget * minimapWidget)
{

    if(0 == minimapWidget )
        return 0;

//    minimapWidget->setStyleSheet("QWidget { border: 1px; border-radius: 0px; }");

    QWidget * returnedWidget = m_minimapWidget;
    if(!m_pInnerLayout->isEmpty())
    {
        m_pInnerLayout->removeWidget(m_minimapWidget);
    }

    m_minimapWidget = minimapWidget;
    m_pInnerLayout->setMargin(0);
    m_pInnerLayout->setSpacing(0);

    if(0 == m_minimapWidget)
    {
        return 0;
    }

    m_minimapWidget->setFixedSize(size().width() - m_pMainLayout->margin()*2 , size().height()-m_pMainLayout->margin()*2);

    if(m_pInnerLayout->isEmpty())
    {
        m_pInnerLayout->addWidget(minimapWidget);
    }
    else
    {
        qDebug() <<" MiniMapContainerWidget::setMiniMap(): ERROR! Container not empty!!";
    }

    return returnedWidget;
}


void MiniMapContainerWidget::paintEvent(QPaintEvent *e)
{
    QWidget::paintEvent(e);
    QPainter pr(this);
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(m_boundWidth);

    pr.setPen(pen);
    pr.drawRect(m_boundWidth/2,m_boundWidth/2,width() -m_boundWidth , height() -m_boundWidth );
}
