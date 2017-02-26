#ifndef VISUALIZERWIDGETSFAKEPLUGIN_H
#define VISUALIZERWIDGETSFAKEPLUGIN_H

#include <ctrcore/plugin/ctrappvisualizerplugin.h>

#include "scene2dwidget.h"

class VisualizerWidgetsFakePlugin : public CtrAppVisualizerPlugin
{
  QMap <QString, bool> m_button_to_checked;

  sw::SceneWidget* m_sw;

public:
  VisualizerWidgetsFakePlugin(sw::SceneWidget* sw);
  QList<InitPluginData> getInitPluginData();
  bool isChecked(const QString & buttonName);
  void checked(const QString & buttonName, bool on_off);

};

#endif // VISUALIZERWIDGETSFAKEPLUGIN_H
