#include "tilemapper.h"
#include <math.h>
#include <QDebug>
#include <QImage>
#include <QSettings>
#include <QPixmap>
//#include <QBitmap>
//#include <QPainter>
#include <ctrcore/provider/timagecache.h>
//#include <dpf/improc/filter.h>
//#include <dpf/io/base/image_object.h>
//#include <dpf/io/base/image_io.h>
//#include <dpf/core/data_types/image.h>
//#include "dpf/improc/improc_transform.h"
//#include <dpf/improc/filter.h>

using namespace visualize_system;

TileMapper::TileMapper(uint visualizerId, uint providerId, const SceneDescriptor & _sceneDescriptor)
    : AbstractTileMapper(providerId, _sceneDescriptor)
    , baseZlevel(1)
    , tasksInProcessingsPool(0)
    , processingPoolSenderId( (uint64_t)this )
    , MAX_Z_LEVEL(25)
    , MIN_Z_LEVEL(0)
    , VISUALIZER_ID(visualizerId)
    , PROVIDER_ID(providerId)
    , _hasProcessing(false)
    , _mapperViewMode(MapperViewMode::SHOW_BORDER_AND_RASTER)
{
    // PROVIDER_URL(providerId->getProviderUrl().toString())
//    connect(ProcessingPool::instance(), SIGNAL(taskCompleted(RuntimeTask*)), this, SLOT(slotTaskCompleted(RuntimeTask*)));
}

TileMapper::~TileMapper()
{
//    ProcessingPool::instance()->removeTasks(processingPoolSenderId);
}

//bool TileMapper::setHisto(const QString& title, const QVariant& value)
//{
//    bool RET(false);

//    QList<QVariant>list(value.toList());
//    if(title == "bricon")
//    {
//        if(list[0].toUInt() == m_THisto.brightness && list[1].toUInt() == m_THisto.contrast && list[2].toUInt() == m_THisto.min && list[3].toUInt() == m_THisto.max)
//            return false;

//        if(list[0].toUInt() == 0 && list[1].toUInt() == 0)
//            m_THisto = THisto();
//        else
//            m_THisto = THisto(title, list[0].toUInt(), list[1].toUInt(), list[2].toUInt(), list[3].toUInt());
//    }
//    else /*if(title == "")*/
//    {
//        if(list[0].toUInt() == m_THisto.left && list[1].toUInt() == m_THisto.mid && list[2].toUInt() == m_THisto.right && list[3].toUInt() == m_THisto.min && list[4].toUInt() == m_THisto.max)
//            return false;

//        if(list[0].toUInt() == 0 && list[1].toUInt() == 127 && list[2].toUInt() == 255)
//            m_THisto = THisto();
//        else
//            m_THisto = THisto(title, list[0].toUInt(), list[1].toUInt(), list[2].toUInt(), list[3].toUInt(), list[4].toUInt());
//    }

//    tasksInProcessingsPool = 0;
////    qDebug() << "===> TileMapper::setHisto, algorithmsPoolKey :" << algorithmsPoolKey;
//    ProcessingPool::instance()->removeTasks(processingPoolSenderId);

//    return RET;
//}

//void TileMapper::slotTaskCompleted(RuntimeTask* task)
//{
//    if(task->senderId != processingPoolSenderId)
//        return;

//    --tasksInProcessingsPool;

//    TImageCache::instance()->insert(task->cacheKey, task->img);

//    QImage _img(QSize(task->img->w, task->img->h), QImage::Format_ARGB32);
//    memcpy(_img.bits(), task->img->prt, task->img->depth / 8 * task->img->w * task->img->h);
//    QPixmap pm(QPixmap::fromImage(_img));

////    qDebug() << "18 ===> TileMapper QPixmap()" << task->key;
//    emit signal_paintTile(PROVIDER_ID, task->key, pm, ORIGINAL);

//    if(dataLoaded())
//    {
////        qDebug() << "+++ TileMapper::slotTaskCompleted ===> emit signal_dataLoaded()";
//        emit signal_dataLoaded();
//    }
////    else
////        qDebug() << "--- TileMapper::slotTaskCompleted";
//}

QPoint TileMapper::getTile(const QPointF wgsPos, int Z)
{
    double POW(pow(2,Z-1));
    const qulonglong W(sceneDescriptor.sceneSize().width() * POW);
    const qulonglong H(sceneDescriptor.sceneSize().height() * POW);

    double posX( (wgsPos.y() + 180 ) * W/360.0 );
    double posY( ( -wgsPos.x() + 90) * H/180.0 );

    int X = posX / sceneDescriptor.tileSize().width();
    int Y = posY / sceneDescriptor.tileSize().height();

//    qDebug() << "W" << W << "posX" << posX << "X" << X;
//    qDebug() << "H" << H << "posY" << posY << "Y" << Y;

    return QPoint(X,Y);
}

int TileMapper::getMinZlevel()
{
    return MIN_Z_LEVEL;
}

void TileMapper::setMapperViewMode(MapperViewMode mapperViewMode)
{
    _mapperViewMode = mapperViewMode;
}

MapperViewMode TileMapper::mapperViewMode()
{
    return _mapperViewMode;
}

void TileMapper::setMinZlevel(int minZlevel)
{
//    qDebug() << "*** TileMapper::setMinZlevel:" << minZlevel;
    MIN_Z_LEVEL = minZlevel;
}

QRectF TileMapper::tileSceneRect(int x, int y, int z)
{
    double POW(pow(2,z-1));
    double tileWOnView(sceneDescriptor.tileSize().width() / POW);
    double tileHOnView(sceneDescriptor.tileSize().height() / POW);
    return QRectF( x * tileWOnView, y * tileHOnView, tileWOnView, tileHOnView );
}

void TileMapper::setCurrentViewParams(int currentZ, QRectF tilesOnViewport)
{
    queryArea.z = currentZ;
    queryArea.rect = tilesOnViewport.toRect();
    queryArea.rect = QRect(queryArea.rect.x(), queryArea.rect.y(), queryArea.rect.width()+1, queryArea.rect.height()+1);
}

QString TileMapper::getProviderUrl()
{
    return PROVIDER_URL;
}
uint TileMapper::getProviderId()
{
    return PROVIDER_ID;
}

uint TileMapper::getVisualizerId()
{
    return VISUALIZER_ID;
}

void TileMapper::setHasProcessing(bool hasProcessing)
{
    _hasProcessing = hasProcessing;
}





