#include "rb_translator.h"

#include <iostream>

#include "rb_manager.h"

using namespace regionbiz;

bool BaseTranslator::init( QVariantMap settings )
{
    loadFunctions();
    bool init_correct = initBySettings( settings );
    return init_correct;
}

bool BaseTranslator::checkTranslator(QString &err)
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
    QString tmp_str;
    bool check_state = checkTranslator( tmp_str );
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
