#include "locationitem.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

LocationItem::LocationItem(qulonglong id, int minZoom, QRectF bSceneRect, const QList<QGraphicsItem*>& items, QGraphicsScene* scene)
    : _id(id)
    , _minZoom(minZoom)
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
//    foreach(QGraphicsItem * item, _items)
//        item;
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

void LocationItem::setItemselected(bool on_off)
{
    _selected = on_off;
    setPixmap(_selected ? QPixmap(":/img/mark_on_map_selected.png") : QPixmap(":/img/mark_on_map.png"));
}

bool LocationItem::locationIsVisible()
{
    return _isVisible;
}

void LocationItem::addItem(QGraphicsItem *item)
{
    item->setVisible(_isVisible);
    _items.append(item);
}

void LocationItem::removeItem(QGraphicsItem *item)
{
    for(auto it = _items.begin(); it != _items.end(); ++it)
        if( (*it) == item)
        {
            _items.erase(it);
            break;
        }
}

void LocationItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalSelectItem(_id, false);
}

void LocationItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalSelectItem(_id, true);
    emit setViewport(_bSceneRect);
}















