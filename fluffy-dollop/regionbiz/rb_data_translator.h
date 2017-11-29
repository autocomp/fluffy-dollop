#ifndef RB_DATA_TRANSLATOR_H
#define RB_DATA_TRANSLATOR_H

#include <functional>
#include <vector>
#include <memory>
#include <QVariantMap>

#include "rb_translator.h"
#include "rb_locations.h"
#include "rb_biz_relations.h"
#include "rb_metadata.h"
#include "rb_marks.h"
#include "rb_files.h"
#include "rb_group.h"
#include "rb_layers.h"
#include "rb_transform_matrix.h"
#include "rb_graph.h"

namespace regionbiz {

class BaseDataTranslator: public BaseTranslator
{
public:
    enum CheckType
    {
        CT_READ = 0x1,
        CT_COMMIT = 0x2,
        CT_DELETE = 0x3
    };

    // TODO think about different checkers
    bool checkTranslator( CheckType check_type, QString& err );
    bool checkTranslator( CheckType check_type );

    // locations
    RegionPtrs loadRegions();
    LocationPtrs loadLocations();
    FacilityPtrs loadFacilitys();
    FloorPtrs loadFloors();
    RoomPtrs loadRooms();

    // TODO check all inside functions

    // commit locations
    bool commitArea(BaseAreaPtr area);

    // delete locations
    bool deleteArea(BaseAreaPtr area);

//    // relations
//    std::vector< PropertyPtr > loadPropertys();
//    std::vector< RentPtr > loadRents();

    // metadata
    BaseMetadataPtrs loadMetadata();

    // marks
    MarkPtrs loadMarks();
    bool commitMark( MarkPtr mark );
    bool deleteMark( MarkPtr mark );

    // files
    BaseFileKeeperPtrs loadFiles();

    // groups
    GroupEntityPtrs loadGroups();
    bool commitGroups( GroupEntityPtrs groups );
    bool deleteGroup( GroupEntityPtr group );

    // layers
    LayerPtrs loadLayers();
    bool commitLayers();
    bool deleteLayer( LayerPtr layer );

    // transform matrixes
    TransformById loadTransformMatrixes();
    bool commitTransformMatrix( FacilityPtr facility );

    // graph
    GraphEntityPtrs loadGraphs();
    bool commitGraph(GraphEntityPtr graph);
    bool deleteGraph( GraphEntityPtr graph );

protected:
    void setParentForBaseLocation( BaseAreaPtr loc, uint64_t parent_id );
    void setAreaForBaseRalation( BaseBizRelationPtr relation, uint64_t id );
    void appendNewLayerFromBase( uint64_t id, QString name );
    void freeChangedGroups();
    void setParentForGraph( GraphEntityPtr graph, uint64_t id_parent );
    void appendNodeForGraph( GraphEntityPtr graph, GraphNodePtr node );
    void appendEdgeForGraph( GraphEntityPtr graph, GraphEdgePtr edge );

    // locations
    std::function< std::vector< RegionPtr >( void ) > _load_regions;
    std::function< std::vector< LocationPtr >( void ) > _load_locations;
    std::function< std::vector< FacilityPtr >( void ) > _load_facilitys;
    std::function< std::vector< FloorPtr >( void ) > _load_floors;
    std::function< std::vector< RoomPtr >( void ) > _load_rooms;
    std::function< GraphEntityPtrs( void ) > _load_graphs;

    // delete location
    std::function< bool( BaseAreaPtr ) > _delete_area;

    // commit locations
    std::function< bool( BaseAreaPtr ) > _commit_area;

    // relations
    std::function< std::vector< PropertyPtr >( void ) > _load_propertys;
    std::function< std::vector< RentPtr >( void ) > _load_rents;

    // metadata
    std::function< std::vector< BaseMetadataPtr >( void ) > _load_metadata;

    // marks
    std::function< std::vector< MarkPtr >( void ) > _load_marks;
    std::function< bool( MarkPtr ) > _delete_mark;
    std::function< bool( MarkPtr ) > _commit_mark;

    // files
    std::function< std::vector< BaseFileKeeperPtr >( void ) > _load_files;

    // groups
    std::function< GroupEntityPtrs( void ) > _load_groups;
    std::function< bool( GroupEntityPtrs ) > _commit_groups;
    std::function< bool( GroupEntityPtr ) > _delete_group;

    // layers
    std::function< LayerPtrs( void ) > _load_layers;
    std::function< bool() > _commit_layers;
    std::function< bool( LayerPtr ) > _delete_layer;

    // transform matrixes
    std::function< TransformById() > _load_transform_matrix;
    std::function< bool( FacilityPtr ) > _commit_transform_matrix;

    // graph
    std::function< bool( GraphEntityPtr ) > _commit_graph;
    std::function< bool( GraphEntityPtr ) > _delete_graph;

private:
    /**
     * Template functors to check (by bool interpret) all elemets
     * CheckTuple - check functor. It's recursive form template
     * checkFunctions -function, that create CheckTuple by size
     */
    template< int index, typename... Ts>
    struct CheckTuple {
        bool operator() (const std::tuple<Ts...>& t) {
            if( !( std::get< index >( t )))
                return false;
            // recursive step
            return CheckTuple< index - 1, Ts... >{}( t );
        }
    };
    template<typename... Ts>
    struct CheckTuple<0, Ts...> {
        bool operator() (const std::tuple<Ts...>& t) {
            // recursive base
            return (bool) ( std::get< 0 >( t ));
        }
    };
    template<typename... Ts>
    bool checkFunctions( const std::tuple<Ts...>& t ) {
        // determine size
        const auto size = std::tuple_size<std::tuple<Ts...>>::value;
        // start recursive
        return CheckTuple<size - 1, Ts...>{}( t );
    }

    template< typename Return, typename Func >
    Return loadAreas( Func function );
};
typedef std::shared_ptr< BaseDataTranslator > BaseDataTranslatorPtr;
typedef std::vector< BaseDataTranslatorPtr > BaseDataTranslatorPtrs;

}

#endif // RB_TRANSLATOR_H
