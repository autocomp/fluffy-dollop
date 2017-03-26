#include "workstate.h"
#include "xmlreader.h"
#include "areagraphicsitem.h"
#include "locationitem.h"
#include "types.h"
#include <regionbiz/rb_manager.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

using namespace regionbiz;

WorkState::WorkState()
{
//    QString name = "/home/sergey/contour_ng/1/11.tiff";
//    QPixmap pm(name);
//    if(pm.isNull() == false)
//    {
//        pm = pm.scaled(pm.width()/2., pm.height()/2., Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//        bool saved = pm.save("/home/sergey/contour_ng/10/11.tiff");
//    }

//    QDir dir("/home/sergey/contour_ng/2/");
//    foreach(QFileInfo fi, dir.entryInfoList())
//    {
//        QString name = fi.absoluteFilePath();
//        QPixmap pm(name);
//        if(pm.isNull() == false)
//        {
//            pm = pm.scaled(pm.width()/2., pm.height()/2., Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//            bool saved = pm.save("/home/sergey/contour_ng/10/" + fi.fileName());
//        }
//    }
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

    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::BlockGUI, this, SLOT(slotBlockGUI(QVariant)),
                                      qMetaTypeId< bool >(),
                                      QString("visualize_system") );

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
    mngr->subscribeCenterOn(this, SLOT(slotCenterOn(uint64_t)));

    std::vector<RegionPtr> regions = mngr->getRegions();
    for( RegionPtr regionPtr: regions )
    {
        AreaGraphicsItem * areaGraphicsItem = new AreaGraphicsItem(regionPtr->getCoords());
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
                AreaGraphicsItem * areaGraphicsItem = new AreaGraphicsItem(locationPtr->getCoords());
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
                    AreaGraphicsItem * areaGraphicsItem = new AreaGraphicsItem(facilityPtr->getCoords());
                    connect(areaGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                    facInitData.id = facilityPtr->getId();
                    areaGraphicsItem->init(facInitData);
                    _scene->addItem(areaGraphicsItem);
                    _items.insert(facilityPtr->getId(), areaGraphicsItem);
                }
            }
        }
    }
}

void WorkState::slotBlockGUI(QVariant var)
{
    _blockGUI = var.toBool();
}

void WorkState::slotSelectItem(qulonglong id, bool centerOnArea)
{
    if( ! _blockGUI)
    {
        regionbiz::RegionBizManager::instance()->selectArea(id);
        if(centerOnArea)
            regionbiz::RegionBizManager::instance()->centerOnArea(id);
    }
}

void WorkState::slotSelectionItemsChanged(uint64_t prev_id, uint64_t curr_id)
{
    if(_prevSelectedFacilityId > 0)
    {
        auto it = _items.find(_prevSelectedFacilityId);
        if(it != _items.end())
            it.value()->setItemselected(false);
        _prevSelectedFacilityId = 0;
    }

    if(prev_id > 0)
    {
        auto it = _items.find(prev_id);
        if(it != _items.end())
            it.value()->setItemselected(false);
    }
    if(curr_id > 0)
    {
        BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(curr_id);
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
                it.value()->setItemselected(true);
                if(ptr->getType() == BaseArea::AT_LOCATION)
                {
                    QPolygonF pol = it.value()->polygon();
                    centerViewOn(pol.boundingRect().center());
                }
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
            _prevSelectedFacilityId = facilityId;
            auto it = _items.find(facilityId);
            if(it != _items.end())
            {
                it.value()->setItemselected(true);
                QPolygonF pol = it.value()->polygon();
                centerViewOn(pol.boundingRect().center());
            }
        }
    }
}

void WorkState::centerViewOn(QPointF pos)
{
    QRectF sceneContentsRect(_view->mapToScene(_view->contentsRect().topLeft()), _view->mapToScene(_view->contentsRect().bottomRight()));
    if(sceneContentsRect.contains(pos) == false)
        _view->centerOn(pos);
}

void WorkState::slotCenterOn(uint64_t id)
{
    BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(id);
    if( ! ptr)
        return;

    uint64_t facilityId(0);
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
            emit switchOnMap();
        }
    }break;

    case BaseArea::AT_FACILITY :
    {
        if(it != _items.end())
        {
            BaseAreaPtr parentPtr = ptr->getParent();
            if(parentPtr)
                if(parentPtr->getType() == BaseArea::AT_LOCATION)
                {
                    auto parentIt = _items.find(parentPtr->getId());
                    if(parentIt != _items.end())
                    {
                        QPolygonF pol = parentIt.value()->polygon();
                        setPrefferZoomForSceneRect(pol.boundingRect());
                    }
                }

            QPolygonF pol = it.value()->polygon();
            _view->centerOn(pol.boundingRect().center());
            facilityId = id;
        }
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
        if(ptr->getType() != BaseArea::AT_FACILITY)
            emit centerEditorOn(id);
    }
}

void WorkState::zoomChanged()
{
    foreach(LocationItem * locationItem, _locationItems)
        locationItem->zoomChanged(*_zoom);
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
    qDebug() << "mousePressEvent :" << QString::number(scenePos.x(), 'f', 8) << QString::number(scenePos.y(), 'f', 8);
    return ScrollBaseState::mousePressEvent(e, scenePos);
}

//!    for second images :
//    _scaleX = 0.00000382951662997968;
//    _scaleY = 0.00000382951662997968;
//    _pixmapItem->setPos(299.30530569955715236574, 148.83659651624239472767);



















