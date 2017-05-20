#include "foto360graphicsitem.h"
#include <QGraphicsView>
#include <QDir>
#include <QDebug>
#include <QtMath>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <regionbiz/rb_manager.h>
#include <regionbiz/rb_locations.h>

using namespace regionbiz;

Foto360GraphicsItem::Foto360GraphicsItem(qulonglong id)
    : _id(id)
{
    QPixmap pm(":/img/foto360.png");
    setPixmap(pm);
    setOffset(-pm.width()/2., -pm.height()/2.);

    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    setZValue(1000000);

    reinit();
}

Foto360GraphicsItem::~Foto360GraphicsItem()
{

}

void Foto360GraphicsItem::setItemselected(bool on_off)
{
    setPixmap(on_off ? QPixmap(":/img/foto360_selected.png") : QPixmap(":/img/foto360.png"));
}

void Foto360GraphicsItem::centerOnItem()
{
    foreach(QGraphicsView * view, scene()->views())
    {
        QRectF viewportSceneRect(view->mapToScene(view->contentsRect().topLeft()), view->mapToScene(view->contentsRect().bottomRight()));
        if(viewportSceneRect.contains(scenePos()) == false)
            view->centerOn(this);
    }
}

quint64 Foto360GraphicsItem::getId()
{
    return _id;
}

void Foto360GraphicsItem::reinit()
{
    MarkPtr ptr = RegionBizManager::instance()->getMark(_id);
    if( ! ptr)
        return;

    setPos(ptr->getCenter());
}

void Foto360GraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalSelectItem(_id, false);
}

void Foto360GraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalSelectItem(_id, true);
}





































