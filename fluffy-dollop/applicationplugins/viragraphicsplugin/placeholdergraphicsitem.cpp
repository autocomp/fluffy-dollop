#include "placeholdergraphicsitem.h"
#include <QGraphicsView>
#include <QDir>
#include <QDebug>
#include <QtMath>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <regionbiz/rb_manager.h>
#include <regionbiz/rb_locations.h>

using namespace regionbiz;

PlaceholderGraphicsItem::PlaceholderGraphicsItem(qulonglong id)
    : _id(id)
{
    QPixmap pm(":/img/placeholder.png");
    setPixmap(pm);
    setOffset(-3, -26);

    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    setZValue(1000000);

    reinit();
}

PlaceholderGraphicsItem::~PlaceholderGraphicsItem()
{

}

void PlaceholderGraphicsItem::setItemselected(bool on_off)
{
    setPixmap(on_off ? QPixmap(":/img/placeholder_selected.png") : QPixmap(":/img/placeholder.png"));
}

void PlaceholderGraphicsItem::setItemEnabled(bool on_off)
{
    setEnabled(on_off);
    setAcceptedMouseButtons(on_off ? Qt::AllButtons : Qt::NoButton);
}

void PlaceholderGraphicsItem::setItemVisible(bool on_off)
{
    setVisible(on_off);
}

void PlaceholderGraphicsItem::centerOnItem()
{
    foreach(QGraphicsView * view, scene()->views())
    {
        QRectF viewportSceneRect(view->mapToScene(view->contentsRect().topLeft()), view->mapToScene(view->contentsRect().bottomRight()));
        if(viewportSceneRect.contains(scenePos()) == false)
            view->centerOn(this);
    }
}

ViraGraphicsItem::ItemType PlaceholderGraphicsItem::getItemType()
{
    return ItemType_Placeholder;
}

quint64 PlaceholderGraphicsItem::getId()
{
    return _id;
}

void PlaceholderGraphicsItem::reinit()
{
    MarkPtr ptr = RegionBizManager::instance()->getMark(_id);
    if( ! ptr)
        return;

    setPos(ptr->getCenter());
}

void PlaceholderGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalSelectItem(_id, false);
}

void PlaceholderGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalSelectItem(_id, true);
}





































