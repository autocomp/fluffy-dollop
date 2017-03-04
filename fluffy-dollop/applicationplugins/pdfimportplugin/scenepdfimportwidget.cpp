#include "scenepdfimportwidget.h"

ScenePdfImportWidget::ScenePdfImportWidget(QWidget *parent)
    : Scene2DWidget(parent)
{
    getKompasWidget()->hide();
    getRowScale()->hide();
    getMinimapContainerWidget()->hide();
    getCoordBar()->hide();
    getZoomBar()->hide();

    vcp = new visualize_system::PdfImporterPanel(this);
    connect(vcp, SIGNAL(changeMode(int)), this, SIGNAL(changeMode(int)));
    //addToolbarWidget(vcp);

    connect(getZoomBar(), SIGNAL(zoomChanged(int)), this, SIGNAL(changeZoom(int)));
    connect(this, SIGNAL(newSize(int,int)), this, SLOT(sceneWidgetSizeChanged(int,int)));

    vcp->movePanel(width(), height());
}

ScenePdfImportWidget::~ScenePdfImportWidget()
{
    delete vcp;
}

void ScenePdfImportWidget::addButtonToButtonPanel(QToolButton *pBtn)
{
    QAction *pAction = vcp->addItem(pBtn->text(), pBtn->text(), pBtn->icon(), pBtn->toolTip(), pBtn->isCheckable());
    if(pBtn->isCheckable())
        connect(pAction, SIGNAL(triggered(bool)), pBtn, SIGNAL(clicked(bool)));
    else
        connect(pAction, SIGNAL(triggered()), pBtn, SIGNAL(clicked()));
}

void ScenePdfImportWidget::createModeButtonGroup()
{
    vcp->createModeButtonGroup();
}

void ScenePdfImportWidget::slotChangeMode(int mode)
{
    vcp->setMode(mode);
}

void ScenePdfImportWidget::readSettings()
{
    vcp->readSettings();
//    getOpenMenuButton()->setHeight(vcp->getItemSize().height() + 4);
//    getOpenMenuButton()->setViewMenuOpenButtonWidget();
//    getMenuBar()->setHeight(vcp->getItemSize().height() + 4);
//    getMenuBar()->setWidth(vcp->getPanelWidth());

    //    slotUpdateChildsGeometry();
}

void ScenePdfImportWidget::sceneWidgetSizeChanged(int newW, int newH)
{
    if(vcp)
        vcp->movePanel(newW - 16, newH - 16);/// -16 - Учёт толщены скроллбаров на сценвиджете
}
