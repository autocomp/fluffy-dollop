#include "pixelworkstate.h"
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
#include <QHBoxLayout>
#include <QLabel>

using namespace regionbiz;

PixelWorkState::PixelWorkState(uint pixelVisualizerId)
    : _pixelVisualizerId(pixelVisualizerId)
{
    _upButton = new QToolButton();
    _upButton->setIcon(QIcon(":/img/up.png"));
    _upButton->setFixedSize(32,32);
    _upButton->setStyleSheet("border-radius:4;border-color: rgb(255, 255, 255);");
    connect(_upButton, SIGNAL(clicked(bool)), this, SLOT(slotFloorUp()));

    _currentFacility = new QLabel;
    _currentFacility->setAlignment(Qt::AlignCenter);
    _currentFacility->setStyleSheet("border-radius:4;border-color: rgb(255, 255, 255);");
    _currentFacility->setMinimumSize(100, 30);

    _downButton = new QToolButton();
    _downButton->setIcon(QIcon(":/img/down.png"));
    _downButton->setFixedSize(32,32);
    _downButton->setStyleSheet("border-radius:4;border-color: rgb(255, 255, 255);");
    connect(_downButton, SIGNAL(clicked(bool)), this, SLOT(slotFloorDown()));

    QVariant _selCol = CtrConfig::getValueByName(QString("styles.roomdefaultColor_RGB"));
    if(!_selCol.isValid())
    {
        CtrConfig::setValueByName(QString("styles.roomdefaultColor_RGB"), "#A0A0A0");
        _selCol = CtrConfig::getValueByName(QString("styles.roomdefaultColor_RGB"));
    }
    QString  strCol = _selCol.toString();
    strCol = strCol.remove("#");
    int r = strCol.mid(0,2).toInt(0,16);
    int g = strCol.mid(2,2).toInt(0,16);
    int b = strCol.mid(4,2).toInt(0,16);
    _roomDefaultColor = QColor(r,g,b);
}

PixelWorkState::~PixelWorkState()
{
}

void PixelWorkState::statePushedToStack()
{
    for(auto it = _itemsOnFloor.begin(); it != _itemsOnFloor.end(); ++it)
    {
        ViraGraphicsItem * viraGraphicsItem = dynamic_cast<ViraGraphicsItem*>(it.value());
        if(viraGraphicsItem)
            viraGraphicsItem->setItemEnabled(false);
    }
}

void PixelWorkState::statePoppedFromStack()
{
    setActiveForScene(true);
    for(auto it = _itemsOnFloor.begin(); it != _itemsOnFloor.end(); ++it)
    {
        ViraGraphicsItem * viraGraphicsItem = dynamic_cast<ViraGraphicsItem*>(it.value());
        if(viraGraphicsItem)
            viraGraphicsItem->setItemEnabled(true);
    }
}

void PixelWorkState::init(QGraphicsScene *scene, QGraphicsView *view, const int *zoom, const double *scale, double frameCoef, uint visualizerId)
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

    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::EditAreaGeometryOnPlan, this, SLOT(slotEditAreaGeometry(QVariant)),
                                      qMetaTypeId< quint64 >(),
                                      QString("visualize_system") );

    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::LayerVisibleChanged, this, SLOT(slotSetLayerVisible(QVariant)),
                                      qMetaTypeId< QVariantList >(),
                                      QString("visualize_system") );

    auto mngr = RegionBizManager::instance();
    mngr->subscribeOnCurrentChange(this, SLOT(slotSelectionItemsChanged(uint64_t,uint64_t)));
    mngr->subscribeCenterOn(this, SLOT(slotCenterOn(uint64_t)));
    mngr->subscribeOnChangeEntity(this, SLOT(slotObjectChanged(uint64_t)));
    mngr->subscribeOnAddEntity(this, SLOT(slotAddObject(uint64_t)));
    mngr->subscribeOnDeleteEntity(this, SLOT(slotDeleteObject(uint64_t)));

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addWidget(_upButton);
    hLayout->setAlignment( _upButton, Qt::AlignTop );
    hLayout->addWidget(_currentFacility);
    hLayout->setAlignment( _currentFacility, Qt::AlignTop );
    hLayout->addWidget(_downButton);
    hLayout->setAlignment( _downButton, Qt::AlignTop );
    hLayout->addStretch();
    _view->setLayout(hLayout);

    reinit();
}

void PixelWorkState::reinit()
{
    uint64_t floorId = 0;
    if(_currFloor_id > 0)
    {
        floorId = _currFloor_id;
        _currFloor_id = 0;
    }
    setFloor(floorId);
}

void PixelWorkState::slotObjectChanged(uint64_t id)
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
            auto it = _itemsOnFloor.find(id);
            if(it != _itemsOnFloor.end())
            {
                ViraGraphicsItem * viraGraphicsItem = dynamic_cast<ViraGraphicsItem*>(it.value());
                if(viraGraphicsItem)
                    viraGraphicsItem->reinit();
            }
            else
                slotAddObject(id);
        }
    }
}

void PixelWorkState::slotAddObject(uint64_t id)
{
    MarkPtr markPtr = RegionBizManager::instance()->getMark(id);
    if(markPtr)
    {
        BaseAreaPtr parentPtr = RegionBizManager::instance()->getBaseArea(markPtr->getParentId());
        if(parentPtr)
        {
            switch(parentPtr->getType())
            {
            case BaseArea::AT_FLOOR :
            case BaseArea::AT_ROOM :
                break;
            default:
                return;
            }

            auto it = _itemsOnFloor.find(parentPtr->getId());
            if(it == _itemsOnFloor.end())
                return;



            switch(markPtr->getMarkType())
            {
            case Mark::MT_DEFECT : //if(mark_type_str == QString::fromUtf8("дефект"))
            {
                QPointF center = markPtr->getCenter();
                DefectGraphicsItem * _defectGraphicsItem = new DefectGraphicsItem(markPtr->getId());
                _defectGraphicsItem->setPos(center);
                _scene->addItem(_defectGraphicsItem);
                connect(_defectGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                _itemsOnFloor.insert(markPtr->getId(), _defectGraphicsItem);
            }break;
            case Mark::MT_PHOTO : //else if(mark_type_str == QString::fromUtf8("фотография"))
            {
                QPointF center = markPtr->getCenter();
                FotoGraphicsItem * _photoGraphicsItem = new FotoGraphicsItem(markPtr->getId());
                _photoGraphicsItem->setPos(center);
                _scene->addItem(_photoGraphicsItem);
                connect(_photoGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                _itemsOnFloor.insert(markPtr->getId(), _photoGraphicsItem);
            }break;
            case Mark::MT_PHOTO_3D : //else if(mark_type_str == QString::fromUtf8("панорамная фотография"))
            {
                QPointF center = markPtr->getCenter();
                Foto360GraphicsItem * _photo360GraphicsItem = new Foto360GraphicsItem(markPtr->getId());
                _photo360GraphicsItem->setPos(center);
                _scene->addItem(_photo360GraphicsItem);
                connect(_photo360GraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                _itemsOnFloor.insert(markPtr->getId(), _photo360GraphicsItem);
            }break;
            }
            insertItemToLayer(markPtr->getLayer(), markPtr->getId());
        }
    }
}

void PixelWorkState::insertItemToLayer(regionbiz::LayerPtr ptr, qulonglong itemId)
{
    uint64_t layerId(ptr ? ptr->getId() : 0);
    auto it = _itemsInLayers.find(layerId);
    if(it == _itemsInLayers.end())
    {
        _itemsInLayers.insert(layerId, QList<qulonglong>() << itemId);
    }
    else
    {
        it.value().append(itemId);
    }
}

void PixelWorkState::slotDeleteObject(uint64_t id)
{
    auto markIt = _itemsOnFloor.find(id);
    if(markIt != _itemsOnFloor.end())
    {
        delete markIt.value();
        _itemsOnFloor.erase(markIt);
    }
}

void PixelWorkState::slotBlockGUI(QVariant var)
{
    _blockGUI = var.toBool();
}

void PixelWorkState::slotSetMarkPosition(QVariant var)
{
    quint64 type = var.toUInt();
    if(type > 0)
    {
        uint id = regionbiz::RegionBizManager::instance()->getCurrentEntity();
        if(id == _currFloor_id)
        {
            emit setMarkOnPlan(type, QPolygonF());
            //emit switchOnEditor();
        }
        else
        {
            auto it = _itemsOnFloor.find(id);
            if(it != _itemsOnFloor.end())
            {
                ViraGraphicsItem * viraGraphicsItem = dynamic_cast<ViraGraphicsItem*>(it.value());
                if(viraGraphicsItem)
                {
                    emit setMarkOnPlan(type, viraGraphicsItem->getPolygon());
                    //emit switchOnEditor();
                }
            }
        }
    }
    else
    {
        emit setMarkOnPlan(0, QPolygonF());
    }
}

void PixelWorkState::slotSelectionItemsChanged(uint64_t prev_id, uint64_t curr_id)
{
    if(prev_id > 0)
    {
        auto it = _itemsOnFloor.find(prev_id);
        if(it != _itemsOnFloor.end())
        {
            ViraGraphicsItem * viraGraphicsItem = dynamic_cast<ViraGraphicsItem*>(it.value());
            if(viraGraphicsItem)
                viraGraphicsItem->setItemselected(false);
        }
    }

    if(curr_id > 0)
    {
        BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(curr_id);
        if(ptr)
        {
            uint64_t facilityId(0);
            switch(ptr->getType())
            {
            case BaseArea::AT_REGION :
            case BaseArea::AT_LOCATION :
                return;

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
                if(_currFacilityId != facilityId)
                {
                    _currFacilityId = facilityId;
                    reinit(facilityId);
                    return;
                }
        }

        auto it = _itemsOnFloor.find(curr_id);
        if(it != _itemsOnFloor.end())
        {
            ViraGraphicsItem * viraGraphicsItem = dynamic_cast<ViraGraphicsItem*>(it.value());
            if(viraGraphicsItem)
                viraGraphicsItem->setItemselected(true);
        }
        else
        {
            bool found(false);
            for(auto it = _floorsMap.begin(); it != _floorsMap.end(); ++it)
                if(it.value().id == curr_id)
                {
                    setFloor(curr_id);
                    found = true;
                    break;
                }
            if( ! found)
            {
                BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(curr_id, BaseArea::AT_ROOM);
                RoomPtr roomPtr = BaseArea::convert< Room >(ptr);
                if(roomPtr)
                    for(auto it = _floorsMap.begin(); it != _floorsMap.end(); ++it)
                        if(it.value().id == roomPtr->getParentId())
                        {
                            setFloor(roomPtr->getParentId());
                            break;
                        }
            }
        }
    }
}

void PixelWorkState::reinit(qulonglong facilityId)
{
    foreach(QGraphicsItem * item, _itemsOnFloor)
        delete item;
    _itemsOnFloor.clear();

    _floorsMap.clear();

    _upButton->setDisabled(true);
    _downButton->setDisabled(true);
    _currentFacility->clear();

    if(facilityId != 0)
    {
        BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(facilityId, BaseArea::AT_FACILITY);
        FacilityPtr facilityPtr = BaseArea::convert< Facility >(ptr);
        if(facilityPtr)
        {
            FloorPtrs floors = facilityPtr->getChilds();
            for( FloorPtr floorPtr: floors )
            {
                FloorInfo floorInfo;
                floorInfo.name = floorPtr->getName();
                floorInfo.id = floorPtr->getId();
                _floorsMap.insert(floorPtr->getNumber(), floorInfo);
            }
            if(_floorsMap.isEmpty() == false)
                setFloor(_floorsMap.begin().value().id);
        }
    }
}

void PixelWorkState::setFloor(qulonglong floorId)
{
    if(_currFloor_id == floorId)
        return;

    _currFloor_id = 0;

    foreach(QGraphicsItem * item, _itemsOnFloor.values())
        delete item;
    _itemsOnFloor.clear();
    _itemsInLayers.clear();

    // NOTE file load path
    QString destPath;
    QVariant regionBizInitJson_Path = CtrConfig::getValueByName("application_settings.regionBizFilesPath",
                                                                "./data", true);
    if(regionBizInitJson_Path.isValid())
         destPath = regionBizInitJson_Path.toString() + QDir::separator();

    BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(floorId, BaseArea::AT_FLOOR);
    FloorPtr floorPtr = BaseArea::convert< Floor >(ptr);
    if(floorPtr)
    {
        _currFloor_id = floorId;

        _currentFacility->setText(floorPtr->getName());
        int i(0);
        for(auto it = _floorsMap.begin(); it != _floorsMap.end(); ++it)
        {
            if(it.value().id == _currFloor_id)
            {
                _upButton->setDisabled(i == _floorsMap.size()-1);
                _downButton->setDisabled(i == 0);
                break;
            }
            ++i;
        }

        MarkPtrs marks_of_floor = floorPtr->getMarks();
        for( MarkPtr mark: marks_of_floor )
            if(markInArchive(mark) == false)
                slotAddObject(mark->getId());

        RoomPtrs rooms = floorPtr->getChilds();
        for( BaseAreaPtr room_ptr: rooms )
        {
            RoomPtr room = BaseArea::convert< Room >( room_ptr );
            if(room)
            {
                QColor roomColor(_roomDefaultColor);
                BaseMetadataPtr statusPtr = room->getMetadata("status");
                if(statusPtr)
                {
                    QString status = statusPtr->getValueAsVariant().toString();
                    if(status == QString::fromUtf8("Свободно"))
                        roomColor = QColor(86,206,18);
                    else if(status == QString::fromUtf8("В аренде"))
                        roomColor = QColor(226,224,111);
                    else
                        roomColor = QColor(_roomDefaultColor);
                }
                AreaInitData roomInitData(true, 10000, roomColor);

                AreaGraphicsItem * areaGraphicsItem = new AreaGraphicsItem(room->getCoords());
                areaGraphicsItem->setAcceptHoverEvents(true);
                roomInitData.id = room->getId();
                areaGraphicsItem->init(roomInitData);
                _scene->addItem(areaGraphicsItem);
                connect(areaGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                _itemsOnFloor.insert(room->getId(), areaGraphicsItem);

                MarkPtrs marks_of_room = room->getMarks();
                for( MarkPtr mark: marks_of_room )
                    if(markInArchive(mark) == false)
                        slotAddObject(mark->getId());
            }
        }

//        qDebug() << "-------------------------------";
//        for(auto it = _itemsInLayers.begin(); it != _itemsInLayers.end(); ++it)
//        {
//            qDebug() << "Layer Id :" << it.key() << ", all marks :" << it.value();
//        }
//        qDebug() << "-------------------------------";
    }
}

void PixelWorkState::centerViewOn(QPointF pos)
{
//    QRectF sceneContentsRect(_view->mapToScene(_view->contentsRect().topLeft()), _view->mapToScene(_view->contentsRect().bottomRight()));
//    if(sceneContentsRect.contains(pos) == false)
//        _view->centerOn(pos);
}

void PixelWorkState::slotCenterOn(uint64_t id)
{
//    BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(id);
//    if( ! ptr)
//        return;

//    uint64_t facilityId(0);
//    auto it = _items.find(id);
//    switch(ptr->getType())
//    {
//    case BaseArea::AT_REGION :
//    case BaseArea::AT_LOCATION :
//    {
//        if(it != _items.end())
//        {
//            QPolygonF pol = it.value()->getPolygon();
//            setPrefferZoomForSceneRect(pol.boundingRect());
//            emit switchOnMap();
//        }
//    }break;

//    case BaseArea::AT_FACILITY :
//    {
//        if(it != _items.end())
//        {
//            BaseAreaPtr parentPtr = ptr->getParent();
//            if(parentPtr)
//                if(parentPtr->getType() == BaseArea::AT_LOCATION)
//                {
//                    auto parentIt = _items.find(parentPtr->getId());
//                    if(parentIt != _items.end())
//                    {
//                        QPolygonF pol = parentIt.value()->getPolygon();
//                        setPrefferZoomForSceneRect(pol.boundingRect());
//                    }
//                }

//            QPolygonF pol = it.value()->getPolygon();
//            _view->centerOn(pol.boundingRect().center());
//            facilityId = id;
//        }
//    }break;

//    case BaseArea::AT_FLOOR :
//    case BaseArea::AT_ROOM :
//    {
//        BaseAreaPtr parentPtr = ptr->getParent();
//        while(parentPtr)
//        {
//            if(parentPtr->getType() == BaseArea::AT_FACILITY)
//            {
//                facilityId = parentPtr->getId();
//                break;
//            }
//            parentPtr = parentPtr->getParent();
//        }
//    }break;

//    }

//    if(facilityId > 0)
//    {
//        emit showFacility(facilityId);
//        if(ptr->getType() != BaseArea::AT_FACILITY)
//            emit centerEditorOn(id);
//    }
}

void PixelWorkState::zoomChanged()
{
}

void PixelWorkState::areaGeometryEdited(QPolygonF pol)
{
    if(_editObjectGeometry > 0)
    {
        BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(_editObjectGeometry);
        if(ptr)
        {
            QColor roomColor(_roomDefaultColor);
            BaseMetadataPtr statusPtr = ptr->getMetadata("status");
            if(statusPtr)
            {
                QString status = statusPtr->getValueAsVariant().toString();
                if(status == QString::fromUtf8("Свободно"))
                    roomColor = QColor(86,206,18);
                else if(status == QString::fromUtf8("В аренде"))
                    roomColor = QColor(226,224,111);
                else
                    roomColor = QColor(_roomDefaultColor);
            }

            auto it = _itemsOnFloor.find(_editObjectGeometry);
            if(it != _itemsOnFloor.end())
            {
                AreaGraphicsItem * areaGraphicsItem = dynamic_cast<AreaGraphicsItem*>(it.value());
                if(areaGraphicsItem)
                {
                    if(pol.isEmpty() == false)
                    {
                        areaGraphicsItem->setPolygon(pol);
                        areaGraphicsItem->setColor(roomColor);
                    }
                    areaGraphicsItem->show();
                }
            }
            else if(pol.isEmpty() == false)
            {
                AreaInitData roomInitData(true, 10000, roomColor);

                AreaGraphicsItem * areaGraphicsItem = new AreaGraphicsItem(pol);
                areaGraphicsItem->setAcceptHoverEvents(true);
                roomInitData.id = ptr->getId();
                areaGraphicsItem->init(roomInitData);
                _scene->addItem(areaGraphicsItem);
                connect(areaGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                _itemsOnFloor.insert(ptr->getId(), areaGraphicsItem);
            }

            if(pol.isEmpty() == false)
            {
                ptr->setCoords(pol);
                ptr->commit();
            }

            quint64 id(_editObjectGeometry);
            CommonMessageNotifier::send( (uint)visualize_system::BusTags::EditModeFinish, QVariant(id), QString("visualize_system"));
        }
    }
    _editObjectGeometry = 0;
}

void PixelWorkState::slotSetItemselect(qulonglong id, bool on_off)
{
//    auto it = _itemId_facilityFolder.find(id);
//    if(it != _itemId_facilityFolder.end())
//    {
//        it.value()->setItemselect(on_off);
//    }

}

bool PixelWorkState::mousePressEvent(QMouseEvent* e, QPointF scenePos)
{
//    qDebug() << "mousePressEvent :" << QString::number(scenePos.x(), 'f', 8) << QString::number(scenePos.y(), 'f', 8);
    return ScrollBaseState::mousePressEvent(e, scenePos);
}

bool PixelWorkState::mouseDoubleClickEvent(QMouseEvent *e, QPointF scenePos)
{
    QList<QGraphicsItem*> _items = _scene->items(scenePos);
    foreach(QGraphicsItem* item, _items)
    {
        ViraGraphicsItem * viraGraphicsItem = dynamic_cast<ViraGraphicsItem*>(item);
        if(viraGraphicsItem)
        {
//! !!! pereosmislit eto mesto !!!
//            regionbiz::RegionBizManager::instance()->centerOnEntity(viraGraphicsItem->getId());
            return true;
        }
    }

    regionbiz::RegionBizManager::instance()->setCurrentEntity(_currFloor_id);
    return true;
}

bool PixelWorkState::markInArchive(regionbiz::MarkPtr markPtr)
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

void PixelWorkState::slotFloorUp()
{
//    if(_mode != ScrollMode)
//        return;

    int i(0);
    for(auto it = _floorsMap.begin(); it != _floorsMap.end(); ++it)
    {
        if(it.value().id == _currFloor_id)
        {
            ++it;
            if(it != _floorsMap.end())
                RegionBizManager::instance()->setCurrentEntity(it.value().id);
            break;
        }
        ++i;
    }
}

void PixelWorkState::slotFloorDown()
{
//    if(_mode != ScrollMode)
//        return;

    int i(0);
    for(auto it = _floorsMap.begin(); it != _floorsMap.end(); ++it)
    {
        if(it.value().id == _currFloor_id)
        {
            if(it != _floorsMap.begin())
            {
                --it;
                RegionBizManager::instance()->setCurrentEntity(it.value().id);
            }
            break;
        }
        ++i;
    }
}

void PixelWorkState::slotSelectItem(qulonglong id, bool centerOnEntity)
{
    if(_blockGUI == false)
    {
        regionbiz::RegionBizManager::instance()->setCurrentEntity(id);
//! !!! pereosmislit eto mesto !!!
        if(centerOnEntity)
            regionbiz::RegionBizManager::instance()->centerOnEntity(id);
    }
}

void PixelWorkState::slotEditAreaGeometry(QVariant var)
{
    uint id = var.toUInt();

    if(id > 0)
    {
        _editObjectGeometry = id;
        auto it = _itemsOnFloor.find(_editObjectGeometry);
        if(it != _itemsOnFloor.end())
        {
            AreaGraphicsItem * areaGraphicsItem = dynamic_cast<AreaGraphicsItem*>(it.value());
            if(areaGraphicsItem)
                areaGraphicsItem->hide();
        }
        emit editAreaGeometry(true);
    }
    else
    {
        auto it = _itemsOnFloor.find(_editObjectGeometry);
        if(it != _itemsOnFloor.end())
        {
            AreaGraphicsItem * areaGraphicsItem = dynamic_cast<AreaGraphicsItem*>(it.value());
            if(areaGraphicsItem)
                areaGraphicsItem->show();
        }
        _editObjectGeometry = 0;
        emit editAreaGeometry(false);
    }
}

void PixelWorkState::slotSetLayerVisible(QVariant var)
{
    QList<QVariant>list = var.toList();
    if(list.size() != 3)
        return;

    uint64_t layerId(list.at(0).toUInt());
    uint markType(list.at(1).toUInt());
    bool on_off(list.at(2).toBool());
    auto layerIt = _itemsInLayers.find(layerId);
    if(layerIt != _itemsInLayers.end())
    {
        foreach(qulonglong itemId, layerIt.value())
        {
            auto itemIt = _itemsOnFloor.find(itemId);
            if(itemIt != _itemsOnFloor.end())
            {
                ViraGraphicsItem * viraGraphicsItem = dynamic_cast<ViraGraphicsItem*>(itemIt.value());
                if(viraGraphicsItem)
                    switch(markType)
                    {
                    case 0 : {
                        viraGraphicsItem->setItemVisible(on_off);
                    }break;
                    case 1 : {
                        if(viraGraphicsItem->getItemType() == ViraGraphicsItem::ItemType_Defect)
                            viraGraphicsItem->setItemVisible(on_off);
                    }break;
                    case 2 : {
                        if(viraGraphicsItem->getItemType() == ViraGraphicsItem::ItemType_Foto)
                            viraGraphicsItem->setItemVisible(on_off);
                    }break;
                    case 3 : {
                        if(viraGraphicsItem->getItemType() == ViraGraphicsItem::ItemType_Foto360)
                            viraGraphicsItem->setItemVisible(on_off);
                    }break;
                    }
            }
        }
    }
}










