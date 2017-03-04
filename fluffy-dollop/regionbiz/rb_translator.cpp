#include "rb_translator.h"

#include <iostream>

#include "rb_translator_sqlite.h"

using namespace regionbiz;

bool BaseTranslator::init( QVariantMap settings )
{
    loadFunctions();
    bool init_correct = initBySettings( settings );
    return init_correct;
}

bool BaseTranslator::checkTranslator(std::string &err)
{
    if( !_load_regions )
    {
        err = "Load Regions functon not declared";
        return false;
    }

    if( !_load_locations )
    {
        err = "Load Locations functon not declared";
        return false;
    }

    if( !_load_facilitys )
    {
        err = "Load Facilitys functon not declared";
        return false;
    }

    if( !_load_floors )
    {
        err = "Load Floors functon not declared";
        return false;
    }

    if( !_load_rooms_groups )
    {
        err = "Load Rooms Groups functon not declared";
        return false;
    }

    if( !_load_rooms )
    {
        err = "Load Rooms functon not declared";
        return false;
    }

    return true;
}

bool BaseTranslator::checkTranslator()
{
    std::string tmp_str;
    bool check_state = checkTranslator( tmp_str );
    return check_state;
}

std::vector<RegionPtr> BaseTranslator::loadRegions()
{
    if( _load_regions )
        return _load_regions();
    else
        return std::vector< RegionPtr >();
}

std::vector<LocationPtr> BaseTranslator::loadLocations()
{
    if( _load_locations )
        return _load_locations();
    else
        return std::vector<LocationPtr>();
}

std::vector<FacilityPtr> BaseTranslator::loadFacilitys()
{
    if( _load_facilitys )
        return _load_facilitys();
    else
        return std::vector<FacilityPtr>();
}

std::vector<FloorPtr> BaseTranslator::loadFloors()
{
    if( _load_floors )
        return _load_floors();
    else
        return std::vector<FloorPtr>();
}

std::vector<RoomsGroupPtr> BaseTranslator::loadRoomsGroups()
{
    if( _load_rooms_groups )
        return _load_rooms_groups();
    else
        return std::vector<RoomsGroupPtr>();
}

std::vector<RoomPtr> BaseTranslator::loadRooms()
{
    if( _load_rooms )
        return _load_rooms();
    else
        return std::vector<RoomPtr>();
}

void BaseTranslator::setParentForBaseLocation( BaseAreaPtr loc, uint64_t parent_id )
{
    loc->setParent( parent_id );
}

BaseTranslatorPtr BaseTranslatorFabric::getTranslatorByType(std::string &type)
{
    if( "sqlite" == type )
    {
        auto sqlite_trans = BaseTranslatorPtr( new SqliteTranslator() );
        return sqlite_trans;
    }

    return nullptr;
}
