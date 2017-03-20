#include "rb_locations.h"

#include "rb_manager.h"

using namespace regionbiz;

typedef BaseArea::AreaType LocationType;
typedef Region::RegionChildFilter RegionChildFilter;
typedef PlanKeeper::PlanParams PlanParams;

BaseArea::BaseArea(uint64_t id):
    BaseEntity( id )
{}

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

BaseAreaPtrs BaseArea::getChilds()
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

Region::Region(uint64_t id):
    BaseArea( id )
{}

LocationType Region::getType()
{
    return AT_REGION;
}

QString Region::getDescription()
{
    return _description;
}

void Region::setDesription(QString descr)
{
    _description = descr;
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
   BaseArea( id )
{}

BaseArea::AreaType Location::getType()
{
    return AT_LOCATION;
}

QString Location::getDescription()
{
    return _description;
}

void Location::setDesription(QString descr)
{
    _description = descr;
}

QString Location::getAddress()
{
    return _address;
}

void Location::setAddress(QString address)
{
    _address = address;
}

std::vector<FacilityPtr> Location::getChilds()
{
    auto mngr = RegionBizManager::instance();
    std::vector<FacilityPtr> facilitys = mngr->getFacilitysByParent( _id );

    return facilitys;
}

QString PlanKeeper::getPlanPath()
{
    return _plan_path;
}

void PlanKeeper::setPlanPath(QString path)
{
    _plan_path = path;
}

QFilePtr PlanKeeper::getPlanFile()
{
    QFile* file = new QFile( _plan_path );
    if( file->exists() )
        return QFilePtr( file );
    else
        return QFilePtr();
}

void PlanKeeper::setPlanParams( PlanParams params )
{
    _params = params;
}

PlanParams PlanKeeper::getPlanParams()
{
    return _params;
}

Facility::Facility(uint64_t id):
    BizRelationKepper( id )
{}

BaseArea::AreaType Facility::getType()
{
    return AT_FACILITY;
}

QString Facility::getDescription()
{
    return _description;
}

void Facility::setDesription(QString descr)
{
    _description = descr;
}

QString Facility::getAddress()
{
    return _address;
}

void Facility::setAddress(QString address)
{
    _address = address;
}

QString Facility::getCadastralNumber()
{
    return _cadastral_number;
}

void Facility::setCadastralNumber(QString number)
{
    _cadastral_number = number;
}

std::vector<FloorPtr> Facility::getChilds()
{
    auto mngr = RegionBizManager::instance();
    std::vector<FloorPtr> floors = mngr->getFloorsByParent( _id );

    return floors;
}

Floor::Floor(uint64_t id):
    BizRelationKepper( id )
{}

BaseArea::AreaType Floor::getType()
{
    return AT_FLOOR;
}

QString Floor::getName()
{
    return _name;
}

void Floor::setName(QString name)
{
    _name = name;
}

uint16_t Floor::getNumber()
{
    return _number;
}

void Floor::setNumber(uint16_t number)
{
    _number = number;
}

BaseAreaPtrs Floor::getChilds(Floor::FloorChildFilter filter)
{
    auto mngr = RegionBizManager::instance();
    std::vector<BaseAreaPtr> childs;

    if( FCF_ALL == filter ||
            FCF_GROUPS == filter )
    {
        std::vector< RoomsGroupPtr > groups = mngr->getRoomsGroupsByParent( _id );
        for( RoomsGroupPtr rg: groups )
            childs.push_back( rg );
    }

    if( FCF_ALL == filter
            || FCF_ROOMS == filter
            || FCF_ALL_ROOMS == filter )
    {
        std::vector<RoomPtr> rooms = mngr->getRoomsByParent( _id );
        for( RoomPtr roo: rooms )
            childs.push_back( roo );
    }

    if( FCF_ALL_ROOMS == filter )
    {
        std::vector< RoomsGroupPtr > groups = mngr->getRoomsGroupsByParent( _id );
        for( RoomsGroupPtr rg: groups )
        {
            std::vector<RoomPtr> rooms = mngr->getRoomsByParent( rg->getId() );
            for( RoomPtr roo: rooms )
                childs.push_back( roo );
        }
    }

    return childs;
}

RoomsGroup::RoomsGroup(uint64_t id):
    BizRelationKepper( id )
{}

BaseArea::AreaType RoomsGroup::getType()
{
    return AT_ROOMS_GROUP;
}

QString RoomsGroup::getAddress()
{
    return _address;
}

void RoomsGroup::setAddress(QString address)
{
    _address = address;
}

QString RoomsGroup::getCadastralNumber()
{
    return _cadastral_number;
}

void RoomsGroup::setCadastralNumber(QString number)
{
    _cadastral_number = number;
}

std::vector< RoomPtr > RoomsGroup::getChilds()
{
    auto mngr = RegionBizManager::instance();
    std::vector<RoomPtr> rooms = mngr->getRoomsByParent( _id );

    return rooms;
}

Room::Room(uint64_t id):
    BizRelationKepper( id )
{}

BaseArea::AreaType Room::getType()
{
    return AT_ROOM;
}

QString Room::getName()
{
    return _name;
}

void Room::setName(QString name)
{
    _name = name;
}
