#ifndef PIXELWORKSTATE_H
#define PIXELWORKSTATE_H

#include <ctrvisual/state/scrollbasestate.h>
#include <ctrvisual/scenewidget/rowscale.h>
#include <regionbiz/rb_manager.h>

class LocationItem;
class AreaGraphicsItem;
class ViraGraphicsItem;
class CompasWidget;
class QLabel;

class PixelWorkState : public ScrollBaseState
{
    Q_OBJECT

public:
    PixelWorkState(uint pixelVisualizerId);
    ~PixelWorkState();
    virtual QString stateName() {return QString("PixelWorkState");}
    virtual void statePushedToStack();
    virtual void statePoppedFromStack();
    virtual bool mousePressEvent(QMouseEvent* e, QPointF scenePos);
    virtual bool mouseDoubleClickEvent(QMouseEvent *e, QPointF scenePos);
    virtual void init(QGraphicsScene * scene, QGraphicsView * view, const int * zoom, const double * scale, double frameCoef, uint visualizerId);
    virtual void zoomChanged();
    void areaGeometryEdited(QPolygonF);

signals:
    //void showFacility(qulonglong id);
    void switchOnMap();
    void switchOnEditor();
    void centerEditorOn(qulonglong id);
    void setMarkOnPlan(qulonglong markType, QPolygonF area);
    void editAreaGeometry(bool on_off);

protected:
    void centerViewOn(QPointF pos);
    bool markInArchive(regionbiz::MarkPtr markPtr);
    void reinit(qulonglong facilityId, qulonglong floorId = 0);
    void setFloor(qulonglong floorId);
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
    void slotFloorUp();
    void slotFloorDown();
    void slotEditAreaGeometry(QVariant var);
    void slotSetLayerVisible(QVariant var);
    void slotZoomChanged(int zoomLevel);
    void slotToolButtonInPluginChecked(QVariant var);
    void slotSetRoomVisibleOnFloor(QVariant var);
    void slotResetPixelVisualizer(QVariant var);
    void slotRotateCompasWidget(QVariant var);

private:
    struct FloorInfo
    {
        QString name;
        uint64_t id;
    };

    const uint _pixelVisualizerId;
    qulonglong _currFacilityId = 0;
    uint64_t _currFloor_id = 0;
    QMap<qulonglong, QGraphicsItem *> _itemsOnFloor;
    QMap<uint64_t, QList<qulonglong> > _itemsInLayers;
    QMap<uint16_t, FloorInfo> _floorsMap;
    QToolButton* _upButton, *_downButton;
    QLabel * _currentFacility;
    QColor _roomDefaultColor;
    bool _blockGUI = false;
    uint64_t _editObjectGeometry = 0;
    RowScale * _rowScale;
    CompasWidget * _compasWidget;

//    QMap<qulonglong, ViraGraphicsItem*> _items;
//    QMap<uint64_t, LocationItem *> _locationItems;

//    QMap<qulonglong, QString> _itemId_facilityFolder;
//    qulonglong _prevSelectedFacilityId = 0, _prevSelectedLocationId = 0;
    //uint64_t _editObjectGeometry = 0;

};



#endif // PixelWorkState_H
