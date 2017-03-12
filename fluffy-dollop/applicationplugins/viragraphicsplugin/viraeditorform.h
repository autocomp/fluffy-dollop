#ifndef VIRAEDITORFORM_H
#define VIRAEDITORFORM_H

#include "scenevirawidget.h"
#include "virapageslistwidget.h"
#include "viraeditorview.h"
#include <QWidget>
#include <QFutureWatcher>
#include <QGraphicsScene>

namespace Ui {
class ViraEditorForm;
}

class ViraEditorForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit ViraEditorForm(QWidget *parent = 0);
    ~ViraEditorForm();
    void setParentWindowId(qulonglong parentWindowId);

private slots:
    void slotSelectionItemsChanged(uint64_t,uint64_t);
    void centerEditorOn(qulonglong id);

signals:
    void switchOnMap();

private slots:
//    void finishRun();
    void slotSetFloor(qulonglong);

private:
//    void run(QString pdfFilePath, QString savePath);
//    QString getSavePath();

    Ui::ViraEditorForm *ui;
    SceneViraWidget * _sceneViraWidget;
    ViraPagesListWidget * _viraPagesListWidget;
    QFutureWatcher<void>* _futureWatcher;
    QGraphicsScene _scene;
    ViraEditorView * _view;
    qulonglong _parentWindowId = 0;
    qulonglong _currFacilityId = 0;
};

#endif // ViraEditorForm_H
