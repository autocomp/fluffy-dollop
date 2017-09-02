#include "pixelvisbuttonsplugin.h"
#include <ctrcore/ctrcore/ctrconfig.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>
#include <ctrwidgets/components/layersmanager/commontypes.h>
#include <QDebug>

PixelVisButtonsPlugin::PixelVisButtonsPlugin()
{
}

PixelVisButtonsPlugin::~PixelVisButtonsPlugin()
{
}

void PixelVisButtonsPlugin::init(uint visualizerId, quint64 visualizerWindowId)
{
    CtrAppVisualizerPlugin::init(visualizerId, visualizerWindowId);
}

QList<InitPluginData> PixelVisButtonsPlugin::getInitPluginData()
{
    QList<InitPluginData>list;

    InitPluginData initPluginData_1;
    initPluginData_1.buttonName = QString("PixelVisButtonsPlugin_SCALE_WIDGET");
    initPluginData_1.translateButtonName = QString::fromUtf8("масштабная линейка");
    initPluginData_1.iconForButtonOn = QIcon("://img/tools_metr_icon.png");
    initPluginData_1.iconForButtonOff = QIcon("://img/tools_metr_icon.png");
    initPluginData_1.tooltip = QString::fromUtf8("масштабная линейка");
    initPluginData_1.isCheckable = true;
    list.append(initPluginData_1);

    InitPluginData initPluginData_2;
    initPluginData_2.buttonName = QString("PixelVisButtonsPlugin_GRID");
    initPluginData_2.translateButtonName = QString::fromUtf8("масштабная сетка");
    initPluginData_2.iconForButtonOn = QIcon("://img/grid_icon.png");
    initPluginData_2.iconForButtonOff = QIcon("://img/grid_icon.png");
    initPluginData_2.tooltip = QString::fromUtf8("масштабная сетка");
    initPluginData_2.isCheckable = true;
    list.append(initPluginData_2);

    return list;
}

bool PixelVisButtonsPlugin::isChecked(const QString & buttonName)
{
    if(buttonName == QString("PixelVisButtonsPlugin_SCALE_WIDGET"))
        return scaleWidgetButtonCheched;
    else if(buttonName == QString("PixelVisButtonsPlugin_GRID"))
        return gridButtonCheched;
    else
        return false;
}

void PixelVisButtonsPlugin::checked(const QString & buttonName, bool on_off)
{
    if(buttonName == QString("PixelVisButtonsPlugin_SCALE_WIDGET"))
    {
        scaleWidgetButtonCheched = on_off;
        QList<QVariant> list;
        list << (int)plugin_types::SCALE_WIDGET << scaleWidgetButtonCheched;
        CommonMessageNotifier::send( (uint)visualize_system::BusTags::ToolButtonInPluginChecked, list, QString("visualize_system"));
    }
    else if(buttonName == QString("PixelVisButtonsPlugin_GRID"))
    {
        gridButtonCheched = on_off;
        QList<QVariant> list;
        list << (int)plugin_types::GRID << gridButtonCheched;
        CommonMessageNotifier::send( (uint)visualize_system::BusTags::ToolButtonInPluginChecked, list, QString("visualize_system"));
    }
}



































