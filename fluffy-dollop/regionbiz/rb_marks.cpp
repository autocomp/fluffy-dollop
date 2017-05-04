#include "rb_marks.h"

#include "rb_manager.h"

using namespace regionbiz;

Mark::Mark(uint64_t id):
    BaseEntity( id )
{}

BaseEntity::EntityType Mark::getEntityType()
{
    return EntityType::ET_MARK;
}

QPointF Mark::getCenter()
{
    QRectF bound = _coords.boundingRect();
    QPointF tl = { bound.x(), bound.y() };
    QPointF rb = { bound.x() + bound.width(), bound.y() + bound.height() };

    QPointF center = ( tl + rb ) * 0.5;
    return center;
}

QPolygonF Mark::getCoords()
{
    return _coords;
}

void Mark::setCenter(QPointF center)
{
    _coords = QPolygonF( { center } );
}

void Mark::setCoords(QPolygonF coords)
{
    _coords = coords;
}

uint64_t Mark::getParentId()
{
    return _parent_id;
}

void Mark::setParentId(uint64_t id)
{
    _parent_id = id;
}

bool Mark::commit()
{
    auto mngr = RegionBizManager::instance();
    return mngr->commitMark( _id );
}
