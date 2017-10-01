#include "transformingstate.h"
#include "transformingitem.h"
#include "handleitem.h"
#include "lineitem.h"
#include "fogitem.h"
#include <ctrwidgets/components/waitdialog/waitdialog.h>
#include <ctrcore/ctrcore/tempdircontroller.h>
#include <math.h>
#include <QMouseEvent>
#include <QPainter>
#include <QDir>
#include <QGraphicsSvgItem>
#include <QDomDocument>
#include <QDebug>

using namespace transform_state;

TransformingState::TransformingState(const QPixmap & pixmap, const QString &rasterFileBaseName, QPointF scenePos, int zValue, double originalScale)
    : _transformingItemType(TransformingItem::PixmapItem)
    , _pixmap(pixmap)
    , _rasterFileBaseName(rasterFileBaseName)
    , _scenePos(scenePos)
    , _originalScale(originalScale)
    , _zValue(zValue)
{
}

TransformingState::TransformingState(const QPixmap & pixmap, const QString &rasterFileBaseName, QPointF scenePos, double scW, double scH, double rotation, int zValue)
    : _transformingItemType(TransformingItem::PixmapItem)
    , _pixmap(pixmap)
    , _rasterFileBaseName(rasterFileBaseName)
    , _scenePos(scenePos)
    , _scW(scW)
    , _scH(scH)
    , _rotation(rotation)
    , _zValue(zValue)
{
}

TransformingState::TransformingState(const QPolygonF & polygon, const QRectF &bRectOnMapScene, const QTransform &transformer, int zValue)
    : _transformingItemType(TransformingItem::PolygonItem)
    , _polygon(polygon)
    , _bRectOnMapScene(bRectOnMapScene)
    , _transformer(transformer)
    , _scW(1)
    , _scH(1)
    , _rotation(0)
    , _zValue(zValue)
{
}

TransformingState::TransformingState(const QString &svgFilePath, QPointF scenePos, int zValue, double originalScale)
    : _transformingItemType(TransformingItem::SvgItem)
    , _svgFilePath(svgFilePath)
    , _scenePos(scenePos)
    , _originalScale(originalScale)
    , _rotation(0)
    , _zValue(zValue)
{
}

TransformingState::TransformingState(const QString &svgFilePath, QPointF scenePos, double scW, double scH, double rotation, int zValue)
    : _transformingItemType(TransformingItem::SvgItem)
    , _svgFilePath(svgFilePath)
    , _scenePos(scenePos)
    , _scW(scW)
    , _scH(scH)
    , _rotation(rotation)
    , _zValue(zValue)
    , _originalScale(1)
{
}

TransformingState::~TransformingState()
{
    delete _transformingItem;
}

void TransformingState::init(QGraphicsScene *scene, QGraphicsView *view, const int *zoom, const double *scale, double frameCoef, uint visualizerId)
{
    ScrollBaseState::init(scene, view, zoom, scale, frameCoef, visualizerId);
    setActiveForScene(true);

    QPointF handleItemTopLeftPos;
    QPointF handleItemTopRightPos;
    QPointF handleItemBottomRightPos;
    QPointF handleItemBottomLeftPos;
    QPointF handleItemTopCenterPos;
    QPointF handleItemBottomCenterPos;
    QPointF handleItemLeftCenterPos;
    QPointF handleItemRightCenterPos;
    QPointF handleItemAnchorPos;

    switch (_transformingItemType)
    {
    case TransformingItem::PolygonItem : {
        PolygonTransformingItem * transformingPolygonItem = new PolygonTransformingItem(*this);
        transformingPolygonItem->setPolygon(_polygon);

        _originW = transformingPolygonItem->boundingRect().width();
        _originH = transformingPolygonItem->boundingRect().height();
        _transformingItem = transformingPolygonItem;

        handleItemTopLeftPos = transformingPolygonItem->mapFromScene(_transformer.map(_bRectOnMapScene.topLeft()));
        handleItemTopRightPos = transformingPolygonItem->mapFromScene(_transformer.map(_bRectOnMapScene.topRight()));
        handleItemBottomRightPos = transformingPolygonItem->mapFromScene(_transformer.map(_bRectOnMapScene.bottomRight()));
        handleItemBottomLeftPos = transformingPolygonItem->mapFromScene(_transformer.map(_bRectOnMapScene.bottomLeft()));
        handleItemAnchorPos = transformingPolygonItem->mapFromScene(_transformer.map(_bRectOnMapScene.center()));
    }break;

    case TransformingItem::PixmapItem : {
        PixmapTransformingItem * transformingPixmapItem = new PixmapTransformingItem(*this);
        QPixmap pm(_pixmap.size());
        pm.fill(QColor(0,0,0,1));
        QPainter painter(&pm);
        painter.drawPixmap(0,0,_pixmap);
        transformingPixmapItem->setPixmap(pm);
        transformingPixmapItem->setPos(_scenePos);

        _originW = _pixmap.width();
        _originH = _pixmap.height();
        _transformingItem = transformingPixmapItem;

        handleItemTopLeftPos = QPointF(0,0);
        handleItemTopRightPos = QPointF(_originW,0);
        handleItemBottomRightPos = QPointF(_originW, _originH);
        handleItemBottomLeftPos = QPointF(0, _originH);
        handleItemTopCenterPos = QPointF(_originW/2.,0);
        handleItemBottomCenterPos = QPointF(_originW/2., _originH);
        handleItemLeftCenterPos = QPointF(0, _originH/2);
        handleItemRightCenterPos = QPointF(_originW, _originH/2);
        handleItemAnchorPos = QPointF(_originW/2., _originH/2.);
    }break;

    case TransformingItem::SvgItem : {
        SvgTransformingItem * svgItem = new SvgTransformingItem(_svgFilePath, *this);
        svgItem->setPos(_scenePos);

        _originW = svgItem->boundingRect().width();
        _originH = svgItem->boundingRect().height();
        _transformingItem = svgItem;

        handleItemTopLeftPos = QPointF(0,0);
        handleItemTopRightPos = QPointF(_originW,0);
        handleItemBottomRightPos = QPointF(_originW, _originH);
        handleItemBottomLeftPos = QPointF(0, _originH);
        handleItemTopCenterPos = QPointF(_originW/2.,0);
        handleItemBottomCenterPos = QPointF(_originW/2., _originH);
        handleItemLeftCenterPos = QPointF(0, _originH/2);
        handleItemRightCenterPos = QPointF(_originW, _originH/2);
        handleItemAnchorPos = QPointF(_originW/2., _originH/2.);
    }break;
    }

    _transformingItem->setZValue(_zValue);
    _scene->addItem(_transformingItem->castToGraphicsItem());

    _handleItemTopLeft = new HandleItem(*this, HandleType::TopLeft, _scene, _transformingItem->castToGraphicsItem());
    _handleItemTopLeft->setPos(handleItemTopLeftPos);
    _handleItems.append(_handleItemTopLeft);

    _handleItemTopRight = new HandleItem(*this, HandleType::TopRight, _scene, _transformingItem->castToGraphicsItem());
    _handleItemTopRight->setPos(handleItemTopRightPos);
    _handleItems.append(_handleItemTopRight);

    _handleItemBottomRight = new HandleItem(*this, HandleType::BottomRight, _scene, _transformingItem->castToGraphicsItem());
    _handleItemBottomRight->setPos(handleItemBottomRightPos);
    _handleItems.append(_handleItemBottomRight);

    _handleItemBottomLeft = new HandleItem(*this, HandleType::BottomLeft, _scene, _transformingItem->castToGraphicsItem());
    _handleItemBottomLeft->setPos(handleItemBottomLeftPos);
    _handleItems.append(_handleItemBottomLeft);

    _handleItemTopCenter = new HandleItem(*this, HandleType::TopCenter, _scene, _transformingItem->castToGraphicsItem());
    _handleItemTopCenter->setPos(handleItemTopCenterPos);
    _handleItems.append(_handleItemTopCenter);

    _handleItemBottomCenter = new HandleItem(*this, HandleType::BottomCenter, _scene, _transformingItem->castToGraphicsItem());
    _handleItemBottomCenter->setPos(handleItemBottomCenterPos);
    _handleItems.append(_handleItemBottomCenter);

    _handleItemLeftCenter = new HandleItem(*this, HandleType::LeftCenter, _scene, _transformingItem->castToGraphicsItem());
    _handleItemLeftCenter->setPos(handleItemLeftCenterPos);
    _handleItems.append(_handleItemLeftCenter);

    _handleItemRightCenter = new HandleItem(*this, HandleType::RightCenter, _scene, _transformingItem->castToGraphicsItem());
    _handleItemRightCenter->setPos(handleItemRightCenterPos);
    _handleItems.append(_handleItemRightCenter);

    QColor penColor(Qt::blue);
    penColor.setAlpha(100);
    QPen pen(penColor);
    pen.setCosmetic(true);
    pen.setWidth(2);

    if(_transformingItemType == TransformingItem::PolygonItem)
    {
        _stateMode = StateMode::ScrollMap;
    }
    else
    {
        _topLine = new LineItem(_transformingItem->castToGraphicsItem(), pen);
        _topLine->setLine(QLineF(_handleItemTopLeft->pos(), _handleItemTopRight->pos()));

        _bottomLine = new LineItem(_transformingItem->castToGraphicsItem(), pen);
        _bottomLine->setLine(QLineF(_handleItemBottomLeft->pos(), _handleItemBottomRight->pos()));

        _leftLine = new LineItem(_transformingItem->castToGraphicsItem(), pen);
        _leftLine->setLine(QLineF(_handleItemTopLeft->pos(), _handleItemBottomLeft->pos()));

        _rightLine = new LineItem(_transformingItem->castToGraphicsItem(), pen);
        _rightLine->setLine(QLineF(_handleItemTopRight->pos(), _handleItemBottomRight->pos()));
    }

    _handleItemAnchor = new AnchorHandleItem(*this, _scene, _transformingItem->castToGraphicsItem());
    _handleItemAnchor->setPos(handleItemAnchorPos);
    _handleItems.append(_handleItemAnchor);

    _handleItemRotater = new RotaterHandleItem(*this, _originH * 0.75, _scene, _transformingItem->castToGraphicsItem());
    _handleItemRotater->setPos(_handleItemAnchor->pos().x(), _handleItemAnchor->pos().y() - _handleItemRotater->deltaByVertical() ); // _handleItemRotater->setPos(_originW/2.,-_originH/4.);
    _handleItems.append(_handleItemRotater);

    if(_originalScale < 0)
    {
        _originalScale = 1; // = 1. / (pow(2,(*zoom)-1));
    }
    if(_scW < 0)
        _scW = _originalScale;
    if(_scH < 0)
        _scH = _originalScale;

    _transformingItem->castToGraphicsItem()->setTransform(QTransform().rotate(_rotation).scale(_scW, _scH));
    _transformingItem->castToGraphicsItem()->setPos(_handleItemAnchor->scenePos());
    _transformingItem->castToGraphicsItem()->setTransform(QTransform().rotate(_rotation).scale(_scW, _scH).translate(-_handleItemAnchor->x(), -_handleItemAnchor->y()));

    pen.setWidth(1);
    pen.setStyle(Qt::DashLine);

    QPen pen2(Qt::white);
    pen2.setCosmetic(true);
    pen2.setWidth(1);

    _rotaterLine = new RotaterLineItem(_transformingItem->castToGraphicsItem(), pen2, pen);
    _rotaterLine->setLine(_handleItemAnchor->pos(), _handleItemRotater->pos());

    setMode(StateMode::ScrollMap);

    QString filePath;
    if(_transformingItemType == TransformingItem::PixmapItem)
    {
        filePath = TempDirController::createTempDirForCurrentUser() + QDir::separator() + _rasterFileBaseName + ".tif";
        bool res = _pixmap.save(filePath, "TIF");
    }
    UndoAct undoAct;
    undoAct.filePath = filePath;
    undoAct.scenePos = _scenePos;
    undoAct.scaleW = _scW;
    undoAct.scaleH = _scH;
    _undoStack.push(undoAct);

    setHandlesVisible(false);

    //view->centerOn(_transformingItem->castToGraphicsItem());
}

void TransformingState::setMode(StateMode stateMode)
{
    if(stateMode == _stateMode)
        return;

    switch(_stateMode)
    {
    case StateMode::TransformImage : {
        QPointF scenePos =_transformingItem->mapToScene(QPointF(0,0));
        _transformingItem->setPos(scenePos);
        _handleItemAnchor->setPos(QPointF(0,0));
        _transformingItem->setTransform(QTransform().rotate(_rotation).scale(_scW, _scH).translate(-_handleItemAnchor->x(), -_handleItemAnchor->y()));
    }break;
    case StateMode::CropImage : {
        delete _fogItem;
        _fogItem = nullptr;

        _handleItemTopLeft->setPos(0,0);
        _handleItemTopRight->setPos(_originW,0);
        _handleItemBottomRight->setPos(_originW, _originH);
        _handleItemBottomLeft->setPos(0, _originH);
        _handleItemTopCenter->setPos(_originW/2.,0);
        _handleItemBottomCenter->setPos(_originW/2., _originH);
        _handleItemLeftCenter->setPos(0, _originH/2);
        _handleItemRightCenter->setPos(_originW, _originH/2);

        _handleItemRotater->setPos(_originW/2.,-_originH/4.);
        _handleItemAnchor->setPos(_originW/2., _originH/2.);

        if(_transformingItemType != TransformingItem::PolygonItem)
        {
            _topLine->setLine(QLineF(_handleItemTopLeft->pos(), _handleItemTopRight->pos()));
            _bottomLine->setLine(QLineF(_handleItemBottomLeft->pos(), _handleItemBottomRight->pos()));
            _leftLine->setLine(QLineF(_handleItemTopLeft->pos(), _handleItemBottomLeft->pos()));
            _rightLine->setLine(QLineF(_handleItemTopRight->pos(), _handleItemBottomRight->pos()));
        }
        _rotaterLine->setLine(_handleItemAnchor->pos(), _handleItemRotater->pos());
    }break;
    case StateMode::GetAreaOnImage : {
        if(_stateMode == StateMode::GetColorOnImage || _areaLineItems.size() < 3)
            clearAreaOnImage();
    }break;
    }

    // если были изменения - уложить в стек последнее

    _stateMode = stateMode;
    _transformingItem->setCursor(QCursor(Qt::ArrowCursor));

    switch(_stateMode)
    {
    case StateMode::ScrollMap : {
        setHandlesVisible(false);
    }break;
    case StateMode::TransformImage : {
        clearAreaOnImage();
        _transformingItem->setCursor(QCursor(QPixmap("://img/cross_cursor.png")));
        QPointF handleItemAnchorPos(_originW/2., _originH/2.);
        QPointF handleItemRotaterPos(_originW/2.,-_originH/4.);
        if(_transformingItemType == TransformingItem::PolygonItem)
        {
            QPolygonF pol;
            pol << _handleItemTopLeft->pos() << _handleItemTopRight->pos() << _handleItemBottomRight->pos() << _handleItemBottomLeft->pos();
            handleItemAnchorPos = pol.boundingRect().center();
            handleItemRotaterPos = QPointF(handleItemAnchorPos.x(), -_originH/4);
        }
        _handleItemAnchor->setPos(handleItemAnchorPos);
        _transformingItem->setPos(_handleItemAnchor->scenePos());
        _transformingItem->setTransform(QTransform().rotate(_rotation).scale(_scW, _scH).translate(-_handleItemAnchor->x(), -_handleItemAnchor->y()));
        _handleItemRotater->setPos(handleItemRotaterPos);
        _rotaterLine->setLine(_handleItemAnchor->pos(), _handleItemRotater->pos());
        setHandlesVisible(true);
    }break;
    case StateMode::CropImage : {
        clearAreaOnImage();
        setHandlesVisible(true);

        _handleItemAnchor->setVisible(false);
        _handleItemRotater->setVisible(false);
        _rotaterLine->setVisible(false);

        _cropArea = QRectF(0, 0, _originW, _originH);
        _fogItem = new FogItem(_transformingItem->castToGraphicsItem(), _cropArea);
    }break;
    case StateMode::GetColorOnImage : {
        setHandlesVisible(false);

        _transformingItem->setCursor(QCursor(QPixmap("://img/getcolor_cursor.png"), 0, 0));
    }break;
    case StateMode::GetAreaOnImage : {
        clearAreaOnImage();
        setHandlesVisible(false);

        QRectF imageArea = QRectF(0, 0, _originW, _originH);
        _fogItem = new FogItem(_transformingItem->castToGraphicsItem(), imageArea);
        _fogItem->setArea(QPolygonF());
        _transformingItem->setCursor(QCursor(QPixmap("://img/polygon_cursor.png"), 0, 0));
    }break;
    }

}

StateMode TransformingState::mode()
{
    return _stateMode;
}

void TransformingState::setTransparentBackgroundForPixmapItem(bool on_off)
{
    PixmapTransformingItem * pixmapTransformingItem = dynamic_cast<PixmapTransformingItem*>(_transformingItem);
    if(pixmapTransformingItem)
        pixmapTransformingItem->setTransparentBackground(on_off);
}

void TransformingState::setHandlesVisible(bool on_off)
{
    if(on_off)
    {
        switch(_globalMode)
        {
        case GlobalMode::AllAction :
        case GlobalMode::MoveRotateAndCropOnly : {
            foreach(HandleItem* item, _handleItems)
                item->setVisible(true);
        }break;
        case GlobalMode::MoveAndRotateOnly : {
            foreach(HandleItem* item, _handleItems)
                switch (item->getHandleType())
                {
                case HandleType::Rotater :
                case HandleType::Anchor :
                    item->setVisible(true);
                    break;
                }
        }break;
        }
    }
    else
    {
        foreach(HandleItem* item, _handleItems)
            item->setVisible(false);
    }
    _rotaterLine->setVisible(on_off);
}

bool TransformingState::wheelEvent(QWheelEvent* e, QPointF scenePos)
{
    if(_blockWheelEvent)
        return false;
    else
        return ScrollBaseState::wheelEvent(e, scenePos);
}

bool TransformingState::mouseMoveEvent(QMouseEvent *event, QPointF scenePos)
{
    switch(_stateMode)
    {
    case StateMode::ScrollMap : {

    }break;

    case StateMode::CropImage : {

        QPointF pos =_transformingItem->mapFromScene(scenePos);

        if(pos.x() < 0)
            pos.setX(0);
        else if(pos.x() > _originW-1)
            pos.setX(_originW-1);

        if(pos.y() < 0)
            pos.setY(0);
        else if(pos.y() > _originH-1)
            pos.setY(_originH-1);

        bool processed = true;
        QRectF r;
        switch(_currentHandleType)
        {
        case HandleType::TopLeft : {
            r = QRectF(pos, _cropArea.bottomRight());
        }break;
        case HandleType::TopCenter : {
            r = QRectF(_cropArea.x(), pos.y(), _cropArea.width(), _cropArea.bottom() - pos.y());
        }break;
        case HandleType::TopRight : {
            r = QRectF(pos, _cropArea.bottomLeft());
        }break;
        case HandleType::RightCenter : {
            r = QRectF(_cropArea.x(), _cropArea.y(), pos.x() - _cropArea.x(), _cropArea.height());
        }break;
        case HandleType::BottomRight : {
            r = QRectF(_cropArea.x(), _cropArea.y(), pos.x() - _cropArea.x(), pos.y() - _cropArea.y());
        }break;
        case HandleType::BottomCenter : {
            r = QRectF(_cropArea.x(), _cropArea.y(), _cropArea.width(), pos.y() - _cropArea.y());
        }break;
        case HandleType::BottomLeft : {
            r = QRectF(pos, _cropArea.topRight());
        }break;
        case HandleType::LeftCenter : {
            r = QRectF(pos.x(), _cropArea.y(), _cropArea.right() - pos.x(), _cropArea.height());
        }break;

        default:
            processed = false;
        }

        if(processed)
        {
            _cropArea = r.normalized();
            foreach(HandleItem* item, _handleItems)
                item->updatePos();

            if(_transformingItemType != TransformingItem::PolygonItem)
            {
                _topLine->setLine(QLineF(_handleItemTopLeft->pos(), _handleItemTopRight->pos()));
                _bottomLine->setLine(QLineF(_handleItemBottomLeft->pos(), _handleItemBottomRight->pos()));
                _leftLine->setLine(QLineF(_handleItemTopLeft->pos(), _handleItemBottomLeft->pos()));
                _rightLine->setLine(QLineF(_handleItemTopRight->pos(), _handleItemBottomRight->pos()));
            }

            if(_fogItem)
                _fogItem->setArea(_cropArea);
            return false;
        }


    }break;

    case StateMode::TransformImage : {
        bool processed = true;
        double w = lenght(_handleItemTopLeft->scenePos(), _handleItemTopRight->scenePos());
        double h = lenght(_handleItemTopLeft->scenePos(), _handleItemBottomLeft->scenePos());
        double sideProportion = w / h;
        double scW(_scW), scH(_scH);
        switch(_currentHandleType)
        {
        case HandleType::UserHandle: {
            double l = lenght(scenePos, _handleItemAnchor->scenePos());
            double coef = l / _userHandleLenght;
            _scW = _userHandleSCW * coef;
            _scH = _userHandleSCH * coef;
        }break;

        case HandleType::TopLeft: {
            double l = lenght(scenePos, _handleItemTopRight->scenePos());
            _scW = l / _originW;
            if(event->modifiers() & Qt::ShiftModifier)
                _scH = l / sideProportion / _originH;
            else
                _scH = lenght(scenePos, _handleItemBottomLeft->scenePos()) / _originH;
        }break;

        case HandleType::BottomRight: {
            double l = lenght(scenePos, _handleItemBottomLeft->scenePos());
            _scW = l / _originW;
            if(event->modifiers() & Qt::ShiftModifier)
                _scH = l / sideProportion / _originH;
            else
                _scH = lenght(scenePos, _handleItemTopRight->scenePos()) / _originH;
        }break;

        case HandleType::TopRight: {
            double l = lenght(scenePos, _handleItemTopLeft->scenePos());
            _scW = l / _originW;
            if(event->modifiers() & Qt::ShiftModifier)
                _scH = l / sideProportion / _originH;
            else
                _scH = lenght(scenePos, _handleItemBottomRight->scenePos()) / _originH;
        }break;

        case HandleType::BottomLeft: {
            double l = lenght(scenePos, _handleItemBottomRight->scenePos());
            _scW = l / _originW;
            if(event->modifiers() & Qt::ShiftModifier)
                _scH = l / sideProportion / _originH;
            else
                _scH = lenght(scenePos, _handleItemTopLeft->scenePos()) / _originH;
        }break;

        case HandleType::TopCenter: {
            _scH = lenght(scenePos, _handleItemBottomCenter->scenePos()) / _originH;
        }break;

        case HandleType::BottomCenter: {
            _scH = lenght(scenePos, _handleItemTopCenter->scenePos()) / _originH;
        }break;

        case HandleType::LeftCenter: {
            _scW = lenght(scenePos, _handleItemRightCenter->scenePos()) / _originW;
        }break;

        case HandleType::RightCenter: {
            _scW = lenght(scenePos, _handleItemLeftCenter->scenePos()) / _originW;
        }break;

        case HandleType::Rotater: {
            QPointF pos = _transformingItem->mapFromScene(scenePos);
            double l = lenght(_handleItemAnchor->pos(), pos);
            _handleItemRotater->setDeltaByVertical(l);

            QLineF line(_handleItemAnchor->scenePos(), scenePos);
            _rotation = ((360 - line.angle()) + 90);
            while(_rotation > 360)
                _rotation -= 360.0;

            if(event->modifiers() & Qt::ShiftModifier)
            {
                int r = _rotation / 45;
                int rMod = (int)_rotation % 45;
                if(rMod > 45/2.)
                    r += 1;
                _rotation = r * 45;
            }
            _transformingItem->setTransform(QTransform().rotate(_rotation).scale(_scW, _scH).translate(-_handleItemAnchor->x(), -_handleItemAnchor->y()));

            if( (event->modifiers() & Qt::ShiftModifier) == false )
                _handleItemRotater->setPos(_handleItemAnchor->pos().x(), _handleItemAnchor->pos().y() - l);

            _rotaterLine->setLine(_handleItemAnchor->pos(), _handleItemRotater->pos());
            _lastScenePos = scenePos;
        }return false; //! <------------------------------- !!! так надо :)

        default:
            processed = false;
        }

        _lastScenePos = scenePos;

        if(processed)
        {
            _transformingItem->setFlags(0);
            if(_scW > _scWmax || _scH > _scHmax)
            {
                _scW = scW;
                _scH = scH;
            }
            else
            {
                _transformingItem->setTransform(QTransform().rotate(_rotation).scale(_scW, _scH).translate(-_handleItemAnchor->x(), -_handleItemAnchor->y()));
            }
            return false;
        }
    }break;

    case StateMode::GetAreaOnImage : {
        QPointF pos = _transformingItem->mapFromScene(scenePos);
        if(_transformingItem->contains(pos))
        {
            if(_fogItem)
            {
                QPolygonF areaOnImage(_areaOnImage);
                areaOnImage.append(pos);
                _fogItem->setArea(areaOnImage);
            }

            if(_areaLineItems.size() != 0)
            {
                _areaLineItems.first()->setLine( QLineF( _areaLineItems.first()->line().p1(), pos) );
                _areaLineItems.last()->setLine( QLineF( _areaLineItems.last()->line().p1(), pos) );
            }
        }
    }break;

    default : {}
    }


    return ScrollBaseState::mouseMoveEvent(event, scenePos);
}

bool TransformingState::mousePressEvent(QMouseEvent *e, QPointF scenePos)
{
    switch(_stateMode)
    {
    case StateMode::GetAreaOnImage : {
        QPointF pos = _transformingItem->mapFromScene(scenePos);
        if( _transformingItem->contains(pos) && (e->modifiers() & Qt::ControlModifier) == false )
        {
            _areaOnImage.append(pos);

            if(_areaLineItems.size() == 0) //  || _areaLineItems.size() == 1)
                _areaLineItems.append(createAreaLineItem(pos, pos));
            _areaLineItems.append(createAreaLineItem(pos, pos));

            return false;
        }
    }break;
    default : {}
    }

    _blockWheelEvent = true;
    return ScrollBaseState::mousePressEvent(e, scenePos);
}

bool TransformingState::mouseReleaseEvent(QMouseEvent *e, QPointF scenePos)
{
    switch(_stateMode)
    {
    case StateMode::GetAreaOnImage : {
        if( _transformingItem->contains( _transformingItem->mapFromScene(scenePos) ) && (e->modifiers() & Qt::ControlModifier) == false )
        {


        }
    }break;
    case StateMode::TransformImage : {
    }break;
    default : {}
    }

    foreach(HandleItem * handleItem,_handleItems)
        handleItem->mouseRelease();

    _blockWheelEvent = false;
    _currentHandleType = HandleType::Invalid;
    _transformingItem->setFlags(QGraphicsItem::ItemIsMovable);

    return ScrollBaseState::mouseReleaseEvent(e, scenePos);
}

bool TransformingState::mouseDoubleClickEvent(QMouseEvent *e, QPointF scenePos)
{
    switch(_stateMode)
    {
    case StateMode::GetAreaOnImage : {
        QPointF pos = _transformingItem->mapFromScene(scenePos);
        if( _transformingItem->contains(pos) && _areaLineItems.size() > 2)
            emit signalAreaSetted();
    }break;
    case StateMode::TransformImage : {
        QPointF pos = _transformingItem->mapFromScene(scenePos);
        if(_transformingItem->contains(pos))
        {
            _handleItemAnchor->setPos(pos);

            _transformingItem->setPos(_handleItemAnchor->scenePos());
            _transformingItem->setTransform(QTransform().rotate(_rotation).scale(_scW, _scH).translate(-_handleItemAnchor->x(), -_handleItemAnchor->y()));

            _handleItemRotater->setPos(_handleItemAnchor->pos().x(), _handleItemAnchor->pos().y() - _handleItemRotater->deltaByVertical() );
            _rotaterLine->setLine(_handleItemAnchor->pos(), _handleItemRotater->pos());
        }
    }break;
    default : {}
    }

    return ScrollBaseState::mouseDoubleClickEvent(e, scenePos);
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
    setActiveForScene(true);
}

void TransformingState::setOpacity(double val)
{
    PixmapTransformingItem * pixmapTransformingItem = dynamic_cast<PixmapTransformingItem*>(_transformingItem);
    if(pixmapTransformingItem)
    {
        if(val < 0.01)
            val = 0.01;
        pixmapTransformingItem->setOpacity(val);
    }
}

void TransformingState::setTransparency(int percent)
{
    setOpacity(1 - percent / 100.);
}

QPolygonF TransformingState::getPixmapCorners()
{
    QPolygonF cornersInSceneCoords;
    cornersInSceneCoords.append(_handleItemTopLeft->scenePos());
    cornersInSceneCoords.append(_handleItemTopRight->scenePos());
    cornersInSceneCoords.append(_handleItemBottomRight->scenePos());
    cornersInSceneCoords.append(_handleItemBottomLeft->scenePos());
    return cornersInSceneCoords;
}

bool TransformingState::changed()
{
    return _undoStack.size() > 1; //! первое состояние устанавливается в ините и берется не стека не QStack::pop(), a QStack::top() !
}

QRectF TransformingState::getCropArea()
{
    return _cropArea;
}

bool TransformingState::cropItem()
{
    if(_stateMode != StateMode::CropImage)
        return false;

    if(_cropArea.width() < 1 || _cropArea.height() < 1)
        return false;

    PixmapTransformingItem * pixmapTransformingItem = dynamic_cast<PixmapTransformingItem*>(_transformingItem);
    if(pixmapTransformingItem)
    {
        QPixmap pm = _pixmap.copy(_cropArea.toRect());
        _pixmap = QPixmap(pm.size());
        _pixmap.fill(QColor(0,0,0,1));
        QPainter painter(&_pixmap);
        painter.drawPixmap(0,0,pm);

        _originW = _pixmap.width();
        _originH = _pixmap.height();

        QPointF scenePos = pixmapTransformingItem->mapToScene(_cropArea.topLeft());
        pixmapTransformingItem->setPos(scenePos);
        pixmapTransformingItem->setPixmap(_pixmap);

        //-----------------------

        _handleItemTopLeft->setPos(0,0);
        _handleItemTopRight->setPos(_originW,0);
        _handleItemBottomRight->setPos(_originW, _originH);
        _handleItemBottomLeft->setPos(0, _originH);
        _handleItemTopCenter->setPos(_originW/2.,0);
        _handleItemBottomCenter->setPos(_originW/2., _originH);
        _handleItemLeftCenter->setPos(0, _originH/2);
        _handleItemRightCenter->setPos(_originW, _originH/2);

        _handleItemRotater->setPos(_originW/2.,-_originH/4.);
        _handleItemAnchor->setPos(_originW/2., _originH/2.);

        if(_transformingItemType != TransformingItem::PolygonItem)
        {
            _topLine->setLine(QLineF(_handleItemTopLeft->pos(), _handleItemTopRight->pos()));
            _bottomLine->setLine(QLineF(_handleItemBottomLeft->pos(), _handleItemBottomRight->pos()));
            _leftLine->setLine(QLineF(_handleItemTopLeft->pos(), _handleItemBottomLeft->pos()));
            _rightLine->setLine(QLineF(_handleItemTopRight->pos(), _handleItemBottomRight->pos()));
        }
        _rotaterLine->setLine(_handleItemAnchor->pos(), _handleItemRotater->pos());

        _cropArea = QRectF(0, 0, _originW, _originH);
        if(_fogItem)
            delete _fogItem;
        _fogItem = new FogItem(pixmapTransformingItem, _cropArea);

        return true;
    }

    SvgTransformingItem * svgTransformingItem = dynamic_cast<SvgTransformingItem*>(_transformingItem);
    if(svgTransformingItem)
    {
        QDomDocument doc;
        QFile fileIn(_svgFilePath);
        if(fileIn.open(QIODevice::ReadOnly))
            if(doc.setContent(&fileIn))
            {
                QDomElement docElem = doc.documentElement();
                QStringList viewBoxStrParams = docElem.attribute("viewBox").split(QString(" "));
                double x(0), y(0);
                if(viewBoxStrParams.size() == 4)
                {
                    x = viewBoxStrParams.at(0).toDouble();
                    y = viewBoxStrParams.at(1).toDouble();
                }
                QString X = QString::number(x + _cropArea.left(), 'f', 0);
                QString Y = QString::number(y + _cropArea.top(), 'f', 0);
                QString W = QString::number(_cropArea.width(), 'f', 0);
                QString H = QString::number(_cropArea.height(), 'f', 0);

                docElem.setAttribute("min-x", X);
                docElem.setAttribute("min-y", Y);
                docElem.setAttribute("width", W);
                docElem.setAttribute("height", H);
                QString viewBox = X + QString(" ") + Y + QString(" ") + W + QString(" ") + H;
                docElem.setAttribute("viewBox", viewBox);

                svgTransformingItem->setRenderer(doc.toByteArray());
                QPointF scenePos = svgTransformingItem->mapToScene(_cropArea.topLeft());
                svgTransformingItem->setPos(scenePos);

                _svgFilePath = TempDirController::createTempDirForCurrentUser() + QDir::separator() + QFileInfo(_svgFilePath).fileName();

                QFile fileOut(_svgFilePath);
                if(fileOut.open(QIODevice::WriteOnly))
                {
                    fileOut.write(doc.toByteArray());
                    fileOut.close();
                }

                _originW = _cropArea.width();
                _originH = _cropArea.height();

                _handleItemTopLeft->setPos(0,0);
                _handleItemTopRight->setPos(_originW,0);
                _handleItemBottomRight->setPos(_originW, _originH);
                _handleItemBottomLeft->setPos(0, _originH);
                _handleItemTopCenter->setPos(_originW/2.,0);
                _handleItemBottomCenter->setPos(_originW/2., _originH);
                _handleItemLeftCenter->setPos(0, _originH/2);
                _handleItemRightCenter->setPos(_originW, _originH/2);

                _handleItemRotater->setPos(_originW/2.,-_originH/4.);
                _handleItemAnchor->setPos(_originW/2., _originH/2.);

                _topLine->setLine(QLineF(_handleItemTopLeft->pos(), _handleItemTopRight->pos()));
                _bottomLine->setLine(QLineF(_handleItemBottomLeft->pos(), _handleItemBottomRight->pos()));
                _leftLine->setLine(QLineF(_handleItemTopLeft->pos(), _handleItemBottomLeft->pos()));
                _rightLine->setLine(QLineF(_handleItemTopRight->pos(), _handleItemBottomRight->pos()));

                _rotaterLine->setLine(_handleItemAnchor->pos(), _handleItemRotater->pos());

                _cropArea = QRectF(0, 0, _originW, _originH);
            }
        fileIn.close();

        return true;
    }

    return false;
}

void TransformingState::pressHandle(HandleType handleType)
{
    if(handleType != HandleType::Invalid)
    {
        _scWmax = _scW*15;
        _scHmax = _scH*15;

        UndoAct undoAct;
        undoAct.scenePos = _handleItemTopLeft->scenePos();
        undoAct.scaleW = _scW;
        undoAct.scaleH = _scH;
        undoAct.rotation = _rotation;
        _undoStack.push(undoAct);

        emit signalItemChanged();
    }
    _currentHandleType = handleType;
}

void TransformingState::anchorMoved()
{
    _handleItemRotater->setPos(_handleItemAnchor->pos().x(), _handleItemAnchor->pos().y() - _handleItemRotater->deltaByVertical() );
    _rotaterLine->setLine(_handleItemAnchor->pos(), _handleItemRotater->pos());
}

void TransformingState::anchorReplaced()
{
    _transformingItem->setPos(_handleItemAnchor->scenePos());
    _transformingItem->setTransform(QTransform().rotate(_rotation).scale(_scW, _scH).translate(-_handleItemAnchor->x(), -_handleItemAnchor->y()));
}

void TransformingState::createUserHandler(QPointF scenePos)
{
    if(_globalMode == GlobalMode::AllAction)
    {
        _userHandlePos = scenePos;
        _currentHandleType = HandleType::UserHandle;
        _userHandleLenght = lenght(_handleItemAnchor->scenePos(), scenePos);
        _userHandleSCW = _scW;
        _userHandleSCH = _scH;
        _scWmax = _scW*15;
        _scHmax = _scH*15;
    }
}

void TransformingState::resendColor(QColor color)
{
    emit signalSendColor(color);
}

void TransformingState::pixmapMoved()
{
    emit signalItemChanged();
}

double TransformingState::lenght(QPointF p1, QPointF p2)
{
    return sqrt( (p1.x() - p2.x()) * (p1.x() - p2.x()) + (p1.y() - p2.y()) * (p1.y() - p2.y()) );
}

double TransformingState::lenght(QColor c1, QColor c2)
{
    return sqrt( (c1.red() - c2.red()) * (c1.red() - c2.red()) + (c1.green() - c2.green()) * (c1.green() - c2.green()) + (c1.blue() - c2.blue()) * (c1.blue() - c2.blue()) );
}

void TransformingState::clearAreaOnImage()
{
    foreach(LineItem * item, _areaLineItems)
        delete item;
    _areaLineItems.clear();
    _areaOnImage.clear();
    if(_fogItem)
    {
        delete _fogItem;
        _fogItem = nullptr;
    }
}

void TransformingState::getCurrentVertex(QPolygonF &vertex, double &pixmapWidth, double &pixmapHeight)
{
    pixmapWidth = _pixmap.width();
    pixmapHeight = _pixmap.height();
    vertex << _handleItemTopLeft->scenePos() << _handleItemTopRight->scenePos() << _handleItemBottomRight->scenePos() << _handleItemBottomLeft->scenePos();
}

UndoAct TransformingState::getCurrentParams()
{
    UndoAct currentParams;
    PixmapTransformingItem * pixmapTransformingItem = dynamic_cast<PixmapTransformingItem*>(_transformingItem);
    if(pixmapTransformingItem)
    {
        QString filePath = TempDirController::createTempDirForCurrentUser() + QDir::separator() + _rasterFileBaseName + ".tif";
        bool res = _pixmap.save(filePath, "TIF");
        currentParams.filePath = filePath;
    }

    SvgTransformingItem * svgTransformingItem = dynamic_cast<SvgTransformingItem*>(_transformingItem);
    if(svgTransformingItem)
    {
        currentParams.filePath = _svgFilePath;
    }

    currentParams.scenePos = _handleItemTopLeft->scenePos();
    currentParams.scaleW = _scW;
    currentParams.scaleH = _scH;
    currentParams.rotation = _rotation;
    return currentParams;
}

LineItem* TransformingState::createAreaLineItem(QPointF p1, QPointF p2)
{
    QColor penColor(Qt::magenta);
    penColor.setAlpha(100);
    QPen pen(penColor);
    pen.setCosmetic(true);
    pen.setWidth(1);

    LineItem* item = new LineItem(_transformingItem->castToGraphicsItem(), pen);
    item->setLine(QLineF(p1, p2));
    return item;
}

void TransformingState::changeImageColor(QColor inColor, QColor outColor, int sensitivity)
{
    if(_pixmap.isNull())
        return;

    PixmapTransformingItem * pixmapTransformingItem = dynamic_cast<PixmapTransformingItem*>(_transformingItem);
    if( ! pixmapTransformingItem)
        return;

    if(inColor.isValid() && outColor.isValid())
        inColor.setAlpha(outColor.alpha());

    QString filePath = TempDirController::createTempDirForCurrentUser() + QDir::separator() + _rasterFileBaseName + ".tif";
    bool res = _pixmap.save(filePath, "TIF");
    UndoAct undoAct;
    undoAct.filePath = filePath;
    undoAct.scenePos = _handleItemTopLeft->scenePos();
    undoAct.scaleW = _scW;
    undoAct.scaleH = _scH;
    undoAct.rotation = _rotation;
    _undoStack.push(undoAct);
    emit signalItemChanged();

    QString text;
    if(_areaOnImage.isEmpty()) // всё изображени
    {
        text = QString::fromUtf8("Замена цвета изображения");
    }
    else // только область
    {
        if(inColor.isValid() && outColor.isValid() == false)
            text = QString::fromUtf8("Заливка выбранной области");
        else
            text = QString::fromUtf8("Замена цвета в выбранной области");
    }

    QColor transparencyColor(0,0,0,1);
    QImage image(_pixmap.toImage().convertToFormat(QImage::Format_ARGB32));
    ProgressDialog progressDialog(text);
    double oneImageByPercent(100. / (double)(image.width()) );
    for(int x(0); x < image.width(); ++x)
    {
        for(int y(0); y < image.height(); ++y)
        {
            if(_areaOnImage.isEmpty() || _areaOnImage.containsPoint(QPointF(x,y), Qt::OddEvenFill))
            {
                if(inColor.isValid() && outColor.isValid()) // замена "outColor" на "inColor"
                {
                    QColor pixelColor = QColor::fromRgba( image.pixel(x,y) );
                    if(pixelColor.alpha() > 1 && lenght(pixelColor, outColor) <= sensitivity)
                        image.setPixel(x, y, inColor.rgba());
                }
                else if(inColor.isValid() == false && outColor.isValid()) // производится замена на полностью прозрачный черный пиксель
                {
                    QColor pixelColor = QColor::fromRgb( image.pixel(x,y) );
                    if(lenght(pixelColor, outColor) <= sensitivity)
                        image.setPixel(x, y, transparencyColor.rgba());
                }
                else if(inColor.isValid() && outColor.isValid() == false) // производится заливка всей области цветом inColor
                {
                    image.setPixel(x, y, inColor.rgba());
                }
                else if(inColor.isValid() == false && outColor.isValid() == false) // производится заливка всей области полностью прозрачным черным пикселем
                {
                    image.setPixel(x, y, transparencyColor.rgba());
                }
            }
        }
        progressDialog.setPersent( oneImageByPercent * x );
    }

    _pixmap = QPixmap::fromImage(image);
    pixmapTransformingItem->setPixmap(_pixmap);
}

int TransformingState::getTransformingItemType() const
{
    return _transformingItemType;
}

void TransformingState::setGlobalMode(GlobalMode mode)
{
    _globalMode = mode;
}

GlobalMode TransformingState::getGlobalMode()
{
    return _globalMode;
}

void TransformingState::undoAction()
{
    if(_undoStack.isEmpty())
        return;

    UndoAct undoAct;
    if(_undoStack.size() == 1)
        undoAct = _undoStack.top();
    else
        undoAct = _undoStack.pop();

    if(undoAct.filePath.isEmpty() == false)
    {
        PixmapTransformingItem * pixmapTransformingItem = dynamic_cast<PixmapTransformingItem*>(_transformingItem);
        if(pixmapTransformingItem)
        {
            _pixmap = QPixmap(undoAct.filePath);

            QPixmap pm(_pixmap.size());
            pm.fill(QColor(0,0,0,1));
            QPainter painter(&pm);
            painter.drawPixmap(0,0,_pixmap);

            pixmapTransformingItem->setPixmap(pm);
        }
    }

    _originW = _pixmap.width();
    _originH = _pixmap.height();

    _transformingItem->setPos(undoAct.scenePos);

    _handleItemTopLeft->setPos(0,0);
    _handleItemTopRight->setPos(_originW,0);
    _handleItemBottomRight->setPos(_originW, _originH);
    _handleItemBottomLeft->setPos(0, _originH);
    _handleItemTopCenter->setPos(_originW/2.,0);
    _handleItemBottomCenter->setPos(_originW/2., _originH);
    _handleItemLeftCenter->setPos(0, _originH/2);
    _handleItemRightCenter->setPos(_originW, _originH/2);

    if(_transformingItemType != TransformingItem::PolygonItem)
    {
        _topLine->setLine(QLineF(_handleItemTopLeft->pos(), _handleItemTopRight->pos()));
        _bottomLine->setLine(QLineF(_handleItemBottomLeft->pos(), _handleItemBottomRight->pos()));
        _leftLine->setLine(QLineF(_handleItemTopLeft->pos(), _handleItemBottomLeft->pos()));
        _rightLine->setLine(QLineF(_handleItemTopRight->pos(), _handleItemBottomRight->pos()));
    }

    if(undoAct.anchorPos.isNull())
        _handleItemAnchor->setPos(_originW/2., _originH/2.);
    else
        _handleItemAnchor->setPos(undoAct.anchorPos);

    _handleItemRotater->setPos(_handleItemAnchor->pos().x(), _handleItemAnchor->pos().y() - _handleItemRotater->deltaByVertical() ); // _handleItemRotater->setPos(_originW/2.,-_originH/4.);

    _scW = undoAct.scaleW;
    _scH = undoAct.scaleH;
    _rotation = undoAct.rotation;

    _transformingItem->setTransform(QTransform().scale(_scW, _scH));
    _transformingItem->setPos(_handleItemAnchor->scenePos());
    _transformingItem->setTransform(QTransform().rotate(_rotation).scale(_scW, _scH).translate(-_handleItemAnchor->x(), -_handleItemAnchor->y()));

    _rotaterLine->setLine(_handleItemAnchor->pos(), _handleItemRotater->pos());

    if(_fogItem)
        _fogItem->setArea(QRectF(0, 0, _pixmap.width(), _pixmap.height()));

    emit signalItemChanged();
}














