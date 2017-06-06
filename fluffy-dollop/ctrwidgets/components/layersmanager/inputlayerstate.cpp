#include "inputlayerstate.h"
#include <QDebug>
#include <QMessageBox>
#include<QGraphicsLineItem>
#include<QPen>

InputLayerState::InputLayerState(const QPixmap &pixmap)
    : _pixmap(pixmap)
{
//    QPixmap pm(":/195_turning_point.png");
    _turningCursor = QCursor(QPixmap(":/img/icons_31_5.png")); //pm, pm.width()/2, pm.height()/2);

//    pm = QPixmap(":/202_cur_cross.png");
    _crossCursor = QCursor(QPixmap(":/img/202_cur_cross.png")); //pm, pm.width()/2, pm.height()/2);
}

InputLayerState::~InputLayerState()
{
    delete _pixmapItem;
}

void InputLayerState::init(QGraphicsScene * scene, QGraphicsView * view, const int * zoom, const double * scale, double frameCoef, uint visualizerId)
{
    StateObject::init(scene, view, zoom, scale, frameCoef, visualizerId);

//    const double K(pow(2,(*_zoom)-1));
    _scaleX = 1; // 1/K;
    _scaleY = 1; // 1/K;
    _angel = 0;

    _pixmapItem = new QGraphicsPixmapItem(_pixmap);
//    _pixmapItem->setOpacity(0.5);
//    _pixmapItem->setTransform(QTransform().scale(_scaleX, _scaleY));
    _pixmapItem->setZValue(1000000);
//    _pixmapItem->setPos(_sceneVertex.boundingRect().topLeft());
    _scene->addItem(_pixmapItem);

    QPolygonF vertex;
    vertex << QPointF(0,0) << QPointF(_pixmap.width(),0) << QPointF(_pixmap.width(),_pixmap.height()) << QPointF(0,_pixmap.height());
    _polygonItem = new QGraphicsPolygonItem(vertex, _pixmapItem);

    QPen pen(Qt::blue);
    pen.setWidth(3);
    pen.setCosmetic(true);
    _polygonItem->setPen(pen);

    moveMarks(true);
}

bool InputLayerState::mouseMoveEvent(QMouseEvent* e, QPointF scenePos)
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
        case PROPORTIONAL_RESIZING:
            _view->setCursor(QCursor(Qt::SizeVerCursor));
            break;
        default:
        {
            if((e->modifiers() & Qt::ControlModifier) && _polygonItem->contains(_pixmapItem->mapFromScene(scenePos))) // if(_pixmapItem->contains(_pixmapItem->mapFromScene(scenePos)))
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
    else if(_state == PROPORTIONAL_RESIZING)
    {
//        double d;
////        if(_angel == 0)
////            d = (_pixmapItem->scenePos().y() - _originalCenter.y()) * 2;
////        else
//        {
//            double fi, r;
//            convertDecartToPolar(scenePos - _originalCenter, r, fi);
//            QPointF currScenePosTransform = convertPolarToDecar(r, normalizeDegree(fi-_angel), _originalCenter);

//            d = (currScenePosTransform.y() - _originalCenter.y()) * 2;
//        }

//        d /= (_pixmap.height()*_scaleAfterResizeY);
//        if(d*_scaleAfterResizeX > 0)
//            _scaleX = d*_scaleAfterResizeX;
//        if(d*_scaleAfterResizeY > 0)
//            _scaleY = d*_scaleAfterResizeY;

//        _pixmapItem->resetTransform();
//        QPointF c = markList[0]->scenePos() - _pixmapItem->scenePos();
//        double dx = c.x()*_scaleX;
//        double dy = c.y()*_scaleY;
//        _pixmapItem->setTransform(QTransform().translate(dx,dy).rotate(_angel).translate(-dx,-dy).scale(_scaleX, _scaleY));
//        QPointF offset(_originalCenter.x() - dx, _originalCenter.y() - dy);
//        _pixmapItem->setPos(offset);
//        _isChanged = true;
    }

    _lastMousePos = scenePos;

    return false;
}

bool InputLayerState::mouseReleaseEvent(QMouseEvent *e, QPointF scenePos)
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

bool InputLayerState::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key_Plus : {
        _pixmapItem->resetTransform();
        _scaleX += 0.001;
        _scaleY += 0.001;
        _pixmapItem->setTransform(QTransform().scale(_scaleX, _scaleY));
        _isChanged = true;
    }break;
    case Qt::Key_Minus : {
        _pixmapItem->resetTransform();
        _scaleX -= 0.001;
        _scaleY -= 0.001;
        _pixmapItem->setTransform(QTransform().scale(_scaleX, _scaleY));
        _isChanged = true;
    }break;
    case Qt::Key_Escape : {
        emit signalStateAborted();
    }break;
    }
}

bool InputLayerState::mousePressEvent(QMouseEvent* e, QPointF scenePos)
{
    if(e->button() == Qt::LeftButton)
    {
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
            case PROPORTIONAL_RESIZING:
//                _state = PROPORTIONAL_RESIZING;
//                _scaleAfterResizeX = _scaleX;
//                _scaleAfterResizeY = _scaleY;
                break;
            default:
            {
                if((e->modifiers() & Qt::ControlModifier) && _polygonItem->contains(_pixmapItem->mapFromScene(scenePos))) // if(_pixmapItem->contains(_pixmapItem->mapFromScene(scenePos)))
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
    }
    else
    {
        qDebug() << QString::number(_pixmapItem->scenePos().x(), 'f', 10) << QString::number(_pixmapItem->scenePos().y(), 'f', 10) << QString::number(_scaleX, 'f', 10);

    }
    return false;
}

void InputLayerState::moveMarks(bool createMarks)
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
        pEllipseItem->hide();
        markList.append(pEllipseItem);

        pEllipseItem = new global_state::Mark(RESIZING, _pixmapItem);
        pEllipseItem->setFlag(QGraphicsItem::ItemIgnoresTransformations);
        pEllipseItem->setBrush(QBrush(QBrush(colPen)));
        pEllipseItem->setPos(QPointF(w,h));
        pEllipseItem->setToolTip(QString::fromUtf8("Изменить размер"));
        pEllipseItem->setRect(-C/2,-C/2,C,C);
        pEllipseItem->setZValue(1000);
        pEllipseItem->hide();
        markList.append(pEllipseItem);
    }
}

QPointF InputLayerState::convertPolarToDecar(double const & r, double const & fi, const QPointF center)const
{
    return QPointF( center.x() + r * cos( fi * M_PI / 180 ), center.y() + r * sin( fi * M_PI / 180 ) );
}

void InputLayerState::convertDecartToPolar(QPointF const & p, double& r, double& fi)const
{
    r = sqrt( p.x() * p.x() +  p.y() * p.y() );
    fi = atan2(p.y(), p.x()) * 180 / M_PI;
    if (fi < 0 )
        fi += 360;
}

double InputLayerState::getLenght(const QPointF& p1, const QPointF& p2)const
{
    return sqrt( (p1.x() - p2.x()) * (p1.x() - p2.x()) +  (p1.y() - p2.y()) * (p1.y() - p2.y()) );
}

int InputLayerState::normalizeDegree(int degree)
{
    if (degree < 0)
        degree += 360;
    else
        if (degree > 359)
            degree -= 360;

    return degree;
}

bool InputLayerState::check(double coef, QPointF p1, QPointF p2)
{
    return sqrt( (p1.x() - p2.x()) * (p1.x() - p2.x()) +  (p1.y() - p2.y()) * (p1.y() - p2.y()) ) <= coef;
}






