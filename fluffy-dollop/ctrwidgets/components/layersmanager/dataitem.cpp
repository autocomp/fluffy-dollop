#include "dataitem.h"

#include <QGraphicsSvgItem>
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>

using namespace layers_manager_form;


LayerItem::LayerItem(QTreeWidget * parent, LayerTypes layerType)
    : QTreeWidgetItem(parent,
                      ( layerType == LayerTypes::Base ? QStringList() << QString::fromUtf8("базовый") : QStringList() << QString::fromUtf8("опорный") )
                      , (int)ItemTypes::Layer)
    , _layerType(layerType)
{
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
    if(name.isEmpty())
    {
        name = QString::fromUtf8("изображение");
        LayerItem * layerItem = dynamic_cast<LayerItem*>(parent());
        if(layerItem)
            if(layerItem->getLayerType() == LayerTypes::Etalon)
                name = QString::fromUtf8("опорное изображение");
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

double RasterItem::getScW()
{
    return _scW;
}

double RasterItem::getScH()
{
    return _scH;
}

double RasterItem::getRotate()
{
    return _rotate;
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

void SvgItem::setSvg(QGraphicsSvgItem * svg, QString name)
{
    _svg = svg;
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

///-----------------------------------------------

FacilityPolygonItem::FacilityPolygonItem(QTreeWidgetItem * parentItem, uint64_t entityId, const QPolygonF &polygon)
    : DataItem(parentItem, entityId, QString(), (int)ItemTypes::FacilityPolygonOnPlan)
{
    _polygonItem = new QGraphicsPolygonItem(polygon);
    setText(0, QString::fromUtf8("контур здания"));
}

FacilityPolygonItem::~FacilityPolygonItem()
{
    delete _polygonItem;
}

void FacilityPolygonItem::setPolygon(const QPolygonF &polygon)
{
    _polygonItem->setPolygon(polygon);
}

QGraphicsPolygonItem * FacilityPolygonItem::getPolygonItem()
{
    return _polygonItem;
}

















