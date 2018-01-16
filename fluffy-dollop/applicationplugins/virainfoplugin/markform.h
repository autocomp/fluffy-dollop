#ifndef MARKFORM_H
#define MARKFORM_H

#include <QWidget>
#include <QListWidget>
#include <QPixmap>
#include "pixmaplistwidget.h"
#include <regionbiz/rb_manager.h>
#include <libpanini/pvQtView.h>

namespace Ui {
class MarkForm;
}

class MarkFormListWidget;

class MarkForm : public QWidget
{
    Q_OBJECT

public:
    explicit MarkForm(QWidget *parent = 0);
    ~MarkForm();
    void showWidget(quint64 id);
    void showWidgetAndCreateMark(regionbiz::Mark::MarkType markType, quint64 parentId, QPolygonF markArea, double direction = 0);

signals:
    void signalCloseWindow();

protected:
    void actulize();
    void closeAndCommit(bool moveToArchive);

private slots:
    void slotLoadImage();
    void slotApply();
    void slotToArchieve();
    void slotReload();

private:
    Ui::MarkForm *ui;
    regionbiz::Mark::MarkType _markType;
    quint64 _id, _parentId;
    QPolygonF _markArea;
    double _direction;
    PixmapListWidget * _listWidget;
    QList<QPixmap> _pixmaps;
    QString _foto360FilePath;
    pvQtView* _photo360Viewer = nullptr;
};


#endif // MARKFORM_H
