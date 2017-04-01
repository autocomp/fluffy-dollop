#include "workstate.h"
#include "xmlreader.h"
#include "areagraphicsitem.h"
#include "locationitem.h"
#include "types.h"
#include <regionbiz/rb_manager.h>
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
                QString planPath = locationPtr->getPlanPath();
                if(planPath.isEmpty() == false)
                {
                    planPath = destPath + QString::number(locationPtr->getId()) + QDir::separator() + planPath;
                    QGraphicsPixmapItem * pixmapItem = new QGraphicsPixmapItem(QPixmap(planPath));
                    PlanKeeper::PlanParams planParams = locationPtr->getPlanParams();
                    pixmapItem->setTransform(QTransform().scale(planParams.scale_w, planParams.scale_h));
                    pixmapItem->setPos(planParams.x, planParams.y);
                    pixmapItem->setZValue(100);
                    pixmapItem->setOpacity(0.5);
                    _scene->addItem(pixmapItem);

                }

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
//                    if(facilityPtr->getId() == 21)
//                    {
//                        QPolygonF pol;
//                        pol.append(QPointF(299.11338806, 149.05607224));
//                        pol.append(QPointF(299.11358261, 149.05606461));
//                        pol.append(QPointF(299.11357498, 149.05584717));
//                        pol.append(QPointF(299.11426926, 149.05582428));
//                        pol.append(QPointF(299.11429596, 149.05656433));
//                        pol.append(QPointF(299.11475372, 149.05654907));
//                        pol.append(QPointF(299.11473846, 149.05598068));
//                        pol.append(QPointF(299.11454391, 149.05598450));
//                        pol.append(QPointF(299.11452103, 149.05527496));
//                        pol.append(QPointF(299.11536407, 149.05525208));
//                        pol.append(QPointF(299.11539459, 149.05619049));
//                        pol.append(QPointF(299.11514282, 149.05620956));
//                        pol.append(QPointF(299.11516190, 149.05670929));
//                        pol.append(QPointF(299.11557770, 149.05670929));
//                        pol.append(QPointF(299.11560440, 149.05716324));
//                        pol.append(QPointF(299.11517334, 149.05717468));
//                        pol.append(QPointF(299.11518860, 149.05765533));
//                        pol.append(QPointF(299.11543655, 149.05765915));
//                        pol.append(QPointF(299.11545944, 149.05860138));
//                        pol.append(QPointF(299.11462021, 149.05862808));
//                        pol.append(QPointF(299.11459351, 149.05792618));
//                        pol.append(QPointF(299.11479568, 149.05792236));
//                        pol.append(QPointF(299.11477661, 149.05734253));
//                        pol.append(QPointF(299.11431885, 149.05735779));
//                        pol.append(QPointF(299.11434174, 149.05810928));
//                        pol.append(QPointF(299.11365128, 149.05813599));
//                        pol.append(QPointF(299.11363602, 149.05791473));
//                        pol.append(QPointF(299.11344528, 149.05791473));
//                        pol.append(QPointF(299.11343384, 149.05745316));
//                        pol.append(QPointF(299.11362839, 149.05743408));
//                        pol.append(QPointF(299.11361313, 149.05722809));
//                        pol.append(QPointF(299.11342239, 149.05722427));
//                        pol.append(QPointF(299.11341095, 149.05676270));
//                        pol.append(QPointF(299.11360168, 149.05675888));
//                        pol.append(QPointF(299.11359787, 149.05654907));
//                        pol.append(QPointF(299.11339951, 149.05654907));
//                        facilityPtr->setCoords(pol);
//                        facilityPtr->commit();
//                    }

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
//    foreach(LocationItem * locationItem, _locationItems)
//        locationItem->zoomChanged(*_zoom);
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



















