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
    explicit ViraEditorForm(qulonglong facilityId, QWidget *parent = 0);
    ~ViraEditorForm();
    void reinit(qulonglong facilityId);
    void setParentWindowId(qulonglong parentWindowId);

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
    qulonglong _parentWindowId;
};

#endif // ViraEditorForm_H
