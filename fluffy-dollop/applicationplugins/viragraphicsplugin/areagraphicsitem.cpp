#include "areagraphicsitem.h"
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <regionbiz/rb_manager.h>
#include <ctrcore/ctrcore/ctrconfig.h>

AreaInitData::AreaInitData(bool selectableFromMap, double _zValue, QColor color)
    : isSelectableFromMap(selectableFromMap)
    , zValue(_zValue)
{
    setColor(color);
}

void AreaInitData::setColor(QColor color)
{
    uint area_normal_alpha = CtrConfig::getValueByName("vira_graphic_settings.area_normal_alpha", (quint64)110, true).toUInt();
    uint area_hover_alpha = CtrConfig::getValueByName("vira_graphic_settings.area_hover_alpha", (quint64)140, true).toUInt();
    uint area_select_alpha = CtrConfig::getValueByName("vira_graphic_settings.area_select_alpha", (quint64)180, true).toUInt();

    penNormal.setColor(color);
    penNormal.setCosmetic(true);
    penNormal.setWidth(2);

    penHoverl.setColor(color);
    penHoverl.setCosmetic(true);
    penHoverl.setWidth(3);

    color.setAlpha(area_normal_alpha);
    brushNormal.setColor(color);
    brushNormal.setStyle(Qt::SolidPattern);

    color.setAlpha(area_hover_alpha);
    brushHoverl.setColor(color);
    brushHoverl.setStyle(Qt::SolidPattern);

    color.setAlpha(area_select_alpha);
    brushSelect.setColor(color);
    brushSelect.setStyle(Qt::SolidPattern);
}

//-----------------------------

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

void AreaGraphicsItem::setColor(QColor color)
{
    _areaInitData.setColor(color);
    if(_isSelected)
    {
        QPen pen(_areaInitData.penNormal.color(), 4, Qt::DotLine);
        pen.setCosmetic(true);
        setPen(pen);
        setBrush(_areaInitData.brushSelect);
    }
    else
    {
        setPen(_areaInitData.penNormal);
        setBrush(_areaInitData.brushNormal);
        setZValue(_areaInitData.zValue);
    }

    update();
}

void AreaGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if(_areaInitData.sendDoubleClick)
    {
        if(polygon().containsPoint(event->pos(), Qt::OddEvenFill))
        {
            event->accept();
            emit signalSelectItem(_areaInitData.id, true);
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
        QPen pen1(Qt::black, 4, Qt::SolidLine);
        pen1.setCosmetic(true);
        painter->setPen(pen1);
        painter->setBrush(QBrush(Qt::NoBrush));
        painter->drawPolygon(polygon());

        QGraphicsPolygonItem::paint(painter, option, widget);
    }
    else
        QGraphicsPolygonItem::paint(painter, option, widget);
}

void AreaGraphicsItem::setItemselected(bool on_off)
{
    _isSelected = on_off;

    if(on_off)
    {
        QPen pen(_areaInitData.penNormal.color(), 4, Qt::DotLine);
        pen.setCosmetic(true);
        setPen(pen);
        setBrush(_areaInitData.brushSelect);
        setZValue(_areaInitData.zValue + 1);

        if(polygon().isEmpty() == false)
            foreach(QGraphicsView * view, scene()->views())
            {
                QRectF viewportSceneRect(view->mapToScene(view->contentsRect().topLeft()), view->mapToScene(view->contentsRect().bottomRight()));
                QPolygonF pol = ( polygon().intersected( QPolygonF(viewportSceneRect) ) );
                if(pol.isEmpty())
                    view->centerOn(this);
            }
    }
    else
    {
        setPen(_areaInitData.penNormal);
        setBrush(_areaInitData.brushNormal);
        setZValue(_areaInitData.zValue);
    }
}

void AreaGraphicsItem::setItemEnabled(bool on_off)
{
    setEnabled(on_off);
    setAcceptedMouseButtons(on_off ? Qt::AllButtons : Qt::NoButton);
}

void AreaGraphicsItem::setItemVisible(bool on_off)
{
    setVisible(on_off);
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

QPolygonF AreaGraphicsItem::getPolygon()
{
    return polygon();
}

void AreaGraphicsItem::reinit()
{

}

void AreaGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(_areaInitData.isSelectableFromMap)
    {
        if(polygon().containsPoint(event->pos(), Qt::OddEvenFill))
        {
            //event->accept();
            emit signalSelectItem(_areaInitData.id, false);
            qDebug() << "AreaGraphicsItem::mousePressEvent, pos:" << event->pos() << "isAccepted:" << event->isAccepted();
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
