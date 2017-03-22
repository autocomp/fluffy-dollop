#include "tmstilemapper.h"
#include <ctrcore/provider/timagecache.h>
#include <ctrrasterrender/tms/tmsdataprovider.h>
#include <ctrrasterrender/tms/tileloaderpool.h>
#include <math.h>

using namespace visualize_system;

TmsTileMapper::TmsTileMapper(uint visualizerId, uint providerId, const SceneDescriptor & _sceneDescriptor)
    : TileMapper(visualizerId, providerId, _sceneDescriptor)
{
    tms_provider = raster_provider.dynamicCast<data_system::TileDataProvider>();
    connect(tms_provider.data(), SIGNAL(tmsDataReady(int,uint,TileDataProvider::Result)), this, SLOT(slotDataReady(int,uint,TileDataProvider::Result)));

    QSharedPointer<TMSDataProvider>tmsDataProvider = tms_provider.dynamicCast<TMSDataProvider>();
    mapperKey = tmsDataProvider->registerSource(TileDataProvider::TmsMapper2D);

//    qDebug()  << "!!! tms_provider, spatialRect :" << tms_provider->spatialRect();
    int zLevel(tms_provider->minZoomLevel());
    while(zLevel <= MAX_Z_LEVEL)
    {
        QPoint bottomLeft = getTile(tms_provider->spatialRect().bottomLeft(), zLevel);
        QPoint topRight = getTile(tms_provider->spatialRect().topRight(), zLevel);
        QRect area(topRight, bottomLeft);
        tileAreaForZlevel.insert(zLevel, area);
        //qDebug()  << "---> zLevel :" << zLevel << ", topRight :" << topRight << ", bottomLeft :" << bottomLeft << ", area :" << area << ", area.bottomRight :" << area.bottomRight();
        ++zLevel;
    }
}

TmsTileMapper::~TmsTileMapper()
{
    tms_provider->unregisterSource(mapperKey);
//    qDebug() << "---> TmsTileMapper::~TmsTileMapper()";
}


QPolygonF TmsTileMapper::getBoundingPolygone()
{
    QPolygonF pixelPolygon;
    if(sceneDescriptor.isGeoScene())
    {
        QPolygonF spatialPolygon(tms_provider->spatialRect());
        foreach(QPointF p, spatialPolygon)
        {
//            if(sceneDescriptor.refEpsgSRS() == 4326)
//            {
//                double x(p.y()), y(p.x());
//                sceneDescriptor.convertNativeToScene(x,y);
//                pixelPolygon.append(QPointF(x,y));
//            }
//            else
            {
                double x(p.x()), y(p.y());
                sceneDescriptor.convertRefSystemToScene(x,y);
                pixelPolygon.append(QPointF(x,y));
            }
        }
    }
    return pixelPolygon;
}

void TmsTileMapper::loadTiles(const QList<visualize_system::URL> list)
{
    foreach (visualize_system::URL url, list)
    {
        bool _continue(false);
//        if(sceneDescriptor.refEpsgSRS() == 4326)
//        {
//            if(url.z >= tms_provider->minZoomLevel() && tileAreaForZlevel[url.z].contains(url.x, url.y))
//                ;
//            else
//                _continue = true;
//        }
//        else
        {
            if(url.z >= tms_provider->minZoomLevel())
                ;
            else
                _continue = true;
        }

        if(_continue)
        {
            QPixmap pm;
            //                qDebug() << "1 ===> TmsTileMapper QPixmap()" << url.returnId;
            emit signal_paintTile(PROVIDER_ID, url.returnId, pm, ORIGINAL);
            continue;
        }

        image_types::TImage* tImg = new image_types::TImage;
        QString cacheKey(QString::number((qulonglong)this) + "_" + QString::number(url.x) + "_" + QString::number(url.y) + "_" + QString::number(url.z));

        if(url.z > tms_provider->maxZoomLevel())
        {
            if(tms_provider->scaleForBottomLevels()) // нужно срендерить картинку с нижнего уровня
            {
                //                    qDebug() << "111, scaleForBottomLevels !!!" << cacheKey;
                Temp temp(tImg, url.returnId, url.x, url.y, url.z, cacheKey, QPoint());
                analizeNotFoundTask(temp);
            }
            else
            {
                delete tImg;
                QPixmap pm;
                //                    qDebug() << "2 ===> TmsTileMapper QPixmap()" << url.returnId;
                emit signal_paintTile(PROVIDER_ID, url.returnId, pm, ORIGINAL);
            }
            continue;
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

////                qDebug() << "3 ===> TmsTileMapper QPixmap()" << url.returnId;
//                emit signal_paintTile(PROVIDER_ID, url.returnId, pm, ORIGINAL);

//                delete []tImg->prt;
//                tImg->prt = 0;
//                delete tImg;

//                continue;
//            }
//        }

        if(loadingTaskKey.contains(cacheKey))
        {
            // сейчас идет обновление вьюпорта, а тайл по этому кэш-ключ итак уже поставлен на загрузку !
//            qDebug() << "&&& cacheKey in loadingTaskKey :" << cacheKey;
            continue;
        }

        if(TImageCache::instance()->find(cacheKey, tImg))
        {
//            qDebug() << "TmsTileMapper::loadTiles, IN CACHE, prov :" << ((u_int64_t)(tms_provider));

//            if(m_THisto.title.isEmpty() == false)
//            {
//                PoolTask poolTask(processingPoolSenderId, tImg, url.returnId);
//                ++tasksInProcessingsPool;
//                AlgorithmsPool::instance()->setTask(poolTask);
//            }
//            else
            {
                QImage _img(QSize(tImg->w, tImg->h), QImage::Format_ARGB32);
                memcpy(_img.bits(), tImg->prt, tImg->depth / 8 * tImg->w * tImg->h);
                QPixmap pm(QPixmap::fromImage(_img));

//                if(pm.isNull())
//                    qDebug() << "!!! 222 pm.isNull() !!!";

//                qDebug() << "4 ===> TmsTileMapper QPixmap()" << url.returnId;
                emit signal_paintTile(PROVIDER_ID, url.returnId, pm, ORIGINAL);

                delete []tImg->prt;
                tImg->prt = 0;
                delete tImg;
            }
        }
        else
        {
            if(sceneDescriptor.isGeoScene() && isTMSProviderMapper()==false && url.z < MIN_Z_LEVEL)
            {
                delete []tImg->prt;
                tImg->prt = 0;
                delete tImg;

                QPixmap pm;
//                qDebug() << "5 ===> TmsTileMapper QPixmap()" << url.returnId;
                emit signal_paintTile(PROVIDER_ID, url.returnId, pm, ORIGINAL);

                qDebug() << "mapper, MIN_Z_LEVEL" << MIN_Z_LEVEL << "url.z" << url.z;

                continue;
            }

//            qDebug() << "getTile" << url.x << url.y << url.z;
            //int ret = loadTile(tImg, url.x, url.y, url.z, shiftImageInTile);
            int ret = tms_provider->tile(*tImg, url.x, url.y, url.z, mapperKey);

//            qDebug() << "TmsTileMapper::loadTiles, ret :" << ret << ", prov :" << ((u_int64_t)(tms_provider));

            if(ret >= 0)
            {
                if(ret == 0) // картинка уже загружена в TImage (синхронно)
                {
//                    if(m_THisto.title.isEmpty() == false)
//                    {
//                        PoolTask poolTask(processingPoolSenderId, tImg, url.returnId);
//                        ++tasksInProcessingsPool;
//                        AlgorithmsPool::instance()->setTask(poolTask);
//                    }
//                    else
                    {
                        TImageCache::instance()->insert(cacheKey, tImg); // сохраняем тайл в кешер
                        QImage _img(QSize(tImg->w, tImg->h), QImage::Format_ARGB32);
                        memcpy(_img.bits(), tImg->prt, tImg->depth / 8 * tImg->w * tImg->h);
                        QPixmap pm(QPixmap::fromImage(_img));
//                        if(pm.isNull())
//                            qDebug() << "!!! 333 pm.isNull() !!!";

//                        qDebug() << "6 ===> TmsTileMapper QPixmap()" << url.returnId;
                        emit signal_paintTile(PROVIDER_ID, url.returnId, pm, ORIGINAL);

                        delete []tImg->prt;
                        tImg->prt = 0;
                        delete tImg;
                    }
                }
                else // картинка загружается асинхронно
                {
                    m_Map.insert(ret, Temp(tImg, url.returnId, url.x, url.y, url.z, cacheKey, QPoint()));
                    loadingTaskKey.insert(cacheKey);
                }
            }
            else // картинки с заданными x,y,z - не существует !
            {
                bool deleteImg(true);
                if(tms_provider->scaleForBottomLevels()) // нужно срендерить картинку с нижнего уровня
                {
                    // qDebug() << "scaleForBottomLevels !!!" << cacheKey;
                    Temp temp(tImg, url.returnId, url.x, url.y, url.z, cacheKey, QPoint());
                    analizeNotFoundTask(temp);
                    deleteImg = false;
                }

                if(deleteImg)
                {
                    delete []tImg->prt;
                    tImg->prt = 0;
                    delete tImg;

                    QPixmap pm;
//                    qDebug() << "7 ===> TmsTileMapper QPixmap()" << url.returnId;
                    emit signal_paintTile(PROVIDER_ID, url.returnId, pm, ORIGINAL);
                }
            }
        }
    }

    if(m_MapForOtherTiles.isEmpty() && m_Map.isEmpty() && tasksInProcessingsPool <= 0)
        emit signal_dataLoaded();
}

void TmsTileMapper::slotDataReady(int task, uint sourceId, TileDataProvider::Result result)
{
    if(sourceId != mapperKey)
        return;

    // ищем задачу среди тайлов предназначенных для "разрезания" и формирования тайлов на текущем z-уровне, зачача сформирована в маппере
    QMap<uint, OtherLevelData>::iterator otherLevelIt(m_MapForOtherTiles.find(task));
    if(otherLevelIt != m_MapForOtherTiles.end())
    {
        OtherLevelData otherLevelData(otherLevelIt.value());
        {
            QString cacheKey(QString::number((qulonglong)this) + "_" + QString::number(otherLevelData.tileCoords.x) + "_" + QString::number(otherLevelData.tileCoords.y) + "_" + QString::number(otherLevelData.tileCoords.z));
            loadingTaskKey.remove(cacheKey);
        }
//        qDebug() << "---------------";

        if(otherLevelData.tImg->isValid())
        {
//            qDebug() << "slotDataReady, task" << task << "tile loaded";

            cutTile(otherLevelData);

            QString cacheKey(QString::number((qulonglong)this) + "_" + QString::number(otherLevelData.tileCoords.x) + "_" + QString::number(otherLevelData.tileCoords.y) + "_" + QString::number(otherLevelData.tileCoords.z));
            TImageCache::instance()->insert(cacheKey, otherLevelData.tImg);
        }
        else // данного тайла НЕТ
        {
//            qDebug() << "slotDataReady, task" << task << " NOT loaded, need level :" << otherLevelData.targetZlevel;

//            foreach(Temp temp, otherLevelData.list)
//                qDebug() << "       task in list :" << temp.x << temp.y << temp.z;

            TileCoords tileCoords(otherLevelData.tileCoords.x / 2, otherLevelData.tileCoords.y / 2, otherLevelData.tileCoords.z - 1);
            QString cacheKey(QString::number((qulonglong)this) + "_" + QString::number(tileCoords.x) + "_" + QString::number(tileCoords.y) + "_" + QString::number(tileCoords.z));

//             qDebug() << "%%% Need :" << otherLevelData.tileCoords.x << otherLevelData.tileCoords.y << otherLevelData.tileCoords.z << ", Calc :" << tileCoords.x << tileCoords.y << tileCoords.z;
//             qDebug() << "cacheKey :" << cacheKey;

            // три варианта : 1)тайл в кеше, 2)тайл уже загружается и есть в "m_MapForOtherTiles", 3)нужно поставить тайл на загрузку.
            // 1 - тайл в кеше
            if(TImageCache::instance()->find(cacheKey, otherLevelData.tImg))
            {
                otherLevelData.tileCoords = tileCoords;
                cutTile(otherLevelData);
//                qDebug() << "---> var 111, load from cache !";
            }
            else
            {
            // 2 - тайл загружается в m_MapForOtherTiles
                bool tileFound(false);
//                qDebug() << "find in m_MapForOtherTiles, find :" << tileCoords.x << tileCoords.y << tileCoords.z;
                for(QMap<uint, OtherLevelData>::iterator iter(m_MapForOtherTiles.begin()); iter != m_MapForOtherTiles.end(); ++iter)
                {
//                    qDebug() << "curr :" << iter.value().tileCoords.x << iter.value().tileCoords.y << iter.value().tileCoords.z;

                    if(iter.value().tileCoords.equival(tileCoords.x, tileCoords.y, tileCoords.z))
                    {
                       iter.value().list.append(otherLevelData.list);
                       tileFound = true;
//                       qDebug() << "---> var 222, tile found";
                       break;
                    }
                }
//                if(tileFound == false) qDebug() << "NOT found";

            // 3 - нужно поставить тайл на загрузку
                if(tileFound == false)
                {
//                    qDebug() << "old :" << otherLevelData.tileCoords.x << otherLevelData.tileCoords.y << otherLevelData.tileCoords.z;
                    otherLevelData.tileCoords = tileCoords;
                    //int ret = loadTile(otherLevelData.tImg, otherLevelData.tileCoords.x, otherLevelData.tileCoords.y, otherLevelData.tileCoords.z, shiftImageInTile);
                    int ret = tms_provider->tile(*(otherLevelData.tImg), otherLevelData.tileCoords.x, otherLevelData.tileCoords.y, otherLevelData.tileCoords.z, mapperKey);
                    m_MapForOtherTiles.insert(ret, otherLevelData); // вставляем в мап с новым ключем
                    m_MapForOtherTiles.erase(otherLevelIt); // удаляем старую копию из мапа

//                    qDebug() << "---> var 333, old task :" << task << ", new task :" << ret;
//                    qDebug() << "new :" << tileCoords.x << tileCoords.y << tileCoords.z;

                    return; // выходим чтоб не удалять матрицу
                }
            }
        }

        delete []otherLevelData.tImg->prt;
        otherLevelData.tImg->prt = 0;
        delete otherLevelData.tImg;
        m_MapForOtherTiles.erase(otherLevelIt);

        if(m_MapForOtherTiles.isEmpty() && m_Map.isEmpty() && tasksInProcessingsPool <= 0)
            emit signal_dataLoaded();
//        else
//            qDebug() << "*** m_MapForOtherTiles.size :" << m_MapForOtherTiles.size();

        return;
    }

    // ишем задачу среди обычных задач поставленных извне
    QMap<uint, Temp>::iterator it(m_Map.find(task));
    if(it != m_Map.end())
    {
        Temp temp(it.value());
        loadingTaskKey.remove(temp.cacheKey);

        if(result == TileDataProvider::TileDataProvider::Loaded)
        {
//            qDebug() << "---> tImg->isValid() :" << temp.x << temp.y << temp.z;

            image_types::TImage* tImg(temp.tImg);

            TImageCache::instance()->insert(temp.cacheKey, tImg); // сохраняем тайл в кешер
//            qDebug() << "---> tImg->isValid() :" << temp.x << temp.y << temp.z << ", savedToCache :" << B;

//            if(m_THisto.title.isEmpty() == false)
//            {
//                PoolTask poolTask(processingPoolSenderId, tImg, temp.task);
//                ++tasksInProcessingsPool;
//                AlgorithmsPool::instance()->setTask(poolTask);
//            }
//            else
            {
                QImage _img(QSize(tImg->w, tImg->h), QImage::Format_ARGB32);
                memcpy(_img.bits(), tImg->prt, tImg->depth / 8 * tImg->w * tImg->h);
                QPixmap pm(QPixmap::fromImage(_img));
//                if(_img.isNull())
//                    qDebug() << "^^^ QImage is NULL :" << temp.cacheKey;
//                else if(pm.isNull())
//                    qDebug() << "^^^ QPixmap is NULL :" << temp.cacheKey;
//                else
//                    pm.save("/home/sergvas/Desktop/img/" + temp.cacheKey + ".bmp", "BMP");

                emit signal_paintTile(PROVIDER_ID, temp.task, pm, ORIGINAL);

                delete [] tImg->prt;
                tImg->prt = 0;
                delete tImg;
            }
        }
        else // провайдер уведомляет, что данного тайла НЕТ
        {
            if(queryArea.z != temp.z)
            {
                // Что случилось ?
                if(result == TileDataProvider::TileDataProvider::NotFound)
                {
                    // 1) тайла НЕТ и его необходимо подобрать из нижних уровней
                    //                        qDebug() << "Background NotFound ! Find down z-level ! ---> XYZ :" << temp.x << temp.y << temp.z << ", query Z :" << queryArea.z;
                    analizeNotFoundTask(temp);
                }
                else // (result == TileDataProvider::TileDataProvider::Aborted)
                {
                    // 2) тайла выбыл из очереди, его необходимо перезапросить
                    if(TImageCache::instance()->find(temp.cacheKey, temp.tImg)) // картинка загружена из кешера
                    {
                        //                            qDebug() << "Reload background from cahce ---> XYZ :" << temp.x << temp.y << temp.z << ", query Z :" << queryArea.z;
//                        if(m_THisto.title.isEmpty() == false)
//                        {
//                            PoolTask poolTask(processingPoolSenderId, temp.tImg, temp.task);
//                            ++tasksInProcessingsPool;
//                            AlgorithmsPool::instance()->setTask(poolTask);
//                        }
//                        else
                        {
                            QImage _img(QSize(temp.tImg->w, temp.tImg->h), QImage::Format_ARGB32);
                            memcpy(_img.bits(), temp.tImg->prt, temp.tImg->depth / 8 * temp.tImg->w * temp.tImg->h);
                            QPixmap pm(QPixmap::fromImage(_img));

                            //                                if(pm.isNull())
                            //                                    qDebug() << "!!! 444 pm.isNull() !!!";

                            //                                qDebug() << "9 ===> TmsTileMapper QPixmap()" << temp.task;
                            emit signal_paintTile(PROVIDER_ID, temp.task, pm, ORIGINAL);

                            delete []temp.tImg->prt;
                            temp.tImg->prt = 0;
                            delete temp.tImg;
                        }
                    }
                    else
                    {
                        //int ret = loadTile(temp.tImg, temp.x, temp.y, temp.z, shiftImageInTile);
                        int ret = tms_provider->tile(*(temp.tImg), temp.x, temp.y, temp.z, mapperKey);
                        if(ret >= 0)
                        {
                            if(ret == 0) // картинка уже загружена в TImage (синхронно)
                            {
                                //                                    qDebug() << "Background loaded sinc ! ---> XYZ :" << temp.x << temp.y << temp.z << ", query Z :" << queryArea.z;
                                TImageCache::instance()->insert(temp.cacheKey, temp.tImg); // сохраняем тайл в кешер
//                                if(m_THisto.title.isEmpty() == false)
//                                {
//                                    PoolTask poolTask(processingPoolSenderId, temp.tImg, temp.task);
//                                    ++tasksInProcessingsPool;
//                                    AlgorithmsPool::instance()->setTask(poolTask);
//                                }
//                                else
                                {
                                    QImage _img(QSize(temp.tImg->w, temp.tImg->h), QImage::Format_ARGB32);
                                    memcpy(_img.bits(), temp.tImg->prt, temp.tImg->depth / 8 * temp.tImg->w * temp.tImg->h);
                                    QPixmap pm(QPixmap::fromImage(_img));
                                    //                                        qDebug() << "10 ===> TmsTileMapper QPixmap()" << temp.task;
                                    emit signal_paintTile(PROVIDER_ID, temp.task, pm, ORIGINAL);
                                }
                            }
                            else // картинка загружается асинхронно
                            {
                                //                                    qDebug() << "Reload background ---> XYZ :" << temp.x << temp.y << temp.z << ", query Z :" << queryArea.z;
                                m_Map.insert(ret, temp);
                            }
                        }
                        else // картинки почемуто нет ...
                        {
                            //                                qDebug() << "Background RET==-1 ! ---> XYZ :" << temp.x << temp.y << temp.z << ", query Z :" << queryArea.z;

                            delete []temp.tImg->prt;
                            temp.tImg->prt = 0;
                            delete temp.tImg;

                            QPixmap pm;
                            //                                qDebug() << "11 ===> TmsTileMapper QPixmap()" << temp.task;
                            emit signal_paintTile(PROVIDER_ID, temp.task, pm, ORIGINAL);
                        }
                    }
                }
            }
            else
            {
                if(queryArea.rect.contains(QPoint(temp.x, temp.y)))
                {
                    // тайл все еще в области видимости, нужна инфа о том что случилось :
                    if(result == TileDataProvider::TileDataProvider::NotFound)
                    {
                        // 1) тайла НЕТ и его необходимо подобрать из нижних уровней
                        //                            qDebug() << "Main cover NotFound ! Find down z-level ! ---> XYZ :" << temp.x << temp.y << temp.z << ", query Z :" << queryArea.z;
                        analizeNotFoundTask(temp);
                    }
                    else // (result == TileDataProvider::TileDataProvider::Aborted)
                    {
                        // 2) тайла выбыл из очереди, его необходимо перезапросить
                        if(TImageCache::instance()->find(temp.cacheKey, temp.tImg)) // картинка загружена из кешера
                        {
                            //                                qDebug() << "Reload main cover from cahce ---> XYZ :" << temp.x << temp.y << temp.z << ", query Z :" << queryArea.z;

//                            if(m_THisto.title.isEmpty() == false)
//                            {
//                                PoolTask poolTask(processingPoolSenderId, temp.tImg, temp.task);
//                                ++tasksInProcessingsPool;
//                                AlgorithmsPool::instance()->setTask(poolTask);
//                            }
                            {
                                QImage _img(QSize(temp.tImg->w, temp.tImg->h), QImage::Format_ARGB32);
                                memcpy(_img.bits(), temp.tImg->prt, temp.tImg->depth / 8 * temp.tImg->w * temp.tImg->h);
                                QPixmap pm(QPixmap::fromImage(_img));

                                //                                    if(pm.isNull())
                                //                                        qDebug() << "!!! pm.isNull() !!!";

                                //                                    qDebug() << "12 ===> TmsTileMapper QPixmap()" << temp.task;
                                emit signal_paintTile(PROVIDER_ID, temp.task, pm, ORIGINAL);

                                delete []temp.tImg->prt;
                                temp.tImg->prt = 0;
                                delete temp.tImg;
                            }
                        }
                        else
                        {
                            //int ret = loadTile(temp.tImg, temp.x, temp.y, temp.z, shiftImageInTile);
                            int ret = tms_provider->tile(*(temp.tImg), temp.x, temp.y, temp.z, mapperKey);
                            if(ret >= 0)
                            {
                                if(ret == 0) // картинка уже загружена в TImage (синхронно)
                                {
                                    //                                        qDebug() << "Main cover loaded sinc ! ---> XYZ :" << temp.x << temp.y << temp.z << ", query Z :" << queryArea.z;
                                    TImageCache::instance()->insert(temp.cacheKey, temp.tImg); // сохраняем тайл в кешер
//                                    if(m_THisto.title.isEmpty() == false)
//                                    {
//                                        PoolTask poolTask(processingPoolSenderId, temp.tImg, temp.task);
//                                        ++tasksInProcessingsPool;
//                                        AlgorithmsPool::instance()->setTask(poolTask);
//                                    }
//                                    else
                                    {
                                        QImage _img(QSize(temp.tImg->w, temp.tImg->h), QImage::Format_ARGB32);
                                        memcpy(_img.bits(), temp.tImg->prt, temp.tImg->depth / 8 * temp.tImg->w * temp.tImg->h);
                                        QPixmap pm(QPixmap::fromImage(_img));
                                        //                                            qDebug() << "13 ===> TmsTileMapper QPixmap()" << temp.task;
                                        emit signal_paintTile(PROVIDER_ID, temp.task, pm, ORIGINAL);
                                    }
                                }
                                else // картинка загружается асинхронно
                                {
                                    //                                        qDebug() << "Reload main cover ---> XYZ :" << temp.x << temp.y << temp.z << ", query Z :" << queryArea.z;
                                    m_Map.insert(ret, temp);
                                }
                            }
                            else // картинки почемуто нет ...
                            {
                                //                                    qDebug() << "Main cover RET==-1 ! ---> XYZ :" << temp.x << temp.y << temp.z << ", query Z :" << queryArea.z;
                                delete []temp.tImg->prt;
                                temp.tImg->prt = 0;
                                delete temp.tImg;

                                QPixmap pm;
                                //                                    qDebug() << "14 ===> TmsTileMapper QPixmap()" << temp.task;
                                emit signal_paintTile(PROVIDER_ID, temp.task, pm, ORIGINAL);
                            }
                        }
                    }
                }
                else
                {
                    // тайл уже выбыл из области видимости !
                    //                        qDebug() << "Main cover outside from viewport ! ---> XYZ :" << temp.x << temp.y << temp.z << ", query Z :" << queryArea.z << ", RECT :" << queryArea.rect;
                    QPixmap pm;
                    //                        qDebug() << "15 ===> TmsTileMapper QPixmap()" << temp.task;
                    emit signal_paintTile(PROVIDER_ID, temp.task, pm, ORIGINAL);

                    delete []temp.tImg->prt;
                    temp.tImg->prt = 0;
                    delete temp.tImg;
                }
            }
        }

        m_Map.erase(it);
    }

//    qDebug() << "===> slotRasterDataReady, m_MapForOtherTiles" << m_MapForOtherTiles.size() << "m_Map" << m_Map.size() << "processingPoolSenderId" << processingPoolSenderId;
    if(m_MapForOtherTiles.isEmpty() && m_Map.isEmpty() && tasksInProcessingsPool <= 0)
        emit signal_dataLoaded();
}


void TmsTileMapper::analizeNotFoundTask(Temp temp)
{
    if(/*temp.z == queryArea.z && */queryArea.z > 3)
    {
        TileCoords tileCoords(temp.x/2, temp.y/2, temp.z-1);
        QString cacheKey(QString::number((qulonglong)this) + "_" + QString::number(tileCoords.x) + "_" + QString::number(tileCoords.y) + "_" + QString::number(tileCoords.z));

        // qDebug() << "temp :" << temp.x << temp.y << temp.z;
        // qDebug() << "cacheKey :" << cacheKey;

        OtherLevelData otherLevelData(tileCoords, temp.z);
        otherLevelData.list.append(temp);
        image_types::TImage* tImg = new image_types::TImage;
        otherLevelData.tImg = tImg;

        // три варианта : 1)тайл в кеше, 2)тайл уже загружается и есть в "m_MapForOtherTiles", 3)нужно поставить тайл на загрузку.
        // 1 - тайл в кеше
        if(TImageCache::instance()->find(cacheKey, otherLevelData.tImg))
        {
            cutTile(otherLevelData);

            delete []otherLevelData.tImg->prt;
            otherLevelData.tImg->prt = 0;
            delete otherLevelData.tImg;
//            qDebug() << "---> analizeNotFoundTask, load from cache :" << temp.x << temp.y << temp.z;
        }
        else
        {
        // 2 - тайл загружается в m_MapForOtherTiles
            bool tileFound(false);
//            qDebug() << "find in m_MapForOtherTiles, find :" << tileCoords.x << tileCoords.y << tileCoords.z;
            for(QMap<uint, OtherLevelData>::iterator iter(m_MapForOtherTiles.begin()); iter != m_MapForOtherTiles.end(); ++iter)
            {
//                qDebug() << "curr :" << iter.value().tileCoords.x << iter.value().tileCoords.y << iter.value().tileCoords.z;

                if(iter.value().tileCoords.equival(tileCoords.x, tileCoords.y, tileCoords.z))
                {
                   iter.value().list.append(otherLevelData.list); // перекидываем все задачи в найденную загрузку

                   delete []otherLevelData.tImg->prt;
                   otherLevelData.tImg->prt = 0;
                   delete otherLevelData.tImg;

                   tileFound = true;
//                   qDebug() << "---> var 222, tile found !!!";
                   break;
                }
            }
//            if(tileFound == false) qDebug() << "not found !!!";

        // 3 - нужно поставить тайл на загрузку
            if(tileFound == false)
            {
//                qDebug() << "}}} analizeNotFoundTask, set loading :" << otherLevelData.tileCoords.x << otherLevelData.tileCoords.y << otherLevelData.tileCoords.z;
                //int ret = loadTile(otherLevelData.tImg, otherLevelData.tileCoords.x, otherLevelData.tileCoords.y, otherLevelData.tileCoords.z, shiftImageInTile);
                int ret = tms_provider->tile(*(otherLevelData.tImg), otherLevelData.tileCoords.x, otherLevelData.tileCoords.y, otherLevelData.tileCoords.z, mapperKey);
                if(ret >= 0)
                {
                    m_MapForOtherTiles.insert(ret, otherLevelData);
                }
                else
                {
//                    qDebug() << "---> var 333, analizeNotFoundTask, scaleForBottomLevels" << cacheKey << ((u_int64_t)(tms_provider));
                    if(tms_provider->scaleForBottomLevels())
                    {
                        while(tileCoords.z > tms_provider->maxZoomLevel())
                            tileCoords = TileCoords(tileCoords.x/2, tileCoords.y/2, tileCoords.z-1);
                        //                            qDebug() << "scaleForBottomLevels, target Z :" << temp.z << ", valid Z :" << tileCoords.z;
                    }

                    if(tileCoords.z < tms_provider->minZoomLevel())
                    {
//                        qDebug() << "+++ analizeNotFoundTask, tileCoords.z < tms_provider->minZoomLevel()";
                        delete []otherLevelData.tImg->prt;
                        otherLevelData.tImg->prt = new char[sceneDescriptor.tileSize().width() * sceneDescriptor.tileSize().height() * 4];
                        memset(otherLevelData.tImg->prt, 0, sceneDescriptor.tileSize().width() * sceneDescriptor.tileSize().height() * 4);

                        cutTile(otherLevelData);

                        delete []otherLevelData.tImg->prt;
                        otherLevelData.tImg->prt = 0;
                        delete otherLevelData.tImg;
                    }
                    else
                    {
                        otherLevelData.tileCoords = tileCoords;
                        //ret = loadTile(otherLevelData.tImg, otherLevelData.tileCoords.x, otherLevelData.tileCoords.y, otherLevelData.tileCoords.z, shiftImageInTile);
                        int ret = tms_provider->tile(*(otherLevelData.tImg), otherLevelData.tileCoords.x, otherLevelData.tileCoords.y, otherLevelData.tileCoords.z, mapperKey);
                        m_MapForOtherTiles.insert(ret, otherLevelData);
                    }
                }

//                qDebug() << "---> var 333, new task :" << ret;
//                qDebug() << "old :" << temp.x << temp.y << temp.z;
//                qDebug() << "new :" << otherLevelData.tileCoords.x << otherLevelData.tileCoords.y << otherLevelData.tileCoords.z;
            }
        }
    }
//    else
//        qDebug() << "***** analizeNotFoundTask, ELSE, temp :" << temp.x << temp.y << temp.z << ", queryArea.z :" << queryArea.z;
}

void TmsTileMapper::cutTile(OtherLevelData& otherLevelData)
{
//    qDebug() << "!!!!! TmsTileMapper::cutTile !!!!!";

    int deltaZ(otherLevelData.targetZlevel - otherLevelData.tileCoords.z);
    const int K(pow(2, deltaZ > 8 ? 8 : deltaZ));

    QImage _img(QSize(otherLevelData.tImg->w, otherLevelData.tImg->h), QImage::Format_ARGB32);
    memcpy(_img.bits(), otherLevelData.tImg->prt, otherLevelData.tImg->depth / 8 * otherLevelData.tImg->w * otherLevelData.tImg->h);

    // QString name = QString::number(otherLevelData.tileCoords.x) + "_" + QString::number(otherLevelData.tileCoords.y);
    // _img.save("/home/sergvas/Desktop/img/big_" + name + ".png");

//    qDebug() << "===>" << otherLevelData.tileCoords.x << otherLevelData.tileCoords.y << otherLevelData.tileCoords.z << " Koef :" << K;
    foreach(Temp temp, otherLevelData.list)
    {

//        qDebug() << "BEFORE SAVE TO CACHE, otherLevelData :" << otherLevelData.tileCoords.x << otherLevelData.tileCoords.y << otherLevelData.tileCoords.z;
//        qDebug() << "BEFORE SAVE TO CACHE, temp :" << temp.x << temp.y << temp.z << ", deltaZ :" << deltaZ;

        //--------------------------------
        // все промежуточные тайлы между нужным и найденым сохраняются в кэш !
//        TileCoords _tileCoords_(temp.x/2, temp.y/2, temp.z-1);
//        while( _tileCoords_.z > otherLevelData.tileCoords.z)
//        {
//            QString _cacheKey_(QString::number((qulonglong)this) + "_" + QString::number(_tileCoords_.x) + "_" + QString::number(_tileCoords_.y) + "_" + QString::number(_tileCoords_.z));
//            if( TImageCache::instance()->contains(_cacheKey_) == false )
//            {
//                qDebug() << "NEED SAVE TO CACHE :" << _tileCoords_.x << _tileCoords_.y << _tileCoords_.z;

//                int _deltaZ_(otherLevelData.targetZlevel - _tileCoords_.z);
//                const int rasterMapperCoef(pow(2, _deltaZ_ > 8 ? 8 : _deltaZ_));
//                int X = _tileCoords_.x - otherLevelData.tileCoords.x * rasterMapperCoef;
//                int Y = _tileCoords_.y - otherLevelData.tileCoords.y * rasterMapperCoef;
//                int W = tileSize.width() / rasterMapperCoef;
//                int H = tileSize.height() / rasterMapperCoef;
//                QImage _sourceImage_ = _img.copy(X * W, Y * H, W, H);
//                _sourceImage_ = _sourceImage_.scaled(_sourceImage_.width() * rasterMapperCoef, _sourceImage_.height() * rasterMapperCoef);

//                image_types::TImage* _sourceImg_ = new image_types::TImage;
//                _sourceImg_->depth = 32;
//                _sourceImg_->w = _sourceImage_.width();
//                _sourceImg_->h = _sourceImage_.height();
//                _sourceImg_->prt = new char[_sourceImg_->w * _sourceImg_->h * 4];
//                memcpy(_sourceImg_->prt, _sourceImage_.bits(), _sourceImg_->w * _sourceImg_->h * 4);

//                QString cacheKey(QString::number((qulonglong)this) + "_" + QString::number(_tileCoords_.x) + "_" + QString::number(_tileCoords_.y) + "_" + QString::number(_tileCoords_.z));
//                TImageCache::instance()->insert(cacheKey, _sourceImg_);

//                delete []_sourceImg_->prt;
//                _sourceImg_->prt = 0;
//                delete _sourceImg_;
//            }
//            else
//                qDebug() << "TILE IN CACHE :" << _tileCoords_.x << _tileCoords_.y << _tileCoords_.z;


//            _tileCoords_.x /= 2;
//            _tileCoords_.y /= 2;
//            _tileCoords_.z -= 1;
//        }
        //--------------------------------

        int X = temp.x - otherLevelData.tileCoords.x * K;
        int Y = temp.y - otherLevelData.tileCoords.y * K;
        int W = sceneDescriptor.tileSize().width() / K;
        int H = sceneDescriptor.tileSize().height() / K;
        QImage sourceImage = _img.copy(X * W, Y * H, W, H);

        sourceImage = sourceImage.scaled(sourceImage.width() * K, sourceImage.height() * K);

        image_types::TImage* sourceImg = new image_types::TImage;
        sourceImg->depth = 32;
        sourceImg->w = sourceImage.width();
        sourceImg->h = sourceImage.height();
        sourceImg->prt = new char[sourceImg->w * sourceImg->h * 4];
        memcpy(sourceImg->prt, sourceImage.bits(), sourceImg->w * sourceImg->h * 4);

        QString cacheKey(QString::number((qulonglong)this) + "_" + QString::number(temp.x) + "_" + QString::number(temp.y) + "_" + QString::number(temp.z));
        TImageCache::instance()->insert(cacheKey, sourceImg);

//        if(m_THisto.title.isEmpty() == false)
//        {
//            PoolTask poolTask(processingPoolSenderId, sourceImg, temp.task);
//            ++tasksInProcessingsPool;
//            AlgorithmsPool::instance()->setTask(poolTask);
//        }
//        else
        {
            memcpy(sourceImage.bits(), sourceImg->prt, sourceImg->w * sourceImg->h * 4);
            QPixmap pm(QPixmap::fromImage(sourceImage));
//            qDebug() << "17 ===> TmsTileMapper QPixmap()" << temp.task;
            emit signal_paintTile(PROVIDER_ID, temp.task, pm, ORIGINAL);

            delete []sourceImg->prt;
            sourceImg->prt = 0;
            delete sourceImg;
        }

        // QString name = QString::number(temp.x) + "_" + QString::number(temp.y);
        // targetImage.save("/home/sergvas/Desktop/img/" + name + ".png");

//        qDebug() << "->" << X << Y << W << H << temp.x << temp.y << temp.z;
    }
}


bool TmsTileMapper::dataLoaded()
{
//    qDebug() << "TmsTileMapper::dataLoaded(), m_MapForOtherTiles"  << m_MapForOtherTiles.size() << ", m_Map:" << m_Map.size() << ", tasksInProcessingsPool:" << tasksInProcessingsPool;
    return m_MapForOtherTiles.isEmpty() && m_Map.isEmpty() && tasksInProcessingsPool <= 0;
}

void TmsTileMapper::abort(QString taskKey)
{
    loadingTaskKey.remove(taskKey);
}

void TmsTileMapper::abort()
{
    tasksInProcessingsPool = 0;
//    if(_hasProcessing)
//        processing_system::ProcessingPool::instance()->removeTasks(processingPoolSenderId);

    m_Map.clear();
    m_MapForOtherTiles.clear();
//    qDebug() << "^^^ TmsTileMapper::abort, loadingTaskKey.size :" << loadingTaskKey.size();
    loadingTaskKey.clear();

    tms_provider->abortAllTaskFromSource(mapperKey);
}







