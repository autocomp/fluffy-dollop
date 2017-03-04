#ifndef PDFIMPORTERPANEL_H
#define PDFIMPORTERPANEL_H

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

class PdfImporterPanel : public VisualizerControlPanel
{
    Q_OBJECT
public:
    explicit PdfImporterPanel(QWidget *parent = 0);
    void init(uint visualizerId, quint64 visualizerWindowId, QWidget * sceneWidgetPanel);
    void createModeButtonGroup();
    void setMode(int mode);
    
public slots:
    void slotModeChanged();
    void readSettings();

signals:
    void changeMode(int);

private:
    QAction * _modeMenu;
    QMenu * _menu;
    
};

}

#endif // PDFIMPORTERPANEL_H
