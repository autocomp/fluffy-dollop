#include "fogitem.h"
#include <QPainter>

using namespace pixmap_transform_state;

FogItem::FogItem(QGraphicsItem *parent, QRectF pixmapArea)
    : QGraphicsRectItem(pixmapArea, parent)
{
    setZValue(1);
    _area = pixmapArea;
}

FogItem::~FogItem()
{
}

void FogItem::setArea(QRectF area)
{
    _area = QPolygonF(area);
    update();
}

void FogItem::setArea(QPolygonF area)
{
    _area = area;
    update();
}

void FogItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->save();

    QColor colorFog;
    QRgb rgb = qRgba(128,128,128,170);
    colorFog.setRgba(rgb);

    QColor colorArea;
    rgb = qRgba(0,0,0,0);
    colorArea.setRgba(rgb);

    QPainterPath path;
    path.addRect(rect());

    path.addPolygon(_area);

    painter->fillRect(rect(), colorArea);

    painter->setPen(Qt::NoPen);
    painter->setBrush(colorFog);
    painter->drawPath(path);

    painter->restore();
}
