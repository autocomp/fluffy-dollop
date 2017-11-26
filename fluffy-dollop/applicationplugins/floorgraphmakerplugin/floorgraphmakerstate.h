#ifndef FLOORGRAPHMAKERSTATE_H
#define FLOORGRAPHMAKERSTATE_H

#include <QObject>
#include <ctrvisual/state/scrollbasestate.h>

namespace floor_graph_maker
{

enum class Mode {Scroll, Node, Edge, Door, Window};

class FloorGraphMakerState : public ScrollBaseState
{
    Q_OBJECT
public:
    explicit FloorGraphMakerState();
    ~FloorGraphMakerState();

    virtual void init(QGraphicsScene *scene, QGraphicsView *view, const int *zoom, const double *scale, double frameCoef, uint visualizerId);
    virtual bool wheelEvent(QWheelEvent* e, QPointF scenePos);
    virtual bool mouseMoveEvent(QMouseEvent *e, QPointF scenePos);
    virtual bool mousePressEvent(QMouseEvent *e, QPointF scenePos);
    virtual bool mouseReleaseEvent(QMouseEvent *e, QPointF scenePos);
    virtual bool mouseDoubleClickEvent(QMouseEvent *e, QPointF scenePos);
    virtual QString stateName();
    virtual void statePushedToStack();
    virtual void statePoppedFromStack();
    void setMode(Mode mode);



signals:

public slots:


private:
    Mode _mode = Mode::Scroll;
};

}

#endif // FLOORGRAPHMAKERSTATE_H
