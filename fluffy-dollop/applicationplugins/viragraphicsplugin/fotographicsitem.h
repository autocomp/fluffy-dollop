#ifndef FOTOGRAPHICSITEM_H
#define FOTOGRAPHICSITEM_H

#include "viragraphicsitem.h"
#include <QGraphicsPixmapItem>

class HandleDirectionItem;
class QGraphicsLineItem;

class FotoGraphicsItem : public ViraGraphicsItem, public QGraphicsPixmapItem
{
public:
    FotoGraphicsItem(qulonglong id);
    ~FotoGraphicsItem();
    void setItemselected(bool on_off);
    void centerOnItem();
    quint64 getId();
    void reinit();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

    const qulonglong _id;
    HandleDirectionItem * _directionItem = nullptr;
    QGraphicsLineItem * _line = nullptr;
};

#endif // FOTOGRAPHICSITEM_H
