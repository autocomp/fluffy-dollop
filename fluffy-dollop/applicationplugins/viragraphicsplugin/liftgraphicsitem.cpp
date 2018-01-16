#include "liftgraphicsitem.h"
#include <QGraphicsView>
#include <QDir>
#include <QDebug>
#include <QtMath>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <regionbiz/rb_manager.h>
#include <regionbiz/rb_locations.h>

using namespace regionbiz;

LiftGraphicsItem::LiftGraphicsItem(qulonglong id)
    : _id(id)
{
    QPixmap pm(":/img/lift.png");
    setPixmap(pm);
    setOffset(-pm.width()/2., -pm.height()/2.);

    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    setZValue(1000000);

    reinit();
}

LiftGraphicsItem::~LiftGraphicsItem()
{

}

void LiftGraphicsItem::setItemselected(bool on_off)
{
    setPixmap(on_off ? QPixmap(":/img/lift_selected.png") : QPixmap(":/img/lift.png"));
}

void LiftGraphicsItem::setItemEnabled(bool on_off)
{
    setEnabled(on_off);
    setAcceptedMouseButtons(on_off ? Qt::AllButtons : Qt::NoButton);
}

void LiftGraphicsItem::setItemVisible(bool on_off)
{
    setVisible(on_off);
}

void LiftGraphicsItem::centerOnItem()
{
    foreach(QGraphicsView * view, scene()->views())
    {
        QRectF viewportSceneRect(view->mapToScene(view->contentsRect().topLeft()), view->mapToScene(view->contentsRect().bottomRight()));
        if(viewportSceneRect.contains(scenePos()) == false)
            view->centerOn(this);
    }
}

ViraGraphicsItem::ItemType LiftGraphicsItem::getItemType()
{
    return ItemType_Lift;
}

quint64 LiftGraphicsItem::getId()
{
    return _id;
}

void LiftGraphicsItem::reinit()
{
    MarkPtr ptr = RegionBizManager::instance()->getMark(_id);
    if( ! ptr)
        return;

    setPos(ptr->getCenter());
}

void LiftGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalSelectItem(_id, false);
}

void LiftGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalSelectItem(_id, true);
}





































