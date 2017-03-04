#include "scrollbasestate.h"

ScrollBaseState::ScrollBaseState()
{
}

bool ScrollBaseState::mouseMoveEvent(QMouseEvent*, QPointF)
{
    return true;
}

bool ScrollBaseState::mousePressEvent(QMouseEvent*, QPointF)
{
    if(_view)
        _view->setDragMode(QGraphicsView::ScrollHandDrag);
    return true;
}

bool ScrollBaseState::mouseReleaseEvent(QMouseEvent *e, QPointF scenePos)
{
    if(_view)
    {
        _view->setDragMode(QGraphicsView::NoDrag);
        _view->setCursor(QCursor(Qt::ArrowCursor));
    }
    return true;
}

bool ScrollBaseState::wheelEvent(QWheelEvent* e, QPointF scenePos)
{
    return true;
}
