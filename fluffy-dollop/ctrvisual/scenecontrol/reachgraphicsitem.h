#ifndef REACHGRAPHICSITEM_H
#define REACHGRAPHICSITEM_H

#include <QObject>
#include <QGraphicsEllipseItem>
#include <QPolygonF>
#include <QPen>
#include <QBrush>
#include "measuretextitem.h"

/**
* @brief Класс измирителя области обнаружения для 2D сцены
*/

namespace visualize_system {
class SceneController;
}

class ReachGraphicsItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT

    visualize_system::SceneController * m_sceneControl;
    QPointF m_center, m_radiusPoint;
    QRectF m_rect;
    qreal m_radius, m_radiusX, m_radiusY;
    QRectF m_boundRect, m_ellipseRect;
    QPen m_pen;
    QBrush m_brush;
    QPainterPath m_shape;
    bool m_centerSetted;
    bool m_selectingFinished;
    qreal m_scaleK;
    MeasureTextItem * m_TextItem;

public:
    explicit ReachGraphicsItem(visualize_system::SceneController * contr, QGraphicsEllipseItem * gParent = 0, QObject *parent = 0);
    void setCenter(const QPointF & point);
    QRectF boundingRect() const;
    QPainterPath shape();
    void finishSelecting(bool finish);
    int type(){return QGraphicsItem::UserType + 2003;}
    bool isSelectingFinished();
    void setPenBrush(QPen pen, QBrush brush);
    void setRadiusPoint(QPointF radiusPoint);
    qreal getRadius();
    bool isCenterSetted();
    void clear();
    void setSceneScale(qreal sceneScale);
    QPainterPath ellipsePath();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QString convertMetersToNormString(qreal meters);
    QPointF getDirectGeoPoint(
            QPointF lat_lon,    // гео. координаты исходной точки
            double  yaw,     // направление (градусы)
            double  distanse    // расстояние   (м)
            );

private:
    void calcEllipse();

signals:
    void signalCalcWGS84Radius(QLineF line, qreal * wgsDistance);

public slots:

};

#endif // REACHGRAPHICSITEM_H
