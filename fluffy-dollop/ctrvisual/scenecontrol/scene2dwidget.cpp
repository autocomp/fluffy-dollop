#include "scene2dwidget.h"
#include <QSizePolicy>


using namespace sw;


Scene2DWidget::Scene2DWidget(QWidget *parent) :
    sw::SceneWidget(parent)
{
    m_loadSettingsComplete = false;
    menuBarFixedWidth = 530;
    setMinimapSize(QSize(200,200));

//    QTranslator *trans = new QTranslator(this);
//    if(trans->load("./translate/" + QString("libscenewidget.qm")))
//        QCoreApplication::installTranslator(trans);

    //m_bookmark->setBookmarkWidgetType(Bookmark::BWT_2D);
    reInit();

    m_visualizer_control_fake_plugin = new VisualizerWidgetsFakePlugin(this);
    m_measurenemt_fake_plugin = new MeasureFakePlugin(this);

}

void Scene2DWidget::slotScaleChanged(int scale, int zoomMax, int zoomMin)
{
//    getZoomBar()->setMaxZoom(zoomMax);
//    getZoomBar()->setMinZoom(zoomMin);
//    getZoomBar()->setZoomValue(scale);
}

void Scene2DWidget::slotSetInterpixelDistance(qreal mInPixel)
{
    getRowScale()->setInterpixelDistance(mInPixel);
}

void Scene2DWidget::keyPressEvent(QKeyEvent *e)
{
    int a = 5;
}

void Scene2DWidget::setOpennedDataType(OpennedDataType dt)
{
    m_opennedDataType = dt;
    reInit();
}

void Scene2DWidget::slotSetBarCoord(qreal lat, qreal longt, int prec, double height)
{
    getCoordBar()->setCurrentCoords(lat, longt, prec, height);
}

void Scene2DWidget::slotShowDistance(bool visible, double dist)
{
    getRowScale()->setDistanceValue(dist);

    if(visible)
    {
        getRowScale()->switchToMeasureDistance();
    }
    else
    {
        getRowScale()->switchToScale();
    }

}

void Scene2DWidget::slotSetCurrentScaleMeters(qreal meters)
{
    int rowScaleLen = m_rowScale->getScalePxLenght();

    qreal interpixelDistance = 1.0 / ((qreal)rowScaleLen / meters);
    emit signalSetCurrentInterpixelDistance(interpixelDistance);
    m_rowScale->setUserSet(meters);
}

void Scene2DWidget::reInit()
{
    m_loadSettingsComplete = false;
//    m_pBtnSave->setIcon(QIcon(":/273_capture_window_on.png"));
//    m_pBtnSave->setToolTip(QString(tr("Save screenshot")));
//    if(0 != m_pBtnPrint)
//        m_pBtnPrint->hide();

//    if(0 != m_pBtnSave)
//        m_pBtnSave->hide();

//    if(0 != m_pBtnRecordVideo)
//        m_pBtnRecordVideo->hide();

//    if(0 != m_pBtnReloadClicked)
//        m_pBtnReloadClicked->hide();


    if(0 == m_embeddedWidgets.count())
    {
        m_embendedBoxWidgets->reinit(1);
    }
    else
    {
        int counter = 0;
        foreach (sw::EmbendedWidgetStruct* str, m_embeddedWidgets)
        {
            if(str->hasVisibilityButton)
            {
                counter++;
            }
        }

        m_embendedBoxWidgets->reinit(counter);
    }

    delete m_pTimeBar;
    m_pTimeBar = 0;

    m_menuListWidget->clear();
    m_menuListWidget->setFixedWidth(200);
    m_menuListWidget->setMinimumWidth(180);
    m_kompasVisibleCB = m_menuListWidget->addItem(QString(tr("show kompas")));
    m_kompasVisibleCB->setSelected(true);
    connect(m_kompasVisibleCB, SIGNAL(sigCheckChange(bool)), this, SLOT(slotSetKompasVisible(bool)));

    m_zoomBarVisibleCB = m_menuListWidget->addItem(QString(tr("show zoom bar")));
    m_zoomBarVisibleCB->setSelected(true);
    connect(m_zoomBarVisibleCB, SIGNAL(sigCheckChange(bool)), this, SLOT(slotSetZoomBarVisible(bool)));

    m_minimapVisibleCB  = m_menuListWidget->addItem(QString(tr("show minimap")));
    m_minimapVisibleCB->setSelected(false);
    connect(m_minimapVisibleCB, SIGNAL(sigCheckChange(bool)), this, SLOT(slotSetMinimapVisible(bool)));

    m_coordBarVisibleCB = m_menuListWidget->addItem(QString(tr("show coordinate bar")));
    m_coordBarVisibleCB ->setSelected(true);
    connect(m_coordBarVisibleCB , SIGNAL(sigCheckChange(bool)), this, SLOT(slotSetCoordinateBarVisible(bool)));

    m_timebarVisisbleCB = 0;
//    m_timebarVisisbleCB = m_menuListWidget->addItem(QString(tr("show timebar")));
//    m_timebarVisisbleCB->setSelected(true);
//    connect(m_timebarVisisbleCB, SIGNAL(sigCheckChange(bool)), this, SLOT(slotSetTimeBarVisible(bool)));

    QMapIterator<quint64 , sw::EmbendedWidgetStruct*> it(m_embeddedWidgets);
    while(it.hasNext())
    {
        it.next();
        sw::EmbendedWidgetStruct * str = it.value();
        if(str->hasVisibilityButton)
        {

            MenuBarButtonBoxItem * it = m_embendedBoxWidgets->addItem(str->id,str->checkedIcon, str->uncheckedIcon,str->tooltip, true,str->tooltip);
            str->m_btn = it;
        }

        if(str->hasVisibilityCheckbox)
        {
            str->chBox = m_menuListWidget->addItem(str->visibleCbText);
            connect(str->chBox, SIGNAL(sigCheckChange(bool)), this, SLOT(slotEmbendedVisibleChanged(bool)));
        }

        if(str->addHided)
        {
            if(0 != str->chBox && str->hasVisibilityCheckbox)
            {
                str->chBox->setSelected(false);
            }

            if(0 != str->m_btn && str->hasVisibilityButton)
            {
                int btnid = m_embendedBoxWidgets->getIdByPtr(str->m_btn);
                if(-1 != btnid)
                {
                    m_embendedBoxWidgets->setButtonStateWithoutEmit(btnid,MenuBarButtonBoxItem::BS_RELEASED);
                }
            }

            str->collapsedWidgetPtr->hide();
        }
        else
        {
            if(0 != str->chBox && str->hasVisibilityCheckbox)
            {
                str->chBox->setSelected(true);
            }

            if(0 != str->m_btn && str->hasVisibilityButton)
            {
                int btnid = m_embendedBoxWidgets->getIdByPtr(str->m_btn);
                if(-1 != btnid)
                {
                    m_embendedBoxWidgets->setButtonStateWithoutEmit(btnid,MenuBarButtonBoxItem::BS_PRESSED);
                }
            }

            str->collapsedWidgetPtr->show();
        }

        m_embeddedWidgets.insert(str->id, str);
    }

    switch (m_opennedDataType) {
    case ODT_GEO_PICTURE:
        setDenyRotateKompas(false);
        break;
    case ODT_SIMLE_PICTURE:
        setDenyRotateKompas(false);
        break;
    default:
        break;
    }

    m_scaleBarVisibleCB = m_menuListWidget->addItem(QString(tr("show scale bar")));
    m_scaleBarVisibleCB->setSelected(false);
    connect(m_scaleBarVisibleCB, SIGNAL(sigCheckChange(bool)), this, SLOT(slotSetRowScaleVisible(bool)));

    ButtonBoxWidget * bbw1 = getTopBarButtonBoxWidget1();
    bbw1->reinit(5);
    connect(bbw1, SIGNAL(signalButtonClicked(int)), this, SLOT(slotBBW1ButtonClicked(int)) );

    MenuBarButtonBoxItem * tmpBtn  = m_buttonBoxWidget1->addItem((int)sw::BTN_CALC_ANGLES, QIcon(":/69_angle_measure(2d).png"),
                                                                 QIcon(":/117_angle_measure(2d)_off.png"),
                                                                 QString(tr("Measure angle")), true,QString(tr("Measure angle")));
    connect(tmpBtn, SIGNAL(clicked()), this, SLOT(slotAngleMeasureClicked()) );

//    tmpBtn = m_buttonBoxWidget1->addItem((int)sw::BTN_DETECT_LINE_RESOLUTION, QIcon(":/170_linear_resolution_measure_on.png"),
//                                         QIcon(":/182_linear_resolution_measure_off.png"),
//                                         QString(tr("Analize line resolution")), true,QString(tr("Analize line resolution")));

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

    if(ODT_GEO_SCENE == m_opennedDataType || ODT_GEO_PICTURE == m_opennedDataType )
    {
        tmpBtn = m_buttonBoxWidget1->addItem((int)sw::BTN_CALC_ROUTE, QIcon(":/70_routes_measure.png"),
                                             QIcon(":/119_routes_measure_off.png"), QString(tr("Measure route"))
                                             , true,QString(tr("Measure route")));
        connect(tmpBtn, SIGNAL(clicked()), this, SLOT(slotRouteMeasureClicked()) );
    }



    m_menuListWidget->hide();
    m_buttonBoxWidget1->hide();



    /** Доп кнопки */
    ButtonBoxWidget * bbw2 = getTopBarButtonBoxWidget2();
    connect(bbw2, SIGNAL(signalButtonClicked(int)), this, SLOT(slotBBW2ButtonClicked(int)) );
//    tmpBtn  = bbw2->addItem((int)sw::BTN_CALC_ANGLES, QIcon(":/69_angle_measure(2d).png"),
//                                                                 QIcon(":/117_angle_measure(2d)_off.png"),
//                                                                 QString(tr("Measure angle")), true,QString(tr("Measure angle")));
//    connect(tmpBtn, SIGNAL(clicked()), this, SLOT(slotAngleMeasureClicked()) );

//    _toolBarWidget = new QWidget(m_topMenuBar);
//    _toolBarLayout = new QHBoxLayout(_toolBarWidget);
//    _toolBarLayout->setMargin(0);
//    m_topMenuBar->insertItem(_toolBarWidget);//, 2);

    /**** Доп кнопки */


    switch (m_opennedDataType) {
    case ODT_GEO_PICTURE:
        bbw2->reinit(1);
        break;
    case ODT_SIMLE_PICTURE:
        bbw2->reinit(1);
        bbw2->hide();
        break;

    case ODT_3D:
    case ODT_GEO_SCENE:
        bbw2->reinit(2);
        bbw2->hide();
        break;

    default:
        break;
    }

    if(ODT_GEO_PICTURE == m_opennedDataType )
    {
        bbw2->addItem((int)sw::BTN_VI_DETECT_INTERESTING_ZONE ,QIcon(":/101_zi_mark_on.png"),
                      QIcon(":/102_zi_mark_off.png"),QString(tr("Interesting zone")),true,QString(tr("Set interesting zone")));

        //        bbw2->addItem((int)sw::BTN_2D_PIX_FORM_RADAR_PORTRAIT ,QIcon(":/390_cut.png"),
        //                      QIcon(":/390_cut.png"),QString(tr("Radar portrait")),true,QString(tr("Show radar portrait widgets")));
    }

    if(ODT_SIMLE_PICTURE == m_opennedDataType )
    {
        //        bbw2->addItem((int)sw::BTN_2D_PIX_FORM_RADAR_PORTRAIT ,QIcon(":/390_cut.png"),
        //                      QIcon(":/390_cut.png"),QString(tr("Radar portrait")),true,QString(tr("Show radar portrait widgets")));

        m_buttonBoxWidget1->setDisactive((int)sw::BTN_CALC_ANGLES,true);
        m_buttonBoxWidget1->setDisactive((int)sw::BTN_CALC_LENGHT,true);
        m_buttonBoxWidget1->setDisactive((int)sw::BTN_CALC_REACH,true);
        m_buttonBoxWidget1->setDisactive((int)sw::BTN_CALC_SQUARE,true);
        m_buttonBoxWidget1->setDisactive((int)sw::BTN_CALC_ROUTE,true);
    }

    //m_topMenuBar->setFixedWidth(menuBarFixedWidth);
    //setFixedWidth(700);

    ///loadSettings();

    m_loadSettingsComplete = true;
}

//void Scene2DWidget::addToolbarWidget(QWidget * widget)
//{
//    m_topMenuBar->addItem(widget);
//   // widget->layout()->setContentsMargins(0,0,0,0);
//    /*
//    widget->setParent(_toolBarWidget);
//    _toolBarLayout->addWidget(widget);*/
//}

void Scene2DWidget::slotBBW1ButtonClicked(int buttonId)
{
    //qDebug() << "Scene2DWidget::slotBBW1ButtonClicked() id: " << buttonId;

}

void Scene2DWidget::slotBBW2ButtonClicked(int buttonId)
{
    //qDebug() << "Scene2DWidget::slotBBW2ButtonClicked() id: " << buttonId;

    if((int)sw::BTN_VIS_OBJ_BROWSER == buttonId)
    {
        //        if(MenuBarButtonBoxItem::BS_PRESSED == getTopBarButtonBoxWidget2()->getButtonState((int)sw::BTN_VIS_OBJ_BROWSER))
        //            slotSetBookmarkVisible(true);
        //        else
        //            slotSetBookmarkVisible(false);
    }
}

void Scene2DWidget::slotBtnSaveClicked()
{
    emit saveViewportImage();
}

void Scene2DWidget::slotSetCurrentCoords(QString lattitude, QString longtitude, QString heightMeters)
{
    getCoordBar()->setCurrentCoords(lattitude, longtitude, heightMeters);
}

CtrAppVisualizerPlugin* Scene2DWidget::getMeasurePlugin()
{
    return m_measurenemt_fake_plugin;
}

CtrAppVisualizerPlugin* Scene2DWidget::getControlsWidgetsPlugin()
{
    return m_visualizer_control_fake_plugin;
}

