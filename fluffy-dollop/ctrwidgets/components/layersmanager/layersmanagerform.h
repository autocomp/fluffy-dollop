#ifndef LAYERSMANAGERFORM_H
#define LAYERSMANAGERFORM_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <QGraphicsLineItem>
#include <regionbiz/rb_manager.h>
#include <ctrcore/plugin/embifacenotifier.h>

class QGraphicsView;
class QGraphicsPixmapItem;
class QGraphicsSvgItem;
class LayerInstrumentalForm;
class SvgEditorForm;

namespace Ui {
class LayersManagerForm;
}

namespace layers_manager_form
{

enum class ItemTypes
{
    Layer = 1,
    Rasters, // [1..N] Raster
    Raster,
    Vectors, // [1..N] Vector
    Vector,
    Marks, // [1..N] Defect and\or Photo and\orPhoto3d
    Defect,
    Photo,
    Photo3d
};

enum class ZoomState
{
    TenCentimeters,
    OneMeter,
    TenMeter,
    HundredMeter
};

struct CurrentData
{
    enum State{NoState, Create, Edit};
    enum Object{NoObject, Raster, Vector};

    void clear()
    {
        state = NoState;
        object = NoObject;
        layerId = 0;
        planPath.clear();
    }

    uint64_t areaId = 0; //! clear() not change areaId !
    State state = NoState;
    Object object = NoObject;
    uint64_t layerId = 0;
    QString planPath;
};

class DataItem;
class LayerItem;

class LayersManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit LayersManagerForm(QWidget *parent = 0);
    ~LayersManagerForm();
    void setEmbeddedWidgetId(quint64 id);
    void reset();
    void reload(regionbiz::BaseAreaPtr ptr, bool isGeoScene);

private slots:
    void slotItemChanged(QTreeWidgetItem *item, int column);
    void slotAddEntity();
    void slotEditEntity();
    void slotDeleteEntity();
    void slotSelectionChanged();
    void slotBlockGUI(QVariant);
    void slotEditorFormClose();
    void slotAddLayer();
    void slotDeleteLayer();
    void slotRasterSaved(QString filePath, QPointF scenePos, double scaleW, double scaleH, double rotate);
    void slotSvgSaved(QString filePath, QPointF scenePos);
    void slotFileLoaded(regionbiz::BaseFileKeeperPtr);
    void slotSyncMarks();
    void slotZoomChanged(int zoomLevel);
    void slotToolButtonInPluginChecked(QVariant var);

private:
    void syncChechState(QTreeWidgetItem *item, bool setVisible);
    void reinitLayers();
    void reinitLayer(LayerItem * layerItem);
    void syncMarks(bool hideAll = false);
    void calcSceneRect();
    void redrawItems(int pixelDelta);

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
    int _pixelDelta;
    bool _lineItemsVisible = false;
};

class LayerItem : public QTreeWidgetItem
{
public:
    LayerItem(QTreeWidget * parentItem);                                  //! for base layer only !!!
    LayerItem(QTreeWidget * parentItem, QString name, uint64_t layerId);  //! for other layers !!!
    uint64_t getLayerId();
    bool isBaseLayer();
    void clearData();
    QTreeWidgetItem * rasterItems();
    QTreeWidgetItem * vectorItems();

private:
    uint64_t _layerId;
};

class DataItem : public QTreeWidgetItem
{
public:
    DataItem(QTreeWidgetItem * parentItem, uint64_t entityId, QString path, int type);
    virtual ~DataItem() {}
    uint64_t getEntityId();
    QString getPath();

protected:
    const uint64_t _entityId;
    QString _path;
};

class RasterItem : public DataItem
{
public:
    RasterItem(QTreeWidgetItem * parentItem, uint64_t entityId, QString path);
    ~RasterItem();
    void setRaster(QGraphicsPixmapItem * item, double scW, double scH, double rotate, QString name = QString());
    void reinit(QString path);
    QGraphicsPixmapItem * getRaster();
    uint64_t getEntityId();
    double getScW();
    double getScH();
    double getRotate();

private:
    double _scW = 1, _scH = 1, _rotate = 1;
    QGraphicsPixmapItem * _pixmapItem = nullptr;
};

class SvgItem : public DataItem
{
public:
    SvgItem(QTreeWidgetItem * parentItem, uint64_t entityId, QString path);
    ~SvgItem();
    void setSvg(QGraphicsSvgItem * svg, QString name = QString());
    void reinit(QString path);
    QGraphicsSvgItem * getSvg();

private:
    QGraphicsSvgItem * _svg = nullptr;
};

}

#endif // LAYERSMANAGERFORM_H
