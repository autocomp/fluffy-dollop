#ifndef ANGLEGRAPHICSITEM_H
#define ANGLEGRAPHICSITEM_H

#include <QObject>
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QPen>
#include <QBrush>
#include "measuretextitem.h"

/**
 * @brief Класс QGraphicsitem для измерения углов
 */
class AngleGraphicsItem : public QObject, public QGraphicsItem
{
    Q_OBJECT

    QPolygonF m_poly;
    QLineF m_firstLine, m_secondLine;
    qreal m_angle;
    QRectF m_boundRect, m_ellipseRect;
    QPen m_pen;
    QBrush m_brush;
    qreal m_scaleK;
    MeasureTextItem * m_TextItem;

public:
    explicit AngleGraphicsItem(QGraphicsItem *gParent = 0, QObject * parent = 0);
    void setPolygon(const QPolygonF &polygon);
    QRectF boundingRect() const;
    QPainterPath shape();
    int type(){return QGraphicsItem::UserType + 2000;}
    void setSceneScale(qreal sceneScale);

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

signals:
    void signalCalcRealAngle(QLineF l1, QLineF l2, qreal * angle);
    
};

#endif // ANGLEGRAPHICSITEM_H
