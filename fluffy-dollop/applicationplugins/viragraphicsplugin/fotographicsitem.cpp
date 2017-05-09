#include "fotographicsitem.h"
#include "handledirectionitem.h"
#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QDir>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <regionbiz/rb_manager.h>
#include <regionbiz/rb_locations.h>

using namespace regionbiz;

FotoGraphicsItem::FotoGraphicsItem(qulonglong id)
    : _id(id)
{
    setPixmap(QPixmap(":/img/foto.png"));
    setOffset(-25, -22);

    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    setZValue(1000000);

    reinit();
}

FotoGraphicsItem::~FotoGraphicsItem()
{

}

void FotoGraphicsItem::setItemselected(bool on_off)
{
    if(on_off)
    {
        setPixmap(QPixmap(":/img/foto_selected.png"));
    }
    else
    {
        setPixmap(QPixmap(":/img/foto.png"));
    }
}

void FotoGraphicsItem::centerOnItem()
{
    foreach(QGraphicsView * view, scene()->views())
    {
        QRectF viewportSceneRect(view->mapToScene(view->contentsRect().topLeft()), view->mapToScene(view->contentsRect().bottomRight()));
        if(viewportSceneRect.contains(scenePos()) == false)
            view->centerOn(this);
    }
}

quint64 FotoGraphicsItem::getId()
{
    return _id;
}

void FotoGraphicsItem::reinit()
{
    MarkPtr ptr = RegionBizManager::instance()->getMark(_id);
    if( ! ptr)
        return;

    setPos(ptr->getCenter());

    double direction(-90);
    BaseMetadataPtr directionPtr = ptr->getMetadata("foto_direction");
    if(directionPtr)
        direction = directionPtr->getValueAsVariant().toDouble();
    //direction -= 180;

    QPen pen;
    QColor colPen;
    colPen.setRgba(qRgba(0,0,0,255)); // qRgba(0x09,0x61,0xD3,150));
    pen.setColor(colPen);
    pen.setWidth(2);
    pen.setCosmetic(true);
    pen.setJoinStyle(Qt::RoundJoin);

    double r(45);
    QPointF pos(r * cos( -direction * M_PI / 180 ), r * sin( -direction * M_PI / 180 ) );
    QLineF line(QPointF(0,0), pos);

    if( ! _line)
    {
        _line = new QGraphicsLineItem(this);
        _line->setFlag(QGraphicsItem::ItemStacksBehindParent, true);
        _line->setPen(pen);
    }
    _line->setLine(line);

    if( ! _directionItem)
        _directionItem = new HandleDirectionItem(this, false);
    _directionItem->setPos(pos);
}

void FotoGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalSelectItem(_id, false);
}

void FotoGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalSelectItem(_id, true);
}





































