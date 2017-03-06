#include "roomgraphicstem.h"
#include <QPen>
#include <QBrush>
#include <QPainter>

RoomGraphicstem::RoomGraphicstem(const pdf_editor::Room &room)
    : QGraphicsPolygonItem(room.coordsOnFloor)
{
    QPen pen(Qt::green);
    pen.setWidth(2);
    pen.setCosmetic(true);

    QColor col(Qt::green);
    col.setAlpha(30);
    QBrush brush(col);

    setZValue(100);
    setPen(pen);
    setBrush(brush);
    setToolTip(room.name);

    setAcceptHoverEvents(true);
}

void RoomGraphicstem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    QGraphicsPolygonItem::paint(painter, option, widget);
}

void RoomGraphicstem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QColor col(Qt::green);
    col.setAlpha(100);
    QBrush brush(col);
    setBrush(brush);

    QGraphicsPolygonItem::hoverEnterEvent(event);
}

void RoomGraphicstem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QColor col(Qt::green);
    col.setAlpha(30);
    QBrush brush(col);
    setBrush(brush);

    QGraphicsPolygonItem::hoverLeaveEvent(event);
}
