#ifndef MARKGRAPHICSITEM_H
#define MARKGRAPHICSITEM_H

#include "viragraphicsitem.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>

class MarkPreviewItem;

class MarkGraphicsItem : public ViraGraphicsItem, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    MarkGraphicsItem(qulonglong id);
    ~MarkGraphicsItem();
    void setItemselected(bool on_off);
    void centerOnItem();
    quint64 getId();
    void reinit();

protected slots:
    void hoverEnterEventInPreview(bool on_off);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    const qulonglong _id;
    QPixmap _pixmap;
    MarkPreviewItem * _preview = nullptr;
    QGraphicsPolygonItem * _area = nullptr;
};

class MarkPreviewItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    MarkPreviewItem(QPixmap pixmap, QGraphicsItem * parent);

signals:
    void hoverEnterEvent(bool on_off);

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
};

#endif // MARKGRAPHICSITEM_H
