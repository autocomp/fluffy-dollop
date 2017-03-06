#ifndef VIRAPANEL_H
#define VIRAPANEL_H

#include <ctrvisual/components/visualizercontrolpanel.h>

#include <QWidget>
#include <QBoxLayout>
#include <QAction>
#include <QSplitter>
#include <QToolBar>
#include <QToolButton>
#include <QPushButton>
#include <QWidgetAction>

namespace visualize_system
{

class ViraPanel : public VisualizerControlPanel
{
    Q_OBJECT
public:
    explicit ViraPanel(QWidget *parent = 0);
    void init(uint visualizerId, quint64 visualizerWindowId, QWidget * sceneWidgetPanel);
    void createModeButtonGroup();
    void setMode(int mode);
    
public slots:
    void slotModeChanged();
    void readSettings();

signals:
    void changeMode(int);
    void switchOnMap();

private:
    QAction * _modeMenu;
    QMenu * _menu;
    
};

}

#endif // PDFIMPORTERPANEL_H
