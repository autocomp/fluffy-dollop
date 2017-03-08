#include "areagraphicsitem.h"
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QDebug>
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
//    if(_areaInitData.sendDoubleClick)
//    {
//        if(polygon().containsPoint(event->pos(), Qt::OddEvenFill))
//        {
//            event->accept();
//            emit dubleClickOnItem(_areaInitData.id);
//            qDebug() << "mouseDoubleClickEvent:" << toolTip();
//        }
//        else
//        {
//            event->ignore();
//            QGraphicsPolygonItem::mouseDoubleClickEvent(event);
//        }
//    }
//    else
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
        painter->drawPolygon(polygon()); // drawConvexPolygon
    }
    else
        QGraphicsPolygonItem::paint(painter, option, widget);
}

bool AreaGraphicsItem::itemIsSelected()
{
    return _isSelected;
}

void AreaGraphicsItem::setItemselected(bool on_off)
{
    _isSelected = on_off;

    QPen pen(Qt::gray, 2, Qt::DotLine);
    pen.setCosmetic(true);
    setPen(_isSelected ? pen : _areaInitData.penNormal);
    setBrush(_areaInitData.brushNormal);
}

void AreaGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(_areaInitData.isSelectableFromMap)
    {
        if(polygon().containsPoint(event->pos(), Qt::OddEvenFill))
        {
            if( ! _isSelected)
            {
                regionbiz::RegionBizManager::instance()->selectArea(_areaInitData.id);
                qDebug() << "slotItemSelectionChanged, ID :" << _areaInitData.id;
            }
        }
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









