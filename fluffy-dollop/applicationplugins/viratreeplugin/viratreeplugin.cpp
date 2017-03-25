#include "viratreeplugin.h"
#include "viratreewidget.h"
#include <libembeddedwidgets/embeddedstruct.h>
#include <libembeddedwidgets/embeddedapp.h>
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
#include <QDebug>


ViraTreePlugin::ViraTreePlugin()
{
}

ViraTreePlugin::~ViraTreePlugin()
{
}

void ViraTreePlugin::init(uint visualizerId, quint64 visualizerWindowId)
{
    CtrAppVisualizerPlugin::init(visualizerId, visualizerWindowId);

    visualize_system::ViewInterface * viewInterface = visualize_system::VisualizerManager::instance()->getViewInterface(visualizerId);
    _viraTreeWidget = new ViraTreeWidget();
    viewInterface->addWidgetToSplitterLeftArea(_viraTreeWidget);
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



































