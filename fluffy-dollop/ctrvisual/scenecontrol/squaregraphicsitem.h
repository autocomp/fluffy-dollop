#ifndef SQUAREGRAPHICSITEM_H
#define SQUAREGRAPHICSITEM_H

#include <QObject>
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QPen>
#include <QBrush>

#include "measuretextitem.h"

/**
 * @brief Класс измирителя площадей для 2D сцены
 */
class SquareGraphicsItem : public QObject, public QGraphicsPolygonItem
{
    Q_OBJECT

    QPolygonF m_poly;
    QRectF m_boundRect, m_ellipseRect;
    QPen m_pen;
    QBrush m_brush;
    const qreal m_pointRadius;
    QPainterPath m_shape;
    bool m_textMode;
    bool m_selectingFinished;
    qreal m_scaleK;
    QVector<QLineF> m_lines;
    MeasureTextItem * m_TextItem;

public:
    explicit SquareGraphicsItem(QGraphicsPolygonItem * gParent = 0, QObject *parent = 0);
    void setPolygon(const QPolygonF &polygon);
    QRectF boundingRect() const;
    QPainterPath shape();
    void finishSelecting(bool finish);
    int type(){return QGraphicsItem::UserType + 2002;}
    bool isSelectingFinished();
    void setPenBrush(QPen pen, QBrush brush);
    void setPhysValue(QString str);
    void setTextMode(bool isTextMode);
    void setSceneScale(qreal sceneScale);
protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    QString convertMetersSquareToNormString(qreal square);

signals:
    void signalCalcWGS84Square(QPolygonF poly, qreal * wgsDistance);
};

#endif // SQUAREGRAPHICSITEM_H
