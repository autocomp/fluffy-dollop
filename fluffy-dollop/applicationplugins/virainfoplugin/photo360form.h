#ifndef PHOTO360FORM_H
#define PHOTO360FORM_H

#include <QWidget>
#include <regionbiz/rb_manager.h>
#include <libpanini/pvQtView.h>

namespace Ui {
class Photo360Form;
}

class Photo360Form : public QWidget
{
    Q_OBJECT

public:
    explicit Photo360Form();
    ~Photo360Form();
    void showWidget(quint64 id);

signals:
    void signalCloseWindow();

private slots:
    void slotCloseWindow();
    void slotFileLoaded(regionbiz::BaseFileKeeperPtr imagePtr);
    void slotReload();
    void slotProjChanged(QString);

private:
    Ui::Photo360Form *ui;
    QString _filePath, _fileKey, _markName;
    pvQtView* _photo360Viewer;
};

#endif
