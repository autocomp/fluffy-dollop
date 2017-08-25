#ifndef WORKSTATE_H
#define WORKSTATE_H

#include <ctrvisual/state/scrollbasestate.h>
#include <regionbiz/rb_manager.h>
#include "areagraphicsitem.h"

class LocationItem;
class AreaGraphicsItem;
class ViraGraphicsItem;

class WorkState : public ScrollBaseState
{
    Q_OBJECT

public:
    WorkState();
    ~WorkState();
    virtual QString stateName() {return QString("WorkState");}
    virtual void statePushedToStack();
    virtual void statePoppedFromStack();
    virtual bool mousePressEvent(QMouseEvent* e, QPointF scenePos);
    virtual bool mouseDoubleClickEvent(QMouseEvent *e, QPointF scenePos);
    virtual void init(QGraphicsScene * scene, QGraphicsView * view, const int * zoom, const double * scale, double frameCoef, uint visualizerId);
    virtual void zoomChanged();
    void areaGeometryEdited(QPolygonF);

signals:
    void showFacility(qulonglong id);
    void switchOnMap();
    void switchOnEditor();
    void centerEditorOn(qulonglong id);
    void setMarkOnMap(qulonglong markType);
    void editAreaGeometry(bool on_off);

protected:
    void centerViewOn(QPointF pos);
    bool markInArchive(regionbiz::MarkPtr markPtr);
    void insertItemToLayer(regionbiz::LayerPtr ptr, qulonglong itemId);


protected slots:
    void slotSetItemselect(qulonglong id, bool on_off);
    void slotSelectionItemsChanged(uint64_t,uint64_t);
    void slotCenterOn(uint64_t);
    void slotSelectItem(qulonglong id, bool centerOnEntity);
    void slotBlockGUI(QVariant var);
    void slotSetMarkPosition(QVariant var);
    void reinit();
    void slotObjectChanged(uint64_t id);
    void slotAddObject(uint64_t id);
    void slotDeleteObject(uint64_t id);
    void slotSetLayerVisible(QVariant var);
    void slotEditAreaGeometry(QVariant var);

private:
    void getAreaInitData(AreaInitData & regionInitData, AreaInitData & locationInitData, AreaInitData & facInitData);
    int getPrefferZoomForSceneRect(QRectF rect);

    QMap<qulonglong, ViraGraphicsItem*> _items;
    QMap<uint64_t, QList<qulonglong> > _itemsInLayers;
    QMap<uint64_t, LocationItem *> _locationItems;
    uint64_t _editObjectGeometry = 0;
    QMap<qulonglong, QString> _itemId_facilityFolder;
    qulonglong _prevSelectedFacilityId = 0, _prevSelectedLocationId = 0;
    //uint64_t _editObjectGeometry = 0;
    bool _blockGUI = false;
};



#endif // WORKSTATE_H
