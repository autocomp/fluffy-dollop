#include "floorgraphmakerstate.h"

using namespace floor_graph_maker;

FloorGraphMakerState::FloorGraphMakerState()
{

}

FloorGraphMakerState::~FloorGraphMakerState()
{

}

void FloorGraphMakerState::init(QGraphicsScene *scene, QGraphicsView *view, const int *zoom, const double *scale, double frameCoef, uint visualizerId)
{
    ScrollBaseState::init(scene, view, zoom, scale, frameCoef, visualizerId);

}

bool FloorGraphMakerState::wheelEvent(QWheelEvent *e, QPointF scenePos)
{
    return true;
}

bool FloorGraphMakerState::mouseMoveEvent(QMouseEvent *e, QPointF scenePos)
{
    return true;
}

bool FloorGraphMakerState::mousePressEvent(QMouseEvent *e, QPointF scenePos)
{
        return true;
}

bool FloorGraphMakerState::mouseReleaseEvent(QMouseEvent *e, QPointF scenePos)
{
        return true;
}

bool FloorGraphMakerState::mouseDoubleClickEvent(QMouseEvent *e, QPointF scenePos)
{
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

}

void FloorGraphMakerState::setMode(floor_graph_maker::Mode mode)
{
    _mode = mode;

}
