#include "anglegraphicsitem.h"
#include <QPainter>
#include <QPen>
#include <QLineF>
#include <QRegion>
#include <QPointF>
#include <QWidget>
#include <QClipboard>
#include <QApplication>
#include <QDebug>

AngleGraphicsItem::AngleGraphicsItem(QGraphicsItem *gParent, QObject *parent)
    : QObject(parent)
    , QGraphicsItem(gParent)
    , m_angle(0)
{
    setFlag(ItemIsMovable , false);
    setFlag(ItemIsSelectable, false);
    setFlag(ItemIsFocusable, false);
    setZValue(100);
    m_angle = 0;

    QColor colPen;
    colPen.setRgba(qRgba(0x09,0x61,0xD3,150));
    m_pen.setColor(colPen);
    m_pen.setWidth(2);
    m_pen.setCosmetic(true);
    m_pen.setJoinStyle(Qt::RoundJoin);

    QColor colBr;
    colBr.setRgba(qRgba(0xF3,0xF0,0x0F,150));
    m_brush.setColor(colBr);
    m_brush.setStyle(Qt::SolidPattern);

    m_TextItem = new MeasureTextItem(this, QBrush(QColor(0xF3,0xF0,0x0F)), QPen(QColor(0x09,0x61,0xD3)));
    m_TextItem->setFlags(ItemIgnoresTransformations);
    m_TextItem->setPen(QPen(QColor(0x09,0x61,0xD3)));
    m_TextItem->setZValue(101);
}

void AngleGraphicsItem::setPolygon(const QPolygonF &polygon)
{
    m_poly.clear();
    for(int i =0; i<polygon.count(); i++)
    {
        m_poly << polygon[i] - polygon[0];
    }

    m_poly = polygon;
    if(2 == m_poly.count() )
    {
        m_firstLine = QLineF(m_poly.at(0), m_poly.at(1));
        m_boundRect = m_poly.boundingRect();
    }

    if(2 < m_poly.count())
    {
        m_firstLine = QLineF(m_poly.at(1), m_poly.at(0));
        m_secondLine = QLineF(m_poly.at(1), m_poly.at(2));
        QRectF r1(m_firstLine.p1(), m_firstLine.p2()), r2(m_secondLine.p1(), m_secondLine.p2());
        r1 = r1.normalized();
        r2 = r2.normalized();

        qreal moreLen1, moreLen2;
        moreLen1 = qMin(r1.width(), r2.width());
        moreLen2 = qMin(r1.height(), r2.height());

        qreal moreLenRes = qMax(m_firstLine.length(), m_secondLine.length());
        m_boundRect = QRectF(m_poly.at(1).x() - moreLenRes, m_poly.at(1).y() - moreLenRes,
                             moreLenRes*2.0, moreLenRes*2.0);
        qreal lessLenRes = (qMin(m_firstLine.length(), m_secondLine.length()))/2.0;
        m_ellipseRect = QRectF(m_poly.at(1).x() - lessLenRes , m_poly.at(1).y() - lessLenRes ,
                               lessLenRes*2.0, lessLenRes*2.0);

        qreal diff = (m_firstLine.angle() - m_secondLine.angle()) ;
        //qDebug() << diff;
        qreal tmpAngle = (diff/360.0 - qRound((diff/360.0))) * 360.0;

        if(tmpAngle < 0)
            tmpAngle +=360;

        m_angle = tmpAngle;

        QLineF middleLine = m_firstLine;
        middleLine.setAngle(m_firstLine.angle() -m_angle/2.0 );
        middleLine.setLength(qAbs(m_ellipseRect.width()/2));

        qreal angleRes = 0;
        emit signalCalcRealAngle(m_firstLine,  m_secondLine, &angleRes);
        m_TextItem->setText(QString(" ") + QString::number( (int)angleRes) + QString(QChar(176)) + QString(" "));
        m_TextItem->setPos(middleLine.p2().x(), middleLine.p2().y());
        m_TextItem->translate( -(m_TextItem->getTextSize().width() / 2.), 2 );
        m_TextItem->show();

        if(polygon.size() == 4)
        {
            QClipboard* clipboard = QApplication::clipboard();
            clipboard->setText(QString::number(angleRes, 'f', 0));
        }
    }
    else
    {
        m_TextItem->hide();

        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText(QString());
    }

    prepareGeometryChange();
    update();
}


QRectF AngleGraphicsItem::boundingRect() const
{
    if(m_poly.count() > 2)
    {
        return QRectF(m_boundRect);
    }
    else
    {
        return QRectF(m_boundRect);
    }
}


QPainterPath AngleGraphicsItem::shape()
{
    QPainterPath sh;
    if(m_poly.count() > 2)
    {
        sh.addRect(m_boundRect);
        return sh;
    }
    else
    {
        sh.addRect(m_boundRect);
        return sh;

        /*return QGraphicsPolygonItem::shape();*/
    }

}


void AngleGraphicsItem::setSceneScale(qreal sceneScale)
{
    m_scaleK = sceneScale;
    update();
}

void AngleGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(m_pen);
    painter->setBrush(m_brush);

    if(m_poly.count() == 2)
    {
        QLineF ln = QLineF(mapFromScene(m_firstLine.p1()), mapFromScene(m_firstLine.p2()));

        if(ln.length() > 0)
            painter->drawLine(ln);
    }

    if(m_poly.count() > 2)
    {
        if(m_firstLine.length() > 0)
            painter->drawLine(m_firstLine);

        if(m_secondLine.length() > 0)
            painter->drawLine(m_secondLine);


        painter->save();
        painter->setBrush(m_brush);
        painter->drawArc(QRectF(m_ellipseRect),
                         m_firstLine.angle()*16, -m_angle*16);
        painter->setPen(Qt::NoPen);
        painter->drawPie(QRectF(m_ellipseRect),
                         m_firstLine.angle()*16, -m_angle*16);

        painter->restore();
    }
}
