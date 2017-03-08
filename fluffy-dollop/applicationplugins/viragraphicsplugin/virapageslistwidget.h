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

signals:
    void setFloor(qulonglong floorId);

protected:
    virtual void resizeEvent(QResizeEvent * e);

private slots:
    void slotSelectionChanged();
    void slotSelectionItemsChanged(uint64_t,uint64_t);

private:
    QSize _iconSize;
    double _coef;
    QMap<qulonglong, QListWidgetItem*> _items;
    bool _block = false;
};

#endif // ViraPagesListWidget_H
