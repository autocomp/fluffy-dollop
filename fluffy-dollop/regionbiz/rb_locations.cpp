#include "rb_locations.h"

#include "rb_manager.h"

using namespace regionbiz;

typedef BaseArea::AreaType LocationType;
typedef Region::RegionChildFilter RegionChildFilter;

BaseArea::BaseArea(uint64_t id):
    BaseEntity( id )
{}

BaseEntity::EntityType BaseArea::getEntityType()
{
    return EntityType::ET_AREA;
}

QPolygonF BaseArea::getCoords()
{
    return _coords;
}

void BaseArea::setCoords(QPolygonF coord)
{
    _coords = coord;
}

uint64_t BaseArea::getParentId()
{
    return _parent_id;
}

BaseAreaPtr BaseArea::getParent(AreaType parent_type)
{
    auto mngr = RegionBizManager::instance();
    BaseAreaPtr parent = mngr->getBaseArea( _parent_id, parent_type );

    return parent;
}

BaseAreaPtr BaseArea::getParent()
{
    auto mngr = RegionBizManager::instance();
    BaseAreaPtr parent = mngr->getBaseArea( _parent_id );

    return parent;
}

BaseAreaPtrs BaseArea::getBaseAreaChilds()
{
    auto mngr = RegionBizManager::instance();
    auto childs = mngr->getAreaChildsByParent( _id );

    return childs;
}

bool BaseArea::commit()
{
    auto mngr = RegionBizManager::instance();
    bool comm_res =  mngr->commitArea( _id );

    return comm_res;
}

void BaseArea::setParent(uint64_t id)
{
    _parent_id = id;
}

BizRelationKepper::BizRelationKepper(uint64_t id):
    BaseArea( id )
{}

BaseBizRelationPtrs BizRelationKepper::getBizRelations()
{
    auto mngr = RegionBizManager::instance();
    BaseBizRelationPtrs relations = mngr->getBizRelationByArea( _id );

    return relations;
}

PropertyPtrs BizRelationKepper::getPropertys()
{
    auto mngr = RegionBizManager::instance();
    BaseBizRelationPtrs relations = mngr->getBizRelationByArea( _id,
                                                                BaseBizRelation::RT_PROPERTY );

    PropertyPtrs propertys;
    for( BaseBizRelationPtr rel: relations )
    {
        PropertyPtr prop = BaseBizRelation::convert< Property >( rel );
        if( prop )
            propertys.push_back( prop );
    }
    return propertys;
}

RentPtrs BizRelationKepper::getRents()
{
    auto mngr = RegionBizManager::instance();
    BaseBizRelationPtrs relations = mngr->getBizRelationByArea( _id,
                                                                BaseBizRelation::RT_RENT );

    RentPtrs rents;
    for( BaseBizRelationPtr rel: relations )
    {
        RentPtr rent = BaseBizRelation::convert< Rent >( rel );
        if( rent )
            rents.push_back( rent );
    }
    return rents;
}

MarksHolder::MarksHolder(uint64_t id):
    _holder_id( id )
{}

uint64_t MarksHolder::getHolderId()
{
    return _holder_id;
}

MarkPtrs MarksHolder::getMarks()
{
    MarkPtrs marks;

    if( checkHolderId() )
    {
        auto mngr = RegionBizManager::instance();
        marks = mngr->getMarksByParent( _holder_id );
    }

    return marks;
}

MarkPtr MarksHolder::addMark( QPointF center )
{
    if( checkHolderId() )
    {
        auto mngr = RegionBizManager::instance();
        return mngr->addMark( _holder_id, center );
    }

    return MarkPtr();
}

bool MarksHolder::commitMarks()
{
    if( checkHolderId() )
    {
        auto mngr = RegionBizManager::instance();
        MarkPtrs marks = mngr->getMarksByParent( _holder_id );
        bool comm = true;
        for( MarkPtr mark: marks )
            comm = comm && mark->commit();

        return comm;
    }

    return false;
}

bool MarksHolder::deleteMarks()
{
    if( checkHolderId() )
    {
        auto mngr = RegionBizManager::instance();
        MarkPtrs marks = mngr->getMarksByParent( _holder_id );
        bool del = true;
        for( MarkPtr mark: marks )
            del = del && mngr->deleteMark( mark );

        return del;
    }

    return false;
}

bool MarksHolder::checkHolderId()
{
    auto mngr = RegionBizManager::instance();
    BaseEntityPtr ent = mngr->getBaseArea( _holder_id );
    if( ent )
    {
        MarksHolderPtr holder = BaseEntity::convert< MarksHolder >( ent );
        if( holder )
            return true;
    }

    return false;
}

Region::Region(uint64_t id):
    BaseArea( id )
{}

LocationType Region::getType()
{
    return AT_REGION;
}



std::vector<BaseAreaPtr> Region::getChilds(RegionChildFilter filter )
{
    auto mngr = RegionBizManager::instance();
    std::vector<BaseAreaPtr> childs;

    if( RCF_ALL == filter ||
            RCF_LOCATIONS == filter )
    {
        std::vector<LocationPtr> locations = mngr->getLocationsByParent( _id );
        for( LocationPtr loc: locations )
            childs.push_back( loc );
    }

    if( RCF_ALL == filter
            || RCF_FACILITYS == filter
            || RCF_ALL_FACILITYS == filter )
    {
        std::vector<FacilityPtr> facilitys = mngr->getFacilitysByParent( _id );
        for( FacilityPtr fac: facilitys )
            childs.push_back( fac );
    }

    if( RCF_ALL_FACILITYS == filter )
    {
        std::vector<LocationPtr> locations = mngr->getLocationsByParent( _id );
        for( LocationPtr loc: locations )
        {
            std::vector<FacilityPtr> facilitys = mngr->getFacilitysByParent( loc->getId() );
            for( FacilityPtr fac: facilitys )
                childs.push_back( fac );
        }
    }

    return childs;
}

Location::Location(uint64_t id):
   BaseArea( id ),
   MarksHolder( id )
{}

BaseArea::AreaType Location::getType()
{
    return AT_LOCATION;
}

QString Location::getAddress()
{
    if( isMetadataPresent( "adress" ))
        return getMetadataValue( "adress" ).toString();

    return "";
}

void Location::setAddress(QString address)
{
    if( isMetadataPresent( "address" ))
        setMetadataValue( "address", address );
    else
        addMetadata( "string", "address", address );
}

std::vector<FacilityPtr> Location::getChilds()
{
    auto mngr = RegionBizManager::instance();
    std::vector<FacilityPtr> facilitys = mngr->getFacilitysByParent( _id );

    return facilitys;
}

Facility::Facility(uint64_t id):
    BizRelationKepper( id )
{}

BaseArea::AreaType Facility::getType()
{
    return AT_FACILITY;
}

QString Facility::getAddress()
{
    if( isMetadataPresent( "adress" ))
        return getMetadataValue( "adress" ).toString();

    return "";
}

void Facility::setAddress(QString address)
{
    if( isMetadataPresent( "address" ))
        setMetadataValue( "address", address );
    else
        addMetadata( "string", "address", address );
}

QString Facility::getCadastralNumber()
{
    if( isMetadataPresent( "cadastral_number" ))
        return getMetadataValue( "cadastral_number" ).toString();

    return "";
}

void Facility::setCadastralNumber(QString number)
{
    if( isMetadataPresent( "cadastral_number" ))
        setMetadataValue( "cadastral_number", number );
    else
        addMetadata( "string", "cadastral_number", number );
}

std::vector<FloorPtr> Facility::getChilds()
{
    auto mngr = RegionBizManager::instance();
    std::vector<FloorPtr> floors = mngr->getFloorsByParent( _id );

    return floors;
}

Floor::Floor(uint64_t id):
    BizRelationKepper( id ),
    MarksHolder( id )
{}

BaseArea::AreaType Floor::getType()
{
    return AT_FLOOR;
}

uint16_t Floor::getNumber()
{
    if( isMetadataPresent( "number" ))
        return getMetadataValue( "number" ).toInt();

    return 0;
}

void Floor::setNumber(uint16_t number)
{
    if( isMetadataPresent( "number" ))
        setMetadataValue( "number", number );
    else
        addMetadata( "int", "number", number );
}

RoomPtrs Floor::getChilds()
{
    auto mngr = RegionBizManager::instance();
    std::vector<RoomPtr> rooms = mngr->getRoomsByParent( _id );

    return rooms;
}

Room::Room(uint64_t id):
    BizRelationKepper( id ),
    MarksHolder( id )
{}

BaseArea::AreaType Room::getType()
{
    return AT_ROOM;
}
