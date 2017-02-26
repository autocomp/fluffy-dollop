#include "measurestateobject.h"
#include <QPainter>
#include <QPen>
#include <QLineF>
#include <QRegion>
#include <QPointF>
#include <QWidget>
#include <QDebug>

void MeasureStateObject::statePushedToStack()
{
    emit signalPushedToStack();
}

void MeasureStateObject::statePoppedFromStack()
{
    emit signalPoppedFromStack();
}


