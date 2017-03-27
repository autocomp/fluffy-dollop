#include "rb_translator.h"

#include <iostream>
#include <tuple>

#include "rb_manager.h"

using namespace regionbiz;

bool BaseTranslator::init( QVariantMap settings )
{
    loadFunctions();
    bool init_correct = initBySettings( settings );
    return init_correct;
}

// use template checker
#define CHECK_FUNCTIONS( this_type, total_type, ret, ... ) \
    if( this_type & total_type ) \
        ret = checkFunctions( std::make_tuple( __VA_ARGS__ )) \

bool BaseTranslator::checkTranslator( CheckType type, QString &err )
{
    bool no_warning = true;

    // all load
    CHECK_FUNCTIONS( CT_READ, type, no_warning,
                     _load_regions, _load_locations, _load_facilitys,
                     _load_floors, _load_rooms_groups, _load_rooms,
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

bool BaseTranslator::checkTranslator(CheckType check_type)
{
    QString tmp_str;
    bool check_state = checkTranslator( check_type, tmp_str );
    return check_state;
}

RegionPtrs BaseTranslator::loadRegions()
{
    return loadAreas< RegionPtrs >( _load_regions );
}

LocationPtrs BaseTranslator::loadLocations()
{
    return loadAreas< LocationPtrs >( _load_locations );
}

FacilityPtrs BaseTranslator::loadFacilitys()
{
    return loadAreas< FacilityPtrs >( _load_facilitys );
}

FloorPtrs BaseTranslator::loadFloors()
{
    return loadAreas< FloorPtrs >( _load_floors );
}

RoomsGroupPtrs BaseTranslator::loadRoomsGroups()
{
    return loadAreas< RoomsGroupPtrs >( _load_rooms_groups );
}

RoomPtrs BaseTranslator::loadRooms()
{
    return loadAreas< RoomPtrs >( _load_rooms );
}

bool BaseTranslator::commitArea( BaseAreaPtr area )
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

bool BaseTranslator::deleteArea(BaseAreaPtr area)
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
            mngr->removeArea( area );
        }
        return del;
    }
    else
        return false;
}

//std::vector<PropertyPtr> BaseTranslator::loadPropertys()
//{
//    if( _load_propertys )
//        return _load_propertys();
//    else
//        return std::vector<PropertyPtr>();
//}

//std::vector<RentPtr> BaseTranslator::loadRents()
//{
//    if( _load_rents )
//        return _load_rents();
//    else
//        return std::vector<RentPtr>();
//}

BaseMetadataPtrs BaseTranslator::loadMetadata()
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

MarkPtrs BaseTranslator::loadMarks()
{
    if( _load_marks )
    {
        auto marks_vec = _load_marks();
        for( MarkPtr mark: marks_vec )
        {
            // TODO check type of object
            auto mngr = RegionBizManager::instance();
            mngr->_marks.push_back( mark );
        }
        return marks_vec;
    }
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
    {
        bool del = _delete_mark( mark );
        if( del )
        {
            auto mngr = RegionBizManager::instance();
            mngr->_metadata.erase( mark->getId() );
            BaseEntity::deleteEntity( mark );
            mngr->removeMark( mark );
        }
        return del;
    }
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

bool BaseTranslator::initBySettings(QVariantMap /*settings*/)
{
    return true;
}

//-------------------------------------------------

template<typename Return, typename Func>
auto BaseTranslator::loadAreas( Func function ) -> Return
{
    if( function )
    {
        Return areas = function();
        for( BaseAreaPtr area: areas )
        {
            // TODO check id
            area->getId();
            area->getParentId();
            RegionBizManager::instance()->appendArea( area );
        }

        return areas;
    }
    else
        return Return();
}

//--------------------------------------------------------------

BaseTranslatorPtr BaseTranslatorFabric::getTranslatorByName( QString &name )
{
    for( BaseTranslatorPtr tranlator: getTranslators() )
        if( tranlator->getTranslatorName() == name )
            return tranlator;

    return nullptr;
}

void BaseTranslatorFabric::addTranslator( BaseTranslatorPtr translator )
{
    std::cout << "Add translator: "
              << translator->getTranslatorName().toUtf8().data() << std::endl;
    getTranslators().push_back( translator );
}

BaseTranslatorPtrs &BaseTranslatorFabric::getTranslators()
{
    static BaseTranslatorPtrs translarots;
    return translarots;
}
