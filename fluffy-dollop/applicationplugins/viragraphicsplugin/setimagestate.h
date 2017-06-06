#ifndef SETIMAGESTATE_H
#define SETIMAGESTATE_H

#include <ctrvisual/state/scrollbasestate.h>
#include <QGraphicsPixmapItem>

class RasterPixmapItem;
class HandleDirectionItem;

class SetImageState : public ScrollBaseState
{
    Q_OBJECT
public:
    SetImageState();
    SetImageState(QPointF pos, double direction);
    ~SetImageState();
    void setBoundingArea(const QPolygonF & boundingArea);
    void init(QGraphicsScene * scene, QGraphicsView * view, const int * zoom, const double * scale, double frameCoef, uint visualizerId);
    bool mousePressEvent(QMouseEvent* e, QPointF scenePos);
    bool mouseReleaseEvent(QMouseEvent *e, QPointF scenePos);
    bool mouseMoveEvent(QMouseEvent* e, QPointF scenePos);
    bool mouseDoubleClickEvent(QMouseEvent *e, QPointF scenePos);
    //bool wheelEvent(QWheelEvent* e, QPointF scenePos);
    bool keyPressEvent(QKeyEvent * e);
    void statePushedToStack() {}
    void statePoppedFromStack() {}
    QString stateName() {return QString("SetImageState");}
    QPointF getCurrentWgsPos();
    double getCurrentDirection();

public slots:
    void handleDirectionReplaced(QPointF pos, double direction);

signals:
    void signalCreated(QPointF pos, double direction);
    void signalAbort();

protected:
    void createItem();
    void finishCreatedItem();

    QPointF _pos;
    double _direction = 0;
    RasterPixmapItem * _rasterPixmapItem = nullptr;
    HandleDirectionItem * _directionItem = nullptr;
    QGraphicsLineItem * _line = nullptr;
    bool _showDirection = false;
    QPolygonF _boundingArea;
    
};

class RasterPixmapItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    RasterPixmapItem();
    virtual ~RasterPixmapItem();
    //void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

signals:
    //void rasterReplaced();

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

};

#endif
