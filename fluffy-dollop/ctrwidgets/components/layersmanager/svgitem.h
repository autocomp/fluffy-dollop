#ifndef SVGITEM_H
#define SVGITEM_H

#include "svgeditortypes.h"
#include <QObject>
#include <QPolygonF>
#include <QPen>
#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QList>

namespace svg_editor_state
{

class SvgLine;
class PolygonItem;

class SvgItem : public QObject
{
    Q_OBJECT
public:
    SvgItem(QGraphicsView * view, const QPolygonF & points, const StrokeParams & strokeParams);
    virtual ~SvgItem();
    virtual QString getXmlLine(QPointF delta = QPointF()) = 0;
    virtual void setStrokeParams(const StrokeParams & strokeParams) = 0;
    virtual void setZValue(double z) = 0;
    virtual void viewZoomChanged() = 0;
    virtual void clickOnLine(QGraphicsSceneMouseEvent *event);
    virtual void selectItem(bool on_off);

signals:
    void signalClickOnItem(bool withControl);

protected:
    QGraphicsView * _view;
    QList<SvgLine*> _lineItems;
    QPolygonF _points;
    StrokeParams _strokeParams;
    PolygonItem * _selectedPolygon;

};

class SvgLineItem : public SvgItem
{
public:
    SvgLineItem(QGraphicsView * view, const QPolygonF & points, const StrokeParams & strokeParams, bool onTop);
    ~SvgLineItem();
    virtual QString getXmlLine(QPointF delta = QPointF());
    virtual void setStrokeParams(const StrokeParams & strokeParams);
    virtual void setZValue(double z);
    virtual void viewZoomChanged();

protected:

};

class SvgPolygonItem : public SvgItem
{
public:
    SvgPolygonItem(QGraphicsView * view, const QPolygonF & points, const StrokeParams & strokeParams, bool onTop);
    ~SvgPolygonItem();

    virtual QString getXmlLine(QPointF delta = QPointF());
    virtual void setStrokeParams(const StrokeParams & strokeParams);
    virtual void setZValue(double z);
    virtual void viewZoomChanged();

protected:
    PolygonItem * _polygonItem = nullptr;
};

class SvgLine : public QGraphicsLineItem
{
public:
    SvgLine(SvgItem * svgItem, QGraphicsScene * scene, QPen pen, QLineF line, bool onTop);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QPainterPath shape() const;
    void recalcShape(QGraphicsView *view);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * event);
    virtual void hoverMoveEvent(QGraphicsSceneHoverEvent * event);

    SvgItem * _svgItem;
    QPolygonF _shapePolygon;
};

class PolygonItem : public QGraphicsPolygonItem
{
public:
    PolygonItem(const QPolygonF & points = QPolygonF());
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

};

}

#endif // SVGITEM_H






