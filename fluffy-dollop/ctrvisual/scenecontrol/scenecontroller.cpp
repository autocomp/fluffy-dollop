#include "scenecontroller.h"
#include "parallelcursor.h"
#include "math.h"
#include <ctrcore/visual/basescenelayer.h>

#include <QDebug>
#include <QGraphicsItem>
//#include <objrepr/pframe.h>
//#include <objrepr/reprServer.h>
//#include <ctrcore/visual/dpfcoordtransformer.h>

#include <GeographicLib/PolygonArea.hpp>
#include <GeographicLib/Geodesic.hpp>
#include <GeographicLib/Constants.hpp>

//using namespace objrepr;
using namespace visualize_system;

SceneController::SceneController(uint visualizerId, const SceneDescriptor &sceneDescriptor, QObject *parent)
    : QObject(parent)
    , m_visualizerId(visualizerId)
    , m_isGeo(sceneDescriptor.isGeoScene())
    , m_sceneDescriptor(sceneDescriptor)
    , m_tileSize(sceneDescriptor.tileSize())
    , m_tilesCount(sceneDescriptor.sceneSizeInTiles())
    , m_zoomMin(1)
    , m_zoomMax(22) //! ScenesHolder::instance()->getMaxZlevel()) - НЕОБХОДИМО БРАТЬ ПАРАМЕТР ИЗ ВНЕ !!!
    , m_blockAbortLoading(false)
    , m_Scene(sceneDescriptor.isGeoScene(), sceneDescriptor.sceneSize(), this)
    , m_TileScene(sceneDescriptor.isGeoScene(), sceneDescriptor.sceneSize(), this)
    , m_MinimapScene(sceneDescriptor.isGeoScene(), sceneDescriptor.sceneSize(), this)
    , m_viewCounter(0)
    , m_Koef(0)
    , m_BaseZlevel(-1)
    , m_FrameId(-1)
    , m_HighLightBaseObject(0)
    , m_curCoordSyst(sceneDescriptor.isGeoScene() ? 1 : 0) // гео - "WGS", не гео - "пиксель"
    , m_currentZ(-1)
    , m_BaseGeoZlevel(-1)
    , m_viewOptimization(false)
    , m_altReloadTimer(this)
    , m_policy(Qt::ScrollBarAlwaysOff)
//    , m_SceneInThread(m_isGeo ? new SceneInThread : 0)
{
    m_parallelCursor = new ParallelCursor(getScene());
    m_parallelCursor->hide();

//    QTranslator *trans = new QTranslator(this);
//    if(trans->load("./translate/" + QString("libscenewidget.qm")))
//        QCoreApplication::installTranslator(trans);

//    QTranslator *trans2 = new QTranslator(this);
//    if(trans2->load("./translate/" + QString("libscenecontrol.qm")))
//        QCoreApplication::installTranslator(trans2);

//    connect(&m_altReloadTimer, SIGNAL(timeout()), this, SLOT(slotAltReload()));
    m_TileSceneAsinc.setAsincMode(true); //! ScenesHolder::instance()->asincLoadingOnGeoMap()) - НЕОБХОДИМО БРАТЬ ПАРАМЕТР ИЗ ВНЕ !!!

    if( ! m_isGeo)
        m_Scene.setMouseMoveEnabled(true);

    ///--------------------------------------------------------------------------------
//    if(false)//m_isGeo)
//    {
//        QTime t;
//        t.start();
//        QPolygonF contur;
//        for(int x(-180); x <= 180; ++x)
//        {
//            double X(x), Y(-90);
//            QPointF P = geoToScene(QPointF(Y,X));
//            X = P.x();
//            Y = P.y();
//            if(X >=0 && X < m_tileSize.width()*m_tilesCount.width() && Y >=0 && Y < m_tileSize.height()*m_tilesCount.height())
//                contur.append(QPointF(X,Y));
//            qDebug() << x << "-90" << "1 ---> " << X << Y;
//        }

//        for(int y(-90); y <= 90; ++y)
//        {
//            double X(180), Y(y);
//            QPointF P = geoToScene(QPointF(Y,X));
//            X = P.x();
//            Y = P.y();
//            if(X >=0 && X < m_tileSize.width()*m_tilesCount.width() && Y >=0 && Y < m_tileSize.height()*m_tilesCount.height())
//                contur.append(QPointF(X,Y));
//            qDebug() << "180" << y << "2 ---> " << X << Y;
//        }

//        for(int x(180); x >= -180; --x)
//        {
//            double X(x), Y(90);
//            QPointF P = geoToScene(QPointF(Y,X));
//            X = P.x();
//            Y = P.y();
//            if(X >=0 && X < m_tileSize.width()*m_tilesCount.width() && Y >=0 && Y < m_tileSize.height()*m_tilesCount.height())
//                contur.append(QPointF(X,Y));
//            qDebug() << x << "90" << "3 ---> " << X << Y;
//        }

//        for(int y(90); y >= -90; --y)
//        {
//            double X(-180), Y(y);
//            QPointF P = geoToScene(QPointF(Y,X));
//            X = P.x();
//            Y = P.y();
//            if(X >=0 && X < m_tileSize.width()*m_tilesCount.width() && Y >=0 && Y < m_tileSize.height()*m_tilesCount.height())
//                contur.append(QPointF(X,Y));
//            qDebug() << "-180" << y << "4 ---> " << X << Y;
//        }

//        //        QGraphicsPolygonItem * polItem = new QGraphicsPolygonItem(contur);
//        //        QPen pen(Qt::black);
//        //        pen.setCosmetic(true);
//        //        polItem->setPen(pen);
//        //        getScene()->addItem(polItem);
//        //        polItem->setZValue(50);

//        qDebug() << "===> t.elapsed() :" << t.elapsed();

//        QBrush brush;
//        QColor colBr(Qt::black);
//        brush.setColor(colBr);
//        brush.setStyle(Qt::SolidPattern);

//        //        int C = 2;
//        QPixmap pm(m_tileSize.width()*m_tilesCount.width()/**C*/, m_tileSize.height()*m_tilesCount.height()/**C*/);
//        pm.fill(Qt::transparent);
//        //        for(auto it = contur.begin(); it != contur.end(); ++it)
//        //        {
//        //            QPointF p = *it;
//        //            (*it) = QPointF(p.x()*C, p.y()*C);
//        //        }

//        QPainter pr(&pm);
//        pr.setRenderHint(QPainter::Antialiasing);
//        pr.setPen(Qt::NoPen);
//        pr.setBrush(brush);
//        pr.drawPolygon(contur, Qt::WindingFill);

//        QPixmap pm2(pm.size());
//        pm2.fill(Qt::white);
//        pm2.setMask(pm.createMaskFromColor(Qt::black));

//        QGraphicsPixmapItem * pixmapItem = new QGraphicsPixmapItem(pm2);
//        //        pixmapItem->scale(1/C, 1/C);
//        getScene()->addItem(pixmapItem);
//        pixmapItem->setZValue(50);
//    }
}

SceneController::~SceneController()
{
    foreach(ViewerController* viewerController, m_viewList)
        delete viewerController;
}

void SceneController::updateContent()
{
    updateContent(true);
}

void SceneController::updateContent(bool deepRepaint)
{
    m_blockAbortLoading = true;
    foreach(ViewerController* viewerController, m_viewList)
        viewerController->updateContent(deepRepaint);
    m_blockAbortLoading = false;
}


void SceneController::setFirstProviderUrl(QString url)
{
    m_firstProviderUrl = url;
}


bool SceneController::isGeo()
{
    return m_isGeo;
}

QString SceneController::getProviderUrl()
{
    return m_firstProviderUrl;
}

void SceneController::slotBindLayer(quint64 id, bool isBind)
{
    foreach(ViewerController* viewerController, m_viewList)
    {
        viewerController->slotBindLayer(id, isBind);  ;
    }
}

void SceneController::slotBlockSceneScroll(bool isBlocked)
{
    foreach(ViewerController* viewerController, m_viewList)
        viewerController->slotBlockScrollAndSelect(isBlocked);  ;
}

void SceneController::slotConvertPixelToGeo(QPointF pxPoint, QPointF* pGeoPoint)
{
    *pGeoPoint = sceneToGeo(pxPoint);
}


void SceneController::slotConvertGeoToPixel(QPointF geoPoint, QPointF* pPxPoint)
{
    *pPxPoint = geoToScene(geoPoint);
}


void SceneController::slotGetInterpixelDistance(qreal* res)
{
    *res = getIntepixelDistance();
}



void SceneController::slotGetInterMeterDistance(qreal*dist)
{
    (*dist) = getInterMeterDistanceWGS84();
}

qreal SceneController::getInterMeterDistanceWGS84()
{
    double R = 6367444.6571; // m
    double degToRad = 57.295779513;

    qreal dist = 360.0 / (2 * M_PI * R );

    return dist;

}

double SceneController::getDistanceWGS84(QPointF firstWGS84Point, QPointF secondWGS84Point)
{
    /*double scale = 0;
    double R = 6367444.6571; // m
    double degToRad = 57.295779513;

    double xSize = (secondWGS84Point.x() - firstWGS84Point.x());
    double ySize = (secondWGS84Point.y() - firstWGS84Point.y()) ;

    scale = cos( (firstWGS84Point.y() + (secondWGS84Point.y() - firstWGS84Point.y())/2) * degToRad);
    double xLen = (M_PI*R*( (secondWGS84Point.x() - firstWGS84Point.x()) )/180.0)* scale ;

    double yLen = (M_PI*R*( (secondWGS84Point.y() - firstWGS84Point.y()) )/ 180.0);
    double dist = sqrt( xLen*xLen + yLen*yLen );*/

    GeographicLib::Geodesic geod(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f());

    GeographicLib::PolygonArea poly(geod);

    //sceneToRealGeo(firstWGS84Point);
    //sceneToRealGeo(secondWGS84Point);
    double lat1 = firstWGS84Point.x();
    double lon1 = firstWGS84Point.y(); // JFK
    double lat2 = secondWGS84Point.x();
    double lon2 = secondWGS84Point.y(); // SIN
    double s12, azi1, azi2;

    double a12 = geod.Inverse(lat1, lon1, lat2, lon2, s12, azi1, azi2);
    return s12;
}

/**
 * @brief Считает расстояние между двумя пикселями экрана в метрах карты. Измеряется горизонтальное расстояние. Точность
 *  исключительно для рисования графических элементов.
 * @param zoom
 * @return
 */
qreal SceneController::getIntepixelDistance(qreal m_scale)
{
    qulonglong sceneWPx = tilesCount().width()* tileSize().width();
    int sceneHPx = tilesCount().height()* tileSize().height();

    qreal R = 6367444.6571; // m
    qreal degToRad = 57.295779513;

    qreal dist = ((2* M_PI * R) / (qreal)sceneWPx) / m_scale;

    return dist;
}


qreal SceneController::getIntepixelDistance()
{
    qreal dist = 0;
    
    if(m_viewList.count() > 0)
    {
        ViewerController * vContr = m_viewList.at(0);
        dist = vContr->getIntepixelDistance();
    }
    
    return dist;
}

void SceneController::slotSetMainViewCenterOnGeoPoint(quint64 id, QRectF geoRect, int minVizZLevel,
                                                      int maxVizZLevel, bool useZoom, bool force)
{
    if(m_viewList.count() > 0)
    {
        ViewerController * vContr = m_viewList.at(0);

        QPointF topLeft1( geoRect.topLeft().y(), geoRect.topLeft().x() );
        QPointF topRight1( geoRect.topRight().y(), geoRect.topRight().x() );
        QPointF bottomLeft1( geoRect.bottomLeft().y(), geoRect.bottomLeft().x() );
        QPointF bottomRight1( geoRect.bottomRight().y(), geoRect.bottomRight().x() );

        QPointF topLeft = geoToScene( topLeft1 );
        QPointF topRight = geoToScene( topRight1 );
        QPointF bottomLeft = geoToScene( bottomLeft1 );
        QPointF bottomRight = geoToScene( bottomRight1 );


        QRectF sceneRect;
        sceneRect.setBottomLeft(bottomLeft);
        sceneRect.setBottomRight(bottomRight);
        sceneRect.setTopLeft(topLeft);
        sceneRect.setTopRight(topRight);

        vContr->centerViewOnWithZoom(id, sceneRect,minVizZLevel, maxVizZLevel, useZoom, force);
        /* else
            vContr->centerViewOn(sceneRect.center());*/
    }
    
}

void SceneController::setViewCenterOnObject(quint64 id)
{
    foreach(ViewerController* vc, m_viewList)
        if(vc->setViewCenterOnObject(id))
            break;
}

void SceneController::slotMainViewScaleChanged(qreal scale, int zLevel)
{
    
    emit signalMainViewScaleChanged(scale,zLevel);
}

void SceneController::slotMainViewRotateAngleChanged(qreal angle)
{
    emit signalMainViewAngleChanged(angle);
}


void SceneController::slotObjectCreated(QList<QGraphicsItem*>* list)
{
    if(0 != list)
    {
        foreach(QGraphicsItem * item, *list)
        {
            item->setZValue(item->zValue() + 100);
            
            if(0 == item->scene())
            {
                bool isVisisble = true;
                if(!item->isVisible())
                {
                    isVisisble = false;
                }
                getScene()->addItem(item);
                item->setVisible(isVisisble);
                //item->setCacheMode(QGraphicsItem::ItemCoordinateCache);

            }
            
            BaseSceneLayer* layer = dynamic_cast<BaseSceneLayer*>(item);
            if(layer)
            {
                foreach(ViewerController* vc, m_viewList)
                {
                    connect(layer, SIGNAL(signalSetCreatingMode(bool)), vc, SLOT(slotCreatingMode(bool)));
                    connect(layer, SIGNAL(signalDestroyed()), this, SLOT(slotBaseObjectDestroyed()));
                    connect(layer, SIGNAL(signalSetLayerToCenterViewport(QPointF)), vc, SLOT(slotSetLayerToCenterViewport(QPointF)));
                    layer->setZValue(item->zValue() + 130);
                }
            }
        }
        
        list->clear();
    }
    
    
    delete list;
    emit readyDraw(0);
    
}


void SceneController::setHighLightBaseObject(BaseSceneLayer* highLightBaseObject)
{
    if(highLightBaseObject != m_HighLightBaseObject)
    {
        if(m_HighLightBaseObject)
            m_HighLightBaseObject->setHighLightObject(false);

        if(highLightBaseObject)
            highLightBaseObject->setHighLightObject(true);

        m_HighLightBaseObject = highLightBaseObject;
    }
}

void SceneController::slotBaseObjectDestroyed()
{
    //    qDebug() << "SceneController::slotBaseObjectDestroyed";
    BaseSceneLayer* layer = dynamic_cast<BaseSceneLayer*>(sender());
    if(layer)
    {
        if(m_HighLightBaseObject == layer)
        {
            m_HighLightBaseObject = 0;
            //            qulonglong THIS = (qulonglong)m_HighLightBaseObject;
            //            qDebug() << "--->slotBaseObjectDestroyed, m_HighLightBaseObject:" << THIS;
        }
    }
}


ViewerController* SceneController::addView()
{
    ViewerController* viewerController = new ViewerController(this, ++m_viewCounter);
    viewerController->setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // QGraphicsView::BoundingRectViewportUpdate);
    viewerController->setAttribute(Qt::WA_NoBackground, true);
    m_viewList.append(viewerController);
    viewerController->setMinMaxZoom(m_zoomMin, m_zoomMax);
    viewerController->setHorizontalScrollBarPolicy(m_policy);
    viewerController->setVerticalScrollBarPolicy(m_policy);

    if(1 == m_viewCounter)
    {
        connect(viewerController, SIGNAL(signalSetAngle(qreal)), this, SLOT( slotMainViewRotateAngleChanged(qreal)) );
        connect(viewerController, SIGNAL(signalScaleChanged(qreal, int)), this, SLOT( slotMainViewScaleChanged(qreal, int)) );
    }
    
    return viewerController;
}


void SceneController::addView(ViewerController* viewerController)
{
    viewerController->setViewportUpdateMode(QGraphicsView::FullViewportUpdate); // QGraphicsView::BoundingRectViewportUpdate);
    viewerController->setAttribute(Qt::WA_NoBackground, true);
    m_viewList.append(viewerController);
    viewerController->setMinMaxZoom(m_zoomMin, m_zoomMax);
    viewerController->setHorizontalScrollBarPolicy(m_policy);
    viewerController->setVerticalScrollBarPolicy(m_policy);

    if(1 == m_viewCounter)
    {
        connect(viewerController, SIGNAL(signalSetAngle(qreal)), this, SLOT( slotMainViewRotateAngleChanged(qreal)) );
        connect(viewerController, SIGNAL(signalScaleChanged(qreal, int)), this, SLOT( slotMainViewScaleChanged(qreal, int)) );
    }
}


void SceneController::removeView(ViewerController* viewerController)
{
    m_viewList.removeOne(viewerController);
    delete viewerController;
    if(m_viewList.empty())
        m_parallelCursor->hide();
}

void SceneController::centerViewsOn(QPointF scenePos)
{
    m_parallelCursor->show();
    m_parallelCursor->setPos(scenePos);
    
    foreach(ViewerController* viewerController, m_viewList)
        viewerController->centerViewOn(scenePos);
}

void SceneController::abortLoading()
{
    if(m_blockAbortLoading == false)
        emit signal_abortLoading();
}

void SceneController::getTiles(const QList<visualize_system::URL>& list, int currentZ, QRectF viewportArea, QRect tilesOnViewport)
{
    // qDebug() << "---> void SceneController::getTiles, currentZ :" << currentZ << ", tilesOnViewport :" << tilesOnViewport;
    emit signal_getTiles(list, currentZ, tilesOnViewport); // viewportArea);

    if(isGeo())
    {
        m_currentZ = currentZ;
        m_tilesOnViewport = tilesOnViewport;
    }
}

void SceneController::removeTiles(const QStringList& list)
{
    emit signal_removeTiles(list);
}

void SceneController::initPixelScene(QPointF deltaPixFrame, double baseZlevel, int frameId, int baseGeoZlevel, QString path)
{
    m_BaseZlevel = baseZlevel;
    m_DeltaPixFrame = deltaPixFrame;
    m_BaseGeoZlevel = baseGeoZlevel;
    m_Koef = pow(2,baseZlevel-1);
    m_FrameId = frameId;
    m_filePath = path;
    // ScenesHolder::instance()->registerFrame(m_FrameId, this);

    m_zoomMax = baseZlevel + 4; //! ScenesHolder::instance()->getZoomLevelPixelScene() - НЕОБХОДИМО БРАТЬ ПАРАМЕТР ИЗ ВНЕ !!!

    QSize totalSceneSize(m_tileSize.width()*m_tilesCount.width(), m_tileSize.height()*m_tilesCount.width());
    QRectF _sceneRect_(m_DeltaPixFrame.x(), m_DeltaPixFrame.y(), totalSceneSize.width() - m_DeltaPixFrame.x()*2, totalSceneSize.height() - m_DeltaPixFrame.y()*2);
    m_Scene.setSceneRect(_sceneRect_);
    qDebug() << "xxxxxxxxxxxxx +++> SceneController::initPixelScene :" << deltaPixFrame << m_Koef << ", m_zoomMax :" << m_zoomMax << ", sceneRect :" << _sceneRect_;
    foreach (ViewerController* viewerController, m_viewList)
    {
        viewerController->setMinMaxZoom(m_zoomMin, m_zoomMax);
    }
}

//bool SceneController::startSelectingPolygonalArea(QCursor *cursor)
//{
//    if(m_viewList.count())
//    {
//        m_viewList.last()->startSelectingPolygonalArea(cursor);
//        return true;
//    }
//    else
//        return false;
//}

//bool SceneController::getPoint(QCursor *cursor)
//{
//    if(m_viewList.count())
//    {
//        m_viewList.last()->getPoint(cursor);
//        return true;
//    }
//    else
//        return false;
//}

//void SceneController::finish_getPoint()
//{
//    if(m_viewList.count())
//        m_viewList.last()->finish_getPoint();
//}

//void SceneController::finishSelectingPolygonalArea()
//{
//    if(m_viewList.count())
//        m_viewList.last()->finishSelectingPolygonalArea();
//}

QPointF SceneController::geoToScene(const QPointF& coordinateIn)
{
    double X(m_isGeo ? coordinateIn.y() : coordinateIn.x());
    double Y(m_isGeo ? coordinateIn.x() : coordinateIn.y());
    m_sceneDescriptor.convertNativeToScene(X,Y); // m_sceneDescriptor.convertRefSystemToScene(X,Y);
    return QPointF(X,Y);
}

QPointF SceneController::sceneToGeo(const QPointF& point)
{
    double X(point.x()), Y(point.y());
    m_sceneDescriptor.convertSceneToNative(X,Y);
    return m_isGeo ? QPointF(Y, X) : QPointF(X, Y); //! FIXME - НУЖНО ПЕРЕДЕЛАТЬ !!!
}

void SceneController::convertEpsgToScene(int epsgCode, QPointF& point, bool &success)
{
    double X(point.x()), Y(point.y());
    success = m_sceneDescriptor.convertEpsgToScene(epsgCode,X,Y);
    point = QPointF(X,Y);
}

void SceneController::convertSceneToEpsg(int epsgCode, QPointF& point, bool &success)
{
    double X(point.x()), Y(point.y());
    success = m_sceneDescriptor.convertSceneToEpsg(epsgCode,X,Y);
    point = QPointF(X,Y);
}

void SceneController::realGeoToScene(QPointF& point)
{
    if(sceneIsGeo())
    {
        point = geoToScene(QPointF(point.x(), point.y()));
    }
//    else if(getFrameId() != -1)
//    {
//        PFramePtr pFramePtr(RepresentationServer::instance()->getPFrameById(getFrameId()));
//        if(pFramePtr)
//        {
//            GeoCoord geoCoord(point.y(), point.x(), 0);
//            if(pFramePtr->transformToLocal(&geoCoord))
//            {
//                QPointF framePos(geoCoord.x,geoCoord.y);
//                point = geoToScene(framePos);
//            }
//        }
//    }
}

void SceneController::frameToScene(QPointF& point)
{
    point = geoToScene(point);
}

void SceneController::sceneToFrame(QPointF& point)
{
    point = sceneToGeo(point);
}

void SceneController::sceneToRealGeo(QPointF &point)
{
    QPointF wgsPoint(sceneToGeo(point));

    if(sceneIsGeo())
    {
        point = sceneToGeo(point);
        return;
    }

//    if(getFrameId() != -1)
//    {
//        PFramePtr pFramePtr(RepresentationServer::instance()->getPFrameById(getFrameId()));
//        if(pFramePtr)
//        {
//            GeoCoord geoCoord(wgsPoint.x(), wgsPoint.y(), 0);
//            if(pFramePtr->transformToGlobal(&geoCoord))
//                point = QPointF(geoCoord.y,geoCoord.x);
//        }
//    }
}


bool SceneController::frameIsMatching()
{
//    if(getFrameId() != -1)
//    {
//        PFramePtr pFramePtr(RepresentationServer::instance()->getPFrameById(getFrameId()));
//        if(pFramePtr)
//        {
//            const CoordTransformer * coordTransformer = pFramePtr->transformer();
//            const DpfCoordTransformer * transformer = dynamic_cast<const DpfCoordTransformer*>(coordTransformer);
//            if(transformer)
//                if(transformer->isMatched())
//                    return true;
//        }
//    }
    return false;
}

void SceneController::slotCurCoordSystChanged(int curCoordSyst)
{
    m_curCoordSyst = curCoordSyst;
    emit signalShowCoords("", "", "");
}

void SceneController::moveParallelCursorTo(QPointF wgs84grad)
{
//    if(sceneIsGeo())
//    {
////        m_altReloadTimer.stop();

//        m_parallelCursor->show();
//        QPointF scenePos(geoToScene(wgs84grad));
//        m_parallelCursor->setPos(scenePos);
//        foreach(ViewerController* viewerController, m_viewList)
//            viewerController->autoScroll(scenePos);

//        ////// выводим позицию в окне координат
//        int alt(0);
//        bool success = false;//! ScenesHolder::instance()->getAltitudeFromGeoScene(alt, wgs84grad);
//        double H(alt);
//        dpf::geo::GeoPoint geo_src(wgs84grad.x(), wgs84grad.y(), H);
//        dpf::geo::GeoPoint geo_dst;
//        systemConvert.convert(geo_src, dpf::geo::CoordinateSystem::WGS_84, geo_dst, getCoordinateType());
//        QPointF targetSystemPoint(geo_dst.latitude(), geo_dst.longitude());
//        H = geo_dst.altitude();
//        QString strAlt;
//        if(success)
//            strAlt = QString::number(H,'f',0) + QString::fromUtf8("м");

//        QString yStr, xStr;
//        coordToString2(targetSystemPoint.x(), targetSystemPoint.y(), xStr, yStr);
//        emit signalShowCoords(yStr, xStr, strAlt);
//    }
//    else if(getFrameId() != -1)
//    {
//        PFramePtr pFramePtr(RepresentationServer::instance()->getPFrameById(getFrameId()));
//        if(pFramePtr && frameIsMatching())
//        {
//            GeoCoord geoCoord(wgs84grad.y(), wgs84grad.x(), 0);
//            pFramePtr->transformToLocal(&geoCoord);
//            QPointF framePos = QPointF(geoCoord.x,geoCoord.y);
//            QPointF scenePos = geoToScene(framePos);
//            if(m_Scene.sceneRect().contains(scenePos))
//            {
//                m_parallelCursor->show();
//                m_parallelCursor->setPos(scenePos);
//                foreach(ViewerController* viewerController, m_viewList)
//                    viewerController->autoScroll(scenePos);

//                ////// выводим позицию в окне координат
//                if(m_curCoordSyst == 0)
//                    emit signalShowCoords(QString::number(framePos.y(), 'f', 0), QString::number(framePos.x(), 'f', 0), "");
//                else
//                {
//                    double H(0);
//                    dpf::geo::GeoPoint geo_src(wgs84grad.x(), wgs84grad.y(), H);
//                    dpf::geo::GeoPoint geo_dst;
//                    systemConvert.convert(geo_src, dpf::geo::CoordinateSystem::WGS_84, geo_dst, getCoordinateType());
//                    QPointF targetSystemPoint(geo_dst.latitude(), geo_dst.longitude());
//                    H = geo_dst.altitude();

//                    QString yStr, xStr;
//                    coordToString2(targetSystemPoint.x(), targetSystemPoint.y(), xStr, yStr);
//                    emit signalShowCoords(yStr, xStr, "");

//                    //emit signalShowCoords(coordToString(targetSystemPoint.y()), coordToString(targetSystemPoint.x()), "");
//                }
//            }
//            else
//                m_parallelCursor->hide();
//        }
//        else
//            m_parallelCursor->hide();
//    }
}



QPointF SceneController::sceneToGeo2(const QPointF& pixPos)
{
    if(sceneIsGeo())
    {
        QPointF wgs84grad(sceneToGeo(pixPos));
        return wgs84grad;
    }
//    else if(getFrameId() != -1)
//    {
//        QPointF framePos(pixPos);
//        sceneToFrame(framePos);
//        PFramePtr pFramePtr(RepresentationServer::instance()->getPFrameById(getFrameId()));
//        if(pFramePtr && frameIsMatching())
//        {
//            QPointF wgs84grad(framePos);
//            GeoCoord geoCoord(wgs84grad.x(), wgs84grad.y(), 0);
//            pFramePtr->transformToGlobal(&geoCoord);
//            wgs84grad = QPointF(geoCoord.x,geoCoord.y);
//            return wgs84grad;
//        }
//        else
//            return pixPos;
//    }
    else
        return pixPos;
}

void SceneController::slotAltReload()
{
//    m_altReloadTimer.stop();

//    double lat(m_lastMousePosInView.x()), lon(m_lastMousePosInView.y()), alt(0);
//    AltitudeController::SearchResult searchResult = ScenesHolder::instance()->getAltitudeFromGeoScene(lat, lon, alt, m_lastZoomInView);
//    dpf::geo::GeoPoint geo_src(lat, lon, alt);
//    dpf::geo::GeoPoint geo_dst;
//    systemConvert.convert(geo_src, dpf::geo::CoordinateSystem::WGS_84, geo_dst, getCoordinateType());
//    QPointF targetSystemPoint(geo_dst.latitude(), geo_dst.longitude());
//    alt = geo_dst.altitude();
//    QString strAlt;
//    if(searchResult == AltitudeController::Found)
//        strAlt = QString::number(alt,'f',0) + QString::fromUtf8("м");
//    else if(searchResult == AltitudeController::InProgress)
//    {
//        strAlt = QString::fromUtf8("загрузка");
//        m_altReloadTimer.start(1000);
//    }

//    QString yStr, xStr;
//    coordToString2(targetSystemPoint.x(), targetSystemPoint.y(), xStr, yStr);
//    emit signalShowCoords(yStr, xStr, strAlt);
}

void SceneController::moveMouseInView(const QPointF& scenePos, int zLevel)
{
    if(sceneIsGeo())
    {
//        m_altReloadTimer.stop();
        QPointF wgs84grad(sceneToGeo(scenePos));
        if(wgs84grad.x() < -180 || wgs84grad.y() < -180)
        {
            emit signalShowCoords(QString(),QString(),QString());
        }
//        else
//        {
//            double lat(wgs84grad.x()), lon(wgs84grad.y()), alt(0);
////            AltitudeController::SearchResult searchResult = ScenesHolder::instance()->getAltitudeFromGeoScene(lat, lon, alt, zLevel);
//            dpf::geo::GeoPoint geo_src(wgs84grad.x(), wgs84grad.y(), alt);
//            dpf::geo::GeoPoint geo_dst;
//            systemConvert.convert(geo_src, dpf::geo::CoordinateSystem::WGS_84, geo_dst, getCoordinateType());
//            QPointF targetSystemPoint(geo_dst.latitude(), geo_dst.longitude());
//            alt = geo_dst.altitude();
//            QString strAlt;
////            if(searchResult == AltitudeController::Found)
////                strAlt = QString::number(alt,'f',0) + QString::fromUtf8("м");
////            else if(searchResult == AltitudeController::InProgress)
////            {
////                strAlt = QString::fromUtf8("загрузка");
////                m_lastMousePosInView = wgs84grad;
////                m_lastZoomInView = zLevel;
////                m_altReloadTimer.start(1000);
////            }

//            QString yStr, xStr;
//            coordToString2(targetSystemPoint.x(), targetSystemPoint.y(), xStr, yStr);
//            emit signalShowCoords(yStr, xStr, strAlt);

////            qDebug() << yStr << xStr << strAlt;

//            //emit signalShowCoords(QString::number(pixPos.x(),'f',8), QString::number(pixPos.y(),'f',8), strAlt);
//            //emit signalShowCoords(coordToString(targetSystemPoint.y()), coordToString(targetSystemPoint.x()), strAlt);
//        }
    }
    else /*if(getFrameId() != -1)*/
    {
        QPointF framePos(scenePos);
        sceneToFrame(framePos);
        if(m_curCoordSyst == 0)
        {
            emit signalShowCoords(QString::number((int)(framePos.y())), QString::number((int)(framePos.x())), "");
            // qDebug() << "===>" << (int)framePos.x() << (int)framePos.y();
        }
//        else
//        {
//            PFramePtr pFramePtr(RepresentationServer::instance()->getPFrameById(getFrameId()));
//            if(pFramePtr && frameIsMatching())
//            {
//                QPointF wgs84grad(framePos);
//                GeoCoord geoCoord(wgs84grad.x(), wgs84grad.y(), 0); // lat lon
//                pFramePtr->transformToGlobal(&geoCoord);
//                wgs84grad = QPointF(geoCoord.y,geoCoord.x);

//                int alt(0);
//                //                bool success = ScenesHolder::instance()->getAltitudeFromPixelScene(alt, wgs84grad, zLevel+m_BaseGeoZlevel);
//                double H(alt);

//                dpf::geo::GeoPoint geo_src(wgs84grad.x(), wgs84grad.y(), H);
//                dpf::geo::GeoPoint geo_dst;
//                systemConvert.convert(geo_src, dpf::geo::CoordinateSystem::WGS_84, geo_dst, getCoordinateType());
//                QPointF targetSystemPoint(geo_dst.latitude(), geo_dst.longitude());
//                H = geo_dst.altitude();

//                QString strAlt;
//                //                if(success)
//                //                    strAlt = QString::number(H,'f',0) + QString::fromUtf8("м");


//                QString yStr, xStr;
//                coordToString2(targetSystemPoint.x(), targetSystemPoint.y(), xStr, yStr);
//                emit signalShowCoords(yStr, xStr, strAlt);

//                //emit signalShowCoords(coordToString(targetSystemPoint.y()), coordToString(targetSystemPoint.x()), strAlt);
//            }
//            //            else
//            //                qDebug() << "frameIsMatching == false";
//        }
    }
}

void SceneController::coordToString2(double dataX, double dataY, QString & xStr, QString & yStr)
{
//    switch(m_curCoordSyst)
//    {
//    case 1 :
//    case 4 :
//    case 8 :
//    case 11:
//        xStr = QString::fromUtf8("Ш :") + " " + QString::fromStdString(angleConvertion.dec_deg(dataX));
//        yStr = QString::fromUtf8("Д :") + " " + QString::fromStdString(angleConvertion.dec_deg(dataY));
//        break;
//    case 2 :
//    case 5 :
//    case 9 :
//    case 12 :
//        xStr = QString::fromUtf8("Ш :") + " " + QString::fromStdString(angleConvertion.deg_min(dataX));
//        yStr = QString::fromUtf8("Д :") + " " + QString::fromStdString(angleConvertion.deg_min(dataY));
//        break;
//    case 3 :
//    case 6 :
//    case 10 :
//    case 13 :
//        xStr = QString::fromUtf8("Ш :") + " " + QString::fromStdString(angleConvertion.deg_min_sec(dataX));
//        yStr = QString::fromUtf8("Д :") + " " + QString::fromStdString(angleConvertion.deg_min_sec(dataY));
//        break;
//    case 7 :
//    case 14 :
//        xStr = QString::fromUtf8("X :") + " " + QString::number(dataX, 'f', 0);// + tr("m");
//        yStr = QString::fromUtf8("Y :") + " " + QString::number(dataY, 'f', 0);// + tr("m");
//        break;
//    }
}


QString SceneController::coordToString(double data)
{
    QString ret;
//    switch(m_curCoordSyst)
//    {
//    case 1 :
//    case 4 :
//    case 8 :
//    case 11:
//        ret = QString::fromStdString(angleConvertion.dec_deg(data));
//        break;
//    case 2 :
//    case 5 :
//    case 9 :
//    case 12 :
//        ret = QString::fromStdString(angleConvertion.deg_min(data));
//        break;
//    case 3 :
//    case 6 :
//    case 10 :
//    case 13 :
//        ret = QString::fromStdString(angleConvertion.deg_min_sec(data));
//        break;
//    case 7 :
//    case 14 :
//        ret = QString::number(data, 'f', 0);
//        break;
//    }
    return ret;
}

//dpf::geo::CoordinateSystem SceneController::getCoordinateType()
//{
//    switch(m_curCoordSyst)
//    {
//    case 4 :
//    case 5 :
//    case 6 :
//        return dpf::geo::CoordinateSystem::PULKOVO_1942;
//    case 7 :
//        return dpf::geo::CoordinateSystem::GAUSS_KRUGER;
//    case 8 :
//    case 9 :
//    case 10 :
//        return dpf::geo::CoordinateSystem::SK_95;
//    case 11 :
//    case 12 :
//    case 13 :
//        return dpf::geo::CoordinateSystem::PZ_9011;
//    case 14 :
//        return dpf::geo::CoordinateSystem::NORTH_POLAR;
//    }
//    return dpf::geo::CoordinateSystem::WGS_84;
//}

void SceneController::moveParallelCursorInView(const QPointF& point)
{
//    m_parallelCursor->hide();

//    if(sceneIsGeo())
//    {
//        QPointF wgs84grad(sceneToGeo(point));
////        ScenesHolder::instance()->moveParallelCursor(wgs84grad, this);
//    }
//    else if(getFrameId() != -1)
//    {
//        QPointF framePos(point);
//        sceneToFrame(framePos);

//        PFramePtr pFramePtr(RepresentationServer::instance()->getPFrameById(getFrameId()));
//        if(pFramePtr)
//        {
////            ScenesHolder::instance()->emit_signalMovePreview(framePos, pFramePtr->id());

//            if(frameIsMatching())
//            {
//                QPointF wgs84grad(framePos);
//                GeoCoord geoCoord(wgs84grad.x(), wgs84grad.y(), 0);
//                pFramePtr->transformToGlobal(&geoCoord);
//                wgs84grad = QPointF(geoCoord.y,geoCoord.x);
////                ScenesHolder::instance()->moveParallelCursor(wgs84grad, this);
//            }
//        }
//    }
}


double SceneController::deg_rad(double x)const
{
    return x * (M_PI/180.);
}


double SceneController::rad_deg(double x)const
{
    return x * (180./M_PI);
}


void SceneController::mousePressEventInView(QMouseEvent const * const e, QPointF scenePos)
{
    QPointF wgs84 = sceneToGeo(scenePos);
    emit signal_mousePressEvent(e, scenePos, wgs84);
}

//void SceneController::setReperPointsOnMaster(bool on_off, bool onMaster, int masterFrameId)
//{
//    foreach(ViewerController* vc, m_viewList)
//        vc->setReperPointsOnMaster(on_off,onMaster, masterFrameId);
//}

//SceneControl::ReperPoint* SceneController::createReperPoints(QPointF framePos)
//{
//    SceneControl::ReperPoint * rp = new SceneControl::ReperPoint;
//    getScene()->addItem(rp);
//    rp->setPos( geoToScene(framePos) );
//    rp->setZValue(1000000);
//    connect(rp, SIGNAL(signal_sceneToGeo(QPointF&)), this, SLOT(sceneToRealGeo(QPointF&)));
//    connect(rp, SIGNAL(signal_sceneToFrame(QPointF&)), this, SLOT(sceneToFrame(QPointF&)));
//    connect(rp, SIGNAL(signal_geoToScene(QPointF&)), this, SLOT(realGeoToScene(QPointF&)));
//    connect(rp, SIGNAL(signal_frameToScene(QPointF&)), this, SLOT(frameToScene(QPointF&)));
//    rp->setSelected(true);
//    return rp;
//}

void SceneController::setEditMode(bool on_off, QPixmap pm)
{
    foreach(ViewerController* vc, m_viewList)
        vc->setEditMode(on_off, pm);
}

void SceneController::setSceneActive(bool on_off)
{
    m_Scene.setSceneActive(on_off);
}

//void SceneController::setReperPointsState(bool on_off)
//{
//    foreach(ViewerController* vc, m_viewList)
//        vc->setReperPointsState(on_off);
//}


void SceneController::getCoordsForMark(quint64 objBrowserId, quint64 markId, QString iconPath)
{
    if(m_viewList.isEmpty() == false)
    {
        QVariantMap varMap;
        varMap.insert("modeOn", true);
        varMap.insert("browserId", objBrowserId);
        varMap.insert("markId", markId);
        varMap.insert("iconPath", iconPath);

        m_viewList.first()->slotDetectViewerMode("GetPointForMark", varMap);
    }
}

QRectF SceneController::getViewportRect()
{
    QRectF nativeRect;
    if(m_viewList.isEmpty() == false)
    {
        QPolygonF geoPolygon;
        foreach(QPointF p, QPolygonF(m_viewList.first()->getViewportRect()))
        {
            double x(p.x()), y(p.y());
            m_sceneDescriptor.convertSceneToNative(x,y);
            geoPolygon.append(QPointF(x,y));
        }
        nativeRect = geoPolygon.boundingRect();
    }
    return nativeRect;
}

void SceneController::emit_signalUserViewControlAction(UserAction userAction)
{
    emit signalUserViewControlAction(userAction);
}

void SceneController::getViewportRect(QRectF & r)
{
    if(m_viewList.isEmpty() == false)
    {
        r = m_viewList.first()->getViewportRect();
    }
}

void SceneController::getSceneParamsForSave(QPointF& scenePos, int& zoomLevel)
{
    if(m_viewList.isEmpty() == false)
    {
        scenePos = m_viewList.first()->getViewportRect().center();
        QPointF p =sceneToGeo(scenePos);
        scenePos = p;
        zoomLevel = m_viewList.first()->getCurrentZoomeLevel();
    }
}

void SceneController::setSceneParams(QPointF scenePos, int zoomLevel)
{
    if(m_viewList.isEmpty() == false)
    {
        m_viewList.first()->changeZoom(zoomLevel);
        m_viewList.first()->centerOn(scenePos);
    }
}

void SceneController::setSceneRect(QRectF sceneRect)
{
    m_DeltaPixFrame = sceneRect.topLeft();
    m_Scene.setSceneRect(sceneRect);
}

void SceneController::setZoomLevels(uint minZlevel, uint maxZlevel, uint baseZlevel)
{
    m_BaseZlevel = baseZlevel;
    m_zoomMax = maxZlevel;
    m_zoomMin = minZlevel;
    foreach (ViewerController* viewerController, m_viewList)
        viewerController->setMinMaxZoom(m_zoomMin, m_zoomMax);
}

void SceneController::setAsincLoadingOnGeoMap(bool on_off)
{
    m_TileSceneAsinc.setAsincMode(on_off);
}

void SceneController::initView(QColor backGroundColor, Qt::ScrollBarPolicy policy)
{
    m_TileScene.setBackgroundBrush(backGroundColor);
    m_Scene.setBackgroundBrush(backGroundColor);
    m_policy = policy;
    foreach(ViewerController* vc, m_viewList)
    {
        vc->setHorizontalScrollBarPolicy(policy);
        vc->setVerticalScrollBarPolicy(policy);
    }
}

void SceneController::slotConvertSceneCoordToGlobal(QPointF & pos)
{
    foreach(ViewerController* vc, m_viewList)
    {
        QPoint viewPos = vc->mapFromScene(pos);
        QPoint globalPos = vc->mapToGlobal(viewPos);
        pos = QPointF(globalPos);
        break;
    }
}

void SceneController::slotSetActiveForScene(bool on_off)
{
    m_Scene.setSceneActive(on_off);
}






