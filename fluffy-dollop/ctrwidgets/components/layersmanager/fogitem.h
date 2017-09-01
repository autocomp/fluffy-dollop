#ifndef FOGITEM_H
#define FOGITEM_H

#include <QGraphicsRectItem>

namespace transform_state
{

class FogItem : public QGraphicsRectItem
{
public:
    FogItem(QGraphicsItem *parent, QRectF pixmapArea);
    ~FogItem();
    void setArea(QRectF area);
    void setArea(QPolygonF area);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private:
    QPolygonF _area;
};

}

#endif // FOGITEM_H
