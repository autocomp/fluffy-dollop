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
    return _center;
}

void Mark::setCenter( QPointF center )
{
    _center = center;
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
