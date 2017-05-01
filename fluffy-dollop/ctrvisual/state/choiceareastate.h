#ifndef CHOICEAREASTATE_H
#define CHOICEAREASTATE_H

#include <ctrvisual/state/stateobject.h>

class ChoiceAreaState : public visualize_system::StateObject
{
    Q_OBJECT
public:
    enum AreaType
    {
        POINT,
        POLYLINE,
        POLYGON,
        POINT_OR_POLYGON
    };

    ChoiceAreaState(AreaType areaType, QCursor cursor = QCursor(Qt::ArrowCursor));
    ~ChoiceAreaState();
    void setPen(QPen pen);
    void setEmitingAbortAfterPushedToStack(bool on_off);

    virtual QString stateName();
    virtual void statePushedToStack();
    virtual void statePoppedFromStack();

    virtual bool mouseMoveEvent(QMouseEvent* e, QPointF scenePos);
    virtual bool mousePressEvent(QMouseEvent* e, QPointF scenePos);
    virtual bool mouseDoubleClickEvent(QMouseEvent *e, QPointF scenePos);
    virtual bool keyPressEvent(QKeyEvent * e);

signals:
    void signalAreaChoiced(QPolygonF);
    void signalAbort();

private:
    void finishChoice();
    QGraphicsLineItem* createItem(QPointF p1, QPointF p2);

    const AreaType _areaType;
    QCursor _cursor;
    QPolygonF _polygon;
    QList<QGraphicsLineItem*> _lineItems;
    QPen _pen;
    QPointF _lastMousePos;
    bool _emitAbortAfterPushedToStack;
};

#endif // CHOICEOBJETCSTATE_H
