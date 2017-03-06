#ifndef SCENEVIRAWIDGET_H
#define SCENEVIRAWIDGET_H

#include <ctrvisual/scenecontrol/scene2dwidget.h>
#include <ctrvisual/components/visualizercontrolpanel.h>
#include "virapanel.h"

using namespace sw;

class SceneViraWidget : public Scene2DWidget
{
    Q_OBJECT
public:
    explicit SceneViraWidget(QWidget *parent = 0);
    ~SceneViraWidget();
    void addButtonToButtonPanel(QToolButton *pBtn);
    void createModeButtonGroup();
    visualize_system::ViraPanel *vcp = nullptr;

public slots:
    void slotChangeMode(int mode);
    void readSettings();
    void sceneWidgetSizeChanged(int newW, int newH);

signals:
    void changeZoom(int z);
    void changeMode(int);
    void switchOnMap();
    
};

#endif // SceneViraWidget_H
