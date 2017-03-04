#ifndef MEASUREFAKEPLUGIN_H
#define MEASUREFAKEPLUGIN_H

#include <ctrcore/plugin/ctrappvisualizerplugin.h>

#include "scene2dwidget.h"

class MeasureFakePlugin : public CtrAppVisualizerPlugin
{
  Q_OBJECT

  QMap <QString, bool> m_button_to_checked;

  sw::SceneWidget* m_sw;
  sw::BBW_BUTTON_ID m_bbw_button_id;

  QString m_last_button_name;

  bool m_button_last_state;

public:
  MeasureFakePlugin(sw::SceneWidget* sw);

  QList<InitPluginData> getInitPluginData();
  bool isChecked(const QString & buttonName);
  void checked(const QString & buttonName, bool on_off);

private slots:
  void slotButtonSwitchOff(int); // int - sw::BBW_BUTTON_ID
};

#endif // MEASUREFAKEPLUGIN_H
