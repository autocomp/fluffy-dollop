#include "rb_graph.h"

#include "rb_manager.h"

using namespace regionbiz;

QPointF GraphNode::getCoord()
{
    return _coord;
}

QPolygonF GraphNode::getCoords()
{
    QPolygonF poly;
    poly.push_back( getCoord() );
    return poly;
}

void GraphNode::setCoord(QPointF coord)
{
    _coord = coord;
}

BaseEntity::EntityType GraphNode::getEntityType()
{
    return ET_GRAPH_NODE;
}

bool GraphNode::commit()
{
    return _parent_graph->commit();
}

GraphEntityPtr GraphNode::getParentGraph()
{
    return _parent_graph;
}

uint64_t GraphNode::getParentId()
{
    return _parent_graph->getId();
}

GraphEdgePtrs GraphNode::getEdges()
{
    auto itself = BaseEntity::convert< GraphNode >( getItself() );
    return _parent_graph->getEdgesByNode( itself );
}

void GraphNode::setParentGraph(GraphEntityPtr parent_graph)
{
    _parent_graph = parent_graph;
}

GraphNode::GraphNode(uint64_t id):
    BaseEntity( id )
{}

BaseEntity::EntityType GraphEdge::getEntityType()
{
    return ET_GRAPH_EDGE;
}

bool GraphEdge::commit()
{
    return _parent_graph->commit();
}

GraphEntityPtr GraphEdge::getParentGraph()
{
    return _parent_graph;
}

uint64_t GraphEdge::getParentId()
{
    return _parent_graph->getId();
}

GraphNodePtr GraphEdge::getFirstPoint()
{
    return _nodes.first;
}

GraphNodePtr GraphEdge::getSecondPoint()
{
    return _nodes.second;
}

TwoGraphNodePtr GraphEdge::getPoints()
{
    return _nodes;
}

void GraphEdge::setFirstPoint(GraphNodePtr point)
{
    _nodes.first = point;
}

void GraphEdge::setSecondPoint(GraphNodePtr point)
{
    _nodes.second = point;
}

QString GraphEdge::getType()
{
    if( !isMetadataPresent( "type" ))
        return "";

    QString type = getMetadataValue( "type" ).toString();
    return type;
}

bool GraphEdge::setType(QString type)
{
    bool set = addMetadata( "enum", "type", type );
    return set;
}

GraphEdge::GraphEdge(uint64_t id):
    BaseEntity( id )
{}

void GraphEdge::setParentGraph(GraphEntityPtr parent_graph)
{
    _parent_graph = parent_graph;
}

BaseEntity::EntityType GraphEntity::getEntityType()
{
    return ET_GRAPH;
}

bool GraphEntity::commit()
{
    // commit graph
    auto mngr = RegionBizManager::instance();
    return mngr->commitGraph( getId() );
}

uint64_t GraphEntity::getParentId()
{
    return _parent_id;
}

GraphNodePtrs GraphEntity::getNodes()
{
    return _nodes;
}

GraphNodePtrs GraphEntity::getNodesByEdge( GraphEdgePtr edge )
{
    GraphNodePtrs res;
    res.push_back( edge->getFirstPoint() );
    res.push_back( edge->getSecondPoint() );
    return res;
}

GraphNodePtr GraphEntity::addNode()
{
    return addNode( QPointF() );
}

GraphNodePtr GraphEntity::addNode( QPointF coord )
{
    GraphNodePtr node = BaseEntity::createWithId< GraphNode >(
                getMaxId() + 1, getId() );
    if( node )
    {
        node->setCoord( coord );
        node->setParentGraph( getItself()->convert< GraphEntity >() );
        appendNode( node );
    }
    return node;
}

void GraphEntity::removeNode( GraphNodePtr node )
{
    for( auto it = _edges.begin(); it != _edges.end(); )
    {
        GraphNodePtr first = (*it)->getFirstPoint();
        GraphNodePtr second = (*it)->getSecondPoint();
        if( first->getId() == node->getId()
                || second->getId() == node->getId() )
        {
            // remove edge
            BaseEntity::deleteEntity( (*it), (*it)->getParentId() );
            _edges.erase( it );
        }
        else
            ++it;
    }

    for( auto it = _nodes.begin(); it != _nodes.end(); ++it )
    {
        if( (*it)->getId() == node->getId() )
        {
            // delete node graph from system
            BaseEntity::deleteEntity( (*it), (*it)->getParentId() );
            _nodes.erase( it );
            break;
        }
    }
}

GraphEdgePtrs GraphEntity::getEdges()
{
    return _edges;
}

GraphEdgePtrs GraphEntity::getEdgesByNode(GraphNodePtr node)
{
    GraphEdgePtrs res;
    for( auto it = _edges.begin(); it != _edges.end(); ++it )
    {
        GraphNodePtr first = (*it)->getFirstPoint();
        GraphNodePtr second = (*it)->getSecondPoint();
        if( first->getId() == node->getId()
                || second->getId() == node->getId() )
        {
            res.push_back( *it );
        }
    }

    return res;
}

GraphEdgePtr GraphEntity::addEdge( GraphNodePtr first_node,
                                   GraphNodePtr second_node )
{
    GraphEdgePtr edge = BaseEntity::createWithId< GraphEdge >(
                getMaxId() + 1, getId() );
    if( edge )
    {
        edge->setFirstPoint( first_node );
        edge->setSecondPoint( second_node );
        edge->setParentGraph( getItself()->convert< GraphEntity >() );
        appendEdge( edge );
    }
    return edge;
}

void GraphEntity::removeEdge(GraphEdgePtr edge)
{
    if( !edge )
        return;

    for( auto it = _edges.begin(); it != _edges.end(); ++it )
        if( (*it)->getId() == edge->getId() )
        {
            // delete from system
            BaseEntity::deleteEntity( (*it), (*it)->getParentId() );
            _edges.erase( it );
            return;
        }
}

void GraphEntity::appendNode(GraphNodePtr node)
{
    if( node )
    {
        node->setParentGraph( getItself()->convert< GraphEntity >() );
        _nodes.push_back( node );
    }
}

void GraphEntity::appendEdge(GraphEdgePtr edge)
{
    if( edge )
    {
        edge->setParentGraph( getItself()->convert< GraphEntity >() );
        _edges.push_back( edge );
    }
}

GraphEntity::GraphEntity(uint64_t id):
    BaseEntity( id )
{}

void GraphEntity::setParentId(uint64_t parent_id)
{
    _parent_id = parent_id;
}
