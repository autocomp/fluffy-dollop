#include "dataitem.h"

#include <QSvgRenderer>
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>
#include <QPainter>

using namespace layers_manager_form;


LayerItem::LayerItem(QTreeWidget * parent, LayerTypes layerType)
    : QTreeWidgetItem(parent, QStringList(), (int)ItemTypes::Layer)
    , _layerType(layerType)
{
    switch(layerType)
    {
    case LayerTypes::Base : setText(0, QString::fromUtf8("базовый")); break;
    case LayerTypes::Etalon : setText(0, QString::fromUtf8("опорный")); break;
    case LayerTypes::TopoScheme : setText(0, QString::fromUtf8("топосхема")); break;
    }
}

LayerItem::LayerItem(QTreeWidget * parent, QString name, uint64_t layerId)
    : QTreeWidgetItem(parent, QStringList() << name, (int)ItemTypes::Layer)
    , _layerType(LayerTypes::Other)
    , _layerId(layerId)
{
}

uint64_t LayerItem::getLayerId()
{
    return _layerId;
}

LayerTypes LayerItem::getLayerType()
{
    return _layerType;
}

void LayerItem::clearData()
{
    for(int i(0); i < childCount(); ++i)
    {
        QTreeWidgetItem * children = child(i);
        switch(children->type())
        {
        case (int)ItemTypes::Rasters :
        case (int)ItemTypes::Vectors :
        {
            if(children->checkState(0) == Qt::PartiallyChecked)
                children->setCheckState(0, Qt::Checked);

            QList<QTreeWidgetItem*> items;
            for(int i(0); i < children->childCount(); ++i)
                items.append(children->child(i));

            foreach(QTreeWidgetItem* item, items)
                delete item;
        }break;
        }
    }

    int cheched(0), partially(0), ucheched(0);
    for(int a(0); a<childCount() ; ++a)
        switch(child(a)->checkState(0))
        {
        case Qt::Checked : ++cheched; break;
        case Qt::PartiallyChecked : ++partially; break;
        case Qt::Unchecked : ++ucheched; break;
        }

    if(partially > 0)
        setCheckState(0, Qt::PartiallyChecked);
    else if(cheched > 0 && partially == 0 && ucheched == 0)
        setCheckState(0, Qt::Checked);
    else if(cheched == 0 && partially == 0 && ucheched > 0)
        setCheckState(0, Qt::Unchecked);
    else // cheched == 0 && partially == 0 && ucheched == 0
        setCheckState(0, Qt::Checked);
}

QTreeWidgetItem *LayerItem::getChild(ItemTypes itemTypes)
{
    for(int i(0); i<childCount(); ++i)
        if(child(i)->type() == (int)itemTypes)
            return child(i);

    return nullptr;
}

///-----------------------------------------------

DataItem::DataItem(QTreeWidgetItem * parentItem, uint64_t entityId, QString path, int type)
    : QTreeWidgetItem(parentItem, QStringList() << QString::fromUtf8("... загрузка ..."), type)
    , _entityId(entityId)
    , _path(path)
{
    Qt::CheckState checkState = parentItem->checkState(0);
    setCheckState(0, checkState);
}

uint64_t DataItem::getEntityId()
{
    return _entityId;
}

QString DataItem::getPath()
{
    return _path;
}

double DataItem::getScW()
{
    return _scW;
}

double DataItem::getScH()
{
    return _scH;
}

double DataItem::getRotate()
{
    return _rotate;
}

///-----------------------------------------------

RasterItem::RasterItem(QTreeWidgetItem *parentItem, uint64_t entityId, QString path, ItemTypes itemType)
    : DataItem(parentItem, entityId, path, (int)itemType)
{
}

RasterItem::~RasterItem()
{
    if(_pixmapItem)
        delete _pixmapItem;
}

void RasterItem::setRaster(QGraphicsPixmapItem *item, double scW, double scH, double rotate, QString name)
{
    _scW = scW;
    _scH = scH;
    _rotate = rotate;
    _pixmapItem = item;

    if(type() == (int)ItemTypes::FacilityEtalonRaster)
        name = QString::fromUtf8("опорное изображение");
    else if(name.isEmpty())
    {
        name = QString::fromUtf8("изображение");
//        LayerItem * layerItem = dynamic_cast<LayerItem*>(parent());
//        if(layerItem)
//            if(layerItem->getLayerType() == LayerTypes::Etalon)
//                name = QString::fromUtf8("опорное изображение");
    }
    setText(0, name);

    Qt::CheckState checkState = parent()->checkState(0);
    setCheckState(0, checkState);
    _pixmapItem->setVisible(checkState == Qt::Checked);
}

void RasterItem::reinit(QString path)
{
    _path = path;
    _scW = 1;
    _scH = 1;
    _rotate = 1;
    delete _pixmapItem;
    _pixmapItem = nullptr;

    setText(0, QString::fromUtf8("... загрузка ..."));
}

QGraphicsPixmapItem *RasterItem::getRaster()
{
    return _pixmapItem;
}

QString RasterItem::getRasterFileBaseName()
{
    QStringList list = text(0).split(QString("."));
    if(list.isEmpty() == false)
        return list.first();
    else
        return QString("images");
}

///-----------------------------------------------

SvgItem::SvgItem(QTreeWidgetItem * parentItem, uint64_t entityId, QString path)
    : DataItem(parentItem, entityId, path, (int)ItemTypes::Vector)
{
}

SvgItem::~SvgItem()
{
    if(_svg)
        delete _svg;
}

void SvgItem::setSvg(QGraphicsSvgItem * svg, QString svgFilePath, double scW, double scH, double rotate, QString name)
{
    _svg = svg;
    _svgFilePath = svgFilePath;
    _scW = scW;
    _scH = scH;
    _rotate = rotate;

    if(name.isEmpty())
        setText(0, QString::fromUtf8("данные"));
    else
        setText(0, name);

    Qt::CheckState checkState = parent()->checkState(0);
    setCheckState(0, checkState);
    _svg->setVisible(checkState == Qt::Checked);
}

void SvgItem::reinit(QString path)
{
    _path = path;
    delete _svg;
    _svg = nullptr;

    setText(0, QString::fromUtf8("... загрузка ..."));
}

QGraphicsSvgItem * SvgItem::getSvg()
{
    return _svg;
}

QString SvgItem::getSvgFilePath()
{
    return _svgFilePath;
}

///-----------------------------------------------

FacilityPolygonItem::FacilityPolygonItem(QTreeWidgetItem * parentItem, uint64_t entityId, const QPolygonF &facilityCoordsOnMapScene, const QTransform &transformer)
    : DataItem(parentItem, entityId, QString(), (int)ItemTypes::FacilityPolygonOnPlan)
    , _transformer(transformer)
{
    _bRectOnMapScene = facilityCoordsOnMapScene.boundingRect();
    QPolygonF facilityCoordsOnFloorScene = _transformer.map(facilityCoordsOnMapScene);
    _polygonItem = new QGraphicsPolygonItem(facilityCoordsOnFloorScene);
    setText(0, QString::fromUtf8("контур здания"));
}

FacilityPolygonItem::~FacilityPolygonItem()
{
    delete _polygonItem;
}

void FacilityPolygonItem::reinit(const QPolygonF &facilityCoordsOnMapScene, const QTransform &transformer)
{
    _transformer = transformer;
    _bRectOnMapScene = facilityCoordsOnMapScene.boundingRect();
    QPolygonF facilityCoordsOnFloorScene = _transformer.map(facilityCoordsOnMapScene);
    _polygonItem->setPolygon(facilityCoordsOnFloorScene);
}

QGraphicsPolygonItem * FacilityPolygonItem::getPolygonItem()
{
    return _polygonItem;
}

QTransform FacilityPolygonItem::getTransformer()
{
    return _transformer;
}

QRectF FacilityPolygonItem::getBrectOnMapScene()
{
    return _bRectOnMapScene;
}

///-----------------------------------------------

GraphicsSvgItem::GraphicsSvgItem(const QString &filePath)
    : QGraphicsSvgItem(filePath)
{
}

void GraphicsSvgItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setClipping(true);
    QRegion region(boundingRect().toRect());
    painter->setClipRegion(region);

    QGraphicsSvgItem::paint(painter, option, widget);
}











