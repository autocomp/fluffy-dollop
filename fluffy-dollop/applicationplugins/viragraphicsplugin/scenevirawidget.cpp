#include "scenevirawidget.h"

SceneViraWidget::SceneViraWidget(QWidget *parent)
    : Scene2DWidget(parent)
{
//    getKompasWidget()->hide();
//    getRowScale()->hide();
//    getMinimapContainerWidget()->hide();
//    getCoordBar()->hide();
//    getZoomBar()->hide();

    vcp = new visualize_system::ViraPanel(this);
    connect(vcp, SIGNAL(changeMode(int)), this, SIGNAL(changeMode(int)));
    connect(vcp, SIGNAL(switchOnMap()), this, SIGNAL(switchOnMap()));
    //addToolbarWidget(vcp);

//    connect(getZoomBar(), SIGNAL(zoomChanged(int)), this, SIGNAL(changeZoom(int)));
    connect(this, SIGNAL(newSize(int,int)), this, SLOT(sceneWidgetSizeChanged(int,int)));

    vcp->movePanel(width(), height());
}

SceneViraWidget::~SceneViraWidget()
{
    delete vcp;
}

void SceneViraWidget::addButtonToButtonPanel(QToolButton *pBtn)
{
    QAction *pAction = vcp->addItem(pBtn->text(), pBtn->text(), pBtn->icon(), pBtn->toolTip(), pBtn->isCheckable());
    if(pBtn->isCheckable())
        connect(pAction, SIGNAL(triggered(bool)), pBtn, SIGNAL(clicked(bool)));
    else
        connect(pAction, SIGNAL(triggered()), pBtn, SIGNAL(clicked()));
}

void SceneViraWidget::createModeButtonGroup()
{
    vcp->createModeButtonGroup();
}

void SceneViraWidget::slotChangeMode(int mode)
{
    vcp->setMode(mode);
}

void SceneViraWidget::readSettings()
{
    vcp->readSettings();
//    getOpenMenuButton()->setHeight(vcp->getItemSize().height() + 4);
//    getOpenMenuButton()->setViewMenuOpenButtonWidget();
//    getMenuBar()->setHeight(vcp->getItemSize().height() + 4);
//    getMenuBar()->setWidth(vcp->getPanelWidth());

    //    slotUpdateChildsGeometry();
}

void SceneViraWidget::sceneWidgetSizeChanged(int newW, int newH)
{
    if(vcp)
        vcp->movePanel(newW - 16, newH - 16);/// -16 - Учёт толщены скроллбаров на сценвиджете
}
