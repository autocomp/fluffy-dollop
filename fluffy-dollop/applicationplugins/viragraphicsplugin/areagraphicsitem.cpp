#include "areagraphicsitem.h"
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <regionbiz/rb_manager.h>

AreaGraphicsItem::AreaGraphicsItem(const QPolygonF &polygon)
    : QGraphicsPolygonItem(polygon)
{
    setZValue(100);
}

void AreaGraphicsItem::init(const AreaInitData& areaInitData)
{
    _areaInitData = areaInitData;
    setPen(_areaInitData.penNormal);
    setBrush(_areaInitData.brushNormal);
    setZValue(_areaInitData.zValue);
//    setToolTip(_areaInitData.tooltip);
//    setAcceptHoverEvents(true);
}

void AreaGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(_areaInitData.sendDoubleClick)
    {
        if(polygon().containsPoint(event->pos(), Qt::OddEvenFill))
        {
            event->accept();
            emit signalSelectItem(_areaInitData.id, true); // regionbiz::RegionBizManager::instance()->centerOnArea(_areaInitData.id);
        }
        else
        {
            event->ignore();
            QGraphicsPolygonItem::mouseDoubleClickEvent(event);
        }
    }
    else
        QGraphicsPolygonItem::mouseDoubleClickEvent(event);
}

void AreaGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    if(_isSelected)
    {
        QPen pen1(Qt::black, 2, Qt::SolidLine);
        pen1.setCosmetic(true);
        painter->setPen(pen1);
        painter->drawPolygon(polygon());

        QColor col(Qt::yellow);
        col.setAlpha(70);
        painter->setBrush(QBrush(col, Qt::SolidPattern));

        QPen pen2(Qt::yellow, 2, Qt::DotLine);
        pen2.setCosmetic(true);
        painter->setPen(pen2);
        painter->drawPolygon(polygon());
    }
    else
        QGraphicsPolygonItem::paint(painter, option, widget);
}

void AreaGraphicsItem::setItemselected(bool on_off)
{
    _isSelected = on_off;

    QPen pen(Qt::gray, 2, Qt::DotLine);
    pen.setCosmetic(true);
    setPen(_isSelected ? pen : _areaInitData.penNormal);
    setBrush(_areaInitData.brushNormal);

    if(on_off)
    {
        if(polygon().isEmpty() == false)
            foreach(QGraphicsView * view, scene()->views())
            {
                QRectF viewportSceneRect(view->mapToScene(view->contentsRect().topLeft()), view->mapToScene(view->contentsRect().bottomRight()));
                QPolygonF pol = ( polygon().intersected( QPolygonF(viewportSceneRect) ) );
                if(pol.isEmpty())
                    view->centerOn(this);
            }
    }
}

void AreaGraphicsItem::centerOnItem()
{
    if(polygon().isEmpty() == false)
        foreach(QGraphicsView * view, scene()->views())
        {
            QRectF viewportSceneRect(view->mapToScene(view->contentsRect().topLeft()), view->mapToScene(view->contentsRect().bottomRight()));
            if(viewportSceneRect.contains(scenePos()) == false)
                view->centerOn(this);
        }
}

quint64 AreaGraphicsItem::getId()
{
    return _areaInitData.id;
}

void AreaGraphicsItem::reinit()
{

}

void AreaGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(_areaInitData.isSelectableFromMap)
    {
        if(polygon().containsPoint(event->pos(), Qt::OddEvenFill))
            emit signalSelectItem(_areaInitData.id, false);
        else
            QGraphicsPolygonItem::mousePressEvent(event);
    }
    else
        QGraphicsPolygonItem::mousePressEvent(event);
}

void AreaGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if( ! _isSelected)
    {
        emit itemIsHover(_areaInitData.id, true);
        setPen(_areaInitData.penHoverl);
        setBrush(_areaInitData.brushHoverl);
    }
    QGraphicsPolygonItem::hoverEnterEvent(event);
}

void AreaGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if( ! _isSelected)
    {
        emit itemIsHover(_areaInitData.id, false);
        setPen(_areaInitData.penNormal);
        setBrush(_areaInitData.brushNormal);
    }
    QGraphicsPolygonItem::hoverLeaveEvent(event);
}









