#ifndef MEASURETEXTITEM_H
#define MEASURETEXTITEM_H

#include <QGraphicsSimpleTextItem>
#include <QBrush>
#include <QPen>

class MeasureTextItem : public QGraphicsSimpleTextItem
{
public:
    MeasureTextItem(QGraphicsItem * parent, const QBrush &brush, const QPen &pen);
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
    void setText(const QString &text);
    void translate(qreal dx, qreal dy);

    QSize getTextSize();
    void setValue(qreal value);
    qreal getValue();


private:
    QBrush _brush;
    QPen _pen;
    QSize _size;
    qreal _dx, _dy, _value;
};

#endif // MEASURETEXTITEM_H
