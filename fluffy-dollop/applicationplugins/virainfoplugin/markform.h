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
    explicit MarkForm(QWidget *parent = 0);
    ~MarkForm();
    void showWidget(quint64 id);

signals:
    void signalCloseWindow();

protected:
    void closeAndCommit(bool moveToArchive);

private slots:
    void slotLoadImage();
    void slotApply();
    void slotToArchieve();

private:
    Ui::MarkForm *ui;
    quint64 _id;
    PixmapListWidget * _listWidget;
    QList<QPixmap> _pixmaps;
};


#endif // MARKFORM_H
