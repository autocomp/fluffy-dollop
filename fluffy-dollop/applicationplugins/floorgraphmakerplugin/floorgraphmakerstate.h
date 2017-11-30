#ifndef FLOORGRAPHMAKERSTATE_H
#define FLOORGRAPHMAKERSTATE_H

#include <QObject>
#include <QTimer>
#include <ctrvisual/state/scrollbasestate.h>
#include <ctrwidgets/components/layersmanager/lineitem.h>
#include "floorgraphtypes.h"
#include "floorgraphelement.h"
//#include "elementpropertyform.h"

namespace floor_graph_maker
{

enum class Mode {ScrollMode, SetNodeOrEdgeElement};

class InstrumentalForm;

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
    explicit FloorGraphMakerState(uint64_t floorId);
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
    void edgeStateChanged(EdgeProperty property);

signals:

private slots:
    void slotRemoveElement(uint);
    void showElementPropertyForm();
    void slotSetEdgeProperty(uint edgeId, EdgeProperty property);
//    void slotElementFormClose();
    void slotEdgeStateChanged(uint elementId, EdgeProperty property);
    void slotRemoveElementFromForm(uint elementId);

private:
    const uint64_t _floorId;
    Mode _mode = Mode::ScrollMode;
    EdgeProperty _edgeDefaultProperty;

    NodeElement * _lastNodeElement = nullptr;
    transform_state::LineItem * _tempEdgeElement = nullptr;

    uint _tempElementCounter = 0;
    QMap<uint, FloorGraphElement*> _graphElements;
    QPointF _lastMousePos;

    FloorGraphElement * _elementHovered = nullptr;
    InstrumentalForm * _elementPropertyForm = nullptr;
//    QTimer _timer;
    QPen _pen;

};

}

#endif // FLOORGRAPHMAKERSTATE_H
