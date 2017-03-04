#ifndef SCROLLBASESTATE_H
#define SCROLLBASESTATE_H

#include "stateobject.h"

class ScrollBaseState : public visualize_system::StateObject
{
public:
    ScrollBaseState();
    virtual bool mouseMoveEvent(QMouseEvent* e, QPointF scenePos);
    virtual bool mousePressEvent(QMouseEvent* e, QPointF scenePos);
    virtual bool mouseReleaseEvent(QMouseEvent *e, QPointF scenePos);
    virtual bool wheelEvent(QWheelEvent* e, QPointF scenePos);
};

#endif // SCROLLBASESTATE_H
