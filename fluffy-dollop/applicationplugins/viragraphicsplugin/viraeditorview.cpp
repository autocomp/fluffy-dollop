#include "viraeditorview.h"
#include "areagraphicsitem.h"
#include "markgraphicsitem.h"
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDir>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <regionbiz/rb_manager.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>
#include "viragraphicsitem.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>

using namespace regionbiz;


ViraEditorView::ViraEditorView()
{
    setRenderHint(QPainter::Antialiasing, true);
    setRenderHint(QPainter::HighQualityAntialiasing, true);
    setRenderHint(QPainter::SmoothPixmapTransform, true);

    setMouseTracking(true);
    setDragMode(QGraphicsView::NoDrag);
    setCursor(QCursor(Qt::ArrowCursor));

    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::EditAreaGeometry, this, SLOT(slotEditAreaGeometry(QVariant)),
                                      qMetaTypeId< quint64 >(),
                                      QString("visualize_system") );

    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::SetMarkPosition, this, SLOT(slotSetMarkPosition(QVariant)),
                                      qMetaTypeId< quint64 >(),
                                      QString("visualize_system") );

    auto mngr = RegionBizManager::instance();
    mngr->subscribeOnChangeEntity(this, SLOT(slotObjectChanged(uint64_t)));
    mngr->subscribeOnAddEntity(this, SLOT(slotAddObject(uint64_t)));
    mngr->subscribeOnDeleteEntity(this, SLOT(slotDeleteObject(uint64_t)));

    _layersMenu = new LayersMenu();
    connect(_layersMenu, SIGNAL(rastersVisibleChanged()), this, SLOT(slotRastersVisibleChanged()));
    connect(_layersMenu, SIGNAL(getNeedVisibleRasters(bool&,bool&,bool&,bool&,bool&,bool&,bool&)),
            this, SLOT(slotGetNeedVisibleRasters(bool&,bool&,bool&,bool&,bool&,bool&,bool&)));
    _layersMenu->setAlignment(Qt::AlignCenter);
    _layersMenu->setStyleSheet("border-radius:4;border-color: rgb(255, 255, 255);");
    _layersMenu->setMinimumSize(75, 30);

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

    QHBoxLayout* hLayout = new QHBoxLayout;


    hLayout->addWidget(_layersMenu);
    hLayout->setAlignment(_layersMenu, Qt::AlignTop);
    //hLayout->addSpacerItem(new QSpacerItem(20, 0, QSizePolicy::Fixed, QSizePolicy::Fixed));

    hLayout->addStretch();
    hLayout->addWidget(_upButton);
    hLayout->setAlignment( _upButton, Qt::AlignTop );
    hLayout->addWidget(_currentFacility);
    hLayout->setAlignment( _currentFacility, Qt::AlignTop );
    hLayout->addWidget(_downButton);
    hLayout->setAlignment( _downButton, Qt::AlignTop );
    hLayout->addStretch();
    setLayout(hLayout);

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

ViraEditorView::~ViraEditorView()
{
    // reinit();
}

void ViraEditorView::reinit(qulonglong facilityId)
{
//    foreach(GraphicsPixmapItem * item, _owerViews)
//        delete item;
//    _owerViews.clear();
    delete _baseRasterItem;
    _baseRasterItem = nullptr;
    delete _axisRasterItem;
    _axisRasterItem = nullptr;
    delete _sizesRasterItem;
    _sizesRasterItem = nullptr;
    delete _waterDisposalRasterItem;
    _waterDisposalRasterItem = nullptr;
    delete _waterSupplyRasterItem;
    _waterSupplyRasterItem = nullptr;
    delete _heatingRasterItem;
    _heatingRasterItem = nullptr;
    delete _electricityRasterItem;
    _electricityRasterItem = nullptr;
    delete _doorsRasterItem;
    _doorsRasterItem = nullptr;

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

void ViraEditorView::setFloor(qulonglong floorId)
{
    if(_currFloor_id == floorId)
        return;

    _currFloor_id = 0;

    clearTempItems();

//    foreach(GraphicsPixmapItem * item, _owerViews)
//        delete item;
//    _owerViews.clear();
    delete _baseRasterItem;
    _baseRasterItem = nullptr;
    delete _axisRasterItem;
    _axisRasterItem = nullptr;
    delete _sizesRasterItem;
    _sizesRasterItem = nullptr;
    delete _waterDisposalRasterItem;
    _waterDisposalRasterItem = nullptr;
    delete _waterSupplyRasterItem;
    _waterSupplyRasterItem = nullptr;
    delete _heatingRasterItem;
    _heatingRasterItem = nullptr;
    delete _electricityRasterItem;
    _electricityRasterItem = nullptr;
    delete _doorsRasterItem;
    _doorsRasterItem = nullptr;


    foreach(QGraphicsItem * item, _itemsOnFloor.values())
        delete item;
    _itemsOnFloor.clear();

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

//        QString pixmapPath = destPath + QString::number(floorId) + QDir::separator() + QString("area.tiff"); // floorPtr->getPlanPath();
//        QPixmap pm(pixmapPath);
//        GraphicsPixmapItem* item = new GraphicsPixmapItem(pm);
//        scene()->addItem(item);
//        setSceneRect(0, 0, pm.width(), pm.height());
//        _owerViews.append(item);
        QString path = destPath + QString::number(floorId) + QDir::separator();
        {
            QString filePath;
            if(QFile::exists(path + QString("area.tiff")))
                filePath = path + QString("area.tiff");
            else if(QFile::exists(path + QString("base.tiff")))
                filePath = path + QString("base.tiff");

            if(filePath.isEmpty() == false)
            {
                QPixmap pm(filePath);
                _baseRasterItem = new GraphicsPixmapItem(pm);
                scene()->addItem(_baseRasterItem);
                setSceneRect(0, 0, pm.width(), pm.height());
            }
        }
        if(QFile::exists(path + QString("axis.tiff")))
        {
            QPixmap pm(path + QString("axis.tiff"));
            _axisRasterItem = new GraphicsPixmapItem(pm);
            scene()->addItem(_axisRasterItem);
        }
        if(QFile::exists(path + QString("sizes.tiff")))
        {
            QPixmap pm(path + QString("sizes.tiff"));
            _sizesRasterItem = new GraphicsPixmapItem(pm);
            scene()->addItem(_sizesRasterItem);
        }
        if(QFile::exists(path + QString("water_disposal.tiff")))
        {
            QPixmap pm(path + QString("water_disposal.tiff"));
            _waterDisposalRasterItem = new GraphicsPixmapItem(pm);
            scene()->addItem(_waterDisposalRasterItem);
        }
        if(QFile::exists(path + QString("water_supply.tiff")))
        {
            QPixmap pm(path + QString("water_supply.tiff"));
            _waterSupplyRasterItem = new GraphicsPixmapItem(pm);
            scene()->addItem(_waterSupplyRasterItem);
        }
        if(QFile::exists(path + QString("heating.tiff")))
        {
            QPixmap pm(path + QString("heating.tiff"));
            _heatingRasterItem = new GraphicsPixmapItem(pm);
            scene()->addItem(_heatingRasterItem);
        }
        if(QFile::exists(path + QString("electricity.tiff")))
        {
            QPixmap pm(path + QString("electricity.tiff"));
            _electricityRasterItem = new GraphicsPixmapItem(pm);
            scene()->addItem(_electricityRasterItem);
        }
        if(QFile::exists(path + QString("doors.tiff")))
        {
            QPixmap pm(path + QString("doors.tiff"));
            _doorsRasterItem = new GraphicsPixmapItem(pm);
            scene()->addItem(_doorsRasterItem);
        }

        MarkPtrs marks_of_floor = floorPtr->getMarks();
        for( MarkPtr mark: marks_of_floor )
            if(markInArchive(mark) == false)
            {
                QPointF center = mark->getCenter();
                MarkGraphicsItem * _markGraphicsItem = new MarkGraphicsItem(mark->getId());
                _markGraphicsItem->setPos(center);

                scene()->addItem(_markGraphicsItem);
                connect(_markGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                _itemsOnFloor.insert(mark->getId(), _markGraphicsItem);
            }

        RoomPtrs rooms = floorPtr->getChilds();
        for( BaseAreaPtr room_ptr: rooms )
        {
            RoomPtr room = BaseArea::convert< Room >( room_ptr );
            if(room)
            {
                MarkPtrs marks_of_room = room->getMarks();
                for( MarkPtr mark: marks_of_room )
                    if(markInArchive(mark) == false)
                    {
                        // quint64 id = mark->getId();
                        // bool res = RegionBizManager::instance()->deleteMark(id);
                        // qDebug() << "===> mark" << id << "res" << res;

                        QPointF center = mark->getCenter();
                        MarkGraphicsItem * _markGraphicsItem = new MarkGraphicsItem(mark->getId());
                        _markGraphicsItem->setPos(center);

                        scene()->addItem(_markGraphicsItem);
                        connect(_markGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                        _itemsOnFloor.insert(mark->getId(), _markGraphicsItem);
                    }

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
                scene()->addItem(areaGraphicsItem);
                connect(areaGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                _itemsOnFloor.insert(room->getId(), areaGraphicsItem);
            }
        }
    }

    slotRastersVisibleChanged();

    /*
    reinit();
    if(pageNumber < 0)
        return;

    const pdf_editor::Floor& floor(_facility.floors.at(pageNumber));
    QMap<double, GraphicsPixmapItem*> zValue_item;
    foreach(pdf_editor::OwerView owerView, floor.owerViews)
    {
        QString fileName = _path + QDir::separator() + floor.folder + QDir::separator() + owerView.file;
        QPixmap pm(fileName);
        GraphicsPixmapItem* item = new GraphicsPixmapItem(pm);
        scene()->addItem(item);
        double zValue = 1. / owerView.scale;
        item->setZValue(zValue);
        if(owerView.isOriginal)
            setSceneRect(0, 0, pm.width(), pm.height());
        else
            item->setTransform(QTransform().scale(owerView.scale, owerView.scale));
        zValue_item.insert(zValue, item);
    }
    for(auto it = zValue_item.begin(); it != zValue_item.end(); ++it)
        _owerViews.append(it.value());

    if(_recalcZoom)
    {
        zoomReset();
        _zoomMin = 1 - (_owerViews.size() - 1);
        _zoomMax = 1;

        while(_zoom > _zoomMin+1)
            zoomOut();
    }
    _recalcZoom = false;

    foreach(pdf_editor::Room room, floor.rooms)
    {
        RoomGraphicstem * item = new RoomGraphicstem(room);
        scene()->addItem(item);
        _itemsOnFloor.append(item);
    }

    syncItems();
    */
}

void ViraEditorView::selectViraItem(qulonglong id, bool centerOnEntity)
{
    if(_mode == ScrollMode)
    {
        regionbiz::RegionBizManager::instance()->selectEntity(id);
        if(centerOnEntity)
            regionbiz::RegionBizManager::instance()->centerOnEntity(id);
    }
}

void ViraEditorView::slotSelectItem(qulonglong id, bool centerOnEntity)
{
    if(_mode == ScrollMode)
    {
        regionbiz::RegionBizManager::instance()->selectEntity(id);
        if(centerOnEntity)
            regionbiz::RegionBizManager::instance()->centerOnEntity(id);
    }
}

void ViraEditorView::selectionItemsChanged(uint64_t prev_id, uint64_t curr_id)
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

//            auto it = _floorsMap.find(curr_id);
//            if(it != _floorsMap.end())
//            {
//                setFloor(curr_id);
//            }
//            else
//            {
//                BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(curr_id, BaseArea::AT_ROOM);
//                RoomPtr roomPtr = BaseArea::convert< Room >(ptr);
//                if(roomPtr)
//                {
//                    auto it = _floorsMap.find(roomPtr->getParentId());
//                    if(it != _floorsMap.end())
//                        setFloor(it.key());
//                }
//            }
        }
    }
}

void ViraEditorView::slotEditAreaGeometry(QVariant var)
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
        _mode = EditAreaMode;
        setCursor(QCursor(QPixmap(":/img/cursor_polygon.png"), 0, 0));
    }
    else
    {
        clearTempItems();
        auto it = _itemsOnFloor.find(_editObjectGeometry);
        if(it != _itemsOnFloor.end())
        {
            AreaGraphicsItem * areaGraphicsItem = dynamic_cast<AreaGraphicsItem*>(it.value());
            if(areaGraphicsItem)
                areaGraphicsItem->show();
        }
        _editObjectGeometry = 0;
        _mode = ScrollMode;
        setCursor(QCursor(Qt::ArrowCursor));
    }
}

void ViraEditorView::slotSetMarkPosition(QVariant var)
{
    _editObjectGeometry = var.toUInt();
    if(_editObjectGeometry > 0)
    {
        if(_editObjectGeometry == _currFloor_id)
        {
            _editObjectExtend.clear();
            setCursor(QCursor(QPixmap(":/img/cursor_mark.png"), 0, 0));
            _mode = EditMarkMode;
        }
        else
        {
            auto room_ptr = RegionBizManager::instance()->getBaseArea( _editObjectGeometry );
            auto room = BaseArea::convert<Room>(room_ptr);
            if(room)
            {
                _editObjectExtend = room->getCoords();
                setCursor(QCursor(QPixmap(":/img/cursor_mark.png"), 0, 0));
                _mode = EditMarkMode;
            }
            else
            {
                _editObjectGeometry = 0;
            }
        }
    }
    else
    {
        setCursor(QCursor(Qt::ArrowCursor));
        _mode = ScrollMode;
    }
}

void ViraEditorView::slotObjectChanged(uint64_t id)
{
    auto it = _itemsOnFloor.find(id);
    if(it != _itemsOnFloor.end())
    {
        AreaGraphicsItem * areaItem = dynamic_cast<AreaGraphicsItem*>(it.value());
        if(areaItem)
        {
            BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(id, BaseArea::AT_ROOM);
            RoomPtr roomPtr = BaseArea::convert< Room >(ptr);
            if(roomPtr)
            {
                QColor roomColor(_roomDefaultColor);
                BaseMetadataPtr statusPtr = roomPtr->getMetadata("status");
                if(statusPtr)
                {
                    QString status = statusPtr->getValueAsVariant().toString();
                    if(status == QString::fromUtf8("Свободно"))
                        roomColor = QColor(86,206,18);
                    else if(status == QString::fromUtf8("В аренде"))
                        roomColor = QColor(226,224,111);
                    else
                        roomColor = QColor(_roomDefaultColor);

                    areaItem->setColor(roomColor);
                }
            }
        }
        else
        {
            MarkGraphicsItem * markItem = dynamic_cast<MarkGraphicsItem*>(it.value());
            if(markItem)
            {
                MarkPtr markPtr = RegionBizManager::instance()->getMark(id);
                if(markInArchive(markPtr))
                {
                    _itemsOnFloor.erase(it);
                    delete markItem;
                    return;
                }
                markItem->reinit();
            }
        }
    }
}

bool ViraEditorView::markInArchive(regionbiz::MarkPtr markPtr)
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

void ViraEditorView::slotAddObject(uint64_t id)
{
    MarkPtr markPtr = RegionBizManager::instance()->getMark(id);
    if( ! markPtr)
        return;

    BaseAreaPtr baseAreaPtr = RegionBizManager::instance()->getBaseArea(markPtr->getParentId());
    if( ! baseAreaPtr)
        return;

    bool addMark(false);
    switch(baseAreaPtr->getType())
    {
    case BaseArea::AT_FLOOR :
    {
        if(_currFloor_id == baseAreaPtr->getId())
            addMark = true;
    }break;
    case BaseArea::AT_ROOM :
    {
        if(_currFloor_id == baseAreaPtr->getParentId())
            addMark = true;
    }break;
    }

    if(addMark)
    {
        QPointF center = markPtr->getCenter();
        MarkGraphicsItem * _markGraphicsItem = new MarkGraphicsItem(markPtr->getId());
        _markGraphicsItem->setPos(center);

        scene()->addItem(_markGraphicsItem);
        connect(_markGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
        _itemsOnFloor.insert(markPtr->getId(), _markGraphicsItem);
    }
}

void ViraEditorView::slotDeleteObject(uint64_t id)
{
    auto it = _itemsOnFloor.find(id);
    if(it != _itemsOnFloor.end())
    {
        MarkGraphicsItem * markItem = dynamic_cast<MarkGraphicsItem*>(it.value());
        if(markItem)
        {
            delete markItem;
            _itemsOnFloor.erase(it);
        }
    }
}

void ViraEditorView::slotFloorUp()
{
    if(_mode != ScrollMode)
        return;

    int i(0);
    for(auto it = _floorsMap.begin(); it != _floorsMap.end(); ++it)
    {
        if(it.value().id == _currFloor_id)
        {
            ++it;
            if(it != _floorsMap.end())
                RegionBizManager::instance()->selectEntity(it.value().id);
            break;
        }
        ++i;
    }
}

void ViraEditorView::slotFloorDown()
{
    if(_mode != ScrollMode)
        return;

    int i(0);
    for(auto it = _floorsMap.begin(); it != _floorsMap.end(); ++it)
    {
        if(it.value().id == _currFloor_id)
        {
            if(it != _floorsMap.begin())
            {
                --it;
                RegionBizManager::instance()->selectEntity(it.value().id);
            }
            break;
        }
        ++i;
    }
}

void ViraEditorView::wheelEvent(QWheelEvent *e)
{
    if(e->delta() > 0)
        zoomIn();
    else
        zoomOut();
}

void ViraEditorView::mouseMoveEvent(QMouseEvent* e)
{
    QPointF scenePos = mapToScene(e->pos());
    if(_mode == EditAreaMode)
    {
        if(_lines.size() >= 2)
        {
            QPointF scenePos = mapToScene(e->pos());
            _lines.first()->setLine(QLineF(scenePos, _lines.first()->line().p2()));
            _lines.last()->setLine(QLineF(_lines.last()->line().p1(), scenePos));
        }
    }
    else if(_mode == EditMarkMode)
    {

        if(_editObjectExtend.isEmpty() || _editObjectExtend.containsPoint(scenePos, Qt::OddEvenFill))
            setCursor(QCursor(QPixmap(":/img/cursor_mark.png"), 0, 0)); // setCursor(QCursor(Qt::ArrowCursor));
        else
            setCursor(QCursor(Qt::ForbiddenCursor));

        if(_lines.size() >= 2)
        {
            QPointF scenePos = mapToScene(e->pos());
            _lines.first()->setLine(QLineF(scenePos, _lines.first()->line().p2()));
            _lines.last()->setLine(QLineF(_lines.last()->line().p1(), scenePos));
        }
    }
    _lastMouseScenePos = scenePos;
    QGraphicsView::mouseMoveEvent(e);
}

void ViraEditorView::mousePressEvent(QMouseEvent *e)
{
    if(e->button() & Qt::LeftButton)
    {
        QPointF scenePos = mapToScene(e->pos());

        if(_mode == EditAreaMode)
        {
            QPen pen(Qt::red);
            pen.setCosmetic(true);
            pen.setWidth(1);
            if(_lines.isEmpty())
            {
                QGraphicsLineItem * line = new QGraphicsLineItem();
                scene()->addItem(line);
                line->setZValue(1000);
                line->setPen(pen);
                line->setLine(QLineF(scenePos, scenePos));
                _lines.append(line);

                line = new QGraphicsLineItem();
                scene()->addItem(line);
                line->setZValue(1000);
                line->setPen(pen);
                line->setLine(QLineF(scenePos, scenePos));
                _lines.append(line);
            }
            else
            {
                QGraphicsLineItem * line = new QGraphicsLineItem();
                scene()->addItem(line);
                line->setZValue(1000);
                line->setPen(pen);
                line->setLine(QLineF(scenePos, scenePos));
                _lines.append(line);
            }
        }
        else if(_mode == EditMarkMode)
        {
            if(_editObjectExtend.isEmpty() || _editObjectExtend.containsPoint(scenePos, Qt::OddEvenFill))
            {
                QPen pen(Qt::blue);
                pen.setCosmetic(true);
                pen.setWidth(1);
                if(_lines.isEmpty())
                {
                    QGraphicsLineItem * line = new QGraphicsLineItem();
                    scene()->addItem(line);
                    line->setZValue(1000);
                    line->setPen(pen);
                    line->setLine(QLineF(scenePos, scenePos));
                    _lines.append(line);

                    line = new QGraphicsLineItem();
                    scene()->addItem(line);
                    line->setZValue(1000);
                    line->setPen(pen);
                    line->setLine(QLineF(scenePos, scenePos));
                    _lines.append(line);
                }
                else
                {
                    QGraphicsLineItem * line = new QGraphicsLineItem();
                    scene()->addItem(line);
                    line->setZValue(1000);
                    line->setPen(pen);
                    line->setLine(QLineF(scenePos, scenePos));
                    _lines.append(line);
                }


//                auto room_ptr = RegionBizManager::instance()->getBaseArea( _editObjectGeometry );
//                auto room = BaseArea::convert<Room>(room_ptr);
//                MarkPtr markPtr = room->addMark( scenePos );
//                markPtr->setName(QString::fromUtf8("дефект"));
//                bool res = markPtr->commit();

//                MarkGraphicsItem * _markGraphicsItem = new MarkGraphicsItem(markPtr->getId());
//                _markGraphicsItem->setPos(scenePos);
//                scene()->addItem(_markGraphicsItem);
//                connect(_markGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
//                _itemsOnFloor.insert(markPtr->getId(), _markGraphicsItem);

//                _editObjectExtend.clear();
//                _editObjectGeometry = 0;
//                _mode = ScrollMode;
//                setCursor(QCursor(Qt::ArrowCursor));

//                quint64 markId(markPtr->getId());
//                CommonMessageNotifier::send( (uint)visualize_system::BusTags::EditModeFinish, QVariant(markId), QString("visualize_system"));
            }
        }
        else
        {
            QList<QGraphicsItem*> _items = items(e->pos());
            foreach(QGraphicsItem* item, _items)
            {
                ViraGraphicsItem * viraGraphicsItem = dynamic_cast<ViraGraphicsItem*>(item);
                if(viraGraphicsItem)
                {
                    regionbiz::RegionBizManager::instance()->selectEntity(viraGraphicsItem->getId());
                    return;
                }
            }

            setDragMode(QGraphicsView::ScrollHandDrag);
            QGraphicsView::mousePressEvent(e);
        }
    }



    if(e->button() & Qt::MiddleButton)
    {
        zoomReset();
    }

    //    if(e->button() & Qt::RightButton)
    //    {
    //        qDebug() << "mousePressEvent:" << mapToScene(e->pos());
    //    }
}

void ViraEditorView::mouseReleaseEvent(QMouseEvent *e)
{
    QGraphicsView::mouseReleaseEvent(e);
    setDragMode(QGraphicsView::NoDrag);
    setCursor(QCursor(Qt::ArrowCursor));
}

void ViraEditorView::mouseDoubleClickEvent(QMouseEvent *e)
{
    if(e->button() & Qt::LeftButton)
    {
        QPointF scenePos = mapToScene(e->pos());
        if(_mode == EditAreaMode)
        {
            if(_lines.size() > 2)
            {
                QPolygonF pol;
                for(int i(0); i < _lines.size()-1; ++i)
                {
                    QGraphicsLineItem * item = _lines.at(i);
                    QLineF line = item->line();
                    pol.append(line.p1());
                }

                BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(_editObjectGeometry, BaseArea::AT_ROOM);
                RoomPtr room = BaseArea::convert< Room >(ptr);
                if(room)
                {
                    room->setCoords(pol);
                    room->commit();

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

                    AreaGraphicsItem * areaGraphicsItem = new AreaGraphicsItem(pol);
                    areaGraphicsItem->setAcceptHoverEvents(true);
                    roomInitData.id = room->getId();
                    areaGraphicsItem->init(roomInitData);
                    scene()->addItem(areaGraphicsItem);
                    connect(areaGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                    _itemsOnFloor.insert(room->getId(), areaGraphicsItem);

                    areaGraphicsItem->setItemselected(true);

                    _editObjectGeometry = 0;
                    _mode = ScrollMode;
                    setCursor(QCursor(Qt::ArrowCursor));

                    quint64 roomId(room->getId());
                    CommonMessageNotifier::send( (uint)visualize_system::BusTags::EditModeFinish, QVariant(roomId), QString("visualize_system"));
                }

                clearTempItems();
            }
        }
        else if(_mode == EditMarkMode)
        {
            if(_editObjectExtend.isEmpty() || _editObjectExtend.containsPoint(scenePos, Qt::OddEvenFill))
            {
                finishCreateMark();
            }
        }
        else
        {
            QList<QGraphicsItem*> _items = items(e->pos());
            foreach(QGraphicsItem* item, _items)
            {
                ViraGraphicsItem * viraGraphicsItem = dynamic_cast<ViraGraphicsItem*>(item);
                if(viraGraphicsItem)
                {
                    regionbiz::RegionBizManager::instance()->centerOnEntity(viraGraphicsItem->getId());
                    return;
                }
            }

            regionbiz::RegionBizManager::instance()->selectEntity(_currFloor_id); //0);
            QGraphicsView::mouseReleaseEvent(e);
        }
    }
}

void ViraEditorView::removeLastPointInMarkArea()
{
    if(_lines.size() == 2)
    {
        clearTempItems();
    }
    else if(_lines.size() > 2)
    {
        delete _lines.last();
        _lines.removeLast();

        if(_lines.size() >= 2)
        {
            _lines.first()->setLine(QLineF(_lastMouseScenePos, _lines.first()->line().p2()));
            _lines.last()->setLine(QLineF(_lines.last()->line().p1(), _lastMouseScenePos));
        }
    }
}

void ViraEditorView::abortCreateMark()
{
    _editObjectGeometry = 0;
    _mode = ScrollMode;
    setCursor(QCursor(Qt::ArrowCursor));

    clearTempItems();

    QList<QVariant>list;
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::MarkCreated, list, QString("visualize_system"));
}

void ViraEditorView::finishCreateMark()
{
    if(_lines.size() == 2 || _lines.size() > 3)
    {
        QPolygonF pol;
        for(int i(0); i < _lines.size()-1; ++i)
        {
            QGraphicsLineItem * item = _lines.at(i);
            QLineF line = item->line();
            pol.append(line.p1());
        }

        _editObjectGeometry = 0;
        _mode = ScrollMode;
        setCursor(QCursor(Qt::ArrowCursor));

        qDebug() << pol.size() << ", pol :" << pol;


        clearTempItems();

        QList<QVariant>list;

        QVariant type(1);
        list.append(type);

        QVariant polygon;
        polygon.setValue(pol);
        list.append(polygon);

        CommonMessageNotifier::send( (uint)visualize_system::BusTags::MarkCreated, list, QString("visualize_system"));
    }
}

void ViraEditorView::clearTempItems()
{
    foreach(QGraphicsLineItem * item, _lines)
        delete item;
    _lines.clear();
}

void ViraEditorView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        if(_mode == EditAreaMode)
            clearTempItems();
        else if(_mode == EditMarkMode)
            abortCreateMark();
    }
    else if(event->key() == Qt::Key_Space || event->key() == Qt::Key_Return)
    {
        if(_mode == EditMarkMode)
            finishCreateMark();
    }
    else if(event->key() == Qt::Key_Backspace)
    {
        if(_mode == EditMarkMode)
            removeLastPointInMarkArea();
    }

    QGraphicsView::keyPressEvent(event);
}

void ViraEditorView::zoomReset()
{
    resetTransform();
    _scale = 1;
    _zoom = 1;
    //    syncItems();
}

void ViraEditorView::centerEditorOn(qulonglong id)
{
    auto it = _itemsOnFloor.find(id);
    if(it != _itemsOnFloor.end())
    {
        ViraGraphicsItem * viraGraphicsItem = dynamic_cast<ViraGraphicsItem*>(it.value());
        if(viraGraphicsItem)
            viraGraphicsItem->centerOnItem();
    }
}

void ViraEditorView::zoomIn()
{
    if(_zoom < _zoomMax+3)
    {
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        _scale *= 2;
        scale(2,2);
        ++_zoom;
//        syncItems();
    }
}

void ViraEditorView::zoomOut()
{
    if(_zoom > _zoomMin-3)
    {
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        _scale /= 2;
        scale(0.5,0.5);
        --_zoom;
//        syncItems();
    }
}



//void ViraEditorView::syncItems()
//{
//    if(_owerViews.size() <= 1)
//        return;

//    foreach(GraphicsPixmapItem * item, _owerViews)
//        item->setVisible(false);

//    if(_zoom >= _zoomMax)
//    {
//        _owerViews.last()->setVisible(true);
//    }
//    else if(_zoom <= _zoomMin)
//    {
//        _owerViews.first()->setVisible(true);
//    }
//    else
//    {
//        int index = (-1 * _zoomMin) + _zoom;
//        _owerViews.at(index)->setVisible(true);
//    }
//}

void ViraEditorView::slotRastersVisibleChanged()
{
    if(_axisRasterItem)
    {
        QVariant axisRasterVisible = CtrConfig::getValueByName("application_settings.axisRasterVisible", true, true);
        _axisRasterItem->setVisible(axisRasterVisible.toBool());
    }
    if(_sizesRasterItem)
    {
        QVariant sizesRasterVisible = CtrConfig::getValueByName("application_settings.sizesRasterVisible", true, true);
        _sizesRasterItem->setVisible(sizesRasterVisible.toBool());
    }
    if(_waterDisposalRasterItem)
    {
        QVariant waterDisposalRasterVisible = CtrConfig::getValueByName("application_settings.waterDisposalRasterVisible", true, true);
        _waterDisposalRasterItem->setVisible(waterDisposalRasterVisible.toBool());
    }
    if(_waterSupplyRasterItem)
    {
        QVariant waterSupplyRasterVisible = CtrConfig::getValueByName("application_settings.waterSupplyRasterVisible", true, true);
        _waterSupplyRasterItem->setVisible(waterSupplyRasterVisible.toBool());
    }
    if(_heatingRasterItem)
    {
        QVariant heatingRasterVisible = CtrConfig::getValueByName("application_settings.heatingRasterVisible", true, true);
        _heatingRasterItem->setVisible(heatingRasterVisible.toBool());
    }
    if(_electricityRasterItem)
    {
        QVariant electricityRasterVisible = CtrConfig::getValueByName("application_settings.electricityRasterVisible", true, true);
        _electricityRasterItem->setVisible(electricityRasterVisible.toBool());
    }
    if(_doorsRasterItem)
    {
        QVariant doorsRasterVisible = CtrConfig::getValueByName("application_settings.doorsRasterVisible", true, true);
        _doorsRasterItem->setVisible(doorsRasterVisible.toBool());
    }
    QVariant defectsRasterVisible = CtrConfig::getValueByName("application_settings.defectsRasterVisible", true, true);
    bool visibleDefects = defectsRasterVisible.toBool();
    foreach(QGraphicsItem * item, _itemsOnFloor.values())
    {
        MarkGraphicsItem * markGraphicsItem = dynamic_cast<MarkGraphicsItem*>(item);
        if(markGraphicsItem)
            markGraphicsItem->setVisible(visibleDefects);
    }
}

void ViraEditorView::slotGetNeedVisibleRasters(bool &axis, bool &sizes, bool &waterDisposal, bool &waterSupply, bool &heating, bool &electricity, bool &doors)
{
    axis = (_sizesRasterItem != nullptr);
    sizes = (_sizesRasterItem != nullptr);
    waterDisposal = (_waterDisposalRasterItem != nullptr);
    waterSupply = (_waterSupplyRasterItem != nullptr);
    heating = (_heatingRasterItem != nullptr);
    electricity = (_electricityRasterItem != nullptr);
    doors = (_doorsRasterItem != nullptr);
}

///----------------------------------------------------------------------------------------------

LayersMenu::LayersMenu()
    : QLabel(QString::fromUtf8("Слои"))
{
}

void LayersMenu::mousePressEvent(QMouseEvent *)
{
    QMenu menu(this);

    bool axis, sizes, waterDisposal, waterSupply, heating, electricity, doors;
    emit getNeedVisibleRasters(axis, sizes, waterDisposal, waterSupply, heating, electricity, doors);

    QAction * allAct = menu.addAction(QString::fromUtf8("все слои"));
    allAct->setCheckable(true);
    allAct->setChecked(true);
    QAction * separator = menu.addSeparator();
    bool removeAllAct(true);

    QAction * axisAct = nullptr;
    if(axis)
    {
        removeAllAct = false;
        axisAct = menu.addAction(QString::fromUtf8("оси"));
        axisAct->setCheckable(true);
        QVariant axisRasterVisible = CtrConfig::getValueByName("application_settings.axisRasterVisible", true, true);
        axisAct->setChecked(axisRasterVisible.toBool());
        if(axisRasterVisible.toBool() == false)
            allAct->setChecked(false);
    }
    QAction * sizesAct = nullptr;
    if(sizes)
    {
        removeAllAct = false;
        sizesAct = menu.addAction(QString::fromUtf8("размер"));
        sizesAct->setCheckable(true);
        QVariant sizesRasterVisible = CtrConfig::getValueByName("application_settings.sizesRasterVisible", true, true);
        sizesAct->setChecked(sizesRasterVisible.toBool());
        if(sizesRasterVisible.toBool() == false)
            allAct->setChecked(false);
    }
    QAction * waterDisposalAct = nullptr;
    if(waterDisposal)
    {
        removeAllAct = false;
        waterDisposalAct = menu.addAction(QString::fromUtf8("водоотведение"));
        waterDisposalAct->setCheckable(true);
        QVariant waterDisposalRasterVisible = CtrConfig::getValueByName("application_settings.waterDisposalRasterVisible", true, true);
        waterDisposalAct->setChecked(waterDisposalRasterVisible.toBool());
        if(waterDisposalRasterVisible.toBool() == false)
            allAct->setChecked(false);
    }
    QAction * waterSupplyAct = nullptr;
    if(waterSupply)
    {
        removeAllAct = false;
        waterSupplyAct = menu.addAction(QString::fromUtf8("водоснабжение"));
        waterSupplyAct->setCheckable(true);
        QVariant waterSupplyRasterVisible = CtrConfig::getValueByName("application_settings.waterSupplyRasterVisible", true, true);
        waterSupplyAct->setChecked(waterSupplyRasterVisible.toBool());
        if(waterSupplyRasterVisible.toBool() == false)
            allAct->setChecked(false);
    }
    QAction * heatingAct = nullptr;
    if(heating)
    {
        removeAllAct = false;
        heatingAct = menu.addAction(QString::fromUtf8("отопление"));
        heatingAct->setCheckable(true);
        QVariant heatingRasterVisible = CtrConfig::getValueByName("application_settings.heatingRasterVisible", true, true);
        heatingAct->setChecked(heatingRasterVisible.toBool());
        if(heatingRasterVisible.toBool() == false)
            allAct->setChecked(false);
    }
    QAction * electricityAct = nullptr;
    if(electricity)
    {
        removeAllAct = false;
        electricityAct = menu.addAction(QString::fromUtf8("электрика"));
        electricityAct->setCheckable(true);
        QVariant electricityRasterVisible = CtrConfig::getValueByName("application_settings.electricityRasterVisible", true, true);
        electricityAct->setChecked(electricityRasterVisible.toBool());
        if(electricityRasterVisible.toBool() == false)
            allAct->setChecked(false);
    }
    QAction * doorsAct = nullptr;
    if(doors)
    {
        removeAllAct = false;
        doorsAct = menu.addAction(QString::fromUtf8("двери"));
        doorsAct->setCheckable(true);
        QVariant doorsRasterVisible = CtrConfig::getValueByName("application_settings.doorsRasterVisible", true, true);
        doorsAct->setChecked(doorsRasterVisible.toBool());
        if(doorsRasterVisible.toBool() == false)
            allAct->setChecked(false);
    }

    if(removeAllAct)
    {
        menu.removeAction(allAct);
        allAct = nullptr;

        menu.removeAction(separator);
        separator = nullptr;
    }

    QAction * defectsAct = menu.addAction(QString::fromUtf8("дефекты"));
    defectsAct->setCheckable(true);
    QVariant defectsRasterVisible = CtrConfig::getValueByName("application_settings.defectsRasterVisible", true, true);
    defectsAct->setChecked(defectsRasterVisible.toBool());

    if(allAct)
    {
        if(defectsRasterVisible.toBool() == false)
            allAct->setChecked(false);
        if(allAct->isChecked())
            allAct->setDisabled(true);
    }

    QPoint globalPos = mapToGlobal(pos());
    QAction * act = menu.exec(QPoint(globalPos.x(), globalPos.y() + 22));
    if( ! act )
        return;

    if(act == allAct)
    {
        if(axis)
            CtrConfig::setValueByName("application_settings.axisRasterVisible", true);
        if(sizes)
            CtrConfig::setValueByName("application_settings.sizesRasterVisible", true);
        if(waterDisposal)
            CtrConfig::setValueByName("application_settings.waterDisposalRasterVisible", true);
        if(waterSupply)
            CtrConfig::setValueByName("application_settings.waterSupplyRasterVisible", true);
        if(heating)
            CtrConfig::setValueByName("application_settings.heatingRasterVisible", true);
        if(electricity)
            CtrConfig::setValueByName("application_settings.electricityRasterVisible", true);
        if(doors)
            CtrConfig::setValueByName("application_settings.doorsRasterVisible", true);
        CtrConfig::setValueByName("application_settings.defectsRasterVisible", true);
    }
    else if(act == axisAct)
        CtrConfig::setValueByName("application_settings.axisRasterVisible", act->isChecked());
    else if(act == sizesAct)
        CtrConfig::setValueByName("application_settings.sizesRasterVisible", act->isChecked());
    else if(act == waterDisposalAct)
        CtrConfig::setValueByName("application_settings.waterDisposalRasterVisible", act->isChecked());
    else if(act == waterSupplyAct)
        CtrConfig::setValueByName("application_settings.waterSupplyRasterVisible", act->isChecked());
    else if(act == heatingAct)
        CtrConfig::setValueByName("application_settings.heatingRasterVisible", act->isChecked());
    else if(act == electricityAct)
        CtrConfig::setValueByName("application_settings.electricityRasterVisible", act->isChecked());
    else if(act == doorsAct)
        CtrConfig::setValueByName("application_settings.doorsRasterVisible", act->isChecked());
    else if(act == defectsAct)
        CtrConfig::setValueByName("application_settings.defectsRasterVisible", act->isChecked());

    emit rastersVisibleChanged();
}











