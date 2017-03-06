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

protected:
    const int ID = Qt::UserRole + 1;
    QMap<qulonglong, QTreeWidgetItem*> _items;
};

#endif // VIRATREEWIDGET_H
