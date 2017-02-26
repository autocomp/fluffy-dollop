#ifndef TRANSFORMINGSTATE_H
#define TRANSFORMINGSTATE_H

#include "rematchingstate.h"
#include <ctrvisual/state//mark.h>
#include <QGraphicsPixmapItem>

namespace visualize_system {

class TransformingState : public RematchingState
{
    Q_OBJECT

public:
    TransformingState(uint rasterId, const QPixmap & pixmap, int zLevel, QPolygonF sceneVertex, int transparancy);
    ~TransformingState();

    virtual QString stateName();
    virtual void statePushedToStack();
    virtual void statePoppedFromStack();
    virtual void init(QGraphicsScene * scene, QGraphicsView * view, const int * zoom, const double * scale, double frameCoef, uint visualizerId);

    virtual bool mouseMoveEvent(QMouseEvent* e, QPointF scenePos);
    virtual bool mousePressEvent(QMouseEvent* e, QPointF scenePos);
    virtual bool mouseReleaseEvent(QMouseEvent *e, QPointF scenePos);
    virtual bool mouseDoubleClickEvent(QMouseEvent *e, QPointF scenePos);
    virtual bool keyPressEvent(QKeyEvent * e);

    void setTrancparency(int val);
    QPolygonF getCurrentVertex();

signals:
    void signalTransformFinished();

private:
    void moveMarks(bool createMarks);

    QPointF _originalCenter;
    QPixmap _pixmap;
    int _zLevel;
    double _transparancy;
    QPolygonF _vertexOnItem;
    QGraphicsPixmapItem * _pixmapItem;
    double _scaleX, _scaleY, _angel, _scaleAfterResizeX, _scaleAfterResizeY;
    QList<global_state::Mark*> markList;
    QCursor _turningCursor, _crossCursor;
};
}

#endif // TRANSFORMINGSTATE_H
