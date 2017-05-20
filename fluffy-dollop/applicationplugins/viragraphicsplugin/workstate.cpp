#include "workstate.h"
#include "xmlreader.h"
#include "areagraphicsitem.h"
#include "locationitem.h"
#include "types.h"
#include "defectgraphicsitem.h"
#include "fotographicsitem.h"
#include "foto360graphicsitem.h"
#include <regionbiz/rb_entity_filter.h>
#include <ctrcore/ctrcore/ctrconfig.h>
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

void WorkState::statePoppedFromStack()
{
    setActiveForScene(true);
}

void WorkState::init(QGraphicsScene *scene, QGraphicsView *view, const int *zoom, const double *scale, double frameCoef, uint visualizerId)
{
    ScrollBaseState::init(scene, view, zoom, scale, frameCoef, visualizerId);
    setActiveForScene(true);

    regionbiz::EntityFilter::subscribeOnFilterChacnge(this, SLOT(reinit()));

    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::BlockGUI, this, SLOT(slotBlockGUI(QVariant)),
                                      qMetaTypeId< bool >(),
                                      QString("visualize_system") );

    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::SetMarkPosition, this, SLOT(slotSetMarkPosition(QVariant)),
                                      qMetaTypeId< quint64 >(),
                                      QString("visualize_system") );

    auto mngr = RegionBizManager::instance();
    mngr->subscribeOnSelect(this, SLOT(slotSelectionItemsChanged(uint64_t,uint64_t)));
    mngr->subscribeCenterOn(this, SLOT(slotCenterOn(uint64_t)));
    mngr->subscribeOnChangeEntity(this, SLOT(slotObjectChanged(uint64_t)));
    mngr->subscribeOnAddEntity(this, SLOT(slotAddObject(uint64_t)));
    mngr->subscribeOnDeleteEntity(this, SLOT(slotDeleteObject(uint64_t)));

    reinit();
}

void WorkState::reinit()
{
    _prevSelectedFacilityId = 0;
    _prevSelectedLocationId = 0;
    _itemId_facilityFolder.clear();

    foreach(ViraGraphicsItem * item, _items.values())
        delete item;
    _items.clear();

    foreach(LocationItem * locationItem, _locationItems.values())
        delete locationItem;
    _locationItems.clear();

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
    std::vector<RegionPtr> regions = mngr->getRegions();
    for( RegionPtr regionPtr: regions )
    {
        AreaGraphicsItem * areaGraphicsItem = new AreaGraphicsItem(regionPtr->getCoords());
        regionInitData.id = regionPtr->getId();
        areaGraphicsItem->init(regionInitData);
        _scene->addItem(areaGraphicsItem);
        _items.insert(regionPtr->getId(), areaGraphicsItem);

        QString destPath;
        QVariant regionBizInitJson_Path = CtrConfig::getValueByName("application_settings.regionBizFilesPath",
                                                                    "./data", true);
        if(regionBizInitJson_Path.isValid())
             destPath = regionBizInitJson_Path.toString() + QDir::separator();

        std::vector<BaseAreaPtr> locations = regionPtr->getChilds( Region::RCF_LOCATIONS );
        for( BaseAreaPtr ptr: locations )
        {
            LocationPtr locationPtr = BaseArea::convert< Location >( ptr );
            if(locationPtr)
            {
                const qulonglong locationId(locationPtr->getId());
                QList<QGraphicsItem*> graphicsItems;

                QString planPath = "";
                auto plans = locationPtr->getFilesByType( BaseFileKeeper::FT_PLAN );
                if( plans.size() )
                {
                    BaseFileKeeperPtr plan = plans.at( 0 );
                    QFileInfo info( *( plan->getLocalFile().get() ));
                    planPath = info.filePath();
                }
                if(planPath.isEmpty() == false)
                {
                    QGraphicsPixmapItem * pixmapItem = new QGraphicsPixmapItem(QPixmap(planPath));

                    BaseFileKeeperPtr base_plan = plans.at( 0 );
                    PlanFileKeeperPtr plan = BaseFileKeeper::convert< PlanFileKeeper >( base_plan );
                    PlanFileKeeper::PlanParams planParams = plan->getPlanParams();
                    pixmapItem->setTransform(QTransform().scale(planParams.scale_w, planParams.scale_h));
                    pixmapItem->setPos(planParams.x, planParams.y);
                    pixmapItem->setZValue(100);
                    pixmapItem->setOpacity(0.5);
                    pixmapItem->hide();
                    _scene->addItem(pixmapItem);
                    graphicsItems.append(pixmapItem);
                }

                //qDebug() << "locationPtr :" << QString::fromStdString(locationPtr->getDescription());
                AreaGraphicsItem * areaGraphicsItem = new AreaGraphicsItem(locationPtr->getCoords());
                locationInitData.id = locationPtr->getId();
                areaGraphicsItem->init(locationInitData);
                _scene->addItem(areaGraphicsItem);
                _items.insert(locationPtr->getId(), areaGraphicsItem);
                areaGraphicsItem->hide();
                graphicsItems.append(areaGraphicsItem);

                std::vector< FacilityPtr > facilities = locationPtr->getChilds();
                for( FacilityPtr facilityPtr: facilities )
                {
                    AreaGraphicsItem * areaGraphicsItem = new AreaGraphicsItem(facilityPtr->getCoords());
                    connect(areaGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                    facInitData.id = facilityPtr->getId();
                    areaGraphicsItem->init(facInitData);
                    _scene->addItem(areaGraphicsItem);
                    _items.insert(facilityPtr->getId(), areaGraphicsItem);
                    areaGraphicsItem->hide();
                    graphicsItems.append(areaGraphicsItem);

                    MarkPtrs marks_of_facility = facilityPtr->getMarks();
                    for( MarkPtr mark: marks_of_facility )
                        if(markInArchive(mark) == false)
                        {
                            QString mark_type_str = QString::fromUtf8("дефект");
                            BaseMetadataPtr mark_type = mark->getMetadata("mark_type");
                            if(mark_type)
                            {
                                QString _mark_type_str = mark_type->getValueAsVariant().toString();
                                if(_mark_type_str == QString::fromUtf8("фотография") || _mark_type_str == QString::fromUtf8("панорамная фотография"))
                                    mark_type_str = _mark_type_str;
                            }
                            if(mark_type_str == QString::fromUtf8("дефект"))
                            {
                                QPointF center = mark->getCenter();
                                DefectGraphicsItem * _defectGraphicsItem = new DefectGraphicsItem(mark->getId());
                                _defectGraphicsItem->setProperty("locationId", locationId);
                                _defectGraphicsItem->setPos(center);

                                _scene->addItem(_defectGraphicsItem);
                                connect(_defectGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                                _items.insert(mark->getId(), _defectGraphicsItem);

                                _defectGraphicsItem->hide();
                                graphicsItems.append(_defectGraphicsItem);
                            }
                            else if(mark_type_str == QString::fromUtf8("фотография"))
                            {
                                QPointF center = mark->getCenter();
                                FotoGraphicsItem * _photoGraphicsItem = new FotoGraphicsItem(mark->getId());
                                _photoGraphicsItem->setProperty("locationId", locationId);
                                _photoGraphicsItem->setPos(center);

                                _scene->addItem(_photoGraphicsItem);
                                connect(_photoGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                                _items.insert(mark->getId(), _photoGraphicsItem);

                                _photoGraphicsItem->hide();
                                graphicsItems.append(_photoGraphicsItem);
                            }
                            else if(mark_type_str == QString::fromUtf8("панорамная фотография"))
                            {
                                QPointF center = mark->getCenter();
                                Foto360GraphicsItem * _photo360GraphicsItem = new Foto360GraphicsItem(mark->getId());
                                _photo360GraphicsItem->setProperty("locationId", locationId);
                                _photo360GraphicsItem->setPos(center);

                                _scene->addItem(_photo360GraphicsItem);
                                connect(_photo360GraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                                _items.insert(mark->getId(), _photo360GraphicsItem);

                                _photo360GraphicsItem->hide();
                                graphicsItems.append(_photo360GraphicsItem);
                            }
                        }
                }

                MarkPtrs marks_of_location = locationPtr->getMarks();
                for( MarkPtr mark: marks_of_location )
                    if(markInArchive(mark) == false)
                    {
                        QString mark_type_str = QString::fromUtf8("дефект");
                        BaseMetadataPtr mark_type = mark->getMetadata("mark_type");
                        if(mark_type)
                        {
                            QString _mark_type_str = mark_type->getValueAsVariant().toString();
                            if(_mark_type_str == QString::fromUtf8("фотография") || _mark_type_str == QString::fromUtf8("панорамная фотография"))
                                mark_type_str = _mark_type_str;
                        }
                        if(mark_type_str == QString::fromUtf8("дефект"))
                        {
                            QPointF center = mark->getCenter();
                            DefectGraphicsItem * _defectGraphicsItem = new DefectGraphicsItem(mark->getId());
                            _defectGraphicsItem->setProperty("locationId", locationId);
                            _defectGraphicsItem->setPos(center);

                            _scene->addItem(_defectGraphicsItem);
                            connect(_defectGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                            _items.insert(mark->getId(), _defectGraphicsItem);

                            _defectGraphicsItem->hide();
                            graphicsItems.append(_defectGraphicsItem);
                        }
                        else if(mark_type_str == QString::fromUtf8("фотография"))
                        {
                            QPointF center = mark->getCenter();
                            FotoGraphicsItem * _photoGraphicsItem = new FotoGraphicsItem(mark->getId());
                            _photoGraphicsItem->setProperty("locationId", locationId);
                            _photoGraphicsItem->setPos(center);

                            _scene->addItem(_photoGraphicsItem);
                            connect(_photoGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                            _items.insert(mark->getId(), _photoGraphicsItem);

                            _photoGraphicsItem->hide();
                            graphicsItems.append(_photoGraphicsItem);
                        }
                        else if(mark_type_str == QString::fromUtf8("панорамная фотография"))
                        {
                            QPointF center = mark->getCenter();
                            Foto360GraphicsItem * _photo360GraphicsItem = new Foto360GraphicsItem(mark->getId());
                            _photo360GraphicsItem->setProperty("locationId", locationId);
                            _photo360GraphicsItem->setPos(center);

                            _scene->addItem(_photo360GraphicsItem);
                            connect(_photo360GraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                            _items.insert(mark->getId(), _photo360GraphicsItem);

                            _photo360GraphicsItem->hide();
                            graphicsItems.append(_photo360GraphicsItem);
                        }
                    }

                LocationItem * locationItem = new LocationItem(locationPtr->getId(), 16, locationPtr->getCoords().boundingRect(), graphicsItems, _scene);
                connect(locationItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                locationItem->setToolTip(locationPtr->getDescription());
                connect(locationItem, SIGNAL(setViewport(QRectF)), this, SIGNAL(signalSetPrefferZoomForSceneRect(QRectF)));
                locationItem->zoomChanged(*_zoom);
                _locationItems.insert(locationPtr->getId(), locationItem);
            }
        }
    }
}

void WorkState::slotObjectChanged(uint64_t id)
{
    MarkPtr markPtr = RegionBizManager::instance()->getMark(id);
    if(markPtr)
    {
        if(markInArchive(markPtr))
        {
            slotDeleteObject(id);
        }
        else
        {
            auto markIt = _items.find(id);
            if(markIt != _items.end())
                markIt.value()->reinit();
            else
                slotAddObject(id);
        }
    }
}

void WorkState::slotAddObject(uint64_t id)
{
    MarkPtr markPtr = RegionBizManager::instance()->getMark(id);
    if(markPtr)
    {
        BaseAreaPtr parentPtr = RegionBizManager::instance()->getBaseArea(markPtr->getParentId());
        if(parentPtr)
        {
            LocationPtr locationPtr;
            switch(parentPtr->getType())
            {
            case BaseArea::AT_LOCATION :
            {
                locationPtr = BaseArea::convert<Location>(parentPtr);
            }break;
            case BaseArea::AT_FACILITY :
            {
                auto facility = BaseArea::convert<Facility>(parentPtr);
                locationPtr = BaseArea::convert<Location>(facility->getParent());
            }break;
            }

            if(locationPtr)
            {
                const qulonglong locationId(locationPtr->getId());
                LocationItem * locationItem = nullptr;
                auto locationIt = _locationItems.find(locationId);
                if(locationIt != _locationItems.end())
                    locationItem = locationIt.value();
                else
                    return;

                QString mark_type_str; //  = QString::fromUtf8("дефект");
                BaseMetadataPtr markTypePtr = markPtr->getMetadata("mark_type");
                if(markTypePtr)
                {
                    QString _mark_type_str = markTypePtr->getValueAsVariant().toString();;
                    if(_mark_type_str == QString::fromUtf8("фотография") ||
                            _mark_type_str == QString::fromUtf8("дефект") ||
                            _mark_type_str == QString::fromUtf8("панорамная фотография") )
                        mark_type_str = markTypePtr->getValueAsVariant().toString();
                }

                qDebug() << "slotAddObject, ID:" << id << ", mark_type_str:" << mark_type_str;

                if(mark_type_str == QString::fromUtf8("дефект"))
                {
                    QPointF center = markPtr->getCenter();
                    DefectGraphicsItem * _defectGraphicsItem = new DefectGraphicsItem(markPtr->getId());
                    _defectGraphicsItem->setProperty("locationId", locationId);
                    _defectGraphicsItem->setPos(center);
                    _scene->addItem(_defectGraphicsItem);
                    connect(_defectGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                    _items.insert(markPtr->getId(), _defectGraphicsItem);
                    locationItem->addItem(_defectGraphicsItem);
                }
                else if(mark_type_str == QString::fromUtf8("фотография"))
                {
                    QPointF center = markPtr->getCenter();
                    FotoGraphicsItem * _photoGraphicsItem = new FotoGraphicsItem(markPtr->getId());
                    _photoGraphicsItem->setProperty("locationId", locationId);
                    _photoGraphicsItem->setPos(center);
                    _scene->addItem(_photoGraphicsItem);
                    connect(_photoGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                    _items.insert(markPtr->getId(), _photoGraphicsItem);
                    locationItem->addItem(_photoGraphicsItem);
                }
                else if(mark_type_str == QString::fromUtf8("панорамная фотография"))
                {
                    QPointF center = markPtr->getCenter();
                    Foto360GraphicsItem * _photo360GraphicsItem = new Foto360GraphicsItem(markPtr->getId());
                    _photo360GraphicsItem->setProperty("locationId", locationId);
                    _photo360GraphicsItem->setPos(center);
                    _scene->addItem(_photo360GraphicsItem);
                    connect(_photo360GraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                    _items.insert(markPtr->getId(), _photo360GraphicsItem);
                    locationItem->addItem(_photo360GraphicsItem);
                }
            }
        }
    }
}

void WorkState::slotDeleteObject(uint64_t id)
{
    auto markIt = _items.find(id);
    if(markIt != _items.end())
    {
        bool ok;
        qulonglong locationId  = markIt.value()->property("locationId").toULongLong(&ok);
        if(ok)
        {
            auto locationIt = _locationItems.find(locationId);
            if(locationIt != _locationItems.end())
                locationIt.value()->removeItem( dynamic_cast<QGraphicsItem*>(markIt.value()) );
        }
        delete markIt.value();
        _items.erase(markIt);
    }
}

void WorkState::slotBlockGUI(QVariant var)
{
    _blockGUI = var.toBool();
}

void WorkState::slotSetMarkPosition(QVariant var)
{
    quint64 type = var.toUInt();
    if(type > 0)
    {
        uint id = regionbiz::RegionBizManager::instance()->getSelectedEntity();
        auto it = _items.find(id);
        if(it != _items.end())
        {
            it.value()->setItemselected(true);
            QPolygonF pol = it.value()->getPolygon();
            centerViewOn(pol.boundingRect().center());

            emit setMarkOnMap(type);
        }
    }
    else
    {
        emit setMarkOnMap(0);
    }
}

void WorkState::slotSelectItem(qulonglong id, bool centerOnEntity)
{
    if( ! _blockGUI)
    {
        regionbiz::RegionBizManager::instance()->selectEntity(id);
        if(centerOnEntity)
            regionbiz::RegionBizManager::instance()->centerOnEntity(id);
    }
}

void WorkState::slotSelectionItemsChanged(uint64_t prev_id, uint64_t curr_id)
{
    if(_prevSelectedLocationId > 0)
    {
        auto it = _locationItems.find(_prevSelectedLocationId);
        if(it != _locationItems.end())
            it.value()->setItemselected(false);
        _prevSelectedLocationId = 0;
    }

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
        {
            auto it = _items.find(curr_id);
            if(it != _items.end())
                it.value()->setItemselected(true);

            return;
        }

        uint64_t facilityId(0);
        switch(ptr->getType())
        {
        case BaseArea::AT_REGION :
        case BaseArea::AT_LOCATION :
        {
            auto it1 = _locationItems.find(curr_id);
            if(it1 != _locationItems.end())
            {
                _prevSelectedLocationId = curr_id;
                it1.value()->setItemselected(true);
//                if(ptr->getType() == BaseArea::AT_LOCATION)
//                {
//                    QPolygonF pol = it1.value()->polygon();
//                    centerViewOn(pol.boundingRect().center());
//                }
            }

            auto it2 = _items.find(curr_id);
            if(it2 != _items.end())
            {
                it2.value()->setItemselected(true);
                if(ptr->getType() == BaseArea::AT_LOCATION)
                {
                    QPolygonF pol = it2.value()->getPolygon();
                    centerViewOn(pol.boundingRect().center());
                }
            }
        }return;

        case BaseArea::AT_FACILITY :
        {
            facilityId = curr_id;
        }break;

        case BaseArea::AT_FLOOR :
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
                QPolygonF pol = it.value()->getPolygon();
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
            QPolygonF pol = it.value()->getPolygon();
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
                        QPolygonF pol = parentIt.value()->getPolygon();
                        setPrefferZoomForSceneRect(pol.boundingRect());
                    }
                }

            QPolygonF pol = it.value()->getPolygon();
            _view->centerOn(pol.boundingRect().center());
            facilityId = id;
        }
    }break;

    case BaseArea::AT_FLOOR :
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
    foreach(LocationItem * locationItem, _locationItems.values())
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

bool WorkState::mouseDoubleClickEvent(QMouseEvent *e, QPointF scenePos)
{
    RegionBizManager::instance()->clearSelect();
    return true;
}

bool WorkState::markInArchive(regionbiz::MarkPtr markPtr)
{
    bool _markInArchive(false);
    if(markPtr)
    {
        BaseMetadataPtr status = markPtr->getMetadata("status");
        if(status)
        {
            QString statusStr = status->getValueAsString();
            if(statusStr == QString::fromUtf8("в архиве"))
                _markInArchive = true;
        }
    }
    return _markInArchive;
}

//!    for second images :
//    _scaleX = 0.00000382951662997968;
//    _scaleY = 0.00000382951662997968;
//    _pixmapItem->setPos(299.30530569955715236574, 148.83659651624239472767);

//                if(locationPtr->getId() == 5022)
//                {
//                    PlanKeeper::PlanParams planParams;
//                    planParams.x = 298.4673898270;
//                    planParams.y = 148.0499342829;
//                    planParams.scale_w = 0.0000073504;
//                    planParams.scale_h = 0.0000073504;
//                    planParams.rotate = 0;
//                    locationPtr->setPlanParams(planParams);
//                    locationPtr->setPlanPath("0.tiff");

//                    QPolygonF pol;
//                    pol.append(QPointF(298.46761322, 148.05406952));
//                    locationPtr->setCoords(pol);
//                    locationPtr->commit();
//                }

//                    if(facilityPtr->getId() == 5023)
//                    {
//                        QPolygonF pol;
//                        pol.append(QPointF(298.46937561, 148.05240631));
//                        facilityPtr->setCoords(pol);
//                        facilityPtr->commit();
//                    }















