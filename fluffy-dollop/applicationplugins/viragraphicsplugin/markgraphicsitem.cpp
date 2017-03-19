#include "markgraphicsitem.h"

#include <QGraphicsPixmapItem>

MarkGraphicsItem::MarkGraphicsItem(const QPixmap &pm)
    : QGraphicsPixmapItem(pm)
{
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setZValue(10000);
    setOffset(-6, -43);
}
