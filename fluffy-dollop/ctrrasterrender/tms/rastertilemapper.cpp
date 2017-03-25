#include "rastertilemapper.h"
//#include <ctrcore/processing/processingpool.h>
#include <ctrcore/provider/timagecache.h>
#include <ctrcore/visual/visualizermanager.h>
#include <ctrcore/visual/datainterface.h>
//#include <gdal/ogr_spatialref.h>
#include <QImage>
#include <QPixmap>
#include <QUrl>
#include <QFileInfo>
#include <QPainter>

//#include <dpf/io/base/appdebugwidget.h>

using namespace visualize_system;

RasterTileMapper::RasterTileMapper(uint visualizerId, uint providerId, const SceneDescriptor & _sceneDescriptor)
    : TileMapper(visualizerId, providerId, _sceneDescriptor)
    , _rasterMapperCache(QUrl(PROVIDER_URL).toLocalFile())
    , DELTA_Z_LEVEL_FOR_INSERT_TO_CACHE(5)
    , DOWN_DELTA_Z_LEVEL(2)
    , MAX_Z(18)
{
    connect(raster_provider.data(), SIGNAL(dataReady(int)), this, SLOT(slotRasterDataReady(int)));

    //    QRectF R = raster_provider->spatialRect();
    //    qDebug() << "---> void RasterTileMapper::RasterTileMapper(), raster_provider :" << R.topLeft() << R.topRight() << R.bottomRight() << R.bottomLeft();

    rasterPixBRect = raster_provider->boundingRect();
    if(rasterPixBRect != QRect()) // вообще это для пиксельной сцены ?!
    {
        QSize baseSceneSize(sceneDescriptor.tileSize() * 2);
        forever
        {
            QRect currRect(0, 0, baseSceneSize.width()*pow(2,baseZlevel-1), baseSceneSize.height()*pow(2,baseZlevel-1));
            if(currRect.contains(rasterPixBRect))
            {
                double coef(1. / pow(2,(baseZlevel - 1)) );
                double W(rasterPixBRect.width()*coef);
                double H(rasterPixBRect.height()*coef);

                shiftImageOnScene = QPointF((baseSceneSize.width() - W) / 2, (baseSceneSize.height() - H) / 2);
                rasterSceneRect = QRectF(shiftImageOnScene, QSizeF(W,H));
                double _W = rasterPixBRect.width();
                double _H = rasterPixBRect.height();
                rasterMapperCoef = (_W/rasterSceneRect.width() + _H/rasterSceneRect.height()) / 2.;

                break;
            }
            ++baseZlevel;
        }
    }

    //--------------------------
    connect(raster_provider.data(), SIGNAL(signalProviderChanged()), this, SLOT(slotProviderChanged()));

//    if(sceneDescriptor.isGeoScene())
//    {
//        QPolygonF spatialPolygon;
//        dpf::PolygonF pixelPoly(raster_provider->geoImageObjectPtr()->image_rect());
//        dpf::PolygonF geoPoly = raster_provider->geoImageObjectPtr()->pixel_to_geo(pixelPoly);
//        for(const dpf::PointF& p : geoPoly)
//            spatialPolygon.push_back(QPointF(p.x(), p.y()));

////        qDebug() << "--------------";
//        foreach(QPointF p, spatialPolygon)
//        {
//            double x(p.x()), y(p.y());
////            qDebug() << "MERC x" << QString::number(x, 'f', 10) << "y" << QString::number(y, 'f', 10);
//            sceneDescriptor.convertNativeToScene(x,y); // sceneDescriptor.convertRefSystemToScene(x,y);
////            qDebug() << "SCENE x" << x << "y" << y;
//            _boundingScenePolygone.append(QPointF(x,y));

////            sceneDescriptor.convertSceneToNative(x,y);
////            qDebug() << "WGS x" << QString::number(x, 'f', 10) << "y" << QString::number(y, 'f', 10);
//        }
////        qDebug() << "--------------";

//        _boundingSceneRect = _boundingScenePolygone.boundingRect();
//        for(MIN_Z_LEVEL=1; MIN_Z_LEVEL<MAX_Z; ++MIN_Z_LEVEL)
//        {
//            double POW(pow(2,MIN_Z_LEVEL-1));
//            double W(_boundingSceneRect.width()*POW);
//            double H(_boundingSceneRect.height()*POW);
////            qDebug() << "Z:" << MIN_Z_LEVEL << ", coef:" << POW << ", rasterSpatialSize:" << QString::number(W, 'f', 10) << QString::number(H, 'f', 10) << ", rasterPixBRect:" << raster_provider->boundingRect();
//            if( W >= sceneDescriptor.tileSize().width() || H >= sceneDescriptor.tileSize().height())
//            {
//                MIN_Z_LEVEL -= DOWN_DELTA_Z_LEVEL;
//                break;
//            }
//        }
//        if(MIN_Z_LEVEL == MAX_Z)
//            MIN_Z_LEVEL -= DOWN_DELTA_Z_LEVEL;

//        qDebug() << "result MIN_Z_LEVEL:" << MIN_Z_LEVEL;
//    }
}

RasterTileMapper::~RasterTileMapper()
{
    TImageCache::instance()->clearProviderData( QString::number( ((qulonglong)(this)) ) );
}


QPolygonF RasterTileMapper::getBoundingPolygone()
{
    QPolygonF pixelPolygon;
//    if(sceneDescriptor.isGeoScene())
//    {
//        QPolygonF spatialPolygon;
//        dpf::PolygonF pixelPoly(raster_provider->geoImageObjectPtr()->image_rect());
//        dpf::PolygonF geoPoly = raster_provider->geoImageObjectPtr()->pixel_to_geo(pixelPoly);
//        for(const dpf::PointF& p : geoPoly)
//            spatialPolygon.push_back(QPointF(p.x(), p.y()));

//        foreach(QPointF p, spatialPolygon)
//        {
//            double x(p.x()), y(p.y());
//            sceneDescriptor.convertNativeToScene(x,y); // sceneDescriptor.convertRefSystemToScene(x,y);
//            pixelPolygon.append(QPointF(x,y));
//        }
//    }
//    else
//    {
////        qDebug() << "---> rasterPixBRect :" << rasterPixBRect;
//        pixelPolygon << rasterSceneRect.topLeft() << rasterSceneRect.topRight() << rasterSceneRect.bottomRight() << rasterSceneRect.bottomLeft();
//    }

    return pixelPolygon;
}

void RasterTileMapper::loadTiles(const QList<visualize_system::URL> list)
{
    foreach (visualize_system::URL url, list)
    {
        if(_mapperViewMode == MapperViewMode::SHOW_ONLY_BORDER)
        {
            QPixmap pm;
            emit signal_paintTile(PROVIDER_ID, url.returnId, pm, ORIGINAL);
            continue;
        }

        image_types::TImage* tImg = new image_types::TImage;
        QString cacheKey(QString::number((qulonglong)this) + "_" + QString::number(url.x) + "_" + QString::number(url.y) + "_" + QString::number(url.z));
        QString mapperCacheKey(QString::number(url.x) + "_" + QString::number(url.y) + "_" + QString::number(url.z));

        if(_hasProcessing)
        {
            QString processingCacheKey(QString::number((qulonglong)this) + "_" + QString::number(url.x) + "_" + QString::number(url.y) + "_" + QString::number(url.z) + "_");
//            visualize_system::DataInterface* dataInt = visualize_system::VisualizerManager::instance()->getDataInterface(VISUALIZER_ID);
//            if(dataInt)
//                processingCacheKey.append(dataInt->getProviderProcessingCacheKey(PROVIDER_ID));

            if(TImageCache::instance()->find(processingCacheKey, tImg))
            {
                QImage _img(QSize(tImg->w, tImg->h), QImage::Format_ARGB32);
                memcpy(_img.bits(), tImg->prt, tImg->depth / 8 * tImg->w * tImg->h);
                QPixmap pm(QPixmap::fromImage(_img));

//                if(pm.isNull())
//                    qDebug() << "!!! 111 pm.isNull() !!!";

//                qDebug() << "3 ===> RasterTileMapper QPixmap()" << url.returnId;
                emit signal_paintTile(PROVIDER_ID, url.returnId, pm, ORIGINAL);

                delete []tImg->prt;
                tImg->prt = 0;
                delete tImg;

                continue;
            }
        }
//        if(m_THisto.title.isEmpty() == false)
//        {
//            QString histoCacheKey(QString::number((qulonglong)this) + "_" + url.returnId + "_" + m_THisto.getCacheKey());
//            if(TImageCache::instance()->find(histoCacheKey, tImg))
//            {
//                QImage _img(QSize(tImg->w, tImg->h), QImage::Format_ARGB32);
//                memcpy(_img.bits(), tImg->prt, tImg->depth / 8 * tImg->w * tImg->h);
//                QPixmap pm(QPixmap::fromImage(_img));

////                if(pm.isNull())
////                    qDebug() << "!!! 111 pm.isNull() !!!";

////                qDebug() << "3 ===> RasterTileMapper QPixmap()" << url.returnId;
//                emit signal_paintTile(PROVIDER_ID, url.returnId, pm, ORIGINAL);

//                delete []tImg->prt;
//                tImg->prt = 0;
//                delete tImg;

//                continue;
//            }
//        }

        if(TImageCache::instance()->find(cacheKey, tImg))
        {
//            qDebug() << "RasterTileMapper::loadTiles, IN CACHE, prov :" << ((u_int64_t)(tms_provider));

            if(_hasProcessing)
            {
                ++tasksInProcessingsPool;

                QString cacheKey(QString::number((qulonglong)this) + "_" + QString::number(url.x) + "_" + QString::number(url.y) + "_" + QString::number(url.z) + "_");
//                visualize_system::DataInterface* dataInt = visualize_system::VisualizerManager::instance()->getDataInterface(VISUALIZER_ID);
//                if(dataInt )
//                    cacheKey.append(dataInt->getProviderProcessingCacheKey(PROVIDER_ID));
//                processing_system::ProcessingPool::instance()->setTask(processingPoolSenderId, tImg, url.returnId, PROVIDER_ID, VISUALIZER_ID, cacheKey);
            }
//            if(m_THisto.title.isEmpty() == false)
//            {
//                PoolTask poolTask(algorithmsPoolKey, tImg, url.returnId);
//                ++tasksInAlgorithmsPool;
//                AlgorithmsPool::instance()->setTask(poolTask);
//            }
            else
            {
                QImage _img(QSize(tImg->w, tImg->h), QImage::Format_ARGB32);
                memcpy(_img.bits(), tImg->prt, tImg->depth / 8 * tImg->w * tImg->h);
                QPixmap pm(QPixmap::fromImage(_img));

//                if(pm.isNull())
//                    qDebug() << "!!! 222 pm.isNull() !!!";

//                qDebug() << "4 ===> RasterTileMapper QPixmap()" << url.returnId;
                emit signal_paintTile(PROVIDER_ID, url.returnId, pm, ORIGINAL);

                delete []tImg->prt;
                tImg->prt = 0;
                delete tImg;
            }

            continue;
        }

        if(_rasterMapperCache.find(mapperCacheKey, tImg))
        {
//            qDebug() << "~~~> loaded from rasterMapperCache, cacheKey:" << cacheKey << ", PROVIDER_URL:" << PROVIDER_URL;
            TImageCache::instance()->insert(cacheKey, tImg); // сохраняем тайл в кешер

            if(_hasProcessing)
            {
//                qDebug() << "RasterTileMapper::loadTiles, hasProcessing !!!" << url.returnId;
            }
//            if(m_THisto.title.isEmpty() == false)
//            {
//                PoolTask poolTask(processingPoolSenderId, tImg, url.returnId);
//                ++tasksInProcessingsPool;
//                AlgorithmsPool::instance()->setTask(poolTask);
//            }
            else
            {
                QImage _img(QSize(tImg->w, tImg->h), QImage::Format_ARGB32);
                memcpy(_img.bits(), tImg->prt, tImg->depth / 8 * tImg->w * tImg->h);
                QPixmap pm(QPixmap::fromImage(_img));

//                if(pm.isNull())
//                    qDebug() << "!!! 222 pm.isNull() !!!";

//                qDebug() << "5 ===> RasterTileMapper QPixmap()" << url.returnId;
                emit signal_paintTile(PROVIDER_ID, url.returnId, pm, ORIGINAL);

                delete []tImg->prt;
                tImg->prt = 0;
                delete tImg;
            }

            continue;
        }

        if(loadingTaskKey.contains(url.returnId))
        {
            // сейчас идет обновление вьюпорта, а тайл по этому кэш-ключ итак уже поставлен на загрузку !
//            qDebug() << "&&& cacheKey in loadingTaskKey :" << cacheKey;


//            qDebug() << QFileInfo(QUrl(PROVIDER_URL).toLocalFile()).fileName() << "MAPPER, loadingTaskKey.contains() :" << url.returnId << ", scaleTile";
            scaleTile(url);
            continue;
        }

        {
            if(sceneDescriptor.isGeoScene() && url.z < MIN_Z_LEVEL)
            {
                delete []tImg->prt;
                tImg->prt = 0;
                delete tImg;

                QPixmap pm;
//                qDebug() << "6 ===> RasterTileMapper QPixmap()" << url.returnId;
                emit signal_paintTile(PROVIDER_ID, url.returnId, pm, ORIGINAL);

//                qDebug() << "mapper, MIN_Z_LEVEL" << MIN_Z_LEVEL << "url.z" << url.z;

                continue;
            }

            QPoint shiftImageInTile;
//            qDebug() << "getTile" << url.x << url.y << url.z;
            int ret = loadTile(tImg, url.x, url.y, url.z, shiftImageInTile);

//            qDebug() << "RasterTileMapper::loadTiles, ret :" << ret << ", prov :" << ((u_int64_t)(tms_provider));

            if(ret >= 0)
            {
                if(ret == 0) // картинка уже загружена в TImage (синхронно)
                {
                    if(_hasProcessing)
                    {
//                        qDebug() << "RasterTileMapper::loadTiles, hasProcessing !!!" << url.returnId;
                    }
//                    if(m_THisto.title.isEmpty() == false)
//                    {
//                        PoolTask poolTask(processingPoolSenderId, tImg, url.returnId);
//                        ++tasksInProcessingsPool;
//                        AlgorithmsPool::instance()->setTask(poolTask);
//                    }
                    else
                    {
                        TImageCache::instance()->insert(cacheKey, tImg); // сохраняем тайл в кешер
                        QImage _img(QSize(tImg->w, tImg->h), QImage::Format_ARGB32);
                        memcpy(_img.bits(), tImg->prt, tImg->depth / 8 * tImg->w * tImg->h);
                        QPixmap pm(QPixmap::fromImage(_img));
//                        if(pm.isNull())
//                            qDebug() << "!!! 333 pm.isNull() !!!";

//                        qDebug() << "7 ===> RasterTileMapper QPixmap()" << url.returnId;
                        emit signal_paintTile(PROVIDER_ID, url.returnId, pm, ORIGINAL);

                        delete []tImg->prt;
                        tImg->prt = 0;
                        delete tImg;
                    }
                }
                else // картинка загружается асинхронно
                {
                    m_Map.insert(ret, Temp(tImg, url.returnId, url.x, url.y, url.z, cacheKey, shiftImageInTile));
//                    qDebug() << QFileInfo(QUrl(PROVIDER_URL).toLocalFile()).fileName() << "MAPPER, loadingTaskKey.insert() :" << url.returnId << ", scaleTile";
                    loadingTaskKey.insert(url.returnId);

                    scaleTile(url);
                }
            }
            else // картинки с заданными x,y,z - не существует !
            {
                delete []tImg->prt;
                tImg->prt = 0;
                delete tImg;

                QPixmap pm;
//                qDebug() << "8 ===> RasterTileMapper QPixmap()" << url.returnId;
                emit signal_paintTile(PROVIDER_ID, url.returnId, pm, ORIGINAL);
            }
        }
    }

    if(m_Map.isEmpty() && tasksInProcessingsPool <= 0)
        emit signal_dataLoaded();
}


void RasterTileMapper::scaleTile(visualize_system::URL url)
{
    int scaleFactor(1);
    int x(url.x), y(url.y), z(url.z);

    while(z >= MIN_Z_LEVEL)
    {
        QPoint underTile = tileUnderCurrent(x, y, z);

        //qDebug() << "CURRENT :" << x << y << z << ", UNDER :" << underTile.x() << underTile.y();

        x = underTile.x();
        y = underTile.y();
        z -= 1;
        scaleFactor *= 2;
        QString cacheKey(QString::number((qulonglong)this) + "_" + QString::number(x) + "_" + QString::number(y) + "_" + QString::number(z));
        QString mapperCacheKey(QString::number(x) + "_" + QString::number(y) + "_" + QString::number(z));
        image_types::TImage* _tImg = new image_types::TImage;
        bool tileFound(false);
        if(TImageCache::instance()->find(cacheKey, _tImg))
        {
            tileFound = true;
        }
        else if(_rasterMapperCache.find(mapperCacheKey, _tImg))
        {
            tileFound = true;
            TImageCache::instance()->insert(cacheKey, _tImg); // сохраняем тайл в кешер
        }

        if(tileFound)
        {
            int deltaZ(url.z - z);
            const int K(pow(2, deltaZ > 8 ? 8 : deltaZ));

            QImage _img( ((uchar*)(_tImg->prt)), _tImg->w, _tImg->h, QImage::Format_ARGB32);

            int X = url.x - x * K;
            int Y = url.y - y * K;
            int W = sceneDescriptor.tileSize().width() / K;
            int H = sceneDescriptor.tileSize().height() / K;
            QImage sourceImage = _img.copy(X * W, Y * H, W, H);

            sourceImage = sourceImage.scaled(sourceImage.width() * K, sourceImage.height() * K);

            QPixmap pm(QPixmap::fromImage(sourceImage));
//            qDebug() << "9 ===> RasterTileMapper QPixmap()" << url.returnId;
            emit signal_paintTile(PROVIDER_ID, url.returnId, pm, SCALED);

            delete []_tImg->prt;
            _tImg->prt = 0;
            delete _tImg;

            break;
        }
    }
}

int RasterTileMapper::loadTile(image_types::TImage *img, int x, int y, int z, QPoint & shiftImageInTile)
{
    int provider_task = -1;

    if(sceneDescriptor.isGeoScene())
    {
//        OGRSpatialReference * inRef = new OGRSpatialReference;
//        OGRErr errIn = inRef->importFromEPSG(4326);

//        OGRSpatialReference * outRef = new OGRSpatialReference;
//        // int code = sceneDescriptor.refEpsgSRS();
//        OGRErr errOut = outRef->importFromEPSG(sceneDescriptor.refEpsgSRS());

        QPolygonF refPol;
//        if(errIn == OGRERR_NONE && errOut == OGRERR_NONE)
//        {
//            QPolygonF wgsBboundingPol(raster_provider->spatialRect());
//            OGRCoordinateTransformation * trans = OGRCreateCoordinateTransformation(inRef, outRef);
//            if(trans)
//            {
//                foreach(QPointF p, wgsBboundingPol)
//                {
//                    double x(p.x()), y(p.y());
//                    trans->Transform(1,&x,&y);
//                    refPol.append(QPointF(x,y));
//                }
//                OGRCoordinateTransformation::DestroyCT(trans);
//            }
//        }
//        OGRSpatialReference::DestroySpatialReference(inRef);
//        OGRSpatialReference::DestroySpatialReference(outRef);

        QRectF geoBboundingRect = refPol.boundingRect();
        if(geoBboundingRect != QRectF())
        {
            QRectF tileExtend(getTileExtend(x,y,z));
            bool RES = geoBboundingRect.intersects(tileExtend);
//            if(RES)
//            {
//                qDebug() << "RES :" << RES << "x" << x << "y" << y << "z" << z;
//                qDebug() << "RES :" << RES
//                         << "x" << x << "y" << y << "z" << z
//                         << "tileExtend :" << QString::number( tileExtend.x(), 'f', 0)
//                         << QString::number( tileExtend.y(), 'f', 0)
//                         << QString::number( tileExtend.width(), 'f', 0)
//                         << QString::number( tileExtend.height(), 'f', 0)
//                         << "geoBboundingRect :" << QString::number( geoBboundingRect.x(), 'f', 0)
//                         << QString::number( geoBboundingRect.y(), 'f', 0)
//                         << QString::number( geoBboundingRect.width(), 'f', 0)
//                         << QString::number( geoBboundingRect.height(), 'f', 0);
//            }
            if(RES)
            {
                provider_task = raster_provider->readSpatialData(*img, tileExtend, QRect(0,0,sceneDescriptor.tileSize().width(),sceneDescriptor.tileSize().height()));
            }
        }
    }
    else
    {
        double POW(pow(2,z-1));
        double tileWidthOnView(sceneDescriptor.tileSize().width() / POW);
        double tileHeightOnView(sceneDescriptor.tileSize().height() / POW);
        QRectF tileSceneRect(tileWidthOnView*x, tileHeightOnView*y, tileWidthOnView, tileHeightOnView);
        QRectF intersectSceneRect(rasterSceneRect.intersected(tileSceneRect));
        QRect intersectPixRect((intersectSceneRect.x() - shiftImageOnScene.x()) * rasterMapperCoef,
                               (intersectSceneRect.y() - shiftImageOnScene.y()) * rasterMapperCoef,
                               intersectSceneRect.width() * rasterMapperCoef,
                               intersectSceneRect.height() * rasterMapperCoef);

        if(intersectSceneRect != QRectF() && intersectPixRect.width() > 0 && intersectPixRect.height() > 0)
        {
            if(intersectSceneRect.size() == tileSceneRect.size())
            {
                provider_task = raster_provider->readData(*img, intersectPixRect, sceneDescriptor.tileSize());
            }
            else
            {
                QPointF shiftSceneCoord(intersectSceneRect.topLeft() - tileSceneRect.topLeft());
                shiftImageInTile = QPoint(shiftSceneCoord.x()*POW, shiftSceneCoord.y()*POW);
                if(shiftImageInTile == QPoint())
                    shiftImageInTile = QPoint(-1,-1);
                QSize intersectPixSize(ceil((sceneDescriptor.tileSize().width()*intersectSceneRect.width())/tileSceneRect.width()),
                                       ceil((sceneDescriptor.tileSize().height()*intersectSceneRect.height())/tileSceneRect.height()));

                //                    qDebug() << "intersectPixRect" << intersectPixRect << "intersectPixSize" << intersectPixSize;
                provider_task = raster_provider->readData(*img, intersectPixRect, intersectPixSize);
                if(provider_task == 0) // загрузили синхронно изображение ЧАСТИЧНО перекрывающее тайл, необходимо нормализовать его до размера тайла !
                {
                    image_types::TImage * dest = normalizeTImage(img, shiftImageInTile);

                    delete []img->prt;
                    img->prt = 0;
                    delete img;
                    img = dest;
                }
            }
        }
    }

    return provider_task;
}

void RasterTileMapper::slotRasterDataReady(int task)
{
    auto it(m_Map.find(task)); // ишем задачу среди обычных задач поставленных извне
    if(it != m_Map.end())
    {
        Temp temp(it.value());

        bool success = loadingTaskKey.remove(temp.task);
//        qDebug() << QFileInfo(QUrl(PROVIDER_URL).toLocalFile()).fileName() << "MAPPER, slotRasterDataReady, loadingTaskKey.remove() 111 :" << temp.task  << ", success :" << success;

        image_types::TImage* tImg(temp.tImg);

//        qDebug() << "DataReady, PROVIDER_URL :" << PROVIDER_URL << ", URL :" << temp.cacheKey;

        if(temp.shiftImageInTile != QPoint())
        {
            image_types::TImage * dest = normalizeTImage(tImg, temp.shiftImageInTile);
            delete []tImg->prt;
            tImg->prt = 0;
            delete tImg;
            tImg = dest;
        }

        TImageCache::instance()->insert(temp.cacheKey, tImg); // сохраняем тайл в глобальный кеш в памяти
        if(temp.z < (MIN_Z_LEVEL + DELTA_Z_LEVEL_FOR_INSERT_TO_CACHE))
        {
            QString mapperCacheKey(QString::number(temp.x) + "_" + QString::number(temp.y) + "_" + QString::number(temp.z));
            _rasterMapperCache.insert(mapperCacheKey, tImg); // сохраняем тайл в локальный кеш на диске
        }

        if(_hasProcessing)
        {
            ++tasksInProcessingsPool;
            QString cacheKey(QString::number((qulonglong)this) + "_" + QString::number(temp.x) + "_" + QString::number(temp.y) + "_" + QString::number(temp.z) + "_");
//            visualize_system::DataInterface* dataInt = visualize_system::VisualizerManager::instance()->getDataInterface(VISUALIZER_ID);
//            if(dataInt )
//                cacheKey.append(dataInt->getProviderProcessingCacheKey(PROVIDER_ID));
//            processing_system::ProcessingPool::instance()->setTask(processingPoolSenderId, tImg, temp.task, PROVIDER_ID, VISUALIZER_ID, cacheKey);
        }
//        if(m_THisto.title.isEmpty() == false)
//        {
//            PoolTask poolTask(algorithmsPoolKey, tImg, temp.task);
//            ++tasksInAlgorithmsPool;
//            AlgorithmsPool::instance()->setTask(poolTask);
//        }
        else
        {
            QImage _img(((uchar*)(tImg->prt)), tImg->w, tImg->h, QImage::Format_ARGB32);
            QPixmap pm(QPixmap::fromImage(_img));

//            qDebug() << "10 ===> slotRasterDataReady, RasterTileMapper QPixmap()" << temp.task;

            emit signal_paintTile(PROVIDER_ID, temp.task, pm, ORIGINAL);

            delete []tImg->prt;
            tImg->prt = 0;
            delete tImg;
        }

        m_Map.erase(it);
    }
    else
    {
        auto baseIt(_loadingBaseTImageMap.find(task)); // ишем задачу среди загрузок минимального уровня
        if(baseIt != _loadingBaseTImageMap.end())
        {
            Temp temp(baseIt.value());
            bool success = loadingTaskKey.remove(temp.task);
//            qDebug() << QFileInfo(QUrl(PROVIDER_URL).toLocalFile()).fileName() << "MAPPER, slotRasterDataReady, loadingTaskKey.remove() 222 :" << temp.task  << ", success :" << success;
            image_types::TImage* tImg(temp.tImg);

            TImageCache::instance()->insert(temp.cacheKey, tImg); // сохраняем тайл в кешер
            QString mapperCacheKey(QString::number(temp.x) + "_" + QString::number(temp.y) + "_" + QString::number(temp.z));
            _rasterMapperCache.insert(mapperCacheKey, tImg);

            TImageCache::instance()->insert(temp.cacheKey, tImg); // сохраняем тайл в кешер

            delete []tImg->prt;
            tImg->prt = 0;
            delete tImg;

            _loadingBaseTImageMap.erase(baseIt);
        }
    }

    if(m_Map.isEmpty() && _loadingBaseTImageMap.isEmpty() && tasksInProcessingsPool <= 0)
        emit signal_dataLoaded();
}

bool RasterTileMapper::cacheLoaded()
{
    return _loadingBaseTImageMap.isEmpty();
}

bool RasterTileMapper::dataLoaded()
{
//    qDebug() << "RasterTileMapper::dataLoaded(), m_Map.size() :" << m_Map.size() << ", tasksInProcessingsPool :" << tasksInProcessingsPool;
    return m_Map.isEmpty() && tasksInProcessingsPool <= 0;
}

void RasterTileMapper::abort(QString taskKey)
{
//    qDebug() << "!!! RasterTileMapper::abort() taskKey :" << taskKey;

    for(auto it = m_Map.begin(); it != m_Map.end(); ++it)
        if(it.value().task == taskKey)
        {
            raster_provider->abort(it.key());
//            qDebug() << "RasterTileMapper::abort taskKey :" << taskKey;
            break;
        }
    bool success = loadingTaskKey.remove(taskKey);
//    qDebug() << QFileInfo(QUrl(PROVIDER_URL).toLocalFile()).fileName() << "MAPPER, abort taskKey, loadingTaskKey.remove() :" << taskKey << ", success :" << success;
}

void RasterTileMapper::abort()
{
//    qDebug() << "--- RasterTileMapper::abort() ---";

    tasksInProcessingsPool = 0;

    if(_hasProcessing)
    {
//        processing_system::ProcessingPool::instance()->removeTasks(processingPoolSenderId);
    }
    else
        for(auto it = m_Map.begin(); it != m_Map.end(); ++it)
        {
            raster_provider->abort(it.key());

            Temp temp = it.value();
            image_types::TImage* tImg(temp.tImg);
            delete []tImg->prt;
            tImg->prt = 0;
            delete tImg;
        }

    m_Map.clear();
    loadingTaskKey.clear();
    tasksInProcessingsPool = 0;
}

void RasterTileMapper::loadCache(const QString & path)
{
     _rasterMapperCache.setSavePath(path);

    double POW(pow(2,MIN_Z_LEVEL-1));
    double tileWOnView(sceneDescriptor.tileSize().width() / POW);
    double tileHOnView(sceneDescriptor.tileSize().height() / POW);
    QPointF centerRasterOnScene = _boundingSceneRect.center();
    // ищем тайл в который вписан центр баундингректа растра
    QPoint tilePos( centerRasterOnScene.x() / tileWOnView, centerRasterOnScene.y() / tileHOnView );

//    qDebug() << "===> tilePos" << tilePos << PROVIDER_URL;
    QPoint shiftImageInTile;
    for(int x(tilePos.x()-1); x <= tilePos.x()+1; ++x)
        for(int y(tilePos.y()-1); y <= tilePos.y()+1; ++y)
        {
            QPolygonF tileScenePolygon(tileSceneRect(x,y,MIN_Z_LEVEL));
            QPolygonF interPol = _boundingScenePolygone.intersected(tileScenePolygon);
            if(interPol.isEmpty() == false)
            {
                QString cacheKey(QString::number((qulonglong)this) + "_" + QString::number(x) + "_" + QString::number(y) + "_" + QString::number(MIN_Z_LEVEL));
                QString mapperCacheKey(QString::number(x) + "_" + QString::number(y) + "_" + QString::number(MIN_Z_LEVEL));
                image_types::TImage *tImg = new image_types::TImage;
                if(_rasterMapperCache.find(mapperCacheKey, tImg))
                {
//                    qDebug() << "+++ intersected cacheKey :" << mapperCacheKey << ", loaded from cache";
                    TImageCache::instance()->insert(cacheKey, tImg);

                    delete []tImg->prt;
                    tImg->prt = 0;
                    delete tImg;
                }
                else
                {
//                    qDebug() << "--- intersected cacheKey :" << mapperCacheKey << ", set on loading";
                    int taskId = loadTile(tImg, x, y, MIN_Z_LEVEL, shiftImageInTile);

                    _loadingBaseTImageMap.insert(taskId, Temp(tImg,
                                                              QString(QString::number(MIN_Z_LEVEL) + " " + QString::number(x) + " " + QString::number(y)),
                                                              x,
                                                              y,
                                                              MIN_Z_LEVEL,
                                                              cacheKey,
                                                              shiftImageInTile));
                }
            }
        }
//    qDebug() << "-------------";
}

QPoint RasterTileMapper::tileUnderCurrent(int x, int y, int z)
{
    double POW(pow(2,z-1));
    double currentTileW(sceneDescriptor.tileSize().width() / POW);
    double currentTileH(sceneDescriptor.tileSize().height() / POW);
    double sceneCenterCurrentTileX = currentTileW * x + currentTileW/2.;
    double sceneCenterCurrentTileY = currentTileH * y + currentTileH/2.;

    double underTileW(currentTileW * 2.);
    double undertTileH(currentTileH * 2.);
    return QPoint( sceneCenterCurrentTileX / underTileW, sceneCenterCurrentTileY / undertTileH);
}

void RasterTileMapper::getPixmap(int z, QPixmap & pixmap, QPolygonF& vertexInSceneCoords)
{
    vertexInSceneCoords = getBoundingPolygone();

    double POW(pow(2,z-1));
    QPolygon vertexInTiles, vertexInView;
    foreach(QPointF p, vertexInSceneCoords)
    {
        vertexInView.append( QPoint(p.x() * POW, p.y() * POW) );
        vertexInTiles.append( QPoint( (p.x() * POW) / sceneDescriptor.tileSize().width(),
                                      (p.y() * POW) / sceneDescriptor.tileSize().height() ) );
    }

    QRect bRect = vertexInTiles.boundingRect();

    QPixmap _pixmap(bRect.width()*sceneDescriptor.tileSize().width(), bRect.height()*sceneDescriptor.tileSize().height());
    _pixmap.fill(Qt::transparent);

    QPainter pr(&_pixmap);

    for(int X(0); X < bRect.width(); ++X)
        for(int Y(0); Y < bRect.height(); ++Y)
        {
            image_types::TImage* src = new image_types::TImage;
            QString cacheKey(QString::number((qulonglong)this) + "_" + QString::number(bRect.x()+X) + "_" + QString::number(bRect.y()+Y) + "_" + QString::number(z));
            if(TImageCache::instance()->find(cacheKey, src))
            {
                QImage _img(QSize(src->w, src->h), QImage::Format_ARGB32);
                memcpy(_img.bits(), src->prt, src->depth / 8 * src->w * src->h);

                pr.save();
                pr.setCompositionMode(QPainter::CompositionMode_SourceOver);
                pr.drawImage(X * sceneDescriptor.tileSize().width(),
                             Y * sceneDescriptor.tileSize().height(),
                             _img);
                pr.restore();
            }
            delete []src->prt;
            src->prt = 0;
            delete src;
        }

    QPolygon vertexOnPixmap;
    QPoint shift( bRect.x() * sceneDescriptor.tileSize().width(),
                  bRect.y() * sceneDescriptor.tileSize().height() );
    foreach(QPoint p, vertexInView)
    {
        vertexOnPixmap.append( QPoint( p.x() - shift.x(),
                                       p.y() - shift.y() ) );
    }
    pixmap = _pixmap.copy(vertexOnPixmap.boundingRect());
}


void RasterTileMapper::slotProviderChanged()
{
    TImageCache::instance()->clearProviderData( QString::number( ((qulonglong)(this)) ) );

//    if(sceneDescriptor.isGeoScene())
//    {
//        QPolygonF spatialPolygon;
//        dpf::PolygonF pixelPoly(raster_provider->geoImageObjectPtr()->image_rect());
//        dpf::PolygonF geoPoly = raster_provider->geoImageObjectPtr()->pixel_to_geo(pixelPoly);
//        for(const dpf::PointF& p : geoPoly)
//            spatialPolygon.push_back(QPointF(p.x(), p.y()));

////        qDebug() << "RasterTileMapper::slotProviderChanged() --------";
//        foreach(QPointF p, spatialPolygon)
//        {
//            double x(p.x()), y(p.y());
////            qDebug() << "x:" << QString::number(x, 'f', 6) << ", y:" << QString::number(y, 'f', 6);
//            sceneDescriptor.convertNativeToScene(x,y);
//            _boundingScenePolygone.append(QPointF(x,y));
//        }
////        qDebug() << "-----------------";

//        _boundingSceneRect = _boundingScenePolygone.boundingRect();
//        for(MIN_Z_LEVEL=1; MIN_Z_LEVEL<MAX_Z; ++MIN_Z_LEVEL)
//        {
//            double POW(pow(2,MIN_Z_LEVEL-1));
//            double W(_boundingSceneRect.width()*POW);
//            double H(_boundingSceneRect.height()*POW);
//            if( W >= sceneDescriptor.tileSize().width() || H >= sceneDescriptor.tileSize().height())
//            {
//                MIN_Z_LEVEL -= DOWN_DELTA_Z_LEVEL;
//                break;
//            }
//        }
//        if(MIN_Z_LEVEL == MAX_Z)
//            MIN_Z_LEVEL -= DOWN_DELTA_Z_LEVEL;

//        _rasterMapperCache.clear();
//    }
//    else
//    {
//        rasterPixBRect = raster_provider->boundingRect();
//        if(rasterPixBRect != QRect())
//        {
//            QSize baseSceneSize(sceneDescriptor.tileSize() * 2);
//            forever
//            {
//                QRect currRect(0, 0, baseSceneSize.width()*pow(2,baseZlevel-1), baseSceneSize.height()*pow(2,baseZlevel-1));
//                if(currRect.contains(rasterPixBRect))
//                {
//                    double coef(1. / pow(2,(baseZlevel - 1)) );
//                    double W(rasterPixBRect.width()*coef);
//                    double H(rasterPixBRect.height()*coef);

//                    shiftImageOnScene = QPointF((baseSceneSize.width() - W) / 2, (baseSceneSize.height() - H) / 2);
//                    rasterSceneRect = QRectF(shiftImageOnScene, QSizeF(W,H));
//                    double _W = rasterPixBRect.width();
//                    double _H = rasterPixBRect.height();
//                    rasterMapperCoef = (_W/rasterSceneRect.width() + _H/rasterSceneRect.height()) / 2.;

//                    break;
//                }
//                ++baseZlevel;
//            }
//        }
//    }

    emit signal_providerChanged();
}

















