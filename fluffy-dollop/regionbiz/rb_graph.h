#ifndef RB_GRAPH_H
#define RB_GRAPH_H

#include <set>
#include <QPoint>
#include <QPolygon>

#include "rb_base_entity.h"

namespace regionbiz {

// cam attrs
/**
 * attrs of cam:
 * height (double)
 */

//-------------------------

class GraphEntity;
typedef std::shared_ptr< GraphEntity > GraphEntityPtr;

class GraphEdge;
typedef std::shared_ptr< GraphEdge > GraphEdgePtr;
typedef std::vector< GraphEdgePtr > GraphEdgePtrs;

/**
 * @brief The GraphNode class
 * no attrs
 */
class GraphNode: public BaseEntity
{
    friend class GraphEntity;
    friend class BaseEntity;
public:
    BaseEntity::EntityType getEntityType() override;
    bool commit() override;

    GraphEntityPtr getParentGraph();
    uint64_t getParentId();
    GraphEdgePtrs getEdges();

    QPointF getCoord();
    QPolygonF getCoords();
    void setCoord( QPointF coord );

private:
    GraphNode( uint64_t id );
    void setParentGraph( GraphEntityPtr parent_graph );

    GraphEntityPtr _parent_graph;
    QPointF _coord;
};
typedef std::shared_ptr< GraphNode > GraphNodePtr;
typedef std::vector< GraphNodePtr > GraphNodePtrs;

//----------------------

typedef std::pair< GraphNodePtr, GraphNodePtr > TwoGraphNodePtr;

/**
 * @brief GraphEdge
 * attrs:
 * type (enum): wall, door, window
 * height (double)
 * state (enum): locked, unlocked
 */
class GraphEdge: public BaseEntity
{
    friend class GraphEntity;
    friend class BaseEntity;
public:
    BaseEntity::EntityType getEntityType() override;
    bool commit() override;

    GraphEntityPtr getParentGraph();
    uint64_t getParentId();

    GraphNodePtr getFirstPoint();
    GraphNodePtr getSecondPoint();
    TwoGraphNodePtr getPoints();

    void setFirstPoint( GraphNodePtr point );
    void setSecondPoint( GraphNodePtr point );

    QString getType();

private:
    GraphEdge( uint64_t id );
    void setParentGraph( GraphEntityPtr parent_graph );

    GraphEntityPtr _parent_graph;
    TwoGraphNodePtr _nodes;
};

//----------------------

typedef std::set< GraphEntityPtr > GraphEntityPtrSet;

/**
 * @brief The GraphEntity class
 * no attrs
 */
class GraphEntity: public BaseEntity
{
    friend class BaseEntity;
    friend class RegionBizManager;
    friend class BaseDataTranslator;
public:
    BaseEntity::EntityType getEntityType() override;
    bool commit() override;

    uint64_t getParentId();

    // nodes
    GraphNodePtrs getNodes();
    GraphNodePtrs getNodesByEdge(GraphEdgePtr edge);
    GraphNodePtr addNode();
    GraphNodePtr addNode( QPointF coord );
    void removeNode( GraphNodePtr node );

    // edges
    GraphEdgePtrs getEdges();
    GraphEdgePtrs getEdgesByNode( GraphNodePtr node );
    GraphEdgePtr addEdge( GraphNodePtr first_node,
                          GraphNodePtr second_node );
    void removeEdge( GraphEdgePtr edge );

private:
    GraphEntity( uint64_t id );
    void setParentId( uint64_t parent_id );
    void appendNode( GraphNodePtr node );
    void appendEdge( GraphEdgePtr edge );

    uint64_t _parent_id;

    GraphNodePtrs _nodes;
    GraphEdgePtrs _edges;
};

typedef std::vector< GraphEntityPtr > GraphEntityPtrs;

}

#endif // RB_GRAPH_H
