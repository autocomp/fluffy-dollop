#include "floorgraphmakerplugin.h"
#include "instrumentalform.h"
#include <ctrcore/ctrcore/ctrconfig.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>
#include <ctrwidgets/components/layersmanager/commontypes.h>
#include <regionbiz/rb_manager.h>
#include <regionbiz/rb_locations.h>
#include <QTimer>
#include <QDebug>

using namespace regionbiz;

FloorGraphMakerPlugin::FloorGraphMakerPlugin()
{
}

FloorGraphMakerPlugin::~FloorGraphMakerPlugin()
{
}

void FloorGraphMakerPlugin::init(uint visualizerId, quint64 visualizerWindowId)
{
//    scale_widget_visibile = CtrConfig::getValueByName("vira_graphic_settings.scale_widget_visibile").toBool();
//    grid_visibile = CtrConfig::getValueByName("vira_graphic_settings.grid_visibile").toBool();

    CtrAppVisualizerPlugin::init(visualizerId, visualizerWindowId);

//    if(scale_widget_visibile || grid_visibile)
//        QTimer::singleShot(3000, this, SLOT(slotCheckVisibleState()));
}

void FloorGraphMakerPlugin::slotCheckVisibleState()
{
//    if(scale_widget_visibile)
//        emit setChecked(QString("FloorGraphMakerPlugin_SCALE_WIDGET"), true);

//    if(grid_visibile)
//        emit setChecked(QString("FloorGraphMakerPlugin_GRID"), true);
}

QList<InitPluginData> FloorGraphMakerPlugin::getInitPluginData()
{
    QList<InitPluginData>list;
    InitPluginData initPluginData;
    initPluginData.buttonName = QString("FloorGraphMakerPlugin");
    initPluginData.translateButtonName = QString::fromUtf8("разметка этажа");
    initPluginData.iconForButtonOn = QIcon("://img/floorgraphstate.png");
    initPluginData.iconForButtonOff = QIcon("://img/floorgraphstate.png");
    initPluginData.tooltip = QString::fromUtf8("разметка этажа");
    initPluginData.isCheckable = true;
    list.append(initPluginData);
    return list;
}

bool FloorGraphMakerPlugin::isChecked(const QString & buttonName)
{
    if(buttonName == QString("FloorGraphMakerPlugin"))
        return _cheched;
    else
        return false;
}

void FloorGraphMakerPlugin::checked(const QString & buttonName, bool on_off)
{
    if(buttonName == QString("FloorGraphMakerPlugin"))
    {
        _cheched = on_off;
        if(_cheched)
        {
            uint64_t floorId(0);
            uint64_t id = regionbiz::RegionBizManager::instance()->getCurrentEntity();
            BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(id);
            while(ptr)
            {
                if(ptr->getType() == BaseArea::AT_FLOOR)
                {
                    floorId = ptr->getId();
                    break;
                }
                ptr = ptr->getParent();
            }

            if(floorId)
            {
                _form = new floor_graph_maker::InstrumentalForm(getVisualizerId(), floorId);
                QString tag("FloorGraphMakerForm");
                quint64 widgetId = ewApp()->restoreWidget(tag, _form);
                if(0 == widgetId)
                {
                    ew::EmbeddedWidgetStruct struc;
                    ew::EmbeddedHeaderStruct headStr;
                    headStr.hasCloseButton = true;
                    headStr.windowTitle = QString::fromUtf8("разметка этажа");
                    headStr.headerPixmap = QString(":/img/floorgraphstate.png");
                    struc.widgetTag = tag;
                    //                struc.minSize = QSize(300,25);
                    struc.maxSize = QSize(500,200);
                    //                struc.size = QSize(400,25);
                    struc.header = headStr;
                    struc.iface = _form;
                    struc.topOnHint = true;
                    struc.isModal = false;
                    widgetId = ewApp()->createWidget(struc); //, viewInterface->getVisualizerWindowId());
                }
                connect(_form, SIGNAL(signalClosed()), this, SLOT(slotInstrumentalFormClose()));
                ewApp()->setVisible(_form->id(), true);
            }
            else
                QTimer::singleShot(10, this, SLOT(slotInstrumentalFormClose()));
        }
        else
        {
            slotInstrumentalFormClose();
        }
        CommonMessageNotifier::send( (uint)visualize_system::BusTags::BlockGUI, QVariant(_cheched), QString("visualize_system"));
    }
}

void FloorGraphMakerPlugin::slotInstrumentalFormClose()
{
    if(_form)
    {
        ewApp()->removeWidget(_form->id());
        _form->deleteLater();
        _form = nullptr;
    }

    emit setChecked(QString("FloorGraphMakerPlugin"), false);
}


































