#include "handleitem.h"

using namespace pixmap_transform_state;

HandleItem::HandleItem(PixmapTransformState &controller, HandleType handleType, QGraphicsScene *scene, QGraphicsItem * _parent)
    : QGraphicsPixmapItem(_parent)
    , _controller(controller)
    , _handleType(handleType)
{
    setZValue(4);

    switch(_handleType)
    {
    case HandleType::TopLeft :
    case HandleType::BottomRight :
        _cursor = QCursor(Qt::SizeFDiagCursor);
    break;
    case HandleType::TopCenter :
    case HandleType::BottomCenter :
        _cursor = QCursor(Qt::SizeVerCursor);
    break;
    case HandleType::TopRight :
    case HandleType::BottomLeft :
        _cursor = QCursor(Qt::SizeBDiagCursor);
    break;
    case HandleType::RightCenter :
    case HandleType::LeftCenter :
        _cursor = QCursor(Qt::SizeHorCursor);
    break;
    case HandleType::Rotater :
        _cursor = QCursor(QCursor(QPixmap("://img/turn_cursor.png")));
    break;
    case HandleType::Anchor :
        _cursor = QCursor(QCursor(QPixmap("://img/arrow_cross_cursor.png"), 0, 0));
    break;
    }
    setCursor(_cursor);

    setFlags(QGraphicsItem::ItemIgnoresTransformations | QGraphicsItem::ItemIgnoresParentOpacity);

    switch(_handleType)
    {
    case HandleType::Anchor :
        setFlags(QGraphicsItem::ItemIgnoresTransformations | QGraphicsItem::ItemIgnoresParentOpacity | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemSendsGeometryChanges);
        _pmOrdinar = QPixmap("://img/handle_anchor.png");
        setPixmap(_pmOrdinar);
    break;
    case HandleType::Rotater :
        _pmOrdinar = QPixmap("://img/handle_rotater.png");
        setPixmap(_pmOrdinar);
    break;
    default:
        _pmOrdinar = QPixmap("://img/handle.png");
        setPixmap(_pmOrdinar);
    }
    setPixmap(_pmOrdinar);
    setOffset(-_pmOrdinar.width()/2.,-_pmOrdinar.height()/2.);
}

HandleType HandleItem::getHandleType()
{
    return _handleType;
}

void HandleItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsPixmapItem::mousePressEvent(event);
    setCursor(QCursor(Qt::ArrowCursor));
    _controller.pressHandle(_handleType);
}

void HandleItem::mouseRelease()
{
    setCursor(_cursor);
}

void HandleItem::updatePos()
{
    const QRectF r = _controller.getCropArea();
    switch(_handleType)
    {
    case HandleType::TopLeft : {
        setPos(r.topLeft());
    }break;
    case HandleType::TopCenter : {
        setPos(r.x() + r.width()/2., r.y());
    }break;
    case HandleType::TopRight : {
        setPos(r.topRight());
    }break;
    case HandleType::RightCenter : {
        setPos(r.x() + r.width(), r.y() + r.height()/2.);
    }break;
    case HandleType::BottomRight : {
        setPos(r.bottomRight());
    }break;
    case HandleType::BottomCenter : {
        setPos(r.x() + r.width()/2., r.y() + r.height());
    }break;
    case HandleType::BottomLeft : {
        setPos(r.bottomLeft());
    }break;
    case HandleType::LeftCenter : {
        setPos(r.x(), r.y() + r.height()/2.);
    }break;
    }
}

//----------------------------------------------------------------------------------------------------------

AnchorHandleItem::AnchorHandleItem(PixmapTransformState &controller, QGraphicsScene *scene, QGraphicsItem *_parent)
    : HandleItem(controller, HandleType::Anchor, scene, _parent)
{
    setZValue(5);
}

void AnchorHandleItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    _itemMoved = true;
    HandleItem::mousePressEvent(event);

}

void AnchorHandleItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    _itemMoved = false;
    QGraphicsPixmapItem::mouseReleaseEvent(event);
    setCursor(_cursor);
    setPixmap(_pmOrdinar);
    _controller.pressHandle(HandleType::Invalid);
    _controller.anchorReplaced();
}

QVariant AnchorHandleItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemPositionChange && _itemMoved)
    {
        QPointF currPos = pos();
        QPointF newPos = value.toPointF();
        if(parentItem()->contains(newPos))
        {
            QVariant res = QGraphicsItem::itemChange(change, value);
            _controller.anchorMoved();
            return res;
        }
        else
        {
            return currPos;
        }
    }
    else
       return QGraphicsItem::itemChange(change, value);
}

//----------------------------------------------------------------------------------------------------------

RotaterHandleItem::RotaterHandleItem(PixmapTransformState &controller, int _deltaByVertical_, QGraphicsScene *scene, QGraphicsItem *_parent)
    : HandleItem(controller, HandleType::Rotater, scene, _parent)
    , _deltaByVertical(_deltaByVertical_)
{
}

void RotaterHandleItem::setDeltaByVertical(int val)
{
    _deltaByVertical = val;
}

int RotaterHandleItem::deltaByVertical()
{
    return _deltaByVertical;
}


























