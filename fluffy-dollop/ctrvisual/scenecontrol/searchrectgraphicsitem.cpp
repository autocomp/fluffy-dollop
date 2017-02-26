#include "searchrectgraphicsitem.h"


#include <QPainter>
#include <QPen>
#include <QLineF>
#include <QRegion>
#include <QPointF>
#include <QWidget>
#include <QDebug>


SearchRectGraphicsItem::SearchRectGraphicsItem(quint64 browserId, QGraphicsRectItem * gParent, QObject *parent) :
    QGraphicsRectItem(gParent)
  , QObject(parent)

{
    m_browserId = browserId;
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

}


void SearchRectGraphicsItem::setPoly(QPolygonF &rect)
{
    if(0 == rect.count())
    {
        m_rect = QRectF(QPointF(0,0), QPointF(0,0));
        QGraphicsRectItem::setRect(m_rect);
    }

    if(m_selectingFinished)
    {
        return;
    }

    if(rect.count() >= 2)
    {
        m_rect = QRectF(rect.at(0), rect.at(1));
        QGraphicsRectItem::setRect(m_rect);
    }

}



bool SearchRectGraphicsItem::isSelectingFinished()
{
    return m_selectingFinished;
}


void SearchRectGraphicsItem::finishSelecting(bool finish)
{
    m_selectingFinished = finish;
    if(m_selectingFinished)
    {
        m_pen.setStyle(Qt::SolidLine);
        //m_brush.setStyle(Qt::SolidPattern);
    }
    else
    {
        //m_brush.setStyle(Qt::CrossPattern);
        m_pen.setStyle(Qt::DashLine);
    }
    setPen(m_pen);
    //setBrush(m_brush);
    update();

    if(m_selectingFinished)
    {
        emit signalSelectingFinished(m_rect, m_browserId);
    }
}

