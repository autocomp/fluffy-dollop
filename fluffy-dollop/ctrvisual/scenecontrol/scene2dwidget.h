#ifndef SCENE2DWIDGET_H
#define SCENE2DWIDGET_H

#include "../scenewidget/scenewidget.h"
#include "minimapviewer.h"
#include "viewercontroller.h"


#include <ctrcore/plugin/ctrappvisualizerplugin.h>

#include "visualizerwidgetsfakeplugin.h"
#include "measurefakeplugin.h"

namespace sw {

class Scene2DWidget : public sw::SceneWidget
{
    Q_OBJECT

  CtrAppVisualizerPlugin* m_visualizer_control_fake_plugin;
  CtrAppVisualizerPlugin* m_measurenemt_fake_plugin;

public:
    enum SceneMeasureMode
    {
        SMM_NO_MODE = 0,
        SMM_ANGLES,
        SMM_LENGTH,
        SMM_SQUARE
    };

    explicit Scene2DWidget(QWidget *parent = 0);
    virtual void reInit();
    virtual void setOpennedDataType(OpennedDataType dt);
    //void addToolbarWidget(QWidget * widget);

    CtrAppVisualizerPlugin* getMeasurePlugin();
    CtrAppVisualizerPlugin* getControlsWidgetsPlugin();

protected:
    void keyPressEvent(QKeyEvent *e);
signals:
    void saveViewportImage();
    void signalSwitchMeasureMode(SceneMeasureMode smm);
    
public slots:
    void slotScaleChanged(int scale, int zoomMax, int zoomMin);
    void slotShowDistance(bool visible, double dist);
    void slotSetInterpixelDistance(qreal mInPixel);
    void slotSetBarCoord(qreal , qreal, int, double);
    void slotSetCurrentCoords(QString lattitude, QString longtitude, QString heightMeters);
    void slotBBW1ButtonClicked(int buttonId);
    void slotBBW2ButtonClicked(int buttonId);
    virtual void slotBtnSaveClicked();
    
    virtual void slotSetCurrentScaleMeters(qreal meters);

private:
    //QWidget * _toolBarWidget;
    QHBoxLayout * _toolBarLayout;
};

}

#endif // SCENE2DWIDGET_H
