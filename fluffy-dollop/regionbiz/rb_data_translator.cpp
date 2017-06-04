#include "rb_data_translator.h"

#include <iostream>
#include <tuple>

#include "rb_manager.h"

using namespace regionbiz;

// use template checker
#define CHECK_FUNCTIONS( this_type, total_type, ret, ... ) \
    if( this_type & total_type ) \
        ret = checkFunctions( std::make_tuple( __VA_ARGS__ )) \

bool BaseDataTranslator::checkTranslator( CheckType type, QString &err )
{
    bool no_warning = true;

    // all load
    CHECK_FUNCTIONS( CT_READ, type, no_warning,
                     _load_regions, _load_locations, _load_facilitys,
                     _load_floors, _load_rooms,
                     _load_propertys, _load_rents,
                     _load_metadata, _load_marks );
    if( !no_warning )
    {
        err = "Some of Load functons not declared";
        return no_warning;
    }

    // commit
    CHECK_FUNCTIONS( CT_COMMIT, type, no_warning,
                     _commit_area, _commit_mark );
    if( !no_warning )
    {
        err = "Some of Commit functons not declared";
        return no_warning;
    }

    // delete
    CHECK_FUNCTIONS( CT_DELETE, type, no_warning,
                     _delete_area, _delete_mark );
    if( !no_warning )
    {
        err = "Some of Delete functons not declared";
        return no_warning;
    }

    return no_warning;
}

bool BaseDataTranslator::checkTranslator(CheckType check_type)
{
    QString tmp_str;
    bool check_state = checkTranslator( check_type, tmp_str );
    return check_state;
}

RegionPtrs BaseDataTranslator::loadRegions()
{
    return loadAreas< RegionPtrs >( _load_regions );
}

LocationPtrs BaseDataTranslator::loadLocations()
{
    return loadAreas< LocationPtrs >( _load_locations );
}

FacilityPtrs BaseDataTranslator::loadFacilitys()
{
    return loadAreas< FacilityPtrs >( _load_facilitys );
}

FloorPtrs BaseDataTranslator::loadFloors()
{
    return loadAreas< FloorPtrs >( _load_floors );
}

RoomPtrs BaseDataTranslator::loadRooms()
{
    return loadAreas< RoomPtrs >( _load_rooms );
}

bool BaseDataTranslator::commitArea( BaseAreaPtr area )
{
    // if we try to commit area
    // that hold outside model system
    auto mngr = RegionBizManager::instance();
    if( !mngr->getBaseArea( area->getId() ))
        return false;

    // commit
    if( _commit_area )
        return _commit_area( area );
    else
        return false;
}

bool BaseDataTranslator::deleteArea(BaseAreaPtr area)
{
    if( _delete_area )
    {
        bool del = _delete_area( area );
        if( del )
        {
            // remove from model system
            auto mngr = RegionBizManager::instance();
            mngr->_metadata.erase( area->getId() );
            BaseEntity::deleteEntity( area );
        }
        return del;
    }
    else
        return false;
}

//std::vector<PropertyPtr> BaseDataTranslator::loadPropertys()
//{
//    if( _load_propertys )
//        return _load_propertys();
//    else
//        return std::vector<PropertyPtr>();
//}

//std::vector<RentPtr> BaseDataTranslator::loadRents()
//{
//    if( _load_rents )
//        return _load_rents();
//    else
//        return std::vector<RentPtr>();
//}

BaseMetadataPtrs BaseDataTranslator::loadMetadata()
{
    if( _load_metadata )
    {
        BaseMetadataPtrs metadata = _load_metadata();
        for( BaseMetadataPtr data: metadata )
        {
            // add by parent_id / name of metadata
            auto mngr = RegionBizManager::instance();
            mngr->_metadata[ data->getParentId() ][ data->getName() ] = data;
        }

        return metadata;
    }
    else
        return BaseMetadataPtrs();
}

MarkPtrs BaseDataTranslator::loadMarks()
{
    if( _load_marks )
    {
        auto marks_vec = _load_marks();
        for( MarkPtr mark: marks_vec )
        {
            // TODO check type of object
            auto mngr = RegionBizManager::instance();
        }
        return marks_vec;
    }
    else
        return MarkPtrs();
}

bool BaseDataTranslator::commitMark( MarkPtr mark )
{
    if( _commit_mark )
        return _commit_mark( mark );
    else
        return false;
}

bool BaseDataTranslator::deleteMark( MarkPtr mark )
{
    if( _delete_mark )
    {
        bool del = _delete_mark( mark );
        if( del )
        {
            auto mngr = RegionBizManager::instance();
            mngr->_metadata.erase( mark->getId() );
            BaseEntity::deleteEntity( mark );
        }
        return del;
    }
    else
        return false;
}

BaseFileKeeperPtrs BaseDataTranslator::loadFiles()
{
    if( _load_files )
    {
        auto file_vec = _load_files();
        for( BaseFileKeeperPtr file: file_vec )
        {
            // TODO check entity id of file
            FileKeepersById& files = BaseFileKeeper::getFiles();
            files[ file->getEntityId() ].push_back( file );
        }

        return file_vec;
    }
    else
        return BaseFileKeeperPtrs();
}

GroupEntityPtrs BaseDataTranslator::loadGroups()
{
    if( _load_groups )
    {
        auto groups = _load_groups();
        return groups;
    }
    else
        return GroupEntityPtrs();
}

bool BaseDataTranslator::commitGroups( GroupEntityPtrs groups )
{
    for( GroupEntityPtr group: groups )
        if( group->getElements().empty() )
            deleteGroup( group );

    if( _commit_groups )
    {
        bool comm = _commit_groups( groups );
        return comm;
    }
    else
        return false;
}

bool BaseDataTranslator::deleteGroup(GroupEntityPtr group)
{
    if( _delete_group )
    {
        group->disband();
        bool del = _delete_group( group );
        if( del )
        {
            auto mngr = RegionBizManager::instance();
            mngr->_metadata.erase( group->getId() );
            BaseEntity::deleteEntity( group );
        }
        return del;
    }
    else
        return false;
}

void BaseDataTranslator::setParentForBaseLocation( BaseAreaPtr loc, uint64_t parent_id )
{
    loc->setParent( parent_id );
}

void BaseDataTranslator::setAreaForBaseRalation(BaseBizRelationPtr relation, uint64_t id)
{
    relation->setAreaId( id );
}

void BaseDataTranslator::freeChangedGroups()
{
    GroupWatcher::freeChanged();
}

//-------------------------------------------------

template<typename Return, typename Func>
auto BaseDataTranslator::loadAreas( Func function ) -> Return
{
    if( function )
    {
        Return areas = function();
        for( BaseAreaPtr area: areas )
        {
            // TODO check id
            area->getId();
            area->getParentId();
        }

        return areas;
    }
    else
        return Return();
}

