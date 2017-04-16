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
#include <regionbiz/rb_locations.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>
#include "viragraphicsitem.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

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

    _upButton = new QToolButton();
    _upButton->setIcon(QIcon(":/img/up.png"));
    _upButton->setFixedSize(32,32);
    _upButton->setStyleSheet("border-radius:4;border-color: rgb(255, 255, 255);");
    connect(_upButton, SIGNAL(clicked(bool)), this, SLOT(slotFacilityUp()));

    _currentFacility = new QLabel;
    _currentFacility->setAlignment(Qt::AlignCenter);
    _currentFacility->setStyleSheet("border-radius:4;border-color: rgb(255, 255, 255);");
    _currentFacility->setMinimumSize( 100, 30 );

    _downButton = new QToolButton();
    _downButton->setIcon(QIcon(":/img/down.png"));
    _downButton->setFixedSize(32,32);
    _downButton->setStyleSheet("border-radius:4;border-color: rgb(255, 255, 255);");
    connect(_downButton, SIGNAL(clicked(bool)), this, SLOT(slotFacilityDown()));

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addStretch();
    hLayout->addWidget(_upButton);
    hLayout->setAlignment( _upButton, Qt::AlignTop );
    hLayout->addWidget(_currentFacility);
    hLayout->setAlignment( _currentFacility, Qt::AlignTop );
    hLayout->addWidget(_downButton);
    hLayout->setAlignment( _downButton, Qt::AlignTop );
    hLayout->addStretch();
    setLayout(hLayout);
}

ViraEditorView::~ViraEditorView()
{
    // reinit();
}

void ViraEditorView::reinit(qulonglong facilityId)
{
    foreach(GraphicsPixmapItem * item, _owerViews)
        delete item;
    _owerViews.clear();

    foreach(QGraphicsItem * item, _itemsOnFloor)
        delete item;
    _itemsOnFloor.clear();

    _floorsMap.clear();

    _upButton->setDisabled(true);
    _downButton->setDisabled(true);
    _currentFacility->clear();

    // find first floor
    BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(facilityId, BaseArea::AT_FACILITY);
    FacilityPtr facilityPtr = BaseArea::convert< Facility >(ptr);
    if(facilityPtr)
    {
        FloorPtrs floors = facilityPtr->getChilds();
        for( FloorPtr floorPtr: floors )
            _floorsMap.insert(floorPtr->getId(), floorPtr->getName());

        if(_floorsMap.isEmpty() == false)
            setFloor(_floorsMap.begin().key());
    }
}

void ViraEditorView::setFloor(qulonglong floorId)
{
    if(_currFloor_id == floorId)
        return;

    _currFloor_id = 0;

    clearTempItems();

    foreach(GraphicsPixmapItem * item, _owerViews)
        delete item;
    _owerViews.clear();

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
            if(it.key() == _currFloor_id)
            {
                _upButton->setDisabled(i == _floorsMap.size()-1);
                _downButton->setDisabled(i == 0);
                break;
            }
            ++i;
        }

        QString pixmapPath = destPath + QString::number(floorId) + QDir::separator() + QString("area.tiff"); // floorPtr->getPlanPath();
        QPixmap pm(pixmapPath);
        GraphicsPixmapItem* item = new GraphicsPixmapItem(pm);
        scene()->addItem(item);
        setSceneRect(0, 0, pm.width(), pm.height());
        _owerViews.append(item);

        //MarkPtrs marks_of_floor = floorPtr->getMarks();

        BaseAreaPtrs rooms = floorPtr->getChilds( Floor::FCF_ALL_ROOMS );
        for( BaseAreaPtr room_ptr: rooms )
        {
            RoomPtr room = BaseArea::convert< Room >( room_ptr );
            if(room)
            {
                MarkPtrs marks_of_room = room->getMarks();
                for( MarkPtr mark: marks_of_room )
                {
//                    quint64 id = mark->getId();
//                    bool res = RegionBizManager::instance()->deleteMark(id);
//                    qDebug() << "===> mark" << id << "res" << res;

                    QPointF center = mark->getCenter();
                    MarkGraphicsItem * _markGraphicsItem = new MarkGraphicsItem(mark->getId());
                    _markGraphicsItem->setPos(center);

                    scene()->addItem(_markGraphicsItem);
                    connect(_markGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                    _itemsOnFloor.insert(mark->getId(), _markGraphicsItem);
                }

                QColor roomColor(Qt::gray);
                BaseMetadataPtr statusPtr = room->getMetadata("status");
                if(statusPtr)
                {
                    QString status = statusPtr->getValueAsVariant().toString();
                    if(status == QString::fromUtf8("Свободно"))
                        roomColor = QColor(86,206,18);
                    else if(status == QString::fromUtf8("В аренде"))
                        roomColor = QColor(226,224,111);
                    else
                        roomColor = QColor(Qt::gray);
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
            auto it = _floorsMap.find(curr_id);
            if(it != _floorsMap.end())
            {
                setFloor(curr_id);
            }
            else
            {
                BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(curr_id, BaseArea::AT_ROOM);
                RoomPtr roomPtr = BaseArea::convert< Room >(ptr);
                if(roomPtr)
                {
                    auto it = _floorsMap.find(roomPtr->getParentId());
                    if(it != _floorsMap.end())
                        setFloor(it.key());
                }
            }
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
        auto room_ptr = RegionBizManager::instance()->getBaseArea( _editObjectGeometry );
        auto room = BaseArea::convert<Room>(room_ptr);
        if(room)
        {
            _editObjectExtend = room->getCoords();
            setCursor(QCursor(QPixmap(":/img/cursor_mark.png"), 0, 0));
            _mode = EditMarkMode;
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
                QColor roomColor(Qt::gray);
                BaseMetadataPtr statusPtr = roomPtr->getMetadata("status");
                if(statusPtr)
                {
                    QString status = statusPtr->getValueAsVariant().toString();
                    if(status == QString::fromUtf8("Свободно"))
                        roomColor = QColor(86,206,18);
                    else if(status == QString::fromUtf8("В аренде"))
                        roomColor = QColor(226,224,111);
                    else
                        roomColor = QColor(Qt::gray);

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
                if(markPtr)
                {
                    BaseMetadataPtr status = markPtr->getMetadata("status");
                    if(status)
                    {
                        QString statusStr = status->getValueAsString();
                        if(statusStr == QString::fromUtf8("в архиве"))
                        {
                            _itemsOnFloor.erase(it);
                            delete markItem;
                            return;
                        }
                    }
                }

                markItem->reinit();
            }
        }
    }
}

void ViraEditorView::slotFacilityUp()
{
    if(_mode != ScrollMode)
        return;

    int i(0);
    for(auto it = _floorsMap.begin(); it != _floorsMap.end(); ++it)
    {
        if(it.key() == _currFloor_id)
        {
            ++it;
            if(it != _floorsMap.end())
                RegionBizManager::instance()->selectEntity(it.key());
            break;
        }
        ++i;
    }
}

void ViraEditorView::slotFacilityDown()
{
    if(_mode != ScrollMode)
        return;

    int i(0);
    for(auto it = _floorsMap.begin(); it != _floorsMap.end(); ++it)
    {
        if(it.key() == _currFloor_id)
        {
            if(it != _floorsMap.begin())
            {
                --it;
                RegionBizManager::instance()->selectEntity(it.key());
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
        QPointF scenePos = mapToScene(e->pos());
        if(_editObjectExtend.containsPoint(scenePos, Qt::OddEvenFill))
            setCursor(QCursor(Qt::ArrowCursor)); //
        else
            setCursor(QCursor(Qt::ForbiddenCursor));
    }

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
            if(_editObjectExtend.containsPoint(scenePos, Qt::OddEvenFill))
            {
                auto room_ptr = RegionBizManager::instance()->getBaseArea( _editObjectGeometry );
                auto room = BaseArea::convert<Room>(room_ptr);
                MarkPtr markPtr = room->addMark( scenePos );
                markPtr->setName(QString::fromUtf8("дефект"));
                bool res = markPtr->commit();

                MarkGraphicsItem * _markGraphicsItem = new MarkGraphicsItem(markPtr->getId());
                _markGraphicsItem->setPos(scenePos);
                scene()->addItem(_markGraphicsItem);
                connect(_markGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                _itemsOnFloor.insert(markPtr->getId(), _markGraphicsItem);

                _editObjectExtend.clear();
                _editObjectGeometry = 0;
                _mode = ScrollMode;
                setCursor(QCursor(Qt::ArrowCursor));

                quint64 markId(markPtr->getId());
                CommonMessageNotifier::send( (uint)visualize_system::BusTags::EditModeFinish, QVariant(markId), QString("visualize_system"));
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

                QColor roomColor(Qt::gray);
                BaseMetadataPtr statusPtr = room->getMetadata("status");
                if(statusPtr)
                {
                    QString status = statusPtr->getValueAsVariant().toString();
                    if(status == QString::fromUtf8("Свободно"))
                        roomColor = QColor(86,206,18);
                    else if(status == QString::fromUtf8("В аренде"))
                        roomColor = QColor(226,224,111);
                    else
                        roomColor = QColor(Qt::gray);
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

        regionbiz::RegionBizManager::instance()->selectEntity(0);
        QGraphicsView::mouseReleaseEvent(e);
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
        clearTempItems();
    }
    else if(event->key() == Qt::Key_Space)
    {
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
