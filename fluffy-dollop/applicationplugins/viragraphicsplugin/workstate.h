#ifndef WORKSTATE_H
#define WORKSTATE_H

#include <ctrvisual/state/scrollbasestate.h>

class LocationItem;
class AreaGraphicsItem;

class WorkState : public ScrollBaseState
{
    Q_OBJECT

public:
    WorkState(const QString& xmlFilePath);
    ~WorkState();
    virtual QString stateName() {return QString("WorkState");}
    virtual void statePushedToStack() {}
    virtual void statePoppedFromStack() {}
    virtual void init(QGraphicsScene * scene, QGraphicsView * view, const int * zoom, const double * scale, double frameCoef, uint visualizerId);
    virtual void zoomChanged();

signals:
    void showFacility(qulonglong id);//const QString& xmlFilePath);

protected slots:
    void slotDubleClickOnFacility(qulonglong id);
    void slotSetItemselect(qulonglong id, bool on_off);
    void slotSelectionItemsChanged(uint64_t,uint64_t);

private:
    QString _xmlFilePath;
    QMap<qulonglong, AreaGraphicsItem*> _items;
    QList<LocationItem *> _locationItems;

    QMap<qulonglong, QString> _itemId_facilityFolder;
};



#endif // WORKSTATE_H
