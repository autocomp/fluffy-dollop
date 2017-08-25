#include "svgitem.h"
#include <QPainter>
#include <QCursor>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

using namespace svg_editor_state;

SvgItem::SvgItem(QGraphicsView *view, const QPolygonF &points, const StrokeParams &strokeParams)
    : _view(view)
    , _points(points)
    , _strokeParams(strokeParams)
{
    _selectedPolygon = new PolygonItem(points.boundingRect());
    _selectedPolygon->setZValue(1001);
    QPen pen(Qt::yellow);
    pen.setWidth(2);
    pen.setCosmetic(true);
    _selectedPolygon->setPen(pen);
    _selectedPolygon->hide();
    view->scene()->addItem(_selectedPolygon);
}

SvgItem::~SvgItem()
{
    delete _selectedPolygon;
    foreach(SvgLine* item, _lineItems)
        delete item;
}

void SvgItem::clickOnLine(QGraphicsSceneMouseEvent *event)
{
    if(event->buttons()&Qt::LeftButton)
        emit signalClickOnItem(event->modifiers()&Qt::ControlModifier);
}

void SvgItem::selectItem(bool on_off)
{
    _selectedPolygon->setVisible(on_off);
}

//--------------------------------------------

SvgLineItem::SvgLineItem(QGraphicsView *view, const QPolygonF &points, const StrokeParams &strokeParams, bool onTop)
    : SvgItem(view, points, strokeParams)
{
    for(int i(1); i < points.size(); ++i)
    {
        QPointF p1(points.at(i-1));
        QPointF p2(points.at(i));
        _lineItems.append(new SvgLine(this, view->scene(), strokeParams.getLinePen(), QLineF(p1, p2), onTop));
    }
}

SvgLineItem::~SvgLineItem()
{
}

QString SvgLineItem::getXmlLine(QPointF delta)
{
    QString strLine("<polyline points=\"");
    foreach(QPointF scenePos, _points)
    {
        scenePos = scenePos - delta;
        strLine.append(QString::number(scenePos.x(), 'f', 20) + QString(",") + QString::number(scenePos.y(), 'f', 20) + QString(" "));
    }
    strLine.append(QString("\" stroke-width=\"") + QString::number(_strokeParams.lineWidth) + QString("\" "));
    strLine.append(QString("stroke=\"rgb(") + QString::number(_strokeParams.lineColor.red()) + QString(",") + QString::number(_strokeParams.lineColor.green()) + QString(",") + QString::number(_strokeParams.lineColor.blue()) + QString(")\" "));
    if(_strokeParams.lineStyle != Qt::SolidLine)
    {
        strLine.append(QString("stroke-dasharray=\""));
        QPainterPathStroker stroker;
        stroker.setDashPattern(_strokeParams.lineStyle);
        for(int i(0); i < stroker.dashPattern().size(); ++i)
        {
            if(i != 0)
                strLine.append(QString(","));
            strLine.append(QString::number(stroker.dashPattern().at(i),'f',0));
        }
        strLine.append(QString("\" "));
    }
    strLine.append(QString("fill=\"none\" vector-effect=\"non-scaling-stroke\"/>\n"));
    return strLine;
}

void SvgLineItem::setStrokeParams(const StrokeParams &strokeParams)
{
    foreach(SvgLine* item, _lineItems)
        item->setPen(strokeParams.getLinePen());
}

void SvgLineItem::setZValue(double z)
{
    foreach(SvgLine* item, _lineItems)
        item->setZValue(z);
}

void SvgLineItem::viewZoomChanged()
{
    foreach(SvgLine* item, _lineItems)
        item->recalcShape(_view);
}

//--------------------------------------------

SvgPolygonItem::SvgPolygonItem(QGraphicsView *view, const QPolygonF &points, const StrokeParams & strokeParams, bool onTop)
    : SvgItem(view, points, strokeParams)
{
    _polygonItem = new PolygonItem(points);
    _polygonItem->setZValue(1000);
    _polygonItem->setPen(QPen(Qt::NoPen));
    _polygonItem->setBrush(strokeParams.getBrush());
    _polygonItem->setVisible(strokeParams.fillArea);
    view->scene()->addItem(_polygonItem);

    for(int i(1); i < points.size(); ++i)
    {
        QPointF p1(points.at(i-1));
        QPointF p2(points.at(i));
        _lineItems.append(new SvgLine(this, view->scene(), strokeParams.getLinePen(), QLineF(p1, p2), onTop));
    }
    _lineItems.append(new SvgLine(this, view->scene(), strokeParams.getLinePen(), QLineF(points.first(), points.last()), onTop) );
}

SvgPolygonItem::~SvgPolygonItem()
{
    delete _polygonItem;
}

QString SvgPolygonItem::getXmlLine(QPointF delta)
{
    QString strLine("<polygon points=\"");
    foreach(QPointF scenePos, _points)
    {
        scenePos = scenePos - delta;
        strLine.append(QString::number(scenePos.x(), 'f', 20) + QString(",") + QString::number(scenePos.y(), 'f', 20) + QString(" "));
    }
    strLine.append(QString("\" stroke-width=\"") + QString::number(_strokeParams.lineWidth) + QString("\" "));
    strLine.append(QString("stroke=\"rgb(") + QString::number(_strokeParams.lineColor.red()) + QString(",") + QString::number(_strokeParams.lineColor.green()) + QString(",") + QString::number(_strokeParams.lineColor.blue()) + QString(")\" "));
    if(_strokeParams.lineStyle != Qt::SolidLine)
    {
        strLine.append(QString("stroke-dasharray=\""));
        QPainterPathStroker stroker;
        stroker.setDashPattern(_strokeParams.lineStyle);
        for(int i(0); i < stroker.dashPattern().size(); ++i)
        {
            if(i != 0)
                strLine.append(QString(","));
            strLine.append(QString::number(stroker.dashPattern().at(i),'f',0));
        }
        strLine.append(QString("\" "));
    }
    if(_strokeParams.fillArea)
    {
        strLine.append(QString("fill-opacity=\"") + QString::number(_strokeParams.opacity,'f',2) + QString("\" "));
        strLine.append(QString("fill=\"rgb(") + QString::number(_strokeParams.fillColor.red()) + QString(",") + QString::number(_strokeParams.fillColor.green()) + QString(",") + QString::number(_strokeParams.fillColor.blue()) + QString(")\" "));
    }
    else
    {
        strLine.append(QString("fill=\"none\" "));
    }
    strLine.append(QString("vector-effect=\"non-scaling-stroke\"/>\n"));
    return strLine;
}

void SvgPolygonItem::setStrokeParams(const StrokeParams &strokeParams)
{
    _polygonItem->setBrush(strokeParams.getBrush());
    _polygonItem->setVisible(strokeParams.fillArea);

    foreach(SvgLine* item, _lineItems)
        item->setPen(strokeParams.getLinePen());
}

void SvgPolygonItem::setZValue(double z)
{
    _polygonItem->setZValue(z);
    foreach(SvgLine* item, _lineItems)
        item->setZValue(z);
}

void SvgPolygonItem::viewZoomChanged()
{
    foreach(SvgLine* item, _lineItems)
        item->recalcShape(_view);
}

//--------------------------------------------

SvgLine::SvgLine(SvgItem *svgItem, QGraphicsScene * _scene, QPen pen, QLineF line, bool onTop)
    : QGraphicsLineItem(line)
    , _svgItem(svgItem)
{
    setPen(pen);
    setZValue(onTop ? 1000 : -1000);
    _scene->addItem(this);

    setAcceptHoverEvents(true);
    setCursor(QCursor(Qt::PointingHandCursor));
}

void SvgLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(line().isNull() == false)
    {
        painter->setRenderHint(QPainter::Antialiasing);
        QGraphicsLineItem::paint(painter, option, widget);
    }
}

QPainterPath SvgLine::shape() const
{
    QPainterPath path;
    if(_shapePolygon.isEmpty())
    {
        path.moveTo(line().p1());
        path.lineTo(line().p2());
    }
    else
    {
        path.addPolygon(_shapePolygon);
        path.closeSubpath();
    }
    return path;
}

void SvgLine::recalcShape(QGraphicsView * view)
{
    int W = pen().width() <= 2 ? pen().width() : pen().width() / 2;
    QPolygon polygon;

    QPoint p1 = view->mapFromScene(line().p1());
    QPoint p1_topLeft(p1.x()-W, p1.y()-W);
    polygon.append(p1_topLeft);
    QPoint p1_topRight(p1.x()+W, p1.y()-W);
    polygon.append(p1_topRight);
    QPoint p1_bottomRight(p1.x()+W, p1.y()+W);
    polygon.append(p1_bottomRight);
    QPoint p1_bottomLeft(p1.x()-W, p1.y()+W);
    polygon.append(p1_bottomLeft);

    QPoint p2 = view->mapFromScene(line().p2());
    QPoint p2_topLeft(p2.x()-W, p2.y()-W);
    polygon.append(p2_topLeft);
    QPoint p2_topRight(p2.x()+W, p2.y()-W);
    polygon.append(p2_topRight);
    QPoint p2_bottomRight(p2.x()+W, p2.y()+W);
    polygon.append(p2_bottomRight);
    QPoint p2_bottomLeft(p2.x()-W, p2.y()+W);
    polygon.append(p2_bottomLeft);

    QRect r = polygon.boundingRect();
    QPolygon viewPolygon;
    if(r.topLeft() == p1_topLeft && r.bottomRight() == p2_bottomRight && polygon.contains(r.topRight()) == false)
    {
        // линия идёт от первой точки вниз и на право ко второй точке
        viewPolygon << p1_bottomLeft << p1_topLeft << p1_topRight << p2_topRight << p2_bottomRight << p2_bottomLeft;
    }
    else if(r.topLeft() == p2_topLeft && r.bottomRight() == p1_bottomRight && polygon.contains(r.topRight()) == false)
    {
        // линия идёт от второй точки вниз и на право к первой точке
        viewPolygon << p2_bottomLeft << p2_topLeft << p2_topRight << p1_topRight << p1_bottomRight << p1_bottomLeft;
    }
    else if(r.bottomLeft() == p1_bottomLeft && r.topRight() == p2_topRight && polygon.contains(r.topLeft()) == false)
    {
        // линия идёт от первой точки вверх и на право ко второй точке
        viewPolygon << p1_bottomRight << p1_bottomLeft << p1_topLeft << p2_topLeft << p2_topRight << p2_bottomRight;
    }
    else if(r.bottomLeft() == p2_bottomLeft && r.topRight() == p1_topRight && polygon.contains(r.topLeft()) == false)
    {
        // линия идёт от второй точки вверх и на право к первой точке
        viewPolygon << p2_bottomRight << p2_bottomLeft << p2_topLeft << p1_topLeft << p1_topRight << p1_bottomRight;
    }
    else
    {
        // линия идет или строго по горизонтали\вертикали или схлопнута в одну точку (при текущем масштабе вьюхи)
        viewPolygon = QPolygon(r);
    }

    _shapePolygon.clear();
    foreach(QPoint viewPos, viewPolygon)
        _shapePolygon.append(view->mapToScene(viewPos));
}

void SvgLine::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    _svgItem->clickOnLine(event);
}

void SvgLine::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    qDebug() << "SvgLine::hoverMoveEvent !!!";
}


//--------------------------------------------

PolygonItem::PolygonItem(const QPolygonF &points)
    : QGraphicsPolygonItem(points)
{
}

void PolygonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    QGraphicsPolygonItem::paint(painter, option, widget);
}
