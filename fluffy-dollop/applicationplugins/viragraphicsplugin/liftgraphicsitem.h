#ifndef LIFTGRAPHICSITEM_H
#define LIFTGRAPHICSITEM_H

#include "viragraphicsitem.h"
#include <QGraphicsPixmapItem>

class LiftGraphicsItem : public ViraGraphicsItem, public QGraphicsPixmapItem
{
public:
    LiftGraphicsItem(qulonglong id);
    ~LiftGraphicsItem();
    void setItemselected(bool on_off);
    void setItemEnabled(bool on_off);
    void setItemVisible(bool on_off);
    void centerOnItem();
    ItemType getItemType();
    quint64 getId();
    void reinit();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    const qulonglong _id;
};

#endif // LIFTGRAPHICSITEM_H
