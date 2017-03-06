#ifndef ROOMGRAPHICSTEM_H
#define ROOMGRAPHICSTEM_H

#include <QGraphicsPolygonItem>
#include "types.h"

class RoomGraphicstem : public QGraphicsPolygonItem
{
public:
    RoomGraphicstem(const pdf_editor::Room& room);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

};

#endif // ROOMGRAPHICSTEM_H
