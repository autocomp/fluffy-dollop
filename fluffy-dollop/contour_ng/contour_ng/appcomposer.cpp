#include "appcomposer.h"

#include <QMenuBar>
#include <QFileInfo>
#include <QMessageBox>
#include <QCoreApplication>
#include <QComboBox>
#include <QThread>
#include <QDir>
#include <QDebug>
#include <QPluginLoader>
#include <QMenu>
#include <iostream>
#include <QShortcut>

//#include <objrepr/reprServer.h>

#include <ctrrasterrender/tms/tileloaderpool.h>
//#include <ctrrasterrender/gdal/gdaltaskpool.h>
#include <ctrcore/visual/visualizermanager.h>
#include <ctrcore/provider/dataproviderfactory.h>
#include <ctrcore/visual/datainterface.h>
#include <ctrcore/visual/viewinterface.h>
#include <ctrcore/visual/selectionmodelinterface.h>

#include <ctrcore/ctrcore/ctrconfig.h>
#include <ctrcore/ctrcore/ctrdefines.h>

#include <libembeddedwidgets/embeddedapp.h>
#include <libembeddedwidgets/embeddedmainwindow.h>


//#include <ctrcore/asbtractplugins/adaptersfactory.h>
//#include <ctrcore/asbtractplugins/uiplugininfo.h>
//#include <ctrcore/asbtractplugins/uipluginmanager.h>

//#include <ctrobjectwidgets/managervectorobject/managervectorobjectplugin.h>
//#include <ctrobjectwidgets/scenarioselector/scenarioselector.h>
//#include <ctrobjectwidgets/scenarioselector/scenariomainwidget.h>

#include <ctrcore/plugin/embifacenotifier.h>
#include <ctrcore/plugin/ctrpluginloader.h>
#include <ctrcore/plugin/ctrproviderplugin.h>
#include <QTranslator>
#include <QApplication>
//#include <QCleanlooksStyle>
#include <libembeddedwidgets/sqliteserializer.h>
#include <libembeddedwidgets/docviewerwidget.h>
//#include "libstyledebuggerplugin/debuggerinstaller.h"

#include <ctrcore/undoactions/undocontroller.h>
//#include <ctrwidgets/components/endworkingwidgets/endworkingobject.h>

#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>

AppComposer::AppComposer(QObject *parent) : QObject(parent)
{
    m_openScenarioWId = 0;
    m_createScenarioWId = 0;
    m_editScenarioWId = 0;
    m_applicationName = "";

    QStringList qmListForLoad = QStringList() << ":/contour_ng.qm"
                                              << ":/qt_ru.qm"
                                              << ":/ctrvisual.qm"
                                              << ":/ctrcore.qm"
                                              << ":/ctrobjectwidgets.qm"
                                              << ":/ctrwidgets.qm";
    foreach (QString resourcePath, qmListForLoad)
    {
        QTranslator *trans = new QTranslator();
        if(trans->load(resourcePath))
            QCoreApplication::installTranslator(trans);
    }

    m_pGlobNotif = new GlobalMessageNotifier();
    connect(m_pGlobNotif, SIGNAL(signalMessageReceived(globalbusmessages::GLOBAL_MESSAGE_TYPE,QVariant))
            , this, SLOT(slotGlobalMessageReceived(globalbusmessages::GLOBAL_MESSAGE_TYPE,QVariant)));

//    QApplication::setStyle(new QCleanlooksStyle());

}

AppComposer::~AppComposer()
{
    qDebug() << "===> ~AppComposer()";
    m_pAppPlugins.clear();
    saveLastOpenedScenarioId(0);
}



void AppComposer::init()
{
    m_applicationName = tr("This application name is contour-ng");

    /* создаем главное окно приложения*/

    QMenuBar* mn= createMenuBar();
    QWidget *bottombar = createBottomBar();



    mainWindowId = ewApp()->restoreMainWidget("MainWindow",mn,bottombar);
    if(0 == mainWindowId)
    {
        ew::EmbeddedMainWindowStruct mainStr;
        mainStr.menuBar = mn;
        mainStr.minSize = QSize(300,200);
        mainStr.size = QSize(600,500);
        mainStr.widgetTag = "MainWindow";
        mainStr.bottomWidget = bottombar;
        ew::EmbeddedHeaderStruct mainHeadStr;
        mainHeadStr.hasCloseButton = true;
        mainHeadStr.hasStyleButton = true;
        mainHeadStr.hasMinMaxButton = true;
        mainHeadStr.hasCollapseButton = true;
        mainHeadStr.hasHideHeaderButton = false;
        mainHeadStr.hasSettingsButton  = false;
        mainHeadStr.tooltip = m_applicationName;
        mainHeadStr.windowTitle = m_applicationName;
        mainHeadStr.hasWhatButton = true;
        mainHeadStr.headerPixmap = QString("://pic/icon_logo.png");
        mainStr.header = mainHeadStr;
        mainWindowId = ewApp()->createMainWindow(mainStr);        // Создали окно
    }
    // Задаем название комплекса


    connect(ewApp(), SIGNAL(signalSavedWidgetLoaded(QString,QVariantMap,ew::EmbeddedWidgetType,QWidget*)),
            this, SLOT(slotSavedWidgetLoaded(QString,QVariantMap,ew::EmbeddedWidgetType,QWidget*)));


    // загрузка доступных ресурсов
    //------------------------------------
//    ctrutils::CtrResourceManager::instance()->loadResourcesAsync();

    CtrPluginLoader* loader = CtrPluginLoader::instance();
    QList<CtrPlugin*> listPl = loader->getPluginList(TYPE_PROVIDER);

    foreach(CtrPlugin* plugin, listPl)
    {
        if(dynamic_cast<CtrProviderPlugin*>(plugin))
            dynamic_cast<CtrProviderPlugin*>(plugin)->instanceToFactory();
        delete plugin;
    }

    // иннициализация пула, перенести в САМ ПУЛ !!!
    //------------------------------------

    QString settingsPath("./tilesystemsettings.ini");
    TileLoaderPool::instance()->setSettingsPath(settingsPath);

    // создание окна 2Д-визуализатора
    //------------------------------------

    uint visualizerId = visualize_system::VisualizerManager::instance()->addVisualizer(visualize_system::Visualizer2D);
    if(visualizerId != 0)
    {
        QVariant baseTmsLayer_Url = CtrConfig::getValueByName(QString("application_settings.baseTmsLayer_Url"));
        if(baseTmsLayer_Url.isValid())
        {
            QUrl url(baseTmsLayer_Url.toString());
            QSharedPointer<AbstractDataProvider>dp = data_system::DataProviderFactory::instance()->createProvider(url, QString("tms"));
            if(dp)
            {
                dp->open(url);
                visualize_system::DataInterface * dataInterface = visualize_system::VisualizerManager::instance()->getDataInterface(visualizerId);
                if(dataInterface)
                {
                    QList<uint> providers;
                    providers.append(dp->getProviderId());
                    dataInterface->addBaseProviders(providers);
                }
            }
        }
        else
        {
            CtrConfig::setValueByName(QString("application_settings.baseTmsLayer_Url"), QVariant());
        }

//!       тест работы провайдера изображений
//        {
//            QUrl url = QUrl::fromLocalFile("/home/sergvas/Desktop/pdf/new/IMAGE0016.JPG");
//            QSharedPointer<AbstractDataProvider>dp = data_system::DataProviderFactory::instance()->createProvider(url);
//            if(dp)
//            {
//                dp->open(url);
//                visualize_system::DataInterface * dataInterface = visualize_system::VisualizerManager::instance()->getDataInterface(visualizerId);
//                if(dataInterface)
//                {
//                    QList<uint> providers;
//                    providers.append(dp->getProviderId());
//                    dataInterface->addProviders(providers);
//                }
//            }
//        }

        visualize_system::ViewInterface * viewInterface = visualize_system::VisualizerManager::instance()->getViewInterface(visualizerId);
        if(viewInterface)
        {
            EmbIFaceNotifier *iface = new EmbIFaceNotifier(viewInterface->widget());
            QString visTag = visualize_system::VisualizerManager::instance()->convertVisualizerType(viewInterface->getVisualizerType());
            quint64 windowId = ewApp()->restoreWindow(visTag, iface);
            if(0 == windowId)
            {
                ew::EmbeddedWindowStruct str;
                str.iface = iface;
                str.widgetTag = visTag;
                str.allowMoving = true;
                ew::EmbeddedHeaderStruct headStr;
                headStr.hasCloseButton = true;
                headStr.hasMinMaxButton = true;
                str.header = headStr;
                windowId = ewApp()->createWindow(str,mainWindowId);
            }
            viewInterface->setVisualizerWindowId(windowId);
        }
    }


    //------------------------------------
//    m_pScenarioSelector = new ScenarioSelector("",this);
//    m_pObjTree = new ManagerVectorObjectPlugin(this);
    /*создаем плагин менеджера объектов*/

//    ew::EmbeddedSubIFace * browserIface = m_pObjTree->getObjectBrowser();
//    if(0 == ewApp()->restoreWidget("managerobjectbrowser",browserIface))
//    {
//        ew::EmbeddedWidgetStruct browserStr;
//        browserStr.iface = browserIface;
//        browserStr.addHided = true;
//        browserStr.allowMoving = true;
//        browserStr.topOnHint = true;
//        browserStr.minSize = QSize(300,300);
//        browserStr.size = QSize(400,550);
//        browserStr.widgetTag = "managerobjectbrowser";
//        ew::EmbeddedHeaderStruct browserHeader;
//        browserHeader.headerPixmap = QString(":/pic/korplan_tree.png");
//        browserHeader.hasCloseButton = true;
//        browserHeader.tooltip = tr("Object Tree");
//        browserHeader.windowTitle = tr("Object Tree");
//        browserHeader.hasMinMaxButton = true;
//        browserStr.header = browserHeader;
//        ewApp()->createWidget(browserStr, 0);
//    }

    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::DisableComponent, this, SLOT(slotDisableMenus(QVariant)),
                                      qMetaTypeId< QString >(),
                                      QString("visualize_system") );
    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::EnableComponent, this, SLOT(slotEnabledMenus(QVariant)),
                                      qMetaTypeId< QString >(),
                                      QString("visualize_system") );

    connect(ewApp(),SIGNAL(signalAppCloseRequest()),this,SLOT(slotOnAppClose()));

    // открытие последнего открытого сценария после падения программы (если она не была корректно закрыта)
//    uint32_t lastScenarioId = getLastOpenedScenarioId();
//    if(lastScenarioId!=0)
//    {
//        QMessageBox::StandardButton res = ewApp()->question( 0, tr("Endorsment"),
//                                                             tr("In last session complex has crashed.").append("\n").
//                                                             append(tr("Are you really want to restore last session?")),
//                                                             QMessageBox::Yes|QMessageBox::No,
//                                                             QMessageBox::No );
//        if( res == QMessageBox::Yes )
//            ((ScenarioMainWidget *)m_pScenarioSelector->getOpenScenarioWidget())->slotOpenContext(lastScenarioId);
//    }
}

void AppComposer::slotMainMenuClicked()
{
//    if( (m_pShowAct == sender()) || (m_pCloseAct == sender()) || (m_pCreateAct == sender()))
//    {
//        slotInitiateEwoFinisher((QAction*)sender());
//    }

//    if(m_pEditAct == sender())
//    {
//        if(0 == m_editScenarioWId)
//        {
//            auto iFace = m_pScenarioSelector->getEditScenarioWidget();
//            QString tag = "EditScenario";
//            m_editScenarioWId = ewApp()->restoreWidget(tag,iFace);
//            if(0 == m_editScenarioWId)
//            {
//                ew::EmbeddedWidgetStruct ewStr;
//                ewStr.iface = iFace;
//                ewStr.isModal = true;
//                ewStr.widgetTag = tag;
//                ewStr.header.windowTitle = tr("Scenario editing");
//                ewStr.addHided = true;
//                ewStr.topOnHint = true;
//                ewStr.header.headerPixmap = QString(":/pic/068_icons_32_edit.png");
//                ewStr.header.hasHideHeaderButton = false;
//                ewStr.header.hasSettingsButton = false;
//                ewStr.header.hasCloseButton = true;
//                ewStr.minSize = QSize(400,500);
//                ewStr.size = QSize(600,500);

//                m_pScenarioSelector->slotEditScenario();
//                m_editScenarioWId = ewApp()->createWidget(ewStr, 0);
//            }

//            ewApp()->setVisible(m_editScenarioWId,true);
//        }
//        else
//        {
//            ewApp()->setVisible(m_editScenarioWId,true);
//        }
//    }

//    if(m_pDeleteAct == sender())
//    {
//        m_pScenarioSelector->slotDeleteScenario();
//    }

//    if(m_pExitAct == sender())
//    {
//        ewApp()->close();
//    }

//    /// view
//    if(sender() == m_pObjectBrowserAct)
//    {
//        ewApp()->setVisible(m_pObjTree->getObjectBrowser()->id(), true);
//        //ewApp()->setVisible(m_pObjTree->getObjectBrowser()->id());
//    }

//    if(sender() == m_pManuals)
//    {
//        ewApp()->setVisibleDocViewer(true);
//    }

}


void AppComposer::slotWidgetMetaInfoChanged()
{

}

void AppComposer::slotSavedWidgetLoaded(QString widgetTag, QVariantMap widgetMetaInfo, ew::EmbeddedWidgetType widgetType, QWidget* widgetPtr)
{

}

QMenuBar* AppComposer::createMenuBar()
{
    QMenuBar * m_menuBar   = new QMenuBar()  ;        // Создали меню
//    QMenu * m_fileMenu = new QMenu(tr("Scenario"),m_menuBar);
//    m_pCreateAct = m_fileMenu->addAction(QIcon(":/pic/066_icons_32_create.png"), tr("Create Scenario..."),this, SLOT(slotMainMenuClicked()), QKeySequence("Ctrl+N"));
//    m_pShowAct = m_fileMenu->addAction(QIcon(":/pic/067_icons_32_open.png"), tr("Open Scenario..."),this, SLOT(slotMainMenuClicked()), QKeySequence("Ctrl+O"));
//    m_pEditAct = m_fileMenu->addAction(QIcon(":/pic/068_icons_32_edit.png"), tr("Edit Scenario..."),this, SLOT(slotMainMenuClicked()), QKeySequence("Ctrl+E"));
//    m_pCloseAct = m_fileMenu->addAction(QIcon(":/pic/069_icons_32_close.png"), tr("Close Scenario..."),this, SLOT(slotMainMenuClicked()), QKeySequence("Ctrl+W"));
//    m_pDeleteAct = m_fileMenu->addAction(QIcon(":/pic/070_icons_32_del.png"), tr("Delete Scenario..."),this, SLOT(slotMainMenuClicked()), QKeySequence("Ctrl+Del"));
//    m_pExitAct = m_fileMenu->addAction(QIcon(":/pic/071_icons_32_exit.png"), tr("Exit"),this, SLOT(slotMainMenuClicked()), QKeySequence("Alt+F4"));
//    m_menuBar->addMenu(m_fileMenu);

//    QMenu * m_viewMenu = new QMenu(tr("Instruments"),m_menuBar);
//    m_pOpenFile = 0;//m_viewMenu->addAction(QIcon(":/27_open.png"), tr("Open file"),this, SLOT(slotMainMenuClicked()));
//    m_pOpenStream = 0;//m_viewMenu->addAction(QIcon(":/27_open.png"), tr("Open stream"),this, SLOT(slotMainMenuClicked()));
//    m_pObjectBrowserAct = m_viewMenu->addAction(QIcon(":/pic/korplan_tree.png"), tr("Object Tree"),this, SLOT(slotMainMenuClicked()));
//    m_p3DAct = 0;//m_viewMenu->addAction(QIcon(":/2_3d-editor.png"), tr("Open 3D"),this, SLOT(slotMainMenuClicked()));
//    m_menuBar->addMenu(m_viewMenu);

//    QMenu * quest = m_menuBar->addMenu(tr("Settings"));
//    m_pAppSettings = quest->addAction(QIcon(":/295_settings_manager.png"), tr("Application settings"),this, SLOT(slotMainMenuClicked()));
//    //m_pStyleDebug = quest->addAction(QIcon(":/27_open.png"), tr("Debug stylesheet"),this, SLOT(slotMainMenuClicked()));
//    m_menuBar->addMenu(quest);

//    QMenu * manuals = m_menuBar->addMenu(tr("Help"));
//    m_pManuals = manuals->addAction(QIcon(":/188_users_manual.png"), tr("Help..."),this, SLOT(slotMainMenuClicked()));
//    m_pManuals->setShortcut(QKeySequence(QKeySequence::HelpContents));
//    //m_pInformation = manuals->addAction(/*QIcon(":/188_users_manual.png"),*/ tr("Information..."),this, SLOT(slotInformation()));
//    m_pAbout = manuals->addAction(QIcon(":/189_users_manual.png"), tr("About"),this, SLOT(slotMainMenuClicked()));
//    m_menuBar->addMenu(manuals);

//    //DebuggerInstaller *inst = new DebuggerInstaller(m_pStyleDebug, this);

//    // выставляем флаги кнопкам
//    m_pEditAct->setEnabled(false);
//    m_pCloseAct->setEnabled(false);
//    m_pDeleteAct->setEnabled(false);

    /// Грузим плагины приложения
//    CtrPluginLoader* loader = CtrPluginLoader::instance();
//    CtrPlugin* plugin = loader->getPlugin(TYPE_APP, "libstyleeditplugin.so");
//    if(dynamic_cast<CtrAppVisualizerPlugin*>(plugin))
//    {
//        (dynamic_cast<CtrAppVisualizerPlugin*>(plugin))->initPluginInterface();
//        addPluginToMenu(dynamic_cast<CtrAppVisualizerPlugin*>(plugin), quest);
//    }

    QShortcut * undoShortcut = new QShortcut(m_menuBar);
    undoShortcut->setKey(QKeySequence(QKeySequence::Undo));
    undoShortcut->setContext(Qt::ApplicationShortcut);
    connect(undoShortcut, SIGNAL(activated()), undo_actions::UndoController::instance(), SLOT(undoAction()));

    return m_menuBar;
}


void AppComposer::slotOnCloseCheckFinished(bool isAllOk)
{
//    qDebug()<<"AppComposer::slotOnCloseCheckFinished == "<< isAllOk;

//    if(!isAllOk)
//        return;

//    if(m_ewoFinisher->getState() == EndWorkingObject::EWO_CLOSE_SCENARIO)
//    {
//        objrepr::ContextPtr cont = objrepr::RepresentationServer::instance()->currentContext();
//        if(cont)
//        {
//            m_pScenarioSelector->slotCloseScenario();
//        }

//        if( m_pShowAct == initiatorActionForEwoFinisher )
//        {
//            if(0 == m_openScenarioWId)
//            {
//                auto iFace = m_pScenarioSelector->getOpenScenarioWidget();
//                QString tag = "OpenScenario";
//                m_openScenarioWId = ewApp()->restoreWidget(tag,iFace);
//                if(0 == m_openScenarioWId)
//                {
//                    ew::EmbeddedWidgetStruct ewStr;
//                    ewStr.isModal = true;
//                    ewStr.header.hasHideHeaderButton = false;
//                    ewStr.header.hasSettingsButton = false;
//                    ewStr.header.hasCloseButton = true;
//                    ewStr.header.headerPixmap = QString(":/pic/067_icons_32_open.png");
//                    ewStr.iface = iFace;
//                    ewStr.topOnHint = true;
//                    ewStr.minSize = QSize(400,500);
//                    ewStr.size = QSize(600,500);
//                    ewStr.widgetTag = tag;
//                    ewStr.header.windowTitle = tr("Scenario opening");
//                    ewStr.addHided = true;
//                    m_pScenarioSelector->slotShowScenWidget();
//                    m_openScenarioWId = ewApp()->createWidget(ewStr, 0);
//                }
//                ewApp()->setVisible(m_openScenarioWId,true);
//            }
//            else
//            {
//                ewApp()->setVisible(m_openScenarioWId,true);
//            }
//        }
//        else if( m_pCreateAct == initiatorActionForEwoFinisher )
//        {
//            if(0 == m_createScenarioWId)
//            {
//                auto iFace = m_pScenarioSelector->getCreateScenarioWidget();
//                QString tag = "CreateScenario";
//                m_createScenarioWId = ewApp()->restoreWidget(tag,iFace);
//                if(0 == m_createScenarioWId)
//                {
//                    ew::EmbeddedWidgetStruct ewStr;
//                    ewStr.iface = iFace;
//                    ewStr.isModal = false;
//                    ewStr.addHided = true;
//                    ewStr.topOnHint = true;

//                    ewStr.header.windowTitle = tr("Scenario creating");
//                    ewStr.widgetTag = tag;
//                    ewStr.minSize = QSize(400,500);
//                    ewStr.size = QSize(600,500);
//                    ewStr.header.headerPixmap = QString(":/pic/066_icons_32_create.png");
//                    ewStr.header.hasCloseButton = true;
//                    ewStr.header.hasHideHeaderButton = false;
//                    ewStr.header.hasSettingsButton = false;
//                    m_pScenarioSelector->slotCreateScenario();
//                    m_createScenarioWId = ewApp()->createWidget(ewStr, 0);
//                }
//                ewApp()->setVisible(m_createScenarioWId,true);
//            }
//            else
//            {
//                ewApp()->setVisible(m_createScenarioWId,true);
//            }
//        }
//    }
//    else if(m_ewoFinisher->getState() == EndWorkingObject::EWO_CLOSE_APP)
//    {
//        if(initiatorActionForEwoFinisher == m_pExitAct)
//        {
//            disconnect(ewApp(),SIGNAL(signalAppCloseRequest()),this,SLOT(slotOnAppClose()));
//            if(ewApp()->isCloseDenied())
//                ewApp()->setCloseAppDenied(false);
//            ewApp()->close();
//        }
//    }
}

void AppComposer::slotGlobalMessageReceived(globalbusmessages::GLOBAL_MESSAGE_TYPE mt,QVariant var)
{
//    switch (mt) {
//    case globalbusmessages::GMT_SCENARIO_OPENNED :
//    {
//        m_pShowAct->setEnabled(true);
//        m_pCreateAct->setEnabled(true);
//        m_pCloseAct->setEnabled(true);
//        m_pDeleteAct->setEnabled(true);
//        m_pEditAct->setEnabled(true);
//        objrepr::ContextPtr cont = objrepr::RepresentationServer::instance()->currentContext();
//        if(cont)
//        {
//            ewApp()->setWidgetTitle(mainWindowId, QString("%1 - %2")
//                                    .arg(QString::fromUtf8(cont->name()))
//                                    .arg(m_applicationName));

//        }
////        objrepr::SpatialObjectManager * man = objrepr::RepresentationServer::instance()->objectManager();
////        uint32_t soid = man->selectedObject();
////        qDebug()<<soid;

//        saveLastOpenedScenarioId(cont->id());

//    }break;
//    case globalbusmessages::GMT_SCENARIO_CLOSED :
//    {
//        m_pShowAct->setEnabled(true);
//        m_pCreateAct->setEnabled(true);
//        m_pCloseAct->setEnabled(false);
//        m_pDeleteAct->setEnabled(false);
//        m_pEditAct->setEnabled(false);
//        ewApp()->setWidgetTitle(mainWindowId, m_applicationName);

//        saveLastOpenedScenarioId(0);

//    }break;
//    default:
//        break;
//    }
}


QWidget* AppComposer::createBottomBar()
{
    return 0;
}


bool AppComposer::addPluginToMenu(CtrAppVisualizerPlugin *plugin, QMenu *menu)
{
    if(plugin)
    {
        QList<InitPluginData> InitPluginDataList = plugin->getInitPluginData();
        foreach (InitPluginData initData, InitPluginDataList)
        {
            QAction *pAction = new QAction(initData.iconForButtonOn
                                           , initData.translateButtonName
                                           , menu);
            pAction->setToolTip(initData.tooltip);
            menu->addAction(initData.iconForButtonOn, initData.translateButtonName
                            , plugin, SLOT(showPluginInterface(bool)));

            m_pAppPlugins.append(plugin);
        }
    }

}

void AppComposer::slotStyleClicked(bool)
{
    qDebug() << "AppComposer::showPluginInterface";

}

void AppComposer::slotDisableMenus(QVariant)
{
//    m_pObjectBrowserAct->setDisabled(true);

//    if(ewApp()->isVisible(m_pObjTree->getObjectBrowser()->id()))
//    {
//        m_pObjTree->setProperty("setVisibleAfterEnable", true);
//        ewApp()->setVisible(m_pObjTree->getObjectBrowser()->id(), false);
//    }
//    else
//        m_pObjTree->setProperty("setVisibleAfterEnable", false);
}

void AppComposer::slotEnabledMenus(QVariant)
{
//    m_pObjectBrowserAct->setDisabled(false);

//    if(m_pObjTree->property("setVisibleAfterEnable").toBool())
//    {
//        ewApp()->setVisible(m_pObjTree->getObjectBrowser()->id(), true);
//    }
}

void AppComposer::slotInitiateEwoFinisher(QAction *initiatorAction)
{
//    initiatorActionForEwoFinisher = initiatorAction;

//    EndWorkingObject::EwoState ewoState = EndWorkingObject::EWO_CLOSE_SCENARIO;
//    if(initiatorAction == m_pExitAct)
//        ewoState = EndWorkingObject::EWO_CLOSE_APP;

//    if(nullptr == m_ewoFinisher)
//    {
//        m_ewoFinisher = new EndWorkingObject(ewoState, this);
//        connect(m_ewoFinisher,SIGNAL(sigAllChecked(bool)),SLOT(slotOnCloseCheckFinished(bool)));
//    }
//    if(m_ewoFinisher->getState() != ewoState)
//        if(!m_ewoFinisher->setState(ewoState))
//            return;
//    objrepr::ContextPtr cont = objrepr::RepresentationServer::instance()->currentContext();
//    if(cont == nullptr)
//    {
//        if( initiatorAction == m_pExitAct )
//            m_ewoFinisher->setFinalAskOnSuccess(true);
//        else
//            m_ewoFinisher->setFinalAskOnSuccess(false);
//    }
//    else if(!m_ewoFinisher->isFinalAskOnSuccess())
//    {
//        m_ewoFinisher->setFinalAskOnSuccess(true);
//    }


//    m_ewoFinisher->checkAll();
}

void AppComposer::slotOnAppClose()
{
//    if( ewApp()->isCloseDenied() != true )
//        ewApp()->setCloseAppDenied(true);
//    slotInitiateEwoFinisher( m_pExitAct );
}

void AppComposer::saveLastOpenedScenarioId(uint32_t scenarioId)
{
//    CtrConfig::setValueByName( QString("application_settings.LastOpenedScenarioId"), scenarioId );
}

uint32_t AppComposer::getLastOpenedScenarioId()
{
    return 0;
//    return CtrConfig::getValueByName( QString("application_settings.LastOpenedScenarioId"), 0).toUInt();
}
