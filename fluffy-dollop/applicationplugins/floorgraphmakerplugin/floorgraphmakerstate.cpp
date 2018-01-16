#include "floorgraphmakerstate.h"
#include "instrumentalform.h"
#include <libembeddedwidgets/embeddedapp.h>
#include <regionbiz/rb_manager.h>
#include <regionbiz/rb_graph.h>
#include <regionbiz/rb_locations.h>
#include <QTimer>
#include <QCursor>
#include <QDebug>

using namespace floor_graph_maker;
using namespace regionbiz;

FloorGraphMakerState::FloorGraphMakerState(uint64_t floorId)
    : _floorId(floorId)
//    : _timer(this)
{
//    connect(&_timer, SIGNAL(timeout()), this, SLOT(showElementPropertyForm()));

    _pen.setColor(Qt::blue);
    _pen.setWidth(4);
    _pen.setCosmetic(true);
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


    BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(_floorId);
    if(! ptr)
        return;

    GraphEntityPtr graphPtr = ptr->getGraph();
    if( ! graphPtr)
        return;

    QMap<uint64_t, NodeElement*> nodeElements;
    foreach(GraphNodePtr graphNodePtr, graphPtr->getNodes())
    {
        NodeElement * nodeElement = new NodeElement(++_tempElementCounter, this);
        nodeElement->setPos(graphNodePtr->getCoord());
        nodeElement->setZValue(1000);
        nodeElement->setActive(true);
        nodeElement->setFlag(QGraphicsItem::ItemIsMovable, true);
        nodeElement->setCursor(QCursor(Qt::SizeAllCursor));
        _scene->addItem(nodeElement);
        _graphElements.insert(nodeElement->id(), nodeElement);

        nodeElements.insert(graphNodePtr->getId(), nodeElement);
    }
    foreach(GraphEdgePtr graphEdgePtr, graphPtr->getEdges())
    {
        GraphNodePtr graphNodePtr1 = graphEdgePtr->getFirstPoint();
        NodeElement * nodeElement1 = nullptr;
        auto it1 = nodeElements.find(graphNodePtr1->getId());
        if(it1 != nodeElements.end())
            nodeElement1 = it1.value();

        GraphNodePtr graphNodePtr2 = graphEdgePtr->getSecondPoint();
        NodeElement * nodeElement2 = nullptr;
        auto it2 = nodeElements.find(graphNodePtr2->getId());
        if(it2 != nodeElements.end())
            nodeElement2 = it2.value();

        if(nodeElement1 && nodeElement2)
        {
            EdgeElement * edgeElement = new EdgeElement(++_tempElementCounter, this, _scene, nodeElement1, nodeElement2);
            edgeElement->setZValue(500);
            edgeElement->setActive(true);
            edgeElement->setFlag(QGraphicsItem::ItemIsMovable, false);
            edgeElement->setCursor(QCursor(Qt::SizeAllCursor));

            EdgeProperty edgeDefaultProperty = _edgeDefaultProperty;
            if(graphEdgePtr->getType() == QString("door"))
            {
                edgeDefaultProperty.type = EdgeType::Door;

                BaseMetadataPtr doorState = graphEdgePtr->getMetadata("doorState");
                if(doorState)
                {
                    QString doorStateStr = doorState->getValueAsVariant().toString();
                    if(doorStateStr == QString("locked"))
                        edgeDefaultProperty.state = DoorState::Close;
                }
            }
            else if(graphEdgePtr->getType() == QString("window"))
            {
                edgeDefaultProperty.type = EdgeType::Window;

                BaseMetadataPtr windowHeight = graphEdgePtr->getMetadata("windowHeight");
                if(windowHeight)
                     edgeDefaultProperty.windowHeight = windowHeight->getValueAsVariant().toDouble();

                BaseMetadataPtr windowHeightUnderFloor = graphEdgePtr->getMetadata("windowHeightUnderFloor");
                if(windowHeightUnderFloor)
                     edgeDefaultProperty.windowHeightUnderFloor = windowHeightUnderFloor->getValueAsVariant().toDouble();
            }
            else // WALL
            {
                BaseMetadataPtr wallHeight = graphEdgePtr->getMetadata("wallHeight");
                if(wallHeight)
                     edgeDefaultProperty.wallHeight = wallHeight->getValueAsVariant().toDouble();

                BaseMetadataPtr wallWidth = graphEdgePtr->getMetadata("wallWidth");
                if(wallWidth)
                     edgeDefaultProperty.wallWidth = wallWidth->getValueAsVariant().toDouble();
            }
            edgeElement->setEdgeProperty(edgeDefaultProperty);
            _graphElements.insert(edgeElement->id(), edgeElement);
        }
    }
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

    case Mode::ScrollMode :
        return ScrollBaseState::mouseMoveEvent(e, scenePos);
    }

    return true;
}

bool FloorGraphMakerState::mousePressEvent(QMouseEvent *e, QPointF scenePos)
{
    if(e->button() == Qt::LeftButton)
    {
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
                    EdgeProperty edgeProperty = edgeUnderCurNode->getEdgeProperty();
                    _graphElements.remove(edgeUnderCurNode->id());
                    delete edgeUnderCurNode;
                    _elementHovered = nullptr;

                    EdgeElement * edgeElement1 = new EdgeElement(++_tempElementCounter, this, _scene, nodeElement1, nodeElement);
                    _graphElements.insert(edgeElement1->id(), edgeElement1);
                    edgeElement1->setZValue(zLevel);
                    edgeElement1->setEdgeProperty(edgeProperty);

                    EdgeElement * edgeElement2 = new EdgeElement(++_tempElementCounter, this, _scene, nodeElement2, nodeElement);
                    _graphElements.insert(edgeElement2->id(), edgeElement2);
                    edgeElement2->setZValue(zLevel);
                    edgeElement2->setEdgeProperty(edgeProperty);
                }
            }
            nodeElement->setZValue(++_tempElementCounter + 1000);

            if(_lastNodeElement)
            {
                foreach(EdgeElement * _edgeElement, _lastNodeElement->getEdgeElements())
                    _edgeElement->setActive(true);
                _lastNodeElement->setActive(true);

                EdgeElement * edgeElement = new EdgeElement(_tempElementCounter, this, _scene, _lastNodeElement, nodeElement);
                edgeElement->setZValue(_tempElementCounter + 100);
                edgeElement->setEdgeProperty(_edgeDefaultProperty);
                _graphElements.insert(edgeElement->id(), edgeElement);
            }
            _lastNodeElement = nodeElement;

            if(_tempEdgeElement)
            {
                _tempEdgeElement->setLine(QLineF(_lastNodeElement->pos(), _lastMousePos));
                _tempEdgeElement->setZValue(_tempElementCounter + 999);
            }
            else
            {
                _tempEdgeElement = new transform_state::LineItem(_scene, _pen);
                _tempEdgeElement->setZValue(_tempElementCounter + 999);
            }

        }break;

        case Mode::ScrollMode :
            return ScrollBaseState::mousePressEvent(e, scenePos);
        }
    }
    else if(e->button() == Qt::RightButton)
    {
        switch(_mode)
        {
        case Mode::ScrollMode : {
            showElementPropertyForm();
        }break;
        }
    }

    return true;
}

bool FloorGraphMakerState::mouseReleaseEvent(QMouseEvent *e, QPointF scenePos)
{
    switch(_mode)
    {
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

//    case Mode::ScrollMode : {
//        showElementPropertyForm();
//    }return false;
    }

    return true;
}

bool FloorGraphMakerState::keyPressEvent(QKeyEvent *e)
{
    switch(_mode)
    {
    case Mode::SetNodeOrEdgeElement : {
        switch(e->key())
        {
        case Qt::Key_1 :
            //if(e->modifiers() & Qt::ControlModifier)
                emit signalPressCtr_1();
            break;

        case Qt::Key_2 :
            //if(e->modifiers() & Qt::ControlModifier)
                emit signalPressCtr_2();
            break;

        case Qt::Key_3 :
            //if(e->modifiers() & Qt::ControlModifier)
                emit signalPressCtr_3();
            break;

        case Qt::Key_Escape :
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
            break;
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
//    _timer.stop();

//    if(_mode == Mode::ScrollMode) // || _mode == Mode::SetNodeOrEdgeElement)
//    {
//        if(_elementHovered)
//            _timer.start(1000);
//    }
}

void FloorGraphMakerState::elementDoubleClicked(FloorGraphElement *element)
{
}

void FloorGraphMakerState::slotRemoveElement(uint id)
{
    auto it = _graphElements.find(id);
    if(it != _graphElements.end())
    {
        NodeElement * nodeElement = dynamic_cast<NodeElement*>(it.value());
        EdgeElement * edgeElement = dynamic_cast<EdgeElement*>(it.value());
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

        if(edgeElement)
        {
            edgeElement->getNode1()->remove(edgeElement->id());
            edgeElement->getNode2()->remove(edgeElement->id());
        }
        delete it.value();
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
    BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(_floorId);
    if(! ptr)
        return;

    if(ptr->hasGraph())
        ptr->deleteGraph();

    GraphEntityPtr graphPtr = RegionBizManager::instance()->addGraph(ptr);

    QMap<uint, GraphNodePtr> nodeElements;
    for(auto it = _graphElements.begin(); it != _graphElements.end(); ++it)
    {
        NodeElement * node = dynamic_cast<NodeElement*>(it.value());
        if(node)
        {
            GraphNodePtr graphNodePtr = graphPtr->addNode(node->pos());
            nodeElements.insert(node->id(), graphNodePtr);
        }
    }
    for(auto it = _graphElements.begin(); it != _graphElements.end(); ++it)
    {
        EdgeElement * edge = dynamic_cast<EdgeElement*>(it.value());
        if(edge)
        {
            GraphNodePtr graphNodePtr1;
            auto it1 = nodeElements.find(edge->getNode1()->id());
            if(it1 != nodeElements.end())
                graphNodePtr1 = it1.value();

            GraphNodePtr graphNodePtr2;
            auto it2 = nodeElements.find(edge->getNode2()->id());
            if(it2 != nodeElements.end())
                graphNodePtr2 = it2.value();

            if(graphNodePtr1 && graphNodePtr2)
            {
                GraphEdgePtr graphEdgePtr = graphPtr->addEdge(graphNodePtr1, graphNodePtr2);
                switch(edge->getEdgeProperty().type)
                {
                case EdgeType::Wall :
                    graphEdgePtr->setType("wall");
                    graphEdgePtr->addMetadata("double", "wallHeight", edge->getEdgeProperty().wallHeight);
                    graphEdgePtr->addMetadata("double", "wallWidth", edge->getEdgeProperty().wallWidth);
                    break;
                case EdgeType::Door :
                    graphEdgePtr->setType("door");
                    if(edge->getEdgeProperty().state == DoorState::Open)
                        graphEdgePtr->addMetadata("enum", "doorState", "unlocked");
                    else
                        graphEdgePtr->addMetadata("enum", "doorState", "locked");
                    break;
                case EdgeType::Window :
                    graphEdgePtr->setType("window");
                    graphEdgePtr->addMetadata("double", "windowHeight", edge->getEdgeProperty().windowHeight);
                    graphEdgePtr->addMetadata("double", "windowHeightUnderFloor", edge->getEdgeProperty().windowHeightUnderFloor);
                    break;
                }
            }
        }
    }

    graphPtr->commit();
    ptr->commitGraph();
    ptr->commit();
}

void FloorGraphMakerState::edgeStateChanged(EdgeProperty property)
{
    _edgeDefaultProperty = property;
    switch (_edgeDefaultProperty.type)
    {
    case EdgeType::Wall:
        _pen.setColor(Qt::blue);
        break;
    case EdgeType::Door:
        _pen.setColor(Qt::green);
        break;
    case EdgeType::Window:
        _pen.setColor(Qt::yellow);
        break;
    }
    if(_tempEdgeElement)
        _tempEdgeElement->setPen(_pen);
}

void FloorGraphMakerState::slotSetEdgeProperty(uint edgeId, EdgeProperty property)
{
    auto it = _graphElements.find(edgeId);
    if(it != _graphElements.end())
    {
        EdgeElement * edgeElement = dynamic_cast<EdgeElement*>(it.value());
        if(edgeElement)
            edgeElement->setEdgeProperty(property);
    }
}

void FloorGraphMakerState::showElementPropertyForm()
{
//    _timer.stop();
    if(_elementHovered)
    {
        QString title = QString::fromUtf8("вершина");
        ElementType type = ElementType::Node;
        EdgeElement * edge = dynamic_cast<EdgeElement*>(_elementHovered);
        if(edge)
        {
            title = QString::fromUtf8("грань");
            type = ElementType::Edge;
        }

        _elementPropertyForm = new floor_graph_maker::InstrumentalForm(_elementHovered->id(), type);
        if(edge)
            _elementPropertyForm->setCurrentProperty(edge->getEdgeProperty());
        connect(_elementPropertyForm, SIGNAL(signalClosed()), this, SLOT(slotElementFormClose()));
        connect(_elementPropertyForm, SIGNAL(signalRemoveElement(uint)), this, SLOT(slotRemoveElementFromForm(uint)));
        connect(_elementPropertyForm, SIGNAL(signalEdgeStateChanged(uint,EdgeProperty)), this, SLOT(slotEdgeStateChanged(uint,EdgeProperty)));

        ew::EmbeddedWidgetStruct struc;
        ew::EmbeddedHeaderStruct headStr;
        headStr.hasCloseButton = true;
        headStr.windowTitle = title;
        //headStr.headerPixmap = QString(":/img/floorgraphstate.png");
        struc.widgetTag = QString("FloorGraphMakerElementForm");
        // struc.minSize = QSize(300,25);
        struc.maxSize = QSize(500,200);
        // struc.size = QSize(400,25);
        struc.header = headStr;
        struc.iface = _elementPropertyForm;
        struc.topOnHint = true;
        struc.isModal = true;
        struc.isModalBlock = false;
        // struc.addHided = true;

        ewApp()->createWidget(struc); //, viewInterface->getVisualizerWindowId());

//        _elementPropertyForm = new ElementPropertyForm(_elementHovered);
//        _elementPropertyForm->setStyleSheet(ewApp()->getMainStylesheet());
//        connect(_elementPropertyForm, SIGNAL(signalRemoveElement(uint)), this, SLOT(slotRemoveElement(uint)));
//        connect(_elementPropertyForm, SIGNAL(signalSetEdgeProperty(uint,EdgeProperty)), this, SLOT(slotSetEdgeProperty(uint,EdgeProperty)));
//        _elementPropertyForm->move(QCursor::pos());
//        _elementPropertyForm->show();
    }
}

void FloorGraphMakerState::slotElementFormClose()
{
    ewApp()->removeWidget(_elementPropertyForm->id());
    _elementPropertyForm->deleteLater();
    _elementPropertyForm = nullptr;
}

void FloorGraphMakerState::slotEdgeStateChanged(uint elementId, EdgeProperty property)
{
    auto it = _graphElements.find(elementId);
    if(it != _graphElements.end())
    {
        EdgeElement * edgeElement = dynamic_cast<EdgeElement*>(it.value());
        if(edgeElement)
            edgeElement->setEdgeProperty(property);
    }
}

void FloorGraphMakerState::slotRemoveElementFromForm(uint elementId)
{
    slotRemoveElement(elementId);

    quint64 ID = _elementPropertyForm->id();
    ewApp()->setVisible(ID, false);
    ewApp()->removeWidget(ID);
    _elementPropertyForm->deleteLater();
    _elementPropertyForm = nullptr;
}



































