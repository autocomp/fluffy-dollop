#include "rb_marks.h"

#include "rb_manager.h"
#include "rb_locations.h"

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

LayerPtr Mark::getLayer()
{
    auto mngr = RegionBizManager::instance();
    return mngr->getLayerOfMark( getId() );
}

void Mark::moveToLayer(LayerPtr layer)
{
    if( layer )
        layer->addMark( getId() );
}

void Mark::leaveLayer()
{
    auto layer = getLayer();
    if( layer )
        layer->removeMark( getId() );
}

bool Mark::commit()
{
    auto mngr = RegionBizManager::instance();
    return mngr->commitMark( _id );
}

//--------------------------------------------

MarkPtr MarkFabric::createByType(Mark::MarkType type, uint64_t id, uint64_t parent_id )
{
    MarkPtr mark;
    switch( type )
    {
    case Mark::MT_DEFECT:
        mark = BaseEntity::createWithId< DefectMark >( id, parent_id );
        break;
    case Mark::MT_PHOTO:
        mark = BaseEntity::createWithId< PhotoMark >( id, parent_id );
        break;
    case Mark::MT_PHOTO_3D:
        mark = BaseEntity::createWithId< Photo3dMark >( id, parent_id );
        break;
    case Mark::MT_PLACEHOLDER:
        mark = BaseEntity::createWithId< PlaceHolderMark >( id, parent_id );
        break;
    case Mark::MT_INVALID:
        static_assert( true, "Create mark with invalid type" );
        break;
    }
    return mark;
}

//---------------------------------------------

DefectMark::DefectMark(uint64_t id):
    Mark( id )
{}

Mark::MarkType DefectMark::getMarkType()
{
    return MT_DEFECT;
}

//---------------------------------------------

PhotoMark::PhotoMark(uint64_t id):
    Mark( id )
{}

Mark::MarkType PhotoMark::getMarkType()
{
    return MT_PHOTO;
}

//---------------------------------------------

Photo3dMark::Photo3dMark(uint64_t id):
    Mark( id )
{}

Mark::MarkType Photo3dMark::getMarkType()
{
    return MT_PHOTO_3D;
}

//------------------------------------------

PlaceHolderMark::PlaceHolderMark(uint64_t id):
    Mark( id )
{}

Mark::MarkType PlaceHolderMark::getMarkType()
{
    return MT_PLACEHOLDER;
}


