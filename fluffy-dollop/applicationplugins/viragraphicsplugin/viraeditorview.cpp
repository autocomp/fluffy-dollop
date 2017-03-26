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

using namespace regionbiz;


ViraEditorView::ViraEditorView()
{
    setMouseTracking(true);
    setDragMode(QGraphicsView::NoDrag);
    setCursor(QCursor(Qt::ArrowCursor));

    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::EditAreaGeometry, this, SLOT(slotEditAreaGeometry(QVariant)),
                                      qMetaTypeId< quint64 >(),
                                      QString("visualize_system") );

    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::SetMarkPosition, this, SLOT(slotSetMarkPosition(QVariant)),
                                      qMetaTypeId< quint64 >(),
                                      QString("visualize_system") );
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

    // find first floor
    BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(facilityId, BaseArea::AT_FACILITY);
    FacilityPtr facilityPtr = BaseArea::convert< Facility >(ptr);
    if(facilityPtr)
    {
        FloorPtrs floors = facilityPtr->getChilds();
        for( FloorPtr floorPtr: floors )
        {
            setFloor(floorPtr->getId());
            break;
        }
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

    AreaInitData roomInitData;
    roomInitData.zValue = 10000;
    roomInitData.isSelectableFromMap = true;
    roomInitData.penNormal.setColor(Qt::green);
    roomInitData.penNormal.setCosmetic(true);
    roomInitData.penNormal.setWidth(2);
    roomInitData.penHoverl = roomInitData.penNormal;
    roomInitData.penHoverl.setWidth(3);
    QColor roomBrushColor(Qt::green);
    roomBrushColor.setAlpha(20);
    roomInitData.brushNormal.setColor(roomBrushColor);
    roomInitData.brushNormal.setStyle(Qt::SolidPattern);
    roomBrushColor.setAlpha(70);
    roomInitData.brushHoverl = roomInitData.brushNormal;
    roomInitData.brushHoverl.setColor(roomBrushColor);

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

        QString pixmapPath = destPath + floorPtr->getPlanPath();
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
                    if( mark->isMetadataPresent( "name" ))
                        _markGraphicsItem->setToolTip(mark->getMetadata( "name" )->getValueAsVariant().toString());
                    else
                        _markGraphicsItem->setToolTip(QString::fromUtf8("Deffect"));

                    scene()->addItem(_markGraphicsItem);
                    connect(_markGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                    _itemsOnFloor.insert(mark->getId(), _markGraphicsItem);
                }

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

void ViraEditorView::selectViraItem(qulonglong id, bool centerOnArea)
{
    if(_mode == ScrollMode)
    {
        regionbiz::RegionBizManager::instance()->selectArea(id);
        if(centerOnArea)
            regionbiz::RegionBizManager::instance()->centerOnArea(id);
    }
}

void ViraEditorView::slotSelectItem(qulonglong id, bool centerOnArea)
{
    if(_mode == ScrollMode)
    {
        regionbiz::RegionBizManager::instance()->selectArea(id);
        if(centerOnArea)
            regionbiz::RegionBizManager::instance()->centerOnArea(id);
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
    }
}

void ViraEditorView::slotEditAreaGeometry(QVariant var)
{
    _editObjectGeometry = var.toUInt();
    auto it = _itemsOnFloor.find(_editObjectGeometry);
    if(it != _itemsOnFloor.end())
    {
        AreaGraphicsItem * areaGraphicsItem = dynamic_cast<AreaGraphicsItem*>(it.value());
        if(areaGraphicsItem)
            areaGraphicsItem->hide();
    }
}

void ViraEditorView::slotSetMarkPosition(QVariant var)
{
    _editObjectGeometry = var.toUInt();

    auto room_ptr = RegionBizManager::instance()->getBaseArea( _editObjectGeometry );
    auto room = BaseArea::convert<Room>(room_ptr);
    _editObjectExtend = room->getCoords();

    _mode = EditMarkMode;
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
            setCursor(QCursor(Qt::ArrowCursor));
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
                room->commitMarks();

                MarkGraphicsItem * _markGraphicsItem = new MarkGraphicsItem(markPtr->getId());
                _markGraphicsItem->setPos(scenePos);
                _markGraphicsItem->setToolTip(QString::fromUtf8("Deffect"));
                scene()->addItem(_markGraphicsItem);
                connect(_markGraphicsItem, SIGNAL(signalSelectItem(qulonglong,bool)), this, SLOT(slotSelectItem(qulonglong,bool)));
                _itemsOnFloor.insert(markPtr->getId(), _markGraphicsItem);

                _editObjectExtend.clear();
                _editObjectGeometry = 0;
                _mode = ScrollMode;

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
                    regionbiz::RegionBizManager::instance()->selectArea(viraGraphicsItem->getId());
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

                AreaInitData roomInitData;
                roomInitData.zValue = 10000;
                roomInitData.isSelectableFromMap = true;
                roomInitData.penNormal.setColor(Qt::green);
                roomInitData.penNormal.setCosmetic(true);
                roomInitData.penNormal.setWidth(2);
                roomInitData.penHoverl = roomInitData.penNormal;
                roomInitData.penHoverl.setWidth(3);
                QColor roomBrushColor(Qt::green);
                roomBrushColor.setAlpha(20);
                roomInitData.brushNormal.setColor(roomBrushColor);
                roomInitData.brushNormal.setStyle(Qt::SolidPattern);
                roomBrushColor.setAlpha(70);
                roomInitData.brushHoverl = roomInitData.brushNormal;
                roomInitData.brushHoverl.setColor(roomBrushColor);

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
                regionbiz::RegionBizManager::instance()->centerOnArea(viraGraphicsItem->getId());
                return;
            }
        }

        regionbiz::RegionBizManager::instance()->selectArea(0);
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
