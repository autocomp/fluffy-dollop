#include "routescalegraphicsitem.h"

#include <QPainter>
#include <QPen>
#include <QLineF>
#include <QRegion>
#include <QPointF>
#include <QWidget>
#include <QDebug>


RouteScaleGraphicsItem::RouteScaleGraphicsItem(QGraphicsPolygonItem * gParent, QObject *parent) :
    QGraphicsPolygonItem(gParent)
  , QObject(parent)
  , m_pointRadius(6.0)
  , m_totalDist(0)
  , m_lenghtInMeters(0)
{
    setFlag(ItemIsMovable , false);
    setFlag(ItemIsSelectable, false);
    setFlag(ItemIsFocusable, false);
    setZValue(100);

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
}

RouteScaleGraphicsItem::~RouteScaleGraphicsItem()
{
    foreach(MeasureTextItem * item, m_TextItems)
        delete item;
    m_TextItems.clear();
}

void RouteScaleGraphicsItem::setSceneScale(qreal sceneScale)
{
    m_scaleK = sceneScale;
    update();
}

double RouteScaleGraphicsItem::getLengthInMeters()
{
    return m_lenghtInMeters;
}

void RouteScaleGraphicsItem::setPolygon(const QPolygonF &polygon)
{
    if(polygon.size() > 1)
    {
        QPointF p1 = polygon[polygon.size()-1];
        QPointF p2 = polygon[polygon.size()-2];
        qreal realDist = 100000;
        QLineF line = QLineF(p1, p2);
        emit signalCalcRealDistance(line, &realDist);

        if(m_TextItems.size() < polygon.size()-1)
        {
            if(m_TextItems.isEmpty() == false)
            {
                MeasureTextItem * textItem = m_TextItems.last();
                m_totalDist += textItem->getValue();
            }
            m_lenghtInMeters = m_totalDist;
            QString text( QString(" ") + convertMetersToNormString(m_lenghtInMeters) + QString(" ") );

            MeasureTextItem * textItem = new MeasureTextItem(this, QBrush(QColor(0xF3,0xF0,0x0F)), QPen(QColor(0x09,0x61,0xD3)));
            textItem->setFlags(ItemIgnoresTransformations);
            textItem->setPen(QPen(QColor(0x09,0x61,0xD3)));
            textItem->setText(text);
            textItem->translate( -(textItem->getTextSize().width() / 2.),  -(textItem->getTextSize().height() + 5) );
            textItem->setPos(p1);
            m_TextItems.append(textItem);
        }
        else
        {
            m_lenghtInMeters = m_totalDist + realDist;
            QString text( QString(" ") + convertMetersToNormString(m_lenghtInMeters) + QString(" ") );
            MeasureTextItem * textItem = m_TextItems.last();
            textItem->setText(text);
            textItem->translate( -(textItem->getTextSize().width() / 2.),  -(textItem->getTextSize().height() + 5) );
            textItem->setPos(p1);
            textItem->setValue(realDist);
        }
    }
    else
    {
        foreach(MeasureTextItem * item, m_TextItems)
            delete item;
        m_TextItems.clear();
        m_totalDist = 0;
        m_lenghtInMeters = 0;
    }


    m_poly = polygon;

    int pCount = m_poly.count();
    m_boundRect = m_poly.boundingRect();
    m_shape.addRect(m_boundRect);
    m_lines.clear();

    for(int i =1; i<pCount; i++)
    {
        QLineF line = QLineF(m_poly[i-1], m_poly[i]);
        m_lines.append(line);

        prepareGeometryChange();

        m_shape.addRect(line.p1().x() - m_pointRadius/m_scaleK, line.p1().y() - m_pointRadius/m_scaleK,
                2*m_pointRadius/m_scaleK, 2*m_pointRadius/m_scaleK);
        m_boundRect = m_shape.boundingRect();
    }

    prepareGeometryChange();
    update();
}


QRectF RouteScaleGraphicsItem::boundingRect() const
{
    return m_boundRect;
}

QPainterPath RouteScaleGraphicsItem::shape()
{
    return m_shape;
}

void RouteScaleGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBackgroundMode( Qt::OpaqueMode);

    painter->setPen(m_pen);

    painter->setBrush(m_brush);
    for(int i =0; i<m_lines.count(); i++)
    {
        if(m_lines[i].length() > 0)
            painter->drawLine(m_lines[i]);

        if(i == 0)
        {
            painter->save();
            QBrush br = m_brush;
            br.setColor(QColor(0x30, 0xB9, 0x07, 220));
            painter->setBrush(br);

            QRectF elRect= QRectF(m_lines[i].p1().x() - m_pointRadius/(m_scaleK*2.0)
                    , m_lines[i].p1().y() - m_pointRadius/(m_scaleK*2.0)
                    , m_pointRadius/(m_scaleK), m_pointRadius/(m_scaleK));


            painter->scale(1.0,1.0);
            painter->drawEllipse(elRect);
            painter->restore();

        }
        else
        {
            painter->save();
            QRectF elRect= QRectF(m_lines[i].p1().x() - m_pointRadius/(m_scaleK*2.0)
                    , m_lines[i].p1().y() - m_pointRadius/(m_scaleK*2.0)
                    , m_pointRadius/(m_scaleK), m_pointRadius/(m_scaleK));

            painter->scale(1.0,1.0);
            painter->drawEllipse(elRect);
            painter->restore();
        }

        if(i == m_lines.count()-1)
        {
            painter->save();
            QBrush br = m_brush;
            br.setColor(QColor(0xE6, 0x1A, 0x24, 220));
            painter->setBrush(br);
            QRectF elRect= QRectF(m_lines[i].p2().x() - m_pointRadius/(m_scaleK*2.0)
                    , m_lines[i].p2().y() - m_pointRadius/(m_scaleK*2.0)
                    , m_pointRadius/(m_scaleK), m_pointRadius/(m_scaleK));

            painter->scale(1.0,1.0);
            painter->drawEllipse(elRect);
            painter->restore();
        }
    }
}


QString RouteScaleGraphicsItem::convertMetersToNormString(qreal meters)
{
    QString normStr;
    /*переводим в адекватные единицы измерения*/
    if(meters < 0.1)
        normStr = normStr.setNum( (int)(meters*100),10 ) + QString(" ") + QString(tr("cm"));

    if(meters < 1)
        normStr = normStr.setNum( (int)(meters*10),10 ) + QString(" ")+ QString(tr("dm"));

    if(meters >= 1)
        normStr = QString::number(meters, 'f', 0) + QString(" ")+ QString(tr("m"));

    if(meters>=1000 )
        normStr = normStr.setNum( (meters/1000.0), 'f' ,1 ) + QString(" ")+ QString(tr("km"));

    return normStr;
}
