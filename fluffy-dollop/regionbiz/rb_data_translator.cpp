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
    if( !area )
        return false;

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
            BaseMetadata::removeForEntity( area->getId() );
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
            BaseMetadata::addForEntityByName( data );
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
    if( !mark )
        return false;

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
            BaseMetadata::removeForEntity( mark->getId() );
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
            BaseFileKeeper::addFile( file );
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
            BaseMetadata::removeForEntity( group->getId() );
            BaseEntity::deleteEntity( group );
        }
        return del;
    }
    else
        return false;
}

LayerPtrs BaseDataTranslator::loadLayers()
{
    if( _load_layers )
    {
        auto layers = _load_layers();
        return layers;
    }
    else
        return LayerPtrs();
}

bool BaseDataTranslator::commitLayers()
{
    if( _commit_layers )
        return _commit_layers();
    else
        return false;
}

bool BaseDataTranslator::deleteLayer(LayerPtr layer)
{
    if( _delete_layer )
    {
        bool del = _delete_layer( layer );
        if( del )
        {
            Layer::removeLayer( layer );
            return true;
        }
    }

    return false;
}

TransformById BaseDataTranslator::loadTransformMatrixes()
{
    if( _load_transform_matrix )
    {
        auto matrixes = _load_transform_matrix();
        return matrixes;
    }
    else
        return TransformById();
}

bool BaseDataTranslator::commitTransformMatrix(FacilityPtr facility)
{
    if( _commit_transform_matrix )
        return _commit_transform_matrix( facility );
    else
        return false;
}

GraphEntityPtrs BaseDataTranslator::loadGraphs()
{
    if( _load_graphs )
    {
        auto graph_vec = _load_graphs();
        for( GraphEntityPtr graph: graph_vec )
        {
            // TODO check type of object
            auto mngr = RegionBizManager::instance();
        }
        return graph_vec;
    }
    else
        return GraphEntityPtrs();
}

bool BaseDataTranslator::commitGraph( GraphEntityPtr graph )
{
    if( !graph )
        return false;

    if( _commit_graph )
        return _commit_graph( graph );
    else
        return false;
}

bool BaseDataTranslator::deleteGraph(GraphEntityPtr graph)
{
    if( _delete_graph )
    {
        bool del = _delete_graph( graph );
        if( del )
        {
            BaseMetadata::removeForEntity( graph->getId() );
            BaseEntity::deleteEntity( graph );
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

void BaseDataTranslator::appendNewLayerFromBase(uint64_t id, QString name)
{
    Layer::appendLayer( id, name );
}

void BaseDataTranslator::freeChangedGroups()
{
    GroupWatcher::freeChanged();
}

void BaseDataTranslator::setParentForGraph(GraphEntityPtr graph, uint64_t id_parent)
{
    graph->setParentId( id_parent );
}

void BaseDataTranslator::appendNodeForGraph(GraphEntityPtr graph, GraphNodePtr node)
{
    graph->appendNode( node );
}

void BaseDataTranslator::appendEdgeForGraph(GraphEntityPtr graph, GraphEdgePtr edge)
{
    graph->appendEdge( edge );
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

