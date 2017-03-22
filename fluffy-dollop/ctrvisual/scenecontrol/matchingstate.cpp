#include "matchingstate.h"
#include <QDebug>
#include <QMessageBox>
#include <QGraphicsLineItem>
#include <QPen>
#include <math.h>


using namespace visualize_system;

MatchingState::MatchingState(uint rasterId, QPolygonF sceneVertex)
    : RematchingState(rasterId, sceneVertex)
{
}

MatchingState::MatchingState(uint rasterId, QPolygonF sceneVertex, QList<MatchingPoint> initPoints)
    : RematchingState(rasterId, sceneVertex)
    , _initPoints(initPoints)
{
}

MatchingState::~MatchingState()
{
    foreach(MatchingItem matchingItem, _matchingItems)
        if(matchingItem.itemOnRaster != _currentMatchingItem.itemOnRaster)
        {
            delete matchingItem.itemOnRaster;
            delete matchingItem.itemOnMap;
            delete matchingItem.lineBetween;
        }

    delete _currentMatchingItem.itemOnRaster;
    delete _currentMatchingItem.itemOnMap;
    delete _currentMatchingItem.lineBetween;
}

void MatchingState::init(QGraphicsScene * scene, QGraphicsView * view, const int * zoom, const double * scale, double frameCoef, uint visualizerId)
{
    QPixmap emptyPm(3,3);
    emptyPm.fill(Qt::transparent);
    _emptyCursor = QCursor(emptyPm);

    //QPixmap pm(":/img/icons_31_3.png");
    //QCursor(pm, pm.width()/2+1, pm.height()/2+1);
    _checkCursor = QCursor(QPixmap(":/img/icons_31_3.png"), 0, 0);
    _setPointCursorOnRaster = QCursor(QPixmap(":/img/icons_31_1.png"), 0, 0);
    _setPointCursorOnMap = QCursor(QPixmap(":/img/icons_31_2.png"), 0, 0);

    StateObject::init(scene, view, zoom, scale, frameCoef, visualizerId);

    if(_initPoints.isEmpty() == false)
    {
        QPen pen(Qt::lightGray);
        pen.setCosmetic(true);
        pen.setWidth(1);
        foreach(MatchingPoint mPoint, _initPoints)
        {
            _currentMatchingItem.clear();
            _currentMatchingItem.isSelected = false;
            _currentMatchingItem.itemOnRaster = createPoint(mPoint.onRaster, true);

            _currentMatchingItem.lineBetween = new QGraphicsLineItem(QLineF(mPoint.onRaster, mPoint.onMap));
            _currentMatchingItem.lineBetween->setPen(pen);
            _currentMatchingItem.lineBetween->setZValue(1000000);
            _scene->addItem(_currentMatchingItem.lineBetween);

            _currentMatchingItem.itemOnMap = createPoint(mPoint.onMap, false);
            _matchingItems.append(_currentMatchingItem);
        }
        pen.setColor(Qt::green);
        _currentMatchingItem.isSelected = true;
        _currentMatchingItem.lineBetween->setPen(pen);

        _initPoints.clear();
    }

    _state = SET_POINT_ON_RASTER;
    _view->setCursor(_setPointCursorOnRaster);

    emit signalSetTransparancy(20);
}

bool MatchingState::mouseMoveEvent(QMouseEvent* e, QPointF scenePos)
{
    switch(_state)
    {
    case SET_POINT_ON_RASTER:
    {
        bool checkOn(false);
        const double coeff(5 * 1/(pow(2,(*_zoom)-1)));
        for(auto it = _matchingItems.begin(); it != _matchingItems.end(); ++it)
        {
            if(check(coeff, scenePos, (*it).itemOnRaster->scenePos()))
            {
                _view->setCursor(_checkCursor); // курсор на перемещение точки
                checkOn = true;
                break;
            }
            if(check(coeff, scenePos, (*it).itemOnMap->scenePos()))
            {
                _view->setCursor(_checkCursor); // курсор на перемещение точки
                checkOn = true;
                break;
            }
        }

        if(checkOn == false)
        {
            if(_sceneVertex.containsPoint(scenePos, Qt::OddEvenFill))
            {
                _view->setCursor(_setPointCursorOnRaster); // курсор на установку точки на растре
            }
            else
            {
                _view->setCursor(QCursor(Qt::OpenHandCursor)); // курсор на установку точки на карте
            }
        }

    }break;
    case SET_POINT_ON_MAP:
    {
        _view->setCursor(_setPointCursorOnMap);

        if(_currentMatchingItem.lineBetween)
            _currentMatchingItem.lineBetween->setLine(QLineF(_currentMatchingItem.itemOnRaster->scenePos(), scenePos));

    }break;
    case MOVE_POINT_ON_RASTER:
    {
        _view->setCursor(_emptyCursor);
        _currentMatchingItem.itemOnRaster->setPos(scenePos);
        if(_currentMatchingItem.lineBetween)
            _currentMatchingItem.lineBetween->setLine(QLineF(scenePos, _currentMatchingItem.itemOnMap->scenePos()));

    }break;
    case MOVE_POINT_ON_MAP:
    {
        _view->setCursor(_emptyCursor);
        _currentMatchingItem.itemOnMap->setPos(scenePos);
        if(_currentMatchingItem.lineBetween)
            _currentMatchingItem.lineBetween->setLine(QLineF(_currentMatchingItem.itemOnRaster->scenePos(), scenePos));

    }break;
    case SCROLL_MAP:
        if(e)
        {
            _lastMousePos = scenePos;
            return ScrollBaseState::mouseMoveEvent(e, scenePos);
        }
        break;
    default:
    {
        _view->setCursor(QCursor(Qt::ForbiddenCursor));
        // return ScrollBaseState::mouseMoveEvent(e, scenePos);
    }
    }

    _lastMousePos = scenePos;

    return true;
}

bool MatchingState::mouseReleaseEvent(QMouseEvent *e, QPointF scenePos)
{
    switch(_state)
    {
    case MOVE_POINT_ON_RASTER:
    case MOVE_POINT_ON_MAP:
        _state = SET_POINT_ON_RASTER;
        _view->setCursor(_setPointCursorOnRaster);
        emit signalSetTransparancy(80);
        checkApply();
        mouseMoveEvent(0, scenePos);
        break;

    case SCROLL_MAP:
        _state = SET_POINT_ON_RASTER;
        return ScrollBaseState::mouseReleaseEvent(e, scenePos);
    }
    return false;
}

bool MatchingState::mousePressEvent(QMouseEvent* e, QPointF scenePos)
{
    if(e->button() == Qt::LeftButton)
        switch(_state)
        {
        case SET_POINT_ON_RASTER:
        {
            const double coeff(7 * 1/(pow(2,(*_zoom)-1)));
            for(auto it = _matchingItems.begin(); it != _matchingItems.end(); ++it)
            {
                if(check(coeff, scenePos, (*it).itemOnRaster->scenePos()))
                {
                    _currentMatchingItem = *it;
                    _state = MOVE_POINT_ON_RASTER;
                    emit signalSetTransparancy(20);
                    break;
                }
                if(check(coeff, scenePos, (*it).itemOnMap->scenePos()))
                {
                    _currentMatchingItem = *it;
                    _state = MOVE_POINT_ON_MAP;
                    emit signalSetTransparancy(80);
                    _view->setCursor(_setPointCursorOnMap);
                    break;
                }
            }

            if(_state == MOVE_POINT_ON_RASTER || _state == MOVE_POINT_ON_MAP)
            {
                QPen pen(Qt::lightGray);
                pen.setCosmetic(true);
                pen.setWidth(1);
                for(auto it = _matchingItems.begin(); it != _matchingItems.end(); ++it)
                {
                    (*it).isSelected = false;
                    (*it).lineBetween->setPen(pen);
                }

                pen.setColor(Qt::green);
                _currentMatchingItem.lineBetween->setPen(pen);
                _currentMatchingItem.isSelected = true;
                emit signalEnabledDeleteButton(true);
                return false;
            }

            if(_sceneVertex.containsPoint(scenePos, Qt::OddEvenFill))
            {
                QPen pen(Qt::lightGray);
                pen.setCosmetic(true);
                pen.setWidth(1);
                for(auto it = _matchingItems.begin(); it != _matchingItems.end(); ++it)
                {
                    (*it).isSelected = false;
                    (*it).lineBetween->setPen(pen);
                }
                _currentMatchingItem.clear();
                _currentMatchingItem.isSelected = true;
                _currentMatchingItem.itemOnRaster = createPoint(scenePos, true);

                pen.setColor(Qt::green);
                _currentMatchingItem.lineBetween = new QGraphicsLineItem(QLineF(scenePos, scenePos));
                _currentMatchingItem.lineBetween->setPen(pen);
                _currentMatchingItem.lineBetween->setZValue(1000000);
                _scene->addItem(_currentMatchingItem.lineBetween);

                emit signalSetTransparancy(80);
                _state = SET_POINT_ON_MAP;

                emit signalEnabledDeleteButton(true);
            }
            else
            {
                _state = SCROLL_MAP;
                return ScrollBaseState::mousePressEvent(e, scenePos);
            }
        }break;
        case SET_POINT_ON_MAP:
        {
            _currentMatchingItem.itemOnMap = createPoint(scenePos, false);
            _matchingItems.append(_currentMatchingItem);

            _state = SET_POINT_ON_RASTER;
            _view->setCursor(_setPointCursorOnRaster);
            emit signalSetTransparancy(20);
            mouseMoveEvent(0, scenePos);
            checkApply();
//            emit signalEnabledDeleteButton(false);
        }break;
        default:
        {
        }
        }

    return false;
}

bool MatchingState::mouseDoubleClickEvent(QMouseEvent *e, QPointF scenePos)
{
    return false;
}

bool MatchingState::keyPressEvent(QKeyEvent * e)
{
    switch(e->key())
    {
//    case Qt::Key_Escape:
//        emit signalMatchingFinished();
//        return false;

    case Qt::Key_Delete:
        deleteSelectedPoint();
        return false;
    }

    return true;
}

QList<MatchingState::MatchingPoint> MatchingState::getMatchingPoints()
{
    QList<MatchingState::MatchingPoint> list;
    foreach(MatchingItem matchingItem, _matchingItems)
        if(matchingItem.itemOnRaster && matchingItem.itemOnMap)
        {
            MatchingState::MatchingPoint matchingPoint;
            matchingPoint.onRaster = matchingItem.itemOnRaster->scenePos();
            matchingPoint.onMap = matchingItem.itemOnMap->scenePos();
            list.append(matchingPoint);
        }
    return list;
}

void MatchingState::rasterMatched()
{
    for(auto it = _matchingItems.begin(); it != _matchingItems.end(); ++it)
    {
        QGraphicsPixmapItem* itemOnRaster = (*it).itemOnRaster;
        QGraphicsPixmapItem* itemOnMap = (*it).itemOnMap;
        QGraphicsLineItem* lineBetween = (*it).lineBetween;
        itemOnRaster->setPos(itemOnMap->scenePos());
        lineBetween->setLine(QLineF(itemOnMap->scenePos(), itemOnMap->scenePos()));
    }
}

QGraphicsPixmapItem* MatchingState::createPoint(QPointF pos, bool onRaser)
{
    QPixmap pm(onRaser ? ":/img/icons_31_1b.png" : ":/img/icons_31_2b.png");
    QGraphicsPixmapItem * item = new QGraphicsPixmapItem(pm);
    item->setFlags(QGraphicsItem::ItemIgnoresTransformations);
    item->setPos(pos);
    item->setOffset(-pm.width()/2. , -pm.height()/2.);
    item->setZValue(onRaser ? 1000001 : 1000002);
    _scene->addItem(item);
    return item;
}

void MatchingState::deleteSelectedPoint()
{
    if(_currentMatchingItem.isSelected == true)
    {
        if(_currentMatchingItem.itemOnRaster)
            for(auto it = _matchingItems.begin(); it != _matchingItems.end(); ++it)
                if(_currentMatchingItem.itemOnRaster == (*it).itemOnRaster)
                {
                    delete (*it).itemOnRaster;
                    delete (*it).itemOnMap;
                    delete (*it).lineBetween;
                    _matchingItems.erase(it);

                    _currentMatchingItem.clear();
                    _state = SET_POINT_ON_RASTER;
                    _view->setCursor(_setPointCursorOnRaster);

                    emit signalEnabledDeleteButton(false);
                    mouseMoveEvent(0, _lastMousePos);
                    checkApply();
                    return;
                }

        delete _currentMatchingItem.itemOnRaster;
        delete _currentMatchingItem.itemOnMap;
        delete _currentMatchingItem.lineBetween;
        _currentMatchingItem.clear();
        _state = SET_POINT_ON_RASTER;
        _view->setCursor(_setPointCursorOnRaster);
        emit signalSetTransparancy(20);
        emit signalEnabledDeleteButton(false);
        mouseMoveEvent(0, _lastMousePos);
        checkApply();
    }
}

void MatchingState::checkApply()
{
    emit signalEnabledApplyButton( _matchingItems.size() >= 3 );
}

QString MatchingState::stateName()
{
    return QString("MatchingState");
}

void MatchingState::statePushedToStack()
{
}

void MatchingState::statePoppedFromStack()
{
}


