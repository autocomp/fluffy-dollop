#include "choiceareastate.h"
#include <QDebug>
#include<QPen>

ChoiceAreaState::ChoiceAreaState(AreaType areaType, QCursor cursor)
    : _areaType(areaType)
    , _cursor(cursor)
    , _emitAbortAfterPushedToStack(false)
{
//    QColor colPen;
//    colPen.setRgba(qRgba(0x09,0x61,0xD3,150));
    _pen.setColor(Qt::blue); // colPen);
    _pen.setWidth(2);
    _pen.setCosmetic(true);
    _pen.setJoinStyle(Qt::RoundJoin);
}

ChoiceAreaState::~ChoiceAreaState()
{
    _view->setCursor(Qt::ArrowCursor);
    foreach(QGraphicsLineItem* item, _lineItems)
        delete item;
}

void ChoiceAreaState::setPen(QPen pen)
{
    _pen = pen;
    _pen.setCosmetic(true);
}

bool ChoiceAreaState::mouseMoveEvent(QMouseEvent * e, QPointF scenePos)
{
    if(_boundingArea.isEmpty() == false)
    {
        if(_boundingArea.containsPoint(scenePos, Qt::OddEvenFill) == false)
        {
            _view->setCursor(Qt::ForbiddenCursor);
            return false;
        }
    }
//    else if(_scene->sceneRect().contains(scenePos) == false)
//    {
//        _view->setCursor(Qt::ForbiddenCursor);
//        return false;
//    }

    _view->setCursor(_cursor);

    if(e)
        if(e->modifiers() & Qt::ShiftModifier && _lineItems.size() != 0)
        {
            QPointF _scenePos = _lineItems.last()->line().p1();
            QLineF line(_scenePos, scenePos);
            double angle = ((360 - line.angle()) + 90);
            while(angle > 360)
                angle -= 360.0;

            if(angle >= 45  && angle < 135 )                        // RIGHT
                scenePos = QPointF(scenePos.x(), _scenePos.y());
            else if(angle >= 135  && angle < 225)                   // DOWN
                scenePos = QPointF(_scenePos.x(), scenePos.y());
            else if(angle >= 225  && angle < 315)                   // LEFT
                scenePos = QPointF(scenePos.x(), _scenePos.y());
            else                                                    // UP
                scenePos = QPointF(_scenePos.x(), scenePos.y());
        }

    _lastMousePos = scenePos;

    switch(_areaType)
    {
    case POLYLINE :{
        if(_lineItems.size() != 0)
        {
            _lineItems.last()->setLine( QLineF( _lineItems.last()->line().p1(), scenePos) );
        }
    }break;
    case POLYGON :
    case POINT_OR_POLYGON :{
        if(_lineItems.size() != 0)
        {
            _lineItems.first()->setLine( QLineF( _lineItems.first()->line().p1(), scenePos) );
            _lineItems.last()->setLine( QLineF( _lineItems.last()->line().p1(), scenePos) );
        }
    }break;
    };

    return false;
}

bool ChoiceAreaState::mousePressEvent(QMouseEvent* e, QPointF scenePos)
{
    if(e->button() == Qt::LeftButton)
    {
        if(_boundingArea.isEmpty() == false)
        {
            if(_boundingArea.containsPoint(scenePos, Qt::OddEvenFill) == false)
            {
                _view->setCursor(Qt::ForbiddenCursor);
                return false;
            }
        }
//        else if(_scene->sceneRect().contains(scenePos) == false)
//        {
//            _view->setCursor(Qt::ForbiddenCursor);
//            return false;
//        }

        _polygon.append(scenePos);

        QPointF prevPos(scenePos);
        if(_lineItems.size() > 0)
            prevPos = _lineItems.last()->line().p2();

        switch(_areaType)
        {
        case POINT :{
            finishChoice();
        }break;
        case POLYLINE :{
            _lineItems.append(createItem(scenePos, scenePos));
        }break;
        case POLYGON :
        case POINT_OR_POLYGON :{
            if(_polygon.size() == 1)
                _lineItems.append(createItem(prevPos, scenePos));

            _lineItems.append(createItem(prevPos, scenePos));

            mouseMoveEvent(0, scenePos);
        }break;
        };
    }

    return false;
}

bool ChoiceAreaState::mouseDoubleClickEvent(QMouseEvent *e, QPointF scenePos)
{
    if(e->button() == Qt::LeftButton)
    {
        if(_boundingArea.isEmpty() == false)
        {
            if(_boundingArea.containsPoint(scenePos, Qt::OddEvenFill) == false)
            {
                _view->setCursor(Qt::ForbiddenCursor);
                return false;
            }
        }
//        else if(_scene->sceneRect().contains(scenePos) == false)
//        {
//            _view->setCursor(Qt::ForbiddenCursor);
//            return false;
//        }

        switch(_areaType)
        {
        case POLYLINE :
        case POLYGON :
        case POINT_OR_POLYGON :{
            finishChoice();
        }break;
        };
    }

    return false;
}

bool ChoiceAreaState::keyPressEvent(QKeyEvent * e)
{
    switch(e->key())
    {
    case Qt::Key_Escape:
        emit signalAbort();
        return false;

    case Qt::Key_Backspace:{
        if(_polygon.size() == 0)
            return false;

        switch(_areaType)
        {
        case POLYLINE  :{
            _polygon.pop_back();
            delete _lineItems.last();
            _lineItems.pop_back();
            mouseMoveEvent(0, _lastMousePos);
        }break;

        case POLYGON  :
        case POINT_OR_POLYGON :{
            _polygon.pop_back();
            delete _lineItems.last();
            _lineItems.pop_back();
            if(_polygon.size() == 0)
            {
                foreach(QGraphicsLineItem* item, _lineItems)
                    delete item;
                _lineItems.clear();
            }
            mouseMoveEvent(0, _lastMousePos);
        }break;
        };

    }return false;

    case Qt::Key_Return:
    case Qt::Key_Space:
        finishChoice();
        return false;
    }

    return true;
}

void ChoiceAreaState::finishChoice()
{
    if(_areaType == POLYGON && _polygon.size() < 3)
        return;

    if(_areaType == POINT_OR_POLYGON && _polygon.size() == 2)
        return;

    QPolygonF polygonInNativeCoords(_polygon);
    foreach(QGraphicsLineItem* item, _lineItems)
        delete item;
    _lineItems.clear();
    _polygon.clear();
    emit signalAreaChoiced(polygonInNativeCoords);
}

ChoiceAreaState::LineItem* ChoiceAreaState::createItem(QPointF p1, QPointF p2)
{
    LineItem* item = new LineItem(p1,p2);
    item->setZValue(1000);
    item->setPen(_pen);
    _scene->addItem(item);
    return item;
}

QString ChoiceAreaState::stateName()
{
    return QString("ChoiceAreaState");
}

void ChoiceAreaState::setEmitingAbortAfterPushedToStack(bool on_off)
{
    _emitAbortAfterPushedToStack = on_off;
}

void ChoiceAreaState::setBoundingArea(const QPolygonF &boundingArea)
{
    _boundingArea = boundingArea;
}

void ChoiceAreaState::statePushedToStack()
{
    if(_emitAbortAfterPushedToStack)
        emit signalAbort();
}

void ChoiceAreaState::statePoppedFromStack()
{
}

//-----------------------------------------------------------------

ChoiceAreaState::LineItem::LineItem(QPointF p1, QPointF p2)
    : QGraphicsLineItem(QLineF(p1,p2))
{
}

void ChoiceAreaState::LineItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(line().isNull() == false)
    {
        painter->setRenderHint(QPainter::Antialiasing);
        QGraphicsLineItem::paint(painter, option, widget);
    }
}
