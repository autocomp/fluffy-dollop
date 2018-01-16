#include "stairsgraphicsitem.h"
#include <QGraphicsView>
#include <QDir>
#include <QDebug>
#include <QtMath>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <regionbiz/rb_manager.h>
#include <regionbiz/rb_locations.h>

using namespace regionbiz;

StairsGraphicsItem::StairsGraphicsItem(qulonglong id)
    : _id(id)
{
    QPixmap pm(":/img/stairs_on_floor.png");
    setPixmap(pm);
    setOffset(-pm.width()/2., -pm.height()/2.);

    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    setZValue(1000000);

    reinit();
}

StairsGraphicsItem::~StairsGraphicsItem()
{

}

void StairsGraphicsItem::setItemselected(bool on_off)
{
    setPixmap(on_off ? QPixmap(":/img/stairs_on_floor_selected.png") : QPixmap(":/img/stairs_on_floor.png"));
}

void StairsGraphicsItem::setItemEnabled(bool on_off)
{
    setEnabled(on_off);
    setAcceptedMouseButtons(on_off ? Qt::AllButtons : Qt::NoButton);
}

void StairsGraphicsItem::setItemVisible(bool on_off)
{
    setVisible(on_off);
}

void StairsGraphicsItem::centerOnItem()
{
    foreach(QGraphicsView * view, scene()->views())
    {
        QRectF viewportSceneRect(view->mapToScene(view->contentsRect().topLeft()), view->mapToScene(view->contentsRect().bottomRight()));
        if(viewportSceneRect.contains(scenePos()) == false)
            view->centerOn(this);
    }
}

ViraGraphicsItem::ItemType StairsGraphicsItem::getItemType()
{
    return ItemType_Stairs;
}

quint64 StairsGraphicsItem::getId()
{
    return _id;
}

void StairsGraphicsItem::reinit()
{
    MarkPtr ptr = RegionBizManager::instance()->getMark(_id);
    if( ! ptr)
        return;

    setPos(ptr->getCenter());
}

void StairsGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalSelectItem(_id, false);
}

void StairsGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalSelectItem(_id, true);
}





































