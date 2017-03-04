#include "visualizer2d.h"
#include <ctrcore/visual/viewinterface.h>
#include <ctrcore/visual/datainterface.h>
#include <ctrcore/visual/vectordatainterface.h>
#include <ctrcore/visual/stateinterface.h>
#include <ctrcore/visual/selectionmodelinterface.h>
#include <ctrrasterrender/tms/tileloaderpool.h>
#include <ctrvisual/scenewidget/scenewidget.h>
#include <ctrvisual/components/visualizercontrolpanel.h>
#include <ctrcore/visual/visualizermanager.h>
#include <ctrcore/plugin/ctrpluginloader.h>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <libembeddedwidgets/embeddedapp.h>
#include <ctrcore/provider/sincxmlloader.h>
#include <QHBoxLayout>
#include <QDomDocument>
#include <QTranslator>
#include <QCoreApplication>

using namespace visualize_system;

Visualizer2d::~Visualizer2d()
{
    if(vcp)
   delete vcp;
}

Visualizer2d::Visualizer2d() :
    VISUALIZER_WINDOW_ID(0),
    VISUALIZER_CURSOR_ID(0),
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

void Visualizer2d::init(uint visualizerId)
{
    QTranslator *trans = new QTranslator();
    if(trans->load(":/libvisualizer2d.qm"))
        QCoreApplication::installTranslator(trans);

    VISUALIZER_ID = visualizerId;

    _viewInterface = new ViewInterface(*this);
    _dataInterface = new DataInterface(*this);
    _vectorDataInterface = new VectorDataInterface(*this);
    _stateInterface = new StateInterface(*this);
    _selectionModelInterface = new SelectionModelInterface(*this);
    m_pGlobalNotif = new GlobalMessageNotifier(this);

    QVariant gridName_var = CtrConfig::getValueByName(QString("application_settings.GridName"));
    if(gridName_var.isValid() == false)
    {
        gridName_var = QString("gridwgs");
        CtrConfig::setValueByName(QString("application_settings.GridName"), gridName_var);
    }
    const QString GRID_NAME(gridName_var.toString());
    QList<TmsXmlData> tmsDataList;
    QByteArray byteArray;
    QVariant tmsXMLSource_var = CtrConfig::getValueByName(QString("application_settings.TmsXMLSource"));
    if(tmsXMLSource_var.isValid())
    {
        if(QFileInfo(tmsXMLSource_var.toString()).exists())
        {
            QFile f(tmsXMLSource_var.toString());
            if(f.open(QFile::ReadOnly))
            {
                byteArray = f.readAll();
                f.close();
            }
        }
        else
        {
            QUrl url(tmsXMLSource_var.toString());
            //! FIXME вставить установку порта и лимита времени из ХМЛя !
            bool res = SincXmlLoader::loadXml(url, byteArray);
            if( ! res)
                byteArray = QByteArray();
        }
        if(byteArray.isEmpty() == false)
            getTmsXmlData(byteArray, tmsDataList);
    }
    bool gridFound(false);
    if(tmsDataList.isEmpty() == false)
        foreach(TmsXmlData tmsXmlData, tmsDataList)
            if(tmsXmlData.name == GRID_NAME)
            {
                gridFound = true;
                break;
            }
    qDebug() << "gridFound :" << gridFound << ", GRID_NAME :" << GRID_NAME;

    uint baseTileSetOrder(1);
    if(gridFound == false)
        m_sceneDescriptor = new GeoSceneDescriptor(getDefaultXml(), QString("gridmerc"), baseTileSetOrder);
    else
        m_sceneDescriptor = new GeoSceneDescriptor(QString(byteArray), GRID_NAME, baseTileSetOrder);

    TileLoaderPool::instance()->serWorldSizeInTiles(m_sceneDescriptor->sceneSizeInTiles());

    m_controller = new SceneController(VISUALIZER_ID, *m_sceneDescriptor);
    m_controller->setTilesOnViewport(2); //! m_AddTileOnViewport - ПАРАМЕТР НЕОБХОДИМО ВЗЯТЬ ИЗ ВНЕ !!!
    if(m_sceneDescriptor->isGeoScene())
        if(false) //! m_GeoViewOptimization - ПАРАМЕТР НЕОБХОДИМО ВЗЯТЬ ИЗ ВНЕ !!!
            m_controller->setViewOptimization();

    m_tileMixer = new visualize_system::ConturTileMixer(VISUALIZER_ID, m_controller->getScene(), *m_sceneDescriptor);

    connect(m_tileMixer, SIGNAL(drawTile(const QPixmap&, const QString&)), m_controller, SIGNAL(signal_recivedImage(const QPixmap&, const QString&)));
    connect(m_tileMixer, SIGNAL(loadingFinished()), m_controller, SIGNAL(loadingFinished()));
    connect(m_tileMixer, SIGNAL(updateContent(bool)), m_controller, SLOT(updateContent(bool)));
    connect(m_tileMixer, SIGNAL(signalItemsForDraw(QList<QGraphicsItem*>*)), m_controller, SLOT(slotObjectCreated(QList<QGraphicsItem*>*)));
    connect(m_tileMixer, SIGNAL(signalGetViewportRect(QRectF&)), m_controller, SLOT(getViewportRect(QRectF&)));
    connect(m_tileMixer, SIGNAL(signalProviderMatched(uint)), _dataInterface, SIGNAL(signalProviderMatched(uint)));
    connect(m_tileMixer, SIGNAL(signalSetWidgetsEnabled(bool)), _viewInterface, SIGNAL(signalSetWidgetsEnabled(bool)));
    connect(m_tileMixer, SIGNAL(signalConvertSceneCoordToGlobal(QPointF&)), m_controller, SLOT(slotConvertSceneCoordToGlobal(QPointF&)));

    connect(m_controller, SIGNAL(signal_abortLoading()), m_tileMixer, SLOT(abortLoading()));
    connect(m_controller, SIGNAL(signal_getTiles(const QList<visualize_system::URL>&,int, QRectF)), m_tileMixer, SLOT(getTiles(QList<visualize_system::URL>,int, QRectF)));
    connect(m_controller, SIGNAL(signal_removeTiles(const QStringList&)), m_tileMixer, SLOT(removeTiles(const QStringList&)));
    connect(m_controller, SIGNAL(signal_doubleClickOnScene(QPointF)), m_tileMixer, SLOT(slot_DubleClickOnScene(QPointF)));
    connect(m_controller, SIGNAL(signal_clickRBOnRaster(uint, QPointF, bool)), m_tileMixer, SLOT(slot_ClickRBOnRaster(uint, QPointF, bool)));
    connect(m_controller, SIGNAL(signal_clickRBOnObject(quint64)), m_tileMixer, SLOT(slot_ClickRBOnObject(quint64)));

    connect(_selectionModelInterface, SIGNAL(signalSelectedProvidersChanged(QList<uint>,QList<uint>)),
            m_tileMixer, SLOT(slotSelectedProvidersChanged(QList<uint>,QList<uint>)));
    connect(_selectionModelInterface, SIGNAL(signalMarkedProvidersChanged(QList<uint>,QList<uint>)),
            m_tileMixer, SLOT(slotMarkedProvidersChanged(QList<uint>,QList<uint>)));

    connect(_dataInterface, SIGNAL(signalProviderPropertyChanged(uint,QString)),
            m_tileMixer, SLOT(slotProviderPropertyChanged(uint,QString)));

    m_viewerController = m_controller->addView();

    m_scene2dWidget = new sw::Scene2DWidget();

    m_scene2dWidget->setMainViewWidget(m_viewerController);

    m_scene2dWidget->setMinimapViewWidget(m_viewerController->getMinimap());
    m_viewerController->getMinimap()->show();

    connect(m_controller, SIGNAL(signalShowCoords(QString,QString,QString)), m_scene2dWidget, SLOT(slotSetCurrentCoords(QString,QString,QString)));

//    connect(m_viewerController, SIGNAL(signalSetAngle(qreal)), m_scene2dWidget->getKompasWidget(), SLOT(slotSetRotateAngle(qreal)));
//    connect(m_viewerController, SIGNAL(signalRotateBlocked(bool)), m_scene2dWidget->getKompasWidget(), SLOT(slotRotationBlocked(bool)));
    connect(m_viewerController, SIGNAL(signalScaleChanged(int, int, int)), m_scene2dWidget, SLOT(slotScaleChanged(int, int, int)));
    connect(m_viewerController, SIGNAL(curDistanceUpdateted(bool, double)), m_scene2dWidget, SLOT(slotShowDistance(bool, double)));
    connect(m_viewerController, SIGNAL(sigSetInterpixelDistance(qreal)), m_scene2dWidget, SLOT(slotSetInterpixelDistance(qreal)));
    //    connect(m_viewerController, SIGNAL(signalFindOSMObjectsInRect(QRectF, bool)),m_scene2dWidget, SLOT(slotShowSearchOSMWidget(QRectF, bool)));
    connect(m_viewerController, SIGNAL(signalSwitchOffButton(int)), m_scene2dWidget, SLOT(slotSwitchOffButton(int)));

//    connect(m_scene2dWidget->getKompasWidget(), SIGNAL(sigBlockToNorth(bool)), m_viewerController, SLOT(slotBlockRotateFromKompas(bool)));
//    connect(m_scene2dWidget->getKompasWidget(), SIGNAL(sigContinuousRotate(qreal)), m_viewerController,SLOT(slotRotateFromKompas(qreal)));
//    connect(m_scene2dWidget->getKompasWidget(), SIGNAL(sigOneStepMove(Kompas::KompasOrientation)), m_viewerController, SLOT(slotOneStepMoveMap(Kompas::KompasOrientation)) );
//    connect(m_scene2dWidget->getKompasWidget(), SIGNAL(sigStartContinuousMove(Kompas::KompasOrientation)), m_viewerController, SLOT(slotContinuousMoveMap(Kompas::KompasOrientation)) );
//    connect(m_scene2dWidget->getKompasWidget(), SIGNAL(sigStopMove()), m_viewerController, SLOT(slotStopMoveMap()) );
//    connect(m_scene2dWidget->getZoomBar(), SIGNAL(zoomChanged(int)), m_viewerController, SLOT(changeZoom(int)));
    connect(m_scene2dWidget, SIGNAL(signalSetCurrentInterpixelDistance(qreal)), m_viewerController, SLOT(slotSetCurrentInterpixelDistance(qreal) ));
    connect(m_scene2dWidget, SIGNAL(signalButtonClicked(int,bool)), m_viewerController, SLOT(slotSceneWidgetButtonClicked(int,bool)) );
    connect(m_scene2dWidget, SIGNAL(saveViewportImage()), m_viewerController, SLOT(slotSaveViewportImage()));
    connect(m_scene2dWidget->getCoordBar(), SIGNAL(currentCoordSystemChanged(int)), m_controller, SLOT(slotCurCoordSystChanged(int)));
    connect(m_scene2dWidget, SIGNAL(newSize(int,int)), this, SLOT(sceneWidgetSizeChanged(int,int)));


//    m_pVectorRender = new VectorRender(VectorRender::ST_GEO_WGS84,this);
//    m_pVectorRender->slotInitGeoCursor(" ");

//    m_pVectorRender->setTileInfo(m_sceneDescriptor->tileSize(), m_sceneDescriptor->sceneSizeInTiles());
//    connect(m_pVectorRender, SIGNAL(signalCursorCreated(uint32_t)), this, SLOT( slot2DCursorCreated(uint32_t) ) );

//    connect( m_controller, SIGNAL(signalUserViewControlAction(UserAction)), this, SLOT(slotUserViewControlAction(UserAction)) );
//    connect(m_controller, SIGNAL(readyDraw(QList<QGraphicsItem*>*)), m_pVectorRender, SLOT(slotSendNewDataForDraw(QList<QGraphicsItem*>*)));
//    connect(m_pVectorRender, SIGNAL(signalItemsForDraw(QList<QGraphicsItem*>*)), m_controller, SLOT(slotObjectCreated(QList<QGraphicsItem*>*)));
//    connect( m_pVectorRender, SIGNAL(signalSetMapViewOnGeoPoint(quint64, QRectF, int,int, bool, bool)), m_controller, SLOT(slotSetMainViewCenterOnGeoPoint(quint64, QRectF, int,int, bool, bool))  );
//    connect(m_pVectorRender, SIGNAL( signalConvertPixelToGeo(QPointF, QPointF*) ), m_controller,  SLOT(slotConvertPixelToGeo(QPointF, QPointF*)));
//    connect(m_pVectorRender, SIGNAL( signalConvertGeoToPixel(QPointF, QPointF*) ), m_controller,  SLOT(slotConvertGeoToPixel(QPointF, QPointF*)));
//    connect(m_pVectorRender, SIGNAL( signalGetInterpixelDistance(qreal*) ), m_controller,  SLOT(slotGetInterpixelDistance(qreal*)));
//    connect(m_controller, SIGNAL(signalMainViewScaleChanged(qreal, int)), m_pVectorRender, SLOT(slotScaleChanged(qreal, int)) );
//    connect(m_controller, SIGNAL(signalMainViewAngleChanged(qreal)), m_pVectorRender, SLOT(slotAngleChanged(qreal)) );
//    connect(m_pVectorRender, SIGNAL( signalObjectSelected(uint64_t) ),this, SIGNAL(signalObjectSelected(uint64_t)));
//    connect(m_pVectorRender, SIGNAL(signalObjectSelectionChanged(uint64_t,bool)), this, SLOT(slotObjectSelectionChanged(uint64_t, bool))  );

//    connect( m_pVectorRender, SIGNAL(signalAskLoadedContextName()), this, SIGNAL( signalAskLoadedContextName() )  );
//    connect( m_pVectorRender, SIGNAL(signalItemChangedFromUser(uint64_t)), this, SIGNAL( signalItemChangedFromUser(uint64_t) ) );
//    connect( m_pVectorRender, SIGNAL(signalEditPointSelectedFromMap(qulonglong,uint,uint,uint)), this, SIGNAL(signalEditPointSelectedFromMap(qulonglong,uint,uint,uint)) );
//    connect( m_pVectorRender, SIGNAL(signalGetInterMeterDistance(qreal*)), m_controller, SLOT(slotGetInterMeterDistance(qreal*)));
//    connect(m_pVectorRender, SIGNAL(signalBlockSceneScroll(bool)), m_controller, SLOT(slotBlockSceneScroll(bool)));
//    connect(m_pVectorRender, SIGNAL(signalBindLayer(quint64, bool)), m_controller, SLOT(slotBindLayer(quint64, bool)));
//    connect(m_pVectorRender, SIGNAL( signalAttachObject(quint64)), this, SIGNAL(signalObjectAttached(quint64)) );
//    connect(m_pVectorRender, SIGNAL(signalEditedListUpdated(QList<quint64>*)),
//            this, SIGNAL(signalEditedListUpdated(QList<quint64>*)));
//    connect(m_pVectorRender, SIGNAL(signalGetVectorObjectStyle()), this, SLOT(slotGetVectorObjectStyleRequest()));

//    connect(m_pVectorRender, SIGNAL(signalCreateRasterProvider(QString, contur::common::ProviderDelegate**)), this, SIGNAL(signalCreateRasterProvider(QString, contur::common::ProviderDelegate**)));
//    connect(m_pVectorRender, SIGNAL(signalRemoveRasterProvider(QString)), this, SIGNAL(signalRemoveRasterProvider(QString)));

//    connect(m_viewerController, SIGNAL(signalSelectObject(quint64)), m_pVectorRender, SLOT(slotSelectObjectFromMap(quint64)));
//    connect(m_viewerController, SIGNAL(signalSetSelectedEditPoints(QList<QGraphicsItem *>)), m_pVectorRender, SLOT(slotSelEditPoint(QList<QGraphicsItem *>)));
//    connect(m_pVectorRender, SIGNAL(signalChangeEntityMode(quint32,bool,QPixmap)),m_viewerController, SLOT(slotChangeEntityMode(quint32,bool,QPixmap)));


    QVariant screenCenterX_Var = CtrConfig::getValueByName(QString("visualizer_system_settings.lastGeoscenePosition_ScreenCenterX"));
    QVariant screenCenterY_Var = CtrConfig::getValueByName(QString("visualizer_system_settings.lastGeoscenePosition_ScreenCenterY"));
    QVariant screenCenterZ_Var = CtrConfig::getValueByName(QString("visualizer_system_settings.lastGeoscenePosition_ZLevel"));
    if(screenCenterX_Var.isValid() && screenCenterY_Var.isValid() && screenCenterZ_Var.isValid())
    {
        int zLevel = screenCenterZ_Var.toInt();
        m_viewerController->changeZoom(zLevel, false);

        m_centerScenePos = QPointF(screenCenterX_Var.toDouble(), screenCenterY_Var.toDouble());
        QTimer::singleShot(100, this, SLOT(slotCenterViewOn()));
    }

    if(!vcp)
    {
        vcp = new VisualizerControlPanel(m_scene2dWidget);
        vcp->setItemSize(32,32);
        connect(vcp, SIGNAL(visibleChanged(bool)), this, SLOT(panelVisibleChanged(bool)));
        vcp->show();
    }

}

void Visualizer2d::slotCenterViewOn()
{
    m_viewerController->centerOn(m_centerScenePos);
}



void Visualizer2d::loadPluginCategory(QString pluginType)
{
  QString key =
      QString("visualizer_plugins_config.") +
      visualize_system::VisualizerManager::convertVisualizerType(getVisualizerType()) +
      QString(".") +
      pluginType +
      QString(".plugin");

  QStringList filesInXml = CtrConfig::getStringListByName(key);

  CtrPluginLoader* loader = CtrPluginLoader::instance();
  QList<QAction*> menuActions;

  foreach (QString file, filesInXml)
  {
      CtrAppVisualizerPlugin* plugin = dynamic_cast<CtrAppVisualizerPlugin*>(loader->getPlugin(TYPE_APP, file));
      QPair<ctrplugin::InitPluginData, CtrAppVisualizerPlugin*> struct_plugin;
      if(plugin)
      {
//          if(!vcp)
//          {
//              vcp = new VisualizerControlPanel(m_scene2dWidget);
//              vcp->setItemSize(32,32);
//              connect(vcp, SIGNAL(visibleChanged(bool)), this, SLOT(panelVisibleChanged(bool)));
//             // m_scene2dWidget->addToolbarWidget(vcp);
//              vcp->show();
//          }
          plugin->init(getVisualizerId(), getVisualizerWindowId());

          using namespace ctrplugin;

          QList<InitPluginData> list = plugin->getInitPluginData();
          QAction *pAct;
          for(auto it = list.begin(); it != list.end(); ++it)
          {
              InitPluginData currentPluginData = *it;

              if(currentPluginData.integrateWay == ctrplugin::IntegrateWay::MenuItem)
              {
                  pAct = new QAction(0);

                  pAct->setProperty("buttonName", currentPluginData.buttonName);
                  pAct->setProperty("pluginType", pluginType);
                  pAct->setIcon(currentPluginData.iconForButtonOn);
                  pAct->setText(currentPluginData.translateButtonName);
                  pAct->setToolTip(currentPluginData.tooltip);
                  pAct->setCheckable(currentPluginData.isCheckable);

                  connect(pAct, SIGNAL(triggered(bool)), this, SLOT(slotPluginActivated(bool)));
                  connect(plugin, SIGNAL(setChecked(QString,bool)), this, SLOT(slotPluginChecked(QString,bool)));
                  connect(plugin, SIGNAL(setEnabled(QString,bool)), vcp, SLOT(setPluginButtonEnabled(QString,bool)));
                  menuActions.append(pAct);

                  vcp->addMenuItem( menuActions, tr("Widgets"), vcp->getMenuIcon(), tr("Widgets"));
              }
              else
              {
                  pAct = vcp->addItem(currentPluginData.buttonName, currentPluginData.translateButtonName, currentPluginData.iconForButtonOn, currentPluginData.tooltip, currentPluginData.isCheckable);

                  pAct->setProperty("buttonName", currentPluginData.buttonName);
                  pAct->setProperty("pluginType", pluginType);


                  connect(pAct, SIGNAL(triggered(bool)), this, SLOT(slotPluginActivated(bool)));
                  connect(plugin, SIGNAL(setChecked(QString,bool)), this, SLOT(slotPluginChecked(QString,bool)));
                  connect(plugin, SIGNAL(setEnabled(QString,bool)), vcp, SLOT(setPluginButtonEnabled(QString,bool)));
              }

              struct_plugin.first = currentPluginData;
              struct_plugin.second = plugin;

              m_ActionsToPlugins.insert(pAct, struct_plugin);
          }
      }
  }
}

void Visualizer2d::readSettings()
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

void Visualizer2d::saveSettings(QPair<InitPluginData, CtrAppVisualizerPlugin *> &_struct_plugin)
{
    QList<QAction*> keys = m_ActionsToPlugins.keys();

    for(QAction* pCurrentAction : keys)
    {
        QPair<ctrplugin::InitPluginData, CtrAppVisualizerPlugin*> struct_plugin;
        struct_plugin = m_ActionsToPlugins[pCurrentAction];
        if(struct_plugin.first.buttonName == _struct_plugin.first.buttonName)
        {
            QString pluginType = pCurrentAction->property("pluginType").toString();
            if(pluginType == "measurePlugins")
                return;

            ctrplugin::InitPluginData currentPluginData = struct_plugin.first;

            QString key ;
            key = visualize_system::VisualizerManager::convertVisualizerType(getVisualizerType()).toLower()+"_control_panel."+pluginType+".buttonsSettings."+currentPluginData.buttonName+".activated" ;
            bool isChecked = struct_plugin.second->isChecked(currentPluginData.buttonName);
            CtrConfig::setValueByName(key, isChecked);
            break;
        }
    }
}

void Visualizer2d::createPlugins()
{
    vcp->init(getVisualizerWindowId());

    loadPluginCategory("activeDataPlugins");

    vcp->addSeparator();

   ///MEASURE PLUGINS
    CtrAppVisualizerPlugin* plugin = m_scene2dWidget->getMeasurePlugin();
    QAction *pDefAct;
    plugin->init(getVisualizerId(), getVisualizerWindowId());

    QList<ctrplugin::InitPluginData> initPlgDta = plugin->getInitPluginData();
    for(ctrplugin::InitPluginData currentPluginData: initPlgDta)
    {
        QPair<ctrplugin::InitPluginData, CtrAppVisualizerPlugin*> struct_plugin;

        pDefAct = vcp->addItem(currentPluginData.buttonName, currentPluginData.translateButtonName, currentPluginData.iconForButtonOn, currentPluginData.tooltip, currentPluginData.isCheckable);
        pDefAct->setProperty("buttonName", currentPluginData.buttonName);
        pDefAct->setProperty("pluginType", "measurePlugins");

        struct_plugin.first = currentPluginData;
        struct_plugin.second = plugin;

        connect(pDefAct, SIGNAL(triggered(bool)), this, SLOT(slotPluginActivated(bool)));
        connect(plugin, SIGNAL(setChecked(QString,bool)), this, SLOT(slotPluginChecked(QString,bool)));
        connect(plugin, SIGNAL(setEnabled(QString,bool)), vcp, SLOT(setPluginButtonEnabled(QString,bool)));

        m_ActionsToPlugins.insert(pDefAct, struct_plugin);
    }

    vcp->addSeparator();

    loadPluginCategory("widgetPlugins");

    ///WIDGET PLUGINS
    QList<QAction*> menuActions;

    plugin = m_scene2dWidget->getControlsWidgetsPlugin();
    plugin->init(getVisualizerId(), getVisualizerWindowId());

    initPlgDta = plugin->getInitPluginData();

    for(ctrplugin::InitPluginData currentPluginData: initPlgDta)
    {
        QPair<ctrplugin::InitPluginData, CtrAppVisualizerPlugin*> struct_plugin;
        ///pDefAct = vcp->addItem(currentPluginData.translateButtonName, currentPluginData.iconForButtonOn, currentPluginData.tooltip, currentPluginData.isCheckable);
        QAction *pAct = new QAction(0);

        pAct->setProperty("buttonName", currentPluginData.buttonName);
        pAct->setProperty("pluginType", "fakeWidgetPlugins");
        pAct->setIcon(currentPluginData.iconForButtonOn);
        pAct->setText(currentPluginData.translateButtonName);
        pAct->setToolTip(currentPluginData.tooltip);
        pAct->setCheckable(currentPluginData.isCheckable);

        connect(pAct, SIGNAL(triggered(bool)), this, SLOT(slotPluginActivated(bool)));
        connect(plugin, SIGNAL(setChecked(QString,bool)), this, SLOT(slotPluginChecked(QString,bool)));
        connect(plugin, SIGNAL(setEnabled(QString,bool)), vcp, SLOT(setPluginButtonEnabled(QString,bool)));
        menuActions.append(pAct);

        struct_plugin.first = currentPluginData;
        struct_plugin.second = plugin;

        m_ActionsToPlugins.insert(pAct, struct_plugin);


    }
    vcp->addMenuItem( menuActions, tr("Widgets"), vcp->getMenuIcon(), tr("Widgets"));

    readSettings();
}

void Visualizer2d::slotUserViewControlAction(UserAction act)
{
    globalbusmessages::UserActionOnMap gAct;
    gAct.buttons = act.buttons;
    gAct.epsgCode = act.epsgCode;
    gAct.keybKey = act.keybKey;
    gAct.modifiers = act.modifiers;
    gAct.posf = act.posf;
    gAct.sceneId = act.sceneId;
    gAct.sceneType = act.sceneType;
    gAct.type = act.type;

    m_pGlobalNotif->sendMessageData(globalbusmessages::GMT_USER_ACTION_ON_MAP, QVariant::fromValue(gAct));
}

void Visualizer2d::slotPluginActivated(bool checked)
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

void Visualizer2d::slotPluginChecked(const QString &buttonName, bool checked)
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

void Visualizer2d::slot2DCursorCreated(uint32_t id)
{
    VISUALIZER_CURSOR_ID = id;
}

void Visualizer2d::sceneWidgetSizeChanged(int newW, int newH)
{
    if(vcp)
        vcp->movePanel(newW, newH);
}

void Visualizer2d::panelVisibleChanged(bool tf)
{
    QString key ;
    key = visualize_system::VisualizerManager::convertVisualizerType(getVisualizerType()).toLower()+"_control_panel.menuSettings.visiblity";
    QString visiblity = (tf)?"showed":"hidden";
    CtrConfig::setValueByName(key, visiblity);
}

uint Visualizer2d::getVisualizerId()
{
    return VISUALIZER_ID;
}

VisualizerType Visualizer2d::getVisualizerType()
{
    return VisualizerType::Visualizer2D;
}

ViewInterface * Visualizer2d::getViewInterface()
{
    return _viewInterface;
}

DataInterface * Visualizer2d::getDataInterface()
{
    return _dataInterface;
}

VectorDataInterface * Visualizer2d::getVectorInterface()
{
    return _vectorDataInterface;
}

StateInterface * Visualizer2d::getStateInterface()
{
    return _stateInterface;
}

SelectionModelInterface * Visualizer2d::getSelectionModelInterface()
{
    return _selectionModelInterface;
}

QList<uint> Visualizer2d::addProviders(const QList<uint> &providers)
{
    return m_tileMixer->addProviders(providers);
}

QList<uint> Visualizer2d::addBaseProviders(const QList<uint> &providers)
{
    return m_tileMixer->addBaseProviders(providers);
}

QList<uint> Visualizer2d::removeProviders(const QList<uint> & providers)
{
    return m_tileMixer->removeProviders(providers);
}

QList<uint> Visualizer2d::getProviders(ProviderType providerType)
{
    return m_tileMixer->getProviders(providerType);
}

bool Visualizer2d::setVisualizerState(QSharedPointer<visualize_system::VisualizerStateObject> state)
{
    return m_viewerController->setState(state);
}

QString Visualizer2d::getCurrentState()
{
    return QString();
}

QRectF Visualizer2d::getViewportRect()
{
    return m_controller->getViewportRect();
}

void Visualizer2d::centerOn(QPointF nativeCoords)
{
    double X(nativeCoords.x());
    double Y(nativeCoords.y());
    if(m_sceneDescriptor->convertNativeToScene(X,Y))
        m_viewerController->viewCenterOn(QPointF(X,Y));
}

void Visualizer2d::centerOn(QRectF rectf)
{
    if( !rectf.isNull() && rectf.isValid() )
    {
        m_viewerController->setPrefferZoomForGeoRect(rectf);
    }
}

void Visualizer2d::setZlevel(int zLevel)
{

}

int Visualizer2d::getZlevel()
{
    return 0;
}

int Visualizer2d::getMinZlevel()
{
    return 0;
}

int Visualizer2d::getMaxZlevel()
{
    return 0;
}

void Visualizer2d::setRotate(int angle)
{

}

int Visualizer2d::getRotate()
{
    return 0;
}

void Visualizer2d::blockRotate(bool on_off)
{

}

void Visualizer2d::scrollLeft()
{

}

void Visualizer2d::scrollRight()
{

}

void Visualizer2d::scrollUp()
{

}

void Visualizer2d::scrollDown()
{

}

void Visualizer2d::setVisualizerWindowId(quint64 id)
{
    VISUALIZER_WINDOW_ID = id;

    //------------------
    QTimer::singleShot(500, this, SLOT(createPlugins()));
    //------------------
}

quint64 Visualizer2d::getVisualizerWindowId()
{
    return VISUALIZER_WINDOW_ID;
}

uint32_t Visualizer2d::getVisualizerCursorId()
{
    return VISUALIZER_CURSOR_ID;
}

sw::AbstractSceneWidget *Visualizer2d::getAbstractSceneWidget()
{
    return m_scene2dWidget;
}

QWidget* Visualizer2d::widget()
{
    return m_scene2dWidget;
}

QWidget* Visualizer2d::minimapWidget()
{
    return 0;
}

QPixmap Visualizer2d::snapShort()
{
    return QPixmap();
}

void Visualizer2d::addToolButton(QToolButton* tb)
{
//    if(m_scene2dWidget)
//        m_scene2dWidget->addToolbarWidget(tb);
}

void Visualizer2d::addActionToObjectMenu(QList<QAction *> actions, QString groupName, uint groupWeight, QString subMenuName)
{
    m_tileMixer->addActionToObjectMenu(actions, groupName, groupWeight, subMenuName);
}

void Visualizer2d::addActionToRasterMenu(QList<QAction *> actions, QString groupName, uint groupWeight, QString subMenuName)
{
    m_tileMixer->addActionToRasterMenu(actions, groupName, groupWeight, subMenuName);
}


QString Visualizer2d::getDefaultXml()
{
    QString str;
    str.append("<TileMapService version=\"1.0.0\">"); str.append('\n');
    str.append("  <Grids>"); str.append('\n');
    str.append("    <Grid>"); str.append('\n');
    str.append("      <Name>gridmerc</Name>"); str.append('\n');
    str.append("      <SRSCode>EPSG:900913</SRSCode>"); str.append('\n');
    str.append("      <SRSProj> +proj=merc +a=6378137 +b=6378137 +lat_ts=0.0 +lon_0=0.0 +x_0=0.0 +y_0=0+k=1.0 +units=m +nadgrids=@null +no_defs +over</SRSProj>"); str.append('\n');
    str.append("      <BoundingBox minx=\"-20037508.3428\" miny=\"-20037508.3428\" maxx=\"20037508.3428\"maxy=\"20037508.3428\" />"); str.append('\n');
    str.append("      <Origin x=\"-180.0\" y=\"-90.0\"/>"); str.append('\n');
    str.append("      <TileFormat width=\"256\" height=\"256\"/>"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"156543.033928\" order=\"0\" />"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"78271.516964\" order=\"1\" />"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"39135.758482\" order=\"2\" />"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"19567.879241\" order=\"3\" />"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"9783.9396205\" order=\"4\" />"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"4891.96981025\" order=\"5\" />"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"2445.98490513\" order=\"6\" />"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"1222.99245256\" order=\"7\" />"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"611.496226281\" order=\"8\" />"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"305.748113141\" order=\"9\" />"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"152.87405657\" order=\"10\" />"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"76.4370282852\" order=\"11\" />"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"38.2185141426\" order=\"12\" />"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"19.1092570713\" order=\"13\" />"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"9.55462853565\" order=\"14\" />"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"4.77731426782\" order=\"15\" />"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"2.38865713391\" order=\"16\" />"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"1.19432856696\" order=\"17\" />"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"0.597164283478\" order=\"18\" />"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"0.298582141739\" order=\"19\" />"); str.append('\n');
//    str.append("      <TileSet units-per-pixel=\"78271.516964\" order=\"0\" />"); str.append('\n');
//    str.append("      <TileSet units-per-pixel=\"39135.758482\" order=\"1\" />"); str.append('\n');
//    str.append("      <TileSet units-per-pixel=\"19567.879241\" order=\"2\" />"); str.append('\n');
//    str.append("      <TileSet units-per-pixel=\"9783.9396205\" order=\"3\" />"); str.append('\n');
//    str.append("      <TileSet units-per-pixel=\"4891.96981025\" order=\"4\" />"); str.append('\n');
//    str.append("      <TileSet units-per-pixel=\"2445.98490513\" order=\"5\" />"); str.append('\n');
//    str.append("      <TileSet units-per-pixel=\"1222.99245256\" order=\"6\" />"); str.append('\n');
//    str.append("      <TileSet units-per-pixel=\"611.496226281\" order=\"7\" />"); str.append('\n');
//    str.append("      <TileSet units-per-pixel=\"305.748113141\" order=\"8\" />"); str.append('\n');
//    str.append("      <TileSet units-per-pixel=\"152.87405657\" order=\"9\" />"); str.append('\n');
//    str.append("      <TileSet units-per-pixel=\"76.4370282852\" order=\"10\" />"); str.append('\n');
//    str.append("      <TileSet units-per-pixel=\"38.2185141426\" order=\"11\" />"); str.append('\n');
//    str.append("      <TileSet units-per-pixel=\"19.1092570713\" order=\"12\" />"); str.append('\n');
//    str.append("      <TileSet units-per-pixel=\"9.55462853565\" order=\"13\" />"); str.append('\n');
//    str.append("      <TileSet units-per-pixel=\"4.77731426782\" order=\"14\" />"); str.append('\n');
//    str.append("      <TileSet units-per-pixel=\"2.38865713391\" order=\"15\" />"); str.append('\n');
//    str.append("      <TileSet units-per-pixel=\"1.19432856696\" order=\"16\" />"); str.append('\n');
//    str.append("      <TileSet units-per-pixel=\"0.597164283478\" order=\"17\" />"); str.append('\n');
//    str.append("      <TileSet units-per-pixel=\"0.298582141739\" order=\"18\" />"); str.append('\n');
    str.append("    </Grid>"); str.append('\n');
    str.append("    <Grid>"); str.append('\n');
    str.append("      <Name>gridwgs</Name>"); str.append('\n');
    str.append("      <SRSCode>EPSG:4326</SRSCode>"); str.append('\n');
    str.append("      <SRSProj>+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs +over +towgs84=0,0,0</SRSProj>"); str.append('\n');
    str.append("      <BoundingBox minx=\"-180.0\" miny=\"-90.0\" maxx=\"180.0\" maxy=\"90.0\"/>"); str.append('\n');
    str.append("      <Origin x=\"-180.0\" y=\"-90.0\"/>"); str.append('\n');
    str.append("      <TileFormat width=\"256\" height=\"256\"/>"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"0.703125\" order=\"0\"/>"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"0.3515625\" order=\"1\"/>"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"0.17578125\" order=\"2\"/>"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"0.087890625\" order=\"3\"/>"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"0.0439453125\" order=\"4\"/>"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"0.02197265625\" order=\"5\"/>"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"0.010986328125\" order=\"6\"/>"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"0.0054931640625\" order=\"7\"/>"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"0.00274658203125\" order=\"8\"/>"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"0.00137329101562\" order=\"9\"/>"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"0.000686645507812\" order=\"10\"/>"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"0.000343322753906\" order=\"11\"/>"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"0.000171661376953\" order=\"12\"/>"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"8.58306884766e-05\" order=\"13\"/>"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"4.29153442383e-05\" order=\"14\"/>"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"2.14576721191e-05\" order=\"15\"/>"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"1.07288360596e-05\" order=\"16\"/>"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"5.36441802979e-06\" order=\"17\"/>"); str.append('\n');
    str.append("      <TileSet units-per-pixel=\"2.68220901489e-06\" order=\"18\"/>"); str.append('\n');
    str.append("    </Grid>"); str.append('\n');
    str.append("  </Grids>"); str.append('\n');
    str.append("</TileMapService>"); str.append('\n');
    return str;
}

void Visualizer2d::getTmsXmlData(const QByteArray& byteArray, QList<TmsXmlData> &tmsDataList)
{
    QDomDocument doc;
    if(doc.setContent(byteArray))
    {
        QDomNode firstChild = doc.firstChild();
        while(!firstChild.isNull())
        {
            QDomNode n = firstChild.firstChild();
            while (!n.isNull())
            {
                if (n.isElement())
                {
                    QDomElement e = n.toElement();
                    const QString tagName(e.tagName());
                    if(tagName == QString("Grids"))
                    {
                        qDebug() << "getTmsXmlData, Grids :";
                        QDomNode nn = n.firstChild();
                        while (!nn.isNull())
                        {
                            if (nn.isElement())
                            {
                                QDomElement e = nn.toElement();
                                const QString tagName(e.tagName());
                                if(tagName == QString("Grid"))
                                {
                                    TmsXmlData tmsXmlData;
                                    QDomNode nnn = nn.firstChild();
                                    while (!nnn.isNull())
                                    {
                                        QDomElement e2 = nnn.toElement();
                                        const QString tagName(e2.tagName());
                                        const QString tagText(e2.text());
                                        if(tagName == QString("Name"))
                                            tmsXmlData.name = tagText;
                                        else if(tagName == QString("SRSCode"))
                                            tmsXmlData.srsEpsg = tagText;
                                        else if(tagName == QString("SRSProj"))
                                            tmsXmlData.srsProj = tagText;
                                        nnn = nnn.nextSibling();
                                    }
                                    qDebug() << "+++" << "Name :" << tmsXmlData.name << ", SRSCode :" << tmsXmlData.srsEpsg << ", SRSProj :" << tmsXmlData.srsProj;
                                    tmsDataList.append(tmsXmlData);
                                }
                            }
                            nn = nn.nextSibling();
                        }
                        qDebug() << "   -----------";
                    }
                }
                n = n.nextSibling();
            }
            firstChild = firstChild.nextSibling();
        }
    }
}

//Q_EXPORT_PLUGIN2(2Dvisplugin, AbstractVisualizer2dInterface)
//Q_PLUGIN_METADATA(IID "ru.vega.contour.CtrPluginIface.AbstractVisualizer2dInterface" FILE "myplugin.json")








