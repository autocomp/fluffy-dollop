#ifndef MEASURESTATEOBJECT_H
#define MEASURESTATEOBJECT_H
#include "stateobject.h"
//#include <QtPlugin>
#include <QObject>
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QFontMetrics>

class MeasureStateObject : public visualize_system::StateObject
{
    Q_OBJECT

public:
    MeasureStateObject() {}
    virtual QString measureName() = 0;
    virtual QIcon buttonIcon() = 0;
    virtual QRectF boundingRect() {return QRectF();}

    void statePushedToStack();
    void statePoppedFromStack();

protected:
    QPolygonF m_Poly;

signals:
    // сигналы для контроллера
    void signalPushedToStack();
    void signalPoppedFromStack();
    void signalMeasureFinished();

    // сигналы для визуализатора
    void signalCalcRealAngle(QLineF l1, QLineF l2, qreal * angle);
    void signalCalcRealDistance(QLineF,qreal*);
    void signalCalcWGS84Radius(QLineF,qreal*);
    void signalSelectReachArea(QPolygonF poly);
    void signalCalcWGS84Square(QPolygonF,qreal*);
    void signalGetLineResolution(QPolygonF,qreal*);
};

//Q_DECLARE_INTERFACE(MeasureStateObject, "ru.vega.contur.MeasureStateObject/1.0")
#endif // MEASURESTATEOBJECT_H
