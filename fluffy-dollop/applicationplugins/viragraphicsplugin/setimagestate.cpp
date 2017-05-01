#include "setimagestate.h"
#include <math.h>
#include <QDebug>

SetImageState::SetImageState()
{
}

SetImageState::SetImageState(QPointF pos, double direction)
    : _pos(pos)
    , _direction(direction)
{
}

SetImageState::~SetImageState()
{
    delete _rasterPixmapItem;
}

void SetImageState::init(QGraphicsScene *scene, QGraphicsView *view, const int *zoom, const double *scale, double frameCoef, uint visualizerId)
{
    ScrollBaseState::init(scene, view, zoom, scale, frameCoef, visualizerId);
    setActiveForScene(true);

    if( _pos == QPointF() )
    {

    }
    else
    {
        createItem();
    }
}

void SetImageState::createItem()
{
    _rasterPixmapItem = new RasterPixmapItem();
    _rasterPixmapItem->setPos(_pos);
    _scene->addItem(_rasterPixmapItem);
    connect(_rasterPixmapItem, SIGNAL(rasterReplaced()), this, SIGNAL(rasterReplaced()));

    QPen pen;
    QColor colPen;
    colPen.setRgba(qRgba(0x09,0x61,0xD3,150));
    pen.setColor(colPen);
    pen.setWidth(2);
    pen.setCosmetic(true);
    pen.setJoinStyle(Qt::RoundJoin);

    _direction -= 90; //! <----------- !!!
    double r(75);
    QPointF pos(r * cos( _direction * M_PI / 180 ), r * sin( _direction * M_PI / 180 ) );
    QLineF line(QPointF(0,0), pos);
    _direction = line.angle();

    _line = new QGraphicsLineItem(_rasterPixmapItem);
    _line->setFlag(QGraphicsItem::ItemStacksBehindParent, true);
    _line->setLine(line);
    _line->setPen(pen);

    _directionItem = new HandleDirectionItem(_rasterPixmapItem, true);
    _directionItem->setPos(pos);
    connect(_directionItem, SIGNAL(handleDirectionReplaced(QPointF,double)), this, SLOT(handleDirectionReplaced(QPointF,double)));
}

bool SetImageState::mousePressEvent(QMouseEvent *e, QPointF scenePos)
{
    if( _pos == QPointF() )
    {
        _pos = scenePos;
        createItem();
    }

    return ScrollBaseState::mousePressEvent(e, scenePos);
}

bool SetImageState::mouseReleaseEvent(QMouseEvent *e, QPointF scenePos)
{

    return ScrollBaseState::mouseReleaseEvent(e, scenePos);
}

bool SetImageState::mouseMoveEvent(QMouseEvent *e, QPointF scenePos)
{

    return ScrollBaseState::mouseMoveEvent(e, scenePos);
}

bool SetImageState::keyPressEvent(QKeyEvent * e)
{
//    qDebug() << "e->key():" << e->key();

//    if(e->key() == Qt::Key_Escape)
//    {
//        if(_fogItem)
//        {
//            delete _fogItem;
//            _fogItem = 0;
//        }
//        emit signalAreaChoiced();
//    }
//    else if(e->key() == 16777220) //Qt::Key_Enter)
//    {
//        if(_fogItem)
//            if(_fogItem->areaChoiced())
//                emit signalAreaChoiced();
//    }

    return ScrollBaseState::keyPressEvent(e);
}

QPointF SetImageState::getCurrentWgsPos()
{
    bool succes;
    QPointF wgsPos(_rasterPixmapItem->scenePos());
    convertSceneToEpsg(4326, wgsPos, succes);
    return wgsPos;
}

double SetImageState::getCurrentDirection()
{
    return _direction;
}

void SetImageState::handleDirectionReplaced(QPointF pos, double direction)
{
    _direction = direction;
    emit rasterReplaced();
    if(_line)
        _line->setLine(QLineF(QPointF(0,0), pos));
}

//-------------------------------------------------------------------

RasterPixmapItem::RasterPixmapItem()
{
//    QPixmap pm(":/img/flag.png");
//    setPixmap(pm);
//    setOffset(-4., -38);

    setPixmap(QPixmap(":/img/mark_tr.png"));
    setOffset(-6, -43);

    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

    setZValue(1000000);
    setCursor(QCursor(Qt::CrossCursor));
}

RasterPixmapItem::~RasterPixmapItem()
{
}

QVariant RasterPixmapItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemPositionChange)
        emit rasterReplaced();

    return QGraphicsPixmapItem::itemChange(change, value);
}

//-------------------------------------------------------------------

HandleDirectionItem::HandleDirectionItem(QGraphicsItem * parent, bool isMovable)
    : QGraphicsPixmapItem(parent)
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, isMovable);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    setFlag(QGraphicsItem::ItemStacksBehindParent, true);

    if(isMovable)
        setCursor(QCursor(Qt::CrossCursor));

    QPixmap pm(11,11);
    pm.fill(Qt::transparent);

    QPen pen;
    QColor colPen;
    colPen.setRgba(qRgba(0x09,0x61,0xD3,150));
    pen.setColor(colPen);
    pen.setWidth(2);
    pen.setCosmetic(true);
    pen.setJoinStyle(Qt::RoundJoin);

    QBrush brush;
    QColor colBr;
    colBr.setRgba(qRgba(0xF3,0xF0,0x0F,255));
    brush.setColor(colBr);
    brush.setStyle(Qt::SolidPattern);

    QPainter pr(&pm);
    pr.setPen(pen);
    pr.setBrush(brush);
    pr.drawEllipse(1, 1, pm.width()-2, pm.height()-2);

    setPixmap(pm);
    setOffset(-5, -5);

}

HandleDirectionItem::~HandleDirectionItem()
{
}

QVariant HandleDirectionItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemPositionChange)
    {
        QPointF newPos = value.toPointF();
        QLineF line(QPointF(0,0), newPos);
        double angle = line.angle();
        qDebug() << "angle :" << angle;
        double r(75);
        QPointF pos(r * cos( -angle * M_PI / 180 ), r * sin( -angle * M_PI / 180 ) );
        emit handleDirectionReplaced(pos, angle);
        return QGraphicsPixmapItem::itemChange(change, pos);
    }
    else
        return QGraphicsPixmapItem::itemChange(change, value);
}























