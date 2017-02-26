#include "emptystateobject.h"

EmptyStateObject::EmptyStateObject(QCursor cursor)
    : _cursor(cursor)
{
}

void EmptyStateObject::init(QGraphicsScene * scene, QGraphicsView * view, const int *zoom, const double *scale, double frameCoef, uint visualizerId)
{
    StateObject::init(scene, view, zoom, scale, frameCoef, visualizerId);
    _view->setCursor(_cursor);
}

bool EmptyStateObject::mousePressEvent(QMouseEvent* e, QPointF scenePos)
{
    QPointF nativePos(scenePos);
    convertSceneToNative(nativePos);
    bool eventHandled(false);
    emit signalPressMouseEvent(e, nativePos, eventHandled);
    if(! eventHandled)
        return ScrollBaseState::mousePressEvent(e, scenePos);
    else
        return false;
}

bool EmptyStateObject::mouseReleaseEvent(QMouseEvent* e, QPointF scenePos)
{
    QPointF nativePos(scenePos);
    convertSceneToNative(nativePos);
    bool eventHandled(false);
    emit signalReleaseMouseEvent(e, nativePos, eventHandled);
    if(! eventHandled)
        return ScrollBaseState::mouseReleaseEvent(e, scenePos);
    else
        return false;
}

bool EmptyStateObject::keyReleaseEvent(QKeyEvent * e)
{
    bool eventHandled(false);
    emit signalKeyReleaseEvent(e, eventHandled);
    if(! eventHandled)
        return ScrollBaseState::keyReleaseEvent(e);
    else
        return false;
}

bool EmptyStateObject::keyPressEvent(QKeyEvent * e)
{
    bool eventHandled(false);
    emit signalKeyPressEvent(e, eventHandled);
    if(! eventHandled)
        return ScrollBaseState::keyPressEvent(e);
    else
        return false;
}

void EmptyStateObject::statePushedToStack()
{
    emit signalStatePushedToStack();
}

void EmptyStateObject::statePoppedFromStack()
{
    emit signalStatePoppedToStack();
}

void EmptyStateObject::setCursor(QCursor cursor)
{
    _cursor = cursor;
    _view->setCursor(_cursor);
}

QString EmptyStateObject::stateName()
{
    return QString("EmptyStateObject");
}

QGraphicsScene * EmptyStateObject::getScene()
{
    return _scene;
}

QGraphicsView * EmptyStateObject::getView()
{
    return _view;
}

void EmptyStateObject::setZlevel(int level)
{
    StateObject::setZlevel(level);
}

