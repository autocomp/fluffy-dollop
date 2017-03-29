#ifndef VIRATREEWIDGET_H
#define VIRATREEWIDGET_H

#include <QObject>
#include <QTreeWidget>
#include <QMap>

class ViraTreeWidget : public QTreeWidget
{
    Q_OBJECT
public:
    ViraTreeWidget(QWidget *parent = Q_NULLPTR);

protected slots:
    void slotItemSelectionChanged();
    void slotItemDoubleClicked(QTreeWidgetItem*,int);
    void slotObjectSelectionChanged(uint64_t prev_id, uint64_t curr_id);
    void slotHeaderSectionClicked(int index);
    void slotBlockGUI(QVariant var);
    void slotEditModeFinish(QVariant var);
    void slotResaclArea(const QModelIndex &index);
    void slotSaveItemToDb(const QModelIndex &index);
    void slotUpdateMark(QVariant var);

protected:
    void recalcAreaInFacility(QTreeWidgetItem * facilityItem);

    QMap<qulonglong, QTreeWidgetItem*> _items;
    QColor _defaultColor = QColor(Qt::white);
    QColor _noCoordColor = QColor(Qt::yellow);
};

#endif // VIRATREEWIDGET_H
