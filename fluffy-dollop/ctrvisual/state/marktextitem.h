#ifndef MARKTEXTITEM_H
#define MARKTEXTITEM_H

#include <QGraphicsSimpleTextItem>
#include <QBrush>
#include <QPen>

class MarkTextItem : public QGraphicsSimpleTextItem
{
public:
    MarkTextItem(QGraphicsItem * parent, const QBrush &brush, const QPen &pen);
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget);
    void setText(const QString &text);
    void setText(const QStringList &text);
    void translate(qreal dx, qreal dy);
    QSize getTextSize();
    bool showHoverEnterOnly(bool on_off, uint transparency = 70);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    QBrush _brush;
    QPen _pen;
    QSize _size;
    qreal _dx, _dy;
    bool _draw;
    uint _transparency;
};

#endif // MEASURETEXTITEM_H
