#include "conturtilemixer.h"
#include "intersectsscene.h"
#include <stdint.h>
#include <QDebug>
//#include <objrepr/reprServer.h>
//#include <objrepr/pframe.h>
#include "rasterlayer.h"
//#include <ctrcore/visual/pixelscenedescriptor.h>
#include <ctrrasterrender/tms/tilemapperfactory.h>
#include <ctrrasterrender/tms/tileloaderpool.h>
#include <QApplication>
#include <QFileInfo>
#include <QDir>
#include <QSettings>
#include <ctrcore/provider/dataproviderfactory.h>
#include <ctrcore/visual/visualizermanager.h>
#include <ctrcore/visual/selectionmodelinterface.h>
#include <ctrcore/visual/datainterface.h>
#include <ctrcore/visual/viewinterface.h>

using namespace visualize_system;
//using namespace objrepr;

ConturTileMixer::ConturTileMixer(uint visualiserId, QGraphicsScene *scene, SceneDescriptor &sceneDescriptor)
    : TileMixer(visualiserId, scene, sceneDescriptor)
    , m_timer(this)
    , ADD_RASTERS_TIMEOUT(200)
    , DELTA_Z_LEVEL(1.5)
    , MIN_LIST_SIZE(10)
    , m_blockDialog(0)
    , m_middleMinZlevel(-1)
{
}

ConturTileMixer::~ConturTileMixer()
{
    if(m_blockDialog)
        delete m_blockDialog;
}

QList<uint> ConturTileMixer::addProviders(const QList<uint> & prov_list)
{
    QList<uint> list;

    if(m_settingsPath.isEmpty())
    {
        if(m_sceneDescriptor.isGeoScene())
        {
            visualize_system::SelectionModelInterface * selectionModelInterface = visualize_system::VisualizerManager::instance()->getSelectionModelInterface(m_visualiserId);
            if(selectionModelInterface)
            {
//                connect(selectionModelInterface, SIGNAL(signalSelectedProvidersChanged(QList<uint>)), this, SLOT(slotSelectedProvidersChanged(QList<uint>)));
                connect(selectionModelInterface, SIGNAL(signalCurrentProviderChanged(uint,uint)), this, SLOT(slotCurrentProviderChanged(uint,uint)));
                _defaultRasterSelectionMode = selectionModelInterface->getDefaultRasterSelectionMode();
                connect(selectionModelInterface, SIGNAL(signalDefaultRasterSelectionModeChanged(visualize_system::DefaultRasterSelectionMode)),
                        this, SLOT(slotDefaultRasterSelectionModeChanged(visualize_system::DefaultRasterSelectionMode)));
            }

            visualize_system::DataInterface * dataInterface = visualize_system::VisualizerManager::instance()->getDataInterface(m_visualiserId);
            if(dataInterface)
                connect(dataInterface, SIGNAL(signalProviderProcessingSettingsChanged(uint, QString)), this, SLOT(slotProviderProcessingSettingsChanged(uint, QString)));

            visualize_system::ViewInterface * viewInterface = visualize_system::VisualizerManager::instance()->getViewInterface(m_visualiserId);
            if(viewInterface)
            {
                bool res = connect(this, SIGNAL(signalInitObjectMenu(QMenu &, quint64)), viewInterface, SIGNAL(signalInitObjectMenu(QMenu &, quint64)));
                qDebug() << "connect" << res;
            }
        }

        m_settingsPath = TileLoaderPool::instance()->settingsPath();
        QSettings settings(m_settingsPath, QSettings::NativeFormat);
        QVariant rasterProviderCache_absolutPath = settings.value("rasterProviderCachePath_absolutPath");
        if(rasterProviderCache_absolutPath.isValid() && rasterProviderCache_absolutPath.toString().isEmpty() == false)
        {
            QString val = rasterProviderCache_absolutPath.toString();
            QDir dir(val);
            if(dir.exists())
               RASTER_CACHE_PATH = val;
            else
            {
                if(dir.mkpath(val))
                    RASTER_CACHE_PATH = val;
                else
                    settings.setValue("rasterProviderCachePath_absolutPath", "");
            }
        }
        QVariant showRasterCachingWindow_rastersCount = settings.value("showRasterCachingWindow_rastersCount");
        if(showRasterCachingWindow_rastersCount.isValid())
        {
            bool ok;
            int val = showRasterCachingWindow_rastersCount.toInt(&ok);
            if(ok && val >=3 && val <= 100)
                MIN_LIST_SIZE = val;
            else
                settings.setValue("showRasterCachingWindow_rastersCount", 10);
        }
        QVariant minRasterZlevel_middleDelta = settings.value("minRasterZlevel_middleDelta");
        if(minRasterZlevel_middleDelta.isValid())
        {
            bool ok;
            double val = minRasterZlevel_middleDelta.toDouble(&ok);
            if(ok && val >=1. && val <= 5.)
                DELTA_Z_LEVEL = val;
            else
                settings.setValue("minRasterZlevel_middleDelta", 1.5);
        }
        QVariant groupRastersLoadingTimeout_mSec = settings.value("groupRastersLoadingTimeout_mSec");
        if(groupRastersLoadingTimeout_mSec.isValid())
        {
            bool ok;
            int val = groupRastersLoadingTimeout_mSec.toInt(&ok);
            if(ok && val >=50 && val <= 3000)
                ADD_RASTERS_TIMEOUT = val;
            else
                settings.setValue("groupRastersLoadingTimeout_mSec", 200);
        }
        QVariant onlyResultPixmap = settings.value("paintOnlyResultPixmap");
        if(onlyResultPixmap.isValid())
            m_onlyResultPixmap = onlyResultPixmap.toBool();
        else
            settings.setValue("paintOnlyResultPixmap", true);
    }

    m_loadingCacheList.clear();

    int W(0), H(0), lastW(-1), lastH(-1);
    bool isEquval(true);
    double totalMinZlevel(0);
    QList<TileMapper *> mapperList;
    foreach (uint providerId, prov_list)
    {
        QSharedPointer<data_system::RasterDataProvider> rdp = data_system::DataProviderFactory::instance()->getProvider(providerId).dynamicCast<data_system::RasterDataProvider>();
        if(rdp.isNull())
            continue;

        list.append(providerId);

        TileMapper * tileMapper(0);
        visualize_system::DataProviderProperty dp_new(tileMapper);
        if(m_sceneDescriptor.isGeoScene())
        {
//            if(rdp->getMatchingType() == data_system::MatchingType::Corners)
//            {
//                tileMapper = visualize_system::TileMapperFactory::createTileMapper(m_visualiserId, providerId, m_sceneDescriptor);
//                dp_new.dataProvider = tileMapper;
//                connect(tileMapper, SIGNAL(signal_paintTile(uint,const QString&,QPixmap&,int)), this, SLOT(paintTile(uint,const QString&,QPixmap&,int)));
//                connect(tileMapper, SIGNAL(signal_dataLoaded()), this, SLOT(slot_dataLoaded()));
//                connect(tileMapper, SIGNAL(signal_providerChanged()), this, SLOT(slot_providerChanged()));
//                dp_new.rasterLayer = new visualize_system::RasterLayer(providerId, tileMapper->getBoundingPolygone(), _rasterLayerInitData);
//                dp_new.rasterLayer->setDefaultPen(_defaultRasterSelectionMode == visualize_system::DefaultRasterSelectionMode::COLOR ? _rasterLayerInitData.defaultPen : QPen(QColor(Qt::transparent)));
//            }
//            else
//            {
//                data_system::Location location;
//                if( ! rdp->getLocation(location) )
//                    continue;

//                double X(location.GPSLongitude), Y(location.GPSLatitude);
//                if(m_sceneDescriptor.convertEpsgToScene(4326, X, Y))
//                    dp_new.rasterLayer = new visualize_system::RasterLayer(providerId, QPointF(X, Y), location.GPSImgDirection, location.orientation, _rasterLayerInitData);
//                else
//                    continue;
//            }
            connect(dp_new.rasterLayer, SIGNAL(signalClicked(bool)), this, SLOT(rasterLayerClicked(bool)));

            QList<QGraphicsItem*>* list = new QList<QGraphicsItem*>;
            list->append(dp_new.rasterLayer->getGraphicsItem());
            emit signalItemsForDraw(list);

            if(tileMapper)
            {
                m_intersectsScene->addPolygon(providerId, tileMapper->getBoundingPolygone(), false);
                totalMinZlevel += tileMapper->getMinZlevel();
                mapperList.append(tileMapper);
            }

            m_Providers.insert(providerId, dp_new);
        }
        else
        {
            tileMapper = visualize_system::TileMapperFactory::createTileMapper(m_visualiserId, providerId, m_sceneDescriptor);
            dp_new.dataProvider = tileMapper;
            connect(tileMapper, SIGNAL(signal_paintTile(uint,const QString&,QPixmap&,int)), this, SLOT(paintTile(uint,const QString&,QPixmap&,int)));
            connect(tileMapper, SIGNAL(signal_dataLoaded()), this, SLOT(slot_dataLoaded()));
            connect(tileMapper, SIGNAL(signal_providerChanged()), this, SLOT(slot_providerChanged()));

            // если это пиксельный миксер - то он может отображать одновременно только одно изображение !
            for(auto it = m_Providers.begin(); it != m_Providers.end(); ++it)
            {
                visualize_system::DataProviderProperty dpp(it.value());
                TileMapper* tileMapper = (TileMapper*)(dpp.dataProvider);
                delete dpp.rasterLayer;
                delete tileMapper;
            }

            if(tileMapper->isTMSProviderMapper() == false)
            {
                QRect boundingRect(dp_new.dataProvider->provider()->boundingRect());
                if(lastW != -1 && lastH != -1)
                {
                    if(lastW != boundingRect.width() || lastH != boundingRect.height())
                        isEquval = false;
                }
                lastW = boundingRect.width();
                lastH = boundingRect.height();

                if(H < boundingRect.height())
                    H = boundingRect.height();
                W = W + boundingRect.width() + 1;

                if(m_sceneDescriptor.isPixelScene())
                {
//                    QVariant frame = delegate->property("FrameId");
//                    if(frame.isValid())
//                    {
//                        qDebug() << "===> TileMixer, FrameId :" << frame.toInt();
//                        emit signalPixelCursorCreated(-1, frame.toInt());
                        emit initPixelScene(tileMapper->getFrameDelta(), tileMapper->getBaseZlevel(), -1/*frame.toInt()*/, -1, rdp->getProviderUrl().toLocalFile());
//                    }
                }
                
//                PixelSceneDescriptor* pixelSceneDescriptor = dynamic_cast<PixelSceneDescriptor*>(&m_sceneDescriptor);
//                if(pixelSceneDescriptor)
//                    pixelSceneDescriptor->init(rdp->geoImageObjectPtr());

                if(m_intersectsScene)
                    m_intersectsScene->clearScene();
                m_Providers.clear();
                m_Providers.insert(providerId, dp_new);
                emit updateContent(true);
//                emit signalProvidersAdded();
                return list; /// <------------------ !!! для пиксельной сцены только первый провайдер !!!
            }
        }
    }

    QString cachePath;
    if(RASTER_CACHE_PATH.isEmpty() == false)
    {
        cachePath = RASTER_CACHE_PATH;
    }
    else
    {
        /*
        QString homePath = QString(getenv("HOME"));
        QDir cachePathDir(homePath + QString(QDir::separator()) + QString("raster_provider_cache"));
        if(cachePathDir.exists())
        {
            cachePath = homePath + QString(QDir::separator()) + QString("raster_provider_cache");
        }
        else
        {
            QDir homePathDir(homePath);
            if(homePathDir.mkdir(QString("raster_provider_cache")))
            {
                cachePath = homePath + QString(QDir::separator()) + QString("raster_provider_cache");
            }
            else
            {
                qDebug() << "ConturTileMixer, cachePathDir - dont make dir :" << cachePathDir;
            }
        }
        */
    }

    const int LIST_SIZE(mapperList.size());
    if(LIST_SIZE > MIN_LIST_SIZE)
    {
        if(m_middleMinZlevel < 0)
            m_middleMinZlevel = QString::number(totalMinZlevel / LIST_SIZE, 'f', 0).toInt();

        foreach(TileMapper * mapper,  mapperList)
        {
            double delta = m_middleMinZlevel - mapper->getMinZlevel();
            if(delta < 0)
                delta *= -1;
            if( delta < DELTA_Z_LEVEL )
                mapper->setMinZlevel(m_middleMinZlevel);

            if(cachePath.isEmpty() == false)
                mapper->loadCache(cachePath);
        }

        if(cachePath.isEmpty() == false)
        {
            m_loadingCacheList = mapperList;
            QTimer::singleShot(100, this, SLOT(slotCheckLoadingCache()));
            m_stopLoadingCache = false;
            m_blockDialog = new BlockDialog(m_stopLoadingCache);
            m_blockDialog->show();
        }
    }
    else
    {
        if(cachePath.isEmpty() == false)
        {
            foreach(TileMapper * mapper,  mapperList)
                mapper->loadCache(cachePath);
        }
    }

    emit updateContent(true);
//    emit signalProvidersAdded();

    return list;
}

QList<uint> ConturTileMixer::addBaseProviders(const QList<uint> &prov_list)
{
    QList<uint> list;

    if(m_settingsPath.isEmpty())
    {
        if(m_sceneDescriptor.isGeoScene())
        {
            visualize_system::SelectionModelInterface * selectionModelInterface = visualize_system::VisualizerManager::instance()->getSelectionModelInterface(m_visualiserId);
            if(selectionModelInterface)
            {
//                connect(selectionModelInterface, SIGNAL(signalSelectedProvidersChanged(QList<uint>)), this, SLOT(slotSelectedProvidersChanged(QList<uint>)));
                connect(selectionModelInterface, SIGNAL(signalCurrentProviderChanged(uint,uint)), this, SLOT(slotCurrentProviderChanged(uint,uint)));
                _defaultRasterSelectionMode = selectionModelInterface->getDefaultRasterSelectionMode();
                connect(selectionModelInterface, SIGNAL(signalDefaultRasterSelectionModeChanged(visualize_system::DefaultRasterSelectionMode)),
                        this, SLOT(slotDefaultRasterSelectionModeChanged(visualize_system::DefaultRasterSelectionMode)));
            }

            visualize_system::DataInterface * dataInterface = visualize_system::VisualizerManager::instance()->getDataInterface(m_visualiserId);
            if(dataInterface)
                connect(dataInterface, SIGNAL(signalProviderProcessingSettingsChanged(uint, QString)), this, SLOT(slotProviderProcessingSettingsChanged(uint, QString)));

            visualize_system::ViewInterface * viewInterface = visualize_system::VisualizerManager::instance()->getViewInterface(m_visualiserId);
            if(viewInterface)
            {
                bool res = connect(this, SIGNAL(signalInitObjectMenu(QMenu &, quint64)), viewInterface, SIGNAL(signalInitObjectMenu(QMenu &, quint64)));
                qDebug() << "connect" << res;
            }
        }

        m_settingsPath = TileLoaderPool::instance()->settingsPath();
        QSettings settings(m_settingsPath, QSettings::NativeFormat);
        QVariant rasterProviderCache_absolutPath = settings.value("rasterProviderCachePath_absolutPath");
        if(rasterProviderCache_absolutPath.isValid() && rasterProviderCache_absolutPath.toString().isEmpty() == false)
        {
            QString val = rasterProviderCache_absolutPath.toString();
            QDir dir(val);
            if(dir.exists())
               RASTER_CACHE_PATH = val;
            else
            {
                if(dir.mkpath(val))
                    RASTER_CACHE_PATH = val;
                else
                    settings.setValue("rasterProviderCachePath_absolutPath", "");
            }
        }
        QVariant showRasterCachingWindow_rastersCount = settings.value("showRasterCachingWindow_rastersCount");
        if(showRasterCachingWindow_rastersCount.isValid())
        {
            bool ok;
            int val = showRasterCachingWindow_rastersCount.toInt(&ok);
            if(ok && val >=3 && val <= 100)
                MIN_LIST_SIZE = val;
            else
                settings.setValue("showRasterCachingWindow_rastersCount", 10);
        }
        QVariant minRasterZlevel_middleDelta = settings.value("minRasterZlevel_middleDelta");
        if(minRasterZlevel_middleDelta.isValid())
        {
            bool ok;
            double val = minRasterZlevel_middleDelta.toDouble(&ok);
            if(ok && val >=1. && val <= 5.)
                DELTA_Z_LEVEL = val;
            else
                settings.setValue("minRasterZlevel_middleDelta", 1.5);
        }
        QVariant groupRastersLoadingTimeout_mSec = settings.value("groupRastersLoadingTimeout_mSec");
        if(groupRastersLoadingTimeout_mSec.isValid())
        {
            bool ok;
            int val = groupRastersLoadingTimeout_mSec.toInt(&ok);
            if(ok && val >=50 && val <= 3000)
                ADD_RASTERS_TIMEOUT = val;
            else
                settings.setValue("groupRastersLoadingTimeout_mSec", 200);
        }
        QVariant onlyResultPixmap = settings.value("paintOnlyResultPixmap");
        if(onlyResultPixmap.isValid())
            m_onlyResultPixmap = onlyResultPixmap.toBool();
        else
            settings.setValue("paintOnlyResultPixmap", true);
    }

    m_loadingCacheList.clear();

//    double totalMinZlevel(0);
//    QList<TileMapper *> mapperList;
    foreach (uint providerId, prov_list)
    {
        QSharedPointer<data_system::RasterDataProvider> rdp = data_system::DataProviderFactory::instance()->getProvider(providerId).dynamicCast<data_system::RasterDataProvider>();
        if(rdp.isNull())
            continue;

        list.append(providerId);

        TileMapper * tileMapper = visualize_system::TileMapperFactory::createTileMapper(m_visualiserId, providerId, m_sceneDescriptor);
        visualize_system::DataProviderProperty dp_new(tileMapper);
        dp_new.baseCover = true;
        connect(tileMapper, SIGNAL(signal_paintTile(uint,const QString&,QPixmap&,int)), this, SLOT(paintTile(uint,const QString&,QPixmap&,int)));
        connect(tileMapper, SIGNAL(signal_dataLoaded()), this, SLOT(slot_dataLoaded()));
        connect(tileMapper, SIGNAL(signal_providerChanged()), this, SLOT(slot_providerChanged()));

        //QString providerName = tileMapper->provider()->getProviderUrl().toString();
        if(m_sceneDescriptor.isGeoScene())
        {
//            if( m_sceneDescriptor.refEpsgSRS() == 4326 )
//            {
//                if(tileMapper->provider()->spatialRect() == QRectF(QPointF(-90,-180),QPointF(90,180)))
//                {
                    m_intersectsScene->addPolygon( providerId,
                                                   QPolygonF( QRectF(0,0,m_sceneDescriptor.sceneSize().width(),m_sceneDescriptor.sceneSize().height())),
                                                   true );
//                }
//                else
                {
//                    m_intersectsScene->addPolygon(providerId, tileMapper->getBoundingPolygone(), true);

//                    dp_new.rasterLayer = new visualize_system::RasterLayer(providerId, tileMapper->getBoundingPolygone());
//                    connect(dp_new.rasterLayer, SIGNAL(signalSetActive()), this, SLOT(rasterLayerActivated()));
//                    connect(dp_new.rasterLayer, SIGNAL(signalClicked(bool)), this, SLOT(rasterLayerClicked(bool)));
//                    dp_new.rasterLayer->setSelectedColor(m_selectedPen.color());
//                    dp_new.rasterLayer->setDefaultColor(m_defaultPen.color());

//                    QList<QGraphicsItem*>* list = new QList<QGraphicsItem*>;
//                    list->append(dp_new.rasterLayer);
//                    emit signalItemsForDraw(list);

//                    totalMinZlevel += tileMapper->getMinZlevel();
//                    mapperList.append(tileMapper);
                }
//            }
//            else
//                m_intersectsScene->addPolygon(providerId, tileMapper->getBoundingPolygone(), true);

            m_Providers.insert(providerId, dp_new);
        }
    }

//    QString cachePath;
//    if(RASTER_CACHE_PATH.isEmpty() == false)
//    {
//        cachePath = RASTER_CACHE_PATH;
//    }
//    else
//    {
//        /*
//        QString homePath = QString(getenv("HOME"));
//        QDir cachePathDir(homePath + QString(QDir::separator()) + QString("raster_provider_cache"));
//        if(cachePathDir.exists())
//        {
//            cachePath = homePath + QString(QDir::separator()) + QString("raster_provider_cache");
//        }
//        else
//        {
//            QDir homePathDir(homePath);
//            if(homePathDir.mkdir(QString("raster_provider_cache")))
//            {
//                cachePath = homePath + QString(QDir::separator()) + QString("raster_provider_cache");
//            }
//            else
//            {
//                qDebug() << "ConturTileMixer, cachePathDir - dont make dir :" << cachePathDir;
//            }
//        }
//        */
//    }

//    const int LIST_SIZE(mapperList.size());
//    if(LIST_SIZE > MIN_LIST_SIZE)
//    {
//        if(m_middleMinZlevel < 0)
//            m_middleMinZlevel = QString::number(totalMinZlevel / LIST_SIZE, 'f', 0).toInt();

//        foreach(TileMapper * mapper,  mapperList)
//        {
//            double delta = m_middleMinZlevel - mapper->getMinZlevel();
//            if(delta < 0)
//                delta *= -1;
//            if( delta < DELTA_Z_LEVEL )
//                mapper->setMinZlevel(m_middleMinZlevel);

//            if(cachePath.isEmpty() == false)
//                mapper->loadCache(cachePath);
//        }

//        if(cachePath.isEmpty() == false)
//        {
//            m_loadingCacheList = mapperList;
//            QTimer::singleShot(100, this, SLOT(slotCheckLoadingCache()));
//            m_stopLoadingCache = false;
//            m_blockDialog = new BlockDialog(m_stopLoadingCache);
//            m_blockDialog->show();
//        }
//    }
//    else
//    {
//        if(cachePath.isEmpty() == false)
//        {
//            foreach(TileMapper * mapper,  mapperList)
//                mapper->loadCache(cachePath);
//        }
//    }

    emit updateContent(true);
//    emit signalProvidersAdded();

    return list;
}

void ConturTileMixer::slotCheckLoadingCache()
{
    if(m_stopLoadingCache)
    {
        m_blockDialog->hide();
        delete m_blockDialog;
        m_blockDialog = 0;

        m_loadingCacheList.clear();
    }
    else
    {
        double onePercent = 100. / m_loadingCacheList.size();
        int readyMapper(0);
        foreach(TileMapper * tileMapper, m_loadingCacheList)
            if(tileMapper->cacheLoaded())
                ++readyMapper;

        int percent = readyMapper * onePercent;
        m_blockDialog->setPercent(percent);
        if(readyMapper == m_loadingCacheList.size())
        {
            m_blockDialog->hide();
            delete m_blockDialog;
            m_blockDialog = 0;

            m_loadingCacheList.clear();
        }
        else
        {
            QTimer::singleShot(100, this, SLOT(slotCheckLoadingCache()));
        }
    }
    qApp->processEvents();
}

/*
void ConturTileMixer::slotGeoModelSelectionChanged(const QItemSelection & item_selection, const QItemSelection &)
{
    if(item_selection.count() > 0 && item_selection.indexes().count() > 0)
    {
        QModelIndex index = Contur::system()->model()->getZeroColumnIndexFromListOfIndexes(item_selection.indexes());
        QStandardItem* item = Contur::system()->model()->itemFromIndex(index);

        if(item)
        {
            for(QMap<QString,visualize_system::RasterLayer *>::iterator it(m_altMatrixes.begin()); it != m_altMatrixes.end(); ++it)
                if(it.value()->isLayerSelected())
                    it.value()->setLayerSelected(false);

            if(item->data(ConturModel::PresentationRole).isValid() && item->data(ConturModel::PresentationRole) == ConturModel::DigitalRelief)
            {
                QString path = item->data(ConturModel::ItemUrlRole).toUrl().path();
                QMap<QString,visualize_system::RasterLayer *>::iterator it(m_altMatrixes.find(path));
                if(it != m_altMatrixes.end())
                    it.value()->setLayerSelected(true);
            }

            if(item->data(ConturModel::PresentationRole).isValid() &&
                    (item->data(ConturModel::PresentationRole) == ConturModel::RootLayer ||
                     item->data(ConturModel::PresentationRole) == ConturModel::ProviderImage ||
                     item->data(ConturModel::PresentationRole) == ConturModel::ProviderWMS ||
                     item->data(ConturModel::PresentationRole) == ConturModel::ProviderTMS))
            {
                QList<RasterDataProvider*> listProviders = Contur::system()->model()->getListOfProvidersInItem(item);
                if(!listProviders.isEmpty())
                {
                    foreach (RasterDataProvider *rdp, listProviders)
                    {
                        for(auto it = m_Providers.begin(); it != m_Providers.end(); ++it)
                        {
                            if(it.value().dataProvider->provider() == rdp)
                            {
                                if(it.value().rasterLayer)
                                {
                                    it.value().rasterLayer->setLayerSelected(true);
//                                    m_intersectsScene->setProviderOnTop(it.key());
                                }
                            }
                            else
                            {
                                if( !(listProviders.contains((*it).dataProvider->provider())) )
                                    if(it.value().rasterLayer)
                                        it.value().rasterLayer->setLayerSelected(false);
                            }
                        }
                    }
                }
            }
            else
            {
                //deselect all
                for(auto it = m_Providers.begin(); it != m_Providers.end(); ++it)
                    if(it.value().rasterLayer)
                        it.value().rasterLayer->setLayerSelected(false);
            }
        }
    }
}
*/

void ConturTileMixer::slotPropertyChanged(QString title, QVariant value)
{
    /*
    ProviderDelegate * delegate = (ProviderDelegate *)(sender());
    propertyChanged(title, value, delegate->provider());
    */
}

void ConturTileMixer::rasterLayerActivated()
{
    /*
    visualize_system::RasterLayer* rasterLayer = (visualize_system::RasterLayer*)(sender());
    rasterLayerActivated(rasterLayer);
    */
}

void ConturTileMixer::rasterLayerClicked(bool withControl)
{
    visualize_system::RasterLayer* rasterLayer = (visualize_system::RasterLayer*)(sender());
    for(auto it = m_Providers.begin(); it != m_Providers.end(); ++it)
        if(it.value().rasterLayer == rasterLayer)
        {
            uint ID = it.key();
            QList<uint> selectedProviders;
            selectedProviders.append(ID);

            visualize_system::SelectionModelInterface * selectionModelInterface = visualize_system::VisualizerManager::instance()->getSelectionModelInterface(m_visualiserId);
            if(selectionModelInterface)
            {
                if(withControl)
                {
                    QSet<uint> set(selectionModelInterface->getSelectedProviders().toSet());
                    if(set.contains(ID))
                        selectionModelInterface->clearCurrentProvider();
                    else
                        selectionModelInterface->setCurrentProvider(ID);

                    selectionModelInterface->reversSelection(selectedProviders);
                }
                else
                {
                    selectionModelInterface->clearSelectProviders();
                    selectionModelInterface->setSelectProviders(true, selectedProviders);
                    selectionModelInterface->setCurrentProvider(ID);
                }
            }

            return;
        }
}

void ConturTileMixer::rasterLayerActivated(visualize_system::RasterLayer* rasterLayer)
{
    /*
    for(auto it = m_Providers.begin(); it != m_Providers.end(); ++it)
        if(it.value().rasterLayer == rasterLayer)
        {
            it.key()
            m_intersectsScene->setProviderOnTop(send_url.toString());
            return;
        }

    for(QMap<QString,visualize_system::RasterLayer *>::iterator it(m_altMatrixes.begin()); it != m_altMatrixes.end(); ++it)
        if(it.value() == rasterLayer)
        {
            QUrl send_url(it.key());
            if(QFileInfo(send_url.toString(QUrl::RemoveScheme)).isFile())
                send_url.setScheme("file");
            Contur::system()->model()->slotSetSelectedThisUrlOnAltMatrixes(send_url);
            return;
        }
     */
}






