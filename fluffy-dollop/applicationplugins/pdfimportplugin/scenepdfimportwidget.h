#ifndef SCENEPDFIMPORTWIDGET_H
#define SCENEPDFIMPORTWIDGET_H

#include <ctrvisual/scenecontrol/scene2dwidget.h>
#include <ctrvisual/components/visualizercontrolpanel.h>
#include "pdfimporterpanel.h"

using namespace sw;

class ScenePdfImportWidget : public Scene2DWidget
{
    Q_OBJECT
public:
    explicit ScenePdfImportWidget(QWidget *parent = 0);
    ~ScenePdfImportWidget();
    void addButtonToButtonPanel(QToolButton *pBtn);
    void createModeButtonGroup();
    visualize_system::PdfImporterPanel *vcp = nullptr;

public slots:
    void slotChangeMode(int mode);
    void readSettings();
    void sceneWidgetSizeChanged(int newW, int newH);

signals:
    void changeZoom(int z);
    void changeMode(int);
    
};

#endif // SCENEPDFIMPORTWIDGET_H
