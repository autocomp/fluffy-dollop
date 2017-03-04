#include "measurefakeplugin.h"
#include <ctrcore/visual/visualizertype.h>

MeasureFakePlugin::MeasureFakePlugin(sw::SceneWidget* sw)
{
  m_sw = sw;

  connect(m_sw,
          SIGNAL(buttonSwitchOff(int)),
          SLOT(slotButtonSwitchOff(int)));

  m_button_last_state = false;
}

QList<InitPluginData> MeasureFakePlugin::getInitPluginData()
{
  QList<InitPluginData> init_data_list;

  InitPluginData initData;
    initData.buttonName = QString("MeasureFakePlugin_linear");
    initData.translateButtonName = tr("Distance");
    initData.iconForButtonOn = QIcon(":/img/002_icons_42_size_on.png");
    initData.iconForButtonOff = QIcon(":/img/002_icons_42_size_off.png");
    initData.isCheckable = true;
    initData.tooltip = tr("Distance");

    init_data_list << initData;
    m_button_to_checked.insert(initData.buttonName, false);

    initData = InitPluginData();
    initData.buttonName = QString("MeasureFakePlugin_route");
    initData.translateButtonName = tr("Route");
    initData.iconForButtonOn = QIcon(":/img/005_icons_42_way_on.png");
    initData.iconForButtonOff = QIcon(":/img/005_icons_42_way_off.png");
    initData.isCheckable = true;
    initData.tooltip = tr("Route");

    init_data_list << initData;
    m_button_to_checked.insert(initData.buttonName, false);

    initData = InitPluginData();
    initData.buttonName = QString("MeasureFakePlugin_angle");
    initData.translateButtonName = tr("Angle");
    initData.iconForButtonOn = QIcon(":/img/001_icons_42_angl_on.png");
    initData.iconForButtonOff = QIcon(":/img/001_icons_42_angl_off.png");
    initData.isCheckable = true;
    initData.tooltip = tr("Angle");

    init_data_list << initData;
    m_button_to_checked.insert(initData.buttonName, false);

    initData = InitPluginData();
    initData.buttonName = QString("MeasureFakePlugin_square");
    initData.translateButtonName = tr("Area");
    initData.iconForButtonOn = QIcon(":/img/004_icons_42_ft_on.png");
    initData.iconForButtonOff = QIcon(":/img/004_icons_42_ft_off.png");
    initData.isCheckable = true;
    initData.tooltip = tr("Area");

    init_data_list << initData;
    m_button_to_checked.insert(initData.buttonName, false);

    initData = InitPluginData();
    initData.buttonName = QString("MeasureFakePlugin_radius");
    initData.translateButtonName = tr("Radius");
    initData.iconForButtonOn = QIcon(":/img/003_icons_42_select_on.png");
    initData.iconForButtonOff = QIcon(":/img/003_icons_42_select_off.png");
    initData.isCheckable = true;
    initData.tooltip = tr("Radius");

    init_data_list << initData;
    m_button_to_checked.insert(initData.buttonName, false);

    return init_data_list;
}

bool MeasureFakePlugin::isChecked(const QString & buttonName)
{
  return m_button_to_checked.value(buttonName, false);
}

void MeasureFakePlugin::checked(const QString & buttonName, bool on_off)
{
  m_button_to_checked.insert(buttonName, on_off);

  if(!m_last_button_name.isEmpty())
    {
      if(buttonName != m_last_button_name)
        {
          if(m_button_last_state)
            {
              m_sw->emitSignalButtonClicked(m_bbw_button_id,
                                            !m_button_last_state);
            }
        }
    }

  if(buttonName == "MeasureFakePlugin_linear")
    {
      m_last_button_name = buttonName;
      m_bbw_button_id = sw::BTN_CALC_LENGHT;
      m_button_last_state = on_off;
    }
  else if(buttonName == "MeasureFakePlugin_route")
    {
      m_last_button_name = buttonName;
      m_bbw_button_id = sw::BTN_CALC_ROUTE;
      m_button_last_state = on_off;
    }
  else if(buttonName == "MeasureFakePlugin_angle")
    {
      m_last_button_name = buttonName;
      m_bbw_button_id = sw::BTN_CALC_ANGLES;
      m_button_last_state = on_off;
    }
  else if(buttonName == "MeasureFakePlugin_square")
    {
      m_last_button_name = buttonName;
      m_bbw_button_id = sw::BTN_CALC_SQUARE;
      m_button_last_state = on_off;
    }
  else if(buttonName == "MeasureFakePlugin_radius")
    {
      m_last_button_name = buttonName;
      m_bbw_button_id = sw::BTN_CALC_REACH;
      m_button_last_state = on_off;
    }

  m_sw->emitSignalButtonClicked(m_bbw_button_id,
                                m_button_last_state);
}

void MeasureFakePlugin::slotButtonSwitchOff(int button_id) // int - sw::BBW_BUTTON_ID
{
  QString buttonName;
  switch (button_id) {
    case sw::BTN_CALC_LENGHT:
      buttonName = "MeasureFakePlugin_linear";
      break;
    case sw::BTN_CALC_ROUTE:
      buttonName = "MeasureFakePlugin_route";
      break;
    case sw::BTN_CALC_ANGLES:
      buttonName = "MeasureFakePlugin_angle";
      break;
    case sw::BTN_CALC_SQUARE:
      buttonName = "MeasureFakePlugin_square";
      break;
    case sw::BTN_CALC_REACH:
      buttonName = "MeasureFakePlugin_radius";
      break;
    default:
      break;
    }

  m_button_last_state = false;
  m_last_button_name = "";

  m_button_to_checked.insert(buttonName, false);

  emit setChecked(buttonName, false);
}
