#include "tilemapper3d.h"
//#include "tileloaderpool.h"

//#include <ctrcore/provider/timagecache.h>
//#include <QDebug>
//#include <QImage>
//#include <math.h>

//TileMapper3D::TileMapper3D(RasterDataProvider* provider, QSize _tileSize, QSize _tilesCount)
//    : raster_provider(0)
//    , tileSize(_tileSize)
//    , tilesCount(_tilesCount)
//    , mapperSourceId(0)
//{
//    tms_provider = dynamic_cast<TMSDataProvider*>(provider);
//    if(tms_provider)
//    {
//        mapperSourceId = tms_provider->registerSource(TileDataProvider::TmsMapper3D);
//        connect(tms_provider, SIGNAL(tmsDataReady(int,uint,TileDataProvider::Result)), this, SLOT(slotDataReady(int,uint,TileDataProvider::Result)));
//    }
//    else
//    {
//        raster_provider = provider;
//        connect(raster_provider, SIGNAL(dataReady(int)), this, SLOT(slotRasterDataReady(int)));
//    }
//}

//TileMapper3D::~TileMapper3D()
//{
//    if(tms_provider)
//        tms_provider->unregisterSource(mapperSourceId);

//    if(raster_provider)
//        for(QMap<uint, TempStruct>::iterator it(m_Map.begin()); it != m_Map.end(); ++it)
//            raster_provider->abort(it.key());
//}

//RasterDataProvider* TileMapper3D::provider() const
//{
//    if(raster_provider)
//        return raster_provider;

//    if(tms_provider)
//        return tms_provider;

//    return 0;
//}

//void TileMapper3D::abortAllTasks()
//{
//    qDebug() << "void TileMapper3D::abortAllTasks";

//    if(tms_provider)
//    {
//        tms_provider->abortAllTaskFromSource(mapperSourceId);
//    }

//    if(raster_provider)
//    {
//        raster_provider->abort();
//        // for(QMap<uint, TempStruct>::iterator it(m_Map.begin()); it != m_Map.end(); ++it)
//        //     raster_provider->abort(it.key());
//    }

//    m_Map.clear();
//}

//int TileMapper3D::loadTile(image_types::TImage *img, int x, int y, int z)
//{
////    qDebug() << "void TileMapper3D::loadTile" << x << y << z;

//    int provider_task = -1;

//    if(raster_provider)
//    {
//        QRectF geoBboundingRect = raster_provider->spatialRect();
//        if(geoBboundingRect != QRectF() )
//        {
//            QRectF tileExtend(getTileExtend(x,y,z));
//            if(geoBboundingRect.intersects(tileExtend))
//            {
//                provider_task =  raster_provider->readSpatialData(*img, tileExtend, QRect(QPoint(0,0),tileSize));
//            }
//        }
//    }

//    if(tms_provider)
//    {
//        provider_task = tms_provider->tile(*img, x, y, z, mapperSourceId);
//    }

//    return provider_task;
//}

//void TileMapper3D::loadTile(int x, int y, int z, QString returnKey)
//{
//    image_types::TImage* tImg = new image_types::TImage;
//    QString cacheKey(QString::number((qulonglong)this) + "_" + QString::number(x) + "_" + QString::number(y) + "_" + QString::number(z));

//    if(TImageCache::instance()->find(cacheKey, tImg))
//    {
//        qDebug() << "void TileMapper3D::loadTile, returnKey :" << returnKey << "cacheKey" << cacheKey << "From cache";

//        // картинка загружена из кешера
//        QImage _img(QSize(tImg->w, tImg->h), QImage::Format_ARGB32);
//        memcpy(_img.bits(), tImg->prt, tImg->depth / 8 * tImg->w * tImg->h);
//        emit signal_paintTile(returnKey, _img);
//        emit dataReady(returnKey, _img, TileDataProvider::Loaded);

//        delete []tImg->prt;
//        tImg->prt = 0;
//        delete tImg;
//    }
//    else
//    {
//        if(m_Set.contains(returnKey))
//        {
//            qDebug() << "void TileMapper3D::loadTile, returnKey :" << returnKey << "cacheKey" << cacheKey << "Loading";
//            return;
//        }

//        int ret = loadTile(tImg, x, y, z);
//        if(ret >= 0)
//        {
//            if(ret == 0) // картинка уже загружена в TImage (синхронно)
//            {
//                TImageCache::instance()->insert(cacheKey, tImg); // сохраняем тайл в кешер
//                QImage _img(QSize(tImg->w, tImg->h), QImage::Format_ARGB32);
//                memcpy(_img.bits(), tImg->prt, tImg->depth / 8 * tImg->w * tImg->h);

//                emit signal_paintTile(returnKey, _img);
//                emit dataReady(returnKey, _img, TileDataProvider::Loaded);
//            }
//            else // картинка загружается асинхронно
//            {
//                m_Map.insert(ret, TempStruct(tImg, returnKey, x, y, z, cacheKey));
//                m_Set.insert(returnKey);

//                qDebug() << "void TileMapper3D::loadTile, returnKey :" << returnKey << "cacheKey" << cacheKey << "Asinc";
//            }
//        }
//        else // картинки с заданными x,y,z - не существует !
//        {
//            delete []tImg->prt;
//            tImg->prt = 0;
//            delete tImg;

//            QImage _img;
//            emit signal_paintTile(returnKey, _img);
//            emit dataReady(returnKey, _img, TileDataProvider::NotFound);

//            qDebug() << "void TileMapper3D::loadTile, returnKey :" << returnKey << "cacheKey" << cacheKey << "NotFound";
//        }
//    }
//}

//void TileMapper3D::slotRasterDataReady(int task)
//{
//    slotDataReady(task, mapperSourceId, TileDataProvider::TileDataProvider::Loaded);
//}

//void TileMapper3D::slotDataReady(int task, uint sourceId, TileDataProvider::Result result)
//{
//    if(sourceId != mapperSourceId)
//        return;

//    QMap<uint, TempStruct>::iterator it(m_Map.find(task));
//    if(it != m_Map.end())
//    {
//        TempStruct temp(it.value());
//        image_types::TImage* tImg(temp.tImg);

//        m_Set.remove(temp.task);
////        qDebug() << "TileMapper3D::slotDataReady, set size :" << m_Set.size();

//        if(result == TileDataProvider::TileDataProvider::Loaded)
//        {
//            TImageCache::instance()->insert(temp.cacheKey, tImg); // сохраняем тайл в кешер

//            QImage _img(QSize(tImg->w, tImg->h), QImage::Format_ARGB32);
//            memcpy(_img.bits(), tImg->prt, tImg->depth / 8 * tImg->w * tImg->h);

//            emit signal_paintTile(temp.task, _img);
//            emit dataReady(temp.task, _img, TileDataProvider::Loaded);
//        }
//        else
//        {
//            QImage _img;
//            emit signal_paintTile(temp.task, _img);
//            emit dataReady(temp.task, _img, result);
//        }

//        delete []tImg->prt;
//        tImg->prt = 0;
//        delete tImg;

//        qDebug() << "void TileMapper3D::slotDataReady, cacheKey" << it.value().cacheKey;

//        m_Map.erase(it);
//    }
//}

//void TileMapper3D::abort(const QString& returnKey)
//{
//    for(QMap<uint, TempStruct>::iterator it(m_Map.begin()); it != m_Map.end(); ++it)
//        if( it.value().task == returnKey )
//        {
//            if(raster_provider)
//                raster_provider->abort(it.key());

//            if(tms_provider)
//            {
//                tms_provider->abortTaskFromSource(it.key(), mapperSourceId);
//                //TileLoaderPool::instance()->abortTaskFromSource(it.key(), mapperSourceId); //tms_provider->abort(it.key());
//            }

//            //qDebug() << "void TileMapper3D::abort(const QString& returnKey)" << returnKey << "cacheKey" << it.value().cacheKey;

//            m_Set.remove(it.value().task);
//            m_Map.erase(it);
//            return;
//        }
//}

//bool TileMapper3D::isTMSProviderMapper()
//{
//    return tms_provider != 0;
//}

//QPointF TileMapper3D::geoToScene(const QPointF& coordinateIn)
//{
//    const uint W(tileSize.width() * tilesCount.width());
//    const uint H(tileSize.height() * tilesCount.height());

//    QPointF point;
//    point.setX( (coordinateIn.y() +180 )*W/360.0 ); // long
//    point.setY( ( -coordinateIn.x()  + 90)*( H)/180.0 ); //latt
//    return point;
//}


//QPointF TileMapper3D::sceneToGeo(const QPointF& point)
//{
//    const uint W(tileSize.width() * tilesCount.width());
//    const uint H(tileSize.height() * tilesCount.height());

//    double longitude = (point.x()*(360. / W))-180.;
//    double latitude = -(point.y()*(180. / H))+90.;

//    return QPointF(latitude, longitude);
//}

//QRectF TileMapper3D::getTileExtend(int x, int y, int z)
//{
//    double POW(pow(2,z-1));
//    double tileWidthOnView(tileSize.width() / POW);
//    double tileHeightOnView(tileSize.height() / POW);

//    QRectF tileSceneRect(tileWidthOnView*x, tileHeightOnView*y, tileWidthOnView, tileHeightOnView);
//    QPolygonF sceneRectPolygon(tileSceneRect), geoRectPolygon;
//    foreach(QPointF p, sceneRectPolygon)
//        geoRectPolygon.append(sceneToGeo(p));

//    return geoRectPolygon.boundingRect();
//}

//QPolygonF TileMapper3D::getBoundingPolygone()
//{
//    QPolygonF pixelPolygon;
//    QRectF geoBboundingRect = raster_provider->spatialRect();
//    QPolygonF geoPolygon;
//    geoPolygon << geoBboundingRect.topLeft() << geoBboundingRect.topRight() << geoBboundingRect.bottomRight() << geoBboundingRect.bottomLeft();
//    foreach(QPointF p, geoPolygon)
//        pixelPolygon.append( geoToScene(p) );

//    return pixelPolygon;
//}
