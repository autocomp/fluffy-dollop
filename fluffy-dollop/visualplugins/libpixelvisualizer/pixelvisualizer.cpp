#include "pixelvisualizer.h"
#include <ctrcore/visual/viewinterface.h>
#include <ctrcore/visual/datainterface.h>
#include <ctrcore/visual/vectordatainterface.h>
#include <ctrcore/visual/stateinterface.h>
#include <ctrcore/visual/selectionmodelinterface.h>
#include <ctrcore/visual/visualizermanager.h>

#include <ctrcore/plugin/ctrpluginloader.h>

#include <ctrcore/ctrcore/ctrconfig.h>

#include <ctrvisual/components/visualizercontrolpanel.h>

using namespace visualize_system;

PixelVisualizer::~PixelVisualizer()
{
    delete m_tileMixer;
//    delete m_controller;
//    delete m_viewerController;
//    delete m_scene2dWidget;

    delete m_sceneDescriptor;

    delete _viewInterface;
    delete _dataInterface;
    delete _vectorDataInterface;
    delete _stateInterface;
    delete _selectionModelInterface;


    if(vcp)
        delete vcp;
}

PixelVisualizer::PixelVisualizer() :
    VISUALIZER_WINDOW_ID(0),
    _viewInterface(0),
    _dataInterface(0),
    _vectorDataInterface(0),
    _stateInterface(0),
    _selectionModelInterface(0),
    m_sceneDescriptor(0),
    m_tileMixer(0),
    m_controller(0),
    m_viewerController(0),
    m_scene2dWidget(0)
{
}

void PixelVisualizer::init(uint visualizerId)
{
    VISUALIZER_ID = visualizerId;

    _viewInterface = new ViewInterface(*this);
    _dataInterface = new DataInterface(*this);
    _vectorDataInterface = new VectorDataInterface(*this);
    _stateInterface = new StateInterface(*this);
    _selectionModelInterface = new SelectionModelInterface(*this);

    m_sceneDescriptor = new PixelSceneDescriptor();

    m_controller = new SceneController(VISUALIZER_ID, *m_sceneDescriptor);
    m_controller->setTilesOnViewport(2); //! m_AddTileOnViewport - ПАРАМЕТР НЕОБХОДИМО ВЗЯТЬ ИЗ ВНЕ !!!

    m_tileMixer = new visualize_system::ConturTileMixer(VISUALIZER_ID, m_controller->getScene(), *m_sceneDescriptor);

    connect(m_tileMixer, SIGNAL(drawTile(const QPixmap&, const QString&)), m_controller, SIGNAL(signal_recivedImage(const QPixmap&, const QString&)));
    connect(m_tileMixer, SIGNAL(loadingFinished()), m_controller, SIGNAL(loadingFinished()));
    connect(m_tileMixer, SIGNAL(updateContent(bool)), m_controller, SLOT(updateContent(bool)));
    connect(m_tileMixer, SIGNAL(signalItemsForDraw(QList<QGraphicsItem*>*)), m_controller, SLOT(slotObjectCreated(QList<QGraphicsItem*>*)));
    connect(m_tileMixer, SIGNAL(initPixelScene(QSizeF,double,int,int,QString)), this, SLOT(initPixelScene(QSizeF,double,int,int,QString)));
    //connect(m_tileMixer, SIGNAL(initPixelScene(QPointF,double,int,int,QString)), this, SLOT(initPixelScene(QPointF,double,int,int,QString)));
    connect(m_controller, SIGNAL(signal_abortLoading()), m_tileMixer, SLOT(abortLoading()));
    connect(m_controller, SIGNAL(signal_getTiles(const QList<visualize_system::URL>&,int, QRectF)), m_tileMixer, SLOT(getTiles(QList<visualize_system::URL>,int, QRectF)));
    connect(m_controller, SIGNAL(signal_removeTiles(const QStringList&)), m_tileMixer, SLOT(removeTiles(const QStringList&)));
    connect(m_controller, SIGNAL(signal_doubleClickOnScene(QPointF)), m_tileMixer, SLOT(slot_DubleClickOnScene(QPointF)));
    //connect(m_controller, SIGNAL(signal_clickRBOnRaster(QPointF)), m_tileMixer, SLOT(slot_ClickRBOnRaster(QPointF)));

    connect(_selectionModelInterface, SIGNAL(signalSelectedProvidersChanged(QList<uint>,QList<uint>)),
            m_tileMixer, SLOT(slotSelectedProvidersChanged(QList<uint>,QList<uint>)));
    connect(_selectionModelInterface, SIGNAL(signalMarkedProvidersChanged(QList<uint>,QList<uint>)),
            m_tileMixer, SLOT(slotMarkedProvidersChanged(QList<uint>,QList<uint>)));

    connect(_dataInterface, SIGNAL(signalProviderPropertyChanged(uint,QString)),
            m_tileMixer, SLOT(slotProviderPropertyChanged(uint,QString)));

    m_viewerController = m_controller->addView();
    m_viewerController->setRenderHint(QPainter::Antialiasing, true);
    m_viewerController->setRenderHint(QPainter::HighQualityAntialiasing, true);
    m_viewerController->setRenderHint(QPainter::SmoothPixmapTransform, true);
    m_controller->setZoomLevels(-5, 5, 1);

    m_scene2dWidget = new sw::Scene2DWidget();

    m_scene2dWidget->setMainViewWidget(m_viewerController);

    m_scene2dWidget->setMinimapViewWidget(m_viewerController->getMinimap());
    m_viewerController->getMinimap()->show();

    connect(m_controller, SIGNAL(signalShowCoords(QString,QString,QString)), m_scene2dWidget, SLOT(slotSetCurrentCoords(QString,QString,QString)));

//    connect(m_viewerController, SIGNAL(signalSetAngle(qreal)), m_scene2dWidget->getKompasWidget(), SLOT(slotSetRotateAngle(qreal)));
//    connect(m_viewerController, SIGNAL(signalRotateBlocked(bool)), m_scene2dWidget->getKompasWidget(), SLOT(slotRotationBlocked(bool)));
//    connect(m_viewerController, SIGNAL(signalScaleChanged(int, int, int)), m_scene2dWidget, SLOT(slotScaleChanged(int, int, int)));
//    connect(m_viewerController, SIGNAL(curDistanceUpdateted(bool, double)), m_scene2dWidget, SLOT(slotShowDistance(bool, double)));
//    connect(m_viewerController, SIGNAL(sigSetInterpixelDistance(qreal)), m_scene2dWidget, SLOT(slotSetInterpixelDistance(qreal)));
//    connect(m_viewerController, SIGNAL(signalSwitchOffButton(int)), m_scene2dWidget, SLOT(slotSwitchOffButton(int)));

//    connect(m_scene2dWidget->getKompasWidget(), SIGNAL(sigBlockToNorth(bool)), m_viewerController, SLOT(slotBlockRotateFromKompas(bool)));
//    connect(m_scene2dWidget->getKompasWidget(), SIGNAL(sigContinuousRotate(qreal)), m_viewerController,SLOT(slotRotateFromKompas(qreal)));
//    connect(m_scene2dWidget->getKompasWidget(), SIGNAL(sigOneStepMove(Kompas::KompasOrientation)), m_viewerController, SLOT(slotOneStepMoveMap(Kompas::KompasOrientation)) );
//    connect(m_scene2dWidget->getKompasWidget(), SIGNAL(sigStartContinuousMove(Kompas::KompasOrientation)), m_viewerController, SLOT(slotContinuousMoveMap(Kompas::KompasOrientation)) );
//    connect(m_scene2dWidget->getKompasWidget(), SIGNAL(sigStopMove()), m_viewerController, SLOT(slotStopMoveMap()) );
//    connect(m_scene2dWidget->getZoomBar(), SIGNAL(zoomChanged(int)), m_viewerController, SLOT(changeZoom(int)));
//    connect(m_scene2dWidget, SIGNAL(signalSetCurrentInterpixelDistance(qreal)), m_viewerController, SLOT(slotSetCurrentInterpixelDistance(qreal) ));
//    connect(m_scene2dWidget, SIGNAL(signalButtonClicked(int,bool)), m_viewerController, SLOT(slotSceneWidgetButtonClicked(int,bool)) );
//    connect(m_scene2dWidget, SIGNAL(saveViewportImage()), m_viewerController, SLOT(slotSaveViewportImage()));
//    connect(m_scene2dWidget->getCoordBar(), SIGNAL(currentCoordSystemChanged(int)), m_controller, SLOT(slotCurCoordSystChanged(int)));
    connect(m_scene2dWidget, SIGNAL(newSize(int,int)), this, SLOT(sceneWidgetSizeChanged(int,int)));

    vcp = new VisualizerControlPanel(m_scene2dWidget);//widget());
    vcp->setItemSize(32,32);
    //m_scene2dWidget->addToolbarWidget(vcp);
    vcp->show();
}

uint PixelVisualizer::getVisualizerId()
{
    return VISUALIZER_ID;
}

VisualizerType PixelVisualizer::getVisualizerType()
{
    return VisualizerType::VisualizerPixel;
}

ViewInterface * PixelVisualizer::getViewInterface()
{
    return _viewInterface;
}

DataInterface * PixelVisualizer::getDataInterface()
{
    return _dataInterface;
}

VectorDataInterface * PixelVisualizer::getVectorInterface()
{
    return _vectorDataInterface;
}

StateInterface * PixelVisualizer::getStateInterface()
{
    return _stateInterface;
}

SelectionModelInterface * PixelVisualizer::getSelectionModelInterface()
{
    return _selectionModelInterface;
}

QList<uint> PixelVisualizer::addProviders(const QList<uint> &providers)
{
    return m_tileMixer->addProviders(providers);
}

QList<uint> PixelVisualizer::addBaseProviders(const QList<uint> &providers)
{
    return QList<uint>();
}

QList<uint> PixelVisualizer::removeProviders(const QList<uint> & providers)
{
    return m_tileMixer->removeProviders(providers);
}

QList<uint> PixelVisualizer::getProviders(ProviderType /*providerType*/)
{
    return m_tileMixer->getProviders(ProviderType::BaseCoverAndImages);
}

bool PixelVisualizer::setVisualizerState(QSharedPointer<visualize_system::VisualizerStateObject> state)
{
    return m_viewerController->setState(state);
}

QString PixelVisualizer::getCurrentState()
{
    return QString();
}

QRectF PixelVisualizer::getViewportRect()
{
    return QRectF();
}

void PixelVisualizer::centerOn(QPointF nativeCoords)
{

}

void PixelVisualizer::centerOn(QRectF rectf)
{
    if( !rectf.isNull() && rectf.isValid() )
    {
        m_controller->slotSetMainViewCenterOnGeoPoint(0, rectf, 20, 0,true);
    }
}

void PixelVisualizer::setZlevel(int zLevel)
{

}

int PixelVisualizer::getZlevel()
{
    return 0;
}

int PixelVisualizer::getMinZlevel()
{
    return 0;
}

int PixelVisualizer::getMaxZlevel()
{
    return 0;
}

void PixelVisualizer::setRotate(int angle)
{

}

int PixelVisualizer::getRotate()
{
    return 0;
}

void PixelVisualizer::blockRotate(bool on_off)
{

}

void PixelVisualizer::scrollLeft()
{

}

void PixelVisualizer::scrollRight()
{

}

void PixelVisualizer::scrollUp()
{

}

void PixelVisualizer::scrollDown()
{

}

void PixelVisualizer::setVisualizerWindowId(quint64 id)
{
    VISUALIZER_WINDOW_ID = id;
    createPlugins();
}

quint64 PixelVisualizer::getVisualizerWindowId()
{
    return VISUALIZER_WINDOW_ID;
}

sw::AbstractSceneWidget *PixelVisualizer::getAbstractSceneWidget()
{
    return m_scene2dWidget;
}

SceneDescriptor *PixelVisualizer::getSceneDescriptor()
{
    return m_sceneDescriptor;
}

QWidget* PixelVisualizer::widget()
{
    return m_scene2dWidget;
}

QGraphicsView *PixelVisualizer::graphicsView()
{
    return m_viewerController;
}

QWidget* PixelVisualizer::minimapWidget()
{
    return 0;
}

QPixmap PixelVisualizer::snapShort()
{
    return QPixmap();
}

QStackedWidget *PixelVisualizer::stackedWidget()
{
    return 0;
}

void PixelVisualizer::initPixelScene(QSizeF rasterSceneSize, double baseZlevel, int frameId, int empty, QString pixelVizFilePath)
{
    m_controller->initPixelScene(QPointF(rasterSceneSize.width(), rasterSceneSize.height()), baseZlevel, frameId, empty, pixelVizFilePath);

//    if(m_controller->frameIsMatching())
//    {
//        m_scene2dWidget->setOpennedDataType(sw::ODT_GEO_PICTURE);
//        qDebug() << "SceneComposer::initPixelScene --- IMAGE IS MATCHING !!!";
//    }
//    else
    {
        m_scene2dWidget->setOpennedDataType(sw::ODT_SIMLE_PICTURE);
//        qDebug() << "SceneComposer::initPixelScene --- IMAGE IS NOT MATCHING !!!";
    }

    m_scene2dWidget->getRowScale()->hide();
    m_scene2dWidget->getCoordBar()->clearComboBox();
    m_scene2dWidget->getCoordBar()->initPixelCoordSystems();
//    if(m_controller->frameIsMatching())
    //        m_scene2dWidget->getCoordBar()->initGeoCoordSystems();
}

void PixelVisualizer::createPlugins()
{
    loadPluginCategory("testPlugins");
}

void PixelVisualizer::slotPluginActivated(bool checked)
{
    QPair<ctrplugin::InitPluginData, CtrAppVisualizerPlugin*> struct_plugin;
    struct_plugin = m_ActionsToPlugins.value((QAction*)sender());
    ctrplugin::InitPluginData initPlgDta = struct_plugin.first;

    if(sender()->property("buttonName").toString() == initPlgDta.buttonName)
    {
        if(initPlgDta.isCheckable)
        {
            struct_plugin.second->checked(initPlgDta.buttonName, checked);
        }
        else
        {
            struct_plugin.second->checked(initPlgDta.buttonName,true);
        }
        saveSettings(struct_plugin);
    }
}

void PixelVisualizer::slotPluginChecked(const QString &buttonName, bool checked)
{
    CtrAppVisualizerPlugin *plugin = ((CtrAppVisualizerPlugin*)sender());
    QList<QAction*> keys = m_ActionsToPlugins.keys();
    QPair<ctrplugin::InitPluginData, CtrAppVisualizerPlugin*> struct_plugin;

    for(QAction* pCurrentAction : keys)
    {
        struct_plugin = m_ActionsToPlugins[pCurrentAction];

        if(QString(struct_plugin.first.buttonName) == buttonName)
        {
            disconnect(plugin, SIGNAL(setChecked(QString,bool)), this, SLOT(slotPluginChecked(QString,bool)));
            plugin->checked(buttonName, checked);
            pCurrentAction->setChecked(checked);
            saveSettings(struct_plugin);
            connect(plugin, SIGNAL(setChecked(QString,bool)), this, SLOT(slotPluginChecked(QString,bool)));
            break;
        }
    }
}

void PixelVisualizer::sceneWidgetSizeChanged(int newW, int newH)
{
    if(vcp)
        vcp->movePanel(newW, newH);
}

void PixelVisualizer::panelVisibleChanged(bool tf)
{
    QString key ;
    key = visualize_system::VisualizerManager::convertVisualizerType(getVisualizerType()).toLower()+"_control_panel.menuSettings.visiblity";
    QString visiblity = (tf)?"showed":"hidden";
    CtrConfig::setValueByName(key, visiblity);
}

void PixelVisualizer::loadPluginCategory(QString pluginType)
{
    QString key =
        QString("visualizer_plugins_config.") +
        visualize_system::VisualizerManager::convertVisualizerType(getVisualizerType()) +
        QString(".") +
        pluginType +
        QString(".plugin");

    QStringList filesInXml = CtrConfig::getStringListByName(key);

    CtrPluginLoader* loader = CtrPluginLoader::instance();

    foreach (QString file, filesInXml)
    {
        CtrAppVisualizerPlugin* plugin = dynamic_cast<CtrAppVisualizerPlugin*>(loader->getPlugin(TYPE_APP, file));
        QPair<ctrplugin::InitPluginData, CtrAppVisualizerPlugin*> struct_plugin;
        if(plugin)
          {
            plugin->init(getVisualizerId(), getVisualizerWindowId());

            using namespace ctrplugin;
            QList<InitPluginData> list = plugin->getInitPluginData();
            for(auto it = list.begin(); it != list.end(); ++it)
            {
                InitPluginData currentPluginData = *it;
                QAction *pDefAct = vcp->addItem(currentPluginData.buttonName, currentPluginData.translateButtonName, currentPluginData.iconForButtonOn, currentPluginData.tooltip, currentPluginData.isCheckable);

                pDefAct->setProperty("buttonName", currentPluginData.buttonName);
                pDefAct->setProperty("pluginType", pluginType);

                struct_plugin.first = currentPluginData;
                struct_plugin.second = plugin;

                m_ActionsToPlugins.insert(pDefAct, struct_plugin);

                connect(pDefAct, SIGNAL(triggered(bool)), this, SLOT(slotPluginActivated(bool)));
                connect(plugin, SIGNAL(setChecked(QString,bool)), this, SLOT(slotPluginChecked(QString,bool)));
                connect(plugin, SIGNAL(setEnabled(QString,bool)), vcp, SLOT(setPluginButtonEnabled(QString,bool)));
            }
        }
    }

      if(!filesInXml.isEmpty())
      {
          vcp->addSeparator();
      }
      readSettings();
      m_scene2dWidget->slotUpdateChildsGeometry();
}

void PixelVisualizer::readSettings()
{
    /// РАЗМЕР КНОПОК И РАЗМЕРЫ САМОЙ ПАНЕЛИ
    ///
    QString key = visualize_system::VisualizerManager::convertVisualizerType(getVisualizerType()).toLower()+"_control_panel.menuSettings.buttonSize";
    QString sizeString;
    sizeString = GET_VALUE_BY_NAME(key, "32x32", true).toString();
    QSize itemSize = QSize(sizeString.mid(0,sizeString.indexOf("x")).toInt(), sizeString.mid(sizeString.indexOf("x")+1).toInt());
    if(vcp)
    {
        vcp->setItemSize(itemSize.width(), itemSize.height());
        connect(vcp, SIGNAL(visibleChanged(bool)), this, SLOT(panelVisibleChanged(bool)));
        QRect r(m_scene2dWidget->geometry());
        vcp->movePanel(r.width(), r.height());
    }

    /// ВИДИМОСТЬ ПАНЕЛИ
    ///
    key = visualize_system::VisualizerManager::convertVisualizerType(getVisualizerType()).toLower()+"_control_panel.menuSettings.visiblity";
    QString menuVisiblity = GET_VALUE_BY_NAME(key, "hidden", true).toString();

    if(menuVisiblity == "hidden")
    {
        vcp->hidePanel();
    }
    else
    if(menuVisiblity == "showed")
    {
        vcp->showPanel();
    }

    /// ВИДИМОСТЬ ПЛАГИНОВ
    ///
    QList<QAction*> keys = m_ActionsToPlugins.keys();

    for(QAction* pCurrentAction : keys)
    {
        QPair<ctrplugin::InitPluginData, CtrAppVisualizerPlugin*> struct_plugin;
        struct_plugin = m_ActionsToPlugins[pCurrentAction];

        QString pluginType = pCurrentAction->property("pluginType").toString();
        if(pluginType == "measurePlugins")
            continue;

        QString key ;
        key = visualize_system::VisualizerManager::convertVisualizerType(getVisualizerType()).toLower()+"_control_panel."+pluginType+".buttonsSettings."+struct_plugin.first.buttonName+".activated" ;
        bool activated = GET_VALUE_BY_NAME(key, QVariant(false), true).toBool();
        if(struct_plugin.first.isCheckable)
        {
            struct_plugin.second->checked(struct_plugin.first.buttonName, activated);
            pCurrentAction->setChecked(activated);
        }
        else
        {
            if(activated)
                struct_plugin.second->checked(struct_plugin.first.buttonName, true);
        }
    }
}

void PixelVisualizer::saveSettings(QPair<InitPluginData, CtrAppVisualizerPlugin *> &_struct_plugin)
{

}

void PixelVisualizer::saveSetting(const QString &key, const QString &value)
{

}

uint32_t PixelVisualizer::getVisualizerCursorId()
{
    return 0;
}

void PixelVisualizer::repaintVisualizer()
{

}

void PixelVisualizer::addToolButton(QToolButton* tb)
{

}

void PixelVisualizer::addActionToObjectMenu(QList<QAction *> actions, QString groupName, uint groupWeight, QString subMenuName)
{

}

void PixelVisualizer::addActionToRasterMenu(QList<QAction *> actions, QString groupName, uint groupWeight, QString subMenuName)
{

}













