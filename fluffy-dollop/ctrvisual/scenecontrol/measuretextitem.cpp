#include "measuretextitem.h"
#include <QPainter>
#include <QFontMetrics>

MeasureTextItem::MeasureTextItem(QGraphicsItem * _parent, const QBrush &brush, const QPen &pen)
    : QGraphicsSimpleTextItem(_parent)
    , _brush(brush)
    , _pen(pen)
    , _dx(0)
    , _dy(0)
    , _value(0)
{
}

void MeasureTextItem::translate(qreal dx, qreal dy)
{
//    QGraphicsSimpleTextItem::translate(-_dx, -_dy);
//    QGraphicsSimpleTextItem::translate(dx, dy);
//    _dx = dx;
//    _dy = dy;
}

void MeasureTextItem::setText(const QString &text)
{
    QFont font;
    QFontMetrics fontMetrics(font);
    _size = QSize(fontMetrics.width(text)-1, fontMetrics.height()-1);

    QGraphicsSimpleTextItem::setText(text);
}

QSize MeasureTextItem::getTextSize()
{
    return _size;
}

void MeasureTextItem::setValue(qreal value)
{
    _value = value;
}

qreal MeasureTextItem::getValue()
{
    return _value;
}

void MeasureTextItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    painter->save();
    painter->setPen(_pen);
    painter->setBrush(_brush);
    painter->drawRect(0, 0, _size.width(), _size.height());
    painter->restore();

    QGraphicsSimpleTextItem::paint(painter, option, widget);
}
