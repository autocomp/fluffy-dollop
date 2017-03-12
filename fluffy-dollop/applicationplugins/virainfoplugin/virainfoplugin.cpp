#include "virainfoplugin.h"
#include "virastatusbar.h"
#include <libembeddedwidgets/embeddedstruct.h>
#include <libembeddedwidgets/embeddedapp.h>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <ctrcore/visual/viewinterface.h>
#include <ctrcore/visual/datainterface.h>
#include <ctrcore/visual/visualizermanager.h>
#include <ctrcore/visual/abstractscenewidget.h>
#include <ctrcore/provider/dataproviderfactory.h>
#include <ctrcore/provider/abstractdataprovider.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <QStackedWidget>
#include <QDebug>


ViraInfoPlugin::ViraInfoPlugin()
{
}

ViraInfoPlugin::~ViraInfoPlugin()
{
}

void ViraInfoPlugin::init(uint visualizerId, quint64 visualizerWindowId)
{
    CtrAppVisualizerPlugin::init(visualizerId, visualizerWindowId);

    visualize_system::ViewInterface * viewInterface = visualize_system::VisualizerManager::instance()->getViewInterface(visualizerId);
    _viraStatusBar = new ViraStatusBar(visualizerWindowId);
    viewInterface->addWidgetToStatusBar(_viraStatusBar);
}

QList<InitPluginData> ViraInfoPlugin::getInitPluginData()
{
    return QList<InitPluginData>();
}

bool ViraInfoPlugin::isChecked(const QString&)
{
    return false;
}

void ViraInfoPlugin::checked(const QString &/*buttonName*/, bool /*on_off*/)
{
}



































