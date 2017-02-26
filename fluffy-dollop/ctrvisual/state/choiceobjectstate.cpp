#include "choiceobjectstate.h"
#include <QDebug>

ChoiceObjectState::ChoiceObjectState()
{
    _choicedObgectId = 0;
    _checkableObjectUnderMouse = true;
    _emitAbortAfterPushedToStack = false;
}

bool ChoiceObjectState::checkObjectUnderMouse(quint64 obgectId, QCursor & cursor)
{
    bool objectValid = false;
    if(obgectId != 0)
        emit signalCheckObjectUnderMouse(objectValid, obgectId, cursor);
    return objectValid;
}

bool ChoiceObjectState::objectChoiced(quint64 obgectId)
{
    _choicedObgectId = obgectId;
}

bool ChoiceObjectState::mousePressEvent(QMouseEvent* e, QPointF scenePos)
{
    if(_choicedObgectId != 0)
    {
        emit signalObjectChoiced(_choicedObgectId);
        return false;
    }
    else
        return ScrollBaseState::mousePressEvent(e, scenePos);
}

bool ChoiceObjectState::keyPressEvent(QKeyEvent * e)
{
    if(e->key() == Qt::Key_Escape)
        emit signalAbort();

    return true;
}

void ChoiceObjectState::setEmitingAbortAfterPushedToStack(bool on_off)
{
    _emitAbortAfterPushedToStack = on_off;
}

QString ChoiceObjectState::stateName()
{
    return QString("ChoiceObjectState");
}

void ChoiceObjectState::statePushedToStack()
{
    if(_emitAbortAfterPushedToStack)
        emit signalAbort();
}

void ChoiceObjectState::statePoppedFromStack()
{
}



















