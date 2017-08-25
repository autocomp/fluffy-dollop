#ifndef HANDLEITEM_H
#define HANDLEITEM_H

#include <QGraphicsPixmapItem>
#include "pixmaptransformstate.h"

namespace pixmap_transform_state
{

class HandleItem : public QGraphicsPixmapItem
{
public:
    HandleItem(PixmapTransformState & controller, HandleType handleType, QGraphicsScene *scene, QGraphicsItem *_parent = 0);
    HandleType getHandleType();
    void mouseRelease();
    void updatePos();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event);

protected:
    PixmapTransformState & _controller;
    const HandleType _handleType;
    QCursor _cursor;
    QPixmap _pmOrdinar;
};

class AnchorHandleItem : public HandleItem
{
public:
    AnchorHandleItem(PixmapTransformState & controller, QGraphicsScene *scene, QGraphicsItem *_parent = 0);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

protected:
    QCursor _cursor;
    bool _itemMoved = false;
};

class RotaterHandleItem : public HandleItem
{
public:
    RotaterHandleItem(PixmapTransformState & controller, int _deltaByVertical_, QGraphicsScene *scene, QGraphicsItem *_parent = 0);
    void setDeltaByVertical(int val);
    int deltaByVertical();

protected:
    int _deltaByVertical;
};

}

#endif // HANDLEITEM_H