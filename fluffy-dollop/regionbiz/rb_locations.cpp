#include "rb_locations.h"

#include "rb_manager.h"

using namespace regionbiz;

typedef BaseArea::AreaType LocationType;
typedef Region::RegionChildFilter RegionChildFilter;
typedef PlanKeeper::PlanParams PlanParams;

BaseArea::BaseArea(uint64_t id):
    _id( id )
{}

Coords BaseArea::getCoords()
{
    return _coords;
}

void BaseArea::setCoords(Coords coord)
{
    _coords = coord;
}

uint64_t BaseArea::getId()
{
    return _id;
}

uint64_t BaseArea::getParentId()
{
    return _parent_id;
}

BaseAreaPtr BaseArea::getParent(AreaType parent_type)
{
    auto mngr = RegionBizManager::instance();
    BaseAreaPtr parent = mngr->getBaseLoation( _parent_id, parent_type );

    return parent;
}

BaseAreaPtr BaseArea::getParent()
{
    auto mngr = RegionBizManager::instance();
    BaseAreaPtr parent = mngr->getBaseLoation( _parent_id );

    return parent;
}

void BaseArea::setParent(uint64_t id)
{
    _parent_id = id;
}

Region::Region(uint64_t id):
    BaseArea( id )
{}

LocationType Region::getType()
{
    return AT_REGION;
}

std::string Region::getDescription()
{
    return _description;
}

void Region::setDesription(std::string descr)
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

std::string Location::getDescription()
{
    return _description;
}

void Location::setDesription(std::string descr)
{
    _description = descr;
}

std::string Location::getAddress()
{
    return _address;
}

void Location::setAddress(std::string address)
{
    _address = address;
}

std::vector<FacilityPtr> Location::getChilds()
{
    auto mngr = RegionBizManager::instance();
    std::vector<FacilityPtr> facilitys = mngr->getFacilitysByParent( _id );

    return facilitys;
}

std::string PlanKeeper::getPlanPath()
{
    return _plan_path;
}

void PlanKeeper::setPlanPath(std::string path)
{
    _plan_path = path;
}

QFilePtr PlanKeeper::getPlanFile()
{
    QFile* file = new QFile( QString::fromUtf8( _plan_path.c_str() ));
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
    BaseArea( id )
{}

BaseArea::AreaType Facility::getType()
{
    return AT_FACILITY;
}

std::string Facility::getDescription()
{
    return _description;
}

void Facility::setDesription(std::string descr)
{
    _description = descr;
}

std::string Facility::getAddress()
{
    return _address;
}

void Facility::setAddress(std::string address)
{
    _address = address;
}

std::string Facility::getCadastralNumber()
{
    return _cadastral_number;
}

void Facility::setCadastralNumber(std::string number)
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
    BaseArea( id )
{}

BaseArea::AreaType Floor::getType()
{
    return AT_FLOOR;
}

std::string Floor::getName()
{
    return _name;
}

void Floor::setName(std::string name)
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
    BaseArea( id )
{}

BaseArea::AreaType RoomsGroup::getType()
{
    return AT_ROOMS_GROUP;
}

std::string RoomsGroup::getAddress()
{
    return _address;
}

void RoomsGroup::setAddress(std::string address)
{
    _address = address;
}

std::string RoomsGroup::getCadastralNumber()
{
    return _cadastral_number;
}

void RoomsGroup::setCadastralNumber(std::string number)
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
    BaseArea( id )
{}

BaseArea::AreaType Room::getType()
{
    return AT_ROOM;
}

std::string Room::getName()
{
    return _name;
}

void Room::setName(std::string name)
{
    _name = name;
}
