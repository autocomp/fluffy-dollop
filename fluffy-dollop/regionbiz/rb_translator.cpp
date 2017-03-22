#include "rb_translator.h"

#include <iostream>

#include "rb_translator_sql.h"

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

    // TODO add checkers
    // relations

    // metadata

    // commit

    // marks

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

bool BaseTranslator::commitArea( BaseAreaPtr area )
{
    if( _commit_area )
        return _commit_area( area );
    else
        return false;
}

bool BaseTranslator::deleteArea(BaseAreaPtr area)
{
    if( _delete_area )
        return _delete_area( area );
    else
        return false;
}

std::vector<PropertyPtr> BaseTranslator::loadPropertys()
{
    if( _load_propertys )
        return _load_propertys();
    else
        return std::vector<PropertyPtr>();
}

std::vector<RentPtr> BaseTranslator::loadRents()
{
    if( _load_rents )
        return _load_rents();
    else
        return std::vector<RentPtr>();
}

BaseMetadataPtrs BaseTranslator::loadMetadata()
{
    if( _load_metadata )
        return _load_metadata();
    else
        return BaseMetadataPtrs();
}

MarkPtrs BaseTranslator::loadMarks()
{
    if( _load_marks )
        return _load_marks();
    else
        return MarkPtrs();
}

bool BaseTranslator::commitMark( MarkPtr mark )
{
    if( _commit_mark )
        return _commit_mark( mark );
    else
        return false;
}

bool BaseTranslator::deleteMark( MarkPtr mark )
{
    if( _delete_mark )
        return _delete_mark( mark );
    else
        return false;
}

void BaseTranslator::setParentForBaseLocation( BaseAreaPtr loc, uint64_t parent_id )
{
    loc->setParent( parent_id );
}

void BaseTranslator::setAreaForBaseRalation(BaseBizRelationPtr relation, uint64_t id)
{
    relation->setAreaId( id );
}

BaseTranslatorPtr BaseTranslatorFabric::getTranslatorByType(std::string &type)
{
    if( "sqlite" == type )
    {
        auto sqlite_trans = BaseTranslatorPtr( new SqliteTranslator() );
        return sqlite_trans;
    }

    if( "psql" == type )
    {
        auto psql_trans = BaseTranslatorPtr( new PsqlTranslator() );
        return psql_trans;
    }

    return nullptr;
}
