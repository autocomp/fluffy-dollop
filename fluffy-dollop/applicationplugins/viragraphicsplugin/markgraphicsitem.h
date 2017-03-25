#ifndef MARKGRAPHICSITEM_H
#define MARKGRAPHICSITEM_H

#include "viragraphicsitem.h"
#include <QGraphicsPixmapItem>

class MarkGraphicsItem : public ViraGraphicsItem, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    MarkGraphicsItem(qulonglong id, const QPixmap& pixmap, const QString& annotation);
    void setItemselected(bool on_off);
    void centerOnItem();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    const qulonglong _id;
    QPixmap _pixmap;
    QGraphicsPixmapItem * _preview = nullptr;
};

#endif // MARKGRAPHICSITEM_H
