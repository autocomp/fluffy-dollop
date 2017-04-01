#ifndef INFOFORM_H
#define INFOFORM_H

#include <QWidget>
#include <regionbiz/rb_manager.h>
#include "pixmaplistwidget.h"

class QTreeWidgetItem;

namespace Ui {
class InfoForm;
}

class InfoForm : public QWidget
{
    Q_OBJECT

public:
    explicit InfoForm(QWidget *parent = 0);
    ~InfoForm();

    void showWidget(quint64 id);

signals:
    void signalCloseWindow();
    void signalShowMarkInfoWidget(quint64 id);

private slots:
    void slotLoadImage();
    void slotApply();
    void slotHeaderSectionClicked(int index);
    void slotDoubleClickOnMark(QTreeWidgetItem*,int);

private:
    QString recursiveGetName(regionbiz::BaseAreaPtr area);
    void getFloorFullName(QString &facility, regionbiz::BaseAreaPtr area);
    void getRoomFullName(QString &facility, QString &floor, regionbiz::BaseAreaPtr area);
    void reloadTasks(regionbiz::BaseAreaPtr area);
    void loadTasks(const QString &facilityName, const QString &floorName, regionbiz::BaseAreaPtr area);
    void loadTasks(const QString &facilityName, const QString &floorName, const QString &roomName, regionbiz::MarkPtr mark);


    Ui::InfoForm *ui;
    quint64 _id = 0;
    uint _tasks_new, _tasks_in_work, _tasks_for_check;
    PixmapListWidget * _listWidget;
    QList<QPixmap> _pixmaps;

};

#endif // INFOFORM_H
