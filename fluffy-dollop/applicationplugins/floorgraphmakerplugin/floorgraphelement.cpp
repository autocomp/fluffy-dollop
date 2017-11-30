#include "floorgraphelement.h"
#include "floorgraphmakerstate.h"
#include <QCursor>
#include <QDebug>

using namespace floor_graph_maker;

FloorGraphElement::FloorGraphElement(uint id, ElementHolder *elementHolder)
    : _id(id)
    , _elementHolder(elementHolder)
{
}

FloorGraphElement::~FloorGraphElement()
{
}

uint FloorGraphElement::id()
{
    return _id;
}

//-------------------------------------------------------------------------------------

NodeElement::NodeElement(uint id, ElementHolder * elementHolder)
    : FloorGraphElement(id, elementHolder)
{
    setPixmap(QPixmap("://img/handle_rotater.png"));
    setOffset(-pixmap().width()/2.,-pixmap().height()/2.);
    setFlags(QGraphicsItem::ItemIgnoresTransformations | QGraphicsItem::ItemIgnoresParentOpacity | QGraphicsItem::ItemSendsGeometryChanges);
}

NodeElement::~NodeElement()
{
}

void NodeElement::addEdge(EdgeElement *edge)
{
    _edges.insert(edge->id(), edge);
}

void NodeElement::remove(uint edgeId)
{
    _edges.remove(edgeId);
//    auto it = _edges.find(edgeId);
//    if(it != _edges.end())
//    {
//        delete it.value();
//        _edges.erase(it);
//    }
}

void NodeElement::setActive(bool on_off)
{
    if(on_off)
    {
        setAcceptHoverEvents(true);
        //setCursor(QCursor(Qt::CrossCursor));
    }
    else
    {
        setPixmap(QPixmap("://img/handle_rotater.png"));
        setAcceptHoverEvents(false);
    }
}

QList<EdgeElement *> NodeElement::getEdgeElements()
{
    return _edges.values();
}

void NodeElement::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    _itemMoved = true;
    QGraphicsPixmapItem::mousePressEvent(event);
}

void NodeElement::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    _itemMoved = false;
    QGraphicsPixmapItem::mouseReleaseEvent(event);
}

void NodeElement::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    _elementHolder->elementDoubleClicked(this);
}

QVariant NodeElement::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemPositionChange && _itemMoved)
    {
        QPointF newPos = value.toPointF();
        for(auto it = _edges.begin(); it != _edges.end(); ++it)
        {
            EdgeElement * edgeElement = it.value();
            if(this == edgeElement->getNode1())
                edgeElement->setLine(QLineF(edgeElement->getNode2()->pos(), newPos));
            else if(this == edgeElement->getNode2())
                edgeElement->setLine(QLineF(edgeElement->getNode1()->pos(), newPos));
        }
        return QGraphicsItem::itemChange(change, value);
    }
    else
       return QGraphicsItem::itemChange(change, value);

}

void NodeElement::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setPixmap(QPixmap("://img/handle_rotater_selected.png"));
    _elementHolder->elementHovered(this);
    QGraphicsPixmapItem::hoverEnterEvent(event);
}

void NodeElement::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    _elementHolder->elementHovered();
    setPixmap(QPixmap("://img/handle_rotater.png"));
    QGraphicsPixmapItem::hoverLeaveEvent(event);
}

//-------------------------------------------------------------------------------------

EdgeElement::EdgeElement(uint id, ElementHolder * elementHolder, QGraphicsScene *scene, NodeElement *node1, NodeElement *node2)
    : FloorGraphElement(id, elementHolder)
    , transform_state::LineItem(scene, QPen())
    , _node1(node1)
    , _node2(node2)
{
    _node1->addEdge(this);
    _node2->addEdge(this);

    setLine(QLineF(_node1->pos(), _node2->pos()));

    setEdgeProperty(_edgeProperty);
}

EdgeElement::~EdgeElement()
{

}

void EdgeElement::setActive(bool on_off)
{
    if(on_off)
    {
        setAcceptHoverEvents(true);
        //setCursor(QCursor(Qt::CrossCursor));
    }
    else
    {
        setPen(_pen);
        setAcceptHoverEvents(false);
    }
}

NodeElement *EdgeElement::getNode1()
{
    return _node1;
}

NodeElement *EdgeElement::getNode2()
{
    return _node2;
}

void EdgeElement::mousePressEvent(QGraphicsSceneMouseEvent *event)
{

    QGraphicsLineItem::mousePressEvent(event);
}

void EdgeElement::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{

    QGraphicsLineItem::mouseReleaseEvent(event);
}

void EdgeElement::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    _elementHolder->elementDoubleClicked(this);
}

void EdgeElement::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    _elementHolder->elementHovered(this);
    setPen(_hoverPen);
    QGraphicsLineItem::hoverEnterEvent(event);
}

void EdgeElement::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    _elementHolder->elementHovered();
    setPen(_pen);
    QGraphicsLineItem::hoverLeaveEvent(event);
}

EdgeProperty EdgeElement::getEdgeProperty() const
{
    return _edgeProperty;
}

void EdgeElement::setEdgeProperty(const EdgeProperty &edgeProperty)
{
    _edgeProperty = edgeProperty;

    QPen pen;
    pen.setWidth(4);
    pen.setCosmetic(true);
    switch(_edgeProperty.type)
    {
    case EdgeType::Wall :
        pen.setColor(Qt::blue);
        break;
    case EdgeType::Door :
        pen.setColor(Qt::green);
        break;
    case EdgeType::Window :
        pen.setColor(Qt::yellow);
        break;
    }
    _pen = pen;
    setPen(_pen);

    _hoverPen = _pen;
    _hoverPen.setColor(Qt::red);
}













