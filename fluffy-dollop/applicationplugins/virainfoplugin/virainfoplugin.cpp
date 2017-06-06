#include "virainfoplugin.h"
#include "virastatusbar.h"

#include <ctrcore/visual/viewinterface.h>
#include <ctrcore/visual/datainterface.h>
#include <ctrcore/visual/visualizermanager.h>

#include <ctrwidgets/components/slidewindow/slide_window.h>
#include <ctrwidgets/components/layersmanager/layersmanager.h>

#include "filter_form.h"

void ViraInfoPlugin::init(uint visualizerId, quint64 visualizerWindowId)
{
    // init
    CtrAppVisualizerPlugin::init( visualizerId, visualizerWindowId );

    // add to visual
    visualize_system::ViewInterface* viewInterface =
            visualize_system::VisualizerManager::instance()->getViewInterface(visualizerId);
    _viraStatusBar = new ViraStatusBar( visualizerWindowId );
    viewInterface->addWidgetToStatusBar( _viraStatusBar );

    // add panel to visual
    SlideWindow* sw = SlideWindow::mainInstance();
    sw->setHolder( viewInterface->widget() );
    sw->initGeometry();

    // init widget of filter
    FilterForm& ff = FilterForm::instance();
    ff.setParentId( visualizerWindowId );
    ff.registrateToSlideWindow();

    _layersManager = new LayersManager;
}

ViraInfoPlugin::~ViraInfoPlugin()
{
    delete _viraStatusBar;
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
{}



































