#include "svgeditorstate.h"
#include "lineitem.h"
#include "svgitem.h"
#include <ctrcore/ctrcore/tempdircontroller.h>
#include <QDir>
#include <QDebug>
#include <QFile>
#include <QGraphicsSvgItem>

using namespace svg_editor_state;

SvgEditorState::SvgEditorState(bool onTop)
    : _onTop(onTop)
{
}

SvgEditorState::~SvgEditorState()
{
    _view->setCursor(QCursor(Qt::ArrowCursor));

    foreach(SvgItem * item, _svgLineItems)
        delete item;

    clearTempItems();
}

void SvgEditorState::init(QGraphicsScene *scene, QGraphicsView *view, const int *zoom, const double *scale, double frameCoef, uint visualizerId)
{
    ScrollBaseState::init(scene, view, zoom, scale, frameCoef, visualizerId);
    setActiveForScene(true);
}

bool SvgEditorState::wheelEvent(QWheelEvent *e, QPointF scenePos)
{
    if(_blockWheelEvent)
        return false;
    else
        return ScrollBaseState::wheelEvent(e, scenePos);
}

bool SvgEditorState::mouseMoveEvent(QMouseEvent *e, QPointF scenePos)
{
    switch(_stateMode)
    {
    case StateMode::SelectMode : {
        return ScrollBaseState::mouseMoveEvent(e, scenePos);
    }break;
    case StateMode::AddLineMode :
    case StateMode::AddPolygonMode : {
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
        if(_lineItems.size() != 0)
        {
            if(_stateMode == StateMode::AddPolygonMode)
                _lineItems.first()->setLine( QLineF( _lineItems.first()->line().p1(), scenePos) );
            _lineItems.last()->setLine( QLineF( _lineItems.last()->line().p1(), scenePos) );
        }
        if(_polygonItem)
        {
            QPolygonF pol(_linePoints);
            pol.append(scenePos);
            _polygonItem->setPolygon(pol);
        }

    }break;
    case StateMode::AddRectMode : {
        repaintRectArea(scenePos);
    }break;
    }

    return false;
}

bool SvgEditorState::mousePressEvent(QMouseEvent *e, QPointF scenePos)
{
    switch(_stateMode)
    {
    case StateMode::SelectMode : {
        return ScrollBaseState::mousePressEvent(e, scenePos);
    }break;
    case StateMode::AddLineMode :
    case StateMode::AddPolygonMode : {
        QPointF prevPos(scenePos);
        if(_lineItems.size() > 0)
            prevPos = _lineItems.last()->line().p2();

        if(_stateMode == StateMode::AddPolygonMode && _lineItems.size() == 0)
        {
            StrokeParams strokeParams;
            emit getDefaultStrokeParams(strokeParams);
            _polygonItem = new PolygonItem();
            _polygonItem->setZValue(100000);
            _polygonItem->setPen(QPen(Qt::NoPen));
            _polygonItem->setBrush(strokeParams.getBrush());
            _polygonItem->setVisible(strokeParams.fillArea);
            _scene->addItem(_polygonItem);

            _lineItems.append(createLineItem(prevPos, scenePos));
        }
        _lineItems.append(createLineItem(prevPos, scenePos));

        _linePoints.append(prevPos);
    }break;
    case StateMode::AddRectMode : {
        if(_linePoints.isEmpty())
        {
            StrokeParams strokeParams;
            emit getDefaultStrokeParams(strokeParams);
            _polygonItem = new PolygonItem();
            _polygonItem->setZValue(100000);
            _polygonItem->setPen(QPen(Qt::NoPen));
            _polygonItem->setBrush(strokeParams.getBrush());
            _polygonItem->setVisible(strokeParams.fillArea);
            _scene->addItem(_polygonItem);

            _linePoints.append(scenePos);
        }
    }break;
    }

    return false;
}

bool SvgEditorState::mouseReleaseEvent(QMouseEvent *e, QPointF scenePos)
{
    switch(_stateMode)
    {
    case StateMode::SelectMode : {
        return ScrollBaseState::mouseReleaseEvent(e, scenePos);
    }break;
    case StateMode::AddLineMode : {

    }break;
    case StateMode::AddPolygonMode : {

    }break;
    case StateMode::AddRectMode : {

    }break;
    }

    return false;
}

bool SvgEditorState::mouseDoubleClickEvent(QMouseEvent *e, QPointF scenePos)
{
    StrokeParams strokeParams;
    emit getDefaultStrokeParams(strokeParams);

    bool layerCreated(false);
    switch(_stateMode)
    {
    case StateMode::SelectMode : {
        return ScrollBaseState::mouseDoubleClickEvent(e, scenePos);
    }break;
    case StateMode::AddLineMode : {
        if(_linePoints.size() >= 2)
        {
            SvgLineItem * item = new SvgLineItem(_view, _linePoints, strokeParams, _onTop);
            connect(item, SIGNAL(signalClickOnItem(bool)), this, SLOT(slotClickOnItem(bool)));
            item->viewZoomChanged();
            _svgLineItems.append(item);
            layerCreated = true;
        }
    }break;
    case StateMode::AddPolygonMode : {
        if(_linePoints.size() >= 2)
        {
            SvgPolygonItem * item = new SvgPolygonItem(_view, _linePoints, strokeParams, _onTop);
            connect(item, SIGNAL(signalClickOnItem(bool)), this, SLOT(slotClickOnItem(bool)));
            item->viewZoomChanged();
            _svgLineItems.append(item);
            layerCreated = true;
        }
    }break;
    case StateMode::AddRectMode : {
        if(_linePoints.size() != 0)
        {
            QRectF r(_linePoints.first(), scenePos);
            QPolygonF pol(r.normalized());
            SvgPolygonItem * item = new SvgPolygonItem(_view, pol, strokeParams, _onTop);
            connect(item, SIGNAL(signalClickOnItem(bool)), this, SLOT(slotClickOnItem(bool)));
            item->viewZoomChanged();
            _svgLineItems.append(item);
            layerCreated = true;
        }
    }break;
    }

    if(layerCreated)
    {
        clearTempItems();
        _linePoints.clear();
        emit setEnabledSaveButton(true);
    }

    return false;
}

void SvgEditorState::zoomChanged()
{
    foreach(SvgItem* item, _svgLineItems)
        item->viewZoomChanged();
}

QString SvgEditorState::stateName()
{
    return QString("SvgEditorState");
}

void SvgEditorState::statePushedToStack()
{
}

void SvgEditorState::statePoppedFromStack()
{
    setActiveForScene(true);
    zoomChanged();
}

void SvgEditorState::setMode(StateMode stateMode)
{
    clearTempItems();
    _linePoints.clear();
    _stateMode = stateMode;

    foreach(SvgItem* item, _selectedSvgItems)
        item->selectItem(false);
    _selectedSvgItems.clear();
    emit setEnabledDeleteButton(false);

    switch(_stateMode)
    {
    case StateMode::SelectMode : {
        _view->setCursor(QCursor(Qt::ArrowCursor));
    }break;
    case StateMode::AddLineMode : {
        _view->setCursor(QCursor(QPixmap("://img/line_cursor.png"), 0, 0));
    }break;
    case StateMode::AddPolygonMode : {
        _view->setCursor(QCursor(QPixmap("://img/polygon_cursor.png"), 0, 0));
    }break;
    case StateMode::AddRectMode : {
        _view->setCursor(QCursor(QPixmap("://img/rect_cursor.png"), 0, 0));
    }break;
    }

}

StateMode SvgEditorState::mode()
{
    return _stateMode;
}

void SvgEditorState::setDefaultStrokeParams(const StrokeParams & strokeParams)
{
    switch(_stateMode)
    {
    case StateMode::SelectMode : {
// ...

    }break;
    case StateMode::AddLineMode :
    case StateMode::AddPolygonMode :
    case StateMode::AddRectMode : {
        foreach(transform_state::LineItem * item, _lineItems)
            item->setPen(strokeParams.getLinePen());
        if(_polygonItem)
        {
            _polygonItem->setBrush(strokeParams.getBrush());
            _polygonItem->setVisible(strokeParams.fillArea);
        }
    }break;
    }
}

void SvgEditorState::saveSvgDoc()
{
    QRectF r = _scene->itemsBoundingRect();
    QString X = QString::number(r.x(), 'f', 0);
    QString Y = QString::number(r.y(), 'f', 0);
    QString W = QString::number(r.width(), 'f', 0);
    QString H = QString::number(r.height(), 'f', 0);
    QString svgHeader( QString("<svg version=\"1.1\" width=\"") + W + QString("\" height=\"") + H + QString("\" viewBox=\"") + X + QString(" ") + Y + QString(" ") + W + QString(" ") + H + QString("\">\n") );

    QString svgXml;
    svgXml.append(QString("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"));
    svgXml.append(svgHeader); //QString("<svg version=\"1.1\" width=\"1024\" height=\"1024\" viewBox=\"0 0 1024 1024\">\n"));

//    svgXml.append(QString("<line x1=\"0\" y1=\"0\" x2=\"0.0000000001\" y2=\"0.0000000001\" stroke-width=\"1\" stroke-opacity=\"0\" fill=\"none\" vector-effect=\"non-scaling-stroke\"/>\n"));
//    svgXml.append(QString("<polyline points=\"0,0 1000,1000\" stroke-width=\"1\" fill=\"none\" vector-effect=\"non-scaling-stroke\"/>\n"));

    foreach(SvgItem * item, _svgLineItems)
        svgXml.append(item->getXmlLine()); // r.topLeft()));

    svgXml.append(QString("</svg>"));

    QString filePath = TempDirController::createTempDirForCurrentUser() + QDir::separator() + "temp.svg";
    QFile file(filePath);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(svgXml.toUtf8());
        file.close();
    }

    emit svgDocSaved(filePath, r.topLeft());
}

void SvgEditorState::deleteSelectedItem()
{
    foreach(SvgItem* item, _selectedSvgItems)
    {
        for(auto it = _svgLineItems.begin(); it != _svgLineItems.end(); ++it)
            if( *it == item)
            {
                _svgLineItems.erase(it);
                break;
            }

        delete item;
    }
    _selectedSvgItems.clear();
    emit setEnabledDeleteButton(false);
    emit setEnabledSaveButton(_svgLineItems.isEmpty() == false);
}

void SvgEditorState::clearTempItems()
{
    foreach(transform_state::LineItem * item, _lineItems)
        delete item;

    if(_polygonItem)
    {
        delete _polygonItem;
        _polygonItem = nullptr;
    }

    _lineItems.clear();
}

transform_state::LineItem* SvgEditorState::createLineItem(QPointF p1, QPointF p2)
{
    StrokeParams strokeParams;
    emit getDefaultStrokeParams(strokeParams);

    transform_state::LineItem* item = new transform_state::LineItem(_scene, strokeParams.getLinePen());
    item->setLine(QLineF(p1, p2));
    item->setZValue(100000);
    return item;
}

void SvgEditorState::repaintRectArea(QPointF secondPoint)
{
    if(_linePoints.size() > 0)
    {
        clearTempItems();

        QRectF r(_linePoints.first(), secondPoint);
        QPolygonF pol(r.normalized());
        for(int i(1); i < pol.size(); ++i)
        {
            QPointF p1(pol.at(i-1));
            QPointF p2(pol.at(i));
            _lineItems.append(createLineItem(p1, p2));
        }
        _lineItems.append(createLineItem(pol.first(), pol.last()));
    }
}


void SvgEditorState::slotClickOnItem(bool withControl)
{
    if(_stateMode != StateMode::SelectMode)
        return;

    SvgItem* selectedSvgItem = dynamic_cast<SvgItem*>(sender());
    if(withControl)
    {
        for(auto it = _selectedSvgItems.begin(); it != _selectedSvgItems.end(); ++it)
            if( *it == selectedSvgItem)
            {
                _selectedSvgItems.erase(it);
                selectedSvgItem->selectItem(false);
                emit setEnabledDeleteButton(_selectedSvgItems.isEmpty() == false);
                return;
            }
    }
    else
    {
        foreach(SvgItem* item, _selectedSvgItems)
            item->selectItem(false);
        _selectedSvgItems.clear();
    }

    selectedSvgItem->selectItem(true);
    _selectedSvgItems.append(selectedSvgItem);
    emit setEnabledDeleteButton(true);
}





















/*
    QString svgXml;
    svgXml.append(QString("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"));
    svgXml.append(QString("<svg version=\"1.1\">\n"));
    svgXml.append(QString("<line x1=\"0\" y1=\"0\" x2=\"0.0000000001\" y2=\"0.0000000001\" stroke-width=\"1\" stroke-opacity=\"0\" fill=\"none\" vector-effect=\"non-scaling-stroke\"/>\n"));
    foreach(SvgItem * item, _svgLineItems)
        svgXml.append(item->getXmlLine());
    svgXml.append(QString("</svg>"));

    QString filePath = TempDirController::createTempDirForCurrentUser() + QDir::separator() + "temp.svg";
    QFile file(filePath);
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(svgXml.toUtf8());
        file.close();
    }

    QGraphicsSvgItem * svgItem = new QGraphicsSvgItem(filePath);
    _scene->addItem(svgItem);
    svgItem->setPos(0,0);
    svgItem->setZValue(1000000000);
*/





