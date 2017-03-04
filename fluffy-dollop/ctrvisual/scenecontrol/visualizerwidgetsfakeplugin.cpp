#include "visualizerwidgetsfakeplugin.h"
#include <ctrcore/visual/visualizertype.h>

VisualizerWidgetsFakePlugin::VisualizerWidgetsFakePlugin(sw::SceneWidget* sw)
{
    m_sw = sw;
}

QList<InitPluginData> VisualizerWidgetsFakePlugin::getInitPluginData()
{
    QList<InitPluginData> init_data_list;


    InitPluginData initData;
    initData.buttonName = QString("VisualizerWidgetsPlugin_kompas");
    initData.translateButtonName = QString::fromUtf8("Компас");
    initData.iconForButtonOn = QIcon(":/img/013_icons_32_compas.png");
    initData.iconForButtonOff = QIcon(":/img/013_icons_32_compas.png");
    initData.isCheckable = true;
    initData.tooltip = QString::fromUtf8("Компас");

    init_data_list << initData;
    if(!m_button_to_checked.contains(initData.buttonName))
    m_button_to_checked.insert(initData.buttonName, false);

    initData = InitPluginData();
    initData.buttonName = QString("VisualizerWidgetsPlugin_zoom");
    initData.translateButtonName = QString::fromUtf8("Масштабная линейка");
    initData.iconForButtonOn = QIcon(":/img/020_icons_42_zoom_on.png");
    initData.iconForButtonOff = QIcon(":/img/020_icons_42_zoom_on.png");
    initData.isCheckable = true;
    initData.tooltip = QString::fromUtf8("Масштабная линейка");

    init_data_list << initData;
    if(!m_button_to_checked.contains(initData.buttonName))
    m_button_to_checked.insert(initData.buttonName, false);

    initData = InitPluginData();
    initData.buttonName = QString("VisualizerWidgetsPlugin_coords");
    initData.translateButtonName = QString::fromUtf8("Координаты");
    initData.iconForButtonOn = QIcon(":/img/007_icons_32_geo.png");
    initData.iconForButtonOff = QIcon(":/img/007_icons_32_geo.png");
    initData.isCheckable = true;
    initData.tooltip = QString::fromUtf8("Координаты");

    init_data_list << initData;
    if(!m_button_to_checked.contains(initData.buttonName))
    m_button_to_checked.insert(initData.buttonName, false);

    initData = InitPluginData();
    initData.buttonName = QString("VisualizerWidgetsPlugin_scale");
    initData.translateButtonName = QString::fromUtf8("Масштаб");
    initData.iconForButtonOn = QIcon(":/img/021_icons_42_mashtab_on.png");
    initData.iconForButtonOff = QIcon(":/img/021_icons_42_mashtab_on.png");
    initData.isCheckable = true;
    initData.tooltip = QString::fromUtf8("Масштаб");

    init_data_list << initData;
    if(!m_button_to_checked.contains(initData.buttonName))
    m_button_to_checked.insert(initData.buttonName, false);

    initData = InitPluginData();
    initData.buttonName = QString("VisualizerWidgetsPlugin_minimap");
    initData.translateButtonName = QString::fromUtf8("Миникарта");
    initData.iconForButtonOn = QIcon(":/img/023_icons_42_minimap_on.png");
    initData.iconForButtonOff = QIcon(":/img/023_icons_42_minimap_on.png");
    initData.isCheckable = true;
    initData.tooltip = QString::fromUtf8("Миникарта");

    init_data_list << initData;
    if(!m_button_to_checked.contains(initData.buttonName))
    m_button_to_checked.insert(initData.buttonName, false);

    return init_data_list;
}

bool VisualizerWidgetsFakePlugin::isChecked(const QString & buttonName)
{
  return m_button_to_checked.value(buttonName, false);
}

void VisualizerWidgetsFakePlugin::checked(const QString & buttonName, bool on_off)
{
//  m_button_to_checked.insert(buttonName, on_off);

//  if(buttonName == "VisualizerWidgetsPlugin_kompas")
//    {
//      m_sw->getKompasWidget()->setVisible(on_off);
//    }
//  else if(buttonName == "VisualizerWidgetsPlugin_zoom")
//    {
//      m_sw->getZoomBar()->setVisible(on_off);
//    }
//  else if(buttonName == "VisualizerWidgetsPlugin_coords")
//    {
//      m_sw->getCoordBar()->setVisible(on_off);
//    }
//  else if(buttonName == "VisualizerWidgetsPlugin_scale")
//    {
//      m_sw->getRowScale()->setVisible(on_off);
//    }
//  else if(buttonName == "VisualizerWidgetsPlugin_minimap")
//    {
//      m_sw->getMinimapContainerWidget()->setVisible(on_off);
//    }
}
