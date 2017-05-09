#ifndef MARKFORM_H
#define MARKFORM_H

#include <QWidget>
#include <QListWidget>
#include <QPixmap>
#include "pixmaplistwidget.h"

namespace Ui {
class MarkForm;
}

class MarkFormListWidget;

class MarkForm : public QWidget
{
    Q_OBJECT

public:
    enum MarkType{Defect, Foto, Foto360};

    explicit MarkForm(QWidget *parent = 0);
    ~MarkForm();
    void showWidget(quint64 id);
    void showWidgetAndCreateMark(MarkType markType, quint64 parentId, QPolygonF markArea, double direction = 0);

signals:
    void signalCloseWindow();

protected:
    void actulize();
    void closeAndCommit(bool moveToArchive);

private slots:
    void slotLoadImage();
    void slotApply();
    void slotToArchieve();

private:
    Ui::MarkForm *ui;
    MarkType _markType;
    quint64 _id, _parentId;
    QPolygonF _markArea;
    double _direction;
    PixmapListWidget * _listWidget;
    QList<QPixmap> _pixmaps;
};


#endif // MARKFORM_H
