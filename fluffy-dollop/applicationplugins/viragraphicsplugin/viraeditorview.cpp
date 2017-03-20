#include "viraeditorview.h"
#include "areagraphicsitem.h"
#include "markgraphicsitem.h"
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDir>
#include <QInputDialog>
#include <QDebug>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <regionbiz/rb_manager.h>
#include <regionbiz/rb_locations.h>

using namespace regionbiz;


ViraEditorView::ViraEditorView()
{
    setMouseTracking(true);
    setDragMode(QGraphicsView::NoDrag);
    setCursor(QCursor(Qt::ArrowCursor));
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
    _currFloor_id = 0;

    delete _currentLine;
    _currentLine = 0;
    foreach(QGraphicsLineItem * item, _lines)
        delete item;
    _lines.clear();


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
                                                                "./data/", true);
    if(regionBizInitJson_Path.isValid())
         destPath = regionBizInitJson_Path.toString();

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

        BaseAreaPtrs rooms = floorPtr->getChilds( Floor::FCF_ALL_ROOMS );
        for( BaseAreaPtr room_ptr: rooms )
        {
            RoomPtr room = BaseArea::convert< Room >( room_ptr );
            if(room)
            {
                AreaGraphicsItem * areaGraphicsItem = new AreaGraphicsItem(room->getCoords());
                areaGraphicsItem->setAcceptHoverEvents(true);
                roomInitData.id = room->getId();
                areaGraphicsItem->init(roomInitData);
                scene()->addItem(areaGraphicsItem);
                _itemsOnFloor.insert(room->getId(), areaGraphicsItem);
            }
        }
    }

    /*
    if(floorPtr->getId() == 29)
    {
        {
            MarkGraphicsItem * _markGraphicsItem = new MarkGraphicsItem(QPixmap(":/img/mark.png"));
            _markGraphicsItem->setPos(897,511.75);
            _markGraphicsItem->setToolTip(QString::fromUtf8("Течёт унитаз"));
            scene()->addItem(_markGraphicsItem);
            _itemsOnFloor.insert(10000, _markGraphicsItem);
        }
        {
            MarkGraphicsItem * _markGraphicsItem = new MarkGraphicsItem(QPixmap(":/img/mark.png"));
            _markGraphicsItem->setPos(729.75,595.5);
            _markGraphicsItem->setToolTip(QString::fromUtf8("Оторваны обои"));
            scene()->addItem(_markGraphicsItem);
            _itemsOnFloor.insert(10001, _markGraphicsItem);
        }
    }
    */

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

void ViraEditorView::selectionItemsChanged(uint64_t prev_id, uint64_t curr_id)
{
    if(prev_id > 0)
    {
        auto it = _itemsOnFloor.find(prev_id);
        if(it != _itemsOnFloor.end())
        {
            AreaGraphicsItem * areaGraphicsItem = dynamic_cast<AreaGraphicsItem*>(it.value());
            if(areaGraphicsItem)
                areaGraphicsItem->setItemselected(false);
        }
    }
    if(curr_id > 0)
    {
        auto it = _itemsOnFloor.find(curr_id);
        if(it != _itemsOnFloor.end())
        {
            AreaGraphicsItem * areaGraphicsItem = dynamic_cast<AreaGraphicsItem*>(it.value());
            if(areaGraphicsItem)
                areaGraphicsItem->setItemselected(true);
        }
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

void ViraEditorView::wheelEvent(QWheelEvent *e)
{
    if(e->delta() > 0)
        zoomIn();
    else
        zoomOut();
}

void ViraEditorView::mouseMoveEvent(QMouseEvent* e)
{
    if(_currentLine)
    {
        QPointF scenePos = mapToScene(e->pos());
        _currentLine->setLine(QLineF(_currentLine->line().p1(), scenePos));
    }
    QGraphicsView::mouseMoveEvent(e);
}

void ViraEditorView::mousePressEvent(QMouseEvent *e)
{
    if(e->button() & Qt::LeftButton)
    {
        if(e->modifiers() & Qt::ControlModifier)
        {
            QPointF scenePos = mapToScene(e->pos());
            if(_currentLine)
            {
                _lines.append(_currentLine);
            }
            _currentLine = new QGraphicsLineItem();
            scene()->addItem(_currentLine);
            _currentLine->setZValue(1000);
            QPen pen(Qt::red);
            pen.setCosmetic(true);
            pen.setWidth(1);
            _currentLine->setPen(pen);
            _currentLine->setLine(QLineF(scenePos, scenePos));
        }
        else
        {
            setDragMode(QGraphicsView::ScrollHandDrag);
        }
    }

    if(e->button() & Qt::RightButton)
    {
        qDebug() << "mousePressEvent:" << mapToScene(e->pos());
    }

    QGraphicsView::mousePressEvent(e);
    if(e->button() & Qt::MiddleButton)
    {
        zoomReset();
    }
}

void ViraEditorView::mouseReleaseEvent(QMouseEvent *e)
{
    QGraphicsView::mouseReleaseEvent(e);
    setDragMode(QGraphicsView::NoDrag);
    setCursor(QCursor(Qt::ArrowCursor));
}

void ViraEditorView::keyPressEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_Escape)
    {
        delete _currentLine;
        _currentLine = 0;
        foreach(QGraphicsLineItem * item, _lines)
            delete item;
        _lines.clear();
    }
    else if(event->key() == Qt::Key_Space)
    {
        QPolygonF pol;
        foreach(QGraphicsLineItem * item, _lines)
        {
            QLineF line = item->line();
            pol.append(line.p1());
        }
        pol.append(_lines.last()->line().p2());

        qDebug() << "---------- parent ID:" << _currFloor_id;
        foreach(QPointF p, pol)
            qDebug() << p;
        qDebug() << "----------";

        auto mngr = RegionBizManager::instance();
        auto room = mngr->addArea< Room >( _currFloor_id );
        room->setCoords(pol);
        BaseArea::convert< Room >( room )->setName(QInputDialog::getText(0, QString::fromUtf8("!!!"), QString::fromUtf8("set room name")));
        room->commit();

        setFloor(_currFloor_id);
    }

//    else if(event->key() == Qt::Key_Backspace)
//    {
//        if(_lines.size() > 1)
//        {
//            auto it = _lines.end();
//            --it;
//            delete *it;
//            _lines.erase(it);

//            QLineF line = _lines.last()->line();
//            _currentLine->setLine(line);
//        }
//    }

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
        centerOn(it.value());
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
