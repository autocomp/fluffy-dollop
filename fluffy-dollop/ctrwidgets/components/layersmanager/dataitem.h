#ifndef DATAITEM_H
#define DATAITEM_H

#include "layermanagertypes.h"
#include <QTreeWidgetItem>
#include <QGraphicsSvgItem>

class QGraphicsPixmapItem;
class QGraphicsPolygonItem;
class QSvgRenderer;

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
    double getScW();
    double getScH();
    double getRotate();

protected:
    const uint64_t _entityId;
    QString _path;
    double _scW = 1, _scH = 1, _rotate = 1;
};

class RasterItem : public DataItem
{
public:
    RasterItem(QTreeWidgetItem * parentItem, uint64_t entityId, QString path, ItemTypes itemType = ItemTypes::Raster);
    ~RasterItem();
    void setRaster(QGraphicsPixmapItem * item, double scW, double scH, double rotate, QString name = QString());
    void reinit(QString path);
    QGraphicsPixmapItem * getRaster();
    QString getRasterFileBaseName();

private:
    QGraphicsPixmapItem * _pixmapItem = nullptr;
};

class SvgItem : public DataItem
{
public:
    SvgItem(QTreeWidgetItem * parentItem, uint64_t entityId, QString path);
    ~SvgItem();
    void setSvg(QGraphicsSvgItem * svg, QString svgFilePath, double scW, double scH, double rotate, QString name = QString());
    void reinit(QString path);
    QGraphicsSvgItem * getSvg();
    QString getSvgFilePath();

private:
    QGraphicsSvgItem * _svg = nullptr;
    QString _svgFilePath;
};

class FacilityPolygonItem : public DataItem
{
public:
    FacilityPolygonItem(QTreeWidgetItem * parentItem, uint64_t entityId, const QPolygonF &facilityCoordsOnMapScene, const QTransform &transformer);
    ~FacilityPolygonItem();
    void reinit(const QPolygonF &facilityCoordsOnMapScene, const QTransform &transformer);
    QGraphicsPolygonItem * getPolygonItem();
    QTransform getTransformer();
    QRectF getBrectOnMapScene();

private:
    QGraphicsPolygonItem * _polygonItem = nullptr;
    QRectF _bRectOnMapScene;
    QTransform _transformer;
};

class GraphicsSvgItem : public QGraphicsSvgItem
{
public:
    GraphicsSvgItem(const QString &filePath);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);
};

}

#endif // DATAITEM_H
