#ifndef DISTANCESCALE_H
#define DISTANCESCALE_H

#include <QAbstractGraphicsShapeItem>
#include<QGraphicsRectItem>
#include <QPen>

class DistanceScale : public QObject
{
    Q_OBJECT
    QGraphicsLineItem * m_line;
    QGraphicsScene *m_scene;
public:
    explicit DistanceScale(QGraphicsScene *scene);

public:
    ~DistanceScale();
    void setFirstPoint(QPointF firstPoint);
    QRectF setSecondPoint(QPointF  secondPoint);
private:
    QString m_distStr;
    QPointF m_sceneFirstPoint, m_sceneSecondPoint;
};

#endif // DISTANCESCALE_H
