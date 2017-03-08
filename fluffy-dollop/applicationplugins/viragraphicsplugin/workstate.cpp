#include "workstate.h"
#include "xmlreader.h"
#include "areagraphicsitem.h"
#include "locationitem.h"
#include "types.h"
#include <regionbiz/rb_manager.h>
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

using namespace regionbiz;

WorkState::WorkState()
{
}

WorkState::~WorkState()
{
    for(auto it = _items.begin(); it != _items.end(); ++it)
        delete it.value();
}

void WorkState::init(QGraphicsScene *scene, QGraphicsView *view, const int *zoom, const double *scale, double frameCoef, uint visualizerId)
{
    ScrollBaseState::init(scene, view, zoom, scale, frameCoef, visualizerId);
    setActiveForScene(true);

    AreaInitData regionInitData;
    regionInitData.zValue = 100;
    regionInitData.penNormal.setColor(Qt::green);
    regionInitData.penNormal.setCosmetic(true);
    regionInitData.penNormal.setWidth(2);
    regionInitData.penHoverl = regionInitData.penNormal;
    regionInitData.penHoverl.setWidth(3);
    regionInitData.brushNormal.setStyle(Qt::NoBrush);
    regionInitData.brushHoverl.setStyle(Qt::NoBrush);

    AreaInitData locationInitData;
    locationInitData.zValue = 1000;
    locationInitData.penNormal.setColor(Qt::red);
    locationInitData.penNormal.setCosmetic(true);
    locationInitData.penNormal.setWidth(2);
    locationInitData.penHoverl = locationInitData.penNormal;
    locationInitData.penHoverl.setWidth(3);
    locationInitData.brushNormal.setStyle(Qt::NoBrush);
    locationInitData.brushHoverl.setStyle(Qt::NoBrush);

    AreaInitData facInitData;
    facInitData.zValue = 10000;
    facInitData.sendDoubleClick = true;
    facInitData.isSelectableFromMap = true;
    facInitData.penNormal.setColor(Qt::blue);
    facInitData.penNormal.setCosmetic(true);
    facInitData.penNormal.setWidth(2);
    facInitData.penHoverl = facInitData.penNormal;
    facInitData.penHoverl.setWidth(3);
    QColor facBrushColor(Qt::blue);
    facBrushColor.setAlpha(50);
    facInitData.brushNormal.setColor(facBrushColor);
    facInitData.brushNormal.setStyle(Qt::SolidPattern);
    facBrushColor.setAlpha(50);
    facInitData.brushHoverl = facInitData.brushNormal;
    facInitData.brushHoverl.setColor(facBrushColor);

    auto mngr = RegionBizManager::instance();
    mngr->subscribeOnSelect(this, SLOT(slotSelectionItemsChanged(uint64_t,uint64_t)));
    mngr->subscribeOnCenterOn(this, SLOT(slotCenterOn(uint64_t)));

    std::vector<RegionPtr> regions = mngr->getRegions();
    for( RegionPtr regionPtr: regions )
    {
        QPolygonF scenePolygon;
        for(Coord coord : regionPtr->getCoords())
            scenePolygon.append(QPointF(coord.x, coord.y));
        AreaGraphicsItem * areaGraphicsItem = new AreaGraphicsItem(scenePolygon);
        regionInitData.id = regionPtr->getId();
        areaGraphicsItem->init(regionInitData);
        _scene->addItem(areaGraphicsItem);
        _items.insert(regionPtr->getId(), areaGraphicsItem);

        std::vector<BaseAreaPtr> locations = regionPtr->getChilds( Region::RCF_LOCATIONS );
        for( BaseAreaPtr ptr: locations )
        {
            LocationPtr locationPtr = BaseArea::convert< Location >( ptr );
            if(locationPtr)
            {
                //qDebug() << "locationPtr :" << QString::fromStdString(locationPtr->getDescription());
                QPolygonF scenePolygon;
                for(Coord coord : locationPtr->getCoords())
                    scenePolygon.append(QPointF(coord.x, coord.y));
                AreaGraphicsItem * areaGraphicsItem = new AreaGraphicsItem(scenePolygon);
                locationInitData.id = locationPtr->getId();
                areaGraphicsItem->init(locationInitData);
                _scene->addItem(areaGraphicsItem);
                _items.insert(locationPtr->getId(), areaGraphicsItem);

//                LocationItem * locationItem = new LocationItem(14, scenePolygon.boundingRect(), graphicsItems, _scene);
//                locationItem->setToolTip(location.description);
//                connect(locationItem, SIGNAL(setViewport(QRectF)), this, SIGNAL(signalSetPrefferZoomForSceneRect(QRectF)));
//                locationItem->zoomChanged(*_zoom);
//                _locationItems.append(locationItem);

                std::vector< FacilityPtr > facilities = locationPtr->getChilds();
                for( FacilityPtr facilityPtr: facilities )
                {
                    QPolygonF scenePolygon;
                    for(Coord coord : facilityPtr->getCoords())
                        scenePolygon.append(QPointF(coord.x, coord.y));
                    AreaGraphicsItem * areaGraphicsItem = new AreaGraphicsItem(scenePolygon);
                    connect(areaGraphicsItem, SIGNAL(dubleClickOnItem(qulonglong)), this, SLOT(slotDubleClickOnFacility(qulonglong)));
                    facInitData.id = facilityPtr->getId();
                    areaGraphicsItem->init(facInitData);
                    _scene->addItem(areaGraphicsItem);
                    _items.insert(facilityPtr->getId(), areaGraphicsItem);
                }
            }
        }
    }

/*
    pdf_editor::Location location;
    if( ! pdf_editor::XmlReader::readLocation(_xmlFilePath, location))
        return;

    QList<QGraphicsItem*> graphicsItems;

    if(location.coordsOnScene.size() > 2)
    {
        AreaInitData areaInitData;
        areaInitData.zValue = 1000;
        areaInitData.tooltip = location.name;

        areaInitData.penNormal.setColor(Qt::red);
        areaInitData.penNormal.setCosmetic(true);
        areaInitData.penNormal.setWidth(2);

        areaInitData.penHoverl = areaInitData.penNormal;
        areaInitData.penHoverl.setWidth(3);

        areaInitData.brushNormal.setStyle(Qt::NoBrush);
        areaInitData.brushHoverl.setStyle(Qt::NoBrush);

        AreaGraphicsItem * areaGraphicsItem = new AreaGraphicsItem(location.coordsOnScene);
        areaGraphicsItem->init(areaInitData);
        graphicsItems.append(areaGraphicsItem);
        _scene->addItem(areaGraphicsItem);
    }

    QString destPath = QFileInfo(_xmlFilePath).canonicalPath() + QDir::separator();
    if(location.image.name.isEmpty() == false)
    {
        QPixmap pixmap(destPath + location.image.name);
        if(pixmap.isNull() == false)
        {
            QGraphicsPixmapItem * pixmapItem = new QGraphicsPixmapItem(pixmap);
            pixmapItem->setTransform(QTransform().scale(location.image.scale, location.image.scale));
            pixmapItem->setPos(location.image.pos);
            pixmapItem->setZValue(100);
            pixmapItem->setOpacity(0.5);
            graphicsItems.append(pixmapItem);
            _scene->addItem(pixmapItem);
        }
    }

    AreaInitData areaInitData;
    areaInitData.zValue = 10000;
    areaInitData.sendDoubleClick = true;

    areaInitData.penNormal.setColor(Qt::blue);
    areaInitData.penNormal.setCosmetic(true);
    areaInitData.penNormal.setWidth(2);

    areaInitData.penHoverl = areaInitData.penNormal;
    areaInitData.penHoverl.setWidth(3);

    QColor brushColor(Qt::blue);
    brushColor.setAlpha(50);
    areaInitData.brushNormal.setColor(brushColor);
    areaInitData.brushNormal.setStyle(Qt::SolidPattern);

    brushColor.setAlpha(50);
    areaInitData.brushHoverl = areaInitData.brushNormal;
    areaInitData.brushHoverl.setColor(brushColor);

    qulonglong counter(0);
    foreach(pdf_editor::FacilityInLocation facility, location.facilitiesInLocation)
    {
        areaInitData.id = ++counter;
        areaInitData.tooltip = facility.name;

        AreaGraphicsItem * areaGraphicsItem = new AreaGraphicsItem(facility.coordsOnScene);
        areaGraphicsItem->init(areaInitData);
        graphicsItems.append(areaGraphicsItem);
        _scene->addItem(areaGraphicsItem);
        QString xmlFilePath = destPath + facility.folder + QDir::separator() + QString("facility.xml");
        _itemId_facilityFolder.insert(areaInitData.id, xmlFilePath);
        connect(areaGraphicsItem, SIGNAL(dubleClickOnItem(qulonglong)), this, SLOT(slotDubleClickOnFacility(qulonglong)));
        // connect(areaGraphicsItem, SIGNAL(signal_setItemselect(qulonglong,bool)), this, SLOT(slotSetItemselect(qulonglong,bool)));
    }

    LocationItem * locationItem = new LocationItem(14, location.coordsOnScene.boundingRect(), graphicsItems, _scene);
    locationItem->setToolTip(location.description);
    connect(locationItem, SIGNAL(setViewport(QRectF)), this, SIGNAL(signalSetPrefferZoomForSceneRect(QRectF)));
    locationItem->zoomChanged(*_zoom);
    _locationItems.append(locationItem);

    */
}

void WorkState::slotSelectionItemsChanged(uint64_t prev_id, uint64_t curr_id)
{
    if(prev_id > 0)
    {
        auto it = _items.find(prev_id);
        if(it != _items.end())
            it.value()->setItemselected(false);
    }
    if(curr_id > 0)
    {
        BaseAreaPtr ptr = RegionBizManager::instance()->getBaseLoation(curr_id);
        if( ! ptr)
            return;

        uint64_t facilityId(0);
        switch(ptr->getType())
        {
        case BaseArea::AT_REGION :
        case BaseArea::AT_LOCATION :
        {
            auto it = _items.find(curr_id);
            if(it != _items.end())
            {
                emit switchOnMap();
                it.value()->setItemselected(true);
            }
        }return;

        case BaseArea::AT_FACILITY :
        {
            facilityId = curr_id;
        }break;

        case BaseArea::AT_FLOOR :
        case BaseArea::AT_ROOMS_GROUP :
        case BaseArea::AT_ROOM :
        {
            BaseAreaPtr parentPtr = ptr->getParent();
            while(parentPtr)
            {
                if(parentPtr->getType() == BaseArea::AT_FACILITY)
                {
                    facilityId = parentPtr->getId();
                    break;
                }
                parentPtr = parentPtr->getParent();
            }
        }break;
        }

        if(facilityId > 0)
        {
            emit showFacility(facilityId);

            auto it = _items.find(facilityId);
            if(it != _items.end())
                it.value()->setItemselected(true);
        }
    }
}

void WorkState::slotCenterOn(uint64_t id)
{
    BaseAreaPtr ptr = RegionBizManager::instance()->getBaseLoation(id);
    if( ! ptr)
        return;

    auto it = _items.find(id);
    switch(ptr->getType())
    {
    case BaseArea::AT_REGION :
    case BaseArea::AT_LOCATION :
    {
        if(it != _items.end())
        {
            QPolygonF pol = it.value()->polygon();
            setPrefferZoomForSceneRect(pol.boundingRect());
        }
    }break;

    case BaseArea::AT_FACILITY :
    {
        if(it != _items.end())
        {
            QPolygonF pol = it.value()->polygon();
            _view->centerOn(pol.boundingRect().center());
        }
    }break;

    case BaseArea::AT_FLOOR :
    {
    }break;

    case BaseArea::AT_ROOMS_GROUP :
    case BaseArea::AT_ROOM :
    {
        emit centerEditorOn(id);
    }break;
    }
}

void WorkState::zoomChanged()
{
    foreach(LocationItem * locationItem, _locationItems)
        locationItem->zoomChanged(*_zoom);
}

void WorkState::slotDubleClickOnFacility(qulonglong id)
{
    emit showFacility(id);

//    auto it = _itemId_facilityFolder.find(id);
//    if(it != _itemId_facilityFolder.end())
//    {
//        emit showFacility(it.value());
//    }
}

void WorkState::slotSetItemselect(qulonglong id, bool on_off)
{
//    auto it = _itemId_facilityFolder.find(id);
//    if(it != _itemId_facilityFolder.end())
//    {
//        it.value()->setItemselect(on_off);
//    }

}

bool WorkState::mousePressEvent(QMouseEvent* e, QPointF scenePos)
{
    // qDebug() << "mousePressEvent :" << QString::number(scenePos.x(), 'f', 8) << QString::number(scenePos.y(), 'f', 8);
    return ScrollBaseState::mousePressEvent(e, scenePos);
}

//!    for second images :
//    _scaleX = 0.00000382951662997968;
//    _scaleY = 0.00000382951662997968;
//    _pixmapItem->setPos(299.30530569955715236574, 148.83659651624239472767);



















