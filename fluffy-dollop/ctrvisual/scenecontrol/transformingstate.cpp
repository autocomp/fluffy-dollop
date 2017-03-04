#include "transformingstate.h"
#include <QDebug>
#include <QMessageBox>
#include<QGraphicsLineItem>
#include<QPen>

using namespace visualize_system;

TransformingState::TransformingState(uint rasterId, const QPixmap &pixmap, int zLevel, QPolygonF sceneVertex, int transparancy)
    : RematchingState(rasterId, sceneVertex)
    , _pixmap(pixmap)
    , _zLevel(zLevel)
    , _pixmapItem(0)
    , _scaleX(1)
    , _scaleY(1)
    , _angel(0)
    , _scaleAfterResizeX(1)
    , _scaleAfterResizeY(1)
    , _transparancy(1 - transparancy / 100.)
{
//    QPixmap pm(":/195_turning_point.png");
    _turningCursor = QCursor(QPixmap(":/img/icons_31_5.png")); //pm, pm.width()/2, pm.height()/2);

//    pm = QPixmap(":/202_cur_cross.png");
    _crossCursor = QCursor(QPixmap(":/img/202_cur_cross.png")); //pm, pm.width()/2, pm.height()/2);
}

TransformingState::~TransformingState()
{
    delete _pixmapItem;

//    foreach(QGraphicsLineItem* item, _lineItems)
//        delete item;
}

void TransformingState::init(QGraphicsScene * scene, QGraphicsView * view, const int * zoom, const double * scale, double frameCoef, uint visualizerId)
{
    StateObject::init(scene, view, zoom, scale, frameCoef, visualizerId);

    const double K(pow(2,(*_zoom)-1));
    _scaleX = 1/K;
    _scaleY = 1/K;

    QPixmap pixmap(_pixmap.size());
    pixmap.fill(Qt::transparent);
    QPainter pr(&pixmap);
    pr.save();
    pr.setCompositionMode(QPainter::CompositionMode_SourceOver);
    pr.setOpacity(_transparancy);
    pr.drawPixmap(0,0,_pixmap);
    pr.restore();

    _pixmapItem = new QGraphicsPixmapItem(pixmap);
    _pixmapItem->setTransform(QTransform().scale(_scaleX, _scaleY));
    _pixmapItem->setZValue(1000000);
    _pixmapItem->setPos(_sceneVertex.boundingRect().topLeft());
    _scene->addItem(_pixmapItem);

    _vertexOnItem;
    foreach(QPointF p, _sceneVertex)
        _vertexOnItem.append(_pixmapItem->mapFromScene(p));

    QGraphicsPolygonItem * polItem = new QGraphicsPolygonItem(QPolygonF(_vertexOnItem.boundingRect()), _pixmapItem);
    QColor colPen;
    colPen.setRgba(qRgba(0,0,255,100));
    QPen pen(colPen);
    pen.setWidth(2);
    pen.setCosmetic(true);
    polItem->setPen(pen);
    polItem->setBrush(QBrush(Qt::NoBrush));
    polItem->setZValue(1000000);

    moveMarks(true);
}

void TransformingState::setTrancparency(int transparancy)
{
    _transparancy = 1 - transparancy / 100.;

    if(_pixmapItem)
    {
        QPixmap pixmap(_pixmap.size());
        pixmap.fill(Qt::transparent);
        QPainter pr(&pixmap);
        pr.save();
        pr.setCompositionMode(QPainter::CompositionMode_SourceOver);
        pr.setOpacity(_transparancy);
        pr.drawPixmap(0,0,_pixmap);
        pr.restore();

        _pixmapItem->setPixmap(pixmap);
    }
}

bool TransformingState::mouseMoveEvent(QMouseEvent* e, QPointF scenePos)
{
    if(_state == DONT_EDIT)
    {
        const double C(5 * 1/(pow(2,(*_zoom)-1)));
        int state(0);
        foreach(global_state::Mark* mark, markList)
            if(mark->check(C, scenePos))
            {
                state = mark->type();
                break;
            }

        switch(state)
        {
        case TURN:
            _view->setCursor(_turningCursor);
            break;
        case SET_ORIGINAL_SIZE:
            _view->setCursor(QCursor(Qt::PointingHandCursor));
            break;
        case RESIZING:
            _view->setCursor(QCursor(Qt::SizeFDiagCursor));
            break;
        case PROPORTIONAL_RESIZING:
            _view->setCursor(QCursor(Qt::SizeVerCursor));
            break;
        default:
        {
            if(_pixmapItem->contains(_pixmapItem->mapFromScene(scenePos)))
            {
                _view->setCursor(_crossCursor);
            }
            else
            {
                _view->setCursor(Qt::ArrowCursor);
                return ScrollBaseState::mouseMoveEvent(e, scenePos);
            }
        }
        }
    }
    else if(_state == MOVING)
    {
        const QPointF offset(scenePos - _lastMousePos);
        _pixmapItem->moveBy(offset.x(), offset.y());
        _isChanged = true;
    }
    else if(_state == RESIZING)
    {
        QSize originalSize = _pixmap.size();
        double dx, dy;
        if(_angel == 0)
        {
            dx = scenePos.x() - _pixmapItem->scenePos().x();
            dy = scenePos.y() - _pixmapItem->scenePos().y();
        }
        else
        {
            double fi, r;
            convertDecartToPolar(scenePos - _pixmapItem->scenePos(), r, fi);
            QPointF currScenePosTransform = convertPolarToDecar(r, normalizeDegree(fi-_angel), _pixmapItem->scenePos());

            dx = currScenePosTransform.x() - _pixmapItem->scenePos().x();
            dy = currScenePosTransform.y() - _pixmapItem->scenePos().y();
        }

        _pixmapItem->setTransform(QTransform().scale(1/_scaleX, 1/_scaleY));
        if(dx/originalSize.width() > 0)
            _scaleX = dx/originalSize.width();
        if(dy/originalSize.height() > 0)
            _scaleY = dy/originalSize.height();
        _pixmapItem->setTransform(QTransform().scale(_scaleX, _scaleY));
        _isChanged = true;
    }
    else if(_state == TURN)
    {
        double _r, _fi;
        convertDecartToPolar(scenePos - markList[0]->scenePos(), _r, _fi);
        _angel = normalizeDegree(_fi+90);

//        if(e->modifiers() & Qt::ShiftModifier)
//        {
//            int angel45 = ((int)angel) / 45;
//            int modAngel45 = ((int)angel) % 45;
//            if(modAngel45 > 45/2)
//            {
//                if(_angel == normalizeDegree((angel45 + 1) * 45))
//                    break;
//                _angel = normalizeDegree((angel45 + 1) * 45);
//            }
//            else
//            {
//                if( ((int)_angel) == angel45 * 45)
//                    break;
//                _angel = angel45 * 45;
//            }
//        }

        _pixmapItem->resetTransform();
        QPointF c = markList[0]->scenePos() - scenePos;
        double dx = c.x()*_scaleX;
        double dy = c.y()*_scaleY;
        _pixmapItem->setTransform(QTransform().translate(dx,dy).rotate(_angel).translate(-dx,-dy).scale(_scaleX, _scaleY));
        _isChanged = true;
    }
    else if(_state == PROPORTIONAL_RESIZING)
    {
        double d;
//        if(_angel == 0)
//            d = (_pixmapItem->scenePos().y() - _originalCenter.y()) * 2;
//        else
        {
            double fi, r;
            convertDecartToPolar(scenePos - _originalCenter, r, fi);
            QPointF currScenePosTransform = convertPolarToDecar(r, normalizeDegree(fi-_angel), _originalCenter);

            d = (currScenePosTransform.y() - _originalCenter.y()) * 2;
        }

        d /= (_pixmap.height()*_scaleAfterResizeY);
        if(d*_scaleAfterResizeX > 0)
            _scaleX = d*_scaleAfterResizeX;
        if(d*_scaleAfterResizeY > 0)
            _scaleY = d*_scaleAfterResizeY;

        _pixmapItem->resetTransform();
        QPointF c = markList[0]->scenePos() - _pixmapItem->scenePos();
        double dx = c.x()*_scaleX;
        double dy = c.y()*_scaleY;
        _pixmapItem->setTransform(QTransform().translate(dx,dy).rotate(_angel).translate(-dx,-dy).scale(_scaleX, _scaleY));
        QPointF offset(_originalCenter.x() - dx, _originalCenter.y() - dy);
        _pixmapItem->setPos(offset);
        _isChanged = true;
    }

    _lastMousePos = scenePos;

    return false;
}

bool TransformingState::mouseReleaseEvent(QMouseEvent *e, QPointF scenePos)
{
    if(_state == DONT_EDIT)
    {

        return ScrollBaseState::mouseReleaseEvent(e, scenePos);
    }
    else
    {
        _state = DONT_EDIT;
    }
}

bool TransformingState::mousePressEvent(QMouseEvent* e, QPointF scenePos)
{
    if(e->button() == Qt::LeftButton)
        if(_state == DONT_EDIT)
        {
            _originalCenter = markList[0]->scenePos();
            const double C(5 * 1/(pow(2,(*_zoom)-1)));
            int state(0);
            foreach(global_state::Mark* mark, markList)
                if(mark->check(C, scenePos))
                {
                    state = mark->type();
                    break;
                }

            switch(state)
            {
            case RESIZING:
                _state = RESIZING;
                break;
            case TURN:
                _state = TURN;
                break;
            case PROPORTIONAL_RESIZING:
                _state = PROPORTIONAL_RESIZING;
                _scaleAfterResizeX = _scaleX;
                _scaleAfterResizeY = _scaleY;
                break;
            default:
            {
                if(_pixmapItem->contains(_pixmapItem->mapFromScene(scenePos)))
                {
                    _view->setCursor(_crossCursor);
                    _state = MOVING;
                    return false;
                }
                else
                {
                    _view->setCursor(Qt::ArrowCursor);
                    return ScrollBaseState::mousePressEvent(e, scenePos);
                }
            }
            }
        }
    return false;
}

bool TransformingState::mouseDoubleClickEvent(QMouseEvent *e, QPointF scenePos)
{
    return false;
}

bool TransformingState::keyPressEvent(QKeyEvent * e)
{
    switch(e->key())
    {
//    case Qt::Key_Escape:
//    {
//        emit signalTransformFinished();
//        return false;
//    }

//    case Qt::Key_Backspace:{

    }return false;

//    case Qt::Key_Space:


    return true;
}

QString TransformingState::stateName()
{
    return QString("TransformingState");
}

void TransformingState::statePushedToStack()
{
}

void TransformingState::statePoppedFromStack()
{
}

void TransformingState::moveMarks(bool createMarks)
{
    const int C(10);
    QRectF r(_pixmapItem->boundingRect());
    double w = r.width();
    double h = r.height();
    if(createMarks)
    {
        QColor colPen;
        colPen.setRgba(qRgba(0,0,255,100));

        global_state::Mark* pEllipseItem = new global_state::Mark(SET_ORIGINAL_SIZE, _pixmapItem);
        pEllipseItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        pEllipseItem->setBrush(QBrush(QBrush(colPen)));
        pEllipseItem->setPos(QPointF(w/2,h/2));
        //pEllipseItem->setToolTip(QString::fromUtf8("Установить оригинальный размер"));
        pEllipseItem->setRect(-C/2,-C/2,C,C);
        pEllipseItem->setZValue(0);
        pEllipseItem->hide();
        markList.append(pEllipseItem);

        pEllipseItem = new global_state::Mark(PROPORTIONAL_RESIZING, _pixmapItem);
        pEllipseItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        pEllipseItem->setBrush(QBrush(QBrush(colPen)));
        pEllipseItem->setPos(QPointF(w/2,h));
        pEllipseItem->setToolTip(QString::fromUtf8("Изменить размер пропорционально"));
        pEllipseItem->setRect(-C/2,-C/2,C,C);
        pEllipseItem->setZValue(1000);
        markList.append(pEllipseItem);

        pEllipseItem = new global_state::Mark(TURN, _pixmapItem);
        pEllipseItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        pEllipseItem->setBrush(QBrush(QBrush(colPen)));
        pEllipseItem->setPos(QPointF(w/2,0));
        pEllipseItem->setToolTip(QString::fromUtf8("Повернуть"));
        pEllipseItem->setRect(-C/2,-C/2,C,C);
        pEllipseItem->setZValue(1000);
        markList.append(pEllipseItem);

        pEllipseItem = new global_state::Mark(RESIZING, _pixmapItem);
        pEllipseItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        pEllipseItem->setBrush(QBrush(QBrush(colPen)));
        pEllipseItem->setPos(QPointF(w,h));
        pEllipseItem->setToolTip(QString::fromUtf8("Изменить размер"));
        pEllipseItem->setRect(-C/2,-C/2,C,C);
        pEllipseItem->setZValue(1000);
        markList.append(pEllipseItem);


    }
}

QPolygonF TransformingState::getCurrentVertex()
{
    QPolygonF sceneVertex;
    foreach(QPointF p, _vertexOnItem)
        sceneVertex.append(_pixmapItem->mapToScene(p));

    return sceneVertex;
}










