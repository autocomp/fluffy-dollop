#ifndef EMPTYSTATEOBJECT_H
#define EMPTYSTATEOBJECT_H

#include "scrollbasestate.h"

class EmptyStateObject : public ScrollBaseState
{
    Q_OBJECT

public:
    EmptyStateObject(QCursor cursor = QCursor(Qt::ArrowCursor));
    void init(QGraphicsScene * scene, QGraphicsView * view, const int * zoom,  const double * scale, double frameCoef, uint visualizerId);
    bool mousePressEvent(QMouseEvent* e, QPointF scenePos);
    bool mouseReleaseEvent(QMouseEvent* e, QPointF scenePos);
    bool keyPressEvent(QKeyEvent * e);
    bool keyReleaseEvent(QKeyEvent * e);
    void statePushedToStack();
    void statePoppedFromStack();
    QString stateName();
    void setCursor(QCursor cursor);
    QGraphicsScene * getScene();
    QGraphicsView * getView();
    void setZlevel(int);

signals:
    void signalPressMouseEvent(QMouseEvent* e, QPointF nativePos, bool & eventHandled);
    void signalReleaseMouseEvent(QMouseEvent* e, QPointF nativePos, bool & eventHandled);
    void signalKeyPressEvent(QKeyEvent* e, bool & eventHandled);
    void signalKeyReleaseEvent(QKeyEvent* e, bool & eventHandled);
    void signalStatePushedToStack();
    void signalStatePoppedToStack();

private:
    QCursor _cursor;
};

#endif
