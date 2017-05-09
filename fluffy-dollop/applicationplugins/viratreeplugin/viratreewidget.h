#ifndef VIRATREEWIDGET_H
#define VIRATREEWIDGET_H

#include <regionbiz/rb_entity_filter.h>
#include <QObject>
#include <QTreeWidget>
#include <QMap>

class ViraTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    ViraTreeWidget(QWidget *parent = Q_NULLPTR);

public slots:
    void reinit();

protected slots:
    void slotItemSelectionChanged();
    void slotItemDoubleClicked(QTreeWidgetItem*,int);
    void slotObjectSelectionChanged(uint64_t prev_id, uint64_t curr_id);
    void slotObjectChanged(uint64_t id);
    void slotHeaderSectionClicked(int index);
    void slotBlockGUI(QVariant var);
    void slotEditModeFinish(QVariant var);
    void slotResaclArea(const QModelIndex &index);
    void slotSaveItemToDb(const QModelIndex &index);

protected:
    const int ColumnCount = 4; //8;

    void recalcAreaInFacility(QTreeWidgetItem * facilityItem);
    void recalcTasksInFacility(QTreeWidgetItem * facilityItem);
    void recalcTasksInLocation(QTreeWidgetItem * locationItem);
    bool markIsDefect(regionbiz::MarkPtr markPtr);

    QMap<qulonglong, QTreeWidgetItem*> _items;
    QColor _defaultColor = QColor(Qt::white);
    QColor _noCoordColor = QColor(Qt::yellow);
};

#endif // VIRATREEWIDGET_H
