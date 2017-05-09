#ifndef HANDLEDIRECTIONITEM_H
#define HANDLEDIRECTIONITEM_H

#include <QObject>
#include <QGraphicsPixmapItem>

class HandleDirectionItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    HandleDirectionItem(QGraphicsItem * parent, bool isMovable);
    virtual ~HandleDirectionItem();
    //void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

signals:
    void handleDirectionReplaced(QPointF pos, double direction);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    double _radius = 45;
};

#endif // HANDLEDIRECTIONITEM_H
