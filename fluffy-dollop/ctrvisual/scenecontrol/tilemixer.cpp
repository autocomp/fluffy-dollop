#include "tilemixer.h"
#include "rasterlayer.h"
#include "intersectsscene.h"
#include "rastercontrolwidget.h"
#include "parallelcursor.h"
#include "matchingwidget.h"
#include <stdint.h>
//#include <dpf/geo/geoimage_io.h>
//#include <ctrcore/visual/pixelscenedescriptor.h>
#include <ctrcore/visual/visualizermanager.h>
#include <ctrcore/provider/dataproviderfactory.h>
#include <ctrcore/provider/rasterdataprovider.h>
#include <ctrrasterrender/tms/tileloaderpool.h>
#include <ctrrasterrender/tms/rastertilemapper.h>
#include <ctrcore/visual/datainterface.h>
#include <ctrcore/visual/selectionmodelinterface.h>
#include <ctrcore/visual/viewinterface.h>
#include <ctrcore/visual/pixelvisualizersinterface.h>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>
#include <ctrrasterrender/tms/tilemapperfactory.h>

#include <QPainter>
#include <QImage>
#include <QRect>
#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsPolygonItem>
#include <QFileInfo>
#include <QBitmap>
#include <QTime>
#include <QSettings>
#include <QMenu>
#include <QCursor>

#include <cmath>

//using namespace dpf::geo;
//using namespace dpf::io;
using namespace visualize_system;

TileMixer::TileMixer(uint visualiserId, QGraphicsScene *scene, SceneDescriptor &sceneDescriptor)
    : AbstractTileMixer(visualiserId, scene, sceneDescriptor),
      m_Timer(this),
      m_blockFinishLoadind(false),
      MAX_RASTER_PROVIDERS_IN_TAIL(100),
      UPDATE_VIEWPORT_BY_TIMER_MSEC(1000),
      m_intersectsScene(0),
      m_lastCurrentZ(1),
      m_onlyResultPixmap(true),
      m_rasterControlWidget(0),
      m_matchingWidget(0),
      m_rasterMenu(0)
//      m_objectMenu(0)
{
    if(sceneDescriptor.isGeoScene())
    {
        m_intersectsScene = new IntersectsScene;

        QVariant MAX_RASTER_PROVIDERS_IN_TAIL_var = CtrConfig::getValueByName(QString("visualizer_system_settings.maxRasterProvidersInTile"));
        if(MAX_RASTER_PROVIDERS_IN_TAIL_var.isValid())
            MAX_RASTER_PROVIDERS_IN_TAIL = MAX_RASTER_PROVIDERS_IN_TAIL_var.toUInt();
        else
            CtrConfig::setValueByName(QString("visualizer_system_settings.maxRasterProvidersInTile"), MAX_RASTER_PROVIDERS_IN_TAIL);

        QVariant partOfSide_var = CtrConfig::getValueByName(QString("visualizer_system_settings.rasterLayer_partOfSideForMarkTriangle"));
        if(partOfSide_var.isValid())
        {
            if(partOfSide_var.toUInt() > 3 && partOfSide_var.toUInt() < 30)
                _rasterLayerInitData.partOfSide = partOfSide_var.toUInt();
            else
                CtrConfig::setValueByName(QString("visualizer_system_settings.rasterLayer_partOfSideForMarkTriangle"), _rasterLayerInitData.partOfSide);
        }
        else
            CtrConfig::setValueByName(QString("visualizer_system_settings.rasterLayer_partOfSideForMarkTriangle"), _rasterLayerInitData.partOfSide);

        loadingRasterLayerInitData();

        CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::MatchProvider, this, SLOT(slotMatchProvider(QVariant)),
                                          qMetaTypeId< quint64 >(),
                                          QString("visualize_system") );

        connect(data_system::DataProviderFactory::instance(), SIGNAL(providerChanged(uint)), this, SLOT(slotProviderChanged(uint)));
    }

    connect(&m_Timer, SIGNAL(timeout()), this, SLOT(slot_reloadViewport()));
}

TileMixer::~TileMixer()
{
    for(auto it = m_Providers.begin(); it != m_Providers.end(); ++it)
    {
        delete it.value().dataProvider;
        delete it.value().rasterLayer;
    }

    if(m_intersectsScene)
        delete m_intersectsScene;
    if(m_rasterControlWidget)
        delete m_rasterControlWidget;
}

void TileMixer::addAltMatrix(const QString &path)
{
//    std::string file_path(path.toLocal8Bit());
//    GeoImageObject * geoImageObject = new GeoImageObject(GeoImageObject::open(file_path, dpf::io::ReadOnly));
//    if(geoImageObject->is_open())
//        if(geoImageObject->is_matched())
//        {
//            dpf::PolygonF pixelPoly(geoImageObject->image_rect());
//            dpf::PolygonF geoPoly = geoImageObject->pixel_to_geo(pixelPoly);

//            QPolygonF pol;
//            pol.append(geoToScene(QPointF(geoPoly.at(0).y(),geoPoly.at(0).x())));
//            pol.append(geoToScene(QPointF(geoPoly.at(1).y(),geoPoly.at(1).x())));
//            pol.append(geoToScene(QPointF(geoPoly.at(2).y(),geoPoly.at(2).x())));
//            pol.append(geoToScene(QPointF(geoPoly.at(3).y(),geoPoly.at(3).x())));
//            visualize_system::RasterLayer * rasterLayer = new visualize_system::RasterLayer(pol);
////            rasterLayer->setSelectedPen(m_selectedPen);
////            rasterLayer->setDefaultPen(m_defaultPen);
//            connect(rasterLayer, SIGNAL(signalSetActive()), this, SLOT(rasterLayerActivated()));
//            m_altMatrixes.insert(path, rasterLayer);

//            QList<QGraphicsItem*>* list = new QList<QGraphicsItem*>;
//            list->append(rasterLayer);
//            emit signalItemsForDraw(list);
//        }
}

void TileMixer::removeAltMatrix(const QString &path)
{
    QMap<QString,visualize_system::RasterLayer *>::iterator it(m_altMatrixes.find(path));
    if(it != m_altMatrixes.end())
    {
        delete it.value();
        m_altMatrixes.erase(it);
    }
}

QList<uint> TileMixer::addProviders(const QList<uint> & prov_list)
{
    QList<uint> list;
    return list;
}

QList<uint> TileMixer::addBaseProviders(const QList<uint> &providers)
{
    QList<uint> list;
    return list;
}

QList<uint> TileMixer::removeProviders(const QList<uint> & prov_list)
{
    QList<uint> list;
    foreach(uint providerId, prov_list)
    {
        auto it = m_Providers.find(providerId);
        if(it != m_Providers.end())
        {
            list.append(providerId);

            visualize_system::DataProviderProperty dpp(it.value());
            TileMapper* tileMapper = (TileMapper*)(dpp.dataProvider);
            delete dpp.rasterLayer;
            delete tileMapper;
            m_Providers.erase(it);
        }
        if(m_intersectsScene)
            m_intersectsScene->removePolygon(providerId);
    }
    emit updateContent(true);
    return list;
}

/*
QList<data_system::RasterDataProvider*> TileMixer::removeProviders(QList<data_system::RasterDataProvider*> &prov_list)
{
    abortLoading();
    QList<RasterDataProvider*> listRemovedProviders;
    foreach(data_system::RasterDataProvider* rdp, prov_list)
    {
        auto it = m_Providers.find(rdp->getProviderUrl().toString());
        if(it != m_Providers.end())
        {
            listRemovedProviders.append(rdp);

            visualize_system::DataProviderProperty dpp(it.value());
            TileMapper* tileMapper = (TileMapper*)(dpp.dataProvider);
            delete dpp.rasterLayer;
            delete tileMapper;
            m_Providers.erase(it);
        }
        if(m_intersectsScene)
            m_intersectsScene->removePolygon(rdp->getProviderUrl().toString());
    }
    emit updateContent(true);
    return listRemovedProviders;
}

void TileMixer::removeProviders()
{
    abortLoading();
    for(auto it = m_Providers.begin(); it != m_Providers.end(); ++it)
    {
        visualize_system::DataProviderProperty dpp(it.value());
        TileMapper* tileMapper = (TileMapper*)(dpp.dataProvider);
        delete dpp.rasterLayer;
        delete tileMapper;
    }
    if(m_intersectsScene)
        m_intersectsScene->clearScene();
    m_Providers.clear();
    emit updateContent(true);
}
*/

void TileMixer::concrete_abortLoading()
{
    for(auto it = m_Providers.begin(); it != m_Providers.end(); ++it)
        if(it.value().dataProvider)
            it.value().dataProvider->abort();

//    qDebug() << "concrete_abortLoading -----------------------";
//    for(auto it = m_Cells.begin(); it != m_Cells.end(); ++it)
//    {
//        qDebug() << "cell :" << it.key();
//        TileMixerCell cell = it.value();
//        cell.debug();
//        qDebug() << "------------";
//    }
//    qDebug() << "=============================================";

    m_Cells.clear();
}

void TileMixer::concrete_removeTiles(const QStringList& list)
{
    //    qDebug() << "------------";
    //    foreach(QString key, list)
    //        qDebug() << "---" << key;
    //    qDebug() << "------------";

    //    return;

    //    qDebug() << "---> TileMixer::concrete_removeTiles, m_Cells.size :" << m_Cells.size();

        if(m_Cells.isEmpty() == false)
        {
    //        for(auto it = m_Cells.begin(); it != m_Cells.end(); ++it)
    //            qDebug() <<it.key();
    //        qDebug() << "------------";

            foreach (QString key, list)
            {
    //            qDebug() << "===> DELETE :" << key;
                auto it = m_Cells.find(key);
                if(it != m_Cells.end())
                {
                    TileMixerCell cell = it.value();
                    foreach(uint provId, cell.getLayersName())
                    {
                        auto provIt = m_Providers.find(provId);
                        if(provIt != m_Providers.end())
                        {
                            TileMapper* mapper = dynamic_cast<TileMapper*>(provIt.value().dataProvider);
                            if(mapper)
                            {
    //                            qDebug() << "key :" << key << ", prov find :" << provUrl;
                                mapper->abort(key);
                            }
                        }
                    }
                    m_Cells.erase(it);
    //                qDebug() << "===> concrete_removeTiles, erase cell :" << key;
                }
            }
//            qDebug() << "<--- TileMixer::concrete_removeTiles, m_Cells.size :" << m_Cells.size();

    //        for(auto it = m_Cells.begin(); it != m_Cells.end(); ++it)
    //        {
    //            qDebug() << "cell :" << it.key();
    //            TileMixerCell cell = it.value();
    //            foreach(QString provName, cell.getLayersName())
    //                qDebug() << "provName :" << provName;
    //            qDebug() << "------";
    //        }
    //        qDebug() << "====================";
        }
}

void TileMixer::concrete_getTiles(const QList<visualize_system::URL> list, int currentZ, QRectF tilesOnViewport)
{
    m_blockFinishLoadind = true;

    if(m_lastCurrentZ != currentZ)
    {
        m_Timer.stop();
        m_lastCurrentZ = currentZ;
    }
    else
    {
        m_Timer.stop();
        m_Timer.start(UPDATE_VIEWPORT_BY_TIMER_MSEC);
    }

    if(m_Providers.isEmpty())
    {
        foreach(visualize_system::URL url, list)
            emit drawTile(QPixmap(), url.returnId);
        return;
    }

    if(m_sceneDescriptor.isGeoScene())
    {
        QPointF lastScenePos = tileSceneRect(tilesOnViewport.center().x(), tilesOnViewport.center().y(), currentZ).center();
        CtrConfig::setValueByName(QString("visualizer_system_settings.lastGeoscenePosition_ScreenCenterX"), QString::number(lastScenePos.x(), 'f', 10) );
        CtrConfig::setValueByName(QString("visualizer_system_settings.lastGeoscenePosition_ScreenCenterY"), QString::number(lastScenePos.y(), 'f', 10) );
        CtrConfig::setValueByName(QString("visualizer_system_settings.lastGeoscenePosition_ZLevel"), m_lastCurrentZ);

        for(auto it =  m_Providers.begin(); it != m_Providers.end(); ++it)
        {
            visualize_system::DataProviderProperty prop = it.value();
            TileMapper* mapper = dynamic_cast<TileMapper*>(prop.dataProvider);
            if(mapper)
                mapper->setCurrentViewParams(currentZ, tilesOnViewport);
        }

        foreach(visualize_system::URL url, list)
        {


            QList<uint> baseProviders, otherProviders;
            m_intersectsScene->getProvidersInTile(tileSceneRect(url.x, url.y, url.z), baseProviders, otherProviders, MAX_RASTER_PROVIDERS_IN_TAIL);
            if(baseProviders.isEmpty() && otherProviders.isEmpty())
            {
                emit drawTile(QPixmap(), url.returnId);
            }
            else
            {
                QList<TileMapper*> mappersInCell;
                TileMixerCell tileMixerCell;

                foreach(uint id, baseProviders)
                {
                    auto it = m_Providers.find(id);
                    if(it != m_Providers.end())
                    {
                        visualize_system::DataProviderProperty prop = it.value();
                        TileMapper* mapper = dynamic_cast<TileMapper*>(prop.dataProvider);
                        if(mapper)
                            if(url.z >= mapper->getMinZlevel())
                            {
                                tileMixerCell.addLayer(id, prop.transparency, prop.clearBlackArea, true);
                                mappersInCell.append(mapper);
                                //qDebug() << "--->" << url.x << url.y << url.z << "baseCover, id" << id;
                            }
                    }
                }

                if(currentZ == url.z)
                    foreach(uint id, otherProviders)
                    {
                        auto it = m_Providers.find(id);
                        if(it != m_Providers.end())
                        {
                            visualize_system::DataProviderProperty prop = it.value();
                            TileMapper* mapper = dynamic_cast<TileMapper*>(prop.dataProvider);
                            if(mapper)
                                if(url.z >= mapper->getMinZlevel())
                                {
                                    tileMixerCell.addLayer(id, prop.transparency, prop.clearBlackArea, false);
                                    mappersInCell.append(mapper);
                                    //qDebug() << "layer, id" << id;
                                }
                        }
                    }

                if(tileMixerCell.isEmpty())
                {
                    emit drawTile(QPixmap(), url.returnId);
                }
                else
                {
                    m_Cells.insert(url.returnId, tileMixerCell);

                    QList<visualize_system::URL> currentUrl;
                    currentUrl.append(url);

                    foreach(TileMapper* mapper, mappersInCell)
                        mapper->loadTiles(currentUrl);
                }
            }

            /*
            QList<uint> listIn = m_intersectsScene->check(tileSceneRect(url.x, url.y, url.z), currentZ != url.z, MAX_RASTER_PROVIDERS_IN_TAIL);
//            qDebug() << url.z << url.x << url.y <<  " ===> " << listIn.size();
            if(listIn.isEmpty() == false)
            {
                QList<TileMapper*> mappersInCell;
                TileMixerCell tileMixerCell;
                foreach(uint id, listIn)
                {
                    auto it = m_Providers.find(id);
                    if(it != m_Providers.end())
                    {
                        visualize_system::DataProviderProperty prop = it.value();
                        TileMapper* mapper = dynamic_cast<TileMapper*>(prop.dataProvider);
                        if(mapper)
                            if(url.z >= mapper->getMinZlevel())
                            {
                                tileMixerCell.addLayer(id, prop.transparency, prop.clearBlackArea);
                                mappersInCell.append(mapper);
                            }
                    }
                }

                if(tileMixerCell.isEmpty())
                {
                    emit drawTile(QPixmap(), url.returnId);
                }
                else
                {
                    m_Cells.insert(url.returnId, tileMixerCell);

                    QList<visualize_system::URL> currentUrl;
                    currentUrl.append(url);

                    foreach(TileMapper* mapper, mappersInCell)
                        mapper->loadTiles(currentUrl);
                }
            }
            else
            {
                emit drawTile(QPixmap(), url.returnId);
            }
            */
        }
//        qDebug() << "---------------------" << m_Cells.size();
    }
    else // это для пиксельного единственного провайдера в пиксельном визуализаторе.
    {
        visualize_system::DataProviderProperty prop = m_Providers.begin().value();
        TileMapper* mapper = dynamic_cast<TileMapper*>(prop.dataProvider);
        if(mapper)
            mapper->setCurrentViewParams(currentZ, tilesOnViewport);

        foreach(visualize_system::URL url, list)
        {
            QList<visualize_system::URL> currentUrl;
            currentUrl.append(url);
            mapper->loadTiles(currentUrl);
        }
    }

    m_blockFinishLoadind = false;
    slot_dataLoaded();
}

void TileMixer::paintTile(uint providerId, const QString &OUTKey, QPixmap& pixmap, int tileType)
{
    if(m_sceneDescriptor.isGeoScene())
    {
        auto it = m_Cells.find(OUTKey);
        if(it != m_Cells.end())
        {
            it.value().setLayerPixmap(providerId, pixmap, ((TileMapper::TileType)(tileType)));

            QPixmap resultPixmap;

            TileMapper::TileType cellType(TileMapper::INVALID);
//            if(m_onlyResultPixmap)
            {
                //qDebug() << "TileMixer::paintTile, OUTKey:" << OUTKey << ", providerUrl:" << providerUrl;
                cellType = it.value().getResultPixmap(resultPixmap); //, OUTKey);
            }
//            else
//                cellType = it.value().getPixmap(resultPixmap);
            switch(cellType)
            {
            case TileMapper::INVALID :{
//                qDebug() << "paintTile" << OUTKey << "INVALID";
            }break;
            case TileMapper::ORIGINAL :{
//                qDebug() << "paintTile" << OUTKey << "ORIGINAL";
                emit drawTile(resultPixmap, OUTKey);
                m_Cells.erase(it);
            }break;
            case TileMapper::SCALED :{
//                qDebug() << "paintTile" << OUTKey << "SCALED";
                emit drawTile(resultPixmap, OUTKey);
            }break;
            }
        }
    }
    else
    {
        emit drawTile(pixmap, OUTKey);
    }
}

void TileMixer::slotProviderPropertyChanged(uint providerId, QString propertyName)
{
    auto it = m_Providers.find(providerId);
    if(it == m_Providers.end())
        return;

    visualize_system::DataInterface * dataInterface = visualize_system::VisualizerManager::instance()->getDataInterface(m_visualiserId);
    QVariant value;
    if(dataInterface->getProviderViewProperty(providerId, propertyName, value) == false)
        return;

    bool up(false);
    if(propertyName == "transparency" && m_sceneDescriptor.isGeoScene())
    {
        int percentTransparency(value.toInt());
        if(percentTransparency >= 0 && percentTransparency <= 100)
        {
            it.value().setTransparency(percentTransparency);
            m_intersectsScene->setTransparency(providerId, percentTransparency);
            up = true;
        }
    }
    else if(propertyName == "visibility" && m_sceneDescriptor.isGeoScene())
    {
        bool visible(value.toBool());
        if(it.value().isVisible != visible)
        {
            it.value().setVisible(visible);
            if(it.value().rasterLayer)
                it.value().rasterLayer->setVisible(visible);
            m_intersectsScene->setVisibility(providerId, visible);
            up = true;
        }
    }
    else if(propertyName == "show_only_border" && m_sceneDescriptor.isGeoScene())
    {
        TileMapper* tileMapper = (TileMapper*)((*it).dataProvider);
        if(tileMapper)
        {
            bool on_off(value.toBool());
            if(on_off && tileMapper->mapperViewMode() != MapperViewMode::SHOW_ONLY_BORDER)
            {
                tileMapper->setMapperViewMode(MapperViewMode::SHOW_ONLY_BORDER);
                up = true;
            }
            else if(on_off == false && tileMapper->mapperViewMode() == MapperViewMode::SHOW_ONLY_BORDER)
            {
                tileMapper->setMapperViewMode(MapperViewMode::SHOW_BORDER_AND_RASTER);
                up = true;
            }
        }
    }
    else if(propertyName == "show_without_border" && m_sceneDescriptor.isGeoScene())
    {
        bool on_off(value.toBool());
//        TileMapper* tileMapper = (TileMapper*)((*it).dataProvider);
        if(on_off)// && tileMapper->mapperViewMode() != MapperViewMode::SHOW_ONLY_RASTER)
        {
//            tileMapper->setMapperViewMode(MapperViewMode::SHOW_ONLY_RASTER);
            if(it.value().rasterLayer)
                it.value().rasterLayer->setVisible(false);
        }
        else //if(on_off == false && tileMapper->mapperViewMode() == MapperViewMode::SHOW_ONLY_RASTER)
        {
//            tileMapper->setMapperViewMode(MapperViewMode::SHOW_BORDER_AND_RASTER);
            if(it.value().rasterLayer)
                it.value().rasterLayer->setVisible(true);
        }
    }
    else if(propertyName == "z-index" && m_sceneDescriptor.isGeoScene())
    {
        uint zValue(value.toUInt());
        m_intersectsScene->setZvalue(providerId, zValue);
        up = true;
    }

    if(up)
    {
        m_Timer.stop();
        m_Timer.start(100);
    }
}

void TileMixer::slotProviderProcessingSettingsChanged(uint providerId, QString processingSystemName)
{
    visualize_system::DataInterface * dataInterface = visualize_system::VisualizerManager::instance()->getDataInterface(m_visualiserId);
    if( ! dataInterface)
        return;

    auto it = m_Providers.find(providerId);
    if(it == m_Providers.end())
        return;

//    TileMapper* tileMapper = (TileMapper*)((*it).dataProvider);
//    if(tileMapper)
//    {
//        bool hasProcessing = dataInterface->getProviderProcessingSystemNames(providerId).isEmpty() == false;
//        tileMapper->setHasProcessing(hasProcessing);
//    }

    m_Timer.stop();
    m_Timer.start(100);
}

void TileMixer::slotDefaultRasterSelectionModeChanged(visualize_system::DefaultRasterSelectionMode mode)
{
    _defaultRasterSelectionMode = mode;
    for(auto it = m_Providers.begin(); it != m_Providers.end(); ++it )
    {
        if(it.value().rasterLayer)
            it.value().rasterLayer->setDefaultPen(_defaultRasterSelectionMode == visualize_system::DefaultRasterSelectionMode::COLOR ?
                                                      _rasterLayerInitData.defaultPen :
                                                      QPen(Qt::NoPen));
    }
}

void TileMixer::propertyChanged(QString title, QVariant value, data_system::RasterDataProvider* prov)
{
        //qDebug()<< "---> void TileMixer::slotPropertyChanged, delegate :" << ((u_int64_t)(prov)) << ", title :" << title << ", value :" << value;

/*
        bool up(false);
        if(title == "transparency" && m_sceneDescriptor.isGeoScene())
        {
            auto it = m_Providers.find(prov->getProviderUrl().toString());
            if(it != m_Providers.end())
            {
                int percentTransparency(value.toInt());
                if(percentTransparency >= 0 && percentTransparency <= 100)
                {
                    it.value().setTransparency(percentTransparency);
                    m_intersectsScene->setTransparency(prov->getProviderUrl().toString(), percentTransparency);
                    up = true;
                }
            }
        }
        else if(title == "visibility" && m_sceneDescriptor.isGeoScene())
        {
            auto it = m_Providers.find(prov->getProviderUrl().toString());
            if(it != m_Providers.end())
            {
                bool visible(value.toBool());
                if(it.value().isVisible != visible)
                {
                    it.value().setVisible(visible);
                    if(it.value().rasterLayer)
                        it.value().rasterLayer->setVisible(visible);
                    m_intersectsScene->setVisibility(prov->getProviderUrl().toString(), visible);
                    up = true;
                }
            }
        }
        else if(title == "z-index" && m_sceneDescriptor.isGeoScene())
        {
            QMap<QString,QVariant> providers;
            QVariant varMap = m_Timer.property("providerMapForZLevel");
            if(varMap.isValid())
                providers = varMap.toMap();
            providers.insert(QString::number(value.toInt()), prov->getProviderUrl().toString());
            m_Timer.setProperty("providerMapForZLevel", providers);
            m_Timer.setProperty("propertyChanged", title);
            up = true;
        }
        else if(title == "histo_red" || title == "histo_green" || title == "histo_blue" || title == "histo_rgb" || title == "bricon" || title == "histo_gray")
        {
            auto it = m_Providers.find(prov->getProviderUrl().toString());
            if(it != m_Providers.end())
            {
                TileMapper* provider = (TileMapper*)((*it).dataProvider);
                provider->setHisto(title, value);
                up = true;
            }
        }
        else if(title == "geo_border_properties" && m_sceneDescriptor.isGeoScene())
        {
            //         qDebug()<<"Border properties was changed"
            //                 << value.toList().at(0).toBool()              // isVisible border
            //                 << Qt::PenStyle(value.toList().at(1).toInt()) // line style
            //                 << value.toList().at(2).toInt()               //line width
            //                 << value.toList().at(3).value<QColor>();      // line color

            auto it = m_Providers.find(prov->getProviderUrl().toString());
            if(it != m_Providers.end())
            {
                if(it.value().dataProvider->provider() == prov)
                    if(it.value().rasterLayer)
                    {
                        QColor color(value.toList().at(3).value<QColor>());
                        QPen pen(color);
                        pen.setWidth(value.toList().at(2).toInt());
                        pen.setStyle((Qt::PenStyle)value.toList().at(1).toInt());
                        it.value().rasterLayer->setDefaultPen(pen);
                        it.value().rasterLayer->setVisible(value.toList().at(0).toBool());
                    }
            }
        }
        else if(title == "raster_border_visibility" && m_sceneDescriptor.isGeoScene())
        {
            auto it = m_Providers.find(prov->getProviderUrl().toString());
            if(it != m_Providers.end())
            {
                if(it.value().dataProvider->provider() == prov)
                {
                    bool raster_border_visibility(value.toBool());
                    if(raster_border_visibility)
                    {
                        if(m_sceneDescriptor.refEpsgSRS() == 4326 && it.value().rasterLayer == 0)
                        {
                            TileMapper * tileMapper = dynamic_cast<TileMapper *>(it.value().dataProvider);
                            if(tileMapper)
                            {
                                visualize_system::RasterLayer * rasterLayer = new visualize_system::RasterLayer(tileMapper->getBoundingPolygone());
                                connect(rasterLayer, SIGNAL(signalSetActive()), this, SLOT(rasterLayerActivated()));
                                rasterLayer->setSelectedPen(m_selectedPen);
                                rasterLayer->setDefaultPen(m_defaultPen);
                                it.value().rasterLayer = rasterLayer;

                                QList<QGraphicsItem*>* list = new QList<QGraphicsItem*>;
                                list->append(rasterLayer);
                                emit signalItemsForDraw(list);
                            }
                        }
                    }
                    else
                    {
                        if(it.value().rasterLayer)
                        {
                            delete it.value().rasterLayer;
                            it.value().rasterLayer = 0;
                        }
                    }
                }
            }
        }
        else if(title == "clear_black_area" && m_sceneDescriptor.isGeoScene())
        {
            auto it = m_Providers.find(prov->getProviderUrl().toString());
            if(it != m_Providers.end())
            {
                bool clearBlackArea(value.toBool());
                if((*it).clearBlackArea != clearBlackArea)
                {
                    (*it).clearBlackArea = clearBlackArea;
                    up = true;
                }
            }
        }

        if(up)
        {
            m_Timer.stop();
            m_Timer.start(100);
        }

*/
}

void TileMixer::rasterLayerActivated()
{
}

void TileMixer::rasterLayerActivated(visualize_system::RasterLayer* rasterLayer)
{
}

void TileMixer::setPenForAltMatrix(const QString &path, const QVariant &value)
{
//    for(QMap<QString,visualize_system::RasterLayer *>::iterator it(m_altMatrixes.begin()); it != m_altMatrixes.end(); ++it)
//        if(it.key() == path)
//        {
//            QColor color(value.toList().at(3).value<QColor>());
//            QPen pen(color);
//            pen.setWidth(value.toList().at(2).toInt());
//            pen.setStyle((Qt::PenStyle)value.toList().at(1).toInt());
//            it.value()->setDefaultColor(pen.color());
//            it.value()->setVisible(value.toList().at(0).toBool());
//            break;
//        }
}

void TileMixer::setSelectedPen(QPen pen)
{
    /*
    pen.setCosmetic(true);
    if(pen.width() < 2)
        pen.setWidth(2);
    m_selectedPen = pen;
    for(auto it = m_Providers.begin(); it != m_Providers.end(); ++it)
        if(it.value().rasterLayer)
            it.value().rasterLayer->setSelectedColor(m_selectedPen.color());

    for(QMap<QString,visualize_system::RasterLayer *>::iterator it(m_altMatrixes.begin()); it != m_altMatrixes.end(); ++it)
        it.value()->setSelectedColor(m_selectedPen.color());
        */
}

void TileMixer::setDefaultPen(QPen pen)
{
    /*
    pen.setCosmetic(true);
    if(pen.width() < 2)
        pen.setWidth(2);
    m_defaultPen = pen;

    m_selectedPen = pen;
    for(auto it = m_Providers.begin(); it != m_Providers.end(); ++it)
        if(it.value().rasterLayer)
            it.value().rasterLayer->setSelectedPen(m_defaultPen);

    for(QMap<QString,visualize_system::RasterLayer *>::iterator it(m_altMatrixes.begin()); it != m_altMatrixes.end(); ++it)
        it.value()->setDefaultPen(m_defaultPen);
        */
}

void TileMixer::slot_dataLoaded()
{
    dataLoaded();
}

void TileMixer::dataLoaded()
{
    if(m_blockFinishLoadind)
    {
//        qDebug() << "--- 111 void TileMixer::slot_dataLoaded()";
        return;
    }

    for(auto it = m_Providers.begin(); it != m_Providers.end(); ++it)
    {
        TileMapper* provider = (TileMapper*)(it.value().dataProvider);
        if( ! provider)
            continue;
        if(provider->dataLoaded() == false)
        {
//            qDebug() << "--- 222 void TileMixer::slot_dataLoaded()";
            return;
        }
    }

//    qDebug() << "+++ void TileMixer::slot_dataLoaded(), m_Cells.size :" << m_Cells.size();
    m_Cells.clear();
    emit loadingFinished();
}

QRectF TileMixer::tileSceneRect(int x, int y, int z)
{
    double POW(pow(2,z-1));
    double tileWOnView(m_sceneDescriptor.tileSize().width() / POW);
    double tileHOnView(m_sceneDescriptor.tileSize().height() / POW);
    return QRectF( x * tileWOnView, y * tileHOnView, tileWOnView, tileHOnView );
}

//! метод не адекватен !!! Нужно доработать до любой проекции !!!
//QPointF TileMixer::geoToScene(const QPointF& coordinateIn)
//{
//    const u_int64_t W(m_sceneDescriptor.sceneSize().width());
//    const u_int64_t H(m_sceneDescriptor.sceneSize().height());

//    QPointF point;
//    point.setX( (coordinateIn.y() +180 )*W/360.0 ); // long
//    point.setY( ( -coordinateIn.x()  + 90)*( H)/180.0 ); //latt
//    return point;
//}

void TileMixer::slot_reloadViewport()
{
    /*
    QVariant propertyChanged = m_Timer.property("propertyChanged");
    if(propertyChanged.isValid())
    {
        m_Timer.setProperty("propertyChanged", QVariant());
        if(propertyChanged.toString() == QString("z-index")  && m_sceneDescriptor.isGeoScene())
        {
            QMap<QString,QVariant> providers = m_Timer.property("providerMapForZLevel").toMap();
            if(providers.size() == m_Providers.size())
            {
                m_intersectsScene->clearScene();
                m_Timer.setProperty("providerMapForZLevel", QVariant());
                for(auto mapIt = providers.begin(); mapIt != providers.end(); ++mapIt)
                {
                    QString providerName = mapIt.value().toString();
                    auto it = m_Providers.find(providerName);
                    if(it != m_Providers.end())
                    {
                        TileMapper* tileMapper = (TileMapper*)((*it).dataProvider);
                        if(tileMapper->provider()->spatialRect() == QRectF(QPointF(-90,-180),QPointF(90,180)))
                        {
                            m_intersectsScene->addPolygon( providerName,
                                                           QPolygonF( QRectF(0,0,m_sceneDescriptor.sceneSize().width(),m_sceneDescriptor.sceneSize().height())),
                                                           (tileMapper->isTMSProviderMapper() == false) );
                        }
                        else
                        {
                            m_intersectsScene->addPolygon(providerName, tileMapper->getBoundingPolygone(), (tileMapper->isTMSProviderMapper() == false) );
                        }
                    }
                }
            }
        }
    }
    */
    concrete_abortLoading();
    if(m_lastCurrentZ > 2)
        emit updateContent(false);
    else
        emit updateContent(true); // этот залипон решает проблему "мозаики" при снятии видимости верхнего базового покрытия (всего два базовых) на нижнем Z-уровне.
    m_Timer.stop();
}

void TileMixer::slot_ClickRBOnObject(quint64 id)
{
    if(m_rasterControlWidget || m_matchingWidget)
        return;

//    if(m_objectMenu)
//        if(m_objectMenu->isEmpty() == false)
//        {
//            foreach(QAction * act,  m_objectMenu->actions())
//                act->setProperty("ObjectId", id);

//            m_objectMenu->exec(QCursor::pos());
//        }

    QList<QGraphicsView*> list = m_scene->views();
    QGraphicsView* viewParent(list.isEmpty() ? 0 : list.first());
    QMenu menu(viewParent);
    emit signalInitObjectMenu(menu, id);
    menu.exec(QCursor::pos());
}

void TileMixer::slot_ClickRBOnRaster(uint id, QPointF geoPos, bool controlModifier)
{
    if(m_rasterControlWidget || m_matchingWidget)
        return;

    if(m_rasterMenu)
        if(m_rasterMenu->isEmpty() == false)
        {
            QList<uint> selectedProviders;
            selectedProviders.append(id);

            visualize_system::SelectionModelInterface * selectionModelInterface = visualize_system::VisualizerManager::instance()->getSelectionModelInterface(m_visualiserId);
            if(selectionModelInterface)
            {
                if(selectionModelInterface->getSelectedProviders().toSet().contains(id) == false && controlModifier == false)
                    selectionModelInterface->clearSelectProviders();

                QSet<uint> set(selectionModelInterface->getSelectedProviders().toSet());// если селекция не меняется - нужно захайдить предъидущий активный растр
                if(set.contains(id))
                    selectionModelInterface->setSelectProviders(false, selectedProviders);

                selectionModelInterface->setSelectProviders(true, selectedProviders);
                selectionModelInterface->setCurrentProvider(id);
            }

            QMap<QString, QVariant> map;
            QString propertyName("RasterId");
            QVariant varName(id);
            map.insert(propertyName, varName);

            QString propertyGeoPos("ClickGeoPos");
            QVariant varGeoPos(geoPos);
            map.insert(propertyGeoPos, varGeoPos);

            QList<QGraphicsView*> list = m_scene->views();
            QGraphicsView* viewParent(list.isEmpty() ? 0 : list.first());
            if(viewParent)
            {
                QPixmap pixmap(100,100);
                QPainter painter(&pixmap);
                QRectF pixmapRect(0, 0, pixmap.size().width(), pixmap.size().height() );
                QPoint p = viewParent->mapFromGlobal(QCursor::pos());
                viewParent->render(&painter, pixmapRect, QRect(p.x() - pixmap.size().width()/2., p.y() - pixmap.size().height()/2. , pixmap.size().width(), pixmap.size().height()));

                QVariant var(pixmap);
                map.insert("PixmapUnderMouse", var);
            }

            setActionProperty(map, m_rasterMenu);
            m_rasterMenu->exec(QCursor::pos());
        }
}

void TileMixer::setActionProperty(QMap<QString, QVariant> map, QMenu * menu)
{
    QList<QAction*>list = menu->actions();
    foreach(QAction * act, list)
    {
        if(act->isSeparator())
            continue;

        QMenu * subMenu = act->menu();
        if(subMenu)
            setActionProperty(map, subMenu);
        else
        {
            for(auto it = map.begin(); it != map.end(); ++it)
                act->setProperty(it.key().toUtf8(), it.value());
        }
    }
}

void TileMixer::slot_DubleClickOnScene(QPointF scenePos)
{
    if(m_rasterControlWidget || m_matchingWidget)
        return;

    if(m_sceneDescriptor.isGeoScene())
    {
        QList<uint> list = m_intersectsScene->check(scenePos);
        if(list.size() >= 1)
        {
            QList<uint> _list;
            foreach (uint id, list)
                _list.prepend(id);

            for(auto it = m_Providers.begin(); it != m_Providers.end(); ++it)
            {
                if(it.value().rasterLayer)
                {
                    m_intersectsScene->setVisibility(it.key(), false); // проверку, что только не для базовых слоёв !
                    it.value().rasterLayer->setVisible(false);
                }
            }

            ParallelCursor* parallelCursor = new ParallelCursor(0);
            parallelCursor->setPos(scenePos);
            QList<QGraphicsItem*>* graphicsItemList = new QList<QGraphicsItem*>;
            graphicsItemList->append(parallelCursor);
            emit signalItemsForDraw(graphicsItemList);

            m_rasterControlWidget = new RasterControlWidget(m_visualiserId, _list, parallelCursor);
            connect(m_rasterControlWidget, SIGNAL(signalSetVisibleForRaster(uint,uint)), this, SLOT(slot_setVisibleForRaster(uint,uint)));
            connect(m_rasterControlWidget, SIGNAL(signalRasterControlWidgetClosed()), this, SLOT(slot_ControlWidgetClosed()));

            slot_setVisibleForRaster(_list.last(), 0);

            concrete_abortLoading();
            emit updateContent(true);
            m_Timer.stop();

//            qDebug() << "TileMixer::slot_DubleClickOnScene";
            emit signalSetWidgetsEnabled(false);
        }
    }
}

void TileMixer::slot_setVisibleForRaster(uint rasterShow, uint rasterHide)
{
    if(rasterShow > 0)
    {
        auto it = m_Providers.find(rasterShow);
        if(it != m_Providers.end())
        {
            m_intersectsScene->setVisibility(rasterShow, true);
            if(it.value().rasterLayer)
            {
                it.value().rasterLayer->setVisible(true);
                rasterLayerActivated(it.value().rasterLayer);
            }
        }
    }

    if(rasterHide > 0)
    {
        auto it = m_Providers.find(rasterHide);
        if(it != m_Providers.end())
        {
            m_intersectsScene->setVisibility(rasterHide, false);
            if(it.value().rasterLayer)
            {
                it.value().rasterLayer->setVisible(false);
//                qDebug() << "+++ rasterHide" << rasterHide << ", m_Providers.size():" << m_Providers.size();
            }
            else
            {
//                qDebug() << "--- 111 rasterHide" << rasterHide << ", m_Providers.size():" << m_Providers.size();
            }
        }
        else
        {
//            qDebug() << "--- 222 rasterHide" << rasterHide << ", m_Providers.size():" << m_Providers.size();
        }
    }

    concrete_abortLoading();
    emit updateContent(true);
    m_Timer.stop();
}

void TileMixer::slot_ControlWidgetClosed()
{
//    qDebug() << "TileMixer::slot_ControlWidgetClosed";

    uint providerId = 0;
    if(m_rasterControlWidget)
    {
        m_rasterControlWidget->deleteLater();
        m_rasterControlWidget = 0;
    }
    if(m_matchingWidget)
    {
        providerId = m_matchingWidget->getMatchingProviderId();

        delete m_matchingWidget;//->deleteLater();
        m_matchingWidget = 0;
    }

    for(auto it = m_Providers.begin(); it != m_Providers.end(); ++it)
    {
        m_intersectsScene->setVisibility(it.key(), it.value().isVisible);
        if(it.value().rasterLayer)
            it.value().rasterLayer->setVisible(it.value().isVisible);
    }

    concrete_abortLoading();
    emit updateContent(true);
    m_Timer.stop();

    if(providerId > 0)
        emit signalProviderMatched(providerId);

    emit signalSetWidgetsEnabled(true);
}

QList<uint> TileMixer::getProviders(ProviderType providerType)
{
    if(providerType == ProviderType::BaseCoverAndImages)
        return m_Providers.keys();
    else
    {
        QList<uint> list;
        for(auto it = m_Providers.begin(); it != m_Providers.end(); ++it)
        {
            if(providerType == ProviderType::BaseCover)
            {
                if(it.value().baseCover == true)
                    list.append(it.key());
            }
            else if(providerType == ProviderType::Images)
            {
                if(it.value().baseCover == false)
                    list.append(it.key());
            }
        }
        return list;
    }
}

QList<QString> TileMixer::getAltMatrices()
{
    return m_altMatrixes.keys();
}

void TileMixer::slot_getRasterParams(uint rasterId, QPixmap& pixmap, int& zLevel, QPolygonF& vertexInSceneCoords)
{
    auto it = m_Providers.find(rasterId);
    if(it != m_Providers.end())
    {
        RasterTileMapper* tileMapper = dynamic_cast<RasterTileMapper*>((*it).dataProvider);
        if(tileMapper)
        {
            tileMapper->getPixmap(m_lastCurrentZ, pixmap, vertexInSceneCoords);
            zLevel = m_lastCurrentZ;
        }
    }
}

void TileMixer::slot_applyTransformation(uint rasterId, const QPolygonF& polygonInSceneCoords)
{
//    auto it = m_Providers.find(rasterId);
//    if(it != m_Providers.end())
//    {
//        RasterTileMapper* tileMapper = dynamic_cast<RasterTileMapper*>((*it).dataProvider);
//        if(tileMapper)
//        {
//            QPolygonF polygonInGeoCoords;
//            foreach(QPointF p, polygonInSceneCoords)
//            {
//                double x(p.x()), y(p.y());
//                m_sceneDescriptor.convertSceneToNative(x,y);
//                polygonInGeoCoords.append(QPointF(x, y));
//            }
//            dpf::geo::PolinomialGeoData dst_data;
//            dpf::PolygonF pixelPoly(tileMapper->provider()->geoImageObjectPtr()->image_rect());
//            dst_data.add(pixelPoly.at(0), dpf::geo::GeoPoint(polygonInGeoCoords.at(0).y(), polygonInGeoCoords.at(0).x()));
//            dst_data.add(pixelPoly.at(1), dpf::geo::GeoPoint(polygonInGeoCoords.at(1).y(), polygonInGeoCoords.at(1).x()));
//            dst_data.add(pixelPoly.at(2), dpf::geo::GeoPoint(polygonInGeoCoords.at(2).y(), polygonInGeoCoords.at(2).x()));
//            dst_data.add(pixelPoly.at(3), dpf::geo::GeoPoint(polygonInGeoCoords.at(3).y(), polygonInGeoCoords.at(3).x()));
//            dst_data.set_image_size(tileMapper->provider()->geoImageObjectPtr()->image_size());
//            dst_data.set(1);
//            dst_data.set_proj(4326);

//            tileMapper->provider()->setMatchingData(&dst_data, false);
//        }

//        concrete_abortLoading();
//        emit updateContent(true);
//        m_Timer.stop();
//    }
}

void TileMixer::slot_getPolygonInSceneCoords(uint rasterId, QPolygonF &polygonInSceneCoords)
{
    auto it = m_Providers.find(rasterId);
    if(it != m_Providers.end())
    {
        RasterTileMapper* tileMapper = dynamic_cast<RasterTileMapper*>((*it).dataProvider);
        if(tileMapper)
        {
            polygonInSceneCoords = tileMapper->getBoundingPolygone();
        }
    }
}

// Если послать заполненный "matchingPoints", то в "dst_data" запишутся конвертированные данные и по ним будет установлена привязка.
// Если послать пустой "matchingPoints", и заполненный "dst_data" - по ним будет установлена привязка.
//void TileMixer::slot_applyMatching(uint rasterId, QList<MatchingState::MatchingPoint> matchingPoints, dpf::geo::PolinomialGeoData &dst_data)
//{
//    auto it = m_Providers.find(rasterId);
//    if(it != m_Providers.end())
//    {
//        RasterTileMapper* tileMapper = dynamic_cast<RasterTileMapper*>((*it).dataProvider);
//        if(tileMapper)
//        {
//            if(matchingPoints.isEmpty() == false)
//            {
//                dst_data.clear();
//                foreach(MatchingState::MatchingPoint p, matchingPoints)
//                {
//                    double onRasterX(p.onRaster.x()), onRasterY(p.onRaster.y());
//                    m_sceneDescriptor.convertSceneToNative(onRasterX,onRasterY);

//                    double onMapX(p.onMap.x()), onMapY(p.onMap.y());
//                    m_sceneDescriptor.convertSceneToNative(onMapX,onMapY);

//                    dpf::geo::PixelPoint pixelP = tileMapper->provider()->geoImageObjectPtr()->geo_to_pixel(dpf::geo::GeoPoint(onRasterY,onRasterX));
//                    dst_data.add(pixelP, dpf::geo::GeoPoint(onMapY,onMapX));
//                }
//                dst_data.set_image_size(tileMapper->provider()->geoImageObjectPtr()->image_size());
//            }

//            tileMapper->provider()->setMatchingData(&dst_data, false);

//            concrete_abortLoading();
//            emit updateContent(true);
//            m_Timer.stop();
//        }
//    }
//}

void TileMixer::slotMarkedProvidersChanged(const QList<uint> & providerMarked, const QList<uint> & providerDemarked)
{
    foreach(uint providerId, providerDemarked)
    {
        auto provIt = m_Providers.find(providerId);
        if(provIt != m_Providers.end())
        {
            if(provIt.value().rasterLayer)
                provIt.value().rasterLayer->setLayerMarked(false);
        }
    }
    foreach(uint providerId, providerMarked)
    {
        auto provIt = m_Providers.find(providerId);
        if(provIt != m_Providers.end())
        {
            if(provIt.value().rasterLayer)
                provIt.value().rasterLayer->setLayerMarked(true);
        }
    }
}

void TileMixer::slotMatchProvider(QVariant var)
{
    quint64 id = var.toULongLong();
    if(id > 0)
    {
        matchingProvider(id);
    }
    else
    {
        slot_ControlWidgetClosed();
    }
}

void TileMixer::matchingProvider(uint id)
{
    if(m_sceneDescriptor.isGeoScene() == false)
        return;

    if(m_matchingWidget)
        return;

    auto it = m_Providers.find(id);
    if(it != m_Providers.end())
    {
        RasterTileMapper* tileMapper = dynamic_cast<RasterTileMapper*>((*it).dataProvider);

//! ПЕРЕОСМЫСЛИТЬ ЭТО МЕСТО !!!
        if( ! tileMapper)
            return;

        if(m_rasterControlWidget)
        {
            m_rasterControlWidget->deleteLater();
            m_rasterControlWidget = 0;
        }

        for(auto it2 = m_Providers.begin(); it2 != m_Providers.end(); ++it2)
            if(it2.value().rasterLayer)
            {
                m_intersectsScene->setVisibility(it2.key(), false); // проверку, что только не для базовых слоёв !
                it2.value().rasterLayer->setVisible(false);
            }

//        qDebug() << "TileMixer::matchingProvider ---> ID:" << id;
        m_intersectsScene->setVisibility(id, true); // проверку, что только не для базовых слоёв !
        it.value().rasterLayer->setVisible(true);

        concrete_abortLoading();
        emit updateContent(true);
        m_Timer.stop();

        m_matchingWidget = new MatchingWidget(id, m_visualiserId, tileMapper->getBoundingPolygone());

        QPointF centerBySceneCoords = tileMapper->getBoundingPolygone().boundingRect().center();
        emit signalConvertSceneCoordToGlobal(centerBySceneCoords);
        m_matchingWidget->move(centerBySceneCoords.toPoint());

        connect(m_matchingWidget, SIGNAL(signal_setVisibleForRaster(uint,uint)),
                this, SLOT(slot_setVisibleForRaster(uint,uint)));
        connect(m_matchingWidget, SIGNAL(signal_getRasterParams(uint,QPixmap&,int&,QPolygonF&)),
                this, SLOT(slot_getRasterParams(uint,QPixmap&,int&,QPolygonF&)));
        connect(m_matchingWidget, SIGNAL(signal_applyTransformation(uint,QPolygonF)),
                this, SLOT(slot_applyTransformation(uint,QPolygonF)));
        connect(m_matchingWidget, SIGNAL(signal_getPolygonInSceneCoords(uint,QPolygonF&)),
                this, SLOT(slot_getPolygonInSceneCoords(uint,QPolygonF &)));
//        connect(m_matchingWidget, SIGNAL(signal_applyMatching(uint,QList<MatchingState::MatchingPoint>, dpf::geo::PolinomialGeoData &)),
//                this, SLOT(slot_applyMatching(uint,QList<MatchingState::MatchingPoint>, dpf::geo::PolinomialGeoData &)));

        emit signalSetWidgetsEnabled(false);
    }
}

void TileMixer::addActionToObjectMenu(QList<QAction*> actions, QString /*groupName*/, uint /*groupWeight*/, QString /*subMenuName*/)
{
//    if(! m_objectMenu)
//    {
//        QList<QGraphicsView*> list = m_scene->views();
//        QGraphicsView* viewParent(list.isEmpty() ? 0 : list.first());
//        m_objectMenu = new QMenu(viewParent);
//    }

//    foreach(QAction* act, actions)
//        m_objectMenu->addAction(act);
}


void TileMixer::addActionToRasterMenu(QList<QAction*> actions, QString groupName, uint groupWeight, QString subMenuName)
{
    QList<QGraphicsView*> list = m_scene->views();
    QGraphicsView* viewParent(list.isEmpty() ? 0 : list.first());

    if(! m_rasterMenu)
        m_rasterMenu = new QMenu(viewParent);

    if(m_rasterMenu->isEmpty())
    {
        if(subMenuName.isEmpty())
        {
            foreach(QAction* act, actions)
            {
                act->setProperty("groupName", groupName);
                act->setProperty("groupWeight", groupWeight);
                m_rasterMenu->addAction(act);
            }
        }
        else
        {
            QAction * subMenuAction = new QAction(subMenuName, this);
            subMenuAction->setProperty("groupName", groupName);
            subMenuAction->setProperty("groupWeight", groupWeight);
            QMenu * menu = new QMenu(viewParent);
            foreach(QAction* act, actions)
                menu->addAction(act);
            subMenuAction->setMenu(menu);
            m_rasterMenu->addAction(subMenuAction);
        }
    }
    else
    {
        QMap<QString, QList<QAction*> > groupName_actions;
        QMap<uint, QList<QString> > groupWeight_groupName;
        foreach(QAction* act, m_rasterMenu->actions())
        {
            if(act->isSeparator())
                continue;

            QVariant groupNameVar = act->property("groupName");
            QVariant groupWeightVar = act->property("groupWeight");
            if(groupNameVar.isValid() && groupWeightVar.isValid())
            {
                auto itName = groupName_actions.find(groupNameVar.toString());
                if(itName != groupName_actions.end())
                    itName.value().append(act);
                else
                    groupName_actions.insert(groupNameVar.toString(), QList<QAction*>() << act);


                auto itWeight = groupWeight_groupName.find(groupWeightVar.toUInt());
                if(itWeight != groupWeight_groupName.end())
                    itWeight.value().append(groupNameVar.toString());
                else
                    groupWeight_groupName.insert(groupWeightVar.toUInt(), QList<QString>() << groupNameVar.toString());
            }
        }
        m_rasterMenu->clear();
        delete m_rasterMenu;
        m_rasterMenu = new QMenu(viewParent);
        if(subMenuName.isEmpty())
        {
            foreach(QAction* act, actions)
            {
                act->setProperty("groupName", groupName);
                act->setProperty("groupWeight", groupWeight);

                auto itName = groupName_actions.find(groupName);
                if(itName != groupName_actions.end())
                    itName.value().append(act);
                else
                    groupName_actions.insert(groupName, QList<QAction*>() << act);

                auto itWeight = groupWeight_groupName.find(groupWeight);
                if(itWeight != groupWeight_groupName.end())
                    itWeight.value().append(groupName);
                else
                    groupWeight_groupName.insert(groupWeight, QList<QString>() << groupName);
            }
        }
        else
        {
            bool subMenuNameFound(false);
            for(auto itGroupName = groupName_actions.begin(); itGroupName != groupName_actions.end(); ++itGroupName)
            {
                for(auto itActions = itGroupName.value().begin(); itActions != itGroupName.value().end(); ++itActions)
                    if( (*itActions)->text() == subMenuName)
                    {
                        QMenu * menu = (*itActions)->menu();
                        if(menu)
                            foreach(QAction* act, actions)
                                menu->addAction(act);

                        subMenuNameFound = true;
                        break;
                    }
            }

            if(subMenuNameFound == false)
            {
                QAction * subMenuAction = new QAction(subMenuName, this);
                subMenuAction->setProperty("groupName", groupName);
                subMenuAction->setProperty("groupWeight", groupWeight);
                QMenu * menu = new QMenu(viewParent);
                foreach(QAction* act, actions)
                    menu->addAction(act);
                subMenuAction->setMenu(menu);

                auto itName = groupName_actions.find(groupName);
                if(itName != groupName_actions.end())
                    itName.value().append(subMenuAction);
                else
                    groupName_actions.insert(groupName, QList<QAction*>() << subMenuAction);

                auto itWeight = groupWeight_groupName.find(groupWeight);
                if(itWeight != groupWeight_groupName.end())
                    itWeight.value().append(groupName);
                else
                    groupWeight_groupName.insert(groupWeight, QList<QString>() << groupName);
            }
        }

        bool firstGroup(true);
        for(auto itGroupWeight = groupWeight_groupName.begin(); itGroupWeight != groupWeight_groupName.end(); ++itGroupWeight)
        {
            for(auto itGroupName = itGroupWeight.value().begin(); itGroupName != itGroupWeight.value().end(); ++itGroupName)
            {
                QString _groupName = (*itGroupName);
                auto targetGroup = groupName_actions.find(_groupName);
                if(targetGroup != groupName_actions.end())
                {
                    if(firstGroup)
                        firstGroup = false;
                    else
                        m_rasterMenu->addSeparator();

                    foreach(QAction * act, targetGroup.value())
                        m_rasterMenu->addAction(act);
                }
            }
        }
    }
}

//void TileMixer::slotSelectedProvidersChanged(QList<uint> selectedProviders)
//{
//    if(m_sceneDescriptor.isGeoScene())
//    {
//        if(m_intersectsScene)
//            m_intersectsScene->setProviderOnTop( selectedProviders.size() == 1 ? selectedProviders.first() : 0 );

//        concrete_abortLoading();
//        emit updateContent(true);
//        m_Timer.stop();
//    }
//}

void TileMixer::slotSelectedProvidersChanged(const QList<uint> & providerSelected, const QList<uint> & providerDeselected)
{
    foreach(uint providerId, providerDeselected)
    {
        auto provIt = m_Providers.find(providerId);
        if(provIt != m_Providers.end())
        {
            if(provIt.value().rasterLayer)
                provIt.value().rasterLayer->setLayerSelected(false);
        }
    }
    foreach(uint providerId, providerSelected)
    {
        auto provIt = m_Providers.find(providerId);
        if(provIt != m_Providers.end())
        {
            if(provIt.value().rasterLayer)
                provIt.value().rasterLayer->setLayerSelected(true);
        }
    }


}

void TileMixer::slotCurrentProviderChanged(uint idCurrProv, uint prev)
{
    if(m_sceneDescriptor.isGeoScene() == false)
        return;

    visualize_system::SelectionModelInterface * selectionModelInterface = visualize_system::VisualizerManager::instance()->getSelectionModelInterface(m_visualiserId);
    if(selectionModelInterface)
    {
        QPolygonF subPolygon;
        if(idCurrProv > 0 )
        {
            auto provIt = m_Providers.find(idCurrProv);
            if(provIt != m_Providers.end())
            {
                if(provIt.value().rasterLayer)
                {
                    idCurrProv = provIt.key();
                    subPolygon = provIt.value().rasterLayer->getPolygon();
                    provIt.value().rasterLayer->setOnTop();
                }
            }
        }

        for(auto it = m_Providers.begin(); it != m_Providers.end(); ++it)
            if(it.key() != idCurrProv && it.value().rasterLayer)
                it.value().rasterLayer->subtract(subPolygon);
    }

    if(m_intersectsScene)
        m_intersectsScene->setProviderOnTop(idCurrProv);

    concrete_abortLoading();
    emit updateContent(true);
    m_Timer.stop();
}

void TileMixer::loadingRasterLayerInitData()
{
    QVariant _selCol = CtrConfig::getValueByName(QString("vectorrender.selectedColorRGBA"));
    if(!_selCol.isValid())
    {
        CtrConfig::setValueByName(QString("vectorrender.selectedColorRGBA"), "#FBFF05FF");
        _selCol = CtrConfig::getValueByName(QString("vectorrender.selectedColorRGBA"));
    }

    QVariant _choosCol = CtrConfig::getValueByName(QString("vectorrender.choosedColorRGBA"));
    if(!_choosCol.isValid())
    {
        CtrConfig::setValueByName(QString("vectorrender.choosedColorRGBA"), "#0CFF00FF");
        _choosCol = CtrConfig::getValueByName(QString("vectorrender.choosedColorRGBA"));
    }

    QVariant _highCol = CtrConfig::getValueByName(QString("vectorrender.highlightedColorRGBA"));
    if(!_highCol.isValid())
    {
        CtrConfig::setValueByName(QString("vectorrender.highlightedColorRGBA"), "#91969FFF");
        _highCol = CtrConfig::getValueByName(QString("vectorrender.highlightedColorRGBA"));
    }

    QVariant _markCol = CtrConfig::getValueByName(QString("vectorrender.markedColorRGBA"));
    if(!_markCol.isValid())
    {
        CtrConfig::setValueByName(QString("vectorrender.markedColorRGBA"), "#FBFF057D");
    }

    QVariant _selW = CtrConfig::getValueByName(QString("vectorrender.selectedWidth"));
    if(!_selW.isValid())
    {
        CtrConfig::setValueByName(QString("vectorrender.selectedWidth"), "2");
        _selW = CtrConfig::getValueByName(QString("vectorrender.selectedWidth"));
    }

    QVariant _choosW = CtrConfig::getValueByName(QString("vectorrender.choosedWidth"));
    if(!_choosW.isValid())
    {
        CtrConfig::setValueByName(QString("vectorrender.choosedWidth"), "2");
        _choosW = CtrConfig::getValueByName(QString("vectorrender.choosedWidth"));
    }

    QVariant _highW = CtrConfig::getValueByName(QString("vectorrender.highlightedWidth"));
    if(!_highW.isValid())
    {
        CtrConfig::setValueByName(QString("vectorrender.highlightedWidth"), "2");
        _highW = CtrConfig::getValueByName(QString("vectorrender.highlightedWidth"));
    }

    int  selW = _selW.toInt();
    int  choosW = _choosW.toInt();
    int  highW = _highW.toInt();

    QString  selCol = _selCol.toString();
    QString  choosCol = _choosCol.toString();
    QString  highCol = _highCol.toString();
    QString  markCol = _markCol.toString();

    selCol = selCol.remove("#");
    choosCol = choosCol.remove("#");
    highCol = highCol.remove("#");
    markCol = markCol.remove("#");

    QPen sel, choos, high;
    sel.setWidth(selW);
    sel.setCosmetic(true);
    int r = selCol.mid(0,2).toInt(0,16);
    int g = selCol.mid(2,2).toInt(0,16);
    int b = selCol.mid(4,2).toInt(0,16);
    int a = selCol.mid(6,2).toInt(0,16);
    sel.setColor(QColor(r,g,b,a));
    _rasterLayerInitData.activePen = sel;

    choos.setWidth(choosW);
    choos.setCosmetic(true);
    r = choosCol.mid(0,2).toInt(0,16);
    g = choosCol.mid(2,2).toInt(0,16);
    b = choosCol.mid(4,2).toInt(0,16);
    a = choosCol.mid(6,2).toInt(0,16);
    choos.setColor(QColor(r,g,b,a));
    _rasterLayerInitData.selectPen = choos;

    high.setWidth(highW);
    high.setCosmetic(true);
    r = highCol.mid(0,2).toInt(0,16);
    g = highCol.mid(2,2).toInt(0,16);
    b = highCol.mid(4,2).toInt(0,16);
    a = highCol.mid(6,2).toInt(0,16);
    high.setColor(QColor(r,g,b,a));
    _rasterLayerInitData.defaultPen = high;

    r = markCol.mid(0,2).toInt(0,16);
    g = markCol.mid(2,2).toInt(0,16);
    b = markCol.mid(4,2).toInt(0,16);
    a = markCol.mid(6,2).toInt(0,16);
    _rasterLayerInitData.markBrush.setColor(QColor(r,g,b,a));
}

void TileMixer::slot_providerChanged()
{
    TileMapper* tileMapper = dynamic_cast<TileMapper*>(sender());
    if( ! tileMapper)
        return;

    uint providerId = tileMapper->getProviderId();
    auto it = m_Providers.find(providerId);
    if(it == m_Providers.end())
        return;

    if(m_sceneDescriptor.isGeoScene())
    {
        if(it.value().rasterLayer)
        {
            it.value().rasterLayer->setRasterPolygon(tileMapper->getBoundingPolygone());
        }
        else
        {
            it.value().rasterLayer = new visualize_system::RasterLayer(providerId, tileMapper->getBoundingPolygone(), _rasterLayerInitData);
            connect(it.value().rasterLayer, SIGNAL(signalClicked(bool)), this, SLOT(rasterLayerClicked(bool)));
            it.value().rasterLayer->setDefaultPen(_defaultRasterSelectionMode == visualize_system::DefaultRasterSelectionMode::COLOR ? _rasterLayerInitData.defaultPen : QPen(QColor(Qt::transparent)));
        }

        QList<QGraphicsItem*>* list = new QList<QGraphicsItem*>;
        list->append(it.value().rasterLayer->getGraphicsItem());
        emit signalItemsForDraw(list);

        m_intersectsScene->setPolygon(providerId, tileMapper->getBoundingPolygone());
    }
    else
    {
        TileMapper * tileMapper = visualize_system::TileMapperFactory::createTileMapper(m_visualiserId, providerId, m_sceneDescriptor);
        connect(tileMapper, SIGNAL(signal_paintTile(uint,const QString&,QPixmap&,int)), this, SLOT(paintTile(uint,const QString&,QPixmap&,int)));
        connect(tileMapper, SIGNAL(signal_dataLoaded()), this, SLOT(slot_dataLoaded()));
        connect(tileMapper, SIGNAL(signal_providerChanged()), this, SLOT(slot_providerChanged()));
        delete it.value().dataProvider;
        it.value().dataProvider = tileMapper;

        QSharedPointer<data_system::RasterDataProvider> rdp = data_system::DataProviderFactory::instance()->getProvider(providerId).dynamicCast<data_system::RasterDataProvider>();
        if(rdp)
        {
            QString fileName = QFileInfo(rdp->getProviderUrl().toLocalFile()).fileName();
            if(rdp->providerChanged())
                fileName.prepend(" * ");
            visualize_system::VisualizerManager::instance()->getPixelVisualizersInterface()->setWindowTitle(m_visualiserId, fileName);

            emit initPixelScene(tileMapper->getFrameDelta(), tileMapper->getBaseZlevel(), -1/*frame.toInt()*/, -1, rdp->getProviderUrl().toLocalFile());

//            PixelSceneDescriptor* pixelSceneDescriptor = dynamic_cast<PixelSceneDescriptor*>(&m_sceneDescriptor);
//            if(pixelSceneDescriptor)
//                pixelSceneDescriptor->init(rdp->geoImageObjectPtr());

            if(m_intersectsScene)
                m_intersectsScene->clearScene();
        }
    }

    concrete_abortLoading();
    emit updateContent(true);
    m_Timer.stop();
}

//! слот только для отслеживания изменения растров привязанных по месту съёмки (так как у них нет маппера)
void TileMixer::slotProviderChanged(uint id)
{
//    auto it = m_Providers.find(id);
//    if(it == m_Providers.end())
//        return;

//    QSharedPointer<data_system::RasterDataProvider> rdp = data_system::DataProviderFactory::instance()->getProvider(id).dynamicCast<data_system::RasterDataProvider>();
//    if( ! rdp)
//        return;

//    if(rdp->getMatchingType() != data_system::MatchingType::Location)
//        return;

//    data_system::Location location;
//    if( ! rdp->getLocation(location) )
//        return;

//    double X(location.GPSLongitude), Y(location.GPSLatitude);
//    if(m_sceneDescriptor.convertEpsgToScene(4326, X, Y))
//    {
//        delete (*it).rasterLayer;
//        (*it).rasterLayer = new visualize_system::RasterLayer(id, QPointF(X, Y), location.GPSImgDirection, location.orientation, _rasterLayerInitData);
//        connect((*it).rasterLayer, SIGNAL(signalClicked(bool)), this, SLOT(rasterLayerClicked(bool)));
//        //(*it).rasterLayer->setDefaultPen(_defaultRasterSelectionMode == visualize_system::DefaultRasterSelectionMode::COLOR ? _rasterLayerInitData.defaultPen : QPen(QColor(Qt::transparent)));
//        QList<QGraphicsItem*>* list = new QList<QGraphicsItem*>;
//        list->append((*it).rasterLayer->getGraphicsItem());
//        emit signalItemsForDraw(list);
//    }
}

















