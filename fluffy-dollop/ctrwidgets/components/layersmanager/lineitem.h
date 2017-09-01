#ifndef LINEITEM_H
#define LINEITEM_H

#include <QGraphicsLineItem>

namespace transform_state
{

class LineItem : public QGraphicsLineItem
{
public:
    LineItem(QGraphicsScene * _scene, QPen pen);
    LineItem(QGraphicsItem * _parent, QPen pen);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

class RotaterLineItem
{
public:
    RotaterLineItem(QGraphicsItem * _parent, QPen pen1, QPen pen2);
    void setLine(QPointF p1, QPointF p2);
    void setVisible(bool on_off);

private:
    LineItem * _line1, * _line2;
};

}

#endif // LINEITEM_H
