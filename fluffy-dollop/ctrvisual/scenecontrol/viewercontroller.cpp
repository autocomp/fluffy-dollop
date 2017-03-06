#include "viewercontroller.h"
#include "scenecontroller.h"
#include <QDebug>
#include <QVBoxLayout>
#include "math.h"
#include "tilescene.h"
#include <QScrollBar>
#include <stdlib.h>
#include <QPushButton>
#include <QLineEdit>
#include <QtGui>
#include <QGridLayout>
#include <QToolTip>
#include <QMatrix>
#include <QClipboard>
#include <QRgb>
#include <QPen>
#include <QColor>
#include <QApplication>
#include <QVariant>
#include <QVariantMap>
#include <QTimer>
#include <QIcon>
#include <QMessageBox>
#include <QEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QInputEvent>
#include <QPaintEvent>


#include <iostream>
#include <exception>
#include <GeographicLib/PolygonArea.hpp>
#include <GeographicLib/Geodesic.hpp>
#include <GeographicLib/Constants.hpp>


#include "routescalegraphicsitem.h"
#include "parallelcursor.h"
#include "rasterlayer.h"
#include <ctrcore/visual/basescenelayer.h>
#include "reperpoint.h"
#include "scene2dwidget.h"
#include "anglegraphicsitem.h"
#include "squaregraphicsitem.h"
#include "searchrectgraphicsitem.h"
#include "reachgraphicsitem.h"
#include "lineresolutionitem.h"
//#include "ctrvisual/tmscutter/tmscutter.h"

//#include <objrepr/reprServer.h>
//#include <objrepr/pframe.h>

//#include <dpf/geo/geoimage_io.h>
//#include <dpf/service/immeasure/service.h>

#include <ctrcore/visual/visualizermanager.h>
#include <ctrcore/visual/selectionmodelinterface.h>

//using namespace objrepr;
using namespace visualize_system;

Q_DECLARE_METATYPE(QCursor*)

ViewerController::ViewerController(SceneController* sceneControlle, uint viewNumber, QWidget* parent)
    : QGraphicsView(parent)
    , m_sceneControlle(sceneControlle)
    , m_scale(1)
    , m_angle(0)
    , m_zoom(1)
    , MINI_VIEW_Z(5)
    , m_autoScroll(true)
    , m_centerViewOn(true)
    , m_viewNumber(viewNumber)
    //    , m_distScale(0)
    , pFirstPointPos(0,0)
    , pSecondPointPos(0,0)
    , pParallelCursor1(0)
    , pParallelCursor2(0)
    , pEditedBaseSceneLayer(0)
    , m_viewerState(SelectLayersAndScroll)
    , m_defaultMode(SelectLayersAndScroll)
    , m_miniView(new MiniMapViewer(sceneControlle->getMinimapScene(), parent))
    , m_timer(this)
    , TIME_CONST(100)
    , _backgroundItem(0)
    , m_middleZoomLevel(14)
    ,m_allowBoundMoveMap(true)

{
    m_browserId = 0;
    //m_pSendEventTimer = new QTimer(this);
    //    int dblClickInt = QApplication::doubleClickInterval();
    //    m_pSendEventTimer->setInterval(dblClickInt);
    //    m_pSendEventTimer->setSingleShot(true);
    //    connect(m_pSendEventTimer, SIGNAL(timeout()), this , SLOT(slotSendMouseEventTimeout()));


    m_pWaitMapMoveTimer = new QTimer(this);
    m_pWaitMapMoveTimer->setInterval(500);
    m_pWaitMapMoveTimer->setSingleShot(true);
    connect(m_pWaitMapMoveTimer, SIGNAL(timeout()), this , SLOT(slotWaitReleaseMouseTimeout()) );

    m_changeZoomOnCenter = true;
    m_zoomForSet = -1;
    m_changeZoomDelayTimer = new QTimer();
    m_changeZoomDelayTimer->setInterval(300);
    connect(m_changeZoomDelayTimer, SIGNAL(timeout()), this, SLOT(slotChangeZoomTimeout()));

    QTranslator *trans = new QTranslator(this);
    if(trans->load("./translate/" + QString("libscenewidget.qm")))
        QCoreApplication::installTranslator(trans);

    m_moveSource = MMSS_NO_SENDER;
    m_screenBounds = 5;

    m_boundsBrush = QBrush(qRgba(0xAD, 0xA7, 0xB1, 10));
    m_boundsPen = QPen(qRgba(0xAD, 0xA7, 0xB1, 50));

    m_screenRectWithoutBounds = QRect(m_screenBounds,m_screenBounds,width() - m_screenBounds, height() - m_screenBounds);

    m_leftBoundRect = QRect(0,0,m_screenBounds,height());
    m_rightBoundRect = QRect(width() - m_screenBounds,0,m_screenBounds,height());
    m_topBoundRect = QRect(0,0,width(),m_screenBounds);
    m_bottomBoundRect = QRect(0,height() - m_screenBounds,width(),m_screenBounds);

    m_pCalcAnglesItem = 0;
    m_pCalcSquareItem = 0;
    m_pCalcRouteItem = 0;
    m_pCalcLengthItem = 0;
    m_pCalcLineResolutionItem = 0;
    m_pReachGraphicsItem = 0;
    m_pSelectAreaRect = 0;
    m_pSelectEllipseAreaItem = 0;

    m_pAskRadiusDlg = 0;
    m_pSelectEllipseAreaWithSetRadItem = 0;
    m_ellipseSelectRadWithRad = 0;
    m_reachRadius = 0;
    m_calcLineResolutionPoly.clear();


    m_prevState = m_viewerState;

    m_pViewRectPartLayer = 0;

    if(!QResource::registerResource( "resources/resources.rcc" ))
        qDebug() << "libscenecontrol - no resource file (resources.rcc) !";

    setScene(m_sceneControlle->getScene());
    m_lastGoodViewportPosForBind = QPointF(0,0);
    m_pBindedLayer = 0;
    m_blockRotate = false;
    m_blockRotateFromKompas = false;
    m_blockScrollAndSelect = false;
    m_blockSelectAndScrollOver = false;

    setMouseTracking(true);
    viewport()->setMouseTracking(true);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_viewPortController = new ViewPortController(m_sceneControlle);
    connect(m_viewPortController, SIGNAL(setViewEnabled(bool)), this, SLOT(setEnabled(bool)));

    m_continuousMoveTimer = new QTimer();
    m_continuousMoveTimer->setInterval(20);
    connect(m_continuousMoveTimer, SIGNAL( timeout()), this, SLOT(slotMoveTimerTimeout()) );

    //installEventFilter(this);

    QTimer *testDebugTmr = new QTimer(this);
    testDebugTmr->start(1000);
    connect(testDebugTmr, SIGNAL(timeout()), this, SLOT(slotDebugOut()) );

    const QRectF r(getViewportRect());
    m_viewPortController->update(m_zoom, r, true);
    updateMiniViewer(r);

    QPointF wgs84_1(m_sceneControlle->sceneToGeo2( mapToScene(rect().center())));
    QPointF wgs84_2(m_sceneControlle->sceneToGeo2( mapToScene(rect().center().x() + 1, rect().center().y())));
    qreal mInPixel = m_sceneControlle->getDistanceWGS84(wgs84_1,wgs84_2);
    emit sigSetInterpixelDistance(mInPixel);

    m_viewerState = SelectLayersAndScroll;

//    connect(contur::TmsCutter::instance(), SIGNAL(dataReady(int,int)), this, SLOT(slotGeoImageCutted(int, int)));
    connect(m_miniView, SIGNAL(clickOnMinimap(QPointF)), this, SLOT(viewCenterOn(QPointF)));
    connect(m_miniView, SIGNAL(signalWheelEvent(QWheelEvent*)), this, SLOT(slotWheelEvent(QWheelEvent*)));

    if(sceneControlle->isGeo())
    {
        connect(&m_timer, SIGNAL(timeout()), this, SLOT(slotCheckUpdate()));
        m_timer.start(TIME_CONST);

        _backgroundItem = new BackgroundItem(m_sceneControlle->getScene());
        _backgroundItem->setZValue(30);
        connect(m_viewPortController, SIGNAL(signalUpdateBackgroundItem(QRect)), this, SLOT(slotUpdateBackgroundItem(QRect)), Qt::QueuedConnection);

//        connect(m_sceneControlle->getSceneInThread(), SIGNAL(finishRender(QImage*,QRectF,int)), this, SLOT(slotFinishRender(QImage*,QRectF,int)), Qt::QueuedConnection);
        connect(m_sceneControlle->getTileSceneAsinc(), SIGNAL(finishRender(QImage*,QRectF,int)), this, SLOT(slotFinishRender(QImage*,QRectF,int)), Qt::QueuedConnection);
        connect(this, SIGNAL(updateFromZoom(int,QRectF,bool)), m_viewPortController, SLOT(update(int,QRectF,bool)));
    }
}

void ViewerController::slotDebugOut()
{
    //qDebug() << "ViewerController::slotDebugOut():  " << m_debugMousePos.x() << " , " << m_debugMousePos.y();
}

bool ViewerController::eventFilter(QObject *obj, QEvent *event)
{
    if( (event->type() == QEvent::Wheel) )
    {
        QWheelEvent * e = dynamic_cast<QWheelEvent*>(event);
        wheelEvent(e);
        event->accept();
    }

    if( event->type() == QEvent::KeyRelease)
    {
        QKeyEvent * e = dynamic_cast<QKeyEvent*>(event);
        //keyReleaseEvent(e);
        event->accept();
    }



    if(event->type() == QEvent::KeyPress )
    {
        QKeyEvent * e = dynamic_cast<QKeyEvent*>(event);
        // keyPressEvent(e);
        event->accept();
    }
}


ViewerController::~ViewerController()
{
    m_continuousMoveTimer->stop();
    delete m_continuousMoveTimer;
    delete m_viewPortController;
}


QGraphicsScene* ViewerController::getScene()
{
    return m_sceneControlle->getScene();
}


void ViewerController::autoScroll(QPointF scenePos)
{
    //    qDebug() << "ViewerController::autoScroll, scenePos :" << scenePos;
    if(m_autoScroll && getViewportRect().contains(scenePos) == false)
        centerOn(scenePos);
}

void ViewerController::viewCenterOn(QPointF scenePos)
{
    centerOn(scenePos);
}

void ViewerController::centerViewOn(QPointF scenePos)
{
    if(getViewportRect().contains(scenePos) == false) //m_autoScroll && m_centerViewOn)
        centerOn(scenePos);
}


int ViewerController::getPrefferZoomForSceneRect(QRectF rect, int minVizZoom, int maxVizZoom, BaseSceneLayer::SceneLayerType slt)
{

    if(BaseSceneLayer::SLT_POINT_OBJECT == slt || (0 == rect.width() && 0 == rect.height()))
    {
        if(maxVizZoom == 0 && 0 == minVizZoom)
        {
            return m_middleZoomLevel;
        }
        else
        {
            int middleZoom = (maxVizZoom + minVizZoom)/2;

            if(m_middleZoomLevel < middleZoom)
            {
                return m_middleZoomLevel;
            }

            return middleZoom;
        }
        /*у точечных объектов в основном размеры иконки изменяются от зума, поэтому позиционироваться по brect не надо*/
        if(m_zoom >maxVizZoom && m_zoom < minVizZoom)
        {
            /*объект уже виден, не надо дергать зумом*/
            return maxVizZoom;
        }
        else
        {
            return maxVizZoom;
        }
    }

    QRectF viewRect = getViewportRect();
    int z_level = 1;

    //считаем зум

//    qDebug() << "ViewerController::getPrefferZoomForSceneRect, rect" << rect << "minVizZoom" << minVizZoom << "maxVizZoom" << maxVizZoom << "viewRect" << viewRect;

    if(0 != rect.width() && 0 != rect.height())
    {
        qreal prefferZoomForWidth = qAbs( (rect.width() / viewRect.width() ) );
        qreal prefferZoomForHeight = qAbs( (rect.height() / viewRect.height()) );

        qreal zoom = 1;
        z_level = m_zoom;

//        qDebug() << "ViewerController::getPrefferZoomForSceneRect IF, prefferZoomForWidth" << prefferZoomForWidth << "prefferZoomForHeight" << prefferZoomForHeight << "z_level" << z_level;

        if(prefferZoomForHeight < prefferZoomForWidth)
        {
            zoom = prefferZoomForWidth;
        }
        else
        {
            zoom = prefferZoomForHeight;
        }


        // теперь ищем Z уровень


        if(zoom > 1/*1.4*/ )
        {
            while( zoom > 1/*1.4*/  )
            {
                z_level--;
                zoom = zoom/2;
            }
        }
        else
        {
            if(zoom < 1/*0.7*/ )
                while( zoom < 1/*0.7*/  )
                {
                    z_level++;
                    zoom = zoom*2;
                }

            z_level--;
        }
    }
    else
    {
        z_level = m_zoom;
    }

    if(z_level > (int)m_sceneControlle->zoomMax())
    {
        z_level = (int)m_sceneControlle->zoomMax();
    }

    if(z_level < (int)m_sceneControlle->zoomMin())
    {
        z_level = (int)m_sceneControlle->zoomMin();
    }


    //    if(z_level < maxVizZoom && 0 != maxVizZoom)
    //    {
    //        z_level = maxVizZoom;
    //    }

    //    if(z_level > minVizZoom && 0 != maxVizZoom)
    //    {
    //        z_level = minVizZoom;
    //    }


    return z_level;
}

void ViewerController::setPrefferZoomForGeoRect(QRectF rect)
{
    QPolygonF scenePol;
    QPolygonF geoPol(rect);
    foreach(QPointF p, geoPol)
        scenePol.append(m_sceneControlle->geoToScene(QPointF(p.y(), p.x())));
    rect = scenePol.boundingRect();

    if(rect.width() > 0 && rect.height() > 0)
        setPrefferZoomForSceneRect(rect);
}

void ViewerController::setPrefferZoomForSceneRect(QRectF rect)
{
    QRectF viewRect = getViewportRect();
    int z_level = 1;

    qreal prefferZoomForWidth = qAbs( (rect.width() / viewRect.width() ) );
    qreal prefferZoomForHeight = qAbs( (rect.height() / viewRect.height()) );

    qreal zoom = 1;
    z_level = m_zoom;

    //        qDebug() << "ViewerController::getPrefferZoomForSceneRect IF, prefferZoomForWidth" << prefferZoomForWidth << "prefferZoomForHeight" << prefferZoomForHeight << "z_level" << z_level;

    if(prefferZoomForHeight < prefferZoomForWidth)
    {
        zoom = prefferZoomForWidth;
    }
    else
    {
        zoom = prefferZoomForHeight;
    }

    // теперь ищем Z уровень

    if(zoom > 1/*1.4*/ )
    {
        while( zoom > 1/*1.4*/  )
        {
            z_level--;
            zoom = zoom/2;
        }
    }
    else
    {
        if(zoom < 1/*0.7*/ )
            while( zoom < 1/*0.7*/  )
            {
                z_level++;
                zoom = zoom*2;
            }

        z_level--;
    }

    if(z_level > (int)m_sceneControlle->zoomMax())
    {
        z_level = (int)m_sceneControlle->zoomMax();
    }

    if(z_level < (int)m_sceneControlle->zoomMin())
    {
        z_level = (int)m_sceneControlle->zoomMin();
    }

    changeZoom(z_level);
    centerOn(rect.center());
}


bool ViewerController::setViewCenterOnObject(quint64 id)
{
    foreach(QGraphicsItem* item, items())
    {
        BaseSceneLayer* layer = dynamic_cast<BaseSceneLayer*>(item);
        if(layer)
            if(layer->getId() == id)
            {
                QRectF r(layer->getRect());
                QRectF viewportRect = getViewportRect();
                double viewportW(viewportRect.width()), viewportH(viewportRect.height());
                //                qDebug() << "viewportRect" << viewportRect << "R" << r;
                int zoomDown(0);
                while( viewportW < r.width() || viewportH < r.height())
                {
                    viewportW *= 2.;
                    viewportH *= 2.;
                    ++zoomDown;
                    //                    qDebug() << "--- viewportW :" << viewportW << "viewportH" << viewportH;
                }

                //                qDebug() << "zoomDown" << zoomDown << "m_zoomMin" << m_zoomMin;

                if(zoomDown != 0)
                    changeZoom( (m_zoom - zoomDown) < m_zoomMin ? m_zoomMin : m_zoom - zoomDown );

                centerOn(r.center());
                return true;
            }
    }
    return false;
}


void ViewerController::centerViewOnWithZoom(quint64 id, QRectF bRect, int minVizZoom, int maxVizZoom, bool withZoom, bool force )
{
    bool inView = false;
    QRectF viewportRect = getViewportRect();
    BaseSceneLayer::SceneLayerType layerType = BaseSceneLayer::SLT_UNKNOWN;

    QList<QGraphicsItem*>list(items(rect(),Qt::IntersectsItemShape));
    for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
    {
        BaseSceneLayer* layer = dynamic_cast<BaseSceneLayer*>(*it);
        if(0 != layer)
        {
            layerType = layer->getLayerType();
            if(m_selectedFromMapid == layer->getMyId() && id == m_selectedFromMapid)
            {
                if(layer->isSelectable(viewportRect))
                {
                    inView = true;
                }
                break;
            }
        }
    }

    if(force || !inView || withZoom)
    {
//        qDebug() << "ViewerController::centerViewOnWithZoom, IF, withZoom" << withZoom;
        if(withZoom)
        {
            int zoom = getPrefferZoomForSceneRect(bRect,minVizZoom,maxVizZoom, layerType);
//            qDebug() << "ViewerController::centerViewOnWithZoom, IF, zoom" << zoom;
            changeZoom(zoom);
        }
        centerOn(bRect.center());

    }
    else
    {
//        qDebug() << "ViewerController::centerViewOnWithZoom, ELSE";
    }
}


void ViewerController::keyReleaseEvent(QKeyEvent * e)
{

    int kk = e->key();

    UserAction act;
    act.keybKey = (Qt::Key)e->key();
    act.type = e->type();
    act.modifiers = e->modifiers();

    act.epsgCode = 0;
    m_sceneControlle->emit_signalUserViewControlAction(act);

    switch(m_viewerState)
    {
    case SelectLayersAndScroll :
    {
        if(e->key() == Qt::Key_Alt)
        {
            setDragMode(QGraphicsView::ScrollHandDrag);
        }

        if(e->key() == Qt::Key_A )
        {
            if(MMSS_KEYS == m_moveSource)
            {
                slotStopMoveMap();
                m_moveSource = MMSS_NO_SENDER;
            }

        }

        if(e->key() == Qt::Key_D )
        {
            if(MMSS_KEYS == m_moveSource)
            {
                slotStopMoveMap();
                m_moveSource = MMSS_NO_SENDER;
            }
        }


        if(e->key() == Qt::Key_Space )
        {
            if(MMSS_KEYS == m_moveSource)
            {
                slotStopMoveMap();
                m_moveSource = MMSS_NO_SENDER;
            }
        }

        if(e->key() == Qt::Key_C )
        {
            if(MMSS_KEYS == m_moveSource)
            {
                slotStopMoveMap();
                m_moveSource = MMSS_NO_SENDER;
            }
        }

        if(e->key() == Qt::Key_Left
                || e->key() == Qt::Key_Right
                || e->key() == Qt::Key_Up
                || e->key() == Qt::Key_Down
                )
        {
            return;
        }
    }break;
    case EditObjects:
//    case SetReperPoints:
        if(e->key() == Qt::Key_Control)
            setDragMode(QGraphicsView::ScrollHandDrag);
        break;

    case MeasureAngles:
    {
        if(e->key() == Qt::Key_Escape)
            emit signalSwitchOffButton((int)sw::BTN_CALC_ANGLES);

        if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
        {
            if(0 != m_pCalcAnglesItem && m_calcAnglePoly.count() < 4 && m_calcAnglePoly.count() > 0)
            {
                m_calcAnglePoly.append(m_calcAnglePoly.last());

                m_pCalcAnglesItem->setPolygon(m_calcAnglePoly);
            }
        }
    }break;

    case MeasureRoute:
    {
        if(e->key() == Qt::Key_Escape)
            emit signalSwitchOffButton((int)sw::BTN_CALC_ROUTE);

        if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
        {
            if(0 != m_pCalcRouteItem && m_calcRoutePoly.count() > 0)
            {
                m_calcRouteShowTempWay = false;
                m_pCalcRouteItem->setPolygon(m_calcRoutePoly);
            }
        }

    }break;

    case MeasureLength:
    {
        if(e->key() == Qt::Key_Escape)
            emit signalSwitchOffButton((int)sw::BTN_CALC_LENGHT);

        if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
        {
            if(0 != m_pCalcLengthItem && m_calcLengthPoly.count() > 0)
            {
                m_calcLengthShowTempWay = false;
            }
        }


    }break;

    case MeasureReach:
    {
        if(e->key() == Qt::Key_Escape)
            emit signalSwitchOffButton((int)sw::BTN_CALC_REACH);

        if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
        {
            m_pReachGraphicsItem->finishSelecting(true);
        }
    }break;

    case MeasureLineResolution:
    {
        if(e->key() == Qt::Key_Escape)
            emit signalSwitchOffButton((int)sw::BTN_DETECT_LINE_RESOLUTION);

        if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
        {
            if(0 != m_pCalcLineResolutionItem && m_calcLineResolutionPoly.count() > 0)
            {
                if(!m_pCalcLineResolutionItem->isSelectingFinished())
                {
                    m_pCalcLineResolutionItem->setPolygon(m_calcLineResolutionPoly);
                    m_pCalcLineResolutionItem->finishSelecting(true);
                }
            }
        }

    }

    case MeasureSquare:
    {
        if(e->key() == Qt::Key_Escape)
            emit signalSwitchOffButton((int)sw::BTN_CALC_SQUARE);

        if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return)
        {
            if(0 != m_pCalcSquareItem && m_calcSquarePoly.count() > 0)
            {
                if(!m_pCalcSquareItem->isSelectingFinished())
                {
                    m_pCalcSquareItem->setPolygon(m_calcSquarePoly);
                    m_pCalcSquareItem->finishSelecting(true);
                }
                return;
            }
        }
    }break;

    case StateObjectHandlerEvents:
    {
        if(m_stateObject.isNull() == false)
        {
            if( m_stateObject->keyReleaseEvent(e) == false ) // если состояние хочет отдать событие вьюхе - отдаём !
                return;
        }
    }
        break;
    }

    QGraphicsView::keyReleaseEvent(e);
}


void ViewerController::slotBindLayer(quint64 layerId, bool isBinded)
{

    /*по решению АНАЛитиков [I-3034] режимы bind и attach совпадают*/
    isBinded = false;

    if(0 == layerId)
    {
        switchViewerStateMode(SelectLayersAndScroll, true);
    }

    if((m_blockSelectAndScrollOver != isBinded) || (0 == m_pBindedLayer))
    {
        QVariant var;
        QVariantMap varList;
        varList.insert("LayerId", layerId);
        varList.insert("IsBinded",isBinded);
        var = varList;
        slotDetectViewerMode("slotBindLayer", var);
    }
    else
    {
        if(/*DenyScrollOverAndSelect == m_viewerState &&*/ isBinded && 0 != m_pBindedLayer)
        {
            if(isBinded)
            {
                moveBindedLayerInViewRect();
                QRectF objRect = m_pBindedLayer->boundingRect();
                QPointF layerCenter = objRect.center();
                QRectF viewRect = getViewportRectPart(objRect.width(),objRect.height());
                QRectF viewRect1 = getViewportRectPart(objRect.width()*2,objRect.height()*2);

                if(!viewRect1.contains(layerCenter) )
                    centerOn(viewRect.center().x() + layerCenter.x() - m_bindingLayerPrevCenterPos.x(), viewRect.center().y() + layerCenter.y() - m_bindingLayerPrevCenterPos.y() );

            }
            else
            {
                moveBindedLayerInViewRect();
                QRectF objRect = m_pBindedLayer->boundingRect();
                QPointF layerCenter = objRect.center();
                QRectF viewRect = getViewportRectPart(objRect.width(),objRect.height());
                QRectF viewRect1 = getViewportRectPart(objRect.width()*2,objRect.height()*2);

                //if(!viewRect1.contains(layerCenter) )
                    centerOn(layerCenter.x(), viewRect.center().y() );

            }
        }
    }

    QRectF rect = getViewportRect();

    if(0 != m_pBindedLayer)
        m_bindingLayerPrevCenterPos = m_pBindedLayer->boundingRect().center();
    m_blockSelectAndScrollOver = isBinded;
}

void ViewerController::keyPressEvent(QKeyEvent * e)
{
    switch(m_viewerState)
    {
    case SelectLayersAndScroll :
    {
        Qt::Key kk= (Qt::Key)e->key();
        if(e->key() == Qt::Key_Alt)
        {
            setDragMode(QGraphicsView::NoDrag);
            setCursor(Qt::ArrowCursor);
        }
//        else if(m_sceneControlle->sceneIsGeo() == false && e->key() == Qt::Key_Delete)
//            deleteZonesInteresting();

        if(e->key() == Qt::Key_S)
        {
            UserAction act;
            act.keybKey = Qt::Key_S;
            act.modifiers = Qt::ControlModifier;
            act.type = e->type();

            m_sceneControlle->emit_signalUserViewControlAction(act);
        }

        if(e->key() == Qt::Key_Plus || e->key() == Qt::Key_W)
        {
            changeZoom(m_zoom+1,false);
        }

        if(e->key() == Qt::Key_Minus || e->key() == Qt::Key_S)
        {
            changeZoom(m_zoom-1,false);
        }

        if(e->key() == Qt::Key_A )
        {
            if( MMSS_NO_SENDER == m_moveSource)
            {
                m_moveSource = MMSS_KEYS;
                m_continuousMoveTimer->start();
//                m_orient = Kompas::EAST;
            }
        }

        if(e->key() == Qt::Key_D )
        {
            if( MMSS_NO_SENDER == m_moveSource)
            {
                m_moveSource = MMSS_KEYS;
                m_continuousMoveTimer->start();
//                m_orient = Kompas::WEST;
            }
        }


        if(e->key() == Qt::Key_Space )
        {
            if( MMSS_NO_SENDER == m_moveSource)
            {
                m_moveSource = MMSS_KEYS;
                m_continuousMoveTimer->start();
//                m_orient = Kompas::NORTH;
            }
        }

        if( e->key() == Qt::Key_C )
        {
            if( MMSS_NO_SENDER == m_moveSource)
            {
                m_moveSource = MMSS_KEYS;
                m_continuousMoveTimer->start();
//                m_orient = Kompas::SOUTH;
            }
        }

        if(e->key() == Qt::Key_Left
                || e->key() == Qt::Key_Right
                || e->key() == Qt::Key_Up
                || e->key() == Qt::Key_Down
                )
        {
            return;
        }

        if(e->key() == Qt::Key_AsciiTilde)
        {

            QList<quint64> ids;

            bool listContains = m_objectsUnderMouseForSelect.contains(m_selectedFromMapid);

            if(!m_objectsUnderMouseForSelect.isEmpty())
            {
                {
                    bool prev = false;
                    if(listContains && m_selectedFromMapid != m_objectsUnderMouseForSelect.first())
                    {
                        QListIterator<quint64> it(m_objectsUnderMouseForSelect);
                        it.toBack();
                        //foreach (quint64 id, m_objectsUnderMouseForSelect)
                        while(it.hasPrevious())
                        {
                            quint64 id = it.previous();
                            if(prev)
                            {
                                m_selectedFromMapid = id;
                                m_prevObjectsUnderMouse.clear();
                                m_prevObjectsUnderMouse.append(m_selectedFromMapid);
                                emit signalSelectObject(m_selectedFromMapid);
                                break;
                            }
                            if(id == m_selectedFromMapid)
                            {
                                prev = true;
                                continue;
                            }
                        }
                    }
                    else
                    {
                        m_selectedFromMapid = m_objectsUnderMouseForSelect.last();
                        m_prevObjectsUnderMouse.clear();
                        m_prevObjectsUnderMouse.append(m_selectedFromMapid);

                        emit signalSelectObject(m_selectedFromMapid);
                    }
                }
            }


//            objrepr::RepresentationServer::instance()->objectManager()->chooseObject(m_selectedFromMapid);

            return;
        }
        if(e->key() == Qt::Key_QuoteLeft)
        {

            QList<quint64> ids;

            bool listContains = m_objectsUnderMouseForSelect.contains(m_selectedFromMapid);

            if(!m_objectsUnderMouseForSelect.isEmpty())
            {
                {
                    /*прямой порядок*/
                    bool next = false;
                    if(listContains && m_selectedFromMapid != m_objectsUnderMouseForSelect.last())
                    {
                        foreach (quint64 id, m_objectsUnderMouseForSelect)
                        {
                            if(next)
                            {
                                m_selectedFromMapid = id;
                                m_prevObjectsUnderMouse.clear();
                                m_prevObjectsUnderMouse.append(m_selectedFromMapid);
                                emit signalSelectObject(m_selectedFromMapid);
                                break;
                            }
                            if(id == m_selectedFromMapid)
                            {
                                next = true;
                                continue;
                            }
                        }
                    }
                    else
                    {
                        m_selectedFromMapid = m_objectsUnderMouseForSelect.at(0);
                        m_prevObjectsUnderMouse.clear();
                        m_prevObjectsUnderMouse.append(m_selectedFromMapid);

                        emit signalSelectObject(m_selectedFromMapid);
                    }
                }
            }


//            objrepr::RepresentationServer::instance()->objectManager()->chooseObject(m_selectedFromMapid);

            return;
        }


    }break;

    case EditObjects:
    {
        if(e->key() == Qt::Key_S)
        {
            Qt::KeyboardModifiers mods = e->modifiers();
            UserAction act;
            act.keybKey = Qt::Key_S;
            act.modifiers = e->modifiers();
            act.type = e->type();

            m_sceneControlle->emit_signalUserViewControlAction(act);
        }break;
        if(e->key() == Qt::Key_Enter)
        {
            Qt::KeyboardModifiers mods = e->modifiers();
            UserAction act;
            act.keybKey = Qt::Key_S;
            act.modifiers = e->modifiers();
            act.type = e->type();

            m_sceneControlle->emit_signalUserViewControlAction(act);
        }break;

    }
        break;

    case StateObjectHandlerEvents:
    {
        if(m_stateObject.isNull() == false)
        {
            if( m_stateObject->keyPressEvent(e) == false ) // если состояние хочет отдать событие вьюхе - отдаём !
                return;
        }
    }
        break;
    default:
        ;
    }

    QGraphicsView::keyPressEvent(e);
}

void ViewerController::slotCreatingMode(bool on_off)
{
    pEditedBaseSceneLayer = 0;
    if(on_off)
    {
        BaseSceneLayer * layer = dynamic_cast<BaseSceneLayer*>(sender());
        if(layer)
        {
            pEditedBaseSceneLayer = layer;
            setDragMode(QGraphicsView::NoDrag);
            setCursor(Qt::ArrowCursor);
        }
    }
}

void ViewerController::slotSetLayerToCenterViewport(QPointF pos)
{
    if(getViewportRect().contains(pos) == false)
        centerOn(pos);
}



bool ViewerController::moveBindedLayerInViewRect()
{
    bool contains = false, intersect = false;

    QRectF scRectOut = getViewportRectPart(0,0);
    QRectF scRectIn = getViewportRectPart(viewportBindingPartPx,viewportBindingPartPx);

    if(0 != m_pViewRectPartLayer)
    {
        if(m_pViewRectPartLayer->rect() != scRectIn)
        {
            m_pViewRectPartLayer->setRect(scRectIn);
        }
    }


    /*m_bindingLayerIntersectPoint*/
    if( 0 != m_pBindedLayer && m_viewerState == DenyScrollOverAndSelect)
    {
        if(!m_pBindedLayer->isVisible())
            m_pBindedLayer->show();
        //while(!contains )
        {
            //QRectF scRect = m_pViewRectPartLayer->rect();//getViewportRectPart(viewportBindingPartPx,viewportBindingPartPx);
            //m_pBindedLayer->setRect(scRect);

            QPointF intersectPointTemp = QPointF(0,0);
            QRectF rect = m_pBindedLayer->rect();
            QRectF viewwRect = m_pViewRectPartLayer->rect();
            contains = rect.contains(viewwRect) || viewwRect.contains(rect);
            /*scRect.contains(m_pBindedLayer->boundingRect()) ||
                    m_pBindedLayer->boundingRect().contains(scRect) ;*/
            intersect = m_pBindedLayer->isSelectable(scRectIn, &intersectPointTemp);

            /*вычисляем точки*/
            if(contains)
            {
                m_bindingLayerIntersectPoint = QPointF(0,0);
            }

            if(intersect)
            {
                m_bindingLayerIntersectPoint = (intersectPointTemp - m_pBindedLayer->rect().center())/* / m_zoom*/;
            }


            if(contains || intersect)
            {
                m_lastGoodViewportPosForBind = getViewportRect().center();
                QPointF p1 = m_pBindedLayer->rect().center() + m_bindingLayerIntersectPoint;
                QLineF line ;
                line.setP1(p1 );
                line.setP2(m_lastGoodViewportPosForBind);
                //m_angleBetweenLayerAndViewCenter = line.angle();
                m_lenBetweenLayerAndViewCenter = line.length();
                /*считаем расстояние до центра объекта*/
                contains = true;
            }else
            {
                QPointF p = m_pBindedLayer->rect().center() + m_bindingLayerIntersectPoint;
                //m_pBindedLayer->ensureVisible(p.x(), p.y(),0,0,0,0);
                if(m_bindingLayerIntersectPoint != QPointF(0,0))
                {
                    QLineF line ;
                    line.setP1(p );
                    QPointF p2 = getViewportRect().center();
                    line.setP2(p2);
                    m_lenBetweenLayerAndViewCenter = m_lenBetweenLayerAndViewCenter*0.99;
                    line.setLength(m_lenBetweenLayerAndViewCenter);
                    centerOn(line.p2());
                }
                else
                {
                    centerOn(m_pBindedLayer->rect().center());
                }
            }
        }
    }
    else
    {
        if(0 != m_pBindedLayer)
            m_pBindedLayer->hide();
    }

    return contains || intersect;
}
void ViewerController::allowBoundMoveMap(bool res)
{
    m_allowBoundMoveMap = res;
}

//bool ViewerController::checkMousePos(QPoint pos, Kompas::KompasOrientation & orient)
//{
//    if(!m_allowBoundMoveMap)
//        return false;

//    bool posDetected = false;

//    if(!m_screenRectWithoutBounds.contains(pos,true))
//    {
//        if(m_leftBoundRect.contains(pos) )
//        {
//            posDetected = true;
//            orient = Kompas::EAST;
//        }

//        if(m_rightBoundRect.contains(pos) )
//        {
//            posDetected = true;
//            orient = Kompas::WEST;
//        }

//        if(m_topBoundRect.contains(pos) )
//        {
//            posDetected = true;
//            orient = Kompas::NORTH;
//        }


//        if(m_bottomBoundRect.contains(pos) )
//        {
//            posDetected = true;
//            orient = Kompas::SOUTH;
//        }

//        if(!posDetected)
//        {
//            posDetected = false;
//        }
//    }
//    else
//    {
//        posDetected = false;
//        slotStopMoveMap();
//    }

//    return posDetected;
//}

void ViewerController::mouseMoveEvent(QMouseEvent* e)
{
    const QPointF scenePos(mapToScene(e->pos().x(), e->pos().y()));

    if(sceneRect().contains(scenePos))
        m_sceneControlle->moveMouseInView(scenePos, m_zoom);

//    Kompas::KompasOrientation orient;
//    bool isAllowMoveMap = checkMousePos(e->pos(), orient);
//    if(isAllowMoveMap && MMSS_KOMPAS != m_moveSource)
//    {
//        m_moveSource = MMSS_SCREEN_BOUND;
//        m_continuousMoveTimer->start();
//        m_orient = orient;
//        m_mapMovingFromKompas = true;
//    }
//    else
//    {

//        if(MMSS_SCREEN_BOUND == m_moveSource)
//        {
//            slotStopMoveMap();
//            m_moveSource = MMSS_NO_SENDER;
//        }
//    }

    if(pEditedBaseSceneLayer)
    {
        pEditedBaseSceneLayer->setMouseEventInEditMode(e,scenePos);
        return;
    }

    switch(m_viewerState)
    {

    case CenterOnObject:
    {   if(dragMode() != QGraphicsView::NoDrag)
        {
            setDragMode(QGraphicsView::NoDrag);
        }
    }break;
    case DenyScrollOverAndSelect:
    {

        if(dragMode() != QGraphicsView::ScrollHandDrag)
        {
            setDragMode(QGraphicsView::ScrollHandDrag);
        }

        if( ( e->modifiers() == Qt::NoModifier) && ( 0 != (e->buttons() & Qt::LeftButton) ) )
        {
            bool allowEvent = moveBindedLayerInViewRect();
            if(allowEvent)
                QGraphicsView::mouseMoveEvent(e);
            return;
        }

    }break;
    case SelectLayersAndScroll :
        if(e->modifiers()&Qt::AltModifier && e->button()==Qt::NoButton)
        {
            if(sceneRect().contains(scenePos))
            {
                m_sceneControlle->moveParallelCursorInView(scenePos);
                setCursor(Qt::ArrowCursor);
            }
        }
        else
        {
            QList<QGraphicsItem*>list(items(e->pos()));
            for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
            {
                BaseSceneLayer* layer = dynamic_cast<BaseSceneLayer*>(*it);
                if(layer)
                    if(layer->isClickable(e, scenePos, m_scale))
                    {
                        if(dragMode() != QGraphicsView::NoDrag)
                        {
                            setDragMode(QGraphicsView::NoDrag);
                        }
                        m_sceneControlle->setHighLightBaseObject(layer);
                        setCursor(Qt::CrossCursor);
                        QGraphicsView::mouseMoveEvent(e);
                        return;
                    }
            }
            m_sceneControlle->setHighLightBaseObject();
            //            if(dragMode() != QGraphicsView::ScrollHandDrag)
            //            {
            //                QGraphicsView::mouseMoveEvent(e);
            //                setDragMode(QGraphicsView::ScrollHandDrag);
            //                setCursor(Qt::ArrowCursor);
            //                return;
            //            }



            if(e->buttons() & Qt::LeftButton)
            {
                m_viewerState = ScrollOnly;
                setDragMode(QGraphicsView::ScrollHandDrag);
            }
            else
            {
                setCursor(Qt::ArrowCursor);
            }
            QGraphicsView::mouseMoveEvent(e);
            return;
        }
        break;
    case SelectLayers_RectArea :
    {
        QPoint _p_ = mapFromScene(m_LeftButPressPos);
        QRect rect;
        rect.setRect(_p_.x(), _p_.y(), e->pos().x()-_p_.x(), e->pos().y()-_p_.y());
        m_pSelectAreaRect->setSelectRect(rect);
        QPolygonF pol;
        pol << mapToScene(rect.topLeft()) << mapToScene(rect.topRight()) << mapToScene(rect.bottomRight()) << mapToScene(rect.bottomLeft());
        m_pSelectAreaRect->setPolygon(pol);
        QPixmap pixmap(m_pSelectAreaRect->objectSelection() ==  SelectRectArea::ObjreprLayers ?
                           ":/img/select_obj.png" :
                           ":/img/select_img.png" );
        setCursor(QCursor(pixmap, 0, 0));
        break;
    }
    case MeasureAngles:
    {
        if(0 != m_pCalcAnglesItem && m_calcAnglePoly.count() > 0 && m_calcAnglePoly.count() < 4)
        {
            m_calcAnglePoly.remove(m_calcAnglePoly.count() - 1);
            m_calcAnglePoly.append(scenePos);
            m_pCalcAnglesItem->setPolygon(m_calcAnglePoly);
            return;
        }
    }break;
    case MeasureRoute:
    {
        if(0 != m_pCalcRouteItem && m_calcRouteShowTempWay && m_calcRoutePoly.count() > 0)
        {
            m_calcRoutePoly.remove(m_calcRoutePoly.count() - 1);
            m_calcRoutePoly.append(scenePos);
            m_pCalcRouteItem->setPolygon(m_calcRoutePoly);
            return;
        }
    }break;

    case MeasureLength:
    {
        if(0 != m_pCalcLengthItem && m_calcLengthShowTempWay && m_calcLengthPoly.count() > 0)
        {
            m_calcLengthPoly.remove(m_calcLengthPoly.count() - 1);
            m_calcLengthPoly.append(scenePos);
            m_pCalcLengthItem->setPolygon(m_calcLengthPoly);
            return;
        }
    }break;

    case MeasureLineResolution:
    {
        if(0 != m_pCalcLineResolutionItem && m_calcLineResolutionPoly.count() > 0)
        {
            if(!m_pCalcLineResolutionItem->isSelectingFinished())
            {
                m_calcLineResolutionPoly.remove(m_calcLineResolutionPoly.count() - 1);
                m_calcLineResolutionPoly.append(scenePos);
                m_pCalcLineResolutionItem->setPolygon(m_calcLineResolutionPoly);
            }

            return;
        }
    }break;

    case MeasureReach:
    {        if(0 != m_pReachGraphicsItem )
        {
            if( !m_pReachGraphicsItem->isSelectingFinished() && m_pReachGraphicsItem->isCenterSetted() )
            {
                m_pReachGraphicsItem->setRadiusPoint(scenePos);
                QPolygonF poly = m_pReachGraphicsItem->ellipsePath().toFillPolygon();
                QPolygonF scenePoly;

                foreach (QPointF p, poly)
                {
                    scenePoly << mapFromScene(p.x(), p.y());
                }

                //selectReachArea(poly);
            }
        }
    }break;

    case MeasureSquare:
    {
        if(0 != m_pCalcSquareItem && m_calcSquarePoly.count() > 0)
        {
            if(!m_pCalcSquareItem->isSelectingFinished())
            {
                if(m_calcSquarePoly.count() > 1)
                {
                    m_calcSquarePoly.remove(m_calcSquarePoly.count() - 1);
                    m_calcSquarePoly.append(scenePos);
                    m_pCalcSquareItem->setPolygon(m_calcSquarePoly);
                }
            }
            else
            {
                m_pCalcSquareItem->finishSelecting(true);
            }
            return;
        }
    }break;

    case EditObjects:
    {
        /*if( 0 != (e->modifiers() & Qt::ControlModifier) )
        {

        }
        else
        {*/

        if(!m_waitMoveTimerCompleted)
        {
            bool editRectFound = false;
            QList<QGraphicsItem*>list(items(e->pos()));
            for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
            {
                QGraphicsItem* item(*it);
                if(item)
                    if(item->type() == 666666)
                    {
                        if(dragMode() != QGraphicsView::NoDrag)
                        {
                            setDragMode(QGraphicsView::NoDrag);
                            editRectFound = true;

                        }
                        setCursor(Qt::CrossCursor);
                        return;
                    }
            }
        }

        if(dragMode() == QGraphicsView::NoDrag ||
                (dragMode() != QGraphicsView::NoDrag && (0 == e->modifiers()) && Qt::NoButton == e->buttons()  ) )
        {
            setDragMode(QGraphicsView::NoDrag);
            setCursor(m_addVectorObjectCursor);
        }
    }break;
    case ScrollOnly:
    {
        QGraphicsView::mouseMoveEvent(e);
    }return;
    case StateObjectHandlerEvents:
    {
        if(e->modifiers()&Qt::AltModifier && e->button()==Qt::NoButton)
            if(sceneRect().contains(scenePos))
                m_sceneControlle->moveParallelCursorInView(scenePos);

        if(m_stateObject.isNull() == false)
        {
            bool setEventToView(false);
            if( m_stateObject->mouseMoveEvent(e, scenePos) == true ) // если состояние хочет отдать событие вьюхе - отдаём !
                setEventToView = true;

            bool object_processed = false;
            if( m_stateObject->checkableObjectUnderMouse() )
            {
                quint64 objectId(0);
                QList<QGraphicsItem*>list(items(e->pos()));
                for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
                {
                    BaseSceneLayer* layer = dynamic_cast<BaseSceneLayer*>(*it);
                    if(layer)
                        if(layer->isClickable(e, scenePos, m_scale))
                        {
                            QCursor cursor(Qt::CrossCursor);
                            if(m_stateObject->checkObjectUnderMouse(layer->getId(), cursor))
                            {
                                setCursor(cursor);
                                m_sceneControlle->setHighLightBaseObject(layer);
                                objectId = layer->getId();
                                object_processed = true;
                                break;
                            }
                        }
                }

                m_stateObject->objectChoiced(objectId);

                if(objectId == 0 )
                {
                    m_sceneControlle->setHighLightBaseObject();
                    if( !m_stateObject->checkableRasterUnderMouse() )
                        setCursor(QCursor(Qt::ArrowCursor));
                }
            }

            if( !object_processed
                    && m_stateObject->checkableRasterUnderMouse() )
            {
                uint rasterId(0);
                visualize_system::RasterLayer* rasterLayer(0);
                QList<QGraphicsItem*>list(items(e->pos()));
                for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
                {
                    if( (*it)->type() == visualize_system::RASTER_LAYER_ITEM )
                    {
                        visualize_system::RasterLayerItem* _rasterLayerItem = dynamic_cast<visualize_system::RasterLayerItem*>(*it);
                        if(_rasterLayerItem)
                            if(_rasterLayerItem->getRasterLayerItemType() == visualize_system::RasterLayerItemType::PolygonLayer)
                                if(_rasterLayerItem->getRasterLayer()->containsPoint(scenePos))
                                {
                                    rasterLayer = _rasterLayerItem->getRasterLayer();
                                    break;
                                }
                    }
                }

                if(rasterLayer)
                    rasterId = rasterLayer->id();

                m_stateObject->rasterUnderMouse(rasterId);
            }

            if(setEventToView == false)
                return;
        }
    }break;

    }

    QGraphicsView::mouseMoveEvent(e);
}


//void ViewerController::slotOneStepMoveMap(Kompas::KompasOrientation orient)
//{
//    m_orient = orient;
//    moveMap(10);
//}


void ViewerController::moveMap(double partOfScreen)
{
    if(m_blockScrollAndSelect)
        return;

    double movedPxX = (viewport()->width()/partOfScreen)*sin(m_angle);
    double movedPxY = (viewport()->height()/partOfScreen)*cos(m_angle);

    double degToRad = (M_PI/180.0);

    double angl = m_angle;


//    switch(m_orient)
//    {
//    case Kompas::SOUTH:
//    {
//        movedPxX = (viewport()->width()/(partOfScreen*m_scale)) *qSin( degToRad*(-m_angle + 180));
//        movedPxY = (viewport()->width()/(partOfScreen*m_scale)) *qCos(degToRad*(-m_angle + 180));
//    }break;
//    case Kompas::WEST:
//    {
//        movedPxX = (viewport()->width()/(partOfScreen*m_scale)) *qSin( degToRad*(-m_angle + 90));
//        movedPxY = (viewport()->width()/(partOfScreen*m_scale)) *qCos(degToRad*(-m_angle + 90));
//    }break;
//    case Kompas::EAST:
//    {
//        movedPxX = (viewport()->width()/(partOfScreen*m_scale)) *qSin( degToRad*(-m_angle + 270));
//        movedPxY = (viewport()->width()/(partOfScreen*m_scale)) *qCos(degToRad*(-m_angle + 270));
//    }break;
//    case Kompas::NORTH:
//    {
//        movedPxX = (viewport()->width()/(partOfScreen*m_scale)) *qSin( degToRad*(-m_angle + 0));
//        movedPxY = (viewport()->width()/(partOfScreen*m_scale)) *qCos(degToRad*(-m_angle + 0));

//    }break;
//    }

    centerOn(getViewportRect().center().x() + movedPxX , getViewportRect().center().y() - movedPxY);

    moveBindedLayerInViewRect();


    return;
}

void ViewerController::slotMoveTimerTimeout()
{

//    QPoint globalMousePos = QCursor::pos();
//    QPoint  localPos = mapFromGlobal(globalMousePos);

//    Kompas::KompasOrientation orient;
//    if(MMSS_KOMPAS == m_moveSource || MMSS_KEYS == m_moveSource)
//    {
//        moveMap(100.0);
//    }
//    else
//    {
//        bool isAllowMoveMap = checkMousePos(localPos, orient);
//        if(isAllowMoveMap)
//        {
//            moveMap(100.0);
//        }
//        else
//        {
//            slotStopMoveMap();
//        }
//    }
}

//void ViewerController::slotContinuousMoveMap(Kompas::KompasOrientation orient)
//{
//    if(!m_continuousMoveTimer->isActive())
//        m_continuousMoveTimer->start();
//    m_orient = orient;
//    m_moveSource = MMSS_KOMPAS;
//    m_mapMovingFromKompas = true;
//}

void ViewerController::slotStopMoveMap()
{
    // qDebug() << "123";
    m_continuousMoveTimer->stop();
    m_mapMovingFromKompas = false;
    m_moveSource = MMSS_NO_SENDER;
    update();
}

void ViewerController::mouseReleaseEvent(QMouseEvent *e)
{
//    Kompas::KompasOrientation orient;
//    bool isAllowMoveMap = checkMousePos(e->pos(), orient);
//    if(isAllowMoveMap)
//    {
//        //slotContinuousMoveMap(orient);
//        m_continuousMoveTimer->start();
//        m_orient = orient;
//        m_mapMovingFromKompas = true;
//    }
//    else
//    {
//        slotStopMoveMap();
//    }

    QPointF scenePos = mapToScene(e->pos().x(), e->pos().y());

    if(pEditedBaseSceneLayer)
    {
        pEditedBaseSceneLayer->setMouseEventInEditMode(e,scenePos);
        m_waitMoveTimerCompleted = false;
        return;
    }

    if(m_viewerState == SelectScroll)
    {
        QVariant onOff = false;
        slotDetectViewerMode("SelectScrollOff", onOff);
        mouseReleaseEvent(e);
    }
    else
    {
        if(e->button() == Qt::RightButton)
        {
            switch(m_viewerState)
            {
//            case SelectLayers_EllipseArea:
//            {
//                switchViewerStateMode(SelectLayers_EllipseArea, false);
//            }break;
            default:
            {

            }break;
            };

        }
        if(e->button() == Qt::LeftButton)
        {
            switch(m_viewerState)
            {
            case ScrollOnly:
            {
                m_viewerState = SelectLayersAndScroll;
            }break;
            case SelectLayersAndScroll:
            {
                QGraphicsView::mouseReleaseEvent(e);
                if(Qt::LeftButton == e->button())
                {
                    setDragMode(QGraphicsView::NoDrag);
                    setCursor(QCursor(Qt::ArrowCursor));
                }
                m_waitMoveTimerCompleted = false;
                return;
            }break;
            case EditObjects:
            {

                bool mouseOnEditRect = false;
                QList<QGraphicsItem*>list(items(e->pos()));
                for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
                {
                    QGraphicsItem* item(*it);
                    if(item)
                        if(item->type() == 666666)
                        {
                            setCursor(Qt::SizeAllCursor);
                            m_pWaitMapMoveTimer->stop();
                            m_sceneControlle->setSceneActive(true);
                            QGraphicsView::mousePressEvent(e);
                            mouseOnEditRect = true;
                            //return;
                        }
                }


                QList<QGraphicsItem*> tmpSelectedEditPoints;

                for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
                {
                    QGraphicsItem* item(*it);
                    if(item)
                    {
                        if(item->type() == 666666)
                        {
                            item->setSelected(true);
                            tmpSelectedEditPoints.append(item);
                        }
                    }
                }


                foreach (QGraphicsItem * it, m_selectedEditPoints)
                {
                    if(tmpSelectedEditPoints.contains(it))
                    {
                        m_selectedEditPoints.removeAll(it);
                    }
                }


                m_selectedEditPoints = tmpSelectedEditPoints;
                emit signalSetSelectedEditPoints(m_selectedEditPoints);

                if(!mouseOnEditRect && !m_waitMoveTimerCompleted )
                {
                    m_pressedMousePos = e->pos();
                    m_pWaitMapMoveTimer->start();

                }

                if(!mouseOnEditRect)
                {
                    if( m_pWaitMapMoveTimer->isActive())
                    {
                        m_pWaitMapMoveTimer->stop();
                        UserAction act;
                        act.posf = m_sceneControlle->sceneToGeo(scenePos);
                        act.type = e->type();
                        act.modifiers = e->modifiers();
                        act.buttons = e->button();
                        act.epsgCode = 0;
                        m_sceneControlle->emit_signalUserViewControlAction(act);
                    }
                }

                m_waitMoveTimerCompleted = false;


            }break;
            case SelectLayers_RectArea :
            {
                QPolygonF pol(m_pSelectAreaRect->polygon());
                QList<QGraphicsItem*>list(getScene()->items(pol, Qt::IntersectsItemBoundingRect, Qt::AscendingOrder));
                if(m_pSelectAreaRect->objectSelection() == SelectRectArea::RastersLayers)
                {
                    QList<uint> ids;
                    for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
                    {
                        if( (*it)->type() == visualize_system::RASTER_LAYER_ITEM )
                        {
                            visualize_system::RasterLayerItem* _rasterLayerItem = dynamic_cast<visualize_system::RasterLayerItem*>(*it);
                            if(_rasterLayerItem)
                                if(_rasterLayerItem->getRasterLayer()->intersect(pol))
                                {
                                    ids.append(_rasterLayerItem->getRasterLayer()->id());
                                }
                        }
                    }
                    visualize_system::SelectionModelInterface * selectionModelInterface = visualize_system::VisualizerManager::instance()->getSelectionModelInterface(m_sceneControlle->getVisualizerId());
                    if(selectionModelInterface)
                    {
                        uint currId = selectionModelInterface->getCurrentProvider();
                        selectionModelInterface->clearCurrentProvider();
                        selectionModelInterface->setSelectProviders(true, ids);
                        if(currId > 0)
                            selectionModelInterface->setCurrentProvider(currId);
                    }
                }
                else
                {
                    for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
                    {
//                        BaseSceneLayer* layer = dynamic_cast<BaseSceneLayer*>(*it);
//                        if(layer)
//                            if(layer->isSelectable(pol))
//                                objrepr::RepresentationServer::instance()->objectManager()->chooseObject(layer->getId()); //invertChoosingObject
                    }
                }

//                const SelectRectArea::RectType rectType(m_pSelectAreaRect->rectType());
//                QRect viewAreaRect(m_pSelectAreaRect->getSelectRect());
//                viewAreaRect = viewAreaRect.normalized();
//                QRectF sceneAreaRect(mapToScene(viewAreaRect.topLeft()), mapToScene(viewAreaRect.bottomRight()));
//                if(rectType == SelectRectArea::IsSelectRect)
//                {
//                    QSize viewSize(viewport()->size()), areaSize(viewAreaRect.size());
//                    if(areaSize.width() > 10 && areaSize.height() > 10)
//                    {
//                        int curZoom(m_zoom);
//                        while(areaSize.width() < viewSize.width() && areaSize.height() < viewSize.height())
//                        {
//                            areaSize = QSize(areaSize.width() * 2, areaSize.height() * 2);
//                            ++curZoom;
//                        }

//                        if(curZoom > m_zoom)
//                        {
//                            if(curZoom > m_zoomMax)
//                                curZoom = m_zoomMax;

//                            changeZoom(curZoom-1);
//                            centerOn(sceneAreaRect.center());
//                        }
//                    }
//                }
//                else
//                {
//                    QImage image(viewAreaRect.size(), QImage::Format_ARGB32);
//                    QPainter painter(&image);
//                    render(&painter, QRectF(), viewAreaRect);
//                    QClipboard* clipboard = QApplication::clipboard();
//                    clipboard->setImage(image);
//                    selectArea(m_pSelectAreaRect->polygon());
//                }

                slotDetectViewerMode("SelectLayersAndScrollOn", true);
                mouseReleaseEvent(e);
                break;
            }

            case StateObjectHandlerEvents:
            {
                if(m_stateObject.isNull() == false)
                {
                    if( m_stateObject->mouseReleaseEvent(e, scenePos) == false ) // если состояние хочет отдать событие вьюхе - отдаём !
                        return;
                }
            }break;

            }
        }
    }


    QGraphicsView::mouseReleaseEvent(e);
    m_waitMoveTimerCompleted = false;
}


void ViewerController::selectArea(QPolygonF pol)
{
    QList<uint> ids;
    QList<QGraphicsItem*>list(getScene()->items(pol, Qt::IntersectsItemBoundingRect, Qt::AscendingOrder));
    for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
    {
        if( (*it)->type() == visualize_system::RASTER_LAYER_ITEM )
        {
            visualize_system::RasterLayerItem* _rasterLayerItem = dynamic_cast<visualize_system::RasterLayerItem*>(*it);
            if(_rasterLayerItem)
                if(_rasterLayerItem->getRasterLayer()->intersect(pol))
                {
                    ids.append(_rasterLayerItem->getRasterLayer()->id());
                    //_rasterLayerItem->getRasterLayer()->emit_signalClicked(true);
                }
        }

//        BaseSceneLayer* layer = dynamic_cast<BaseSceneLayer*>(*it);
//        if(layer)
//            if(layer->isSelectable(pol))
//                objrepr::RepresentationServer::instance()->objectManager()->chooseObject(layer->getId()); //invertChoosingObject
    }

    visualize_system::SelectionModelInterface * selectionModelInterface = visualize_system::VisualizerManager::instance()->getSelectionModelInterface(m_sceneControlle->getVisualizerId());
    if(selectionModelInterface)
        selectionModelInterface->setSelectProviders(true, ids);
}


void ViewerController::selectEllipseSetRadArea(QPolygonF poly)
{
    foreach (quint64 id, m_pchoosedFromEllipseAreaWithSetRadIds)
    {
//        objrepr::RepresentationServer::instance()->objectManager()->withdrawObject(id);
    }

    m_pchoosedFromEllipseAreaWithSetRadIds.clear();
    QList<QGraphicsItem*>list(getScene()->items(poly.boundingRect(), Qt::IntersectsItemBoundingRect, Qt::AscendingOrder));
    for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
    {

        BaseSceneLayer* layer = dynamic_cast<BaseSceneLayer*>(*it);
        if(layer)
        {
            if(BaseSceneLayer::SLT_POINT_OBJECT == layer->getLayerType())
            {
                QPointF p = layer->boundingRect().center();
                if(poly.containsPoint(p,Qt::OddEvenFill))/*layer->isSelectable(poly))*/
                {
                    m_pchoosedFromEllipseAreaWithSetRadIds.append(layer->getMyId());
//                    objrepr::RepresentationServer::instance()->objectManager()->chooseObject(layer->getId()); //invertChoosingObject
                }
            }
        }
    }
}


void ViewerController::selectEllipseArea(QPolygonF poly)
{
    foreach (quint64 id, m_choosedFromEllipseSelectIds)
    {
//        objrepr::RepresentationServer::instance()->objectManager()->withdrawObject(id);
    }

    m_choosedFromEllipseSelectIds.clear();
    QList<QGraphicsItem*>list(getScene()->items(poly.boundingRect(), Qt::IntersectsItemBoundingRect, Qt::AscendingOrder));
    for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
    {

        BaseSceneLayer* layer = dynamic_cast<BaseSceneLayer*>(*it);
        if(layer)
        {
            if(BaseSceneLayer::SLT_POINT_OBJECT == layer->getLayerType())
            {
                QPointF p = layer->boundingRect().center();
                if(poly.containsPoint(p,Qt::OddEvenFill))/*layer->isSelectable(poly))*/
                {
                    m_choosedFromEllipseSelectIds.append(layer->getMyId());
//                    objrepr::RepresentationServer::instance()->objectManager()->chooseObject(layer->getId()); //invertChoosingObject
                }
            }
        }
    }
}

void ViewerController::mouseDoubleClickEvent(QMouseEvent *e)
{
    QPointF scenePos = mapToScene(e->pos().x(), e->pos().y());

    //new TestObj(scenePos, getScene()); // ---> !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    if(pEditedBaseSceneLayer)
    {
        pEditedBaseSceneLayer->setMouseEventInEditMode(e,scenePos);
        return;
    }

    switch(m_viewerState)
    {
    case SelectLayersAndScroll:
    {
//        objrepr::SpatialObjectManager *manager = objrepr::RepresentationServer::instance()->objectManager();
        QList<QGraphicsItem*>list(items(e->pos()));

        if(e->modifiers()&Qt::ShiftModifier) /// щечек по растру имеет приоритет над векторными объектами
        {
            if(e->modifiers()&Qt::ControlModifier)
            {
            }
            else
            {
                m_sceneControlle->emit_DubleClickOnScene(scenePos);
            }
        }
        else
        {
            QList<BaseSceneLayer*> baseSceneLayerList;
            foreach (QGraphicsItem* lr, list)
            {
                BaseSceneLayer* layer = dynamic_cast<BaseSceneLayer*>(lr);
                if(!layer)
                {
                    continue;
                }

                if(!layer->isClickable(e, scenePos, m_scale))
                {
                    continue;
                }

                baseSceneLayerList.append(layer);
            }

            QList<quint64> ids;

            if(baseSceneLayerList.isEmpty())
            {
                m_selectedFromMapid = 0;
                m_prevObjectsUnderMouse.clear();
//                manager->selectObject(0);
//                manager->withdrawObject(0);
            }

            //        if(e->button() == Qt::LeftButton)
            //        {
            //            changeZoom(m_zoom+2,false);
            //        }

            //        if(e->button() == Qt::RightButton)
            //        {
            //            changeZoom(m_zoom-2,false);
            //        }

        }
    }break;
    case MeasureRoute:
    {
        if(0 != m_pCalcRouteItem && m_calcRoutePoly.count() > 0)
        {
            m_calcRoutePoly.append(scenePos);

            m_calcRouteShowTempWay = false;
            m_calcRoutePoly.remove(m_calcRoutePoly.count() - 1);
            m_pCalcRouteItem->setPolygon(m_calcRoutePoly);
        }
    }break;


    case MeasureLineResolution:
    {
        if(0 != m_pCalcLineResolutionItem && m_calcLineResolutionPoly.count() > 0)
        {
            if(!m_pCalcLineResolutionItem->isSelectingFinished())
            {
                m_pCalcLineResolutionItem->setPolygon(m_calcLineResolutionPoly);
                m_pCalcLineResolutionItem->finishSelecting(true);
            }
            return;
        }
    }break;


    case MeasureSquare:
    {
        if(0 != m_pCalcSquareItem && m_calcSquarePoly.count() > 0)
        {
            if(!m_pCalcSquareItem->isSelectingFinished())
            {
                m_pCalcSquareItem->setPolygon(m_calcSquarePoly);
                m_pCalcSquareItem->finishSelecting(true);
            }
            return;
        }
    }break;

    case StateObjectHandlerEvents:
    {
        if(m_stateObject.isNull() == false)
        {
            bool setEventToView(false);
            if( m_stateObject->mouseDoubleClickEvent(e, scenePos) == true ) // если хотя бы одно состояние хочет отдать событие вьюхе - отдаём !
                setEventToView = true;

            if( m_stateObject->checkableRasterUnderMouse() ) // в этом режиме система работает с растрами как обычно
            {
                if(e->modifiers()&Qt::ShiftModifier)
                {
                    if(e->modifiers()&Qt::ControlModifier)
                    {
                    }
                    else
                    {
                        m_sceneControlle->emit_DubleClickOnScene(scenePos);
                    }
                }
            }

            if(setEventToView == false)
                return;
        }
    }break;

    }

    QGraphicsView::mouseDoubleClickEvent(e);
}

void ViewerController::slotWaitReleaseMouseTimeout()
{
    if(EditObjects == m_viewerState)
    {
        //setCursor(Qt::OpenHandCursor);
        setDragMode(QGraphicsView::ScrollHandDrag);
        setCursor(QCursor(Qt::ArrowCursor));

        if(!m_waitMoveTimerCompleted)
        {
            m_waitMoveTimerCompleted = true;
            QMouseEvent event(QEvent::MouseButtonPress, m_pressedMousePos, Qt::LeftButton, Qt::LeftButton, 0);
            mousePressEvent(&event);
            //m_waitMoveTimerCompleted = false;
        }
    }
}

void ViewerController::mousePressEvent(QMouseEvent* e)
{
    UserAction act;
    act.buttons = e->button();
    act.modifiers = e->modifiers();

    QPointF scenePos = mapToScene(e->pos().x(), e->pos().y());

    if(pEditedBaseSceneLayer)
    {
        pEditedBaseSceneLayer->setMouseEventInEditMode(e,scenePos);
        return;
    }

    if(e->button() == Qt::LeftButton)
    {
        switch(m_viewerState)
        {
        case SelectLayersAndScroll :
        {
                if(Qt::LeftButton == e->button())
                {
                    setDragMode(QGraphicsView::ScrollHandDrag);
                    setCursor(QCursor(Qt::ArrowCursor));
                }

                QList<QGraphicsItem*>list(items(e->pos()));

                ////////// щечек по растру имеет приоритет над векторными объектами ////////////
                visualize_system::RasterLayer* rasterLayer(0);

                if(e->modifiers()&Qt::ShiftModifier)
                    for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
                    {
                        if( (*it)->type() == visualize_system::RASTER_LAYER_ITEM )
                        {
                            visualize_system::RasterLayerItem* _rasterLayerItem = dynamic_cast<visualize_system::RasterLayerItem*>(*it);
                            if(_rasterLayerItem)
                                if(_rasterLayerItem->getRasterLayer()->containsPoint(scenePos))
                                {
                                    rasterLayer = _rasterLayerItem->getRasterLayer();
                                    break;
                                }
                        }
                    }

                BaseSceneLayer* clickedLayer(0);
                if(rasterLayer)
                {
                    rasterLayer->emit_signalClicked(e->modifiers()&Qt::ControlModifier);
                }
                else
                {
                    if(e->modifiers()&Qt::ShiftModifier)
                    {
                    }
                    else
                    {
                        foreach (QGraphicsItem* lr, list)
                        {
                            BaseSceneLayer* layer = dynamic_cast<BaseSceneLayer*>(lr);
                            if(layer)
                                if(layer->isClickable(e, scenePos, m_scale))
                                {
                                    clickedLayer = layer;
                                    break;
                                }
                        }
                        if(clickedLayer)
                        {
//                            objrepr::SpatialObjectManager * objectManager = objrepr::RepresentationServer::instance()->objectManager();
                            if(e->modifiers()&Qt::ControlModifier)
                            {
//                                std::vector<uint64_t> vec = objectManager->choosedObjects();
                                bool objectChoosed(false);
//                                for(int i(0); i<vec.size(); ++i)
//                                    if(vec.at(i) == clickedLayer->getId())
//                                    {
//                                        objectChoosed = true;
//                                        break;
//                                    }

                                if(objectChoosed)
                                {
                                    // объект уже чузирован, снимаем чуз, снимаем селекцию.
//                                    objectManager->withdrawObject(clickedLayer->getId());
//                                    objectManager->selectObject(0);
                                }
                                else
                                {
//                                    objectManager->chooseObject(clickedLayer->getId());
//                                    objectManager->selectObject(clickedLayer->getId());
                                }
                            }
                            else
                            {
//                                                        objectManager->withdrawObjects(objectManager->choosedObjects());
//                                std::vector<uint64_t> tmpVect = objectManager->choosedObjects();
//                                foreach (uint64_t tmpId, tmpVect) {
//                                    objectManager->withdrawObject(tmpId);
//                                }
//                                objectManager->selectObject(clickedLayer->getId());
//                                objectManager->chooseObject(clickedLayer->getId());
                            }
                        }
                    }
                }

            // если одновременно нажат и контрол и шифт - ничего не делаем.
            if( (e->modifiers()&Qt::ShiftModifier && e->modifiers()&Qt::ControlModifier) == false )
            {
                if(e->modifiers()&Qt::ShiftModifier)
                {
                    m_LeftButPressPos = scenePos;
                    QList<QVariant> list;
                    list << true << (int)(e->modifiers());
                    slotDetectViewerMode("SelectLayers_RectArea", list);
                }
                if(e->modifiers()&Qt::ControlModifier)
                {
                    m_LeftButPressPos = scenePos;
                    QList<QVariant> list;
                    list << true << (int)(e->modifiers());
                    slotDetectViewerMode("SelectLayers_RectArea", list);
                }
            }
            // клик по карте пока не снимает ничего
//            if(e->modifiers()&Qt::ShiftModifier || e->modifiers()&Qt::ControlModifier)
//            {
//            }
//            else if(clickedLayer == false) // если кликнули не на объекте - снимаем селекцию.
//                objrepr::RepresentationServer::instance()->objectManager()->withdrawObject(0);

            break;
        }

        case EditObjects:
        {
            if(0 == e->modifiers())
            {
                m_leftButtonPressedWithoutMods = true;
            }
            else
            {
                m_leftButtonPressedWithoutMods = false;
            }

            bool mouseOnEditRect = false;
            QList<QGraphicsItem*>list(items(e->pos()));
            for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
            {
                QGraphicsItem* item(*it);
                if(item)
                    if(item->type() == 666666)
                    {
                        setCursor(Qt::SizeAllCursor);
                        m_pWaitMapMoveTimer->stop();
                        m_sceneControlle->setSceneActive(true);
                        QGraphicsView::mousePressEvent(e);
                        mouseOnEditRect = true;
                        //return;
                    }
            }

            QList<QGraphicsItem*> tmpSelectedEditPoints;

            for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
            {
                QGraphicsItem* item(*it);
                if(item)
                {
                    if(item->type() == 666666)
                    {
                        item->setSelected(true);
                        tmpSelectedEditPoints.append(item);
                    }
                }
            }


            foreach (QGraphicsItem * it, m_selectedEditPoints)
            {
                if(tmpSelectedEditPoints.contains(it))
                {
                    m_selectedEditPoints.removeAll(it);
                }
            }


            m_selectedEditPoints = tmpSelectedEditPoints;
            emit signalSetSelectedEditPoints(m_selectedEditPoints);

            if(!mouseOnEditRect && !m_waitMoveTimerCompleted )
            {
                m_pressedMousePos = e->pos();
                m_pWaitMapMoveTimer->start();

            }

            //            if(!mouseOnEditRect)
            //            {
            //                if(!m_waitMoveTimerCompleted /*&& !m_pSendEventTimer->isActive()*/)
            //                {
            //                    UserAction act;
            //                    act.posf = scenePos;
            //                    act.type = e->type();
            //                    act.modifiers = e->modifiers();
            //                    act.buttons = e->button();
            //                    act.epsgCode = 0;
            //                    m_sceneControlle->emit_signalUserViewControlAction(act);
            //                }
            //            }


        }break;
        case MeasureAngles:
        {
            if(0 != m_pCalcAnglesItem && m_calcAnglePoly.count() <= 4)
            {
                if(4 == m_calcAnglePoly.count())
                {
                    m_calcAnglePoly.clear();
                    m_pCalcAnglesItem->setPolygon(m_calcAnglePoly);
                }

                if(m_calcAnglePoly.count() == 0)
                {
                    m_calcAnglePoly.append(scenePos);
                }


                if(m_calcAnglePoly.count() == 3)
                {
                    m_calcAnglePoly.remove(2);
                    m_calcAnglePoly.append(scenePos);
                }

                m_calcAnglePoly.append(scenePos);

                m_pCalcAnglesItem->setPolygon(m_calcAnglePoly);
                return;
            }
        }break;

        case MeasureRoute:
        {
            if(0 != m_pCalcRouteItem )
            {


                if(m_calcRoutePoly.count() == 0)
                {
                    m_calcRoutePoly.append(scenePos);
                    m_calcRoutePoly.append(scenePos);
                }
                else
                {
                    if(m_calcRouteShowTempWay)
                    {
                        m_calcRoutePoly.remove(m_calcRoutePoly.count() - 1);
                        m_calcRoutePoly.append(scenePos);
                        m_calcRoutePoly.append(scenePos);
                    }
                    else
                    {
                        m_calcRoutePoly.clear();
                    }

                }

                m_calcRouteShowTempWay = true;
                m_pCalcRouteItem->setPolygon(m_calcRoutePoly);
                return;
            }
        }break;

        case MeasureLength:
        {
            if(0 != m_pCalcLengthItem )
            {
                if(!m_calcLengthShowTempWay)
                {
                    m_calcLengthPoly.clear();
                    m_calcLengthShowTempWay = true;
                    m_pCalcLengthItem->setPolygon(m_calcLengthPoly);
                }

                QClipboard* clipboard = QApplication::clipboard();
                if(m_calcLengthPoly.count() == 0)
                {
                    m_calcLengthPoly.append(scenePos);
                    m_calcLengthPoly.append(scenePos);
                    m_calcLengthShowTempWay = true;
                    m_pCalcLengthItem->setPolygon(m_calcLengthPoly);
                    clipboard->setText(QString());
                }
                else
                {
                    m_calcLengthShowTempWay = false;
                    clipboard->setText(QString::number(m_pCalcLengthItem->getLengthInMeters(), 'f', 0));
                }

                return;
            }
        }break;

        case MeasureReach:
        {
            if(0 != m_pReachGraphicsItem )
            {
                if(m_pReachGraphicsItem->isSelectingFinished())
                {
                    m_pReachGraphicsItem->finishSelecting(false);
                    m_pReachGraphicsItem->setCenter(scenePos);
                    m_pReachGraphicsItem->setRadiusPoint(scenePos);

                }

                if(0 == m_pReachGraphicsItem->getRadius())
                {
                    m_pReachGraphicsItem->finishSelecting(false);
                    m_pReachGraphicsItem->setCenter(scenePos);
                    m_pReachGraphicsItem->setRadiusPoint(scenePos);
                }
                else
                {
                    m_pReachGraphicsItem->finishSelecting(true);
                }

            }
        }break;

        case MeasureLineResolution:
        {
            if(0 != m_pCalcLineResolutionItem)
            {
                if(!m_pCalcLineResolutionItem->isSelectingFinished())
                {
                    if(m_calcLineResolutionPoly.count() == 0)
                    {
                        m_calcLineResolutionPoly.append(scenePos);
                        m_calcLineResolutionPoly.append(scenePos);
                    }
                    else
                    {
                        m_calcLineResolutionPoly.remove(m_calcLineResolutionPoly.count() - 1);
                        m_calcLineResolutionPoly.append(scenePos);
                        m_calcLineResolutionPoly.append(scenePos);
                    }

                    m_pCalcLineResolutionItem->setPolygon(m_calcLineResolutionPoly);
                }
                else
                {
                    m_calcLineResolutionPoly.clear();
                    m_pCalcLineResolutionItem->finishSelecting(false);
                    m_pCalcLineResolutionItem->setPolygon(m_calcLineResolutionPoly);
                }
                return;
            }
        }

        case MeasureSquare:
        {
            if(0 != m_pCalcSquareItem)
            {
                if(!m_pCalcSquareItem->isSelectingFinished())
                {
                    if(m_calcSquarePoly.count() == 0)
                    {
                        m_calcSquarePoly.append(scenePos);
                        m_calcSquarePoly.append(scenePos);
                    }
                    else
                    {
                        m_calcSquarePoly.remove(m_calcSquarePoly.count() - 1);
                        m_calcSquarePoly.append(scenePos);
                        m_calcSquarePoly.append(scenePos);
                    }

                    m_pCalcSquareItem->setPolygon(m_calcSquarePoly);
                }
                else
                {
                    m_calcSquarePoly.clear();
                    m_pCalcSquareItem->finishSelecting(false);
                    m_pCalcSquareItem->setPolygon(m_calcSquarePoly);
                }
                return;
            }
        }break;

        case ScrollOnly:
        {
//            QList<QGraphicsItem*>list(items(e->pos()));
//            for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
//            {
//                QGraphicsItem* item(*it);
//                if(item)
//                    if(item->type() == 777777)
//                    {
//                        setCursor(Qt::SizeAllCursor);
//                        m_sceneControlle->setSceneActive(true);
//                        QGraphicsView::mousePressEvent(e);
//                        return;
//                    }
//            }
//            m_sceneControlle->getScene()->clearSelection();
        }break;
        }
    }

    if(e->button() == Qt::RightButton)
        switch(m_viewerState)
        {
        case SelectLayersAndScroll:
        {
            ////////// щечек по растру имеет приоритет над векторными объектами ////////////
            QList<QGraphicsItem*>list(items(e->pos()));
            visualize_system::RasterLayer* rasterLayer(0);
            if(e->modifiers()&Qt::ShiftModifier)
                for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
                {
                    if( (*it)->type() == visualize_system::RASTER_LAYER_ITEM )
                    {
                        visualize_system::RasterLayerItem* _rasterLayerItem = dynamic_cast<visualize_system::RasterLayerItem*>(*it);
                        if(_rasterLayerItem)
                            if(_rasterLayerItem->getRasterLayer()->containsPoint(scenePos))
                            {
                                rasterLayer = _rasterLayerItem->getRasterLayer();
                                break;
                            }
                    }
                }

            if(rasterLayer)
            {
                QPointF geoPos = m_sceneControlle->sceneToGeo(scenePos);
                m_sceneControlle->emit_ClickRBOnRaster(rasterLayer->id(), geoPos, e->modifiers()&Qt::ControlModifier);
            }
//            else if((e->modifiers() & Qt::ShiftModifier))
//            {
//                switchViewerStateMode(SelectLayers_EllipseAreaWithSettedRadius, true);
//                if(0 != m_pSelectEllipseAreaWithSetRadItem )
//                {
//                    //if(0 == m_pSelectEllipseAreaWithSetRadItem->getRadius())
//                    m_pSelectEllipseAreaWithSetRadItem->finishSelecting(false);
//                    m_pSelectEllipseAreaWithSetRadItem->setCenter(scenePos);
//                    m_pSelectEllipseAreaWithSetRadItem->setRadiusPoint(scenePos);


//                    qreal iPxDist = getIntepixelDistance();

//                    qreal px = m_ellipseSelectRadWithRad*1000/iPxDist;
//                    QPointF radPoint = mapToScene(e->pos().x() + px, e->pos().y());

//                    m_pSelectEllipseAreaWithSetRadItem->setRadiusPoint(radPoint);
//                    QPolygonF poly = m_pSelectEllipseAreaWithSetRadItem->ellipsePath().toFillPolygon();

//                    selectEllipseSetRadArea(poly);
//                    m_pSelectEllipseAreaWithSetRadItem->finishSelecting(true);
//                }
//            }
//            else if((e->modifiers() & Qt::ControlModifier))
//            {
//                switchViewerStateMode(SelectLayers_EllipseArea, true);
//                if(0 != m_pSelectEllipseAreaItem )
//                {
//                    if(0 == m_pSelectEllipseAreaItem->getRadius())
//                    {
//                        m_pSelectEllipseAreaItem->finishSelecting(false);
//                        m_pSelectEllipseAreaItem->setCenter(scenePos);
//                        m_pSelectEllipseAreaItem->setRadiusPoint(scenePos);
//                    }
//                    else
//                    {
//                        m_pSelectEllipseAreaItem->finishSelecting(true);
//                    }
//                }
//            }
            else
            {
//                foreach (QGraphicsItem* lr, list)
//                {
//                    BaseSceneLayer* layer = dynamic_cast<BaseSceneLayer*>(lr);
//                    if(layer)
//                        if(layer->isClickable(e, scenePos, m_scale))
//                        {
////                            objrepr::SpatialObjectManager * objectManager = objrepr::RepresentationServer::instance()->objectManager();
//                            std::vector<uint64_t> vec = objectManager->choosedObjects();
//                            bool objectChoosed(false);
//                            for(int i(0); i<vec.size(); ++i)
//                                if(vec.at(i) == layer->getId())
//                                {
//                                    // объект уже чузирован, селектируем его, другие объекты не трогаем.
//                                    objectChoosed = true;
//                                    break;
//                                }
//                            if(objectChoosed == false)
//                            {
////                                objectManager->withdrawObjects(objectManager->choosedObjects());
//                                std::vector<uint64_t> tmpVect = objectManager->choosedObjects();
//                                foreach (uint64_t tmpId, tmpVect) {
//                                    objectManager->withdrawObject(tmpId);
//                                }
//                            }
//                            objectManager->selectObject(layer->getId());
//                            m_sceneControlle->emit_ClickRBOnObject(layer->getId());
//                            break;
//                        }
//                }
            }
        }break;
        }

    // на пиксельной сцене при нажатиии средней кнопки мыши устанавливается масштаб при котором изображение будет в оригинальном размере.
    if(m_sceneControlle->sceneIsGeo() == false && e->button() & Qt::MiddleButton)
        if(m_sceneControlle->getBaseZlevel() != -1)
            changeZoom(m_sceneControlle->getBaseZlevel());

    if(m_viewerState == StateObjectHandlerEvents)
    {
        if(m_stateObject.isNull() == false)
        {
            bool setEventToView(false);
            if( m_stateObject->mousePressEvent(e, scenePos) == true ) // если состояние хочет отдать событие вьюхе - отдаём !
                setEventToView = true;

            if(e->button() == Qt::LeftButton)
            {
                if( m_stateObject->checkableRasterUnderMouse() ) // в этом режиме система работает с растрами как обычно
                {
                    visualize_system::RasterLayer* rasterLayer(0);

                    if(e->modifiers()&Qt::ShiftModifier)
                    {
                        QList<QGraphicsItem*>list(items(e->pos()));
                        for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
                        {
                            if( (*it)->type() == visualize_system::RASTER_LAYER_ITEM )
                            {
                                visualize_system::RasterLayerItem* _rasterLayerItem = dynamic_cast<visualize_system::RasterLayerItem*>(*it);
                                if(_rasterLayerItem)
                                    if(_rasterLayerItem->getRasterLayer()->containsPoint(scenePos))
                                    {
                                        rasterLayer = _rasterLayerItem->getRasterLayer();
                                        break;
                                    }
                            }
                        }
                    }

                    if(rasterLayer)
                    {
                        rasterLayer->emit_signalClicked(false); //e->modifiers()&Qt::ControlModifier);
                    }
                }
            }
            else if(e->button() == Qt::RightButton)
            {
                bool contextObjectMenuShowed(false);
                if( m_stateObject->showContextObjectMenu() )
                {
                    QList<QGraphicsItem*>list(items(e->pos()));
                    foreach (QGraphicsItem* lr, list)
                    {
                        BaseSceneLayer* layer = dynamic_cast<BaseSceneLayer*>(lr);
                        if(layer)
                            if(layer->isClickable(e, scenePos, m_scale))
                            {
//                                objrepr::RepresentationServer::instance()->objectManager()->chooseObject(layer->getId());
                                m_sceneControlle->emit_ClickRBOnObject(layer->getId());
                                contextObjectMenuShowed = true;
                                break;
                            }
                    }
                }

                if( m_stateObject->showContextRasterMenu() && contextObjectMenuShowed == false)
                {
                    visualize_system::RasterLayer* rasterLayer(0);
                    if(e->modifiers()&Qt::ShiftModifier)
                    {
                        QList<QGraphicsItem*>list(items(e->pos()));
                        for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
                        {
                            visualize_system::RasterLayer* _rasterLayer = dynamic_cast<visualize_system::RasterLayer*>(*it);
                            if(_rasterLayer)
                                if(_rasterLayer->containsPoint(scenePos))
                                {
                                    rasterLayer = _rasterLayer;
                                    break;
                                }
                        }
                    }

                    if(rasterLayer)
                    {
                        QPointF geoPos = m_sceneControlle->sceneToGeo(scenePos);
                        m_sceneControlle->emit_ClickRBOnRaster(rasterLayer->id(), geoPos, e->modifiers()&Qt::ControlModifier);
                    }
                }
            }

            if(setEventToView == false)
                return;
        }
    }

    QGraphicsView::mousePressEvent(e);
    m_sceneControlle->mousePressEventInView(e,scenePos);
}



void ViewerController::slotDetectViewerMode(QString cmd, QVariant var)
{
    ViewerState mode;
    bool onOff;
    QVariant params ;

    if(QString("CalcAngles").toLower() == cmd.toLower())
    {
        bool onOff = var.toBool();
        switchViewerStateMode(MeasureAngles, onOff);
    }

    if(QString("CalcRoute").toLower() == cmd.toLower())
    {
        bool onOff = var.toBool();
        switchViewerStateMode(MeasureRoute, onOff);
    }

    if(QString("CalcLength").toLower() == cmd.toLower())
    {
        bool onOff = var.toBool();
        switchViewerStateMode(MeasureLength, onOff);
    }

    if(QString("CalcSquare").toLower() == cmd.toLower())
    {
        bool onOff = var.toBool();
        switchViewerStateMode(MeasureSquare, onOff);
    }

    if(QString("CalcLineResolution").toLower() == cmd.toLower())
    {
        bool onOff = var.toBool();
        switchViewerStateMode(MeasureLineResolution, onOff);
    }

    if(QString("CalcReach").toLower() == cmd.toLower())
    {
        bool onOff = var.toBool();
        switchViewerStateMode(MeasureReach, onOff);
    }

    if(QString("SelectLayersAndScrollOn").toLower() == cmd.toLower())
    {
        switchViewerStateMode(SelectLayersAndScroll, true);
    }

    if(QString("SelectLayersAndScrollOn").toLower() == cmd.toLower())
    {
        switchViewerStateMode(SelectLayersAndScroll, true);
    }

    if(QString("SelectLayers_RectArea").toLower() == cmd.toLower())
    {
        bool onOff;
        QVariant varData;
        if(var.type() == QVariant::Bool)
            onOff = var.toBool();
        else if(var.type() == QVariant::List)
        {
            QList<QVariant> list(var.toList());
            if(list.count() == 2)
            {
                onOff = list[0].toBool();
                varData = list[1];
            }
            else
                onOff = false;
        }
        switchViewerStateMode(SelectLayers_RectArea, onOff, varData);
    }

    if(QString("SelectScrollOff").toLower() == cmd.toLower())
    {
        switchViewerStateMode(SelectScroll, false);
    }

    if(QString("SwitchToDefaultMode").toLower() == cmd.toLower())
    {
        switchViewerStateMode(m_defaultMode, true);
    }

    if(QString("SetEditMode").toLower() == cmd.toLower())
    {
        QVariantMap vm = var.toMap();
        QVariant varPm  = vm.value("objectIcon");
        bool b = vm.value("onOff").toBool();
        switchViewerStateMode(EditObjects, b, varPm);
    }

    if(QString("slotBlockScrollAndSelect").toLower() == cmd.toLower())
    {
        bool isBlocked = var.toBool();
        m_blockScrollAndSelect = isBlocked;
        switchViewerStateMode(CenterOnObject, isBlocked);
    }


    if(QString("slotBindLayer").toLower() == cmd.toLower())
    {
        QVariantMap varMap = var.toMap();
        bool isBinded = varMap.value("IsBinded").toBool();
        quint64 layerId = varMap.value("LayerId").toULongLong();

        if(0 == layerId)
        {
            // switchViewerStateMode(CenterOnObject, false, var);
            switchViewerStateMode(DenyScrollOverAndSelect, false, var);
        }
        else
        {
            if(isBinded)
            {
                switchViewerStateMode(DenyScrollOverAndSelect, true, var);
            }
            else
            {
                switchViewerStateMode(CenterOnObject, true, var);
            }
        }
    }

    if(QString("stateObjectHandlerEventsOn").toLower() == cmd.toLower())
    {
        switchViewerStateMode(StateObjectHandlerEvents, true, var);
    }

    if(QString("stateObjectHandlerEventsOff").toLower() == cmd.toLower())
    {
        switchViewerStateMode(StateObjectHandlerEvents, false);
    }

}



void ViewerController::switchViewerStateMode(ViewerState mode, bool onOff, QVariant params )
{

    if(mode != StateObjectHandlerEvents)
        if( ((mode == m_viewerState && onOff) || (mode != m_viewerState && !onOff)) )
        {
            // qDebug() << "ViewerController::switchViewerStateMode(): WARN! Try to double turn on mode or turn off non existing mode!";
            return;
        }

    ViewerState switchTo = mode;
    bool switchToOnOff = onOff;

    for(int step = 1; step <= 2; step++)
    {
        if(1 == step)
        {
            /*сначала выключаем режим*/
            switchTo = m_viewerState;
            switchToOnOff = onOff;
            onOff = false;
        }

        if(2 == step)
        {
            onOff = switchToOnOff;

            if(!onOff)
            {
                if(m_stateObjectStack.isEmpty())
                {
                    m_viewerState = m_defaultMode;
                    setDragMode(QGraphicsView::ScrollHandDrag);
                    setCursor(Qt::ArrowCursor);
                }
                else
                {
                    setDragMode(QGraphicsView::NoDrag);
                    m_viewerState = StateObjectHandlerEvents;
                    m_stateObject = m_stateObjectStack.pop();
                    m_stateObject->statePoppedFromStack();
                }

                return;
            }
            else
            {
                switchTo = mode;
                m_viewerState = mode;
            }
        }

        switch (switchTo)
        {

//        case ScrollOnly:
//        {
//            int a = 5;
//        }break;

        case MeasureAngles:
        {
            if(onOff)
            {
                setDragMode(QGraphicsView::NoDrag);
                QPixmap pixmap(":/307_angle_meaure_cur.png");
                setCursor(QCursor(pixmap, 0, 0));

                m_pCalcAnglesItem = new AngleGraphicsItem();
                connect(m_pCalcAnglesItem, SIGNAL(signalCalcRealAngle(QLineF,QLineF,qreal*))
                        , this, SLOT(slotGetRealAngle(QLineF,  QLineF, qreal*)));

                QGraphicsScene * sc = scene();
                sc->addItem(m_pCalcAnglesItem);
                m_calcAnglePoly.clear();
                m_pCalcAnglesItem->show();
                setItemsScale();
            }
            else
            {
                m_calcAnglePoly.clear();
                delete m_pCalcAnglesItem;
                m_pCalcAnglesItem = 0;
                emit signalSwitchOffButton((int)sw::BTN_CALC_ANGLES);
                setDragMode(QGraphicsView::ScrollHandDrag);
                setCursor(QCursor(Qt::ArrowCursor));
            }
        }break;

        case MeasureRoute:
        {
            if(onOff)
            {

                setDragMode(QGraphicsView::NoDrag);
                QPixmap pixmap(":/309_routes_building_cur.png");
                setCursor(QCursor(pixmap, 0, 0));


                m_pCalcRouteItem = new RouteScaleGraphicsItem();

                QPen pn;
                QBrush br;

                QColor colPen;
                colPen.setRgba(qRgba(0x09,0x61,0xD3,150));
                pn.setColor(colPen);
                pn.setWidth(2);
                pn.setCosmetic(true);
                pn.setJoinStyle(Qt::RoundJoin);

                QColor colBr;
                colBr.setRgba(qRgba(0xF3,0xF0,0x0F,150));
                br.setColor(colBr);
                br.setStyle(Qt::SolidPattern);

                m_pCalcRouteItem->setPen(pn);
                m_pCalcRouteItem->setBrush(br);


                connect(m_pCalcRouteItem, SIGNAL(signalCalcRealDistance(QLineF,qreal*)),
                        this, SLOT(slotGetRealDistanceMeters(QLineF,qreal*)));
                QGraphicsScene * sc = scene();
                sc->addItem(m_pCalcRouteItem);
                m_calcRoutePoly.clear();
                m_pCalcRouteItem->show();
                setItemsScale();
            }
            else
            {
                m_calcRouteShowTempWay = false;
                m_calcRoutePoly.clear();
                delete m_pCalcRouteItem;
                m_pCalcRouteItem = 0;

                emit signalSwitchOffButton((int)sw::BTN_CALC_ROUTE);

                setDragMode(QGraphicsView::ScrollHandDrag);
                setCursor(QCursor(Qt::ArrowCursor));
            }
        }break;

        case MeasureLength:
        {
            if(onOff)
            {
                setDragMode(QGraphicsView::NoDrag);
                QPixmap pixmap(":/308_distance_meaure_cur.png");
                setCursor(QCursor(pixmap, 0, 0));

                m_pCalcLengthItem = new RouteScaleGraphicsItem();

                QPen pn;
                QBrush br;

                QColor colPen;
                colPen.setRgba(qRgba(0x2A,0x9F,0x06,150));
                pn.setColor(colPen);
                pn.setWidth(2);
                pn.setCosmetic(true);
                pn.setJoinStyle(Qt::RoundJoin);

                QColor colBr;
                colBr.setRgba(qRgba(0x2A,0x9F,0x06,150));
                br.setColor(colBr);
                br.setStyle(Qt::SolidPattern);

                m_pCalcLengthItem->setPen(pn);
                m_pCalcLengthItem->setBrush(br);

                connect(m_pCalcLengthItem, SIGNAL(signalCalcRealDistance(QLineF,qreal*)),
                        this, SLOT(slotGetRealDistanceMeters(QLineF,qreal*)));
                QGraphicsScene * sc = scene();
                sc->addItem(m_pCalcLengthItem);
                m_calcLengthPoly.clear();
                m_pCalcLengthItem->show();
                setItemsScale();
            }
            else
            {
                m_calcLengthShowTempWay = false;
                m_calcLengthPoly.clear();
                delete m_pCalcLengthItem;
                m_pCalcLengthItem = 0;

                emit signalSwitchOffButton((int)sw::BTN_CALC_LENGHT);

                setDragMode(QGraphicsView::ScrollHandDrag);
                setCursor(QCursor(Qt::ArrowCursor));
            }
        }break;

        case MeasureSquare:
        {
            if(onOff)
            {

                setDragMode(QGraphicsView::NoDrag);
                QPixmap pixmap(":/310_squares_measure_cur.png");
                setCursor(QCursor(pixmap, 0, 0));

                m_pCalcSquareItem = new SquareGraphicsItem();

                QPen pn;
                QBrush br;
                QColor colPen;
                colPen.setRgba(qRgba(0x09,0x61,0xD3,150));
                pn.setColor(colPen);
                pn.setWidth(2);
                pn.setCosmetic(true);
                pn.setJoinStyle(Qt::RoundJoin);


                QColor colBr;
                colBr.setRgba(qRgba(0xF3,0xF0,0x0F,150));
                br.setColor(colBr);
                br.setStyle(Qt::SolidPattern);
                m_pCalcSquareItem->setPenBrush(pn, br);

                connect(m_pCalcSquareItem, SIGNAL(signalCalcWGS84Square(QPolygonF,qreal*)),
                        this, SLOT(slotGetRealSquareMeters(QPolygonF,qreal*)));
                m_pCalcSquareItem->setZValue(100);
                QGraphicsScene * sc = scene();
                sc->addItem(m_pCalcSquareItem);
                setItemsScale();
            }
            else
            {
                m_calcSquarePoly.clear();
                delete m_pCalcSquareItem;
                m_pCalcSquareItem = 0;

                emit signalSwitchOffButton((int)sw::BTN_CALC_SQUARE);

                setDragMode(QGraphicsView::ScrollHandDrag);
                setCursor(QCursor(Qt::ArrowCursor));
            }
        }break;

        case MeasureLineResolution:
        {
            if(onOff)
            {
                setDragMode(QGraphicsView::NoDrag);
                QPixmap pixmap(":/314_linear_resolution_measure_cur.png");
                setCursor(QCursor(pixmap, 0, 0));

                m_pCalcLineResolutionItem = new LineResolutionItem();
                m_pCalcLineResolutionItem->setTextMode(true);

                QPen pn;
                QBrush br;
                QColor colPen;
                colPen.setRgba(qRgba(0x09,0x61,0xD3,150));
                pn.setColor(colPen);
                pn.setWidth(2);
                pn.setCosmetic(true);
                pn.setJoinStyle(Qt::RoundJoin);


                QColor colBr;
                colBr.setRgba(qRgba(0xF3,0xF0,0x0F,150));
                br.setColor(colBr);
                br.setStyle(Qt::SolidPattern);
                m_pCalcLineResolutionItem->setPenBrush(pn, br);

                connect(m_pCalcLineResolutionItem, SIGNAL(signalCalcWGS84Square(QPolygonF,qreal*)),
                        this, SLOT(slotGetLineResolution(QPolygonF,qreal*)));
                m_pCalcLineResolutionItem->setZValue(100);
                QGraphicsScene * sc = scene();
                sc->addItem(m_pCalcLineResolutionItem);
                setItemsScale();
            }
            else
            {
                m_calcLineResolutionPoly.clear();
                delete m_pCalcLineResolutionItem;
                m_pCalcLineResolutionItem = 0;

                emit signalSwitchOffButton((int)sw::BTN_DETECT_LINE_RESOLUTION);

                setDragMode(QGraphicsView::ScrollHandDrag);
                setCursor(QCursor(Qt::ArrowCursor));
            }

        }break;

        case MeasureReach:
        {
            if(onOff)
            {

                setDragMode(QGraphicsView::NoDrag);
                QPixmap pixmap(":/312_accessability_measure_cur.png");
                setCursor(QCursor(pixmap, 0, 0));

                m_reachRadius = 0;
                m_pReachGraphicsItem = new ReachGraphicsItem(m_sceneControlle);
                m_pReachGraphicsItem->clear();

                QPen pn;
                QBrush br;
                QColor colPen;
                colPen.setRgba(qRgba(0x09,0x61,0xD3,150));
                pn.setColor(colPen);
                pn.setWidth(2);
                pn.setCosmetic(true);
                pn.setJoinStyle(Qt::RoundJoin);


                QColor colBr;
                colBr.setRgba(qRgba(0xF3,0xF0,0x0F,150));
                br.setColor(colBr);
                br.setStyle(Qt::SolidPattern);
                m_pReachGraphicsItem->setPenBrush(pn, br);

                connect(m_pReachGraphicsItem, SIGNAL(signalCalcWGS84Radius(QLineF,qreal*)),
                        this, SLOT(slotGetRealDistanceMeters(QLineF,qreal*)));
                //m_pReachGraphicsItem->setZValue(100);
                QGraphicsScene * sc = scene();
                sc->addItem(m_pReachGraphicsItem);
                setItemsScale();
            }
            else
            {
                //                foreach (quint64 id, m_choosedFromReachIds)
                //                {
                //                    objrepr::RepresentationServer::instance()->objectManager()->withdrawObject(id);
                //                }

                m_reachRadius = 0;
                delete m_pReachGraphicsItem;
                m_pReachGraphicsItem = 0;

                emit signalSwitchOffButton((int)sw::BTN_CALC_REACH);

                setDragMode(QGraphicsView::ScrollHandDrag);
                setCursor(QCursor(Qt::ArrowCursor));
            }
        }break;

        case CenterOnObject:
        {
            quint64 layerId = params.toMap().value("LayerId").toULongLong();

            if(onOff)
            {
                blockRotate(true);
                m_pViewRectPartLayer = new BaseSceneLayer(getViewportRectPart(50,50), 0);
                m_pViewRectPartLayer->setZValue(150);
                QColor col;
                col.setRgba(qRgba(0x7D,0x8A,0x85,100));
                QPen pen(col);
                pen.setWidth(3);
                pen.setCosmetic(true);
                pen.setJoinStyle(Qt::RoundJoin);
                m_pViewRectPartLayer->setPen(pen);
                m_pViewRectPartLayer->setSceneLayerType(BaseSceneLayer::SLT_VIEWRECT);
                QGraphicsScene * sc = scene();
                sc->addItem(m_pViewRectPartLayer);


                QList<QGraphicsItem*>list(getScene()->items());
                for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
                {
                    BaseSceneLayer * layer = dynamic_cast<BaseSceneLayer*>(*it);
                    if(0 != layer)
                        if(layer->getMyId() == layerId)
                        {
                            m_pBindedLayer = layer;
                            m_bindingLayerPrevCenterPos = m_pBindedLayer->boundingRect().center();
                            break;
                        }
                }
            }
            else
            {
                blockRotate(false);
                m_pBindedLayer = 0;
                m_lastGoodViewportPosForBind = QPointF(0,0);
            }

        }break;
        case DenyScrollOverAndSelect:
        {
            quint64 layerId = params.toMap().value("LayerId").toULongLong();

            if(onOff)
            {
                blockRotate(true);
                m_pViewRectPartLayer = new BaseSceneLayer(getViewportRectPart(50,50), 0);
                m_pViewRectPartLayer->setZValue(100);
                QColor col;
                col.setRgba(qRgba(0x7D,0x8A,0x85,100));
                QPen pen(col);
                pen.setWidth(3);
                pen.setCosmetic(true);
                pen.setJoinStyle(Qt::RoundJoin);
                m_pViewRectPartLayer->setPen(pen);
                m_pViewRectPartLayer->setSceneLayerType(BaseSceneLayer::SLT_VIEWRECT);
                QGraphicsScene * sc = scene();
                sc->addItem(m_pViewRectPartLayer);


                QList<QGraphicsItem*>list(getScene()->items());
                for(QList<QGraphicsItem*>::iterator it(list.begin()); it != list.end(); ++it)
                {
                    BaseSceneLayer * layer = dynamic_cast<BaseSceneLayer*>(*it);
                    if(0 != layer)
                        if(layer->getMyId() == layerId)
                        {
                            m_pBindedLayer = layer;
                            m_bindingLayerPrevCenterPos = m_pBindedLayer->boundingRect().center();
                            break;
                        }
                }
            }
            else
            {
                delete m_pViewRectPartLayer;
                m_pViewRectPartLayer = 0;

            }
        }break;
        case SelectLayersAndScroll:
        {
            /*это дефолтный режим, его отключить нельзя*/
            if(onOff)
            {
                setDragMode(QGraphicsView::ScrollHandDrag);
                setCursor(Qt::ArrowCursor);
                /*тут никаких телодвижений делать не надо*/
            }

        }break;
        case SelectScroll:
        {
        }break;
        case SelectLayers_RectArea:
        {
            if(onOff)
            {
                int modifier( params.toInt() );
                m_pSelectAreaRect = new SelectRectArea(modifier&Qt::ControlModifier ? SelectRectArea::ObjreprLayers : SelectRectArea::RastersLayers);
                m_pSelectAreaRect->setZValue(150);
                QGraphicsScene * sc = scene();
                sc->addItem(m_pSelectAreaRect);
                m_pSelectAreaRect->show();

                setDragMode(QGraphicsView::NoDrag);
            }
            else
            {
                m_pSelectAreaRect->hide();
                delete m_pSelectAreaRect;
                m_pSelectAreaRect = 0;
                setDragMode(QGraphicsView::ScrollHandDrag);
                setCursor(QCursor(Qt::ArrowCursor));
            }
        }break;
        case EditObjects:
        {
            if(onOff)
            {
                setDragMode(QGraphicsView::NoDrag);

            }
            else
            {
                setDragMode(QGraphicsView::ScrollHandDrag);
                setCursor(QCursor(Qt::ArrowCursor));
                m_sceneControlle->setSceneActive(false);
                m_leftButtonPressedWithoutMods = false;
                m_pWaitMapMoveTimer->stop();
            }
        }break;
        case StateObjectHandlerEvents:
        {
            QSharedPointer<StateObject> stateObject;
            bool addState = false;
            if(params.type() == QVariant::List)
            {
                QList<QVariant> list = params.toList();
                if(list.size() == 2)
                {
                    QVariant varStateObject = list.first();
                    StateObjectInQVariant stateObjectInQVariant = varStateObject.value<StateObjectInQVariant>();
                    stateObject = stateObjectInQVariant.stateObject;

                    addState = list.last().toBool();
                }
            }
            qDebug() << "StateObjectHandlerEvents, onOff" << onOff << "addState :" << addState;

            if(onOff)
            {
                setDragMode(QGraphicsView::NoDrag);

                // проверка, нет ли этого состояния в стеке
                bool stateFound(false);
                for(QStack< QSharedPointer<StateObject> >::iterator stackIt(m_stateObjectStack.begin()); stackIt != m_stateObjectStack.end(); ++stackIt)
                {
                    QSharedPointer<StateObject> ptr = *stackIt;
                    if(ptr.data() == stateObject.data())
                    {
                        stateFound = true;
                        ptr->statePoppedFromStack();
                        m_stateObjectStack.erase(stackIt);
                        break;
                    }
                }

                if(stateFound == false) // состояния нет в стеке, необходимо его проиннициализировать и приконнектить
                {
                    qDebug() << "--- slotStateObjectHandlerEvents ON, state not found in stack, stack size :" << m_stateObjectStack.size();

                    connect(stateObject.data(), SIGNAL(signalCloseState()), this, SLOT(slotStateObjectHandlerEventsOff()), Qt::QueuedConnection);
                    connect(stateObject.data(), SIGNAL(signalConvertNativeToScene(QPointF&)), m_sceneControlle, SLOT(frameToScene(QPointF&)));
                    connect(stateObject.data(), SIGNAL(signalConvertSceneToNative(QPointF&)), m_sceneControlle, SLOT(sceneToFrame(QPointF&)));
                    connect(stateObject.data(), SIGNAL(signalConvertEpsgToScene(int,QPointF&,bool&)), m_sceneControlle, SLOT(convertEpsgToScene(int,QPointF&,bool&)));
                    connect(stateObject.data(), SIGNAL(signalConvertSceneToEpsg(int,QPointF&,bool&)), m_sceneControlle, SLOT(convertSceneToEpsg(int,QPointF&,bool&)));
                    connect(stateObject.data(), SIGNAL(signalSetZlevel(int)), this, SLOT(slotChangeZoom(int)));
                    connect(stateObject.data(), SIGNAL(signalSetActiveForScene(bool)), m_sceneControlle, SLOT(slotSetActiveForScene(bool)));
                    connect(stateObject.data(), SIGNAL(signalSetPrefferZoomForSceneRect(QRectF)), this, SLOT(setPrefferZoomForSceneRect(QRectF)));


                    //                    MeasureStateObject* measureStateObject = dynamic_cast<MeasureStateObject*>(stateObject);
                    //                    if(measureStateObject)
                    //                    {
                    //                        connect(measureStateObject, SIGNAL(signalCalcRealAngle(QLineF,QLineF,qreal*)), this, SLOT(slotGetRealAngle(QLineF,  QLineF, qreal*)));
                    //                        connect(measureStateObject, SIGNAL(signalCalcRealDistance(QLineF,qreal*)), this, SLOT(slotGetRealDistanceMeters(QLineF,qreal*)));
                    //                        connect(measureStateObject, SIGNAL(signalCalcWGS84Radius(QLineF,qreal*)), this, SLOT(slotGetRealDistanceMeters(QLineF,qreal*)));
                    //                        connect(measureStateObject, SIGNAL(signalSelectReachArea(QPolygonF)), this, SLOT(selectReachArea(QPolygonF)));
                    //                        connect(measureStateObject, SIGNAL(signalCalcWGS84Square(QPolygonF,qreal*)), this, SLOT(slotGetRealSquareMeters(QPolygonF,qreal*)));
                    //                        connect(measureStateObject, SIGNAL(signalGetLineResolution(QPolygonF,qreal*)), this, SLOT(slotGetLineResolution(QPolygonF,qreal*)));
                    //                    }

                    stateObject->init(getScene(), this, &m_zoom, &m_scale, m_sceneControlle->getFrameCoef(), m_sceneControlle->getVisualizerId());
                }

                m_stateObject.clear();
                m_stateObject = stateObject;
            }
            else //
            {
                if( addState == false)
                {
                    if(m_stateObject.isNull() == false)
                    {
                        m_stateObjectStack.push(m_stateObject);
                        m_stateObject->statePushedToStack(); // уведомляем о смене режима !
                    }

                    m_stateObject.clear(); // удаляет тот кто его создал после уведомления о закрытии состояния !!!
                }
            }
        }break;

        default:
        {
            m_viewerState = m_defaultMode;
            qDebug() << "ViewerController::slotSwitchModeOn(): WARN! Unsupported ViewerState mode! Check code!";
            return;
        }break;

        }
    } // end  for(int step = 1; step <= 2; step++)
}


void ViewerController::slotSearchRectCreationCompleted(QRectF searchRect, quint64 brId)
{
    qreal wgsRectOffset = 0.0000005;
    bool isPointRect;
    QPointF p1 = searchRect.topLeft();
    QPointF p2 = searchRect.bottomRight();
    QPointF wgsP1 = m_sceneControlle->sceneToGeo(QPointF(p1.x(), p1.y()));
    QPointF wgsP2 = m_sceneControlle->sceneToGeo(QPointF(p2.x(), p2.y()));

    //    QRectF wgsRect;
    //    {
    //        if(wgsP1.x() < wgsP2.x())
    //        {
    //            topLeft.setX( wgsP1.x());
    //        }
    //        else
    //        {
    //            topLeft.setX( wgsP2.x());
    //        }

    //        if(wgsP1.y() > wgsP2.y())
    //        {
    //            topLeft.setY( wgsP1.y());
    //        }
    //        else
    //        {
    //            topLeft.setY( wgsP2.y());
    //        }

    //    }
    QRectF r = QRectF(wgsP1, wgsP2);
    r=r.normalized();
    isPointRect = false;
    QPointF topLeft;

    //    QSizeF sz = QSizeF(qAbs(wgsP1.y() - wgsP2.y()), qAbs(wgsP1.x() - wgsP2.x()));
    //    wgsRect = QRectF(r ,sz);

    emit signalFindOSMObjectsInRect(r, isPointRect, brId, true);

}

void ViewerController::slotGetRealDistanceMeters(QLineF lineF ,qreal* dist)
{
    GeographicLib::Geodesic geod(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f());

    GeographicLib::PolygonArea poly(geod);

    QPointF p1 = lineF.p1();
    QPointF p2 = lineF.p2();

    m_sceneControlle->sceneToRealGeo(p1);
    m_sceneControlle->sceneToRealGeo(p2);
    double lat1 = p1.x();
    double lon1 = p1.y(); // JFK
    double lat2 = p2.x();
    double lon2 = p2.y(); // SIN
    double s12, azi1, azi2;

    double a12 = geod.Inverse(lat1, lon1, lat2, lon2, s12, azi1, azi2);


    *dist = s12;
}


void ViewerController::slotGeoImageCutted(int cutterTaskId, int percent)
{
//    if(cutterTaskId == m_cutterTaskId)
//    {
//        if(100 == percent)
//        {
//            dpf::service::LinearResolutionService lrm_service;
//            dpf::io::GeoImageObject geo_image = dpf::io::open_geoimage(m_cuttedFilename.toStdString());

//            lrm_service.initialize(m_cuttedFilename.toStdString());
//            lrm_service.run();

//            double lx = lrm_service.lx();
//            double ly = lrm_service.ly();

//            if(geo_image.is_matched())
//            {
//                qreal geoScale = geo_image.geo_meter_scale();
//                qreal lineResolution = (lx+ly)*geoScale/2.0;
//                QString str = QString::number(lineResolution, 'f', 2) + QString("m");
//                if(0 != m_pCalcLineResolutionItem)
//                    m_pCalcLineResolutionItem->setPhysValue(str);
//            }
//            else
//            {
//                qDebug() << "ViewerController::slotGeoImageCutted: ERROR! Image from cutter is not matched!";
//                m_pCalcLineResolutionItem->setPhysValue(QString("0m"));
//            }
//        }
//    }
}



void  ViewerController::convertMetersToNormString(qreal meters, QString& valueRes, QString& dimensionStr)
{
    QString normStr;
    /*переводим в адекватные единицы измерения*/
    if(meters < 0.1)
    {
        valueRes = normStr.setNum( (int)(meters*100),10 );
        dimensionStr = QString(tr("cm"));
    }


    if(meters < 1)
    {
        valueRes = normStr.setNum( (int)(meters*10),10 );
        dimensionStr = QString(tr("dm"));

    }

    if(meters >= 1)
    {
        valueRes = normStr.setNum( (int)(meters),10 );
        dimensionStr = QString(tr("m"));

    }

    if(meters>=1000 )
    {
        valueRes = normStr.setNum( (meters/1000.0), 'f' ,1 ) ;
        dimensionStr = QString(tr("km"));
    }

    // return normStr;
}


void  ViewerController::convertMetersSquareToNormString(qreal meters, QString& valueRes, QString& dimensionStr)
{
    QString normStr;
    /*переводим в адекватные единицы измерения*/
    if(meters < 0.01)
    {
        valueRes = normStr.setNum( (int)(meters*10000),10 );
        dimensionStr = QString(tr("cm")) + QString(QChar(178));
    }


    if(meters < 0.1)
    {
        valueRes = normStr.setNum( (int)(meters*100),10 );
        dimensionStr = QString(tr("dm"))+ QString(QChar(178));

    }

    if(meters >= 1)
    {
        valueRes = normStr.setNum( (int)(meters),10 );
        dimensionStr = QString(tr("m"))+ QString(QChar(178));

    }

    if(meters>=1000000 )
    {
        valueRes = normStr.setNum( (meters/1000000.0), 'f' ,1 ) ;
        dimensionStr = QString(tr("km"))+ QString(QChar(178));
    }

    // return normStr;
}

void ViewerController::slotGetLineResolution(QPolygonF polyPoints, qreal* square)
{
    QRectF boundRect = polyPoints.boundingRect();

    if(m_sceneControlle->isGeo())
    {
        QPointF topLeft = boundRect.topLeft();
        QPointF bottomRight = boundRect.bottomRight();

        QPointF topLeftGeo = m_sceneControlle->sceneToGeo(topLeft);
        QPointF bottomRightGeo = m_sceneControlle->sceneToGeo(bottomRight);


        QSizeF sz = boundRect.size();
        sz.setWidth(m_pCalcLineResolutionItem->boundingRect().size().width()*m_scale);
        sz.setHeight(m_pCalcLineResolutionItem->boundingRect().size().height()*m_scale);
//        contur::TmsCutter * cutter = contur::TmsCutter::instance();
//        m_cutterTaskId = cutter->cutArea(QPointF(topLeftGeo.y(),topLeftGeo.x()),
//                                         QPointF(bottomRightGeo.y(), bottomRightGeo.x()),sz.toSize() ,m_cuttedFilename );

    }
    else
    {
//        QString fname = QUrl(m_sceneControlle->getProviderUrl()).toLocalFile();
//        //QString strFilename;6

//        dpf::service::LinearResolutionService lrm_service;
//        dpf::io::GeoImageObject geo_image = dpf::io::open_geoimage(fname.toStdString());

//        lrm_service.initialize(fname.toStdString());
//        lrm_service.run();

//        double lx = lrm_service.lx();
//        double ly = lrm_service.ly();

//        double lineResolution;
//        QString resStr;
//        if(0 ==lx || 0 == ly)
//        {
//            lineResolution = 1;
//        }
//        else
//        {
//            resStr = QString::number(lineResolution, 'f', 2) + "px" ;
//        }


//        if(geo_image.is_matched())
//        {
//            lineResolution = (lx+ly)*geo_image.geo_meter_scale()/2.0;
//            resStr = QString::number(lineResolution, 'f', 2) + "m" ;
//        }
//        else
//        {
//            lineResolution = (lx+ly)/2.0;
//            resStr = QString::number(lineResolution, 'f', 2) + "px" ;
//        }

//        m_pCalcLineResolutionItem->setPhysValue(resStr);
    }
}



void ViewerController::slotGetRealSquareMeters(QPolygonF polyPoints,qreal* square)
{
//    QPolygonF geoCoords;
//    //   QPolygonF polyPointsTmp;
//    bool isNeedInvert = false;
//    QPointF wgs84_1(m_sceneControlle->sceneToGeo2( mapToScene(rect().center())));
//    QPointF wgs84_2(m_sceneControlle->sceneToGeo2( mapToScene(rect().center().x() + 1, rect().center().y() + 1)));
//    /*
//    for(int i = 1; i<polyPoints.count(); i++)
//    {
//        polyPointsTmp.append(polyPoints.at(polyPoints.count() - i));
//    }*/

//    if( !((wgs84_1.x() >= wgs84_2.x() && wgs84_1.y() <= wgs84_2.y())
//          || (wgs84_1.x() <= wgs84_2.x() && wgs84_1.y() >= wgs84_2.y() )) )
//    {
//        isNeedInvert = true;
//    }

//    GeographicLib::Geodesic geod(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f());

//    GeographicLib::PolygonArea poly(geod);

//    foreach (QPointF p, polyPoints)
//    {
//        QPointF pf =
//                m_sceneControlle->sceneToGeo2(p);
//        double lat1 = pf.x();
//        double lon1 = pf.y(); // JFK

//        poly.AddPoint( (std::real)(lat1), (std::real)(lon1) );
//    }

//    double perimeter, area;
//    //poly.
//    /*
//    if(!isNeedInvert)
//    {
//        if()
//    }*/

//    unsigned n = 0;
//    poly.Compute(true, true, perimeter, area);
//    if((qreal)area < 0)
//    {
//        poly.Compute(false, true, perimeter, area);
//    }
//    /* if( m_sceneControlle->sceneIsGeo() || (m_sceneControlle->frameIsMatching() && isNeedInvert))
//    {
//        n = poly.Compute(false, true, perimeter, area);
//        if((qreal)area < 0)
//        {

//        }
//    }

//    if( (m_sceneControlle->frameIsMatching() && !isNeedInvert))
//        n =  poly.Compute(true, true, perimeter, area);
//*/
//    *square = (qreal)area;
}

void ViewerController::slotGetRealAngle(QLineF lineF1, QLineF lineF2, qreal* angle)
{
    GeographicLib::Geodesic geod1(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f());

    QPointF p11 = lineF1.p1();
    QPointF  p12 = lineF1.p2();
    QPointF p21 = lineF2.p1();
    QPointF p22 = lineF2.p2();

    m_sceneControlle->sceneToRealGeo(p11);
    m_sceneControlle->sceneToRealGeo(p12);
    m_sceneControlle->sceneToRealGeo(p21);
    m_sceneControlle->sceneToRealGeo(p22);

    double lat11 = p11.x();
    double lon11 = p11.y(); // JFK
    double lat12 = p12.x();
    double lon12 = p12.y(); // SIN
    double s121, azi112, azi121;

    double a121 = geod1.Inverse(lat11, lon11, lat12, lon12, s121, azi112, azi121);


    GeographicLib::Geodesic geod2(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f());

    double lat21 = p21.x();
    double lon21 = p21.y(); // JFK
    double lat22 = p22.x();
    double lon22 = p22.y(); // SIN
    double s221, azi212, azi221;

    double a122 = geod2.Inverse(lat21, lon21, lat22, lon22, s221, azi212, azi221);

    qreal angle1 = 0;

    if(azi121 < 0)
    {
        azi121 = 360 + azi121;
    }

    if(azi212 < 0)
    {
        azi212 = 360 + azi212;
    }

    if(azi212 > azi121)
    {
        angle1 = azi212 - azi121;
    }
    else
    {
        angle1 = (360 - azi121) + azi212;
    }


    QPolygonF geoCoords;
    // QPolygonF polyPointsTmp = polyPoints;
    bool isNeedInvert = false;
    QPointF wgs84_1(m_sceneControlle->sceneToGeo2( mapToScene(rect().center())));
    QPointF wgs84_2(m_sceneControlle->sceneToGeo2( mapToScene(rect().center().x() + 1, rect().center().y() + 1)));

    if( !((wgs84_1.x() >= wgs84_2.x() && wgs84_1.y() <= wgs84_2.y())
          || (wgs84_1.x() <= wgs84_2.x() && wgs84_1.y() >= wgs84_2.y() )) )
    {
        isNeedInvert = true;
    }

    qDebug() << azi121 << " "<< azi212 << " " << angle1;
    //    *angle = angle1;

    if( m_sceneControlle->sceneIsGeo() || (m_sceneControlle->frameIsMatching() && !isNeedInvert))
        *angle = angle1;

    if( (m_sceneControlle->frameIsMatching() && isNeedInvert))
        *angle = 360.0 - angle1;

    /*if(!isNeedInvert)
        *angle = 360 - angle1;
    else*/

}



void ViewerController::slotSceneWidgetButtonClicked( int buttonId, bool isPressed)
{
    //    sw::SceneWidget * bbw = dynamic_cast<sw::SceneWidget*>(sender());
    //    if(0 != bbw)
    {
        sw::BBW_BUTTON_ID btnId = (sw::BBW_BUTTON_ID)buttonId;

        // if( !m_sceneControlle->sceneIsGeo())
        //     return;

        switch (btnId)
        {
        case sw::BTN_WHAT_THIS:
        {
            slotDetectViewerMode("SelectAreaForSearch",isPressed);
        }break;

        case sw::BTN_CALC_ANGLES:
        {
            slotDetectViewerMode("CalcAngles",isPressed);
        }break;

        case sw::BTN_CALC_LENGHT:
        {
            slotDetectViewerMode("CalcLength",isPressed);
        }break;

        case sw::BTN_CALC_ROUTE:
        {
            slotDetectViewerMode("CalcRoute",isPressed);
        }break;

        case sw::BTN_CALC_SQUARE:
        {
            slotDetectViewerMode("CalcSquare",isPressed);
        }break;

        case sw::BTN_CALC_REACH:
        {
            slotDetectViewerMode("CalcReach",isPressed);
        }break;


        case sw::BTN_DETECT_LINE_RESOLUTION:
        {
            slotDetectViewerMode("CalcLineResolution",isPressed);
        }break;

        case sw::BTN_VI_DETECT_INTERESTING_ZONE:
        {
            slotDetectViewerMode("SelectInterestingZone",isPressed);
        }break;

        default:
        {
            qDebug() << "ViewerController::slotSceneWidgetButtonClicked(): WARN! Button is not exist here!";
        }
            break;
        }
    }
}

void ViewerController::finish_getPoint()
{
    slotDetectViewerMode("finishGetPoint");
}

void ViewerController::getPoint(QCursor *cursor)
{
    QVariant var;
    var.setValue(cursor);
    slotDetectViewerMode("GetPoint",var );
}

void ViewerController::scrollContentsBy(int dx, int dy)
{
    //qDebug() << "111 scrollContentsBy";
    QGraphicsView::scrollContentsBy(dx,dy);
    const QRectF r(getViewportRect());
    emit updateFromScroll(m_zoom, r, true);
    updateMiniViewer(r);
}

void ViewerController::resizeEvent(QResizeEvent* e)
{
    //qDebug() << "111 resizeEvent";

    m_leftBoundRect = QRect(0,0,m_screenBounds,height());
    m_rightBoundRect = QRect(width() - m_screenBounds,0,m_screenBounds,height());
    m_topBoundRect = QRect(0,0,width(),m_screenBounds);
    m_bottomBoundRect = QRect(0,height() - m_screenBounds,width(),m_screenBounds);

    disconnect(this, SIGNAL(updateFromScroll(int,QRectF,bool))
               , m_viewPortController, SLOT(update(int,QRectF,bool)));
    QGraphicsView::resizeEvent(e);
    const QRectF r(getViewportRect());
    emit updateFromResize(m_zoom, r, true);
    updateMiniViewer(r);
    connect(this, SIGNAL(updateFromScroll(int,QRectF,bool))
            , m_viewPortController, SLOT(update(int,QRectF,bool)));
    emit signalResized( QSize(width(), height()) );
}


QRectF ViewerController::getViewportRect()
{
    QSizeF size(contentsRect().size());
    if(m_angle == 0)
        return QRectF( mapToScene(QPoint(0,0)), mapToScene(QPoint(size.width(),size.height())));
    else
        return getViewportPolygon().boundingRect();
}


QRectF ViewerController::getViewportRectPart(int dx, int dy)
{
    QRectF resRect ;

    QSizeF size(contentsRect().size());


    if(m_angle == 0)
    {
        if(size.width() <= 2*dx || size.height() <= 2*dy)
        {
            dy = 0;
            dx = 0;
        }

        resRect = QRectF( mapToScene(QPoint(dx,dx)), mapToScene(QPoint(size.width() - dx,size.height() - dy))).normalized();
    }

    else
    {
        if(size.width() <= 2*dx || size.height() <= 2*dy)
        {
            dy = 0;
            dx = 0;
        }

        QRectF resRect = QRectF( mapToScene(QPoint(dx,dx)), mapToScene(QPoint(size.width() - dx,size.height() - dy))).normalized();

        QMatrix matr;
        QPointF cntr= resRect.center() ;
        //  matr.translate(cntr.x(), cntr.y() );
        matr.rotate( -m_angle );
        QPolygonF poly;

        QPointF p1 = matr.map(QPointF(resRect.topLeft().x() - cntr.x(), resRect.topLeft().y() - cntr.y()));
        QPointF p2 = matr.map(QPointF(resRect.topRight().x() - cntr.x(), resRect.topRight().y() - cntr.y()));
        QPointF p3 = matr.map(QPointF(resRect.bottomRight().x() - cntr.x(), resRect.bottomRight().y() - cntr.y()));
        QPointF p4 = matr.map(QPointF(resRect.bottomLeft().x() - cntr.x(), resRect.bottomLeft().y() - cntr.y()));
        poly << p1 << p2 << p3 << p4;

        resRect = poly.boundingRect();

    }

    return resRect;
}

QPolygonF ViewerController::getViewportPolygon()
{
    QSizeF size(contentsRect().size());
    QPolygonF polygon;
    polygon << mapToScene(QPoint(0,0)) << mapToScene(QPoint(0,size.height())) << mapToScene(QPoint(size.width(),size.height())) << mapToScene(QPoint(size.width(),0));
    return polygon;
}

void ViewerController::updateMiniViewer(const QRectF& r)
{
    m_miniView->viewCenterOn(r.center());
    QPolygonF pol(getViewportPolygon());
    m_miniView->setPolygon(pol);
}
void ViewerController::slotUpdateBackgroundItem(QRect viewArea)
{
    slotCheckUpdate();
}
void ViewerController::slotCheckUpdate()
{
    if(m_sceneControlle->getTileSceneAsinc()->isActive() == false)
    {
        if(m_viewPortController->m_chenged)
        {
            m_viewPortController->m_chenged = false;

            const double K(pow(2, m_zoom-1));
            const uint D_W(m_sceneControlle->addTileOnViewport() * m_sceneControlle->tileSize().width());
            const uint D_H(m_sceneControlle->addTileOnViewport() * m_sceneControlle->tileSize().height());
            QRect viewportRect( -D_W, -D_H, contentsRect().width() + D_W*2, contentsRect().height() + D_H*2);
            QPolygonF polygon;
            polygon << mapToScene(viewportRect.topLeft()) << mapToScene(viewportRect.topRight()) << mapToScene(viewportRect.bottomRight()) << mapToScene(viewportRect.bottomLeft());
            QRectF sceneRect = polygon.boundingRect();
            QSize imageSize(sceneRect.width() * K, sceneRect.height() * K);
            //            qDebug() << "222 imageSize :" << imageSize << ", sceneRect :" << sceneRect << ", items on main scene :" << items().size();
            m_sceneControlle->getTileSceneAsinc()->startRender(imageSize, sceneRect, m_zoom);
        }
        //        else if((QDateTime::currentMSecsSinceEpoch()-_backgroundItem->lastUpdate) > 5 * 1000)
        //        {
        //            qDebug() << "---updateContent---";
        //            _backgroundItem->lastUpdate = QDateTime::currentMSecsSinceEpoch();
        //            m_sceneControlle->updateContent();
        //        }
    }
}
void ViewerController::slotFinishRender(QImage* image, QRectF sceneRect, int zLevel)
{
    _backgroundItem->resetTransform();
    const double old_K(pow(2, zLevel-1));
    _backgroundItem->setTransform(QTransform().scale(1./old_K, 1./old_K));
    _backgroundItem->setPos(sceneRect.topLeft());
    _backgroundItem->setPixmap(QPixmap::fromImage(*image));
    delete image;
}

void ViewerController::slotWheelEvent(QWheelEvent* e)
{
    wheelEvent(e);
}

void ViewerController::wheelEvent(QWheelEvent* e)
{
    if(isEnabled() == false)
        return;

    if(m_viewerState == StateObjectHandlerEvents)
    {
        if(m_stateObject.isNull() == false)
        {
            const QPointF scenePos(mapToScene(e->pos().x(), e->pos().y()));
            if( m_stateObject->wheelEvent(e, scenePos) == false ) // если состояние хочет отдать событие вьюхе - отдаём !
                return;
        }
    }

//    if(e->modifiers() & Qt::ControlModifier)
//    {
//        if((! m_blockRotate) && (! m_blockRotateFromKompas))
//        {
//            disconnect(this, SIGNAL(updateFromScroll(int,QRectF,bool)), m_viewPortController, SLOT(update(int,QRectF,bool)));
//            disconnect(this, SIGNAL(updateFromResize(int,QRectF,bool)), m_viewPortController, SLOT(update(int,QRectF,bool)));

//            QPointF _center;
//            _center = getViewportRect().center();

//            if(e->delta() > 0)
//            {
//                rotate(1);
//                m_angle += 1;
//            }
//            else
//            {
//                rotate(-1);
//                m_angle -= 1;
//            }
//            centerOn(_center);

//            if (m_angle >= 360)
//                m_angle -= 360;
//            if (m_angle < 360)
//                m_angle += 360;

//            emit signalSetAngle(m_angle);

//            QRectF r = getViewportPolygon().boundingRect();
//            double dX = r.center().x() - r.x();
//            double dY = r.center().y() - r.y();
//            double maxD(dX > dY ? dX : dY);
//            QRectF resRect(r.center().x() - maxD, r.center().y() - maxD, maxD*2., maxD*2.);
//            emit updateFromZoom(m_zoom, resRect, false);

//            connect(this, SIGNAL(updateFromScroll(int,QRectF,bool)), m_viewPortController, SLOT(update(int,QRectF,bool)));
//            connect(this, SIGNAL(updateFromResize(int,QRectF,bool)), m_viewPortController, SLOT(update(int,QRectF,bool)));

//            QPointF wgs84_1(m_sceneControlle->sceneToGeo2( mapToScene(rect().center())));
//            QPointF wgs84_2(m_sceneControlle->sceneToGeo2( mapToScene(rect().center().x() + 1, rect().center().y())));
//            qreal mInPixel = m_sceneControlle->getDistanceWGS84(wgs84_1,wgs84_2);
//            emit sigSetInterpixelDistance(mInPixel);
//        }
//    }
//    else
    {
        m_changeZoomOnCenter = false;
        if(e->delta() > 0)
        {
            if(-1 != m_zoomForSet)
                changeZoom(m_zoomForSet + 1);//zoomIn();
            else
                changeZoom(m_zoom + 1);
        }
        else
        {
            if(-1 != m_zoomForSet)
                changeZoom(m_zoomForSet - 1);//zoomIn();
            else
                changeZoom(m_zoom - 1);
        }

        if(m_zoom < MINI_VIEW_Z)
        {
            //            m_viewportArea->hide();
            //            m_miniView->resetTransform();
        }
        else
        {
            //            m_viewportArea->show();
        }
    }

}

void ViewerController::slotSetAutoScroll()
{
    if(m_autoScroll)
        m_autoScroll = false;
    else
        m_autoScroll = true;
    return;
}


void ViewerController::updateContent(bool deepRepaint)
{
    m_viewPortController->updateContent(deepRepaint);
}


void ViewerController::slot_sceneCenterOn(QPointF scenePos)
{
    if(getViewportRect().contains(scenePos) == false)
        centerOn(scenePos);
}

void ViewerController::changeZoom(int zoom, bool withDelay)
{
    //if(m_changeZoomDelayTimer->isActive())
    int zoomForSet = -1;
    if(withDelay)
    {
//        qDebug() << "ViewerController::changeZoom, if(withDelay), zoom" << zoom << "m_zoomMax" << m_zoomMax << "m_zoomMin" << m_zoomMin;

        m_changeZoomDelayTimer->start();
        if(zoom <= m_zoomMax && zoom >= m_zoomMin)
        {
            m_zoomForSet = zoom;
            zoomForSet = m_zoomForSet;
        }
        else
        {
            if(zoom >= m_zoomMax)
            {
                m_zoomForSet = m_zoomMax;
                zoomForSet = m_zoomForSet;
            }

            if(zoom <= m_zoomMin)
            {
                m_zoomForSet = m_zoomMin;
                zoomForSet = m_zoomForSet;
            }
        }

//        qDebug() << "ViewerController::changeZoom, m_zoomForSet" << m_zoomForSet;
    }
    else
    {
//        qDebug() << "ViewerController::changeZoom, if(withDelay) ELSE";

        if(zoom <= m_zoomMax && zoom >= m_zoomMin)
        {
            m_zoomForSet = zoom;
            zoomForSet = m_zoomForSet;
        }
        else
        {
            if(zoom >= m_zoomMax)
            {
                m_zoomForSet = m_zoomMax;
                zoomForSet = m_zoomForSet;
            }

            if(zoom <= m_zoomMin)
            {
                m_zoomForSet = m_zoomMin;
                zoomForSet = m_zoomForSet;
            }
        }
        slotChangeZoomTimeout();
    }


    emit signalScaleChanged(zoomForSet, (int)m_sceneControlle->zoomMax(), (int)m_sceneControlle->zoomMin());
    //emit signalScaleChanged( m_scale, m_zoom);

//    qDebug() << "ViewerController::changeZoom --- end";
}


void ViewerController::slotChangeZoomTimeout()
{
    if(-1 != m_zoomForSet)
    {
//        qDebug() << "ViewerController::slotChangeZoomTimeout, IF, m_zoomForSet" << m_zoomForSet;
        slotChangeZoom(m_zoomForSet);

    }
    else
    {
//        qDebug() << "ViewerController::slotChangeZoomTimeout, ELSE, m_zoomForSet" << m_zoomForSet;
    }

    m_zoomForSet = -1;
    m_changeZoomDelayTimer->stop();
}


void ViewerController::slotChangeZoom(int zoom)
{
    QGraphicsView::ViewportAnchor prevAnchors = transformationAnchor();

    if(m_changeZoomOnCenter)
        setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    else
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    int diffZoom = abs(m_zoom - zoom);
    double scaleK = 1;

    //    qDebug() << "ViewerController::slotChangeZoom, zoom" << zoom;

    if( zoom < (int)m_sceneControlle->zoomMin())
    {
        //emit signalScaleChanged(m_zoom, (int)m_sceneControlle->zoomMax(), (int)m_sceneControlle->zoomMin());
        //emit signalScaleChanged( m_scale, m_zoom);
        return;
    }

    if(0 == diffZoom)
    {
        return;
    }

    if(m_zoom < zoom)
    {
        scaleK = 2;
        m_scale *= pow(scaleK,diffZoom);
    }
    else
    {
        scaleK = 0.5;
        m_scale /= pow(2,diffZoom);
    }

    disconnect(this, SIGNAL(updateFromScroll(int,QRectF,bool)), m_viewPortController, SLOT(update(int,QRectF,bool)));
    disconnect(this, SIGNAL(updateFromResize(int,QRectF,bool)), m_viewPortController, SLOT(update(int,QRectF,bool)));

    //    qDebug() << "ViewerController::slotChangeZoom, scaleK" << scaleK << "diffZoom" << diffZoom;

    scale(pow(scaleK, diffZoom) ,pow(scaleK, diffZoom));

    {
        QMatrix matr = m_miniView->matrix();

        qreal m11;
        qreal m22;


        if(zoom  < MINI_VIEW_Z)
        {
            m11 = 1;
            m22 = 1;
        }
        else
        {
            m11 = pow(2,zoom - MINI_VIEW_Z);//pow(2, m_zoom - MINI_VIEW_Z);
            m22 = pow(2,zoom - MINI_VIEW_Z);//pow(2, m_zoom - MINI_VIEW_Z);
        }

        QMatrix newMatrix(m11, matr.m12(), matr.m21(), m22, matr.dx(), matr.dy() );

        m_miniView->setMatrix(newMatrix, false);
        updateMiniViewer(getViewportRect());
    }

    m_zoom = zoom;
    emit updateFromZoom(m_zoom, getViewportRect(), true);

    connect(this, SIGNAL(updateFromScroll(int,QRectF,bool)), m_viewPortController, SLOT(update(int,QRectF,bool)));
    connect(this, SIGNAL(updateFromResize(int,QRectF,bool)), m_viewPortController, SLOT(update(int,QRectF,bool)));

    //emit signalScaleChanged(m_zoom, (int)m_sceneControlle->zoomMax(), (int)m_sceneControlle->zoomMin());
    //emit signalScaleChanged( m_scale, m_zoom);


    /*залипуха*/
    const QRectF r(getViewportRect());
    emit updateFromResize(m_zoom, r, true);
    updateMiniViewer(r);

    if(DenyScrollOverAndSelect == m_viewerState)
    {
        if(0 != m_pBindedLayer)
        {
            centerOn(m_pBindedLayer->boundingRect().center());

            QRectF scRectIn = getViewportRectPart(viewportBindingPartPx,viewportBindingPartPx);
            if(0 != m_pViewRectPartLayer)
            {
                if(m_pViewRectPartLayer->rect() != scRectIn)
                    m_pViewRectPartLayer->setRect(scRectIn);
            }
        }
    }

    QPointF wgs84_1(m_sceneControlle->sceneToGeo2( mapToScene(rect().center())));
    QPointF wgs84_2(m_sceneControlle->sceneToGeo2( mapToScene(rect().center().x() + 1, rect().center().y())));
    qreal mInPixel = m_sceneControlle->getDistanceWGS84(wgs84_1,wgs84_2);
    emit sigSetInterpixelDistance(mInPixel);

    QPointF _center;

    /*центрируемся по объекту*/
    if(CenterOnObject == m_viewerState ||  DenyScrollOverAndSelect == m_viewerState)
    {
        if(0 != m_pBindedLayer)
        {
            _center = m_pBindedLayer->rect().center();
            centerOn(m_pBindedLayer);
        }
    }

    setTransformationAnchor(prevAnchors);
    setItemsScale();
    m_changeZoomOnCenter = true;

    emit signalScaleChanged( m_scale, m_zoom);

    if(m_stateObject.isNull() == false)
        m_stateObject->zoomChanged();
}


void ViewerController::setItemsScale()
{
    if(0 != m_pCalcSquareItem)
    {
        m_pCalcSquareItem->setSceneScale(m_scale);
    }

    if(0 != m_pCalcLineResolutionItem)
    {
        m_pCalcLineResolutionItem->setSceneScale(m_scale);
    }

    if(0 != m_pCalcRouteItem)
    {
        m_pCalcRouteItem->setSceneScale(m_scale);
    }

    if(0 != m_pCalcLengthItem)
    {
        m_pCalcLengthItem->setSceneScale(m_scale);
    }

    if(0 != m_pCalcAnglesItem)
    {
        m_pCalcAnglesItem->setSceneScale(m_scale);
    }

    if(0 != m_pReachGraphicsItem)
    {
        m_pReachGraphicsItem->setSceneScale(m_scale);
    }
    if(0 != m_pSelectEllipseAreaItem)
    {
        m_pSelectEllipseAreaItem->setSceneScale(m_scale);
    }

    if(0 != m_pSelectEllipseAreaWithSetRadItem)
    {
        m_pSelectEllipseAreaWithSetRadItem->setSceneScale(m_scale);
    }
}

void ViewerController::slotBlockRotateFromKompas(bool blockRotate)
{
    qDebug() << "ViewerController::slotBlockRotateFromKompas, blockRotate :" << blockRotate;

    //    QPointF firstPos(41., 24.706951);
    //    QPointF firstPos(40.703539, 24.706951);
    //    QTime time;
    //    time.start();

    //    static int K(0);
    //    K += 3;

    //    for(int i(K); i < K+3; ++i)
    //    {
    //        double I = i * 0.01;
    //        double X(firstPos.x()+I), Y(firstPos.y()/*+I*/), alt(0);
    //        AltitudeController::SearchResult searchResult = AltitudeController::instance()->getAltitudeSinc(X,Y,alt);
    //        if(searchResult == AltitudeController::Found)
    //            qDebug() << "Found : pos" << X << Y << "-" << alt;
    //        if(searchResult == AltitudeController::NotFound)
    //            qDebug() << "NotFound : pos" << X << Y;
    //        if(searchResult == AltitudeController::InProgress)
    //            qDebug() << "InProgress : pos" << X << Y;
    //    }
    //    qDebug() << "Time :" << time.elapsed();

    m_blockRotateFromKompas = blockRotate;
    slotRotateFromKompas(0);
}


void ViewerController::slotBlockScrollAndSelect(bool isBlocked)
{

    QVariant var;
    var = isBlocked;
    slotDetectViewerMode("slotBlockScrollAndSelect",var);
}

void ViewerController::blockRotate(bool block)
{

    m_blockRotate = block;

    if(m_blockRotate || m_blockRotateFromKompas)
        emit signalRotateBlocked(true);
    else
        emit signalRotateBlocked(false);


}


void ViewerController::slotRotateFromKompas(qreal angle)
{
    if(!m_blockRotate && !m_blockRotateFromKompas)
    {
        disconnect(this, SIGNAL(updateFromScroll(int,QRectF,bool)), m_viewPortController, SLOT(update(int,QRectF,bool)));
        disconnect(this, SIGNAL(updateFromResize(int,QRectF,bool)), m_viewPortController, SLOT(update(int,QRectF,bool)));

        QPointF _center = getViewportRect().center();
        rotate(angle - m_angle );
        m_angle = angle;
        centerOn(_center);

        emit updateFromZoom(m_zoom, getViewportRect(), false);
        emit signalSetAngle(angle);

        connect(this, SIGNAL(updateFromScroll(int,QRectF,bool)), m_viewPortController, SLOT(update(int,QRectF,bool)));
        connect(this, SIGNAL(updateFromResize(int,QRectF,bool)), m_viewPortController, SLOT(update(int,QRectF,bool)));

        QPointF wgs84_1(m_sceneControlle->sceneToGeo2( mapToScene(rect().center())));
        QPointF wgs84_2(m_sceneControlle->sceneToGeo2( mapToScene(rect().center().x() + 1, rect().center().y())));
        qreal mInPixel = m_sceneControlle->getDistanceWGS84(wgs84_1,wgs84_2);
        emit sigSetInterpixelDistance(mInPixel);
    }
}


void ViewerController::setPreview(bool on_off)
{
    m_miniView->setVisible(on_off);
}


qreal ViewerController::getIntepixelDistance()
{


    QPointF wgs84_1(m_sceneControlle->sceneToGeo2( mapToScene(rect().center())));
    QPointF wgs84_2(m_sceneControlle->sceneToGeo2( mapToScene(rect().center().x() + 1, rect().center().y())));

    qreal mInPixel = m_sceneControlle->getDistanceWGS84(wgs84_1,wgs84_2);

    return mInPixel;
}

void ViewerController::closeEvent(QCloseEvent * e)
{
    QGraphicsView::closeEvent(e);
    m_sceneControlle->removeView(this);
}


void ViewerController::showEvent(QShowEvent * e)
{
    QGraphicsView::showEvent(e);

    QPointF wgs84_1(m_sceneControlle->sceneToGeo2( mapToScene(rect().center())));
    QPointF wgs84_2(m_sceneControlle->sceneToGeo2( mapToScene(rect().center().x() + 1, rect().center().y())));
    qreal mInPixel = m_sceneControlle->getDistanceWGS84(wgs84_1,wgs84_2);
    emit sigSetInterpixelDistance(mInPixel);
}


//void ViewerController::slotScreenShot()
//{
//    QRect _viewport = viewport()->rect();
//    QPointF topLeft(mapToScene(0,0));
//    QPointF bottomRight(mapToScene(_viewport.width(),_viewport.height()));
//    QRectF _viewportReal(topLeft, bottomRight);

//    QImage image(_viewport.size(), QImage::Format_ARGB32);
//    QPainter painter(&image);
//    render(&painter);
//    render(&painter, _viewportReal, _viewport);


//    QList<QPointF> list;
//    list << m_sceneControlle->sceneToGeo(mapToScene(0,0)) << m_sceneControlle->sceneToGeo(mapToScene(_viewport.width(), 0)) << m_sceneControlle->sceneToGeo(mapToScene(_viewport.width(),_viewport.height())) << m_sceneControlle->sceneToGeo(mapToScene(0,_viewport.height()));

//    ScenesHolder::instance()->emit_createNewDoc(&image, list);
//}


void ViewerController::setEditMode(bool on_off, QPixmap pm)
{
    QVariantMap vm;
    vm.insert("onOff", on_off);
    vm.insert("objectIcon", pm);

    QPixmap pixmap ;
    if(!pm.isNull())
    {
        pixmap =  pm.scaled(32,32);
    }
    else
    {
        pixmap = QPixmap(":/322_reper_pts_put.png");
        pixmap = pixmap.scaled(32,32);
    }

    QPainter pr(&pixmap);
    pr.drawPixmap(0,0,pixmap.width()/2.0, pixmap.width()/2.0, QPixmap(":/161_cursor_arrow.png"));
    if(pr.isActive())
        pr.end();

    m_addVectorObjectCursor = QCursor(pixmap, 0, 0);

    slotDetectViewerMode("SetEditMode", vm);
}

//void ViewerController::slotSaveViewportImage()
//{
//    QRectF scene_rect(sceneRect());
//    QRect scene_rect_in_view(mapFromScene(scene_rect.topLeft()), mapFromScene(scene_rect.bottomRight()));
//    QRect r(scene_rect_in_view.intersected(viewport()->rect()));
//    QPixmap pixmap(r.size());
//    pixmap.fill(Qt::white);
//    QPainter painter(&pixmap);
//    render(&painter, QRectF(0,0,r.width(),r.height()), r);
//    ScenesHolder::instance()->emit_signalSaveViewport(pixmap);
//}

void ViewerController::setMinMaxZoom(int zoomMin, int zoomMax)
{
    m_zoomMin = zoomMin;
    m_zoomMax = zoomMax;
    if(m_zoom < m_zoomMin || m_zoom > m_zoomMax)
        changeZoom(m_zoomMin, false);
}

qreal ViewerController::getScaleForZoom(int zoomLevel)
{
    qreal scale = qPow(2,zoomLevel);
    return scale;
}

void ViewerController::slotSetCurrentInterpixelDistance(qreal iPixelDistance)
{

    int curCalcedZoom = m_zoom;
    int prevCalcedZoom = m_zoom;


    qreal scale = getScaleForZoom(m_zoom);
    qreal calcedScale = scale;
    qreal curCalcedInterpixelDist = m_sceneControlle->getIntepixelDistance()/calcedScale;
    qreal prevCalcedInterpixelDist = curCalcedInterpixelDist;
    int calcedZoom = m_zoom;
    while ( curCalcedZoom >= m_zoomMin && curCalcedZoom <= m_zoomMax)
    {

        prevCalcedInterpixelDist = curCalcedInterpixelDist;
        prevCalcedZoom = curCalcedZoom;

        bool tmpIPixelDistMore = false;
        if(iPixelDistance/scale < curCalcedInterpixelDist)
        {
            tmpIPixelDistMore = true;
            curCalcedZoom +=1;
            calcedScale = getScaleForZoom(curCalcedZoom);
            curCalcedInterpixelDist = m_sceneControlle->getIntepixelDistance()/calcedScale;
        }
        else
        {
            tmpIPixelDistMore = false;
            curCalcedZoom -= 1;
            calcedScale = getScaleForZoom(curCalcedZoom);
            curCalcedInterpixelDist = m_sceneControlle->getIntepixelDistance()/calcedScale;
        }

        qreal diff1 = qAbs(iPixelDistance/scale - curCalcedInterpixelDist);
        qreal diff2 = qAbs(iPixelDistance/scale - prevCalcedInterpixelDist);

        //if(diff1 > diff2)
        if(iPixelDistance/scale >= curCalcedInterpixelDist && iPixelDistance/scale <= prevCalcedInterpixelDist)
        {
            calcedZoom = curCalcedZoom -1 ;
            break;
        }

        calcedZoom = curCalcedZoom;
        // calcedZoom =
        // prevCalcedZoom = calcedZoom ;
    }

    changeZoom(calcedZoom, false);
}

bool ViewerController::setState(QSharedPointer<visualize_system::VisualizerStateObject> state)
{
    QSharedPointer<visualize_system::StateObject> stateObject = state.dynamicCast<visualize_system::StateObject>();
    if(stateObject.isNull() == false)
    {
        QList<QVariant> list;

        StateObjectInQVariant stateObjectInQVariant;
        stateObjectInQVariant.stateObject = stateObject;
        QVariant varStateObject;
        varStateObject.setValue(stateObjectInQVariant);
        list.append(varStateObject);

        list.append(false);
        slotDetectViewerMode("stateObjectHandlerEventsOn", list);
        return true;
    }
    else
        return false;
}

void ViewerController::slotStateObjectHandlerEventsOff()
{
    qDebug() << "slotStateObjectHandlerEventsOff --- 111";
    StateObject * stateObject = dynamic_cast<StateObject *>(sender());

    if(stateObject)
    {
        qDebug() << "slotStateObjectHandlerEventsOff --- 333";
        disconnect(stateObject, SIGNAL(signalCloseState()), this, SLOT(slotStateObjectHandlerEventsOff()));
        disconnect(stateObject, SIGNAL(signalConvertNativeToScene(QPointF&)), m_sceneControlle, SLOT(frameToScene(QPointF&)));
        disconnect(stateObject, SIGNAL(signalConvertSceneToNative(QPointF&)), m_sceneControlle, SLOT(sceneToFrame(QPointF&)));
        disconnect(stateObject, SIGNAL(signalConvertEpsgToScene(int,QPointF&,bool&)), m_sceneControlle, SLOT(convertEpsgToScene(int,QPointF&,bool&)));
        disconnect(stateObject, SIGNAL(signalConvertSceneToEpsg(int,QPointF&,bool&)), m_sceneControlle, SLOT(convertSceneToEpsg(int,QPointF&,bool&)));
        disconnect(stateObject, SIGNAL(signalSetZlevel(int)), this, SLOT(slotChangeZoom(int)));
        disconnect(stateObject, SIGNAL(signalSetPrefferZoomForSceneRect(QRectF)), this, SLOT(setPrefferZoomForSceneRect(QRectF)));
        qDebug() << "slotStateObjectHandlerEventsOff --- 444";

        if(m_stateObject.isNull() == false)
            if(m_stateObject.data() == stateObject)
            {
                m_stateObject.clear();
                switchViewerStateMode(StateObjectHandlerEvents, false);
                qDebug() << "slotStateObjectHandlerEventsOff --- 666";
                return;
            }

        for(QStack< QSharedPointer<StateObject> >::iterator stackIt(m_stateObjectStack.begin()); stackIt != m_stateObjectStack.end(); ++stackIt)
        {
            QSharedPointer<StateObject> ptr = *stackIt;
            if( ptr.data() ==  stateObject)
            {
                ptr.clear();
                m_stateObjectStack.erase(stackIt);
                break;
            }
        }
    }
}

void ViewerController::slotChangeEntityMode(quint32 cursorId,bool isEditMode,QPixmap pm)
{
    if(isEditMode != (m_viewerState == EditObjects))
    {
        setEditMode(isEditMode,pm);
    }
}




