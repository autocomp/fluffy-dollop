#ifndef WORKSTATE_H
#define WORKSTATE_H

#include <ctrvisual/state/scrollbasestate.h>

class LocationItem;
class AreaGraphicsItem;

class WorkState : public ScrollBaseState
{
    Q_OBJECT

public:
    WorkState();
    ~WorkState();
    virtual QString stateName() {return QString("WorkState");}
    virtual void statePushedToStack() {}
    virtual void statePoppedFromStack() {}
    virtual bool mousePressEvent(QMouseEvent* e, QPointF scenePos);
    virtual void init(QGraphicsScene * scene, QGraphicsView * view, const int * zoom, const double * scale, double frameCoef, uint visualizerId);
    virtual void zoomChanged();

signals:
    void showFacility(qulonglong id);
    void switchOnMap();
    void switchOnEditor();
    void centerEditorOn(qulonglong id);

protected slots:
    void slotDubleClickOnFacility(qulonglong id);
    void slotSetItemselect(qulonglong id, bool on_off);
    void slotSelectionItemsChanged(uint64_t,uint64_t);
    void slotCenterOn(uint64_t);

private:
    QMap<qulonglong, AreaGraphicsItem*> _items;
    QList<LocationItem *> _locationItems;

    QMap<qulonglong, QString> _itemId_facilityFolder;
};



#endif // WORKSTATE_H