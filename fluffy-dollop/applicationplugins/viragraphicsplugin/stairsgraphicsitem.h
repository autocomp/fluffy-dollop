#ifndef STAIRSGRAPHICSITEM_H
#define STAIRSGRAPHICSITEM_H

#include "viragraphicsitem.h"
#include <QGraphicsPixmapItem>

class StairsGraphicsItem : public ViraGraphicsItem, public QGraphicsPixmapItem
{
public:
    StairsGraphicsItem(qulonglong id);
    ~StairsGraphicsItem();
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

#endif // STAIRSGRAPHICSITEM_H
