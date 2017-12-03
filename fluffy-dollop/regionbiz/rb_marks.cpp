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

MarkPtr MarkFabric::createByType(Mark::MarkType type, uint64_t id )
{
    MarkPtr mark;
    switch( type )
    {
    case Mark::MT_DEFECT:
        mark = BaseEntity::createWithId< DefectMark >( id );
        break;
    case Mark::MT_PHOTO:
        mark = BaseEntity::createWithId< PhotoMark >( id );
        break;
    case Mark::MT_PHOTO_3D:
        mark = BaseEntity::createWithId< Photo3dMark >( id );
        break;
    case Mark::MT_PLACEHOLDER:
        mark = BaseEntity::createWithId< PlaceHolderMark >( id );
        break;
    case Mark::MT_LIFT:
        mark = BaseEntity::createWithId< LiftMark >( id );
        break;
    case Mark::MT_STAIRS:
        mark = BaseEntity::createWithId< StairsMark >( id );
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

//-----------------------------------------

#define FLOOR_SET "floor_number_set"
#define FLOOR_SEPARATOR ':'

VerticalWay::VerticalWay(uint64_t id):
    Mark( id )
{}

bool VerticalWay::addFloorNumber( int number )
{
    if( !isMetadataPresent( FLOOR_SET ))
        addMetadata( "string", FLOOR_SET, "" );

    QString floors = getMetadataValue( FLOOR_SET ).toString();
    QStringList floors_list = floors.split( FLOOR_SEPARATOR,
                                            QString::SkipEmptyParts );
    for( QString str: floors_list )
        if( str.toInt() == number )
            return true;

    floors_list.push_back( QString::number( number ));
    QString val = floors_list.join( FLOOR_SEPARATOR );
    return setMetadataValue( FLOOR_SET, val );
}

bool VerticalWay::removeFloorNumber(int number)
{
    if( !isMetadataPresent( FLOOR_SET ))
        addMetadata( "string", FLOOR_SET, "" );

    QString floors = getMetadataValue( FLOOR_SET ).toString();
    QStringList floors_list = floors.split( FLOOR_SEPARATOR,
                                            QString::SkipEmptyParts );
    for( uint i = 0; i < floors_list.size(); ++i )
        if( floors_list[ i ].toInt() == number )
        {
            floors_list.removeAt( i );
            QString val = floors_list.join( FLOOR_SEPARATOR );
            return setMetadataValue( FLOOR_SET, val );
        }

    return true;
}

bool VerticalWay::isPresentOnFloor(int number)
{
    if( !isMetadataPresent( FLOOR_SET ))
        addMetadata( "string", FLOOR_SET, "" );

    QString floors = getMetadataValue( FLOOR_SET ).toString();
    QStringList floors_list = floors.split( FLOOR_SEPARATOR,
                                            QString::SkipEmptyParts );
    for( QString str: floors_list )
        if( str.toInt() == number )
            return true;

    return false;
}

std::vector<int> VerticalWay::getFloorNumbers()
{
    if( !isMetadataPresent( FLOOR_SET ))
        addMetadata( "string", FLOOR_SET, "" );

    QString floors = getMetadataValue( FLOOR_SET ).toString();
    QStringList floors_list = floors.split( FLOOR_SEPARATOR,
                                            QString::SkipEmptyParts );
    std::vector<int> numbers;
    for( QString str: floors_list )
        numbers.push_back( str.toInt() );

    return numbers;
}

#undef FLOOR_SEPARATOR
#undef FLOOR_SET

//------------------------------------------

LiftMark::LiftMark(uint64_t id):
    VerticalWay( id )
{}

Mark::MarkType LiftMark::getMarkType()
{
    return MT_LIFT;
}

//---------------------------------------

StairsMark::StairsMark(uint64_t id):
    VerticalWay( id )
{}

Mark::MarkType StairsMark::getMarkType()
{
    return MT_STAIRS;
}
