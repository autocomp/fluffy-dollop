#ifndef ROUTESCALEGRAPHICSITEM_H
#define ROUTESCALEGRAPHICSITEM_H

#include <QObject>
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QFontMetrics>
#include "measuretextitem.h"

/**
 * @brief Класс измирителя расстояний для 2D сцены
 */
class RouteScaleGraphicsItem : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT

    QPolygonF m_poly;
    QRectF m_boundRect, m_ellipseRect;
    QPen m_pen;
    QString m_totalLength;
    QBrush m_brush;
    const qreal m_pointRadius;
    QPainterPath m_shape;
    QVector<QLineF> m_lines;
    qreal m_scaleK;
    QList<MeasureTextItem *> m_TextItems;
    qreal m_totalDist, m_lenghtInMeters;

public:
    explicit RouteScaleGraphicsItem(QGraphicsPolygonItem * gParent = 0, QObject *parent = 0);
    ~RouteScaleGraphicsItem();
    void setPolygon(const QPolygonF &polygon);
    QRectF boundingRect() const;
    QPainterPath shape();
    int type(){return QGraphicsItem::UserType + 2001;}
    void setSceneScale(qreal sceneScale);
    double getLengthInMeters();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QString convertMetersToNormString(qreal meters);
    
signals:
    void signalCalcRealDistance(QLineF sceneLine , qreal * wgsDistance);
    
public slots:
    
};

#endif // ROUTESCALEGRAPHICSITEM_H
