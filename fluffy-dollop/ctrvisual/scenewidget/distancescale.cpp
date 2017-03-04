#include "distancescale.h"
#include <QGraphicsScene>

DistanceScale::DistanceScale(QGraphicsScene *scene) :
      m_sceneFirstPoint(0,0),
      m_sceneSecondPoint(0,0)

{
    m_scene = scene;
    m_line = new QGraphicsLineItem;
    scene->addItem(m_line);
    m_line->hide();
    m_distStr = QString("");
    QPen m_pen;
    m_pen.setWidth(2);
    m_pen.setColor(QColor(0,0,0,100));
    m_pen.setCosmetic(true);
    m_line->setPen(m_pen);
    m_line->setZValue(255);
}


DistanceScale::~DistanceScale()
{
    m_line->hide();
    delete m_line;

}



void DistanceScale::setFirstPoint(QPointF firstPoint)
{
    m_line->hide();
    m_sceneFirstPoint = firstPoint;
    m_line->setPos(firstPoint);

}

QRectF DistanceScale::setSecondPoint(QPointF  secondPoint)
{

    m_line->hide();
    m_sceneSecondPoint = secondPoint;
    m_line->setLine(0, 0, m_sceneSecondPoint.x() - m_sceneFirstPoint.x(), m_sceneSecondPoint.y() -m_sceneFirstPoint.y());
    m_line->show();
}

