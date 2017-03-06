#include "locationitem.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

LocationItem::LocationItem(int minZoom, QRectF bSceneRect, const QList<QGraphicsItem*>& items, QGraphicsScene* scene)
    : _minZoom(minZoom)
    , _bSceneRect(bSceneRect)
    , _items(items)
{
    QPixmap pm(":/img/mark_on_map.png");
    setPixmap(pm);
    setOffset(-pm.width()/2., -pm.height());
    setFlags(QGraphicsItem::ItemIgnoresTransformations);
    setZValue(1000000);
    setPos(bSceneRect.center());
    scene->addItem(this);

    zoomChanged(0);
}

LocationItem::~LocationItem()
{
    foreach(QGraphicsItem * item, _items)
        delete item;
}

void LocationItem::zoomChanged(int zoom)
{
    if(zoom >= _minZoom)
    {
        if(_isVisible == false)
        {
            hide();
            foreach(QGraphicsItem * item, _items)
                item->setVisible(true);
            _isVisible = true;
        }
    }
    else
    {
        if(_isVisible)
        {
            show();
            foreach(QGraphicsItem * item, _items)
                item->setVisible(false);
            _isVisible = false;
        }
    }
}

void LocationItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit setViewport(_bSceneRect);
}















