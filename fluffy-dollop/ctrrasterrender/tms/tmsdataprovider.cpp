#include "tmsdataprovider.h"
#include <ctrcore/provider/uniqueidfactory.h>
#include "tileloaderpool.h"
#include <QPainter>

using namespace data_system;

TMSDataProvider::TMSDataProvider()
    : mTileSize(QSize(256, 256))
    , m_spatialRect(QRectF(QPointF(-90,-180),QPointF(90,180)))
{
    connect(TileLoaderPool::instance(), SIGNAL(signal_requestFinished(QString,uint,QByteArray*,TileDataProvider::Result)), this, SLOT(slot_requestFinished(QString,uint,QByteArray*,TileDataProvider::Result)));
    //connect(TileLoaderPool::instance(), SIGNAL(signal_requestFinished(int,uint,QByteArray*,TileDataProvider::Result)), this, SLOT(slot_requestFinished(int,uint,QByteArray*,TileDataProvider::Result)));
}

TMSDataProvider::~TMSDataProvider()
{
}

uint TMSDataProvider::registerSource(TileDataProvider::TmsTaskSender taskSender)
{
//     uint sourceId = TileLoaderPool::instance()->registerSource(getTmsAdapter(), taskSender);
//     QMap<int, SavedImage> map;
//     m_map.insert(sourceId, map);
//     return sourceId;

    uint sourceId = TileLoaderPool::instance()->registerSource(getTmsAdapter(), taskSender);
    TmsProviderTasks tasks;
    m_taskFromSourece.insert(sourceId, tasks);
    return sourceId;
}


bool TMSDataProvider::unregisterSource(uint sourceId)
{
//    m_map.remove(sourceId);
//    return TileLoaderPool::instance()->unregisterSource(sourceId);

    auto iter(m_taskFromSourece.find(sourceId));
    if(iter != m_taskFromSourece.end())
    {
        iter.value().clear();
        m_taskFromSourece.erase(iter);
    }
    return TileLoaderPool::instance()->unregisterSource(sourceId);
}

int TMSDataProvider::tile(image_types::TImage& dest, int x, int y, int z, uint sourceId)
{
    if(z < minZoomLevel() || z > maxZoomLevel())
    {
        qDebug() << "TMSDataProvider::tile, Z:" << z << ", minZoomLevel:" << minZoomLevel() << ", maxZoomLevel:" << maxZoomLevel();
        return -1;
    }

//    const QString url(query(x,y,z));
//    const int OutKey(UniqueIdFactory::instance()->getListener());
//    SavedImage savedImage(&dest, OutKey, sourceId);
//    const int InKey(TileLoaderPool::instance()->setRequest(url, sourceId));
//    QMap<uint, QMap<int, SavedImage> >::iterator iter(m_map.find(sourceId));
//    if(iter != m_map.end())
//    {
//        iter.value().insert(InKey, savedImage);
//    }
//    return OutKey;


    const QString url(query(x,y,z).toString());

    // qDebug() << "TMSDataProvider::tile, url:" << url << ", maxZoomLevel:" << maxZoomLevel();

    const int taskId(UniqueIdFactory::instance()->getListener());
    auto it(m_taskFromSourece.find(sourceId));
    if(it != m_taskFromSourece.end())
    {
        it.value().insert(&dest, taskId, url);
    }
    TileLoaderPool::instance()->setRequest(url, sourceId);
    return taskId;
}


//void TMSDataProvider::slot_requestFinished(int inKey, uint sourceId, QByteArray * src, TileDataProvider::Result result)
//{
//    QMap<uint, QMap<int, SavedImage> >::iterator iter(m_map.find(sourceId));
//    if(iter != m_map.end())
//    {
//        QMap<int, SavedImage>::iterator it(iter.value().find(inKey));
//        if(it != iter.value().end())
//        {
//            const SavedImage savedImage(it.value());

//            if(result == TileDataProvider::Loaded)
//            {
//                if(src)
//                {
//                    QImage image = QImage::fromData(*src);
//                    image = image.convertToFormat(QImage::Format_ARGB32);
//                    if(image.size() == mTileSize)
//                    {
//                        image_types::TImage* dest(savedImage.img);
//                        dest->depth = image.depth();
//                        dest->w = image.width();
//                        dest->h = image.height();
//                        dest->prt = new char[dest->w * dest->h * (dest->depth / 8)];
//                        memcpy(dest->prt, image.constBits(), dest->w * dest->h * (dest->depth / 8));
//                    }

//                    emit tmsDataReady(savedImage.listener, sourceId, TileDataProvider::Loaded);
//                }
//                else
//                    emit tmsDataReady(savedImage.listener, sourceId, TileDataProvider::NotFound);
//            }
//            else
//                emit tmsDataReady(savedImage.listener, sourceId, result);

//            emit dataReady(savedImage.listener);
//            iter.value().erase(it);
//        }
//    }

////    qDebug() << "TMSDataProvider, m_InKey_TImage.size() :" << m_InKey_TImage.size();
//}
void TMSDataProvider::slot_requestFinished(QString url, uint sourceId, QByteArray * src, TileDataProvider::Result result)
{
    auto it(m_taskFromSourece.find(sourceId));
    if(it != m_taskFromSourece.end())
    {
        TmsProviderTask tmsProviderTask;
        if(it.value().getTask(url, tmsProviderTask))
        {
            if(result == TileDataProvider::Loaded)
            {
                if(src)
                {
                    QImage image = QImage::fromData(*src);
                    image = image.convertToFormat(QImage::Format_ARGB32);
                    if(image.size() == mTileSize)
                    {
                        image_types::TImage* dest(tmsProviderTask.img);
                        dest->depth = image.depth();
                        dest->w = image.width();
                        dest->h = image.height();
                        dest->prt = new char[dest->w * dest->h * (dest->depth / 8)];
                        memcpy(dest->prt, image.constBits(), dest->w * dest->h * (dest->depth / 8));
                        emit tmsDataReady(tmsProviderTask.taskId, sourceId, TileDataProvider::Loaded);
                    }
                    else
                        emit tmsDataReady(tmsProviderTask.taskId, sourceId, TileDataProvider::NotFound);
                }
                else
                    emit tmsDataReady(tmsProviderTask.taskId, sourceId, TileDataProvider::NotFound);
            }
            else
                emit tmsDataReady(tmsProviderTask.taskId, sourceId, result);
        }
        else
            emit tmsDataReady(tmsProviderTask.taskId, sourceId, TileDataProvider::NotFound);

        emit dataReady(tmsProviderTask.taskId);
    }
}


//void TMSDataProvider::abortTaskFromSource(uint tasksId, uint sourceId)
//{
//    QMap<uint, QMap<int, SavedImage> >::iterator iter(m_map.find(sourceId));
//    if(iter != m_map.end())
//    {
//        QMap<int, SavedImage>::iterator it(iter.value().begin());
//        while(it != iter.value().end())
//        {
//            QMap<int, SavedImage>::iterator currIt(it);
//            ++it;
//            if(currIt.value().listener == tasksId)
//            {
//                iter.value().erase(currIt);
//                TileLoaderPool::instance()->abortTaskFromSource(tasksId, sourceId);
//                break;
//            }
//        }
//    }
//}
void TMSDataProvider::abortTaskFromSource(uint tasksId, uint sourceId)
{
    auto it(m_taskFromSourece.find(sourceId));
    if(it != m_taskFromSourece.end())
    {
        TmsProviderTask tmsProviderTask;
        if(it.value().getTask(tasksId, tmsProviderTask))
        {
            TileLoaderPool::instance()->abortTaskFromSource(tmsProviderTask.url, sourceId);
        }
    }
}


//void TMSDataProvider::abortAllTaskFromSource(uint sourceId)
//{
//    QMap<uint, QMap<int, SavedImage> >::iterator iter(m_map.find(sourceId));
//    if(iter != m_map.end())
//    {
//        iter.value().clear();
//        TileLoaderPool::instance()->abortAllTaskFromSource(sourceId);
//    }
//}
void TMSDataProvider::abortAllTaskFromSource(uint sourceId)
{
    auto it(m_taskFromSourece.find(sourceId));
    if(it != m_taskFromSourece.end())
    {
        it.value().clear();
    }
    TileLoaderPool::instance()->abortAllTaskFromSource(sourceId);
}
