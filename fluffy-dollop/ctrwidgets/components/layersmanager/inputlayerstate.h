#ifndef INPUTLAYERSTATE_H
#define INPUTLAYERSTATE_H

#include <ctrvisual/scenecontrol/rematchingstate.h>
#include <ctrvisual/state/mark.h>
#include <ctrvisual/state/scrollbasestate.h>
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>

class InputLayerState : public ScrollBaseState
{
    Q_OBJECT

public:
    InputLayerState(const QPixmap & pixmap);
    ~InputLayerState();

    virtual QString stateName() {return "InputLayerState";}
    virtual void statePushedToStack() {}
    virtual void statePoppedFromStack() {}
    virtual void init(QGraphicsScene * scene, QGraphicsView * view, const int * zoom, const double * scale, double frameCoef, uint visualizerId);

    virtual bool mouseMoveEvent(QMouseEvent* e, QPointF scenePos);
    virtual bool mousePressEvent(QMouseEvent* e, QPointF scenePos);
    virtual bool mouseReleaseEvent(QMouseEvent *e, QPointF scenePos);
    virtual bool keyPressEvent(QKeyEvent *e);

signals:
    void signalStateAborted();

private:
    enum STATE
    {
        DONT_EDIT,
        MOVING,
        RESIZING,
        TURN,
        SET_ORIGINAL_SIZE,
        PROPORTIONAL_RESIZING,
        SET_POINT_ON_RASTER,
        SET_POINT_ON_MAP,
        MOVE_POINT_ON_RASTER,
        MOVE_POINT_ON_MAP,
        SCROLL_MAP
    };

    void moveMarks(bool createMarks);
    QPointF convertPolarToDecar(double const & r, double const & fi, const QPointF center)const;
    void convertDecartToPolar(QPointF const & p, double& r, double& fi)const;
    double getLenght(const QPointF& p1, const QPointF& p2)const;
    int normalizeDegree(int degree);
    bool check(double coef, QPointF p1, QPointF p2);

    STATE _state = DONT_EDIT;
    bool _isChanged = false;
    QPointF _originalCenter, _lastMousePos;
    QPixmap _pixmap;
    int _zLevel;
    double _transparancy;
    QPolygonF _vertexOnItem;
    QGraphicsPixmapItem * _pixmapItem = nullptr;
    QGraphicsPolygonItem * _polygonItem = nullptr;
    double _scaleX, _scaleY, _angel, _scaleAfterResizeX, _scaleAfterResizeY;
    QList<global_state::Mark*> markList;
    QCursor _turningCursor, _crossCursor;
};

#endif
