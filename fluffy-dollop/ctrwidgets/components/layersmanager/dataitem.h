#ifndef DATAITEM_H
#define DATAITEM_H

#include "layermanagertypes.h"
#include <QTreeWidgetItem>

class QGraphicsSvgItem;
class QGraphicsPixmapItem;
class QGraphicsPolygonItem;

namespace layers_manager_form
{

class LayerItem : public QTreeWidgetItem
{
public:
    LayerItem(QTreeWidget * parentItem, LayerTypes layerType);           //! for base & etalon layers only !!!
    LayerItem(QTreeWidget * parentItem, QString name, uint64_t layerId);  //! for other layers !!!
    uint64_t getLayerId();
    LayerTypes getLayerType();
    void clearData();
    QTreeWidgetItem * getChild(ItemTypes itemTypes);

private:
    const LayerTypes _layerType;
    uint64_t _layerId = 0;
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
    RasterItem(QTreeWidgetItem * parentItem, uint64_t entityId, QString path, ItemTypes itemType = ItemTypes::Raster);
    ~RasterItem();
    void setRaster(QGraphicsPixmapItem * item, double scW, double scH, double rotate, QString name = QString());
    void reinit(QString path);
    QGraphicsPixmapItem * getRaster();
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

class FacilityPolygonItem : public DataItem
{
public:
    FacilityPolygonItem(QTreeWidgetItem * parentItem, uint64_t entityId, const QPolygonF &polygon);
    ~FacilityPolygonItem();
    void setPolygon(const QPolygonF &polygon);
    QGraphicsPolygonItem * getPolygonItem();

private:
    QGraphicsPolygonItem * _polygonItem = nullptr;
};

}

#endif // DATAITEM_H
