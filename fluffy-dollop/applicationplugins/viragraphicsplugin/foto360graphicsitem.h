#ifndef FOTO360GRAPHICSITEM_H
#define FOTO360GRAPHICSITEM_H

#include "viragraphicsitem.h"
#include <QGraphicsPixmapItem>

class Foto360GraphicsItem : public ViraGraphicsItem, public QGraphicsPixmapItem
{
public:
    Foto360GraphicsItem(qulonglong id);
    ~Foto360GraphicsItem();
    void setItemselected(bool on_off);
    void setItemEnabled(bool on_off);
    void centerOnItem();
    quint64 getId();
    void reinit();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    const qulonglong _id;
};

#endif
