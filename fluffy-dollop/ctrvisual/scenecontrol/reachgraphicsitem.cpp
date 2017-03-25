#include "reachgraphicsitem.h"

#include <QPainter>
#include <QPen>
#include <QLineF>
#include <QRegion>
#include <QPointF>
#include <QWidget>
#include <QDebug>

#include "scenecontroller.h"


//#include <GeographicLib/PolygonArea.hpp>
//#include <GeographicLib/Geodesic.hpp>
//#include <GeographicLib/Constants.hpp>


ReachGraphicsItem::ReachGraphicsItem(visualize_system::SceneController * contr,QGraphicsEllipseItem * gParent, QObject *parent) :
    QGraphicsEllipseItem(gParent)
  , QObject(parent)
{

    m_sceneControl = contr;

    m_selectingFinished = false;
    m_centerSetted = false;
    setFlag(ItemIsMovable , false);
    setFlag(ItemIsSelectable, false);
    setFlag(ItemIsFocusable, false);
    setZValue(100);

    QColor colPen;
    colPen.setRgba(qRgba(0x09,0x61,0xD3,150));
    m_pen.setColor(colPen);
    m_pen.setWidth(2);
    m_pen.setCosmetic(true);
    m_pen.setJoinStyle(Qt::RoundJoin);

    QColor colBr;
    colBr.setRgba(qRgba(0xF3,0xF0,0x0F,150));
    m_brush.setColor(colBr);
    m_brush.setStyle(Qt::SolidPattern);

    m_TextItem = new MeasureTextItem(this, QBrush(QColor(0xF3,0xF0,0x0F)), QPen(QColor(0x09,0x61,0xD3)));
    m_TextItem->setFlags(ItemIgnoresTransformations);
    m_TextItem->setPen(QPen(QColor(0x09,0x61,0xD3)));
    m_TextItem->setZValue(101);
    m_TextItem->hide();
}

void ReachGraphicsItem::setPenBrush(QPen pen, QBrush brush)
{
    m_pen = pen;
    m_brush = brush;
}

void ReachGraphicsItem::setCenter(const QPointF & point)
{
    m_centerSetted = true;
    m_selectingFinished = false;
    m_center = point;
    calcEllipse();
}

void ReachGraphicsItem::setRadiusPoint(QPointF radiusPoint)
{
    m_radiusPoint = radiusPoint;
    calcEllipse();
}

qreal ReachGraphicsItem::getRadius()
{
    return m_radius;
}



QPointF ReachGraphicsItem::getDirectGeoPoint(
        QPointF lat_lon,    // гео. координаты исходной точки
        double  yaw,     // направление (градусы)
        double  distanse    // расстояние   (м)
        )
{

//    static GeographicLib::Geodesic * geod = new GeographicLib::Geodesic(
//                GeographicLib::Constants::WGS84_a(),
//                GeographicLib::Constants::WGS84_f()
//                );



    double lat(0), lon(0);
//    geod->Direct(
//                lat_lon.x(),          // широта исх.точки
//                lat_lon.y(),              // долгота исх.точки
//                yaw,           // направление (азимут)
//                distanse,
//                lat,
//                lon
//                );
    return QPointF( lat, lon );
}


void ReachGraphicsItem::calcEllipse()
{
    if(m_selectingFinished)
        return;

    m_radius = QLineF(m_center, m_radiusPoint).length();

    QPointF centr = m_center;
    m_sceneControl->sceneToRealGeo(centr);
    qreal radiusMeters = 0;
    emit signalCalcWGS84Radius(QLineF(m_center, m_radiusPoint), &radiusMeters );

    QPointF p1 = getDirectGeoPoint(centr,0,radiusMeters);
    QPointF p2 = getDirectGeoPoint(centr,90,radiusMeters);
    QPointF p3 = getDirectGeoPoint(centr,180,radiusMeters);
    QPointF p4 = getDirectGeoPoint(centr,270,radiusMeters);

    QPolygonF pol;
    m_sceneControl->realGeoToScene(p1);
    m_sceneControl->realGeoToScene(p2);
    m_sceneControl->realGeoToScene(p3);
    m_sceneControl->realGeoToScene(p4);
    pol << p1 << p2 << p3 << p4;
    m_rect = pol.boundingRect();

    m_radiusY = qAbs(QLineF(m_center,p1).length());
    m_radiusX = qAbs(QLineF(m_center,p2).length());
    m_rect = QRectF(m_center.x() - m_radius, m_center.y() - m_radius, 2*m_radius,2*m_radius);



    QString m_squareText = convertMetersToNormString(radiusMeters);

    m_shape  = QPainterPath();
    m_shape.addRect(m_rect);
    m_boundRect = m_shape.boundingRect();

    m_TextItem->setText(QString(" ") + m_squareText + QString(" "));
    m_TextItem->setPos(m_center);
    m_TextItem->translate( -(m_TextItem->getTextSize().width() / 2.), -(m_TextItem->getTextSize().height() / 2.) );
    m_TextItem->show();

    prepareGeometryChange();
    update();
}

QPainterPath ReachGraphicsItem::ellipsePath()
{
    QPainterPath pp;
    pp.addEllipse(m_center,m_radius, m_radius);
    return pp;
}

QRectF ReachGraphicsItem::boundingRect() const
{
    return m_boundRect;
}

QPainterPath ReachGraphicsItem::shape()
{
    return m_shape;
}

bool ReachGraphicsItem::isSelectingFinished()
{
    return m_selectingFinished;
}

bool ReachGraphicsItem::isCenterSetted()
{
    return m_centerSetted;
}

void ReachGraphicsItem::clear()
{
    m_centerSetted = false;
    m_center = QPointF(0,0);
    m_radiusPoint = QPointF(0,0);
    m_selectingFinished = false;
    calcEllipse();
}

void ReachGraphicsItem::finishSelecting(bool finish)
{
    m_selectingFinished = finish;
    update();
}

void ReachGraphicsItem::setSceneScale(qreal sceneScale)
{
    m_scaleK = sceneScale;
    update();
}

void ReachGraphicsItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(!m_centerSetted )
        return;

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(m_pen);
    painter->setBrush(m_brush);
    painter->drawEllipse(m_center,m_radiusX, m_radiusY);
}

QString ReachGraphicsItem::convertMetersToNormString(qreal meters)
{
    QString normStr;
    /*переводим в адекватные единицы измерения*/
    if(meters < 0.1)
        normStr = normStr.setNum( (int)(meters*100),10 ) + QString(" ") + QString(tr("cm"));

    if(meters < 1)
        normStr = normStr.setNum( (int)(meters*10),10 ) + QString(" ")+ QString(tr("dm"));

    if(meters >= 1)
        normStr = normStr.setNum( (int)(meters),10 ) + QString(" ")+ QString(tr("m"));

    if(meters>=1000 )
        normStr = normStr.setNum( (meters/1000.0), 'f' ,1 ) + QString(" ")+ QString(tr("km"));

    return normStr;
}

