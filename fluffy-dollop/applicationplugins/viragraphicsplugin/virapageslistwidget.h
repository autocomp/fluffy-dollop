#ifndef VIRAPAGESLISTWIDGET_H
#define VIRAPAGESLISTWIDGET_H

#include <QListWidget>
#include "types.h"

class ViraPagesListWidget : public QListWidget
{
    Q_OBJECT
public:
    ViraPagesListWidget();
    void reinit(qulonglong facilityId);
    void selectionItemsChanged(uint64_t,uint64_t);

signals:
    void setFloor(qulonglong floorId);

protected:
    virtual void resizeEvent(QResizeEvent * e);

private slots:
    void slotSelectionChanged();
    void slotBlockGUI(QVariant var);

private:
    QSize _iconSize;
    double _coef;
    QMap<qulonglong, QListWidgetItem*> _items;
    bool _block = false;
};

#endif // ViraPagesListWidget_H
