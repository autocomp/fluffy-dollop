#ifndef FLOORGRAPHMAKERSTATE_H
#define FLOORGRAPHMAKERSTATE_H

#include <QObject>
#include <QTimer>
#include <ctrvisual/state/scrollbasestate.h>
#include <ctrwidgets/components/layersmanager/lineitem.h>
#include "floorgraphelement.h"
#include "elementpropertyform.h"

namespace floor_graph_maker
{

enum class Mode {ScrollMode, SetNodeOrEdgeElement, SetCamera, SetPlaceHolder};

class ElementHolder
{
public:
    ElementHolder() {}
    virtual ~ElementHolder() {}
    virtual void elementHovered(FloorGraphElement * element = nullptr) = 0;
    virtual void elementDoubleClicked(FloorGraphElement * element) = 0;
};

class FloorGraphMakerState : public ScrollBaseState, public ElementHolder
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
    virtual bool keyPressEvent(QKeyEvent * e);
    virtual QString stateName();
    virtual void statePushedToStack();
    virtual void statePoppedFromStack();
    virtual void elementHovered(FloorGraphElement *element = nullptr);
    virtual void elementDoubleClicked(FloorGraphElement * element);
    void setMode(Mode mode);
    void save();

signals:

private slots:
    void slotRemoveElement(uint);
    void showElementPropertyForm();
    void slotEdgeTypeChanged(uint id, floor_graph_maker::EdgeElement::EdgeType type);
    void slotWallWidthChanged(uint edgeId, double val);

private:
    Mode _mode = Mode::ScrollMode;

    NodeElement * _lastNodeElement = nullptr;
    transform_state::LineItem * _tempEdgeElement = nullptr;

    uint _tempElementCounter = 0;
    QMap<uint, FloorGraphElement*> _graphElements;
    QPointF _lastMousePos;

    FloorGraphElement * _elementHovered = nullptr;
    ElementPropertyForm * _elementPropertyForm = nullptr;
    QTimer _timer;

};

}

#endif // FLOORGRAPHMAKERSTATE_H
