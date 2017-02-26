#include "lineresolutionitem.h"
#include "viewercontroller.h"

#include <QPainter>
#include <QPen>
#include <QLineF>
#include <QRegion>
#include <QPointF>
#include <QWidget>
#include <QDebug>

LineResolutionItem::LineResolutionItem(QGraphicsPolygonItem * gParent, QObject *parent)
  : QGraphicsPolygonItem(gParent)
  , QObject(parent)
  , m_pointRadius(6.0)
{
    m_scaleK = 1;
    m_textMode = false;
    m_selectingFinished = false;

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

    m_TextItem = new MeasureTextItem(this, QBrush(QColor(0xF3,0xF0,0x0F)), QPen(QColor(0x09,0x61,0xD3)));
    m_TextItem->setFlags(ItemIgnoresTransformations);
    m_TextItem->setPen(QPen(QColor(0x09,0x61,0xD3)));
    m_TextItem->setZValue(101);
    m_TextItem->hide();
}

void LineResolutionItem::setPenBrush(QPen pen, QBrush brush)
{
    m_pen = pen;
    m_brush = brush;

}

QString LineResolutionItem::convertMetersSquareToNormString(qreal square)
{
    QString value;
    QString dimensions;
    QString fullStr;
    visualize_system::ViewerController::convertMetersSquareToNormString(square,value,dimensions);
    fullStr = value + " " + dimensions;

    return fullStr;
}

void LineResolutionItem::setPolygon(const QPolygonF &polygon)
{
    QPainterPath sh;
    m_poly = polygon;
    int pCount = m_poly.count();
    QPolygonF poly = m_poly;

    if(pCount < 1)
    {
        m_lines.clear();
        m_TextItem->setText("");
        m_TextItem->hide();
        update();
        return;
    }

    m_boundRect = m_poly.boundingRect();
    sh.addRect(m_boundRect);
    m_lines.clear();
    qreal square = 0;

    for(int i =1; i<pCount; i++)
    {
        QLineF line = QLineF(m_poly[i-1], m_poly[i]);
        m_lines.append(line);
    }

    if(!m_textMode)
    {
        QString squareText = convertMetersSquareToNormString(square);
        m_TextItem->setText(QString(" ") + squareText + QString(" "));
        m_TextItem->setPos(m_boundRect.center());
        m_TextItem->translate( -(m_TextItem->getTextSize().width() / 2.), -(m_TextItem->getTextSize().height() / 2.) );
        m_TextItem->show();
    }

    m_boundRect = sh.boundingRect();
    m_shape = sh;

    prepareGeometryChange();
    update();
}

void LineResolutionItem::setTextMode(bool isTextMode)
{
    m_textMode = isTextMode;
}

void LineResolutionItem::setPhysValue(QString str)
{
    QPainterPath sh;
    int pCount = m_poly.count();

    if(pCount < 1)
    {
        return;
    }

    m_boundRect = m_poly.boundingRect();

    m_TextItem->setText(QString(" ") + str + QString(" "));
    m_TextItem->setPos(m_boundRect.center());
    m_TextItem->translate( -(m_TextItem->getTextSize().width() / 2.), -(m_TextItem->getTextSize().height() / 2.) );
    m_TextItem->show();

    sh.addRect(m_boundRect);
    m_lines.clear();

    for(int i =1; i<pCount; i++)
    {
        QLineF line = QLineF(m_poly[i-1], m_poly[i]);
        m_lines.append(line);
    }

    m_boundRect = sh.boundingRect();
    m_shape = sh;

    prepareGeometryChange();
    update();
}

QRectF LineResolutionItem::boundingRect() const
{
    return m_boundRect;
}

QPainterPath LineResolutionItem::shape()
{
    return m_shape;
}

bool LineResolutionItem::isSelectingFinished()
{
    return m_selectingFinished;
}

void LineResolutionItem::finishSelecting(bool finish)
{
    m_selectingFinished = finish;
    qreal square = 0;
    if(m_selectingFinished)
        emit signalCalcWGS84Square(m_poly, &square);
    update();
}

void LineResolutionItem::setSceneScale(qreal sceneScale)
{
    m_scaleK = sceneScale;
    update();
}

void LineResolutionItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(m_pen);
    painter->setBrush(m_brush);

    for(int i =0; i<m_lines.count(); i++)
    {
        if(m_lines.count() == (i+1))
        {
            if(!m_selectingFinished)
            {
                if(m_poly.count() > 2)
                {
                    painter->save();
                    QPen pn = m_pen;
                    pn.setStyle(Qt::DashLine);
                    painter->setPen(pn);

                    QLineF ln = QLineF(m_lines[i].p1(), m_poly[0]);
                    if(ln.length())
                        painter->drawLine(ln);
                    QPolygonF poly = m_poly;

                    painter->setPen(Qt::NoPen);
                    painter->drawPolygon(poly,Qt::WindingFill);
                    painter->restore();
                }
            }
            else
            {
                painter->save();
                painter->setBrush(Qt::NoBrush);
                painter->drawPolygon(m_poly);
                painter->restore();
                painter->drawRect(m_poly.boundingRect());
            }
        }

        if(!m_selectingFinished)
            if(m_lines[i].length() > 0)
                painter->drawLine(m_lines[i]);

        painter->save();
        QRectF elRect= QRectF(m_lines[i].p1().x() - m_pointRadius/(m_scaleK*2.0)
                              , m_lines[i].p1().y() - m_pointRadius/(m_scaleK*2.0)
                              , m_pointRadius/(m_scaleK), m_pointRadius/(m_scaleK));


        painter->scale(1.0,1.0);
        painter->drawEllipse(elRect);
        painter->restore();

        if(i == m_lines.count()-1)
        {
            painter->save();

            QRectF elRect= QRectF(m_lines[i].p2().x() - m_pointRadius/(m_scaleK*2.0)
                                  , m_lines[i].p2().y() - m_pointRadius/(m_scaleK*2.0)
                                  , m_pointRadius/(m_scaleK), m_pointRadius/(m_scaleK));

            painter->drawEllipse(elRect);
            painter->restore();
        }
    }
}


