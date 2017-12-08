#include "test_graph.h"

#include <regionbiz/rb_manager.h>

using namespace regionbiz;

TestGraph::TestGraph()
{
    auto mngr = RegionBizManager::instance();
    mngr->subscribeOnCurrentChange(
                this, SLOT( onChangeCurrent( uint64_t, uint64_t )));
    mngr->subscribeOnChangeEntity(
                this, SLOT( onChangeEntity( uint64_t )));
}

void TestGraph::onChangeCurrent( uint64_t /*prev_id*/, uint64_t new_id )
{
    auto mngr = RegionBizManager::instance();
    auto current_entity = mngr->getBaseEntity( new_id );

    bool search_parent = true;
    while( search_parent )
    {
        // if not found
        if( !current_entity )
            return;

        #define CHANGE_TO_PARENT( classname ) \
        { \
            auto id_parent = current_entity->convert< classname >()->getParentId(); \
            current_entity = mngr->getBaseEntity( id_parent ); \
            break; \
        }

        BaseEntity::EntityType type = current_entity->getEntityType();
        switch( type )
        {
        case BaseEntity::ET_AREA:
            CHANGE_TO_PARENT( BaseArea )
        case BaseEntity::ET_MARK:
            CHANGE_TO_PARENT( Mark )
        case BaseEntity::ET_GRAPH_EDGE:
            CHANGE_TO_PARENT( GraphEdge )
        case BaseEntity::ET_GRAPH_NODE:
            CHANGE_TO_PARENT( GraphNode );
        case BaseEntity::ET_GRAPH:
            CHANGE_TO_PARENT( GraphEntity );
        case BaseEntity::ET_GROUP:
        {
            // WARNING don't get parent of group
            return;
        }
        default:
            break;
        }

        if( BaseEntity::ET_AREA == current_entity->getEntityType() )
        {
            BaseArea::AreaType area_type =
                    current_entity->convert< BaseArea >()->getType();

            // stop search
            if( area_type == BaseArea::AT_REGION )
                return;

            if( area_type == BaseArea::AT_FACILITY )
            {
                uint64_t current_id = current_entity->getId();
                if( current_id != _current_facility_id )
                {
                    _current_facility_id = current_id;
                    recalculate();
                }
                else
                    return;
            }
        }
    }
}

void TestGraph::onChangeEntity( uint64_t /*id*/ )
{
    recalculate();
}

void TestGraph::recalculate()
{
    using namespace std;

    auto mngr = RegionBizManager::instance();
    auto current_facility = mngr->getBaseArea( _current_facility_id,
                                               BaseArea::AT_FACILITY );
    if( !current_facility )
    {
        cerr << "Facility don't finded" << endl;
        return;
    }

    FacilityPtr facility = current_facility->convert< Facility >();
    FloorPtrs floors = facility->getChilds();
    for( FloorPtr floor: floors )
    {
        GraphEntityPtr graph = floor->getGraph();
        if( !graph )
        {
            cerr << "Graph of " << floor->getId()
                 << " don't exists" << endl;
            continue;
        }

        // doing some with graph
        auto nodes = graph->getNodes();
        for( GraphNodePtr node: nodes )
        {
            cout << "Node" << endl;
            cout << node->getCoord().x() << "x"
                 << node->getCoord().y() << endl;
        }
        auto edges = graph->getEdges();
        for( GraphEdgePtr edge: edges)
        {
            cout << "Edge" << endl;
            cout << edge->getPoints().first->getCoord().x() << "x"
                 << edge->getPoints().first->getCoord().y() << "\n"
                 << edge->getPoints().second->getCoord().x() << "x"
                 << edge->getPoints().second->getCoord().y() << endl;
            cout << "Height: "
                 << edge->getMetadataValue( "height" ).toDouble()
                 << endl;
        }

        // TODO doing smth with photos
        MarkPtrs photos = floor->getMarks( Mark::MT_PHOTO_3D );
        for( MarkPtr photo: photos )
        {
            cout << photo->getCenter().x()
                 << photo->getCenter().y() << endl;
            cout << photo->getMetadataValue( "height" ).toDouble() << endl;
        }

        // TODO doing smth with placholders
        MarkPtrs places = floor->getMarks( Mark::MT_PLACEHOLDER );
        for( MarkPtr place: places )
        {
            cout << place->getCenter().x() << "x"
                 << place->getCenter().y() << endl;
            BaseAreaPtr place_area = mngr->getBaseArea( place->getParentId(),
                                                        BaseArea::AT_ROOM );
            if( !place_area )
            {
                cerr << "Placeholder's parent not a room "
                     << place->getParentId() << endl;
                continue;
            }

            cout << place_area->getMetadataValue( "field" ).toDouble() << endl;
        }
    }

    // TODO doing some with elevators
    auto elevators = facility->getGroupsOfRoom( GroupEntity::GT_ELEVATOR );
    for( GroupEntityPtr elevator: elevators )
    {
        BaseAreaPtrs elevator_areas = elevator->getElements();
        for( BaseAreaPtr elevator_area: elevator_areas )
        {
            cout << "Floor of elevator " << elevator_area->getParentId() << endl;
            for( QPointF point: elevator_area->getCoords() )
                cout << point.x() << "x" << point.y() << endl;
        }
    }

    // TODO doing smth with stairs
    auto stairses = facility->getGroupsOfRoom( GroupEntity::GT_STAIRS );
    for( GroupEntityPtr stairs: stairses )
    {
        BaseAreaPtrs stairs_areas = stairs->getElements();
        for( BaseAreaPtr stairs_area: stairs_areas )
        {
            cout << "Floor of stairs " << stairs_area->getParentId() << endl;
            for( QPointF point: stairs_area->getCoords() )
                cout << point.x() << "x" << point.y() << endl;
        }
    }
}
