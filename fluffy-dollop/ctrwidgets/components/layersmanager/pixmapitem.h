#ifndef PIXMAPITEM_H
#define PIXMAPITEM_H

#include "pixmaptransformstate.h"
#include <QGraphicsPixmapItem>

namespace pixmap_transform_state
{

class PixmapItem : public QGraphicsPixmapItem
{
public:
    PixmapItem(PixmapTransformState & controller);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setTransparentBackground(bool on_off);

protected:
    QPointF _deltaPos;
    PixmapTransformState & _controller;
    bool _showTransparentBackground = false;
    QBrush _transparentBrush;
};

}

#endif // PIXMAPITEM_H
