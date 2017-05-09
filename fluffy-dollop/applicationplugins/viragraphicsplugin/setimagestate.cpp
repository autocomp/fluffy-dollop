#include "setimagestate.h"
#include "handledirectionitem.h"
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
    _view->setCursor(Qt::ArrowCursor);
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
    //connect(_rasterPixmapItem, SIGNAL(rasterReplaced()), this, SIGNAL(rasterReplaced()));

    QPen pen;
    QColor colPen;
    colPen.setRgba(qRgba(0,0,0,255)); // qRgba(0x09,0x61,0xD3,150));
    pen.setColor(colPen);
    pen.setWidth(2);
    pen.setCosmetic(true);
    pen.setJoinStyle(Qt::RoundJoin);

    _direction -= 90; //! <----------- !!!
    double r(45);
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

void SetImageState::finishCreatedItem()
{
    if( _pos == QPointF() )
        emit signalAbort();
    else
        emit signalCreated(_pos, _direction);
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
    if(_pos == QPointF())
        _view->setCursor(QCursor(QPixmap(":/img/cursor_foto.png"), 0, 0));
    else
        _view->setCursor(Qt::ArrowCursor);

    return ScrollBaseState::mouseMoveEvent(e, scenePos);
}

bool SetImageState::mouseDoubleClickEvent(QMouseEvent *e, QPointF scenePos)
{
    finishCreatedItem();
}

bool SetImageState::keyPressEvent(QKeyEvent * e)
{
    switch(e->key())
    {
    case Qt::Key_Escape :
        emit signalAbort();
    break;
    case Qt::Key_Enter :
    case Qt::Key_Return :
    case Qt::Key_Space :
        finishCreatedItem();
    break;
    }

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
    //emit rasterReplaced();
    if(_line)
        _line->setLine(QLineF(QPointF(0,0), pos));
}

//-------------------------------------------------------------------

RasterPixmapItem::RasterPixmapItem()
{
//    QPixmap pm(":/img/flag.png");
//    setPixmap(pm);
//    setOffset(-4., -38);

    setPixmap(QPixmap(":/img/foto.png"));
    setOffset(-25, -22);

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
//    if(change == ItemPositionChange)
//        emit rasterReplaced();

    return QGraphicsPixmapItem::itemChange(change, value);
}

























