#ifndef LAYERSMANAGERFORM_H
#define LAYERSMANAGERFORM_H

#include <QWidget>
#include <QTimer>
#include <regionbiz/rb_manager.h>
#include <regionbiz/rb_locations.h>
#include <ctrcore/plugin/embifacenotifier.h>
#include "layerinstrumentalform.h"
#include "layermanagertypes.h"

class QGraphicsView;
class SvgEditorForm;
class QTreeWidgetItem;
class QGraphicsLineItem;

namespace Ui {
class LayersManagerForm;
}

namespace layers_manager_form
{
class DataItem;
class LayerItem;

struct CurrentData
{
    enum State{NoState, Create, Edit};
    enum Object{NoObject, Raster, EtalonRaster, Vector, Polygon};

    void clear()
    {
        state = NoState;
        object = NoObject;
        layerId = 0;
        planPath.clear();
    }

    regionbiz::BaseAreaPtr areaPtr; //! clear() not change areaPtr !
    uint64_t facilityId = 0; //! clear() not change areaPtr !
    bool formDisabled = true; //! clear() not change areaPtr !
    State state = NoState;
    Object object = NoObject;
    uint64_t layerId = 0;
    QString planPath;
};

class LayersManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit LayersManagerForm(QWidget *parent = 0);
    ~LayersManagerForm();
    void setEmbeddedWidgetId(quint64 id);
    void reset(bool showEtalonNode = false);
    void reload(regionbiz::BaseAreaPtr ptr, bool isGeoScene);

private slots:
    void slotItemChanged(QTreeWidgetItem *item, int column);

    void createEtalonImage();
    void slotOpenImage();
    void slotEditEntity();
    void slotCreateSvg();
    void slotOpenSvg();
    void slotDeleteEntity();
    void slotSelectionChanged();
    void slotBlockGUI(QVariant);
    void slotEditorFormClose();
    void slotAddLayer();
    void slotDeleteLayer();
    void slotTransformingItemSaved(TransformingItemSaveDatad data);
    void slotFacilityPolygonOnPlanSaved(QPolygonF polygon);
    void slotEtalotRasterSaved(TransformingItemSaveDatad data);
    void slotSvgSaved(QString filePath, QPointF scenePos);
    void slotFileLoaded(regionbiz::BaseFileKeeperPtr);
    void slotSyncMarks();
    void slotZoomChanged(int zoomLevel);
    void slotToolButtonInPluginChecked(QVariant var);
    void slotCalcSceneRect();

private:
    void syncChechState(QTreeWidgetItem *item, bool setVisible);
    void reinitLayers();
    void reinitLayer(LayerItem * layerItem);
    void syncMarks(bool hideAll = false);
    void redrawItems(int pixelDelta);
    LayerItem *getTopLevelItem(LayerTypes type);

    Ui::LayersManagerForm *ui;
    quint64 _embeddedWidgetId = 0;
    QGraphicsView * _geoView = nullptr;
    QGraphicsView * _pixelView = nullptr;
    uint _geoVisId = 0;
    uint _pixelVisId = 0;
    bool _isGeoScene;
    CurrentData _currentData;
    bool _blockRecalcLayer = false;
    LayerInstrumentalForm * _instrumentalForm = nullptr;
    EmbIFaceNotifier * _ifaceInstrumentalForm = nullptr;
    SvgEditorForm * _svgEditorForm = nullptr;
    QMap<uint64_t, LayerItem*> _layers;
    QMap<QString, DataItem*> _loadingItems;

    QList<QGraphicsLineItem*> _lineItems;
    ZoomState _zoomState = ZoomState::OneMeter;
    int _pixelDelta = 1000; // tot level, kotoriy stoit v PixelWorkState::init ---> setZlevel(-2) !!!
    bool _lineItemsVisible = false;
    QTimer _recalcSceneRectTimer;

};

}

#endif // LAYERSMANAGERFORM_H
