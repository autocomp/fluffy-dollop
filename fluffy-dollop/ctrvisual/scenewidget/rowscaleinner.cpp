#include "rowscaleinner.h"
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QHBoxLayout>

RowScaleInner::RowScaleInner(QWidget *parent) : QWidget(parent)
{
    m_upScalePixelLen = 0;
    m_downScalePixelLen = 0;
}


void RowScaleInner::paintEvent(QPaintEvent * e)
{
    QWidget::paintEvent(e);

    drawScale();
}


void RowScaleInner::setScaleLen(qreal upScalePixelLen, qreal downScalePixelLen)
{
    m_upScalePixelLen = upScalePixelLen;
    m_downScalePixelLen = downScalePixelLen;

}

void RowScaleInner::drawScale()
{

    QPainter pr(this);
    pr.setPen(QColor(Qt::black));

    QString text;
    double scaleLenght = 0;

    int textBottomIndent = 7;
    int borderHeight = 7;
    int littleLineStep = 10;

    /*фон*/
    pr.setRenderHint(QPainter::Antialiasing);

    QFont f = pr.font();
    f.setPointSize(9);
    f.setStyleHint(QFont::Courier);
    pr.setFont(f);
    QFontMetrics fm(f);


    /*в режиме рисования дистанций*/
//    if(true == m_showDistValue)
//    {
////        pr.drawText(rect(),Qt::AlignCenter,m_distStr);
//        return;
//    }


    /*в режиме рисования линейки*/
    QPen pn;
    pn.setColor(Qt::white /*QColor(0xEA, 0xC0 ,0x03)*/ );
    pn.setWidth(2);
    pn.setJoinStyle(Qt::BevelJoin);
    pr.setPen(pn);
   // pr.setOpacity(0.8);


    QPoint center = rect().center();
    QPoint downScaleP1 = QPoint(center.x() - m_downScalePixelLen/2,center.y() );
    QPoint downScaleP2 = QPoint(center.x() + m_downScalePixelLen/2, center.y() );
    pr.drawLine(downScaleP1, downScaleP2);
    pr.drawLine(downScaleP1, downScaleP1 + QPoint(0,borderHeight));
    pr.drawLine(downScaleP2, downScaleP2 + QPoint(0,borderHeight));


    /*верхняя часть линейки*/
    QPoint upScaleP1 = QPoint(center.x() - m_upScalePixelLen/2,center.y() );
    QPoint upScaleP2 = QPoint(center.x() + m_upScalePixelLen/2, center.y() );

    if(upScaleP1.x() > 0 && upScaleP2.x() < width())
    {
        pr.drawLine(upScaleP1, upScaleP2);
        pr.drawLine(upScaleP1, upScaleP1 - QPoint(0,borderHeight));
        pr.drawLine(upScaleP2, upScaleP2 - QPoint(0,borderHeight));
    }
}


void RowScaleInner::switchToMeasureDistance()
{
    m_showDistValue = true;
    update();
}

void RowScaleInner::switchToScale()
{
    m_showDistValue = false;
    update();
}
