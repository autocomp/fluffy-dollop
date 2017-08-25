#ifndef SVGEDITORSTATE_H
#define SVGEDITORSTATE_H

#include <ctrvisual/state/scrollbasestate.h>
#include "svgeditortypes.h"

namespace pixmap_transform_state {
class LineItem;
}

namespace svg_editor_state
{

enum class StateMode
{
    SelectMode,
    AddLineMode,
    AddPolygonMode,
    AddRectMode,
    AddEllipseMode,
    AddAnnotationMode
};

class SvgItem;
class PolygonItem;

class SvgEditorState : public ScrollBaseState
{
    Q_OBJECT
public:
    SvgEditorState(bool onTop);
    ~SvgEditorState();
    virtual void init(QGraphicsScene *scene, QGraphicsView *view, const int *zoom, const double *scale, double frameCoef, uint visualizerId);
    virtual bool wheelEvent(QWheelEvent* e, QPointF scenePos);
    virtual bool mouseMoveEvent(QMouseEvent *e, QPointF scenePos);
    virtual bool mousePressEvent(QMouseEvent *e, QPointF scenePos);
    virtual bool mouseReleaseEvent(QMouseEvent *e, QPointF scenePos);
    virtual bool mouseDoubleClickEvent(QMouseEvent *e, QPointF scenePos);
    virtual void zoomChanged();
    virtual QString stateName();
    virtual void statePushedToStack();
    virtual void statePoppedFromStack();

    void setMode(StateMode stateMode);
    StateMode mode();
    void setDefaultStrokeParams(const StrokeParams &strokeParams);
    void saveSvgDoc();

public slots:
    void deleteSelectedItem();

signals:
    void getDefaultStrokeParams(svg_editor_state::StrokeParams&);
    void setEnabledDeleteButton(bool on_off);
    void setEnabledSaveButton(bool on_off);
    void svgDocSaved(QString filePath, QPointF scenePos);

protected slots:
    void slotClickOnItem(bool withControl);

protected:
    pixmap_transform_state::LineItem* createLineItem(QPointF p1, QPointF p2);
    void repaintRectArea(QPointF secondPoint);
    void clearTempItems();

    bool _onTop;
    StateMode _stateMode=StateMode::SelectMode;
    bool _blockWheelEvent=false;
    QList<pixmap_transform_state::LineItem*> _lineItems;
    QPolygonF _linePoints;
    QList<SvgItem*> _svgLineItems;
    PolygonItem * _polygonItem = nullptr;
    QList<SvgItem*> _selectedSvgItems;

};

}

#endif // SVGEDITORSTATE_H
