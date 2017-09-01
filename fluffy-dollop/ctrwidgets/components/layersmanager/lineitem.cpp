#include "lineitem.h"
#include <QPen>
#include <QPainter>
#include <QGraphicsScene>
#include <QDebug>

using namespace transform_state;

LineItem::LineItem(QGraphicsScene * _scene, QPen pen)
{
    setPen(pen);
    _scene->addItem(this);
}

LineItem::LineItem(QGraphicsItem *_parent, QPen pen)
    : QGraphicsLineItem(_parent)
{
    setPen(pen);
    setZValue(2);
}

void LineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(line().isNull() == false)
    {
        painter->setRenderHint(QPainter::Antialiasing);
        QGraphicsLineItem::paint(painter, option, widget);
    }
}

//-----------------------------------------------------------------

RotaterLineItem::RotaterLineItem(QGraphicsItem *_parent, QPen pen1, QPen pen2)
{
    _line1 = new LineItem(_parent, pen1);
    _line1->setZValue(3);
    _line2 = new LineItem(_parent, pen2);
    _line2->setZValue(3);
}

void RotaterLineItem::setLine(QPointF p1, QPointF p2)
{
    _line1->setLine(QLineF(p1, p2));
    _line2->setLine(QLineF(p1, p2));
}

void RotaterLineItem::setVisible(bool on_off)
{
    _line1->setVisible(on_off);
    _line2->setVisible(on_off);
}
