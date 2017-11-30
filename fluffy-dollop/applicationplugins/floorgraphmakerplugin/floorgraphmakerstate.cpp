#include "floorgraphmakerstate.h"
#include <libembeddedwidgets/embeddedapp.h>
#include <QTimer>
#include <QCursor>
#include <QDebug>

using namespace floor_graph_maker;

FloorGraphMakerState::FloorGraphMakerState()
    : _timer(this)
{
    connect(&_timer, SIGNAL(timeout()), this, SLOT(showElementPropertyForm()));
}

FloorGraphMakerState::~FloorGraphMakerState()
{
    if(_tempEdgeElement)
        delete _tempEdgeElement;

    for(auto it = _graphElements.begin(); it != _graphElements.end(); ++it)
        delete it.value();
}

void FloorGraphMakerState::init(QGraphicsScene *scene, QGraphicsView *view, const int *zoom, const double *scale, double frameCoef, uint visualizerId)
{
    ScrollBaseState::init(scene, view, zoom, scale, frameCoef, visualizerId);
    setActiveForScene(true);
}

bool FloorGraphMakerState::wheelEvent(QWheelEvent *e, QPointF scenePos)
{
    return true;
}

bool FloorGraphMakerState::mouseMoveEvent(QMouseEvent *e, QPointF scenePos)
{
    switch(_mode)
    {
    case Mode::SetNodeOrEdgeElement : {
        if(_lastNodeElement && _tempEdgeElement)
        {
            if(e->modifiers() & Qt::ShiftModifier)
            {
                QPointF _scenePos = _lastNodeElement->pos();
                QLineF line(_scenePos, scenePos);
                double angle = ((360 - line.angle()) + 90);
                while(angle > 360)
                    angle -= 360.0;

                if(angle >= 45  && angle < 135 )                        // RIGHT
                    scenePos = QPointF(scenePos.x(), _scenePos.y());
                else if(angle >= 135  && angle < 225)                   // DOWN
                    scenePos = QPointF(_scenePos.x(), scenePos.y());
                else if(angle >= 225  && angle < 315)                   // LEFT
                    scenePos = QPointF(scenePos.x(), _scenePos.y());
                else                                                    // UP
                    scenePos = QPointF(_scenePos.x(), scenePos.y());
            }
            _tempEdgeElement->setLine(QLineF(_lastNodeElement->pos(), scenePos));
        }
        _lastMousePos = scenePos;
        _view->setCursor(Qt::ArrowCursor);
    }break;

    case Mode::SetCamera :
    case Mode::SetPlaceHolder :
        return true;

    case Mode::ScrollMode :
        return ScrollBaseState::mouseMoveEvent(e, scenePos);
    }

    return true;
}

bool FloorGraphMakerState::mousePressEvent(QMouseEvent *e, QPointF scenePos)
{
    if(e->button() == Qt::LeftButton)
        switch(_mode)
        {
        case Mode::SetNodeOrEdgeElement : {

            NodeElement * nodeElement = nullptr;
            if(_elementHovered)
                nodeElement = dynamic_cast<NodeElement*>(_elementHovered);
            if( ! nodeElement)
            {
                nodeElement = new NodeElement(++_tempElementCounter, this);
                _graphElements.insert(nodeElement->id(), nodeElement);
                nodeElement->setPos(_lastMousePos);
                _scene->addItem(nodeElement);

                EdgeElement * edgeUnderCurNode = dynamic_cast<EdgeElement*>(_elementHovered);
                if(edgeUnderCurNode)
                {
                    NodeElement * nodeElement1 = edgeUnderCurNode->getNode1();
                    nodeElement1->remove(edgeUnderCurNode->id());
                    NodeElement * nodeElement2 = edgeUnderCurNode->getNode2();
                    nodeElement2->remove(edgeUnderCurNode->id());
                    double zLevel = edgeUnderCurNode->zValue();
                    EdgeElement::EdgeType type = edgeUnderCurNode->getEdgeType();
                    double wallWidth = edgeUnderCurNode->getWallWidth();
                    _graphElements.remove(edgeUnderCurNode->id());
                    delete edgeUnderCurNode;
                    _elementHovered = nullptr;

                    EdgeElement * edgeElement1 = new EdgeElement(++_tempElementCounter, this, _scene, nodeElement1, nodeElement);
                    _graphElements.insert(edgeElement1->id(), edgeElement1);
                    edgeElement1->setZValue(zLevel);
                    edgeElement1->setEdgeType(type);
                    edgeElement1->setWallWidth(wallWidth);

                    EdgeElement * edgeElement2 = new EdgeElement(++_tempElementCounter, this, _scene, nodeElement2, nodeElement);
                    _graphElements.insert(edgeElement2->id(), edgeElement2);
                    edgeElement2->setZValue(zLevel);
                    edgeElement2->setEdgeType(type);
                    edgeElement2->setWallWidth(wallWidth);
                }
            }
            nodeElement->setZValue(++_tempElementCounter + 1000);

            if(_lastNodeElement)
            {
                foreach(EdgeElement * _edgeElement, _lastNodeElement->getEdgeElements())
                    _edgeElement->setActive(true);
                _lastNodeElement->setActive(true);

                EdgeElement * edgeElement = new EdgeElement(_tempElementCounter, this, _scene, _lastNodeElement, nodeElement);
                _graphElements.insert(edgeElement->id(), edgeElement);
                edgeElement->setZValue(_tempElementCounter + 100);
            }
            _lastNodeElement = nodeElement;

            if(_tempEdgeElement)
            {
                _tempEdgeElement->setLine(QLineF(_lastNodeElement->pos(), _lastMousePos));
                _tempEdgeElement->setZValue(_tempElementCounter + 999);
            }
            else
            {
                QPen pen(Qt::blue);
                pen.setWidth(4);
                pen.setCosmetic(true);
                _tempEdgeElement = new transform_state::LineItem(_scene, pen);
                _tempEdgeElement->setZValue(_tempElementCounter + 999);
            }

        }break;

        case Mode::SetCamera : {


        }break;

        case Mode::SetPlaceHolder : {


        }break;

//        case Mode::SetDoorElement : {
//            EdgeElement * edgeUnderCurNode = dynamic_cast<EdgeElement*>(_elementHovered);
//            if(edgeUnderCurNode)
//                edgeUnderCurNode->setEdgeType(EdgeElement::DOOR);
//            return ScrollBaseState::mousePressEvent(e, scenePos);
//        }break;

//        case Mode::SetWindowElement : {
//            EdgeElement * edgeUnderCurNode = dynamic_cast<EdgeElement*>(_elementHovered);
//            if(edgeUnderCurNode)
//                edgeUnderCurNode->setEdgeType(EdgeElement::WINDOW);
//            return ScrollBaseState::mousePressEvent(e, scenePos);
//        }break;

        case Mode::ScrollMode :
            return ScrollBaseState::mousePressEvent(e, scenePos);
        }

    return true;
}

bool FloorGraphMakerState::mouseReleaseEvent(QMouseEvent *e, QPointF scenePos)
{
    switch(_mode)
    {
    case Mode::SetCamera :
    case Mode::SetPlaceHolder :
        break;

    case Mode::ScrollMode :
        return ScrollBaseState::mouseReleaseEvent(e, scenePos);
    }

    return true;
}

bool FloorGraphMakerState::mouseDoubleClickEvent(QMouseEvent *e, QPointF scenePos)
{
    switch(_mode)
    {
    case Mode::SetNodeOrEdgeElement : {
        if(_lastNodeElement)
        {
            foreach(EdgeElement * _edgeElement, _lastNodeElement->getEdgeElements())
                _edgeElement->setActive(true);
            _lastNodeElement->setActive(true);
            _lastNodeElement = nullptr;
        }
        if(_tempEdgeElement)
        {
            delete _tempEdgeElement;
            _tempEdgeElement = nullptr;
        }
    }return false;

    case Mode::ScrollMode : {
        showElementPropertyForm();
    }return false;
    }

    return true;
}

bool FloorGraphMakerState::keyPressEvent(QKeyEvent *e)
{
    switch(_mode)
    {
    case Mode::SetNodeOrEdgeElement : {
     if(e->key() == Qt::Key_Escape)
     {
         if(_lastNodeElement)
         {
             foreach(EdgeElement * _edgeElement, _lastNodeElement->getEdgeElements())
                 _edgeElement->setActive(true);
             _lastNodeElement->setActive(true);
             _lastNodeElement = nullptr;
         }
         if(_tempEdgeElement)
         {
             delete _tempEdgeElement;
             _tempEdgeElement = nullptr;
         }
     }

    }return false;
    }

    return true;
}

QString FloorGraphMakerState::stateName()
{
    return "FloorGraphMakerState";
}

void FloorGraphMakerState::statePushedToStack()
{
}

void FloorGraphMakerState::statePoppedFromStack()
{
    setActiveForScene(true);
}

void FloorGraphMakerState::elementHovered(FloorGraphElement * element)
{
    _elementHovered = element;
    _timer.stop();

    if(_mode == Mode::ScrollMode) // || _mode == Mode::SetNodeOrEdgeElement)
    {
        if(_elementHovered)
            _timer.start(1000);
    }
}

void FloorGraphMakerState::elementDoubleClicked(FloorGraphElement *element)
{
}

void FloorGraphMakerState::slotRemoveElement(uint id)
{
    if(_elementPropertyForm)
        _elementPropertyForm->deleteLater();
    _elementPropertyForm = nullptr;

    auto it = _graphElements.find(id);
    if(it != _graphElements.end())
    {
        NodeElement * nodeElement = dynamic_cast<NodeElement*>(it.value());
        _graphElements.erase(it);
        if(nodeElement)
        {
//            if(nodeElement->getEdgeElements().size() == 2)
//            {
//                EdgeElement * edgeElement1 = nodeElement->getEdgeElements().first();
//                EdgeElement::EdgeType type = edgeElement1->getEdgeType();
//                double zLevel = edgeElement1->zValue();
//                double wallWidth = edgeElement1->getWallWidth();
//                edgeElement1->getNode1()->remove(edgeElement1->id());
//                edgeElement1->getNode2()->remove(edgeElement1->id());
//                NodeElement * node1 = ( edgeElement1->getNode1() == nodeElement ? edgeElement1->getNode2() : edgeElement1->getNode1() );
//                _graphElements.remove(edgeElement1->id());
//                delete edgeElement1;

//                EdgeElement * edgeElement2 = nodeElement->getEdgeElements().last();
//                edgeElement2->getNode1()->remove(edgeElement2->id());
//                edgeElement2->getNode2()->remove(edgeElement2->id());
//                NodeElement * node2 = ( edgeElement2->getNode1() == nodeElement ? edgeElement2->getNode2() : edgeElement2->getNode1() );
//                _graphElements.remove(edgeElement2->id());
//                delete edgeElement2;

//                EdgeElement * newEdgeElement = new EdgeElement(++_tempElementCounter, this, _scene, node1, node2);
//                _graphElements.insert(newEdgeElement->id(), newEdgeElement);
//                newEdgeElement->setZValue(zLevel);
//                newEdgeElement->setEdgeType(type);
//                newEdgeElement->setWallWidth(wallWidth);

//            }
//            else
                foreach(EdgeElement * edgeElement, nodeElement->getEdgeElements())
                {
                    edgeElement->getNode1()->remove(edgeElement->id());
                    edgeElement->getNode2()->remove(edgeElement->id());
                    _graphElements.remove(edgeElement->id());
                    delete edgeElement;
                }
        }
        EdgeElement * edgeElement = dynamic_cast<EdgeElement*>(it.value());
        if(edgeElement)
        {
            edgeElement->getNode1()->remove(edgeElement->id());
            edgeElement->getNode2()->remove(edgeElement->id());
        }
        delete it.value();
    }
}

void FloorGraphMakerState::slotEdgeTypeChanged(uint id, floor_graph_maker::EdgeElement::EdgeType type)
{
    auto it = _graphElements.find(id);
    if(it != _graphElements.end())
    {
        EdgeElement * edgeElement = dynamic_cast<EdgeElement*>(it.value());
        if(edgeElement)
            edgeElement->setEdgeType(type);
    }
}

void FloorGraphMakerState::slotWallWidthChanged(uint edgeId, double val)
{
    auto it = _graphElements.find(edgeId);
    if(it != _graphElements.end())
    {
        EdgeElement * edgeElement = dynamic_cast<EdgeElement*>(it.value());
        if(edgeElement)
            edgeElement->setWallWidth(val);
    }
}

void FloorGraphMakerState::setMode(floor_graph_maker::Mode mode)
{
    switch(_mode)
    {
    case Mode::ScrollMode : {
        for(auto it = _graphElements.begin(); it != _graphElements.end(); ++it)
        {
            NodeElement * element = dynamic_cast<NodeElement*>(it.value());
            if(element)
            {
                element->setFlag(QGraphicsItem::ItemIsMovable, false);
                element->setCursor(QCursor(Qt::ArrowCursor));
            }
        }
    }break;
    case Mode::SetNodeOrEdgeElement : {
        if(_lastNodeElement)
        {
            foreach(EdgeElement * _edgeElement, _lastNodeElement->getEdgeElements())
                _edgeElement->setActive(true);
            _lastNodeElement->setActive(true);
            _lastNodeElement = nullptr;
        }
        if(_tempEdgeElement)
        {
            delete _tempEdgeElement;
            _tempEdgeElement = nullptr;
        }
    }
    }

    _mode = mode;

    switch(_mode)
    {
    case Mode::ScrollMode : {
        for(auto it = _graphElements.begin(); it != _graphElements.end(); ++it)
        {
            NodeElement * element = dynamic_cast<NodeElement*>(it.value());
            if(element)
            {
                element->setFlag(QGraphicsItem::ItemIsMovable, true);
                element->setCursor(QCursor(Qt::SizeAllCursor));
            }
        }
    }break;
    case Mode::SetNodeOrEdgeElement : {
        _view->setCursor(Qt::ArrowCursor);
    }break;
    }
}

void FloorGraphMakerState::save()
{

}

void FloorGraphMakerState::showElementPropertyForm()
{
//    _timer.stop();
    if(_elementHovered)
    {
        if(_elementPropertyForm)
            delete _elementPropertyForm;

        _elementPropertyForm = new ElementPropertyForm(_elementHovered);
        _elementPropertyForm->setStyleSheet(ewApp()->getMainStylesheet());
        connect(_elementPropertyForm, SIGNAL(signalRemoveElement(uint)), this, SLOT(slotRemoveElement(uint)));
        connect(_elementPropertyForm, SIGNAL(signalEdgeTypeChanged(uint,floor_graph_maker::EdgeElement::EdgeType)),
                this, SLOT(slotEdgeTypeChanged(uint,floor_graph_maker::EdgeElement::EdgeType)));
        connect(_elementPropertyForm, SIGNAL(signalWallWidthChanged(uint,double)), this, SLOT(slotWallWidthChanged(uint,double)));

        _elementPropertyForm->move(QCursor::pos());
        _elementPropertyForm->show();
    }
}













