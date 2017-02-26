#include "basescenelayer.h"
#include<QPen>
#include <QPainter>
#include<QDebug>

using namespace visualize_system;

BaseSceneLayer::BaseSceneLayer(QRectF rect, quint64 id)
    : QGraphicsRectItem(rect)
    , ID(id)
    , parentId(0)
{

    // qDebug() << "BaseSceneLayer, ID :" << ID << ", rect :" << rect;
    QPen pen(Qt::red);
    pen.setWidth(1);
    pen.setCosmetic(true);
    setPen(pen);
    //seQGraphicsItem::ItemHasNoContents
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    m_layerType = SLT_UNKNOWN;

      //  setBrush(QBrush(Qt::red));
}


void BaseSceneLayer::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget )
{
    //if(ID == 0 && m_layerType == SLT_VIEWRECT)
    {
       // QGraphicsRectItem::paint(painter, option, widget);
    }
}


BaseSceneLayer::~BaseSceneLayer()
{
    qulonglong THIS = (qulonglong)this;
    //qDebug() << "--->BaseSceneLayer::~BaseSceneLayer() :" << THIS;

    emit signalDestroyed();
}

void BaseSceneLayer::setCreatingMode(bool on_off)
{
    emit signalSetCreatingMode(on_off);
}

void BaseSceneLayer::setMouseEventInEditMode(const QMouseEvent * const e, QPointF scenePos)
{
    emit signalMouseEventInEditMode(e,scenePos);
}


void BaseSceneLayer::setRect(const QRectF &rect)
{
    if(m_rect != rect)
    {
        m_rect = rect;
        QGraphicsRectItem::setRect(rect);
    }
}


BaseSceneLayer::SceneLayerType BaseSceneLayer::getLayerType()
{
    return m_layerType;
}

QRectF BaseSceneLayer::getRect()
{
    return m_rect;
}

bool BaseSceneLayer::isClickable(const QMouseEvent * const e, QPointF scenePos, double sceneScale)
{
    bool yes_no(false);
    emit signalIsClickable(yes_no, e, scenePos, sceneScale);
    return yes_no;
}


bool BaseSceneLayer::isSelectable(QPolygonF scenePoly, QPointF *intersectPoint)
{
    bool yes_no(false);
    if(0 == intersectPoint)
    {
        QPointF p1;
        emit signalGetIntersectPoint(yes_no,scenePoly, &p1);
    }
    else
    {
        emit signalGetIntersectPoint(yes_no,scenePoly, intersectPoint);
    }

    return yes_no;
}


bool BaseSceneLayer::isSelectable(QRectF sceneRect, QPointF *intersectPoint)
{
    bool yes_no(false);
    if(0 == intersectPoint)
    {
        QPointF p1;
        emit signalGetIntersectPoint(yes_no,sceneRect, &p1);
    }
    else
    {
        emit signalGetIntersectPoint(yes_no,sceneRect, intersectPoint);
    }

    return yes_no;
}


void BaseSceneLayer::setSceneLayerType(SceneLayerType _type)
{
    m_layerType = _type;
}

void BaseSceneLayer::setLayerToCenterViewport()
{
    emit signalSetLayerToCenterViewport(boundingRect().center());
}

void BaseSceneLayer::setHighLightObject(bool on_off)
{
    //    qulonglong THIS = (qulonglong)this;
    //    qDebug() << "--->BaseSceneLayer::setHighLightObject :" << THIS << on_off;
    emit signalHighLightObject(on_off);
}

