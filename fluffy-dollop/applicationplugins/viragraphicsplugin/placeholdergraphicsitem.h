#ifndef PLACEHOLDERGRAPHICSITEM_H
#define PLACEHOLDERGRAPHICSITEM_H

#include "viragraphicsitem.h"
#include <QGraphicsPixmapItem>

class PlaceholderGraphicsItem : public ViraGraphicsItem, public QGraphicsPixmapItem
{
public:
    PlaceholderGraphicsItem(qulonglong id);
    ~PlaceholderGraphicsItem();
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


#endif // PLACEHOLDERGRAPHICSITEM_H
