#ifndef TRANSFORMINGITEM_H
#define TRANSFORMINGITEM_H

#include "transformingstate.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>
#include <QTransform>

namespace transform_state
{

class TransformingItem
{
public:

    enum TransformingItemType
    {
        PolygonItem,
        PixmapItem,
        SvgItem,
    };

    TransformingItem(TransformingState & controller);
    virtual ~TransformingItem();
    virtual TransformingItemType getTransformingItemType() const = 0;
    virtual QGraphicsItem * castToGraphicsItem() = 0;
    virtual QPointF mapToScene(const QPointF &pos) const = 0;
    virtual QPointF mapFromScene(const QPointF &pos) const = 0;
    virtual void setTransform(const QTransform &matrix) = 0;
    virtual void setPos(const QPointF &pos) = 0;
    virtual void setCursor(const QCursor &cursor) = 0;
    virtual void setFlags(QGraphicsItem::GraphicsItemFlags flags) = 0;
    virtual void setZValue(qreal z) = 0;
    virtual bool contains(const QPointF &pos) const = 0;

protected:
    QPointF _deltaPos;
    TransformingState & _controller;

};


class PolygonTransformingItem : public TransformingItem, public QGraphicsPolygonItem
{
public:
    PolygonTransformingItem(TransformingState & controller);
    virtual TransformingItemType getTransformingItemType() const;
    virtual QGraphicsItem * castToGraphicsItem();
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QPointF mapToScene(const QPointF &pos) const;
    virtual QPointF mapFromScene(const QPointF &pos) const;
    virtual void setTransform(const QTransform &matrix);
    virtual void setPos(const QPointF &pos);
    virtual void setCursor(const QCursor &cursor);
    virtual void setFlags(QGraphicsItem::GraphicsItemFlags flags);
    virtual void setZValue(qreal z);
    virtual bool contains(const QPointF &pos) const;

protected:

};


class PixmapTransformingItem : public TransformingItem, public QGraphicsPixmapItem
{
public:
    PixmapTransformingItem(TransformingState & controller);
    virtual TransformingItemType getTransformingItemType() const;
    virtual QGraphicsItem * castToGraphicsItem();
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QPointF mapToScene(const QPointF &pos) const;
    virtual QPointF mapFromScene(const QPointF &pos) const;
    virtual void setTransform(const QTransform &matrix);
    virtual void setPos(const QPointF &pos);
    virtual void setCursor(const QCursor &cursor);
    virtual void setFlags(QGraphicsItem::GraphicsItemFlags flags);
    virtual void setZValue(qreal z);
    virtual bool contains(const QPointF &pos) const;

    void setTransparentBackground(bool on_off);

protected:
    bool _showTransparentBackground = false;
    QBrush _transparentBrush;
};

}

#endif // PIXMAPITEM_H
