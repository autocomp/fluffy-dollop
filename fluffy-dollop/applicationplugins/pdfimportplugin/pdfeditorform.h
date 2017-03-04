#ifndef PDFEDITORFORM_H
#define PDFEDITORFORM_H

#include <QWidget>
#include "scenepdfimportwidget.h"
#include "pdfpageslistwidget.h"
#include <QFutureWatcher>
#include <QGraphicsScene>
#include "pdfeditorview.h"

namespace Ui {
class PdfEditorForm;
}

class PdfEditorForm : public QWidget
{
    Q_OBJECT
    
public:
    explicit PdfEditorForm(QString pdfFilePath, QWidget *parent = 0);
    ~PdfEditorForm();

private slots:
    void finishRun();
    void slotSetPage(int pageNumber);

private:
    void run(QString pdfFilePath, QString savePath);
    QString getSavePath();

    Ui::PdfEditorForm *ui;
    ScenePdfImportWidget * _scenePdfImportWidget;
    PdfPagesListWidget * _pdfPagesListWidget;
    QFutureWatcher<void>* _futureWatcher;
    QList<QImage> _images;
    bool _runFinished;
    QGraphicsScene _scene;
    PdfEditorView * _view;
};

#endif // PDFEDITORFORM_H
