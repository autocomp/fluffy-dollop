#include "scenewidget.h"

#include <QStyle>
#include <QVBoxLayout>
#include <QDebug>
#include <QSettings>
#include <QDateTime>
#include "videosettings.h"
const int buttonCountBbw1 = 9;
const int buttonCountBbw2 = 10;
const int buttonCountBbw3 = 1;
const int buttonCountBbw4 = 1;


using namespace sw;
void sw::SceneWidget::resizeEvent(QResizeEvent *e)
{

    int w = width();
    if(m_denyGeometryChange)
    {
        e->ignore();
    }
    else
    {
        e->accept();
    }

    slotUpdateChildsGeometry();
    emit testChangeWindow();
    emit newSize(e->size().width(), e->size().height());
}


void sw::SceneWidget::setVisible(bool visible)
{
    if( (false == visible) && (m_denyMinimize || m_denyClose) )
    {
        return;
    }
    else
    {
        QWidget::setVisible(visible);
    }
    emit testVisWindow();
}




void sw::SceneWidget::hideEvent(QHideEvent *e)
{
    if(m_denyMinimize)
    {
        e->accept();
        show();
        setVisible(true);
    }
    emit testVisWindow();
}

void sw::SceneWidget::moveEvent(QMoveEvent *e)
{
    if(m_denyGeometryChange)
    {
        setGeometry(m_geom);
        e->accept();
    }
    emit testChangeWindow();
}

void sw::SceneWidget::slotUpdateChildsGeometry()
{
    quint64 t1 = QDateTime::currentMSecsSinceEpoch();
    int w = width();
    int kompasX =0 ,kompasY=0 , kompasW=0 ,kompasH=0;
    int topMenuBarX =0,topMenuBarY=0, topMenuBarW=0,topMenuBarH =0;
    int openMenuButtonX =0,openMenuButtonY=0 ,  openMenuButtonW =0 ,openMenuButtonH =0 ;
    int coordBarX =0, coordBarY=0, coordBarW=0 ,coordBarH=0 ;
    int minimapContX=0, minimapContY=0, minimapContW=0, minimapContH=0;
    int rowScaleX=0,rowScaleY=0,rowScaleW=0,rowScaleH=0;
    int zoomScrollX=0,zoomScrollY=0,zoomScrollW=0,zoomScrollH=0;
    int navigationX=0, navigationY=0, navigationW=0, navigationH=0;
    int timeBarX = 0, timeBarY = 0, timeBarW = 0, timeBarH = 0;
    int notifyWdgtX = 0, notifyWdgtY = 0, notifyWdgtW = 0, notifyWdgtH = 0;

    bool kompasVis = false, topMenubarVis = false, openMenuButtonVis = false,coordBarVis = false,
            minimapContVis = false, rowScaleVis = false,zoomScrollVis = false, navigationVis = false, timeBarVis = false;

    int mainWidgW = (width() - m_pOuterLeftScrollArea->width()*(int)(m_pOuterLeftScrollArea->isVisible())
                     - m_pOuterRightScrollArea->width()*(int)(m_pOuterRightScrollArea->isVisible()) );
    int mainWidgX = m_pOuterLeftScrollArea->width()*(int)(m_pOuterLeftScrollArea->isVisible());

    //    if((m_kompasVisibleCB)&&(0 != m_kompas))
    //    {
    //        kompasVis3D = isCompas3DVisible();
    //        kompasX = mainWidgX + mainWidgW - m_kompas->width();
    //        kompasY = 50;
    //        kompasW = m_kompas->width();
    //        kompasH = m_kompas->height();
    //        m_kompas->setGeometry(kompasX, kompasY,kompasW,kompasH);
    //    }
//    if(0 != m_kompas)
//    {
//        //        kompasVis = m_kompas->isVisible();
//        kompasVis = m_kompasVisibleCB->getSelectState();
//        kompasX = mainWidgX + mainWidgW - m_kompas->width();
//        kompasY = 50;
//        kompasW = m_kompas->width();
//        kompasH = m_kompas->height();
//        m_kompas->setGeometry(kompasX, kompasY,kompasW,kompasH);
//    }


    if(0 != m_pTimeBar)
    {
        timeBarVis = m_pTimeBar->isVisible();
        timeBarX = mainWidgX + 4 ;
        timeBarY = 4;
        timeBarW = m_pTimeBar->width();
        timeBarH = m_pTimeBar->height();

        m_pTimeBar->setGeometry(timeBarX, timeBarY, timeBarW, timeBarH);
    }

/*
    if(0 != m_topMenuBar)
    {

        topMenubarVis = m_topMenuBar->isVisible();

        if(0 != m_pTimeBar)
        {
            if( mainWidgX + mainWidgW/2 - m_topMenuBar->width()/2 < mainWidgX + m_pTimeBar->x()*(int)timeBarVis + m_pTimeBar->width()*(int)timeBarVis + 20 )
            {
                topMenuBarX = timeBarX + m_pTimeBar->width()*(int)topMenubarVis + 20 ;
            }
            else
            {
                topMenuBarX = mainWidgX + mainWidgW/2
                        - m_topMenuBar->width()/2 ;
            }
        }
        else
        {
            topMenuBarX = mainWidgX + mainWidgW/2 - m_topMenuBar->width()/2 ;
            topMenuBarX = mainWidgW - m_topMenuBar->width() - 4;
        }

        topMenuBarY = 4;
        topMenuBarW = m_topMenuBar->width();
        topMenuBarH = m_topMenuBar->height();



        m_topMenuBar->setGeometry(topMenuBarX, topMenuBarY, topMenuBarW, topMenuBarH);
    }
*/

//    if(0 != m_openMenuButton)
//    {
//        openMenuButtonVis = m_openMenuButton->isVisible();
//        openMenuButtonW = m_openMenuButton->width();
//        openMenuButtonH = m_openMenuButton->height();

//        openMenuButtonX = mainWidgX + mainWidgW/2 + topMenuBarW/2 - m_openMenuButton->width();
//        if(0 != m_topMenuBar)
//        {
//            openMenuButtonX = topMenuBarX + m_topMenuBar->width() - openMenuButtonW;
//        }

//        openMenuButtonY = 0;

//        m_openMenuButton->setGeometry(openMenuButtonX,openMenuButtonY, openMenuButtonW,openMenuButtonH);
//    }


    if(0 != m_coordBar)
    {
        coordBarVis = m_coordBar->isVisible();
        coordBarX = mainWidgX + mainWidgW - m_coordBar->width() - 100 - 4;
        coordBarY = m_mainConteinerWidget->height() - m_coordBar->height() - 4;
        coordBarW = m_coordBar->width();
       // coordBarW = /*mainWidgW - m_coordBar->width() -*/ 128;
        coordBarH = m_coordBar->height();

//        m_coordBar->setGeometry(coordBarX,coordBarY ,coordBarW ,coordBarH );
        m_coordBar->move(coordBarX,coordBarY);
    }

    if(0 != m_minimapContainer)
    {
        minimapContVis = m_minimapContainer->isVisible();
        minimapContX = mainWidgX + 4 ;
        minimapContY = m_mainConteinerWidget->height() - m_minimapContainer->height() - 4;
        minimapContW = m_minimapContainer->width();
        minimapContH = m_minimapContainer->height();

        m_minimapContainer->setGeometry(minimapContX, minimapContY, minimapContW, minimapContH);
    }


    if(m_rowScale && m_coordBar)
    {
        rowScaleW = m_rowScale->width();
        rowScaleH = m_rowScale->height();
        rowScaleVis = m_rowScale->isVisible();
        int emptyW = (coordBarX - mainWidgX + m_minimapContainer->x() + m_minimapContainer->width() + 4);
        int rsPrefferX1 = mainWidgX + mainWidgW/2 - rowScaleW/2 ;
        int rsPrefferX2 = mainWidgX + mainWidgW/2 + rowScaleW ;
        int startX = mainWidgX + (m_minimapContainer->x() + m_minimapContainer->width())*(int)(m_minimapContainer->isVisible()) + 4;
        int endX = coordBarX;
        if(startX < rsPrefferX1 && endX > rsPrefferX2)
        {
            rowScaleX =  rsPrefferX1;
        }
        else
        {
            rowScaleX = mainWidgX
                    +  (minimapContX + minimapContW)*(int)(m_minimapContainer->isVisible())
                    + (( mainWidgW -
                         coordBarW*(int)(m_coordBar->isVisible()) - (minimapContW)*(int)(m_minimapContainer->isVisible() ))/2
                       -  rowScaleW/2) - 4;
        }
        rowScaleY = m_mainConteinerWidget->height() - m_rowScale->height() - 4;

        if(0 != m_pRowScaleControl)
        {
            int rowScaleControlH = m_pRowScaleControl->height();
            int rowScaleControlW = m_pRowScaleControl->width();
            int rowScaleControlX = rowScaleX +rowScaleW/2 - rowScaleControlW/2;
            int rowScaleControlY = rowScaleY - rowScaleControlH - 4;
            m_pRowScaleControl->setGeometry(rowScaleControlX, rowScaleControlY, rowScaleControlW, rowScaleControlH);
        }

        m_rowScale->setGeometry(rowScaleX,rowScaleY, rowScaleW, rowScaleH);
    }




//    if(0 != m_zoomScroll)
//    {
//        //        zoomScrollVis = m_kompasVisibleCB->getSelectState();
//        zoomScrollVis = m_zoomBarVisibleCB->getSelectState();
//        zoomScrollX = mainWidgX + 4 ;
//        zoomScrollY = 50;
//        zoomScrollW = m_zoomScroll->width();
//        zoomScrollH = m_zoomScroll->pos().y() - m_minimapContainer->pos().y();

//        m_zoomScroll->setGeometry(zoomScrollX,zoomScrollY, zoomScrollW, zoomScrollH);
//        m_zoomScroll->setFixedHeight(minimapContY - zoomScrollY );
//    }

    int osmParamsWidgetX, osmParamsWidgetY, osmParamsWidgetW, osmParamsWidgetH;
    if(0 != m_pOsmParamsContainerWidget)
    {
        zoomScrollVis = m_pOsmParamsContainerWidget->isVisible();

        osmParamsWidgetX = m_mainConteinerWidget->x() + m_mainConteinerWidget->width()/2 - m_pOsmParamsContainerWidget->width() - 20;
        osmParamsWidgetY = m_mainConteinerWidget->y() + m_mainConteinerWidget->height()/2 + 20;

        osmParamsWidgetW = m_pOsmParamsContainerWidget->width();
        osmParamsWidgetH = m_pOsmParamsContainerWidget->height();

        osmParamsWidgetX = mainWidgX + 4 + mainWidgW/2 - 20 - osmParamsWidgetW;
        osmParamsWidgetY = height()/2 + 20;

        m_pOsmParamsContainerWidget->setGeometry(osmParamsWidgetX,osmParamsWidgetY, osmParamsWidgetW, osmParamsWidgetH);
    }


    if(0 != m_navigation)
    {
        navigationVis = m_navigation->isVisible();
        navigationX = mainWidgX + mainWidgW - kompasW/2 - m_navigation->width()/2;
        navigationY = 60 + kompasH/**(int)(kompasVis)*/;
        navigationW = m_navigation->width();
        navigationH = m_navigation->height();

        m_navigation->setGeometry(navigationX,navigationY, navigationW, navigationH);
    }


    int yScroll = topMenuBarH*(int)(topMenubarVis)
            + openMenuButtonH*(int)(openMenuButtonVis)
            + kompasH*(int)(kompasVis)
            + navigationH*(int)(navigationVis)
            + 30;



    int scrollMaxHeight = m_mainConteinerWidget->height() - (yScroll
                                                             + coordBarH*(int)(coordBarVis) + 10 );

    int scrollPrefferHeight = m_pInnerRightScrollArea->prefferHeight();

    int xScroll = w - m_pInnerRightScrollArea->width()
            - m_pOuterRightScrollArea->width()*(int)(m_pOuterRightScrollArea->isVisible())- 4;

    int setHeight = 0;

    if(scrollPrefferHeight > scrollMaxHeight || m_pInnerRightScrollArea->isNeedShowFull())
        setHeight = scrollMaxHeight;
    else
        setHeight = scrollPrefferHeight;

    int innerWidgetW = m_pInnerRightScrollArea->width();
    m_pInnerRightScrollArea->setGeometry(xScroll,
                                         yScroll + (scrollMaxHeight - setHeight),
                                         innerWidgetW,
                                         setHeight );


    if(0 != m_pWaitCollapsedWidget)
    {
        bool waitWidgetVis = m_pWaitCollapsedWidget->isVisible();

        QPoint globalPoint = mapToGlobal(m_mainConteinerWidget->geometry().center());
        int waitWidgetX = globalPoint.x() - m_pWaitCollapsedWidget->width()/2 ;
        int waitWidgetY = globalPoint.y() - m_pWaitCollapsedWidget->height()/2 ;

        int waitWidgetW = m_pWaitCollapsedWidget->width();
        int waitWidgetH = m_pWaitCollapsedWidget->height();

        m_pWaitCollapsedWidget->setGeometry(waitWidgetX, waitWidgetY, waitWidgetW, waitWidgetH);

    }


    QMap<quint64 , sw::EmbendedWidgetStruct*>::iterator i;
    for (i = m_embeddedWidgets.begin(); i != m_embeddedWidgets.end(); ++i)
    {
        sw::EmbendedWidgetStruct * eStruct = i.value();
        switch (eStruct->alignType)
        {
        case EWA_LEFT_TOP_CORNER_OFFSET:     /*привязка виджета относительно левого верхнего угла с фиксированным смещением*/
        {
            //if(eStruct->collapsedWidgetPtr->isPinned() || !eStruct->allowMoving)
            {
                eStruct->collapsedWidgetPtr->move(mainWidgX + eStruct->alignPoint.x(), eStruct->alignPoint.y());
            }
        }break;
        case EWA_LEFT_BOTTOM_CORNER_OFFSET:  /*привязка виджета относительно левого нижнего угла с фиксированным смещением*/
        {
            //if(eStruct->collapsedWidgetPtr->isPinned() || !eStruct->allowMoving)
            {
                QPoint p(mainWidgX + eStruct->alignPoint.x()
                         , height() - eStruct->collapsedWidgetPtr->frameGeometry().height() - eStruct->alignPoint.y() );
                eStruct->collapsedWidgetPtr->move(p);
            }
        }break;
        case EWA_RIGHT_TOP_CORNER_OFFSET:    /*привязка виджета относительно правого верхнего угла с фиксированным смещением*/
        {
            //if(eStruct->collapsedWidgetPtr->isPinned() || !eStruct->allowMoving)
            {


                QPoint p(mainWidgX + mainWidgW - eStruct->collapsedWidgetPtr->frameGeometry().width() - eStruct->alignPoint.x()
                         , eStruct->alignPoint.y() );

                eStruct->collapsedWidgetPtr->move(p);
            }
        }break;
        case EWA_RIGHT_BOTTOM_CORNER_OFFSET: /*привязка виджета относительно правого нижнего угла с фиксированным смещением*/
        {
            //if(eStruct->collapsedWidgetPtr->isPinned() || !eStruct->allowMoving)
            {
                QPoint p(mainWidgX + mainWidgW - eStruct->collapsedWidgetPtr->frameGeometry().width() - eStruct->alignPoint.x()
                         , height() - eStruct->collapsedWidgetPtr->frameGeometry().height() - eStruct->alignPoint.y() );
                eStruct->collapsedWidgetPtr->move(p);
            }
        }break;
        case EWA_PERCENTAGE_ALIGN:            /*привязка виджета с динамическим смещением(процент от ширины/высоты виджета)*/
        {
            /// TODO Придумать условие
            //if(eStruct->collapsedWidgetPtr->isPinned() || !eStruct->allowMoving)
            if(0 != eStruct->collapsedWidgetPtr)
            {
                QPoint p((eStruct->alignPoint.x()*width())/100 - eStruct->collapsedWidgetPtr->frameGeometry().width()/2
                         , (eStruct->alignPoint.y()*height())/100 - eStruct->collapsedWidgetPtr->frameGeometry().height()/2 );
                eStruct->collapsedWidgetPtr->move(p);
            }
        }break;

        default:
            break;
        }
    }
}


QWidget * sw::SceneWidget::getMinimapContainerWidget()
{
    return m_minimapContainer;
}


//void sw::SceneWidget::showMenuBar()
//{
//    m_openMenuButton->hide();
//    m_topMenuBar->show();
//}


//void sw::SceneWidget::hideMenuBar()
//{
//    m_openMenuButton->show();
//    m_topMenuBar->hide();
//}

void sw::SceneWidget::slottomeout()
{
//    if(m_needGrabScreen)
//    {
//        if(0 != m_coordBar )
//            m_coordBar->repaint();

//        if( 0 != m_kompas)
//            m_kompas->repaint();

//        if( 0 != m_pInnerRightScrollArea && m_pInnerRightScrollArea->isVisible())
//            m_pInnerRightScrollArea->repaint();
//    }
//    update();

}


SceneWidget::SceneWidget(QWidget *parent)
    : AbstractSceneWidget(parent)
{
    listItem = 0;
    m_needGrabScreen = false;
    m_main3dWidget = 0;
    //    MiniMapViewer* m_miniView;
//    m_kompas = 0;
    m_rowScale = 0;
//    m_zoomScroll = 0;
    //m_topMenuBar = 0;
    m_coordBar = 0;
    m_kompasVisibleCB = 0;
    m_zoomBarVisibleCB = 0;
    m_minimapVisibleCB = 0;
    m_coordBarVisibleCB = 0;
    m_scaleBarVisibleCB = 0;
    // m_bookmarkVisibleCB = 0;
    //m_pBtnPrint = 0;
    //m_pBtnSave = 0;
    m_mainConteinerWidget = 0;
    m_buttonBoxWidget1 = 0;
    m_buttonBoxWidget2 = 0;
    m_buttonBoxWidget3 = 0;
    m_embendedBoxWidgets = 0;
    //m_openMenuButton = 0;
    m_navigation = 0;
    m_minimapContainer = 0;
    m_pRowScaleControl = 0;
    m_pBtnRecordVideo = 0;
    m_pBtnVideoSettings = 0;
    m_denyGeometryChange = false;
    m_denyMinimize = false;
    m_denyClose = false;
    m_reachZone = 0;
    m_pTimeBar = 0;
    m_loadSettingsComplete = false;
    m_embendedWidgetIdCounter = 0;
    menuBarFixedWidth = 500;
    m_embeddedWidgets.clear();


    m_pGetTimeTimer = new QTimer(this);
    m_pGetTimeTimer->setInterval(5);
    connect(m_pGetTimeTimer, SIGNAL(timeout()), this, SLOT(slotGetTimeTimeout()));
    m_pGetTimeTimer->start();

//    QTranslator *trans = new QTranslator(this);
//    if(trans->load("./translate/" + QString("libscenewidget.qm")))
//        QCoreApplication::installTranslator(trans);

//    QTranslator *trans1 = new QTranslator(this);
//    if(trans1->load("./translate/" + QString("libscenecontrol.qm")))
//        QCoreApplication::installTranslator(trans1);


    VideoPlayerTypes::VideoSaveSettings sett ;
    sett.denyCloseWindow = true;
    sett.denyMinimizeWindow = false;
    sett.recordControls = false;
//    sett.fileDestination = QDir::homePath();
    sett.fileFormat = tr("Auto");
    sett.filePrefix = "default";

    QString pathDS = ""; // QFileInfo(contur::plugins::ConturPlugin::configAppPath()).path()+ QDir::separator()+"libvideorecordsettings.ini";
    QSettings settings(pathDS, QSettings::NativeFormat);

    sett.denyCloseWindow = settings.value("DenyCloseWindow",true).toBool();
    sett.denyMinimizeWindow = settings.value("DenyMinimizeWindow",true).toBool();
    sett.recordControls = settings.value("ShowRecordControls",true).toBool();
//    sett.fileDestination = settings.value("FileDestination",QDir::homePath()).toString();
    sett.fileFormat = settings.value("FileFormat",QString("Auto")).toString();
    sett.filePrefix = settings.value("FilePrefix",QString("defaultVideoFile")).toString();
    settings.sync();




    m_pWaitCollapsedWidget = new CollapsedWidget();
    m_pWaitWidget = new WaitWidget(m_pWaitCollapsedWidget);
    connect(m_pWaitWidget, SIGNAL(signalOkClicked()), m_pWaitCollapsedWidget, SLOT(close()));
    m_pWaitCollapsedWidget->setVisisbleCollapseButton(false);
    m_pWaitCollapsedWidget->setVisisblePinButton(false);
    m_pWaitCollapsedWidget->setVisisbleCloseButton(false);
    m_pWaitCollapsedWidget->setPinned(false);
    m_pWaitCollapsedWidget->moveOnlyOnParent(true);
    m_pWaitCollapsedWidget->insertWidget(m_pWaitWidget);
    m_pWaitCollapsedWidget->setModal(true);
    m_pWaitCollapsedWidget->setWindowTitle(tr("Search OSM objects"));

    m_pOsmParamsContainerWidget = 0;

    m_opennedDataType = ODT_GEO_SCENE;
    //setStyle(new QPlastiqueStyle);
    QGridLayout * lt = new QGridLayout(this);
    lt->setMargin(0);
    lt->setSpacing(0);

    m_needGrabScreen = false;
    m_grabMainViewTimer = new QTimer(this);
    m_grabMainViewTimer->setSingleShot(true);
    m_grabMainViewTimer->setInterval(500);
    connect(m_grabMainViewTimer, SIGNAL(timeout()), this, SLOT(slotGrabMainViewWidget()));

    m_main3dWidget = 0;
    QTimer * tmr = new QTimer();

    tmr->setInterval(100);
    connect(tmr, SIGNAL(timeout()), this, SLOT(slottomeout()));
    tmr->start();

    m_mainConteinerWidget = new MainConteinerWidget(this);

    m_pOuterRightScrollArea = new ScrollPanel(false, this);
    m_pInnerRightScrollArea = new ScrollPanel(true, this);
    m_pOuterLeftScrollArea = new ScrollPanel(false,this);

    lt->addWidget(m_pOuterLeftScrollArea,1,0,1,1);
    lt->addWidget(m_mainConteinerWidget,1,1,1,1);
    lt->addWidget(m_pOuterRightScrollArea, 1,2,1,1);

    m_pInnerRightScrollArea->hide();
    m_pOuterRightScrollArea->hide();
    m_pOuterLeftScrollArea->hide();

    connect(m_pOuterRightScrollArea, SIGNAL(signalGeomChanged()), this, SLOT(slotUpdateChildsGeometry()));
    connect(m_pInnerRightScrollArea, SIGNAL(signalGeomChanged()), this, SLOT(slotUpdateChildsGeometry()));
    connect(m_pInnerRightScrollArea, SIGNAL(signalResendUnusedEvents(QEvent*)), this, SLOT(slotResendEventsFromScrollWidget(QEvent*)));

    connect(m_pOuterLeftScrollArea, SIGNAL(signalGeomChanged()), this, SLOT(slotUpdateChildsGeometry()));



//    if(!QResource::registerResource( "resources/resources.rcc" ))
//        QMessageBox::information(NULL, QObject::tr("scenewidget"), QObject::tr("no resource file(resources.rcc)"));

//    if(!QResource::registerResource( "resources/swicons.rcc" ))
//        QMessageBox::information(NULL, QObject::tr("scenewidget"), QObject::tr("no resource file(swicons.rcc)"));

//    if(!QResource::registerResource( "resources/swqml.rcc" ))
//        QMessageBox::information(NULL, QObject::tr("scenewidget"), QObject::tr("no resource file(swqml.rcc)"));

//    if(!QResource::registerResource( "resources/swcss.rcc" ))
//        QMessageBox::information(NULL, QObject::tr("scenewidget"), QObject::tr("no resource file(swqml.rcc)"));


    initKompas();
    initZoomScroll();
    m_minimapContainer = new MiniMapContainerWidget(this);
    m_minimapContainer->hide();
    initCoordBar();
    initRowScale();
    initNavigation();
    initMenuBar();
    initReachZone();
    initTimeBar();

    resize(size());

    ///Пока не понятно чё делать с этим.Все настройки управляются панелью
    ///loadSettings();
    /////////////////////////////////////////////////////////////////////
    slotUpdateChildsGeometry();

    QTimer::singleShot(500,this, SLOT(slotUpdateChildsGeometry()));



}

void sw::SceneWidget::setSettingsPath(QString settFilepath)
{
    m_settingsPath = settFilepath;
    ///loadSettings();
}

void sw::SceneWidget::saveSettings()
{
    if(!m_loadSettingsComplete)
        return;

    QString pathVisSett = m_settingsPath; //
    QSettings settings(pathVisSett, QSettings::NativeFormat);



    switch (m_opennedDataType)
    {
    case ODT_SIMLE_PICTURE:
    {
        settings.remove("VisibilitySimplePictureSettings");
        settings.beginReadArray("VisibilitySimplePictureSettings");

        saveSettingsPrivate(settings);

        settings.endArray();

    }break;
    case ODT_GEO_PICTURE:
    {
        settings.remove("VisibilityGeoPictureSettings");
        settings.beginReadArray("VisibilityGeoPictureSettings");

        saveSettingsPrivate(settings);

        settings.endArray();
    }break;
    case ODT_GEO_SCENE:
    {
        settings.remove("VisibilityGeoSceneSettings");
        settings.beginReadArray("VisibilityGeoSceneSettings");

        saveSettingsPrivate(settings);

        settings.endArray();
    }break;
    case ODT_3D:
    {
        settings.remove("Visibility3DSettings");
        settings.beginReadArray("Visibility3DSettings");

        saveSettingsPrivate(settings);
        settings.setValue("3D_ATMO", (MenuBarButtonBoxItem::BS_PRESSED == m_buttonBoxWidget2->getButtonState(sw::BTN_3D_ATMO)) );
        settings.setValue("3D_CLOUDS", (MenuBarButtonBoxItem::BS_PRESSED == m_buttonBoxWidget2->getButtonState(sw::BTN_3D_CLOUDS)) );
        settings.setValue("3D_COORDGRID", (MenuBarButtonBoxItem::BS_PRESSED == m_buttonBoxWidget2->getButtonState(sw::BTN_3D_COORDGRID)) );
        settings.setValue("3D_MOON", (MenuBarButtonBoxItem::BS_PRESSED == m_buttonBoxWidget2->getButtonState(sw::BTN_3D_MOON)) );
        settings.setValue("3D_RELIEF", (MenuBarButtonBoxItem::BS_PRESSED == m_buttonBoxWidget2->getButtonState(sw::BTN_3D_RELIEF)) );
        settings.setValue("3D_STARS", (MenuBarButtonBoxItem::BS_PRESSED == m_buttonBoxWidget2->getButtonState(sw::BTN_3D_STARS)) );
        settings.setValue("3D_SUNLIGHT", (MenuBarButtonBoxItem::BS_PRESSED == m_buttonBoxWidget2->getButtonState(sw::BTN_3D_SUNLIGHT)) );
        settings.setValue("3D_WATER", (MenuBarButtonBoxItem::BS_PRESSED == m_buttonBoxWidget2->getButtonState(sw::BTN_3D_WATER)) );
        settings.setValue("3D_WATER_DENSITY", (MenuBarButtonBoxItem::BS_PRESSED == m_buttonBoxWidget2->getButtonState(sw::BTN_3D_WATER_DENSITY)) );
        settings.setValue("3D_VIEW_SUBTERRANEAN", (MenuBarButtonBoxItem::BS_PRESSED == m_buttonBoxWidget2->getButtonState(sw::BTN_3D_VIEW_SUBTERRANEAN)) );

        settings.endArray();
    }break;
    case ODT_VIDEO:
    {
        settings.remove("VisibilityVideoSceneSettings");
        settings.beginReadArray("VisibilityVideoSceneSettings");

        saveSettingsPrivate(settings);

        settings.endArray();
    }break;
    default:
        break;
    }
}


void sw::SceneWidget::loadSettingsPrivate(QSettings & settings)
{

    bool kompasVis = settings.value("KompasVisibility",true).toBool();
    slotSetKompasVisible(kompasVis);
    m_kompasVisibleCB->setSelected(kompasVis);

    bool coordBarVis = settings.value("CoordBarVisibility",true).toBool();
    slotSetCoordinateBarVisible(coordBarVis);
    m_coordBarVisibleCB->setSelected(coordBarVis);

    bool zoomBarVis = settings.value("ZoomBarVisibility",true).toBool();
    slotSetZoomBarVisible(zoomBarVis);
    m_zoomBarVisibleCB->setSelected(zoomBarVis);

    bool minimapVis = settings.value("MinimapVisibility",true).toBool();
    slotSetMinimapVisible(minimapVis);
    m_minimapVisibleCB->setSelected(minimapVis);

    bool rowScaleVis = settings.value("RowScaleVisibility",true).toBool();
    slotSetRowScaleVisible(rowScaleVis);
    m_scaleBarVisibleCB->setSelected(rowScaleVis);


    bool bookmarkBarVis = settings.value("BookmarkVisibility",true).toBool();
    //slotSetBookmarkVisible(bookmarkBarVis);
    //m_bookmarkVisibleCB->setSelected(bookmarkBarVis);

    if(0 != m_timebarVisisbleCB)
    {
        bool timeBarVis = settings.value("DateTimeBarVisibility",true).toBool();
        slotSetTimeBarVisible(timeBarVis);
        m_timebarVisisbleCB->setSelected(timeBarVis);
    }

    //    if(0 != m_pBtnShowSearch)
    //    {
    //        bool searchMenuVis = settings.value("SearchMenuVisisbility",false).toBool();
    //        m_pBtnShowSearch->setChecked(searchMenuVis);
    //    }
}

void sw::SceneWidget::saveSettingsPrivate(QSettings & settings)
{
//    if(0 != m_kompas)
//    {
//        if(m_kompas->isVisible())
//        {
//            settings.setValue("KompasVisibility",true);
//        }
//        else
//        {
//            settings.setValue("KompasVisibility",false);
//        }
//    }
//    else
//    {
//        settings.setValue("KompasVisibility",false);
//    }


    if(0 != m_coordBar)
    {
        if(m_coordBar->isVisible())
        {
            settings.setValue("CoordBarVisibility",true);
        }
        else
        {
            settings.setValue("CoordBarVisibility",false);
        }
    }
    else
    {
        settings.setValue("CoordBarVisibility",false);
    }


//    if(0 != m_zoomScroll)
//    {
//        if(m_zoomScroll->isVisible())
//        {
//            settings.setValue("ZoomBarVisibility",true);
//        }
//        else
//        {
//            settings.setValue("ZoomBarVisibility",false);
//        }
//    }
//    else
//    {
//        settings.setValue("ZoomBarVisibility",false);
//    }


    if(0 != m_minimapContainer)
    {
        //        if(m_minimapContainer->isVisible())
        if(m_minimapVisibleCB->getSelectState())
        {
            settings.setValue("MinimapVisibility",true);
        }
        else
        {
            settings.setValue("MinimapVisibility",false);
        }
    }
    else
    {
        settings.setValue("MinimapVisibility",false);
    }

    if(0 != m_rowScale)
    {
        if(m_rowScale->isVisible())
        {
            settings.setValue("RowScaleVisibility",true);
        }
        else
        {
            settings.setValue("RowScaleVisibility",false);
        }
    }
    else
    {
        settings.setValue("RowScaleVisibility",false);
    }



    if(0 != m_pTimeBar)
    {
        if(m_pTimeBar->isVisible())
        {
            settings.setValue("DateTimeBarVisibility",true);
        }
        else
        {
            settings.setValue("DateTimeBarVisibility",false);
        }
    }
    else
    {
        settings.setValue("DateTimeBarVisibility",false);
    }


    //    if(0 != m_pBtnShowSearch)
    //    {
    //        settings.setValue("SearchMenuVisisbility",m_pBtnShowSearch->isChecked());
    //    }
}

void sw::SceneWidget::loadSettings()
{
    m_loadSettingsComplete = false;

    QString pathVisSett = m_settingsPath; // QFileInfo(contur::plugins::ConturPlugin::configAppPath()).path() + QDir::separator()+"libkbase.ini";
    QSettings settings(pathVisSett, QSettings::NativeFormat);
    settings.sync();

    switch (m_opennedDataType)
    {
    case ODT_SIMLE_PICTURE:
    {
        settings.beginReadArray("VisibilitySimplePictureSettings");

        loadSettingsPrivate(settings);

        settings.endArray();

    }break;
    case ODT_GEO_PICTURE:
    {
        settings.beginReadArray("VisibilityGeoPictureSettings");

        loadSettingsPrivate(settings);

        settings.endArray();
    }break;
    case ODT_GEO_SCENE:
    {
        settings.beginReadArray("VisibilityGeoSceneSettings");

        loadSettingsPrivate(settings);

        settings.endArray();
    }break;
    case ODT_3D:
    {
        settings.beginReadArray("Visibility3DSettings");

        loadSettingsPrivate(settings);

        if(settings.value("3D_ATMO", false ).toBool())
        {
            m_buttonBoxWidget2->setButtonState(sw::BTN_3D_ATMO, MenuBarButtonBoxItem::BS_PRESSED);
        }
        else
        {
            m_buttonBoxWidget2->setButtonState(sw::BTN_3D_ATMO, MenuBarButtonBoxItem::BS_RELEASED);
        }


        if(settings.value("3D_CLOUDS", false ).toBool())
        {
            m_buttonBoxWidget2->setButtonState(sw::BTN_3D_CLOUDS, MenuBarButtonBoxItem::BS_PRESSED);
        }
        else
        {
            m_buttonBoxWidget2->setButtonState(sw::BTN_3D_CLOUDS, MenuBarButtonBoxItem::BS_RELEASED);
        }


        if(settings.value("3D_COORDGRID", false ).toBool())
        {
            m_buttonBoxWidget2->setButtonState(sw::BTN_3D_COORDGRID, MenuBarButtonBoxItem::BS_PRESSED);
        }
        else
        {
            m_buttonBoxWidget2->setButtonState(sw::BTN_3D_COORDGRID, MenuBarButtonBoxItem::BS_RELEASED);
        }


        if(settings.value("3D_MOON", false ).toBool())
        {
            m_buttonBoxWidget2->setButtonState(sw::BTN_3D_MOON, MenuBarButtonBoxItem::BS_PRESSED);
        }
        else
        {
            m_buttonBoxWidget2->setButtonState(sw::BTN_3D_MOON, MenuBarButtonBoxItem::BS_RELEASED);
        }

        if(settings.value("3D_RELIEF", false ).toBool())
        {
            m_buttonBoxWidget2->setButtonState(sw::BTN_3D_RELIEF, MenuBarButtonBoxItem::BS_PRESSED);
        }
        else
        {
            m_buttonBoxWidget2->setButtonState(sw::BTN_3D_RELIEF, MenuBarButtonBoxItem::BS_RELEASED);
        }

        if(settings.value("3D_STARS", false ).toBool())
        {
            m_buttonBoxWidget2->setButtonState(sw::BTN_3D_STARS, MenuBarButtonBoxItem::BS_PRESSED);
        }
        else
        {
            m_buttonBoxWidget2->setButtonState(sw::BTN_3D_STARS, MenuBarButtonBoxItem::BS_RELEASED);
        }


        if(settings.value("3D_SUNLIGHT", false ).toBool())
        {
            m_buttonBoxWidget2->setButtonState(sw::BTN_3D_SUNLIGHT, MenuBarButtonBoxItem::BS_PRESSED);
        }
        else
        {
            m_buttonBoxWidget2->setButtonState(sw::BTN_3D_SUNLIGHT, MenuBarButtonBoxItem::BS_RELEASED);
        }

        if(settings.value("3D_WATER", false ).toBool())
        {
            m_buttonBoxWidget2->setButtonState(sw::BTN_3D_WATER, MenuBarButtonBoxItem::BS_PRESSED);
        }
        else
        {
            m_buttonBoxWidget2->setButtonState(sw::BTN_3D_WATER, MenuBarButtonBoxItem::BS_RELEASED);
        }

        if(settings.value("3D_WATER_DENSITY", false ).toBool())
        {
            m_buttonBoxWidget2->setButtonState(sw::BTN_3D_WATER_DENSITY, MenuBarButtonBoxItem::BS_PRESSED);
        }
        else
        {
            m_buttonBoxWidget2->setButtonState(sw::BTN_3D_WATER_DENSITY, MenuBarButtonBoxItem::BS_RELEASED);
        }

        if(settings.value("3D_VIEW_SUBTERRANEAN", false ).toBool())
        {
            m_buttonBoxWidget2->setButtonState(sw::BTN_3D_VIEW_SUBTERRANEAN, MenuBarButtonBoxItem::BS_PRESSED);
        }
        else
        {
            m_buttonBoxWidget2->setButtonState(sw::BTN_3D_VIEW_SUBTERRANEAN, MenuBarButtonBoxItem::BS_RELEASED);
        }


        settings.endArray();
    }break;
    case ODT_VIDEO:
    {
        settings.beginReadArray("VisibilityVideoSceneSettings");

        loadSettingsPrivate(settings);

        settings.endArray();
    }break;
    default:
        break;
    }

    m_loadSettingsComplete = true;
}


void sw::SceneWidget::initTimeBar()
{
    m_pTimeBar = new DateTimeWidget(this);
    m_pTimeBar->setFixedSize(190,35);


}


void sw::SceneWidget::setWaitState(QString str, bool isFinalState)
{
    m_pWaitWidget->setInfoText(str, isFinalState);
}

void sw::SceneWidget::setOpennedDataType(OpennedDataType dt)
{
    m_opennedDataType = dt;
    reInit();

}

void sw::SceneWidget::addWidgetToRightBar(CollapsedWidget *wdg)
{
    m_pOuterRightScrollArea->appendWidget(wdg);
}


//void sw::SceneWidget::addWidgetToRightBar(CollapsedWidget *wdg)
//{
//    m_pOuterRightScrollArea->appendWidget(wdg);
//}


void sw::SceneWidget::slotOSMObjectSelected(quint64 id,QPoint p,QVariantMap mp)
{
//    if(0 != id)
//    {
//        if(!m_pOsmParamsContainerWidget->isVisible())
//            m_pOsmParamsContainerWidget->show();

//        QString str = mp.value("name").toString();
//        qDebug() << "SceneWidget::slotOSMObjectSelected(): OSM object selected. Name " << str;
//        m_pOsmParamsContainerWidget->setWindowTitle( tr("Object params: ") + str);
//        m_pOsmParamsWidget->setSelectedObject(id, mp);
//    }
//    else
//    {
//        if(m_pOsmParamsContainerWidget->isVisible())
//            m_pOsmParamsContainerWidget->hide();

//        m_pOsmParamsContainerWidget->hide();
//        //m_pOsmParamsWidget->setSelectedObject(0,QVariantMap());
//    }
}


void sw::SceneWidget::reInit()
{
    ///loadSettings();
}


void sw::SceneWidget::initMenuBar()
{
    m_buttonSize = 26;
    /**
QToolButton *tmpBtn = new QToolButton(this);
tmpBtn->setCheckable(true);
tmpBtn->setChecked(true);
tmpBtn->resize(256,256);
*/
//    m_topMenuBar = new MenuBar(this);
//    m_openMenuButton = new MenuOpenButtonWidget(this);
//    connect(m_openMenuButton, SIGNAL(needHide()), m_topMenuBar, SIGNAL(needShow()));
//    connect(m_topMenuBar, SIGNAL(needHide()), m_openMenuButton, SIGNAL(needShow()));

//    m_pBtnSave = new MenuButtonItem(this);
//    m_pBtnSave->setFixedSize(m_buttonSize,m_buttonSize);
//    m_pBtnSave->setIcon(QIcon(":/273_capture_window_on.png"));
//    m_pBtnSave->setToolTip(QString(tr("Save screenshot")));
//    m_pBtnSave->show();
//    m_topMenuBar->addItem(m_pBtnSave);


    //    m_pBtnSaveSession = new MenuButtonItem(this);
    //    m_pBtnSaveSession->setFixedSize(m_buttonSize,m_buttonSize);
    //    m_pBtnSaveSession->setIcon(QIcon(":/93_save.png"));
    //    m_pBtnSaveSession->setToolTip(QString(tr("Save session")));
    //    m_topMenuBar->addItem(m_pBtnSaveSession);

    //    m_pBtnLoadSession = new MenuButtonItem(this);
    //    m_pBtnLoadSession->setFixedSize(m_buttonSize,m_buttonSize);
    //    m_pBtnLoadSession->setIcon(QIcon(":/27_open.png"));
    //    m_pBtnLoadSession->setToolTip(QString(tr("Load session")));
    //    m_topMenuBar->addItem(m_pBtnLoadSession);

//    m_pBtnReloadClicked = new MenuButtonItem(this);
//    m_pBtnReloadClicked->setFixedSize(m_buttonSize,m_buttonSize);
//    m_pBtnReloadClicked->setIcon(QIcon(":/10_re-render_1.png"));
//    m_pBtnReloadClicked->setToolTip(QString(tr("Reload session")));
//    m_topMenuBar->addItem(m_pBtnReloadClicked);


    //    connect(m_pBtnSaveSession,SIGNAL(pressed()), this, SIGNAL(signalBtnSaveSessionClicked()));
    //    connect(m_pBtnLoadSession,SIGNAL(pressed()), this, SIGNAL(signalBtnLoadSessionClicked()));

//    connect(m_pBtnSave,SIGNAL(clicked()), this, SLOT(slotBtnSaveClicked()));
//    connect(m_pBtnSave,SIGNAL(clicked()), this, SIGNAL(signalSaveButtonClicked()));
 //   connect(m_pBtnReloadClicked,SIGNAL(pressed()), this, SLOT(slotReloadButtonClicked()));

   // m_pBtnPrint = 0;
    //m_topMenuBar->addVerticalLine();


    m_buttonBoxWidget1 = new ButtonBoxWidget(m_buttonSize, buttonCountBbw1,this);
    connect(m_buttonBoxWidget1, SIGNAL(signalButtonClicked(int)), this, SLOT(slotBBW1ButtonClicked(int)) );

    m_buttonBoxWidget1->setOnePressed(true);
/*
    MenuBarButtonBoxItem * tmpBtn  = m_buttonBoxWidget1->addItem((int)sw::BTN_CALC_ANGLES, QIcon(":/69_angle_measure(2d).png"),
                                                                 QIcon(":/117_angle_measure(2d)_off.png"), QString(tr("Measure angle")), true,QString(tr("Measure angle")));
    connect(tmpBtn, SIGNAL(clicked()), this, SLOT(slotAngleMeasureClicked()) );

    tmpBtn = m_buttonBoxWidget1->addItem((int)sw::BTN_CALC_LENGHT, QIcon(":/67_distance_measure(2d).png"),
                                         QIcon(":/116_distance_measure(2d)_off.png"), QString(tr("Measure distance"))
                                         , true,QString(tr("Measure distance")));
    connect(tmpBtn, SIGNAL(clicked()), this, SLOT(slotDistanceMeasureClicked()) );

    tmpBtn = m_buttonBoxWidget1->addItem((int)sw::BTN_CALC_REACH, QIcon(":/73_reachable_zones_measure.png"),
                                         QIcon(":/122_reachable_zones_measure_off.png"), QString(tr("Measure reach"))
                                         , true,QString(tr("Measure reach")));
    connect(tmpBtn, SIGNAL(clicked()), this, SLOT(slotReachMeasureClicked()) );

    tmpBtn = m_buttonBoxWidget1->addItem((int)sw::BTN_CALC_SQUARE, QIcon(":/71_size_measure.png"),
                                         QIcon(":/120_size_measure_off.png"), QString(tr("Measure area"))
                                         , true,QString(tr("Measure area")));
    connect(tmpBtn, SIGNAL(clicked()), this, SLOT(slotAreaMeasureClicked()) );

    tmpBtn = m_buttonBoxWidget1->addItem((int)sw::BTN_CALC_ROUTE, QIcon(":/70_routes_measure.png"),
                                         QIcon(":/119_routes_measure_off.png"), QString(tr("Measure route"))
                                         , true,QString(tr("Measure route")));
    connect(tmpBtn, SIGNAL(clicked()), this, SLOT(slotRouteMeasureClicked()) );
*/
   // m_topMenuBar->addItem(m_buttonBoxWidget1);
   // m_topMenuBar->addVerticalLine();


    m_buttonBoxWidget2 = new ButtonBoxWidget(m_buttonSize, buttonCountBbw2,this);
    connect(m_buttonBoxWidget2, SIGNAL(signalButtonClicked(int)), this, SLOT(slotBBW2ButtonClicked(int)) );
    //m_topMenuBar->addItem(m_buttonBoxWidget2);
   // m_topMenuBar->addVerticalLine();

    m_buttonBoxWidget3 = new ButtonBoxWidget(m_buttonSize, buttonCountBbw3,this);
    connect(m_buttonBoxWidget3, SIGNAL(signalButtonClicked(int)), this, SLOT(slotBBW3ButtonClicked(int)) );
    //m_topMenuBar->addItem(m_buttonBoxWidget3);
   // m_topMenuBar->addVerticalLine();

    m_embendedBoxWidgets = new ButtonBoxWidget(m_buttonSize, 1,this);
    connect(m_embendedBoxWidgets, SIGNAL(signalButtonClicked(int)), this, SLOT(slotEmbendedButtonClicked(int)) );
   // m_topMenuBar->addItem(m_embendedBoxWidgets);
   // m_topMenuBar->addVerticalLine();

//    m_topMenuBar->setFixedWidth(200);



//    m_embendedBoxWidgets->setFixedWidth(100);


    /*вид*/
    m_menuListWidget = new VisibleListWidget(this);
    connect(m_menuListWidget, SIGNAL(signalVisisbleChanged()), this, SLOT(slotUpdateChildsGeometry()));

    m_menuListWidget->setFixedWidth(200);
    m_menuListWidget->setMinimumWidth(180);
    m_kompasVisibleCB = m_menuListWidget->addItem(QString(tr("show kompas")));
    connect(m_kompasVisibleCB, SIGNAL(sigCheckChange(bool)), this, SLOT(slotSetKompasVisible(bool)));
    m_zoomBarVisibleCB = m_menuListWidget->addItem(QString(tr("show zoom bar")));
    connect(m_zoomBarVisibleCB, SIGNAL(sigCheckChange(bool)), this, SLOT(slotSetZoomBarVisible(bool)));

    m_minimapVisibleCB  = m_menuListWidget->addItem(QString(tr("show minimap")));
    connect(m_minimapVisibleCB, SIGNAL(sigCheckChange(bool)), this, SLOT(slotSetMinimapVisible(bool)));

    m_coordBarVisibleCB = m_menuListWidget->addItem(QString(tr("show coordinate bar")));
    connect(m_coordBarVisibleCB , SIGNAL(sigCheckChange(bool)), this, SLOT(slotSetCoordinateBarVisible(bool)));

    m_scaleBarVisibleCB = m_menuListWidget->addItem(QString(tr("show scale bar")));
    connect(m_scaleBarVisibleCB, SIGNAL(sigCheckChange(bool)), this, SLOT(slotSetRowScaleVisible(bool)));

    // m_bookmarkVisibleCB = m_menuListWidget->addItem(QString(tr("show bookmark")));

    m_timebarVisisbleCB = m_menuListWidget->addItem(QString(tr("show timebar")));
    connect(m_timebarVisisbleCB, SIGNAL(sigCheckChange(bool)), this, SLOT(slotSetTimeBarVisible(bool)));

   // m_topMenuBar->addItem(m_menuListWidget);
   // m_topMenuBar->addVerticalLine();

    /*
    m_pBtnPropertyObject = new MenuButtonItem(this);
    m_pBtnPropertyObject->setFixedSize(m_buttonSize,m_buttonSize);
    m_pBtnPropertyObject->setIcon(QIcon(":/435_icon_obj_info.png"));
//    m_pBtnSave->setToolTip(QString(tr("Save screenshot")));
    m_topMenuBar->addItem(m_pBtnPropertyObject);
    connect(m_pBtnPropertyObject,SIGNAL(clicked()), this, SLOT(slotBtnPropertyObject()));
    m_pBtnPropertyObject->hide();
*/

    m_buttonBoxWidget4 = new ButtonBoxWidget(m_buttonSize, buttonCountBbw4,this);
    connect(m_buttonBoxWidget4, SIGNAL(signalButtonClicked(int)), this, SLOT(slotBBW4ButtonClicked(int)) );
    //m_topMenuBar->addItem(m_buttonBoxWidget4);
    //    m_topMenuBar->addVerticalLine();
    m_buttonBoxWidget4->hide();

    /*поиск*/
    //m_topMenuBar->addStretch(1);

  //  m_topMenuBar->addVerticalLine(1);
//    m_topMenuBar->setBarWidth();
//    m_topMenuBar->setViewMenuBar();


   // int ww = m_topMenuBar->prefferWidth() ;
    setMinimumSize(400 + 250 ,500);
    resize(400,500);
    //m_topMenuBar->setVisibleLine(MenuBar::MBL_MAIN, true);
    m_buttonBoxWidget2->hide();
    m_buttonBoxWidget3->hide();
}


void SceneWidget::slotReloadButtonClicked()
{
    int mainWidgW = (width() - m_pOuterLeftScrollArea->width()*(int)(m_pOuterLeftScrollArea->isVisible())
                     - m_pOuterRightScrollArea->width()*(int)(m_pOuterRightScrollArea->isVisible()) );
    int mainWidgX = m_pOuterLeftScrollArea->width()*(int)(m_pOuterLeftScrollArea->isVisible());

    QMap<quint64 , sw::EmbendedWidgetStruct*>::iterator i;
    for (i = m_embeddedWidgets.begin(); i != m_embeddedWidgets.end(); ++i)
    {
        sw::EmbendedWidgetStruct * eStruct = i.value();
        switch (eStruct->alignType)
        {
        case EWA_LEFT_TOP_CORNER_OFFSET:     /*привязка виджета относительно левого верхнего угла с фиксированным смещением*/
        {
            //if(eStruct->collapsedWidgetPtr->isPinned() || !eStruct->allowMoving)
            {
                eStruct->collapsedWidgetPtr->move(mainWidgX + eStruct->alignPoint.x(), eStruct->alignPoint.y());
            }
        }break;
        case EWA_LEFT_BOTTOM_CORNER_OFFSET:  /*привязка виджета относительно левого нижнего угла с фиксированным смещением*/
        {
            //if(eStruct->collapsedWidgetPtr->isPinned() || !eStruct->allowMoving)
            {
                QPoint p(mainWidgX + eStruct->alignPoint.x()
                         , height() - eStruct->collapsedWidgetPtr->frameGeometry().height() - eStruct->alignPoint.y() );
                eStruct->collapsedWidgetPtr->move(p);
            }
        }break;
        case EWA_RIGHT_TOP_CORNER_OFFSET:    /*привязка виджета относительно правого верхнего угла с фиксированным смещением*/
        {
            //if(eStruct->collapsedWidgetPtr->isPinned() || !eStruct->allowMoving)
            {


                QPoint p(mainWidgX + mainWidgW - eStruct->collapsedWidgetPtr->frameGeometry().width() - eStruct->alignPoint.x()
                         , eStruct->alignPoint.y() );

                eStruct->collapsedWidgetPtr->move(p);
            }
        }break;
        case EWA_RIGHT_BOTTOM_CORNER_OFFSET: /*привязка виджета относительно правого нижнего угла с фиксированным смещением*/
        {
            //if(eStruct->collapsedWidgetPtr->isPinned() || !eStruct->allowMoving)
            {
                QPoint p(mainWidgX + mainWidgW - eStruct->collapsedWidgetPtr->frameGeometry().width() - eStruct->alignPoint.x()
                         , height() - eStruct->collapsedWidgetPtr->frameGeometry().height() - eStruct->alignPoint.y() );
                eStruct->collapsedWidgetPtr->move(p);
            }
        }break;
        case EWA_PERCENTAGE_ALIGN:            /*привязка виджета с динамическим смещением(процент от ширины/высоты виджета)*/
        {
            //if(eStruct->collapsedWidgetPtr->isPinned() || !eStruct->allowMoving)
            {
                QPoint p((eStruct->alignPoint.x()*width())/100 - eStruct->collapsedWidgetPtr->frameGeometry().width()/2
                         , (eStruct->alignPoint.y()*height())/100 - eStruct->collapsedWidgetPtr->frameGeometry().height()/2 );

                eStruct->collapsedWidgetPtr->move(p);
                QRect rr = rect();
                QRect r = eStruct->collapsedWidgetPtr->frameGeometry();
                QRect r1 = eStruct->collapsedWidgetPtr->geometry();
                QRect r2 = eStruct->collapsedWidgetPtr->rect();

                if(!rect().contains(r))
                {
                    eStruct->collapsedWidgetPtr->blockSignals(true);
                    eStruct->collapsedWidgetPtr->setUpdatesEnabled(false);

                    bool moved = false;
                    QLineF l(rect().center(), p);

                    QPoint validCenter(width()/2 - eStruct->collapsedWidgetPtr->frameGeometry().width()/2
                                       , height()/2 - eStruct->collapsedWidgetPtr->frameGeometry().height()/2);

                    for(int i =0; i<l.length(); i++)
                    {
                        p = l.pointAt(0.01*i).toPoint();
                        QRect  tmpRect = r;
                        tmpRect.moveCenter(p);
                        if(rect().contains(tmpRect))
                        {
                            validCenter = p;
                        }
                        else
                        {
                            moved = true;
                            QPoint alignPoint  = QPoint(validCenter.x() - eStruct->collapsedWidgetPtr->frameGeometry().width()/2,
                                                        validCenter.y() - eStruct->collapsedWidgetPtr->frameGeometry().height()/2 );

                            QPoint percPoint(100*((alignPoint.x()*1.0 + eStruct->collapsedWidgetPtr->frameGeometry().width()/2.0)/width())
                                             , 100*((alignPoint.y()*1.0 + eStruct->collapsedWidgetPtr->frameGeometry().height()/2.0)/height()) );

                            eStruct->alignPoint = percPoint;
                            eStruct->collapsedWidgetPtr->move(alignPoint);
                            break;
                        }
                    }

                    eStruct->collapsedWidgetPtr->blockSignals(false);
                    eStruct->collapsedWidgetPtr->setUpdatesEnabled(true);

                    if(moved)
                    {
                        emit signalEmbWidgetSettingsChanged(eStruct);
                    }

                }
            }
        }break;

        default:
            break;
        }
    }
}

void sw::SceneWidget::slotBtnVideoRecordChecked(bool isChecked)
{

    VideoPlayerTypes::VideoSaveSettings sett ;
    sett.denyCloseWindow = true;
    sett.denyMinimizeWindow = false;
    sett.recordControls = false;
//    sett.fileDestination = QDir::homePath();
    sett.fileFormat = tr("Auto");
    sett.filePrefix = "default";

    QString pathDS = ""; //QFileInfo(contur::plugins::ConturPlugin::configAppPath()).path() + QDir::separator()+"libvideorecordsettings.ini";
    QSettings settings(pathDS, QSettings::NativeFormat);
    settings.sync();


    sett.denyCloseWindow = settings.value("DenyCloseWindow",true).toBool();
    sett.denyMinimizeWindow = settings.value("DenyMinimizeWindow",true).toBool();
    sett.recordControls = settings.value("ShowRecordControls",true).toBool();
//    sett.fileDestination = settings.value("FileDestination",QDir::homePath()).toString();
    sett.fileFormat = settings.value("FileFormat",QString("Auto")).toString();
    sett.filePrefix = settings.value("FilePrefix",QString("defaultVideoFile")).toString();

    if(isChecked)
    {
        m_policyBeforeRecord = this->sizePolicy();
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        m_denyClose = sett.denyCloseWindow ;
        m_denyMinimize = sett.denyMinimizeWindow ;
        m_denyGeometryChange = true;
        m_defaultFlags = windowFlags();

        QWidget * parwdg = parentWidget();
        if(0 == parwdg)
            parwdg = this;
        QRect wpos = parwdg->geometry();
        m_minSizeBeforeRecord = minimumSize();
        m_geom = wpos;
        // parwdg->setFixedSize(width(), height());
        parwdg->setWindowFlags( (Qt::CustomizeWindowHint /*| Qt::WindowTitleHint*/ | Qt::WindowStaysOnTopHint)
                                );

        /*if(!m_denyClose)
        {
            setWindowFlags(windowFlags() |Qt::WindowCloseButtonHint| Qt::CustomizeWindowHint);
            //setWindowFlag(windowFlags() & (0xffffffff &  Qt::WindowCloseButtonHint);
        }

        if(!m_denyMinimize)
        {
            setWindowFlags(windowFlags() | Qt::WindowMinMaxButtonsHint );
        }*/

        m_pBtnVideoSettings->setEnabled(false);
        parwdg->setWindowState(parwdg->windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
        showNormal();
        //show();
        setFocus();
        // parwdg->setGeometry(wpos);
    }
    else
    {
        QWidget * parwdg = parentWidget();
        if(0 == parwdg)
            parwdg = this;
        setSizePolicy(m_policyBeforeRecord);
        m_denyClose = false ;
        m_denyMinimize = false ;
        m_denyGeometryChange = false;
        parwdg->setWindowFlags(m_defaultFlags);
        m_pBtnVideoSettings->setEnabled(true);

        setMinimumSize(m_minSizeBeforeRecord);
        setMaximumSize(QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);

        parwdg->setGeometry(m_geom);
        parwdg->setWindowState(parwdg->windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
        showNormal();
        //show();
        setFocus();
    }

    if(sett.recordControls)
    {
        emit signalStartVideoClicked(isChecked, sett.fileDestination, sett.filePrefix, sett.fileFormat, m_mainConteinerWidget);
    }
    else
    {
        emit signalStartVideoClicked( isChecked, sett.fileDestination, sett.filePrefix, sett.fileFormat, this);
    }
}



void sw::SceneWidget::closeEvent(QCloseEvent *e)
{

    if(m_denyClose)
    {
        e->accept();
    }
}


bool sw::SceneWidget::isAllowClose()
{
    return (!m_denyClose);
}

void sw::SceneWidget::slotBtnVideoSettingsClicked()
{
    VideoSettings * m_pVideoSettingsWidget = new VideoSettings(VideoPlayerTypes::ST_VIDEO,0);
    m_pVideoSettingsWidget->setModal(true);
    m_pVideoSettingsWidget->exec();
    delete m_pVideoSettingsWidget;
}

//void sw::SceneWidget::slotShowSearchMenu(bool vis)
//{

//    m_topMenuBar->setVisibleLine(MenuBar::MBL_SEARCH, vis);
//    saveSettings();

//}


void sw::SceneWidget::initNavigation()
{
    m_navigation = new Navigation( this );
    connect(m_navigation, SIGNAL(currentNavigation( int )), this, SLOT(slotSetCurrentNavigation(int)));
    connect(m_navigation, SIGNAL(signalResendUnusedEvents(QEvent*)), this, SLOT( slotResendEventsFromNavigation(QEvent*)) );
    m_navigation->hide();
}

void sw::SceneWidget::initReachZone()
{
    m_reachZone = new ReachZone(this);
    m_reachZone->hide();
    connect(m_reachZone, SIGNAL(signalResendUnusedEvents(QEvent*)), this, SLOT( slotResendEventsFromReachZone(QEvent*)) );
}

void sw::SceneWidget::initCoordBar()
{
}

void sw::SceneWidget::initKompas()
{
}

void sw::SceneWidget::initZoomScroll()
{
}

void sw::SceneWidget::initRowScale()
{
    m_rowScale = new RowScale(this);
    m_pRowScaleControl = new RowScaleControl(this);
    m_pRowScaleControl->setVisible(false);

    //m_rowScale->setVisible(false);
    m_rowScale->switchToScale();
    m_rowScale->setInterpixelDistance(0);
    m_pRowScaleControl->setFocus();
    connect(m_rowScale , SIGNAL(signalShowScaleControl()), m_pRowScaleControl, SLOT( show() ));
    connect(m_pRowScaleControl, SIGNAL(signalSetCurrentScaleMeters(qreal)), this, SLOT ( slotSetCurrentScaleMeters(qreal) ));
    m_rowScale->hide();
}


void sw::SceneWidget::slotSetCurrentScaleMeters(qreal meters)
{

}

CheckBoxActionWidget * sw::SceneWidget::getKompasVisibleCB()
{
    return m_kompasVisibleCB;
}

CheckBoxActionWidget * sw::SceneWidget::getZoomBarVisibleCB()
{
    return m_zoomBarVisibleCB;
}

CheckBoxActionWidget * sw::SceneWidget::getMinimapVisibleCB()
{
    return m_minimapVisibleCB;
}

CheckBoxActionWidget * sw::SceneWidget::getCoordBarVisibleCB()
{
    return m_coordBarVisibleCB;
}

CheckBoxActionWidget * sw::SceneWidget::getScaleBarVisibleCB()
{
    return m_scaleBarVisibleCB;
}

//CheckBoxActionWidget * sw::SceneWidget::getBookmarkVisibleCB()
//{
//    return m_bookmarkVisibleCB;
//}

CheckBoxActionWidget * sw::SceneWidget::getTimebarVisibleCB()
{
    return m_timebarVisisbleCB;
}

void sw::SceneWidget::slotAngleMeasureClicked()
{
    //int a = 5;
}


void sw::SceneWidget::slotDistanceMeasureClicked()
{
    //int a = 5;
}

void sw::SceneWidget::slotReachMeasureClicked()
{
    //int a = 5;
}

void sw::SceneWidget::slotAreaMeasureClicked()
{
    //int a = 5;
}

void sw::SceneWidget::slotRouteMeasureClicked()
{
    //int a = 5;
}

void sw::SceneWidget::slotShowInvisibleAreaClicked()
{
    //int a = 5;
}

void sw::SceneWidget::slotAddBookmark()
{

}

void sw::SceneWidget::slotSetBookmark()
{

}

//void sw::SceneWidget::slotCloseBookmark()
//{
//    m_pBookmarkCollapseWidget->hide();
//    m_bookmarkVisibleCB->setSelected(false);
//}

void sw::SceneWidget::slotMouseReleaseFromZoom()
{

}



void sw::SceneWidget::slotResendEventsFromKompas(QEvent* e)
{
//    if(0 != m_mainViewWidget)
//    {
//        if (e->type() == QEvent::Wheel)
//        {
//            QWheelEvent * wheelE = dynamic_cast<QWheelEvent*>(e);

//            QWheelEvent * newEvent = new QWheelEvent(wheelE->pos() + m_kompas->pos(),
//                                                     wheelE->delta(), wheelE->buttons(), wheelE->modifiers(),
//                                                     wheelE->orientation());
//            QApplication::postEvent(m_mainViewWidget, newEvent);
//        }

//        if( (e->type() == QEvent::KeyRelease) || (e->type() == QEvent::KeyPress) )
//        {
//            QApplication::sendEvent(m_mainViewWidget, e);

//        }
//    }
}


void sw::SceneWidget::slotResendEventsFromZoom(QEvent* e)
{
//    if(0 != m_mainViewWidget)
//    {
//        if (e->type() == QEvent::Wheel)
//        {
//            QWheelEvent * wheelE = dynamic_cast<QWheelEvent*>(e);
//            QWheelEvent * newEvent = new QWheelEvent(wheelE->globalPos() + m_zoomScroll->pos(),
//                                                     wheelE->delta(), wheelE->buttons(), wheelE->modifiers(),
//                                                     wheelE->orientation());
//            QApplication::postEvent(m_mainViewWidget, newEvent);
//        }

//        if( (e->type() == QEvent::KeyRelease) || (e->type() == QEvent::KeyPress) )
//        {
//            QApplication::sendEvent(m_mainViewWidget, e);

//        }
//    }
}


void sw::SceneWidget::slotResendEventsFromScrollWidget(QEvent* e)
{
//    if(0 != m_mainViewWidget)
//    {
//        QEvent::Type t = e->type();
//        if( (e->type() == QEvent::KeyRelease) || (e->type() == QEvent::KeyPress)
//                || (e->type() == QEvent::MouseButtonRelease) || (e->type() == QEvent::MouseButtonPress) || (e->type() == QEvent::Move)
//                || (e->type() == QEvent::Enter)
//                || (e->type() == QEvent::FocusIn)
//                )
//        {
//            QApplication::sendEvent(m_mainViewWidget, e);
//        }


//        if (e->type() == QEvent::Wheel)
//        {
//            QWheelEvent * wheelE = dynamic_cast<QWheelEvent*>(e);

//            QWheelEvent * newEvent = new QWheelEvent(wheelE->pos() + m_pInnerRightScrollArea->pos(),
//                                                     wheelE->delta(), wheelE->buttons(), wheelE->modifiers(),
//                                                     wheelE->orientation());
//            QApplication::postEvent(m_mainViewWidget, newEvent);
//        }
//    }
}



void sw::SceneWidget::slotResendEventsFromNavigation(QEvent* e)
{
//    if(0 != m_mainViewWidget)
//    {
//        if( (e->type() == QEvent::KeyRelease) || (e->type() == QEvent::KeyPress) )
//        {
//            QApplication::sendEvent(m_mainViewWidget, e);
//        }
//    }
}



void sw::SceneWidget::slotResendEventsFromReachZone(QEvent* e)
{
    //    if( (e->type() == QEvent::KeyRelease) || (e->type() == QEvent::KeyPress) )
    //    {
    //        QApplication::sendEvent(m_mainViewWidget, e);
    //    }
//    if (e->type() == QEvent::Wheel)
//    {
//        QWheelEvent * wheelE = dynamic_cast<QWheelEvent*>(e);

//        QWheelEvent * newEvent = new QWheelEvent(wheelE->pos() + m_reachZone->pos(),
//                                                 wheelE->delta(), wheelE->buttons(), wheelE->modifiers(),
//                                                 wheelE->orientation());
//        QApplication::postEvent(m_mainViewWidget, newEvent);
//    }

//    if( (e->type() == QEvent::KeyRelease) || (e->type() == QEvent::KeyPress) )
//    {
//        QApplication::sendEvent(m_mainViewWidget, e);

//    }
}

QWidget * sw::SceneWidget::setMinimapViewWidget(QWidget * minimapViewWidget)
{
    QWidget * wdg = m_minimapContainer->setMiniMap(minimapViewWidget);
    return wdg;
}

void sw::SceneWidget::setMainViewWidget(QWidget * _mainViewWidget, bool isOpenGlContent)
{
    m_mainConteinerWidget->setMainWidget(_mainViewWidget);
    m_mainViewWidget = _mainViewWidget;

//    if(0 != _mainViewWidget && isOpenGlContent)
//    {
//        m_needGrabScreen = isOpenGlContent;

//        setOpennedDataType (ODT_3D);
//        m_zoomScroll->setRollerMovable(false);
//    }
//    else
//    {
//        m_grabMainViewTimer->stop();
//        m_needGrabScreen = false;
//    }

    if(m_needGrabScreen)
    {
        m_grabMainViewTimer->start();
    }

    resize(size());

}


void sw::SceneWidget::slotGrabMainViewWidget()
{
    return;

    if(0 == m_mainViewWidget)
    {
    }

    m_main3dWidget = dynamic_cast<QGLWidget*>(m_mainViewWidget);

    if((!m_needGrabScreen) || (0 == m_main3dWidget ))
    {
        return;
    }

    int mPosX = m_mainConteinerWidget->pos().x();
    int mPosY = m_mainConteinerWidget->pos().y();

    QPixmap kompasPm;
//    if(0 != m_kompas)
//    {
//        if(m_kompas->isVisible() )
//        {
//            kompasPm = QPixmap::fromImage( getScreenFrom3D(m_kompas->pos().x() - mPosX, m_kompas->pos().y() - mPosY, m_kompas->width(),
//                                                           m_kompas->height()));

//            m_kompas->setBackgroundImage(kompasPm);
//        }
//    }

    QPixmap scaleBarPm;
    if(0 != m_rowScale)
    {
        if(m_rowScale->isVisible() )
        {
            scaleBarPm = QPixmap::fromImage( getScreenFrom3D(m_rowScale->pos().x() - mPosX, m_rowScale->pos().y() - mPosY, m_rowScale->width(),
                                                             m_rowScale->height()));
            //            m_rowScale->setBackgroundImage(scaleBarPm);
        }
    }


    QPixmap zoomBarPm ;
//    if(0 != m_zoomScroll)
//    {
//        if(m_zoomScroll->isVisible() )
//        {
//            zoomBarPm = QPixmap::fromImage( getScreenFrom3D(m_zoomScroll->pos().x() - mPosX, m_zoomScroll->pos().y() - mPosY, m_zoomScroll->width(),
//                                                            m_zoomScroll->height()));
//            m_zoomScroll->setBackgroundImage(zoomBarPm);
//        }
//    }

    QPixmap timeBarPm;
    if(0 != m_pTimeBar)
    {
        if(m_pTimeBar->isVisible() )
        {
            timeBarPm = QPixmap::fromImage( getScreenFrom3D(m_pTimeBar->pos().x() - mPosX, m_pTimeBar->pos().y() - mPosY, m_pTimeBar->width(),
                                                            m_pTimeBar->height()));
            //            m_pTimeBar->setBackgroundImage(timeBarPm);
        }

    }

//    QPixmap topMenuBarPm;
//    if(0 != m_topMenuBar)
//    {
//        if(m_topMenuBar->isVisible() )
//        {

//            int topMenuScreeShotX = m_topMenuBar->pos().x() - mPosX;
//            int topMenuScreeShotY = m_topMenuBar->pos().y() - mPosY;
//            int topMenuScreenShowW = m_topMenuBar->width();
//            int topMenuScreenShowH = m_topMenuBar->height();
//            if(topMenuScreeShotX < 0)
//            {
//                topMenuScreenShowW = topMenuScreenShowW - (mPosX - m_topMenuBar->pos().x() );
//            }

//            if(topMenuScreenShowW > m_mainConteinerWidget->width())
//            {
//                topMenuScreenShowW = m_mainConteinerWidget->width() ;
//            }

//            topMenuBarPm = QPixmap::fromImage( getScreenFrom3D(topMenuScreeShotX, topMenuScreeShotY, topMenuScreenShowW, topMenuScreenShowH));
//            m_topMenuBar->setBackgroundImage(topMenuBarPm);
//        }
//    }


    QPixmap coordBarPm;
    if(0 != m_coordBar)
    {
        if(m_coordBar->isVisible() )
        {
            coordBarPm = QPixmap::fromImage( getScreenFrom3D(m_coordBar->pos().x() - mPosX, m_coordBar->pos().y() - mPosY, m_coordBar->width(),
                                                             m_coordBar->height()));
            //            m_coordBar->setBackgroundImage(coordBarPm);
        }
    }


//    QPixmap openButtonPm;
//    if(0 != m_mainConteinerWidget)
//    {
//        if(m_mainConteinerWidget->isVisible())
//        {
//            int openBtnScreenShotX = m_topMenuBar->pos().x() - mPosX;
//            int openBtnScreenShotY = m_topMenuBar->pos().y() - mPosY;
//            int openBtnScreenShotW = m_topMenuBar->width();
//            int openBtnScreenShotH = m_topMenuBar->height();
//            if(openBtnScreenShotX < 0)
//            {
//                openBtnScreenShotW = openBtnScreenShotW - (mPosX - m_topMenuBar->pos().x() );
//            }

//            if(openBtnScreenShotW > m_mainConteinerWidget->width())
//            {
//                openBtnScreenShotW = m_mainConteinerWidget->width() ;
//            }

//            openButtonPm = QPixmap::fromImage( getScreenFrom3D(openBtnScreenShotX, openBtnScreenShotY, openBtnScreenShotW, openBtnScreenShotH));

//            m_openMenuButton->setBackgroundImage(openButtonPm);
//        }
//    }


    if(0 != m_pInnerRightScrollArea)
    {

        if(m_pInnerRightScrollArea->isVisible())
        {
            QPixmap innerRightScrollAreaPm = QPixmap::fromImage( getScreenFrom3D(m_pInnerRightScrollArea->pos().x() - mPosX, m_pInnerRightScrollArea->pos().y() - mPosY, m_pInnerRightScrollArea->width(),
                                                                                 m_pInnerRightScrollArea->height()));

            QSize size = kompasPm.size();
            if(m_pInnerRightScrollArea->isVisible())
                m_pInnerRightScrollArea->setBackgroundImage(innerRightScrollAreaPm);
        }
    }

    m_grabMainViewTimer->start();
}


void sw::SceneWidget::slotEmbendedButtonClicked(int buttonId)
{
    bool isPressed = (m_embendedBoxWidgets->getButtonState(buttonId) == MenuBarButtonBoxItem::BS_PRESSED);

    if(m_embeddedWidgets.contains(buttonId))
    {
        sw::EmbendedWidgetStruct * str = m_embeddedWidgets.value(buttonId);


        m_embendedBoxWidgets->setButtonStateWithoutEmit(str->id,isPressed);
        if(0 != str->chBox)
        {
            str->chBox->setSelected(isPressed);
        }


        str->collapsedWidgetPtr->setVisible(isPressed);

        saveSettings();
    }

    //    qDebug() << "SceneWidget::slotBBW1ButtonClicked() id: " << buttonId;
}


/**
* @brief  Слот вызывает при клике по кнопке: имерение углов между точками на поверхности (пользовательскими объектами).
*/
void sw::SceneWidget::slotBBW1ButtonClicked(int buttonId)
{
    bool isPressed = (m_buttonBoxWidget1->getButtonState(buttonId) == MenuBarButtonBoxItem::BS_PRESSED);
    emit signalButtonClicked(buttonId, isPressed );
        qDebug() << "SceneWidget::slotBBW1ButtonClicked() id: " << buttonId;
}

void sw::SceneWidget::emitSignalButtonClicked(int buttonId, bool isPressed)
{
    emit signalButtonClicked(buttonId, isPressed );
}

void SceneWidget::setMenuBarFixedWidth(int mbfw)
{
      menuBarFixedWidth = mbfw;
}

void SceneWidget::setMenuOpenHeight(int h)
{
    ///todo delete!
//    m_openMenuButton->setHeight(h);
//    m_topMenuBar->setHeight(h);
//    m_topMenuBar->setViewMenuBar();
//    m_openMenuButton->setViewMenuOpenButtonWidget();
}

void sw::SceneWidget::slotBBW2ButtonClicked(int buttonId)
{
    bool isPressed = (m_buttonBoxWidget2->getButtonState(buttonId) == MenuBarButtonBoxItem::BS_PRESSED);
    emit signalButtonClicked(buttonId, isPressed );

    if(sw::BTN_2D_PIX_FORM_RADAR_PORTRAIT == buttonId)
    {
        if(isPressed)
        {
            if(0 < m_pOuterRightScrollArea->insertedWidgetCount())
            {
                m_pOuterRightScrollArea->show();
            }

            if(0 < m_pOuterLeftScrollArea->insertedWidgetCount())
            {
                m_pOuterLeftScrollArea->show();
            }
        }
        else
        {
            m_pOuterRightScrollArea->hide();
            m_pOuterLeftScrollArea->hide();
        }
    }

    //qDebug() << "SceneWidget::slotBBW2ButtonClicked() id: " << buttonId;
}

void sw::SceneWidget::slotBBW3ButtonClicked(int buttonId)
{
    bool isPressed = (m_buttonBoxWidget3->getButtonState(buttonId) == MenuBarButtonBoxItem::BS_PRESSED);
    emit signalButtonClicked(buttonId, isPressed );
    //qDebug() << "SceneWidget::slotBBW2ButtonClicked() id: " << buttonId;
}

void sw::SceneWidget::slotBBW4ButtonClicked(int buttonId)
{
    bool isPressed = (m_buttonBoxWidget4->getButtonState(buttonId) == MenuBarButtonBoxItem::BS_PRESSED);
    emit signalButtonClicked(buttonId, isPressed );
    //qDebug() << "SceneWidget::slotBBW2ButtonClicked() id: " << buttonId;
}

QImage sw::SceneWidget::getScreenFrom3D(int x, int y, int w, int h)
{
//    QTime tmr;
//    tmr.setHMS(0,0,0,0);
//    tmr.start();

//    glFlush();

//    m_main3dWidget->makeCurrent();
//    // QImage res(w,h,QImage::Format_ARGB32_Premultiplied);

//    QImage res(w,h,QImage::Format_RGB888);

//    if(0 == m_main3dWidget)
//    {
//        return res;
//    }


    //glReadPixels(x,m_main3dWidget->height() - y - h ,w,h,GL_BGR, GL_UNSIGNED_BYTE, res.bits());

//    glReadPixels(x,m_main3dWidget->height() - y - h ,w,h,GL_RGB, GL_UNSIGNED_BYTE, res.bits());
    //qDebug() << "Elapsed screen time" <<  tmr.elapsed();

    return QImage(); //res.mirrored();
}


QWidget* sw::SceneWidget::getMainViewWidget()
{
    return m_mainConteinerWidget->getMainViewWidget();
}

void sw::SceneWidget::setKompasVisible(bool visible)
{
//    if(visible)
//    {
//        if(m_opennedDataType == ODT_3D)
//            m_kompas->hide();
//        else
//            m_kompas->show();
//    }
//    else
//        m_kompas->hide();

//    slotUpdateChildsGeometry();
//    saveSettings();
}

int sw::SceneWidget::getCurCoordSystem()
{
    return m_coordBar->getCurCoordSystem();
}

void sw::SceneWidget::setMinimapSize(QSize size)
{
    m_minimapContainer->setFixedSize(size);
}



void sw::SceneWidget::slotOneStepRotate(qreal angle)
{

}


void sw::SceneWidget::slotContinuousRotate(qreal angle)
{
    qDebug() << angle;

}


//void sw::SceneWidget::slotOneStepMove(Kompas::KompasOrientation orient)
//{

//}

void sw::SceneWidget::slotStopMove()
{

}


//void sw::SceneWidget::slotStartContinuousMove(Kompas::KompasOrientation orient)
//{

//}


void sw::SceneWidget::slotBlockToNorth(bool blocked)
{

}



void sw::SceneWidget::slotSetKompasVisible(bool visible)
{
//    if(visible)
//    {
//        if(m_opennedDataType == ODT_3D)
//        {
//            m_kompas->hide();
//        }
//        else
//        {
//            m_kompas->show();
//        }
//    }
//    else
//    {
//        m_kompas->hide();
//    }

//    slotUpdateChildsGeometry();
//    saveSettings();
}


void sw::SceneWidget::slotSetZoomBarVisible(bool visible)
{
//    if(visible)
//    {
//        if(m_opennedDataType == ODT_3D)
//        {
//            m_zoomScroll->hide();
//        }
//        else
//        {
//            m_zoomScroll->show();
//        }
//    }
//    else
//    {
//        m_zoomScroll->hide();
//    }

//    slotUpdateChildsGeometry();
//    saveSettings();
}

void sw::SceneWidget::slotSetMinimapVisible(bool visible)
{

    if(visible)
    {
        m_minimapContainer->show();
    }
    else
    {
        m_minimapContainer->hide();
    }

    slotUpdateChildsGeometry();
    saveSettings();
}

void sw::SceneWidget::slotSetCoordinateBarVisible(bool visible)
{

    if(visible)
        m_coordBar->show();
    else
        m_coordBar->hide();

    slotUpdateChildsGeometry();
    saveSettings();
}

void sw::SceneWidget::slotSetRowScaleVisible(bool visible)
{
    if(visible)
        m_rowScale->show();
    else
        m_rowScale->hide();

    slotUpdateChildsGeometry();
    saveSettings();
}

void sw::SceneWidget::slotGetTimeTimeout()
{
//    emit signalGetModelTime();

//    if(!m_contextPlaying)
//    {
//        slotSetDateTime(QDateTime() , false);
//    }
}

//void sw::SceneWidget::slotSetDateTime(QDateTime tm, bool isPlaying)
//{
//    if(0 != m_pTimeBar)
//    {
//        if(isPlaying)
//        {
//            m_contextPlaying = true;
//            m_pTimeBar->setTime( 3,  tm.time().hour(),  tm.time().minute(),  tm.date().day(),  tm.date().month(), tm.date().year() - 2000);
//        }
//        else
//        {
//            m_contextPlaying = false;
//            tm = QDateTime::currentDateTime();
//            m_pTimeBar->setTime( 3,  tm.time().hour(),  tm.time().minute(),  tm.date().day(),  tm.date().month(), tm.date().year() - 2000);
//        }
//    }
//}

void sw::SceneWidget::slotSetTimeBarVisible(bool visible)
{
    if(visible)
        m_pTimeBar->show();
    else
        m_pTimeBar->hide();

    slotUpdateChildsGeometry();
    saveSettings();
}



//void sw::SceneWidget::slotSetBookmarkVisible(bool visible)
//{
//    if(visible)
//    {
//        if(0 != m_pBtnObjBrowser)
//            m_pBtnObjBrowser->setChecked(true);
//        if(0!= m_bookmarkVisibleCB)
//            m_bookmarkVisibleCB->setSelected(true);

//        if(0 != m_pBookmarkCollapseWidget)
//        {
//            if(!m_pBookmarkCollapseWidget->isVisible())
//                m_pBookmarkCollapseWidget->show();

//        }
//    }
//    else
//    {
//        if(0 != m_pBtnObjBrowser)
//            m_pBtnObjBrowser->setChecked(false);

//        if(0 != m_bookmarkVisibleCB)
//            m_bookmarkVisibleCB->setSelected(false);

//        if(0 != m_pBookmarkCollapseWidget)
//        {
//            if(m_pBookmarkCollapseWidget->isVisible())
//                m_pBookmarkCollapseWidget->hide();
//        }
//    }

//    slotUpdateChildsGeometry();
//    saveSettings();
//}

void sw::SceneWidget::slotBtnPrintClicked()
{
    emit signalButtonClicked(sw::BTN_PRINT,true);
}

//void sw::SceneWidget::slotBtnPropertyObject()
//{
//    emit signalPropertyObject(sw::BTN_PROPERTY,true);
//}


void sw::SceneWidget::slotBtnSaveClicked()
{
    emit signalButtonClicked(sw::BTN_SAVE,true);

}

void sw::SceneWidget::slotCurCoordSystChanged(int curCoordSystem)
{
    m_coordBar->setCurrentCoords(0,0,2,0);
}

void sw::SceneWidget::setDenyRotateKompas(bool denyRotate)
{
//    if(0 != m_kompas)
//    {
//        m_kompas->setDenyRotate(denyRotate);
//    }

}

void sw::SceneWidget::slotSetCurrentNavigation(int id)
{
    qDebug()<<"-->slotSetCurrentNavigation";
}

void sw::SceneWidget::slotButtonToggle(bool)
{
    qDebug()<<"-->slotButtonToggle";
}



void sw::SceneWidget::slotCoordSystem()
{
    //    qDebug()<<"SceneWidget::slotCoordSystem()";
}

void sw::SceneWidget::slotSetOSMSearchCompetePercent(int perc)
{
    QString progr = QString::number(perc) + QString("%");
    m_pWaitWidget->setProgress(progr);
}


void sw::SceneWidget::slotSwitchOffButton(int buttonId)
{
    BBW_BUTTON_ID id = (BBW_BUTTON_ID)(buttonId);

    m_buttonBoxWidget1->setButtonState(buttonId,MenuBarButtonBoxItem::BS_RELEASED);
    m_buttonBoxWidget2->setButtonState(buttonId,MenuBarButtonBoxItem::BS_RELEASED);

    emit buttonSwitchOff((int)buttonId);
}

void sw::SceneWidget::slotOSMSearchResult(QString searchErrorString,
                                          QString renderErrorString,
                                          int objectFoundedCount,
                                          int objectCreatedCount,
                                          bool isSearchFinished){
    QString fullErrString;
    if(!searchErrorString.isEmpty())
        fullErrString = searchErrorString;

    if(!renderErrorString.isEmpty())
        fullErrString += " \n " + renderErrorString ;

    if(fullErrString.isEmpty() && searchErrorString.isEmpty())
    {
        fullErrString = tr("Object created: ")
                + QString::number(objectCreatedCount)
                + "/"
                + QString::number(objectFoundedCount);
    }

    m_pWaitWidget->setInfoText(fullErrString, isSearchFinished);
    m_pWaitCollapsedWidget->adjustSize();
}


void sw::SceneWidget::slotWhatThisClicked()
{
}



void sw::SceneWidget::slotLockCollapsedWidget(bool isLocked)
{

    QMapIterator<quint64 , sw::EmbendedWidgetStruct*> it(m_embeddedWidgets);
    while(it.hasNext())
    {
        it.next();
        sw::EmbendedWidgetStruct * str = it.value();

        if(dynamic_cast<QWidget*>(sender()) == str->collapsedWidgetPtr )
        {
            str->locked = isLocked;

            emit signalEmbWidgetSettingsChanged(str);
        }
    }
}



void sw::SceneWidget::slotPinCollapsedWidget(bool isPinned)
{

    QMapIterator<quint64 , sw::EmbendedWidgetStruct*> it(m_embeddedWidgets);
    while(it.hasNext())
    {
        it.next();
        sw::EmbendedWidgetStruct * str = it.value();

        if(dynamic_cast<QWidget*>(sender()) == str->collapsedWidgetPtr )
        {
            str->allowMoving = !isPinned;
            emit signalEmbWidgetSettingsChanged(str);
        }
    }
}


void sw::SceneWidget::addEmbeddedWidget(sw::EmbendedWidgetStruct str)
{

    setUpdatesEnabled(false);

    sw::EmbendedWidgetStruct * eStruct  = new sw::EmbendedWidgetStruct(str);

    eStruct->id = m_embendedWidgetIdCounter;
    m_embendedWidgetIdCounter++;


    CollapsedWidget * colWdg = 0;
    QString styleSheet = str.embendedWidgetPtr->styleSheet();

    switch (str.alignType)
    {
    case EWA_LEFT_OUTER_BOX:
    {
        bool vis = false;
        vis = m_pOuterLeftScrollArea->isVisible();

        eStruct->collapsedWidgetPtr = new CollapsedWidget(m_pOuterLeftScrollArea);
        colWdg = eStruct->collapsedWidgetPtr;
        m_pOuterLeftScrollArea->addWidgetToSet(eStruct->collapsedWidgetPtr);

        eStruct->collapsedWidgetPtr->setVisisbleCloseButton(eStruct->hasHideButton);
        eStruct->collapsedWidgetPtr->setVisisbleCollapseButton(eStruct->hasCollapseButton);
        eStruct->collapsedWidgetPtr->setVisibleHeader(eStruct->showWindowTitle);
        eStruct->collapsedWidgetPtr->setVisisblePinButton(eStruct->hasPinButton);

        if(eStruct->allowMoving)
        {
            m_pInnerRightScrollArea->addWidgetToSet(colWdg);
            m_pOuterRightScrollArea->addWidgetToSet(colWdg);
        }

        colWdg->insertWidget(eStruct->embendedWidgetPtr);
        m_pOuterLeftScrollArea->appendWidget(colWdg, !eStruct->addHided);

    }break;
    case EWA_RIGHT_OUTER_BOX:
    {

        bool vis = false;
        vis = m_pOuterRightScrollArea->isVisible();


        eStruct->collapsedWidgetPtr = new CollapsedWidget(m_pOuterRightScrollArea);
        colWdg = eStruct->collapsedWidgetPtr;
        m_pOuterRightScrollArea->addWidgetToSet(eStruct->collapsedWidgetPtr);

        eStruct->collapsedWidgetPtr->setVisisbleCloseButton(eStruct->hasHideButton);
        eStruct->collapsedWidgetPtr->setVisisbleCollapseButton(eStruct->hasCollapseButton);
        eStruct->collapsedWidgetPtr->setVisibleHeader(eStruct->showWindowTitle);
        eStruct->collapsedWidgetPtr->setVisisblePinButton(eStruct->hasPinButton);

        if(eStruct->allowMoving)
        {
            m_pInnerRightScrollArea->addWidgetToSet(colWdg);
            m_pOuterLeftScrollArea->addWidgetToSet(colWdg);
        }

        colWdg->insertWidget(eStruct->embendedWidgetPtr);
        m_pOuterRightScrollArea->appendWidget(colWdg, !eStruct->addHided);

    }break;
    case EWA_RIGHT_INNER_BOX:
    {
        bool vis = false;
        vis = m_pInnerRightScrollArea->isVisible();

        eStruct->collapsedWidgetPtr = new CollapsedWidget(m_pInnerRightScrollArea);
        colWdg = eStruct->collapsedWidgetPtr;
        m_pInnerRightScrollArea->addWidgetToSet(eStruct->collapsedWidgetPtr);

        eStruct->collapsedWidgetPtr->setVisisbleCloseButton(eStruct->hasHideButton);
        eStruct->collapsedWidgetPtr->setVisisbleCollapseButton(eStruct->hasCollapseButton);
        eStruct->collapsedWidgetPtr->setVisibleHeader(eStruct->showWindowTitle);
        eStruct->collapsedWidgetPtr->setVisisblePinButton(eStruct->hasPinButton);

        if(eStruct->allowMoving)
        {
            m_pOuterRightScrollArea->addWidgetToSet(colWdg);
            m_pOuterLeftScrollArea->addWidgetToSet(colWdg);
        }

        colWdg->insertWidget(eStruct->embendedWidgetPtr);
        m_pInnerRightScrollArea->appendWidget(colWdg, !eStruct->addHided);

    }break;

    case EWA_PERCENTAGE_ALIGN:
    case EWA_LEFT_BOTTOM_CORNER_OFFSET:
    case EWA_RIGHT_TOP_CORNER_OFFSET:
    case EWA_RIGHT_BOTTOM_CORNER_OFFSET:
    case EWA_LEFT_TOP_CORNER_OFFSET:
    {
        eStruct->collapsedWidgetPtr = new CollapsedWidget(this);
        colWdg = eStruct->collapsedWidgetPtr;

        eStruct->collapsedWidgetPtr->setVisisbleCloseButton(eStruct->hasHideButton);
        eStruct->collapsedWidgetPtr->setVisisbleCollapseButton(eStruct->hasCollapseButton);
        eStruct->collapsedWidgetPtr->setVisibleHeader(eStruct->showWindowTitle);
        eStruct->collapsedWidgetPtr->setVisisblePinButton(eStruct->hasPinButton);
        colWdg->moveOnlyOnParent(true);

        colWdg->insertWidget(eStruct->embendedWidgetPtr);

        if(!eStruct->size.isNull())
        {
            QSize sz(colWdg->getRightMargin() + colWdg->getLeftMargin() + eStruct->size.width()
                     ,colWdg->getTopMargin() + colWdg->getBottomMargin() + eStruct->size.height());

            colWdg->setFixedSize(sz);

        }

        connect(colWdg, SIGNAL(signalPosChanged(quint64, QPoint)),
                this, SLOT(slotColWidgetPosChanged(quint64, QPoint)));

        colWdg->expand();

    }break;

        //eStruct->embendedWidgetPtr->setStyleSheet(styleSheet);



    default:
    {
        qDebug() << "SceneWidget::addEmbeddedWidget(): ERROR! Unsupported align type for embedded widget";
        setUpdatesEnabled(true);
        return;
    }break;
    }

    eStruct->collapsedWidgetPtr->setWindowTitleText(eStruct->windowTitle);
    eStruct->collapsedWidgetPtr->setVisibleHeader(eStruct->showWindowTitle);

    colWdg->setId(eStruct->id);
    colWdg->setDefaultFixedSize(eStruct->size);
    connect(eStruct->embendedWidgetPtr, SIGNAL(destroyed()), this, SLOT(slotEmbeddedWidgetDeleted()));



    if(eStruct->hasVisibilityButton)
    {
        MenuBarButtonBoxItem * it = m_embendedBoxWidgets->addItem(eStruct->id,eStruct->checkedIcon, eStruct->uncheckedIcon,eStruct->tooltip, true,eStruct->tooltip);
        eStruct->m_btn = it;


        //connect(eStruct->m_btn, SIGNAL(sigCheckChange(bool)), this, SLOT(slotEmbendedVisibleChanged(bool)));

    }

    if(eStruct->hasVisibilityCheckbox)
    {
        eStruct->chBox = m_menuListWidget->addItem(eStruct->visibleCbText);
        connect(eStruct->chBox, SIGNAL(sigCheckChange(bool)), this, SLOT(slotEmbendedVisibleChanged(bool)));
    }
    else
    {
        eStruct->chBox =0;
    }


    if(eStruct->addHided)
    {
        if(0 != eStruct->chBox && eStruct->hasVisibilityCheckbox)
        {
            eStruct->chBox->setSelected(false);
        }

        if(0 != eStruct->m_btn && eStruct->hasVisibilityButton)
        {
            int btnid = m_embendedBoxWidgets->getIdByPtr(eStruct->m_btn);
            if(-1 != btnid)
            {
                m_embendedBoxWidgets->setButtonStateWithoutEmit(btnid,MenuBarButtonBoxItem::BS_RELEASED);
            }
        }

        eStruct->collapsedWidgetPtr->hide();
    }
    else
    {
        if(0 != eStruct->chBox && eStruct->hasVisibilityCheckbox)
        {
            eStruct->chBox->setSelected(true);
        }

        if(0 != eStruct->m_btn && eStruct->hasVisibilityButton)
        {
            int btnid = m_embendedBoxWidgets->getIdByPtr(eStruct->m_btn);
            if(-1 != btnid)
            {
                m_embendedBoxWidgets->setButtonStateWithoutEmit(btnid,MenuBarButtonBoxItem::BS_PRESSED);
            }
        }

        eStruct->collapsedWidgetPtr->show();
    }

    //connect(eStruct->embendedWidgetPtr, SIGNAL())

    eStruct->collapsedWidgetPtr->setPinned(!eStruct->allowMoving);
    if(eStruct->hasPinButton)
    {
        connect(eStruct->collapsedWidgetPtr, SIGNAL(signalPinCollapsedWidget(bool)),
                this, SLOT(slotPinCollapsedWidget(bool)));
    }

    eStruct->collapsedWidgetPtr->setVisisbleLockButton(eStruct->hasLockButton);
    if(eStruct->hasLockButton)
    {
        eStruct->collapsedWidgetPtr->setLocked(eStruct->locked);
        connect(eStruct->collapsedWidgetPtr, SIGNAL(signalLockCollapsedWidget(bool))
                , this, SLOT(slotLockCollapsedWidget(bool)));
    }
    eStruct->embendedWidgetPtr->installEventFilter(eStruct->collapsedWidgetPtr);
    m_embeddedWidgets.insert(eStruct->id, eStruct);
    connect(eStruct->collapsedWidgetPtr, SIGNAL(signalVisibleNeedChangeInternal(bool))
            , this, SLOT(slotEmbendedVisibleChangedInternal(bool)));



    reInit();


    //eStruct->collapsedWidgetPtr->setStyleSheet(m_embendedStylesheet);
    //eStruct->embendedWidgetPtr->setStyleSheet(m_embendedStylesheet);

    int mainWidgW = (width() - m_pOuterLeftScrollArea->width()*(int)(m_pOuterLeftScrollArea->isVisible())
                     - m_pOuterRightScrollArea->width()*(int)(m_pOuterRightScrollArea->isVisible()) );
    int mainWidgX = m_pOuterLeftScrollArea->width()*(int)(m_pOuterLeftScrollArea->isVisible());

    switch (eStruct->alignType)
    {
    case EWA_LEFT_TOP_CORNER_OFFSET:     /*привязка виджета относительно левого верхнего угла с фиксированным смещением*/
    {
        //if(eStruct->collapsedWidgetPtr->isPinned() || !eStruct->allowMoving)
        {
            eStruct->collapsedWidgetPtr->move(mainWidgX + eStruct->alignPoint.x(), eStruct->alignPoint.y());
        }
    }break;
    case EWA_LEFT_BOTTOM_CORNER_OFFSET:  /*привязка виджета относительно левого нижнего угла с фиксированным смещением*/
    {
        //if(eStruct->collapsedWidgetPtr->isPinned() || !eStruct->allowMoving)
        {
            QPoint p(mainWidgX + eStruct->alignPoint.x()
                     , height() - eStruct->collapsedWidgetPtr->frameGeometry().height() - eStruct->alignPoint.y() );
            eStruct->collapsedWidgetPtr->move(p);
        }
    }break;
    case EWA_RIGHT_TOP_CORNER_OFFSET:    /*привязка виджета относительно правого верхнего угла с фиксированным смещением*/
    {
        //if(eStruct->collapsedWidgetPtr->isPinned() || !eStruct->allowMoving)
        {


            QPoint p(mainWidgX + mainWidgW - eStruct->collapsedWidgetPtr->frameGeometry().width() - eStruct->alignPoint.x()
                     , eStruct->alignPoint.y() );

            eStruct->collapsedWidgetPtr->move(p);
        }
    }break;
    case EWA_RIGHT_BOTTOM_CORNER_OFFSET: /*привязка виджета относительно правого нижнего угла с фиксированным смещением*/
    {
        //if(eStruct->collapsedWidgetPtr->isPinned() || !eStruct->allowMoving)
        {
            QPoint p(mainWidgX + mainWidgW - eStruct->collapsedWidgetPtr->frameGeometry().width() - eStruct->alignPoint.x()
                     , height() - eStruct->collapsedWidgetPtr->frameGeometry().height() - eStruct->alignPoint.y() );
            eStruct->collapsedWidgetPtr->move(p);
        }
    }break;
    case EWA_PERCENTAGE_ALIGN:            /*привязка виджета с динамическим смещением(процент от ширины/высоты виджета)*/
    {
        //if(eStruct->collapsedWidgetPtr->isPinned() || !eStruct->allowMoving)
        {
            QPoint p((eStruct->alignPoint.x()*width())/100 - eStruct->collapsedWidgetPtr->frameGeometry().width()/2
                     , (eStruct->alignPoint.y()*height())/100 - eStruct->collapsedWidgetPtr->frameGeometry().height()/2 );
            eStruct->collapsedWidgetPtr->move(p);
        }
    }break;
    }

    setUpdatesEnabled(true);
}


void sw::SceneWidget::slotEmbendedVisibleChangedInternal(bool isToogled)
{
    QMapIterator<quint64 , sw::EmbendedWidgetStruct*> it(m_embeddedWidgets);
    while(it.hasNext())
    {
        it.next();
        sw::EmbendedWidgetStruct * str = it.value();

        if(sender() == str->collapsedWidgetPtr )
        {

            m_embendedBoxWidgets->setButtonStateWithoutEmit(str->id,isToogled);
            if(str->chBox)
                str->chBox->setSelected(isToogled);
            if(str->collapsedWidgetPtr)
            {
                str->collapsedWidgetPtr->setVisible(isToogled);
            }

            //str->addHided = !isToogled;

            //emit signalEmbWidgetSettingsChanged(str);
        }
    }

    saveSettings();
}


void sw::SceneWidget::slotEmbendedVisibleChanged(bool isToogled)
{

    QMapIterator<quint64 , sw::EmbendedWidgetStruct*> it(m_embeddedWidgets);
    while(it.hasNext())
    {
        it.next();
        sw::EmbendedWidgetStruct * str = it.value();

        if(sender() == str->m_btn
                || sender() == str->chBox )
        {

            m_embendedBoxWidgets->setButtonStateWithoutEmit(str->id,isToogled);
            if(str->chBox)
                str->chBox->setSelected(isToogled);
            if(str->collapsedWidgetPtr)
                str->collapsedWidgetPtr->setVisible(isToogled);

            str->addHided = !isToogled;

            emit signalEmbWidgetSettingsChanged(str);
        }

    }

    saveSettings();
}

void sw::SceneWidget::slotColWidgetPosChanged(quint64 id, QPoint p)
{
    sw::EmbendedWidgetStruct * eStruct = m_embeddedWidgets.value(id);

    switch (eStruct->alignType)
    {

    case EWA_LEFT_TOP_CORNER_OFFSET:
    {
        if(eStruct->allowMoving && !eStruct->collapsedWidgetPtr->isPinned())
        {
            eStruct->alignPoint = p;
        }
    }break;

    case EWA_LEFT_BOTTOM_CORNER_OFFSET:
    {
        if(eStruct->allowMoving && !eStruct->collapsedWidgetPtr->isPinned())
        {
            eStruct->alignPoint = QPoint( p.x(), (height() - (p.y() + eStruct->collapsedWidgetPtr->frameGeometry().height())  ) );
        }
    }break;

    case EWA_RIGHT_TOP_CORNER_OFFSET:
    {
        if(eStruct->allowMoving && !eStruct->collapsedWidgetPtr->isPinned())
        {
            eStruct->alignPoint = QPoint( (width() - (p.x() + eStruct->collapsedWidgetPtr->frameGeometry().width())), p.y()  );
        }
    }break;

    case EWA_RIGHT_BOTTOM_CORNER_OFFSET:
    {
        if(eStruct->allowMoving && !eStruct->collapsedWidgetPtr->isPinned())
        {
            eStruct->alignPoint = QPoint( (width() - (p.x() + eStruct->collapsedWidgetPtr->frameGeometry().width())), (height() - (p.y() + eStruct->collapsedWidgetPtr->frameGeometry().height())) );
        }
    }break;

    case EWA_PERCENTAGE_ALIGN:
    {
        if(eStruct->allowMoving && !eStruct->collapsedWidgetPtr->isPinned())
        {
            QPoint percPoint(100*((p.x()*1.0 + eStruct->collapsedWidgetPtr->frameGeometry().width()/2.0)/width())
                             , 100*((p.y()*1.0 + eStruct->collapsedWidgetPtr->frameGeometry().height()/2.0)/height()) );
            eStruct->alignPoint = percPoint;
        }
    }break;

    default:
        break;
    }

    emit signalEmbWidgetSettingsChanged(eStruct);

}


void sw::SceneWidget::removeEmbendedWidget(EmbendedWidgetStruct str)
{
    QWidget * wdg = str.embendedWidgetPtr;
    QMap<quint64 , sw::EmbendedWidgetStruct*>::iterator i;
    QList<quint64> ids;
    for (i = m_embeddedWidgets.begin(); i != m_embeddedWidgets.end(); ++i)
    {
        sw::EmbendedWidgetStruct * eStruct = i.value();
        if(wdg == eStruct->embendedWidgetPtr)
        {
            ids.append(i.key());
            eStruct->collapsedWidgetPtr->removeWidget(wdg);

            m_pInnerRightScrollArea->removeWidget(eStruct->collapsedWidgetPtr);
            m_pInnerRightScrollArea->removeWidgetFromSet(eStruct->collapsedWidgetPtr);

            m_pOuterRightScrollArea->removeWidget(eStruct->collapsedWidgetPtr);
            m_pOuterRightScrollArea->removeWidgetFromSet(eStruct->collapsedWidgetPtr);

            m_pOuterLeftScrollArea->removeWidget(eStruct->collapsedWidgetPtr);
            m_pOuterLeftScrollArea->removeWidgetFromSet(eStruct->collapsedWidgetPtr);

            delete eStruct->collapsedWidgetPtr;
            eStruct->collapsedWidgetPtr = 0;
        }
    }

    foreach (quint64 id, ids)
    {
        m_embeddedWidgets.remove(id);
    }
}


void sw::SceneWidget::slotEmbeddedWidgetDeleted()
{
    QWidget * wdg = dynamic_cast<QWidget*>(sender());
    QMap<quint64 , sw::EmbendedWidgetStruct*>::iterator i;
    QList<quint64> ids;
    for (i = m_embeddedWidgets.begin(); i != m_embeddedWidgets.end(); ++i)
    {
        sw::EmbendedWidgetStruct * eStruct = i.value();
        if(wdg == eStruct->embendedWidgetPtr)
        {
            ids.append(i.key());
            eStruct->collapsedWidgetPtr->removeWidget(wdg);
            delete eStruct->collapsedWidgetPtr;
            eStruct->collapsedWidgetPtr = 0;
        }

    }

    foreach (quint64 id, ids)
    {
        m_embeddedWidgets.remove(id);
    }
}



SceneWidget::~SceneWidget()
{

}


MenuButtonItem * sw::SceneWidget::getBtnSave(){return m_pBtnSave;}

//MenuButtonItem * sw::SceneWidget::getBtnPropertyObject(){return m_pBtnPropertyObject;}

//MenuButtonItem * sw::SceneWidget::getBtnShowSearch(){return m_pBtnShowSearch;}
/**
 * @brief Возвращает указатель на ухо верхнего меню
 * @return
 */
//MenuOpenButtonWidget * sw::SceneWidget::getOpenMenuButton(){return m_openMenuButton;}


RowScale * sw::SceneWidget::getRowScale(){return m_rowScale;}
RowScaleControl * sw::SceneWidget::getRowScaleControl(){return m_pRowScaleControl;}


/**
 * @brief Возвращает указатель на виджет панели инструментов
 * @return
 */
//MenuBar * sw::SceneWidget::getMenuBar(){return m_topMenuBar;}

/**
 * @brief Возвращает указатель на виджет управления переключением системы координат
 * @return
 */
CoordBar * sw::SceneWidget::getCoordBar(){return m_coordBar;}

Navigation * sw::SceneWidget::getNavigation(){return m_navigation;}

//Bookmark * sw::SceneWidget::getBookmark(){return m_bookmark;}

ReachZone * sw::SceneWidget::getReachZone(){return m_reachZone;}

ButtonBoxWidget * sw::SceneWidget::getTopBarButtonBoxWidget1(){return m_buttonBoxWidget1;}
ButtonBoxWidget * sw::SceneWidget::getTopBarButtonBoxWidget2(){return m_buttonBoxWidget2;}
ButtonBoxWidget * sw::SceneWidget::getTopBarButtonBoxWidget3(){return m_buttonBoxWidget3;}
ButtonBoxWidget * sw::SceneWidget::getTopBarButtonBoxWidget4(){return m_buttonBoxWidget4;}
ButtonBoxWidget * sw::SceneWidget::getTopBarButtonBoxWidget(){return m_buttonBoxWidget2;}
