#ifndef FLOORGRAPHELEMENT_H
#define FLOORGRAPHELEMENT_H

#include <QGraphicsPixmapItem>
#include <ctrwidgets/components/layersmanager/lineitem.h>
#include <QMap>
#include <QPen>

namespace floor_graph_maker
{

class ElementHolder;

class FloorGraphElement
{
public:
    FloorGraphElement(uint id, ElementHolder * elementHolder);
    virtual ~FloorGraphElement();
    uint id();
    virtual void setActive(bool on_off) = 0;

protected:
    const uint _id;
    ElementHolder * _elementHolder;
};

class EdgeElement;

class NodeElement : public FloorGraphElement, public QGraphicsPixmapItem
{
public:
    NodeElement(uint id, ElementHolder * elementHolder);
    ~NodeElement();
    void addEdge(EdgeElement * edge);
    void remove(uint edgeId);
    virtual void setActive(bool on_off);
    QList<EdgeElement *> getEdgeElements();

    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    QMap<uint, EdgeElement *> _edges;
    bool _itemMoved = false;
};

class EdgeElement : public FloorGraphElement, public transform_state::LineItem
{
public:
    enum  EdgeType {WALL, DOOR, WINDOW};

    EdgeElement(uint id, ElementHolder * elementHolder, QGraphicsScene * scene, NodeElement * node1, NodeElement * node2);
    ~EdgeElement();
    virtual void setActive(bool on_off);
    NodeElement * getNode1();
    NodeElement * getNode2();
    void setEdgeType(EdgeType type);
    EdgeType getEdgeType();
    double getWallWidth() const;
    void setWallWidth(double wallWidth);

    void mousePressEvent(QGraphicsSceneMouseEvent * event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent * event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    NodeElement * _node1, * _node2;
    QPen _pen, _hoverPen;
    EdgeType _edgeType = WALL;
    double _wallWidth = 0.1;
};


}

#endif // FLOORGRAPHELEMENT_H
