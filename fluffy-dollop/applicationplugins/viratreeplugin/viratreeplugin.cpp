#include "viratreeplugin.h"
#include "viratreewidget.h"
#include <dmanager/embeddedstruct.h>
#include <dmanager/embeddedapp.h>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <ctrcore/visual/viewinterface.h>
#include <ctrcore/visual/datainterface.h>
#include <ctrcore/visual/visualizermanager.h>
#include <ctrcore/visual/abstractscenewidget.h>
#include <ctrcore/provider/dataproviderfactory.h>
#include <ctrcore/provider/abstractdataprovider.h>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <QStackedWidget>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDebug>

ViraTreePlugin::ViraTreePlugin()
{
}

ViraTreePlugin::~ViraTreePlugin()
{
    delete _wdg;
}

void ViraTreePlugin::init(uint visualizerId, quint64 visualizerWindowId)
{
    CtrAppVisualizerPlugin::init(visualizerId, visualizerWindowId);

    bool layerEditMode(false);
    QVariant layerEditModeVar = CtrConfig::getValueByName("application_settings.editMode");
    if(layerEditModeVar.isValid())
        layerEditMode = layerEditModeVar.toBool();
    else
        CtrConfig::setValueByName("application_settings.editMode", false);

    _wdg = new QWidget;

    QToolButton * addEntityButton = new QToolButton(_wdg);
    addEntityButton->setIcon(QIcon(":/img/icon_add.png"));
    addEntityButton->setFixedSize(32,32);
    addEntityButton->setStyleSheet("border-radius:4;border-color: rgb(255, 255, 255);");
    addEntityButton->setVisible(layerEditMode);

    QToolButton * deleteEntityButton = new QToolButton(_wdg);
    deleteEntityButton->setIcon(QIcon(":/img/icon_delete.png"));
    deleteEntityButton->setFixedSize(32,32);
    deleteEntityButton->setStyleSheet("border-radius:4;border-color: rgb(255, 255, 255);");
    deleteEntityButton->setVisible(layerEditMode);
    deleteEntityButton->setDisabled(true);

    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->addWidget(addEntityButton);
    hLayout->addWidget(deleteEntityButton);
    hLayout->addSpacerItem(new QSpacerItem(0,0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    hLayout->setContentsMargins(3,3,3,3);
    hLayout->setSpacing(3);

    ViraTreeWidget * viraTreeWidget = new ViraTreeWidget(addEntityButton, deleteEntityButton, _wdg);
    viraTreeWidget->reinit();

    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->addLayout(hLayout);
    vLayout->addWidget(viraTreeWidget);
    vLayout->setContentsMargins(3,3,3,3);
    vLayout->setSpacing(3);

    _wdg->setLayout(vLayout);

    visualize_system::ViewInterface * viewInterface = visualize_system::VisualizerManager::instance()->getViewInterface(visualizerId);
    viewInterface->addWidgetToSplitterLeftArea(_wdg);
}


QList<InitPluginData> ViraTreePlugin::getInitPluginData()
{
    return QList<InitPluginData>();
}

bool ViraTreePlugin::isChecked(const QString&)
{
    return false;
}

void ViraTreePlugin::checked(const QString &/*buttonName*/, bool /*on_off*/)
{
}



































