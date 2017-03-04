#ifndef CHOICEOBJECTSTATE_H
#define CHOICEOBJECTSTATE_H

#include <ctrvisual/state/scrollbasestate.h>

class ChoiceObjectState : public ScrollBaseState
{
    Q_OBJECT
public:
    ChoiceObjectState();
    virtual QString stateName();
    virtual void statePushedToStack();
    virtual void statePoppedFromStack();
    virtual bool mousePressEvent(QMouseEvent* e, QPointF scenePos);
    virtual bool keyPressEvent(QKeyEvent * e);

    bool checkObjectUnderMouse(quint64 obgectId, QCursor & cursor);
    bool objectChoiced(quint64 obgectId);
    void setEmitingAbortAfterPushedToStack(bool on_off);

signals:
    void signalCheckObjectUnderMouse(bool & objectValid, quint64 obgectId, QCursor & cursor);
    void signalObjectChoiced(quint64 obgectId);
    void signalAbort();

private:
    quint64 _choicedObgectId;
    bool _emitAbortAfterPushedToStack;
};

#endif // CHOICEOBJETCSTATE_H
