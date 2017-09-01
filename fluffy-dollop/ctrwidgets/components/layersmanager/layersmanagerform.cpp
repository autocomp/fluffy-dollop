#include "layersmanagerform.h"
#include "ui_layersmanagerform.h"
#include "svgeditorform.h"
#include <QFile>
#include <QDir>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QGraphicsPixmapItem>
#include <QGraphicsSvgItem>
#include <ctrcore/visual/visualizermanager.h>
#include <ctrcore/visual/viewinterface.h>
#include <ctrcore/visual/stateinterface.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <ctrwidgets/components/waitdialog/waitdialog.h>
#include <dmanager/embeddedstruct.h>
#include <dmanager/embeddedapp.h>
#include <ctrwidgets/components/layersmanager/commontypes.h>
#include <ctrcore/ctrcore/tempdircontroller.h>
#include <ctrcore/visual/geographicutils.h>
#include <QDebug>

using namespace regionbiz;
using namespace layers_manager_form;

LayersManagerForm::LayersManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LayersManagerForm),
    _recalcSceneRectTimer(this)
{
    connect(&_recalcSceneRectTimer, SIGNAL(timeout()), this, SLOT(slotCalcSceneRect()));

    auto mngr = RegionBizManager::instance();
    mngr->subscribeFileSynced( this, SLOT(slotFileLoaded(regionbiz::BaseFileKeeperPtr)));

    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::BlockGUI, this, SLOT(slotBlockGUI(QVariant)),
                                      qMetaTypeId< bool >(),
                                      QString("visualize_system") );

    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::ToolButtonInPluginChecked, this, SLOT(slotToolButtonInPluginChecked(QVariant)),
                                      qMetaTypeId< QVariantList >(),
                                      QString("visualize_system") );

    ui->setupUi(this);
    ui->addEntity->setIcon(QIcon(":/img/icon_add.png"));
    connect(ui->addEntity, SIGNAL(clicked()), this, SLOT(slotAddEntity()));

    ui->editEntity->setIcon(QIcon(":/img/edit_mode.png"));
    connect(ui->editEntity, SIGNAL(clicked()), this, SLOT(slotEditEntity()));

    ui->transparentEntity->hide();

    ui->deleteEntity->setIcon(QIcon(":/img/icon_delete.png"));
    connect(ui->deleteEntity, SIGNAL(clicked()), this, SLOT(slotDeleteEntity()));

    ui->upEntity->hide();
    ui->downEntity->hide();

    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));

    bool layerEditMode(false);
    QVariant layerEditModeVar = CtrConfig::getValueByName("application_settings.editMode");
    if(layerEditModeVar.isValid())
        layerEditMode = layerEditModeVar.toBool();
    else
        CtrConfig::setValueByName("application_settings.editMode", false);

    ui->addLayer->setVisible(layerEditMode);
    ui->addLayer->setIcon(QIcon(":/img/icon_add.png"));
    connect(ui->addLayer, SIGNAL(clicked()), this, SLOT(slotAddLayer()));

    ui->deleteLayer->setVisible(layerEditMode);
    ui->deleteLayer->setIcon(QIcon(":/img/icon_delete.png"));
    connect(ui->deleteLayer, SIGNAL(clicked()), this, SLOT(slotDeleteLayer()));

    visualize_system::VisualizerManager * visMng = visualize_system::VisualizerManager::instance();
    foreach(uint id, visMng->getVisualizersId())
        if(visMng->getViewInterface(id)->getVisualizerType() == visualize_system::Visualizer2D)
        {
            _geoVisId = id;
            _geoView = visMng->getViewInterface(id)->graphicsView();
            break;
        }
    foreach(uint id, visMng->getVisualizersId())
        if(visMng->getViewInterface(id)->getVisualizerType() == visualize_system::VisualizerPixel)
        {
            _pixelVisId = id;
            _pixelView = visMng->getViewInterface(id)->graphicsView();
            connect(visMng->getViewInterface(id), SIGNAL(signalZoomChanged(int)), this, SLOT(slotZoomChanged(int)));
            break;
        }

    reinitLayers();
}

LayersManagerForm::~LayersManagerForm()
{
    delete ui;
}

void LayersManagerForm::reinitLayer(LayerItem * layerItem)
{
    layerItem->setCheckState(0, Qt::Checked);

    QTreeWidgetItem * rastersItem = new QTreeWidgetItem(layerItem, QStringList() << QString::fromUtf8("изображения"), (int)ItemTypes::Rasters);
    rastersItem->setCheckState(0, Qt::Checked);

    QTreeWidgetItem * vectorsItem = new QTreeWidgetItem(layerItem, QStringList() << QString::fromUtf8("векторные данные"), (int)ItemTypes::Vectors);
    vectorsItem->setCheckState(0, Qt::Checked);

    QTreeWidgetItem * marksItem = new QTreeWidgetItem(layerItem, QStringList() << QString::fromUtf8("отметки"), (int)ItemTypes::Marks);
    marksItem->setCheckState(0, Qt::Checked);
    QTreeWidgetItem * defect = new QTreeWidgetItem(marksItem, QStringList() << QString::fromUtf8("дефекты"), (int)ItemTypes::Defect);
    defect->setCheckState(0, Qt::Checked);
    QTreeWidgetItem * photo = new QTreeWidgetItem(marksItem, QStringList() << QString::fromUtf8("фотографии"), (int)ItemTypes::Photo);
    photo->setCheckState(0, Qt::Checked);
    QTreeWidgetItem * photo3d = new QTreeWidgetItem(marksItem, QStringList() << QString::fromUtf8("панорамные фотографии"), (int)ItemTypes::Photo3d);
    photo3d->setCheckState(0, Qt::Checked);

}

void LayersManagerForm::reinitLayers()
{
    disconnect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));

    RegionBizManager::instance()->clearCurrent();
    RegionBizManager::instance()->clearSelect();
    _layers.clear();
    ui->treeWidget->clear();

    LayerItem * baseLayerItem = new LayerItem(ui->treeWidget, LayerTypes::Base);
    reinitLayer(baseLayerItem);
    _layers.insert(0, baseLayerItem);

    LayerItem * etalonLayerItem = new LayerItem(ui->treeWidget, LayerTypes::Etalon);
    etalonLayerItem->setCheckState(0, Qt::Checked);
//    QTreeWidgetItem * etalonRaster = new QTreeWidgetItem(etalonLayerItem, QStringList() << QString::fromUtf8("опорное изображение"), (int)ItemTypes::FacilityEtalonRasters);
//    etalonRaster->setCheckState(0, Qt::Checked);
//    QTreeWidgetItem * polygonOnPlan = new QTreeWidgetItem(etalonLayerItem, QStringList() << QString::fromUtf8("контур здания"), (int)ItemTypes::FacilityPolygonOnPlans);
//    polygonOnPlan->setCheckState(0, Qt::Checked);

    foreach(LayerPtr layerPtr, RegionBizManager::instance()->getLayers())
    {
        QString layerName = layerPtr->getName();
        LayerItem * layerItem = new LayerItem(ui->treeWidget, layerName, layerPtr->getId());
        reinitLayer(layerItem);
        _layers.insert(layerPtr->getId(), layerItem);
    }

    connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
}

void LayersManagerForm::setEmbeddedWidgetId(quint64 id)
{
    _embeddedWidgetId = id;
}

void LayersManagerForm::reset(bool showEtalonNode)
{
    ui->treeWidget->clearSelection();

    _currentData.areaPtr.reset();
    _currentData.clear();
    _loadingItems.clear();

    if(_facilitiCoordsOnFloorItem)
    {
        delete _facilitiCoordsOnFloorItem;
        _facilitiCoordsOnFloorItem = nullptr;
    }

    disconnect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
    for(int a(0); a < ui->treeWidget->topLevelItemCount(); ++a)
    {
        LayerItem * layerItem = dynamic_cast<LayerItem*>(ui->treeWidget->topLevelItem(a));
        if(layerItem)
        {
            if(layerItem->getLayerType() == LayerTypes::Etalon)
            {
                QList<QTreeWidgetItem*> items;
                for(int i(0); i < layerItem->childCount(); ++i)
                    items.append(layerItem->child(i));

                foreach(QTreeWidgetItem* item, items)
                    delete item;

                layerItem->setHidden( ! showEtalonNode );
            }
            else
            {
                layerItem->clearData();
            }
        }
    }
    connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
}

void LayersManagerForm::slotFileLoaded(regionbiz::BaseFileKeeperPtr baseFileKeeperPtr)
{
    PlanFileKeeperPtr planFileKeeperPtr = BaseFileKeeper::convert<PlanFileKeeper>(baseFileKeeperPtr);
    if(planFileKeeperPtr)
    {
        auto it = _loadingItems.find(baseFileKeeperPtr->getPath());
        if(it == _loadingItems.end())
            return;

        RasterItem * rasterTreeWidgetItem = dynamic_cast<RasterItem*>(it.value());
        SvgItem * svgTreeWidgetItem = dynamic_cast<SvgItem*>(it.value());
        _loadingItems.erase(it);


        if(rasterTreeWidgetItem)
        {
            qDebug() << "---> slotFileLoaded, baseFileKeeperPtr ID :" << baseFileKeeperPtr->getEntityId() << ", getPath:" << planFileKeeperPtr->getPath();

            QFilePtr file_ptr = planFileKeeperPtr->getLocalFile();
            QFileInfo info( *(file_ptr.get()) );
            QString filePath = info.absoluteFilePath();
            QPixmap pm(filePath);
            if(pm.isNull())
            {
                //delete rasterTreeWidgetItem;

                rasterTreeWidgetItem->setText(0, QString::fromUtf8("хуёвый растр"));
            }
            else
            {
                QGraphicsPixmapItem * pixmapRasterItem = new QGraphicsPixmapItem(pm);
                pixmapRasterItem->setTransformationMode(Qt::SmoothTransformation);
                PlanFileKeeper::PlanParams planParams = planFileKeeperPtr->getPlanParams();

                int zValue(90);
                QPointF scenePos(planParams.x, planParams.y);
                bool setTransform(true);
                LayerItem * layerItem = dynamic_cast<LayerItem*>(rasterTreeWidgetItem->parent()->parent());
                if(layerItem)
                {
                    if(layerItem->getLayerType() == LayerTypes::Base)
                        zValue = 80;
                }
                else if(rasterTreeWidgetItem->type() == (int)ItemTypes::FacilityEtalonRaster)
                {
                    zValue = 70;

                    if(_currentData.areaPtr->getType() == BaseArea::AT_FLOOR)
                    {
                        BaseMetadataPtr posStrPtr = _currentData.areaPtr->getParent()->getMetadata("position_etalon_image_on_plan_floor");
                        if(posStrPtr)
                        {
                            setTransform = false;

                            QString posStr = posStrPtr->getValueAsString();
                            QStringList list = posStr.split(QString(" "));
                            if(list.size() == 2)
                                scenePos = QPointF(list.first().toDouble(), list.last().toDouble());
                        }
                    }
                }
                pixmapRasterItem->setZValue(zValue);
                pixmapRasterItem->setPos(scenePos);
                if(setTransform)
                    pixmapRasterItem->setTransform(QTransform().scale(planParams.scale_w,planParams.scale_h).rotate(planParams.rotate));

                QGraphicsScene * _scene = (_isGeoScene ? _geoView->scene() : _pixelView->scene());
                _scene->addItem(pixmapRasterItem);

                disconnect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
                rasterTreeWidgetItem->setRaster(pixmapRasterItem, planParams.scale_w, planParams.scale_h, planParams.rotate);
                connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));

                if( ! _isGeoScene )
                {
                    _recalcSceneRectTimer.stop();
                    _recalcSceneRectTimer.start(1000);
                }
            }
        }

        if(svgTreeWidgetItem)
        {
            QFilePtr file_ptr = planFileKeeperPtr->getLocalFile();
            QFileInfo info( *(file_ptr.get()) );
            QString filePath = info.absoluteFilePath();

            QGraphicsSvgItem * svgItem = new QGraphicsSvgItem(filePath);
            PlanFileKeeper::PlanParams planParams = planFileKeeperPtr->getPlanParams();
            svgItem->setPos(QPointF(planParams.x, planParams.y));
            LayerItem * layerItem = dynamic_cast<LayerItem*>(svgTreeWidgetItem->parent()->parent());
            if(layerItem)
                svgItem->setZValue(layerItem->getLayerType() == LayerTypes::Base ? 95 : 85);

            QGraphicsScene * _scene = (_isGeoScene ? _geoView->scene() : _pixelView->scene());
            _scene->addItem(svgItem);

            disconnect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
            svgTreeWidgetItem->setSvg(svgItem);
            connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));

            if( ! _isGeoScene )
            {
                _recalcSceneRectTimer.stop();
                _recalcSceneRectTimer.start(1000);
            }
        }
    }
}

void LayersManagerForm::reload(BaseAreaPtr ptr, bool isGeoScene)
{
    bool showEtalonNode = false;
    BaseAreaPtr facilityPtr;
    switch(ptr->getType())
    {
    case BaseArea::AT_FLOOR :
        facilityPtr = ptr->getParent();
        showEtalonNode = true;
        break;
    case BaseArea::AT_FACILITY :
        facilityPtr = ptr;
        showEtalonNode = true;
        break;
    }
    if(facilityPtr)
    {
        if(_currentData.facilityId != facilityPtr->getId())
        {
            qDebug() << "====> NEW FACILITY !!!";
        }
        _currentData.facilityId = facilityPtr->getId();
    }
    else
    {
        _currentData.facilityId = 0;
    }

//-------------------------------------------
    reset(showEtalonNode);
    setDisabled(false);
    _currentData.areaPtr = ptr;
    _isGeoScene = isGeoScene;
//-------------------------------------------

    QString etalonFilePath;
    if(facilityPtr)
    {
        auto facility = BaseArea::convert<Facility>(facilityPtr);
        if(facility)
        {
            BaseFileKeeperPtr currEtalonFile = facility->getEtalonPlan();
            if(currEtalonFile)
            {
                PlanFileKeeperPtr planFileKeeperPtr = BaseFileKeeper::convert<PlanFileKeeper>(currEtalonFile);
                if(planFileKeeperPtr)
                {
                    etalonFilePath = planFileKeeperPtr->getPath();

                    LayerItem * etalonNode = getTopLevelItem(LayerTypes::Etalon);
                    RasterItem * rasterItem = new RasterItem(etalonNode, currEtalonFile->getEntityId(), etalonFilePath, ItemTypes::FacilityEtalonRaster);

                    BaseFileKeeper::FileState fileState = planFileKeeperPtr->getFileState();
                    switch(fileState)
                    {
                    case BaseFileKeeper::FS_UNSYNC : {
                        qDebug() << "reload, etalon file state : FS_UNSYNC";
                         rasterItem->setText(0, "FS_UNSYNC");
                        _loadingItems.insert(etalonFilePath, rasterItem);
                        planFileKeeperPtr->syncFile();
                    }break;
                    case BaseFileKeeper::FS_SYNC : {
                        qDebug() << "reload, etalon file state : FS_SYNC";
                        _loadingItems.insert(etalonFilePath, rasterItem);
                        slotFileLoaded(planFileKeeperPtr);
                    }break;
                    case BaseFileKeeper::FS_INVALID : {
                        qDebug() << "reload, etalon file state : FS_INVALID";
                        rasterItem->setText(0, "FS_INVALID");
                        _loadingItems.insert(etalonFilePath, rasterItem);
                        slotFileLoaded(planFileKeeperPtr);
                    }break;
                    }

                    if(ptr->getType() == BaseArea::AT_FLOOR && facility->getCoords().isEmpty() == false && facility->isHaveTransform())
                    {
                        QTransform transform = facility->getTransform();
                        QPolygonF facilitiCoordsOnFloor = transform.map(facility->getCoords());

                        QPen pen(Qt::red);
                        pen.setCosmetic(true);
                        pen.setWidth(4);
                        _facilitiCoordsOnFloorItem = new QGraphicsPolygonItem(facilitiCoordsOnFloor);
                        _facilitiCoordsOnFloorItem->setZValue(1000);
                        _facilitiCoordsOnFloorItem->setPen(pen);
                        _pixelView->scene()->addItem(_facilitiCoordsOnFloorItem);
                    }
                }
            }
        }
    }

//-------------------------------------------

    BaseFileKeeperPtrs baseFileKeeperPtrs = ptr->getFilesByType(BaseFileKeeper::FT_PLAN);
    qDebug() << "LayersManagerForm::reload, baseFileKeeperPtrs.size :" << baseFileKeeperPtrs.size();
    foreach (BaseFileKeeperPtr baseFileKeeperPtr, baseFileKeeperPtrs)
    {
        PlanFileKeeperPtr planFileKeeperPtr = BaseFileKeeper::convert<PlanFileKeeper>(baseFileKeeperPtr);
        if(planFileKeeperPtr)
        {
            QString filePath = planFileKeeperPtr->getPath();
            //! эталонный файл для здания так же приезжает как обычные файлы, поэтому его не надо обрабатывать !
            if( ! etalonFilePath.isEmpty() && filePath == etalonFilePath)
                continue;

            QString suffix = QFileInfo(filePath).suffix();
            uint64_t layerId(0);
            LayerPtr layerPtr = planFileKeeperPtr->getLayer();
            if(layerPtr)
                layerId = layerPtr->getId();

            DataItem * dataItem(nullptr);
            auto it = _layers.find(layerId);
            if(it != _layers.end())
            {
                if(suffix == QString("svg"))
                {
                    QTreeWidgetItem * parentItem = it.value()->getChild(ItemTypes::Vectors);
                    dataItem = new SvgItem(parentItem, baseFileKeeperPtr->getEntityId(), filePath);
                }
                else
                {
                    QTreeWidgetItem * parentItem = it.value()->getChild(ItemTypes::Rasters);
                    dataItem = new RasterItem(parentItem, baseFileKeeperPtr->getEntityId(), filePath);
                }
            }
            if( ! dataItem)
                continue;

            QString baseName = QFileInfo(filePath).baseName();
            qDebug() << "EntityId:" << baseFileKeeperPtr->getEntityId() << ", baseName:" << baseName << ", suffix:" << suffix << ", getPath:" << filePath;

            BaseFileKeeper::FileState fileState = planFileKeeperPtr->getFileState();
            switch(fileState)
            {
            case BaseFileKeeper::FS_UNSYNC : {
                qDebug() << "reload, FS_UNSYNC";
                dataItem->setText(0, "FS_UNSYNC");
                _loadingItems.insert(filePath, dataItem);
                planFileKeeperPtr->syncFile();
            }break;
            case BaseFileKeeper::FS_SYNC : {
                qDebug() << "reload, FS_SYNC";
                _loadingItems.insert(filePath, dataItem);
                slotFileLoaded(planFileKeeperPtr);
            }break;
            case BaseFileKeeper::FS_INVALID : {
                qDebug() << "reload, FS_INVALID";
                dataItem->setText(0, "FS_INVALID");
            }break;
            }
        }
    }
    qDebug() << "----------";

    QTimer::singleShot(10, this, SLOT(slotSyncMarks()));
}
void LayersManagerForm::slotSyncMarks()
{
    syncMarks();
}

void LayersManagerForm::slotZoomChanged(int zoomLevel)
{
    if(zoomLevel > 0)
    {
        if(_zoomState != ZoomState::OneMeter)
        {
            _zoomState = ZoomState::OneMeter;
            redrawItems(100);
        }
    }
    else if(zoomLevel > -3)
    {
        if(_zoomState != ZoomState::TenMeter)
        {
            _zoomState = ZoomState::TenMeter;
            redrawItems(1000);
        }
    }
    else
    {
        if(_zoomState != ZoomState::HundredMeter)
        {
            _zoomState = ZoomState::HundredMeter;
            redrawItems(10000);
        }
    }
    qDebug() << "LayersManagerForm::slotZoomChanged, zoomLevel : " << zoomLevel;
}

void LayersManagerForm::redrawItems(int pixelDelta)
{
    _pixelDelta = pixelDelta;

    if(_lineItemsVisible)
    {
        QTime t;
        t.start();

        foreach(QGraphicsLineItem * item, _lineItems)
            delete item;
        _lineItems.clear();

        QColor col(Qt::black);
        col.setAlpha(150);
        QPen pen1(col);
        pen1.setCosmetic(true);
        pen1.setWidth(1);

        col.setAlpha(60);
        QPen pen2(col);
        pen2.setCosmetic(true);
        pen2.setWidth(1);

        int min(-50000), max(50000);
        for(int x(min); x <= max; x = x+(pixelDelta/10))
        {
            QGraphicsLineItem * itemH = new QGraphicsLineItem(QLineF(x, min, x, max));
            itemH->setPen(x%pixelDelta == 0 ? pen1 : pen2);
            itemH->setZValue(89);
            _pixelView->scene()->addItem(itemH);
            _lineItems.append(itemH);
        }

        for(int y(min); y <= max; y = y+(pixelDelta/10))
        {
            QGraphicsLineItem * itemV = new QGraphicsLineItem(QLineF(min, y, max, y));
            itemV->setPen(y%pixelDelta == 0 ? pen1 : pen2);
            itemV->setZValue(89);
            _pixelView->scene()->addItem(itemV);
            _lineItems.append(itemV);
        }

        qDebug() << "*** elapsed :" << t.elapsed();
    }
}

void LayersManagerForm::syncMarks(bool hideAll)
{
    for(int a(0); a < ui->treeWidget->topLevelItemCount(); ++a)
    {
        LayerItem * layerItem = dynamic_cast<LayerItem*>(ui->treeWidget->topLevelItem(a));
        if(layerItem)
        {
            for(int b(0); b < layerItem->childCount(); ++b)
            {
                QTreeWidgetItem * item = layerItem->child(b);
                if(item->type() == (int)ItemTypes::Marks)
                {
                    if(hideAll)
                    {
                        QList<QVariant>list;
                        list << (uint)layerItem->getLayerId() << 0 << false;
                        CommonMessageNotifier::send( (uint)visualize_system::BusTags::LayerVisibleChanged, list, QString("visualize_system"));
                    }
                    else
                    {
                        switch(item->checkState(0))
                        {
                        case Qt::Checked : {
                            QList<QVariant>list;
                            list << (uint)layerItem->getLayerId() << 0 << true;
                            CommonMessageNotifier::send( (uint)visualize_system::BusTags::LayerVisibleChanged, list, QString("visualize_system"));
                        }break;
                        case Qt::Unchecked : {
                            QList<QVariant>list;
                            list << (uint)layerItem->getLayerId() << 0 << false;
                            CommonMessageNotifier::send( (uint)visualize_system::BusTags::LayerVisibleChanged, list, QString("visualize_system"));
                        }break;
                        case Qt::PartiallyChecked : {
                            for(int c(0); c < item->childCount(); ++c)
                            {
                                QTreeWidgetItem * markItem = item->child(c);
                                if(markItem->checkState(0) == Qt::Unchecked)
                                {
                                    int type(0);
                                    switch(markItem->type())
                                    {
                                    case (int)ItemTypes::Defect : {
                                        type = 1;
                                    }break;
                                    case (int)ItemTypes::Photo : {
                                        type = 2;
                                    }break;
                                    case (int)ItemTypes::Photo3d : {
                                        type = 3;
                                    }break;
                                    }
                                    QList<QVariant>list;
                                    list << (uint)layerItem->getLayerId() << type << false;
                                    CommonMessageNotifier::send( (uint)visualize_system::BusTags::LayerVisibleChanged, list, QString("visualize_system"));
                                }
                            }
                        }break;
                        }
                    }
                }
            }
        }
    }
}

void LayersManagerForm::slotItemChanged(QTreeWidgetItem *item, int /*column*/)
{
    if(item->type() == (int)ItemTypes::Layer)
    {
//        switch(item->checkState(0))
//        {
//        case Qt::Checked : qDebug() << "layer state ==  Checked"; break;
//        case Qt::PartiallyChecked : qDebug() << "layer state == PartiallyChecked"; break;
//        case Qt::Unchecked : qDebug() << "layer state == Unchecked"; break;
//        }

        _blockRecalcLayer = true;
        for(int a(0); a<item->childCount() ; ++a)
            item->child(a)->setCheckState(0, item->checkState(0));
        _blockRecalcLayer = false;
    }
    else
    {
        uint layerId(0);
        QTreeWidgetItem * _item(item);
        while(_item)
        {
            //qDebug() << "===>" << _item->text(0) << (int)_item->type();
            if(_item->type() == (int)ItemTypes::Layer)
            {
                LayerItem * layerItem = dynamic_cast<LayerItem*>(_item);
                if(layerItem)
                    layerId = layerItem->getLayerId();
                break;
            }
            _item = _item->parent();
        }

        disconnect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
        bool recalcParent(false), recalcLayer(false);
        bool setVisible(item->checkState(0) == Qt::Checked);
        switch(item->type())
        {
        case (int)ItemTypes::Rasters :
        {
            for(int a(0); a<item->childCount() ; ++a)
            {
                RasterItem * subItem = dynamic_cast<RasterItem *>(item->child(a));
                if(subItem)
                {
                    subItem->setCheckState(0, setVisible ? Qt::Checked : Qt::Unchecked);
                    if(subItem->getRaster())
                        subItem->getRaster()->setVisible(setVisible);
                }
            }
            if(item->childCount() > 0)
                recalcLayer = true;
        }break;
        case (int)ItemTypes::Raster :
        {
            RasterItem * subItem = dynamic_cast<RasterItem *>(item);
            if(subItem)
            {
                subItem->setCheckState(0, setVisible ? Qt::Checked : Qt::Unchecked);
                if(subItem->getRaster())
                    subItem->getRaster()->setVisible(setVisible);
            }
            recalcParent = true;
        }break;
        case (int)ItemTypes::Vectors : {
            for(int a(0); a<item->childCount() ; ++a)
            {
                SvgItem * subItem = dynamic_cast<SvgItem *>(item->child(a));
                if(subItem)
                {
                    subItem->setCheckState(0, setVisible ? Qt::Checked : Qt::Unchecked);
                    if(subItem->getSvg())
                        subItem->getSvg()->setVisible(setVisible);
                }
            }
            if(item->childCount() > 0)
                recalcLayer = true;
        }break;
        case (int)ItemTypes::Vector : {
            SvgItem * subItem = dynamic_cast<SvgItem *>(item);
            if(subItem)
            {
                subItem->setCheckState(0, setVisible ? Qt::Checked : Qt::Unchecked);
                if(subItem->getSvg())
                    subItem->getSvg()->setVisible(setVisible);
            }
            recalcParent = true;
        }break;
        case (int)ItemTypes::Marks : {
            for(int a(0); a<item->childCount() ; ++a)
                item->child(a)->setCheckState(0, setVisible ? Qt::Checked : Qt::Unchecked);
            QList<QVariant>list;
            list << layerId << 0 << setVisible;
            CommonMessageNotifier::send( (uint)visualize_system::BusTags::LayerVisibleChanged, list, QString("visualize_system"));
            recalcLayer = true;
        }break;
        case (int)ItemTypes::Defect : {
            QList<QVariant>list;
            list << layerId << 1 << setVisible;
            CommonMessageNotifier::send( (uint)visualize_system::BusTags::LayerVisibleChanged, list, QString("visualize_system"));
            recalcParent = true;
        }break;
        case (int)ItemTypes::Photo : {
            QList<QVariant>list;
            list << layerId << 2 << setVisible;
            CommonMessageNotifier::send( (uint)visualize_system::BusTags::LayerVisibleChanged, list, QString("visualize_system"));
            recalcParent = true;
        }break;
        case (int)ItemTypes::Photo3d : {
            QList<QVariant>list;
            list << layerId << 3 << setVisible;
            CommonMessageNotifier::send( (uint)visualize_system::BusTags::LayerVisibleChanged, list, QString("visualize_system"));
            recalcParent = true;
        }break;

        case (int)ItemTypes::FacilityEtalonRaster : {
        {
            RasterItem * subItem = dynamic_cast<RasterItem *>(item);
            if(subItem)
            {
                subItem->setCheckState(0, setVisible ? Qt::Checked : Qt::Unchecked);
                if(subItem->getRaster())
                    subItem->getRaster()->setVisible(setVisible);
            }
            recalcLayer = true;
        }break;

        }break;
        case (int)ItemTypes::FacilityPolygonOnPlan : {


        }break;

        }

        if(recalcParent)
        {
            int cheched(0), ucheched(0);
            QTreeWidgetItem * parentItem = item->parent();
            for(int a(0); a<parentItem->childCount() ; ++a)
            {
                if(parentItem->child(a)->checkState(0) == Qt::Checked)
                    ++cheched;
                else
                    ++ucheched;
            }
            if(cheched == 0 && ucheched > 0)
                parentItem->setCheckState(0, Qt::Unchecked);
            else if(cheched > 0 && ucheched == 0)
                parentItem->setCheckState(0, Qt::Checked);
            else
                parentItem->setCheckState(0, Qt::PartiallyChecked);

            recalcLayer = true;
        }

        if( !_blockRecalcLayer && recalcLayer)
        {
            QTreeWidgetItem * layerItem(0);
            while(item)
            {
                if(item->type() == (int)ItemTypes::Layer)
                {
                    layerItem = item;
                    break;
                }
                item = item->parent();
            }

            if(layerItem)
            {
                int cheched(0), partially(0), ucheched(0);
                for(int a(0); a<layerItem->childCount() ; ++a)
                    switch(layerItem->child(a)->checkState(0))
                    {
                    case Qt::Checked : ++cheched; break;
                    case Qt::PartiallyChecked : ++partially; break;
                    case Qt::Unchecked : ++ucheched; break;
                    }

                if( partially > 0 || (cheched > 0 && ucheched > 0) )
                    layerItem->setCheckState(0, Qt::PartiallyChecked);
                else if(cheched > 0 && partially == 0 && ucheched == 0)
                    layerItem->setCheckState(0, Qt::Checked);
                else if(cheched == 0 && partially == 0 && ucheched > 0)
                    layerItem->setCheckState(0, Qt::Unchecked);
                else // cheched == 0 && partially == 0 && ucheched == 0
                    layerItem->setCheckState(0, Qt::Checked);
            }
        }
        connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
    }
}

void LayersManagerForm::slotAddEntity()
{
    QList<QTreeWidgetItem*> list = ui->treeWidget->selectedItems();
    if(list.isEmpty())
        return;

    QTreeWidgetItem* item = list.first();
    switch(item->type())
    {
    case (int)ItemTypes::Rasters : {

        LayerItem * layerItem = dynamic_cast<LayerItem*>(item->parent());
        if( ! layerItem)
            return;

        QString filePath = QFileDialog::getOpenFileName(this, QString::fromUtf8("Выберите изображение для слоя \"") + layerItem->text(0) + QString("\""));
        if(filePath.isEmpty() == false)
        {
            QPixmap pixmap(filePath);
            if(pixmap.isNull())
            {
                QMessageBox::information(this, QString::fromUtf8("Внимание"), QString::fromUtf8("Выбранный файл не является изображением !"));
            }
            else
            {
                syncMarks(true);

                _currentData.clear();
                _currentData.state = CurrentData::Create;
                _currentData.object = CurrentData::Raster;
                if(layerItem->getLayerType() == LayerTypes::Other)
                    _currentData.layerId = layerItem->getLayerId();

                int zValue(_currentData.layerId != 0 ? 1000000 : 86);
                _instrumentalForm = new LayerInstrumentalForm( (_isGeoScene ? _geoVisId : _pixelVisId ), pixmap, zValue);
                connect(_instrumentalForm, SIGNAL(signalRasterSaved(RasterSaveDatad)), this, SLOT(slotRasterSaved(RasterSaveDatad)));

                _ifaceInstrumentalForm = new EmbIFaceNotifier(_instrumentalForm);

                QString tag("LayerInstrumentalForm");
                quint64 widgetId = ewApp()->restoreWidget(tag, _ifaceInstrumentalForm);
                if(0 == widgetId)
                {
                    ew::EmbeddedWidgetStruct struc;
                    ew::EmbeddedHeaderStruct headStr;
                    headStr.hasCloseButton = true;
                    headStr.windowTitle = QString("Изображение");
                    headStr.headerPixmap = QString(":/img/icon_edit_image.png");
                    struc.widgetTag = tag;
                    struc.minSize = QSize(245,25);
                    struc.maxSize = QSize(245,250);
                    struc.size = QSize(245,25);
                    struc.header = headStr;
                    struc.iface = _ifaceInstrumentalForm;
                    struc.topOnHint = true;
                    struc.isModal = false;
                    //visualize_system::ViewInterface * viewInterface = visualize_system::VisualizerManager::instance()->getViewInterface(_geoVisId);
                    widgetId = ewApp()->createWidget(struc); //, viewInterface->getVisualizerWindowId());
                }
                _instrumentalForm->setEmbeddedWidgetId(widgetId);
                connect(_ifaceInstrumentalForm, SIGNAL(signalClosed()), this, SLOT(slotEditorFormClose()));
                ewApp()->setVisible(_ifaceInstrumentalForm->id(), true);
                ewApp()->setVisible(_embeddedWidgetId, false);

                if( ! _isGeoScene)
                {
                    int min(-50000), max(50000);
                    QRectF r(QPointF(min, min), QPointF(max, max));
                    qDebug() << "===> setSceneRect" << r;
                    _pixelView->setSceneRect(r);
                }

                bool block(true);
                CommonMessageNotifier::send( (uint)visualize_system::BusTags::BlockGUI, QVariant(block), QString("visualize_system"));
            }
        }
    }break;
    case (int)ItemTypes::Vectors : {

        LayerItem * layerItem = dynamic_cast<LayerItem*>(item->parent());
        if( ! layerItem)
            return;

        syncMarks(true);

        _currentData.clear();
        _currentData.state = CurrentData::Create;
        _currentData.object = CurrentData::Vector;
        if(layerItem->getLayerType() == LayerTypes::Other)
            _currentData.layerId = layerItem->getLayerId();

        _svgEditorForm = new SvgEditorForm( (_isGeoScene ? _geoVisId : _pixelVisId ), _currentData.layerId != 0);
        connect(_svgEditorForm, SIGNAL(svgDocSaved(QString,QPointF)), this, SLOT(slotSvgSaved(QString,QPointF)));

        QString tag("LayerSvgEditorForm");
        quint64 widgetId = 0; //ewApp()->restoreWidget(tag, _svgEditorForm);
        if(0 == widgetId)
        {
            ew::EmbeddedWidgetStruct struc;
            ew::EmbeddedHeaderStruct headStr;
            headStr.hasCloseButton = true;
            headStr.windowTitle = QString::fromUtf8("векторные данные");
            headStr.headerPixmap = QString(":/img/icon_edit_image.png");
            struc.widgetTag = tag;
            struc.minSize = QSize(300,25);
            struc.maxSize = QSize(500,200);
            struc.size = QSize(400,25);
            struc.header = headStr;
            struc.iface = _svgEditorForm;
            struc.topOnHint = true;
            struc.isModal = false;
            widgetId = ewApp()->createWidget(struc); //, viewInterface->getVisualizerWindowId());
        }
        connect(_svgEditorForm, SIGNAL(signalClosed()), this, SLOT(slotEditorFormClose()));
        ewApp()->setVisible(_svgEditorForm->id(), true);
        ewApp()->setVisible(_embeddedWidgetId, false);

        if( ! _isGeoScene)
        {
            int min(-50000), max(50000);
            QRectF r(QPointF(min, min), QPointF(max, max));
            qDebug() << "===> setSceneRect" << r;
            _pixelView->setSceneRect(r);
        }

        bool block(true);
        CommonMessageNotifier::send( (uint)visualize_system::BusTags::BlockGUI, QVariant(block), QString("visualize_system"));
    }break;
    default : {

        LayerItem * layerItem = dynamic_cast<LayerItem*>(item);
        if( ! layerItem)
            return;

        if(layerItem->getLayerType() == LayerTypes::Etalon) //! формирование опорного изображения
        {
            bool hasVisibleItems(false);
            for(int a(0); a < ui->treeWidget->topLevelItemCount(); ++a)
            {
                LayerItem * layerItem = dynamic_cast<LayerItem*>(ui->treeWidget->topLevelItem(a));
                if(layerItem)
                {
                    QTreeWidgetItem * rastersItem = layerItem->getChild(ItemTypes::Rasters);
                    if(rastersItem)
                        for(int b(0); b < rastersItem->childCount(); ++b)
                            if(rastersItem->child(b)->checkState(0) == Qt::Checked)
                            {
                                hasVisibleItems = true;
                                break;
                            }

                    QTreeWidgetItem * vectorsItem = layerItem->getChild(ItemTypes::Vectors);
                    if(vectorsItem)
                        for(int b(0); b < vectorsItem->childCount(); ++b)
                            if(vectorsItem->child(b)->checkState(0) == Qt::Checked)
                            {
                                hasVisibleItems = true;
                                break;
                            }

                    if(hasVisibleItems)
                        break;
                }
            }
            if(hasVisibleItems == false)
            {
                QMessageBox::information(this, QString::fromUtf8("Внимание"), QString::fromUtf8("На плане этажа нет изображений или векторных данных или для них не выставлена видимость !"));
                return;
            }

            QString text = QString::fromUtf8("Сформировать опорное изображение для здания из видимых слоев данного этажа ?");
            if(QMessageBox::Yes != QMessageBox::question(this, QString::fromUtf8("Внимание"), text, QMessageBox::Yes, QMessageBox::No))
                return;

            WaitDialogUnlimited waitDialogUnlimited;
            QTime t;
            t.start();
            while(t.elapsed() < 1000)
                qApp->processEvents();

            RasterItem * facilityEtalonRaster = dynamic_cast<RasterItem*>(layerItem->getChild(ItemTypes::FacilityEtalonRaster));
            if(facilityEtalonRaster)
            {
                _loadingItems.remove(facilityEtalonRaster->getPath());
                disconnect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
                delete facilityEtalonRaster;
                connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
            }

            // hide marks, lines and rooms
            bool isVisible(false);
            CommonMessageNotifier::send( (uint)visualize_system::BusTags::SetRoomVisibleOnFloor, QVariant(isVisible), QString("visualize_system"));
            syncMarks(true);
            if(_lineItemsVisible)
            {
                foreach(QGraphicsLineItem * item, _lineItems)
                    delete item;
                _lineItems.clear();
            }
            if(_facilitiCoordsOnFloorItem)
            {
                delete _facilitiCoordsOnFloorItem;
                _facilitiCoordsOnFloorItem = nullptr;
            }

            //------------------------------------------

            QRectF rectOnScene = _pixelView->scene()->itemsBoundingRect();
            QPixmap pixmap(rectOnScene.size().toSize());
            {
                QPainter painter(&pixmap);
                _pixelView->scene()->render(&painter, QRectF(QPointF(0,0), pixmap.size()) , rectOnScene);
                bool resPainterEnd = painter.end();
                qDebug() << "-----------> resPainterEnd :" << resPainterEnd << ", rectOnScene :" << rectOnScene;
            }
            if(pixmap.isNull())
            {
                qDebug() << "-----------> pixmap.isNull() !!! , rectOnScene :" << rectOnScene;
            }
            else
            {
                QString filePath = TempDirController::createTempDirForCurrentUser() + QDir::separator() + "etalon.tif";
                bool res = pixmap.save(filePath, "TIF");
                if(res)
                {
                    BaseAreaPtr facilityPtr = _currentData.areaPtr->getParent();

                    QPolygonF locationPolygon;
                    BaseAreaPtr locationPtr = facilityPtr->getParent();
                    if(locationPtr)
                        locationPolygon = locationPtr->getCoords();

                    PlanFileKeeper::PlanParams planParams;

                    if(locationPolygon.isEmpty() == false)
                    {
                        double lenghtOnPlanFlorCoord = rectOnScene.width();
                        QPointF locationInMapCoordCenter = locationPolygon.boundingRect().center();
                        double meterInMapCoord = GeographicUtils::meterInSceneCoord(locationInMapCoordCenter);
                        double widthFacilityInMeters = lenghtOnPlanFlorCoord / 100.;
                        double lenghtOnGeoSceneCoord = meterInMapCoord * widthFacilityInMeters;

                        double scale = lenghtOnGeoSceneCoord / lenghtOnPlanFlorCoord;

                        planParams.scale_w = scale;
                        planParams.scale_h = scale;
                        planParams.rotate = 0;
                        planParams.x = locationInMapCoordCenter.x();
                        planParams.y = locationInMapCoordCenter.y();

                        QString posOnPlanFloor = QString::number(rectOnScene.x(), 'f', 5) + QString(" ") + QString::number(rectOnScene.y(), 'f', 5);
                        facilityPtr->addMetadata("string", "position_etalon_image_on_plan_floor", posOnPlanFloor);
                    }


                    BaseFileKeeperPtr basePlan = RegionBizManager::instance()->addFile(filePath, BaseFileKeeper::FT_PLAN, facilityPtr->getId());
                    PlanFileKeeperPtr plan = BaseFileKeeper::convert<PlanFileKeeper>(basePlan);
                    if( ! plan)
                        return;

                    plan->setPlanParams(planParams);
                    bool resCommit = basePlan->commit();

                    auto facility = BaseArea::convert<Facility>(facilityPtr);
                    if(facility)
                    {
                        BaseFileKeeperPtr currEtalonFile = facility->getEtalonPlan();
                        if(currEtalonFile)
                            RegionBizManager::instance()->deleteFile(currEtalonFile);

                        facility->setEtalonPlan(basePlan);

                        facility->resetTransform();
                        facility->commitTransformMatrix();

                        resCommit = facility->commit();
                    }

                    LayerItem * etalonNode = getTopLevelItem(LayerTypes::Etalon);
                    if(etalonNode)
                    {
                        RasterItem * rasterTreeWidgetItem = new RasterItem(etalonNode, plan->getEntityId(), plan->getPath());
                        etalonNode->setExpanded(true);
                        //rasterTreeWidgetItem->setSelected(true);

                        BaseFileKeeper::FileState fileState = plan->getFileState();
                        switch(fileState)
                        {
                        case BaseFileKeeper::FS_UNSYNC : {
                            qDebug() << "slotRasterSaved, FS_UNSYNC";
                            rasterTreeWidgetItem->setText(0, "FS_UNSYNC");
                            _loadingItems.insert(plan->getPath(), rasterTreeWidgetItem);
                            plan->syncFile();
                        }break;
                        case BaseFileKeeper::FS_SYNC : {
                            qDebug() << "slotRasterSaved, FS_SYNC";
                            _loadingItems.insert(plan->getPath(), rasterTreeWidgetItem);
                            slotFileLoaded(plan);
                        }break;
                        case BaseFileKeeper::FS_INVALID : {
                            qDebug() << "slotRasterSaved, FS_INVALID";
                            rasterTreeWidgetItem->setText(0, "FS_INVALID");
                        }break;
                        }
                    }
                }
            }

            //------------------------------------------

            // sync visible for marks, lines and rooms
            isVisible = true;
            CommonMessageNotifier::send( (uint)visualize_system::BusTags::SetRoomVisibleOnFloor, QVariant(isVisible), QString("visualize_system"));
            syncMarks();
            redrawItems(_pixelDelta);
        }
    }
    }
}

void LayersManagerForm::slotRasterSaved(RasterSaveDatad data)
{
    auto it = _layers.find(_currentData.layerId);
    if(it == _layers.end())
        return;
    QTreeWidgetItem * rastersNode(nullptr);
    for(int a(0); a<it.value()->childCount(); ++a)
        if(it.value()->child(a)->type() == (int)ItemTypes::Rasters )
        {
            rastersNode = it.value()->child(a);
            break;
        }
    if( ! rastersNode)
        return;

    BaseFileKeeperPtr basePlan = RegionBizManager::instance()->addFile(data.filePath, BaseFileKeeper::FT_PLAN, _currentData.areaPtr->getId());
    PlanFileKeeperPtr plan = BaseFileKeeper::convert<PlanFileKeeper>(basePlan);
    if( ! plan)
        return;

    PlanFileKeeper::PlanParams planParams;
    planParams.scale_w = data.scaleW;
    planParams.scale_h = data.scaleH;
    planParams.rotate = data.rotate;
    planParams.x = data.scenePos.x();
    planParams.y = data.scenePos.y();
    plan->setPlanParams(planParams);

    LayerPtr layer = RegionBizManager::instance()->getLayer(_currentData.layerId);
    if(layer)
    {
        plan->moveToLayer(layer);
        layer->commit();
    }
    plan->commit();

    RasterItem * rasterTreeWidgetItem(nullptr);
    if(_currentData.state == CurrentData::Create)
    {
        rasterTreeWidgetItem = new RasterItem(rastersNode, plan->getEntityId(), plan->getPath());
    }
    else if(_currentData.state == CurrentData::Edit)
    {
        BaseFileKeeperPtrs baseFileKeeperPtrs = _currentData.areaPtr->getFilesByType(BaseFileKeeper::FT_PLAN);
        foreach (BaseFileKeeperPtr baseFileKeeperPtr, baseFileKeeperPtrs)
            if(baseFileKeeperPtr->getPath() == _currentData.planPath)
            {
                RegionBizManager::instance()->deleteFile(baseFileKeeperPtr);
                break;
            }

        for(int a(0); a < rastersNode->childCount(); ++a)
        {
            RasterItem* rasterItem = dynamic_cast<RasterItem*>(rastersNode->child(a));
            if(rasterItem->getPath() == _currentData.planPath)
            {
                rasterItem->reinit(plan->getPath());
                rasterTreeWidgetItem = rasterItem;
                break;
            }
        }
    }

    _currentData.clear();
    slotEditorFormClose();

    if(rasterTreeWidgetItem)
    {
        rastersNode->setExpanded(true);
        //rasterTreeWidgetItem->setSelected(true);

        BaseFileKeeper::FileState fileState = plan->getFileState();
        switch(fileState)
        {
        case BaseFileKeeper::FS_UNSYNC : {
            qDebug() << "slotRasterSaved, FS_UNSYNC";
            rasterTreeWidgetItem->setText(0, "FS_UNSYNC");
            _loadingItems.insert(plan->getPath(), rasterTreeWidgetItem);
            plan->syncFile();
        }break;
        case BaseFileKeeper::FS_SYNC : {
            qDebug() << "slotRasterSaved, FS_SYNC";
            _loadingItems.insert(plan->getPath(), rasterTreeWidgetItem);
            slotFileLoaded(plan);
        }break;
        case BaseFileKeeper::FS_INVALID : {
            qDebug() << "slotRasterSaved, FS_INVALID";
            rasterTreeWidgetItem->setText(0, "FS_INVALID");
        }break;
        }
    }
}

void LayersManagerForm::slotEtalotRasterSaved(RasterSaveDatad data)
{
    BaseFileKeeperPtr basePlan = RegionBizManager::instance()->addFile(data.filePath, BaseFileKeeper::FT_PLAN, _currentData.areaPtr->getId());
    PlanFileKeeperPtr plan = BaseFileKeeper::convert<PlanFileKeeper>(basePlan);
    if( ! plan)
        return;

    PlanFileKeeper::PlanParams planParams;
    planParams.scale_w = data.scaleW;
    planParams.scale_h = data.scaleH;
    planParams.rotate = data.rotate;
    planParams.x = data.scenePos.x();
    planParams.y = data.scenePos.y();
    plan->setPlanParams(planParams);
    plan->commit();

    RasterItem * rasterTreeWidgetItem(nullptr);
    LayerItem * layerItem = getTopLevelItem(LayerTypes::Etalon);
    if(layerItem)
        rasterTreeWidgetItem = dynamic_cast<RasterItem*>(layerItem->getChild(ItemTypes::FacilityEtalonRaster));

    auto facility = BaseArea::convert<Facility>(_currentData.areaPtr);
    if(facility)
    {
        BaseFileKeeperPtr currEtalonFile = facility->getEtalonPlan();
        if(currEtalonFile)
            RegionBizManager::instance()->deleteFile(currEtalonFile);

        facility->setEtalonPlan(basePlan);
        bool resCommit = facility->commit();

        QPolygonF planOfFloorPolygon;
        BaseMetadataPtr posStrPtr = _currentData.areaPtr->getMetadata("position_etalon_image_on_plan_floor");
        if(posStrPtr)
        {
            QString posStr = posStrPtr->getValueAsString();
            QStringList list = posStr.split(QString(" "));
            if(list.size() == 2)
            {
                QPointF planOfFloorPos = QPointF(list.first().toDouble(), list.last().toDouble());
                planOfFloorPolygon << planOfFloorPos
                                   << QPointF(planOfFloorPos.x() + data.pixmapWidth, planOfFloorPos.y())
                                   << QPointF(planOfFloorPos.x() + data.pixmapWidth, planOfFloorPos.y()  + data.pixmapHeight)
                                   << QPointF(planOfFloorPos.x(), planOfFloorPos.y()  + data.pixmapHeight);
            }
        }
        if(planOfFloorPolygon.isEmpty() == false)
        {
            QTransform transform;
            QPolygonF geoScenePolygon = data.vertex;
            if(QTransform::quadToQuad(geoScenePolygon, planOfFloorPolygon, transform))
            {
                facility->setTransform(transform);
                resCommit = facility->commitTransformMatrix();
            }
        }
    }

    _currentData.clear();
    slotEditorFormClose();

    if(rasterTreeWidgetItem)
    {
        rasterTreeWidgetItem->reinit(plan->getPath());

        BaseFileKeeper::FileState fileState = plan->getFileState();
        switch(fileState)
        {
        case BaseFileKeeper::FS_UNSYNC : {
            qDebug() << "slotRasterSaved, FS_UNSYNC";
            rasterTreeWidgetItem->setText(0, "FS_UNSYNC");
            _loadingItems.insert(plan->getPath(), rasterTreeWidgetItem);
            plan->syncFile();
        }break;
        case BaseFileKeeper::FS_SYNC : {
            qDebug() << "slotRasterSaved, FS_SYNC";
            _loadingItems.insert(plan->getPath(), rasterTreeWidgetItem);
            slotFileLoaded(plan);
        }break;
        case BaseFileKeeper::FS_INVALID : {
            qDebug() << "slotRasterSaved, FS_INVALID";
            rasterTreeWidgetItem->setText(0, "FS_INVALID");
        }break;
        }
    }
}

void LayersManagerForm::slotSvgSaved(QString filePath, QPointF scenePos)
{
    auto it = _layers.find(_currentData.layerId);
    if(it == _layers.end())
        return;
    QTreeWidgetItem * vectorsNode(nullptr);
    for(int a(0); a<it.value()->childCount(); ++a)
        if(it.value()->child(a)->type() == (int)ItemTypes::Vectors )
        {
            vectorsNode = it.value()->child(a);
            break;
        }
    if( ! vectorsNode)
        return;

    BaseFileKeeperPtr basePlan = RegionBizManager::instance()->addFile(filePath, BaseFileKeeper::FT_PLAN, _currentData.areaPtr->getId());
    PlanFileKeeperPtr plan = BaseFileKeeper::convert<PlanFileKeeper>(basePlan);
    if( ! plan)
        return;

    PlanFileKeeper::PlanParams planParams;
    planParams.scale_w = 0;
    planParams.scale_h = 0;
    planParams.rotate = 0;
    planParams.x = scenePos.x();
    planParams.y = scenePos.y();
    plan->setPlanParams(planParams);

    LayerPtr layer = RegionBizManager::instance()->getLayer(_currentData.layerId);
    if(layer)
    {
        plan->moveToLayer(layer);
        layer->commit();
    }
    plan->commit();

    SvgItem * svgItem(nullptr);
    if(_currentData.state == CurrentData::Create)
    {
        svgItem = new SvgItem(vectorsNode, plan->getEntityId(), plan->getPath());
    }
//    else if(_currentData.state == CurrentData::Edit)
//    {
//        BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(_currentData.areaId);
//        BaseFileKeeperPtrs baseFileKeeperPtrs = ptr->getFilesByType(BaseFileKeeper::FT_PLAN);
//        foreach (BaseFileKeeperPtr baseFileKeeperPtr, baseFileKeeperPtrs)
//            if(baseFileKeeperPtr->getPath() == _currentData.planPath)
//            {
//                RegionBizManager::instance()->deleteFile(baseFileKeeperPtr);
//                break;
//            }

//        for(int a(0); a < rastersNode->childCount(); ++a)
//        {
//            RasterItem* rasterItem = dynamic_cast<RasterItem*>(rastersNode->child(a));
//            if(rasterItem->getPath() == _currentData.planPath)
//            {
//                rasterItem->reinit(plan->getPath());
//                rasterTreeWidgetItem = rasterItem;
//                break;
//            }
//        }
//    }

    _currentData.clear();
    slotEditorFormClose();

    if(svgItem)
    {
        vectorsNode->setExpanded(true);
        //svgItem->setSelected(true);

        BaseFileKeeper::FileState fileState = plan->getFileState();
        switch(fileState)
        {
        case BaseFileKeeper::FS_UNSYNC : {
            qDebug() << "slotSvgSaved, FS_UNSYNC";
            svgItem->setText(0, "FS_UNSYNC");
            _loadingItems.insert(plan->getPath(), svgItem);
            plan->syncFile();
        }break;
        case BaseFileKeeper::FS_SYNC : {
            qDebug() << "slotSvgSaved, FS_SYNC";
            _loadingItems.insert(plan->getPath(), svgItem);
            slotFileLoaded(plan);
        }break;
        case BaseFileKeeper::FS_INVALID : {
            qDebug() << "slotSvgSaved, FS_INVALID";
            svgItem->setText(0, "FS_INVALID");
        }break;
        }
    }
}

void LayersManagerForm::slotEditEntity()
{
    QList<QTreeWidgetItem*> list = ui->treeWidget->selectedItems();
    if(list.isEmpty())
        return;

    QTreeWidgetItem* item = list.first();
    if(item->type() == (int)ItemTypes::Raster || item->type() == (int)ItemTypes::FacilityEtalonRaster)
    {
        RasterItem * rasterItem = dynamic_cast<RasterItem*>(item);
        if( ! rasterItem)
            return;

        QGraphicsPixmapItem * graphicsPixmapItem = rasterItem->getRaster();
        if( ! graphicsPixmapItem)
            return;

        syncMarks(true);
        graphicsPixmapItem->hide();

        _currentData.clear();
        _currentData.state = CurrentData::Edit;
        _currentData.object = CurrentData::Raster;
        _currentData.planPath = rasterItem->getPath();

        LayerItem * layerItem = dynamic_cast<LayerItem*>(item->parent()->parent());
        if(layerItem)
            if(layerItem->getLayerType() == LayerTypes::Other)
                _currentData.layerId = layerItem->getLayerId();

        int zValue(_currentData.layerId != 0 ? 1000000 : 86);
        if(item->type() == (int)ItemTypes::Raster)
        {
//            _instrumentalForm = new LayerInstrumentalForm( (_isGeoScene ? _geoVisId : _pixelVisId ), graphicsPixmapItem->pixmap(), graphicsPixmapItem->scenePos(), rasterItem->getScW(), rasterItem->getScH(), rasterItem->getRotate(), zValue);
//            connect(_instrumentalForm, SIGNAL(signalRasterSaved(RasterSaveDatad)), this, SLOT(slotRasterSaved(RasterSaveDatad)));

            //_facilitiCoordsOnFloorItem->hide();
            QPolygonF polygon = _facilitiCoordsOnFloorItem->polygon();
            _instrumentalForm = new LayerInstrumentalForm(_pixelVisId, polygon, 1000000);

        }
        if(item->type() == (int)ItemTypes::FacilityEtalonRaster)
        {
            _instrumentalForm = new LayerInstrumentalForm( (_isGeoScene ? _geoVisId : _pixelVisId ), graphicsPixmapItem->pixmap(), graphicsPixmapItem->scenePos(), rasterItem->getScW(), rasterItem->getScH(), rasterItem->getRotate(), zValue);
            _instrumentalForm->setModeMoveAndRotateOnly();
            connect(_instrumentalForm, SIGNAL(signalRasterSaved(RasterSaveDatad)), this, SLOT(slotEtalotRasterSaved(RasterSaveDatad)));
        }

        _ifaceInstrumentalForm = new EmbIFaceNotifier(_instrumentalForm);

        QString tag("LayerInstrumentalForm");
        quint64 widgetId = ewApp()->restoreWidget(tag, _ifaceInstrumentalForm);
        if(0 == widgetId)
        {
            ew::EmbeddedWidgetStruct struc;
            ew::EmbeddedHeaderStruct headStr;
            headStr.hasCloseButton = true;
            headStr.windowTitle = QString("Изображение");
            headStr.headerPixmap = QString(":/img/icon_edit_image.png");
            struc.widgetTag = tag;
            struc.minSize = QSize(245,25);
            struc.maxSize = QSize(245,250);
            struc.size = QSize(245,25);
            struc.header = headStr;
            struc.iface = _ifaceInstrumentalForm;
            struc.topOnHint = true;
            struc.isModal = false;
            //visualize_system::ViewInterface * viewInterface = visualize_system::VisualizerManager::instance()->getViewInterface(_geoVisId);
            widgetId = ewApp()->createWidget(struc); //, viewInterface->getVisualizerWindowId());
        }
        ewApp()->setWidgetTitle(widgetId, QString("Изображение"));
        _instrumentalForm->setEmbeddedWidgetId(widgetId);
        connect(_ifaceInstrumentalForm, SIGNAL(signalClosed()), this, SLOT(slotEditorFormClose()));
        ewApp()->setVisible(_ifaceInstrumentalForm->id(), true);
        ewApp()->setVisible(_embeddedWidgetId, false);

        bool block(true);
        CommonMessageNotifier::send( (uint)visualize_system::BusTags::BlockGUI, QVariant(block), QString("visualize_system"));
    }
    else if(item->type() == (int)ItemTypes::Vector)
    {
        QMessageBox::information(this, QString::fromUtf8("Внимание"), QString::fromUtf8("Редактирование данных в разработке ! :)"));

    }
}

void LayersManagerForm::slotDeleteEntity()
{
    QList<QTreeWidgetItem*> list = ui->treeWidget->selectedItems();
    if(list.isEmpty() == false)
    {
        QTreeWidgetItem* item = list.first();
        if(item->type() == (int)ItemTypes::Raster || item->type() == (int)ItemTypes::Vector)
        {
            DataItem * dataItem = dynamic_cast<DataItem*>(item);
            if(dataItem)
            {
                auto mngr = RegionBizManager::instance();
                BaseEntityPtr ptr = mngr->getBaseEntity(dataItem->getEntityId());
                if(ptr)
                {
                    BaseFileKeeperPtrs baseFileKeeperPtrs = ptr->getFilesByType(BaseFileKeeper::FT_PLAN);
                    foreach(BaseFileKeeperPtr baseFileKeeperPtr, baseFileKeeperPtrs)
                        if(baseFileKeeperPtr->getPath() == dataItem->getPath())
                        {
                            bool res = mngr->deleteFile(baseFileKeeperPtr);
                            qDebug() << "slotDeleteEntity, getPath :" << dataItem->getPath() << ", res:" << res;
                            _loadingItems.remove(dataItem->getPath());
                            delete item;

                            break;
                        }
                }
            }
        }
    }
}

void LayersManagerForm::slotSelectionChanged()
{
    ui->addEntity->setDisabled(true);
    ui->editEntity->setDisabled(true);
    ui->deleteEntity->setDisabled(true);
    ui->deleteLayer->setDisabled(true);

    QList<QTreeWidgetItem*> list = ui->treeWidget->selectedItems();
    if(list.isEmpty() == false && _currentData.areaPtr)
    {
        QTreeWidgetItem* item = list.first();

        LayerItem * layerItem = dynamic_cast<LayerItem*>(item);
        if(layerItem)
        {
            if(_currentData.areaPtr->getType() == BaseArea::AT_FLOOR && layerItem->getLayerType() == LayerTypes::Etalon)
                ui->addEntity->setEnabled(true);

            if(layerItem->getLayerType() == LayerTypes::Other)
                ui->deleteLayer->setEnabled(true);
        }

        if(item->type() == (int)ItemTypes::Rasters || item->type() == (int)ItemTypes::Vectors)
            ui->addEntity->setEnabled(true);

        if(item->type() == (int)ItemTypes::Raster || item->type() == (int)ItemTypes::Vector)
        {
            ui->editEntity->setEnabled(true);
            ui->deleteEntity->setEnabled(true);
        }

        if(_currentData.areaPtr->getType() == BaseArea::AT_FACILITY)
        {
            if(item->type() == (int)ItemTypes::FacilityEtalonRaster)
                ui->editEntity->setEnabled(true);

            if(item->type() == (int)ItemTypes::FacilityEtalonRaster)
            {
                ui->editEntity->setEnabled(true);
                ui->deleteEntity->setEnabled(true);
            }
        }

        if(_currentData.areaPtr->getType() == BaseArea::AT_FLOOR)
        {
            if(item->type() == (int)ItemTypes::FacilityEtalonRaster)
            {
                ui->editEntity->setEnabled(true);
                ui->deleteEntity->setEnabled(true);
            }

            if(item->type() == (int)ItemTypes::FacilityPolygonOnPlan)
                ui->editEntity->setEnabled(true);
        }
    }
}

void LayersManagerForm::slotBlockGUI(QVariant var)
{
    setDisabled(var.toBool());
}

void LayersManagerForm::slotEditorFormClose()
{
    if(_ifaceInstrumentalForm)
    {
        ewApp()->removeWidget(_ifaceInstrumentalForm->id());
        _instrumentalForm->deleteLater();
        _instrumentalForm = nullptr;

        delete _ifaceInstrumentalForm;
        _ifaceInstrumentalForm = nullptr;
    }
    if(_svgEditorForm)
    {
        ewApp()->removeWidget(_svgEditorForm->id());
        _svgEditorForm->deleteLater();
        _svgEditorForm = nullptr;
    }

    if(_currentData.state == CurrentData::Edit)
    {
        if(_currentData.object == CurrentData::Raster)
        {
            auto it = _layers.find(_currentData.layerId);
            if(it != _layers.end())
                for(int a(0); a<it.value()->childCount(); ++a)
                    if(it.value()->child(a)->type() == (int)ItemTypes::Rasters )
                    {
                        QTreeWidgetItem * item = it.value()->child(a);
                        for(int b(0); b < item->childCount(); ++b)
                        {
                            RasterItem* rasterItem = dynamic_cast<RasterItem*>(item->child(b));
                            if(rasterItem->getPath() == _currentData.planPath)
                            {
                                if(rasterItem->checkState(0) == Qt::Checked)
                                    rasterItem->getRaster()->show();
                                break;
                            }
                        }
                        break;
                    }
        }
        else if(_currentData.object == CurrentData::Vector)
        {


        }
    }
    _currentData.clear();
    syncMarks();

    if( ! _isGeoScene)
        slotCalcSceneRect();

    bool block(false);
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::BlockGUI, QVariant(block), QString("visualize_system"));
    ewApp()->setVisible(_embeddedWidgetId, true);
}

void LayersManagerForm::slotCalcSceneRect()
{
    _recalcSceneRectTimer.stop();

    if(_lineItemsVisible)
    {
        foreach(QGraphicsLineItem * item, _lineItems)
            delete item;
        _lineItems.clear();
    }

    QRectF r = _pixelView->scene()->itemsBoundingRect();
    double shW(r.width() / 4.), shH(r.height() / 4.);
    r = QRectF(r.x()-shW, r.y()-shH, r.width() + shW*2, r.height() + shH*2);
    qDebug() << "===> setSceneRect" << r;
    _pixelView->setSceneRect(r);

    redrawItems(_pixelDelta);
}

void LayersManagerForm::syncChechState(QTreeWidgetItem *item, bool setVisible)
{
    /*
    if(item == _defectLayer)
        CtrConfig::setValueByName("application_settings.defectBaseLayerVisible", setVisible);
    else if(item == _photoLayer)
        CtrConfig::setValueByName("application_settings.photoBaseLayerVisible", setVisible);
    else if(item == _photo3dLayer)
        CtrConfig::setValueByName("application_settings.photo3dBaseLayerVisible", setVisible);
    else if(item == _axisLayer)
        CtrConfig::setValueByName("application_settings.axisRasterVisible", setVisible);
    else if(item == _sizesLayer)
        CtrConfig::setValueByName("application_settings.sizesRasterVisible", setVisible);
    else if(item == _waterDisposalLayer)
        CtrConfig::setValueByName("application_settings.waterDisposalRasterVisible", setVisible);
    else if(item == _waterSupplyLayer)
        CtrConfig::setValueByName("application_settings.waterSupplyRasterVisible", setVisible);
    else if(item == _heatingLayer)
        CtrConfig::setValueByName("application_settings.heatingRasterVisible", setVisible);
    else if(item == _electricityLayer)
        CtrConfig::setValueByName("application_settings.electricityRasterVisible", setVisible);
    else if(item == _doorsLayer)
        CtrConfig::setValueByName("application_settings.doorsRasterVisible", setVisible);
    */
}

void LayersManagerForm::slotAddLayer()
{
    QString layerName = QInputDialog::getText(this, QString::fromUtf8("Создание слоя"), QString::fromUtf8("Задайте имя для нового слоя"));
    if(layerName.isEmpty() == false)
    {
        foreach(LayerPtr layerPtr, RegionBizManager::instance()->getLayers())
            if(layerName == layerPtr->getName())
            {
                QMessageBox::warning(this, QString::fromUtf8("Внимание"), QString::fromUtf8("Слой с таким именем уже существует !"));
                return;
            }

        LayerPtr layerPtr = RegionBizManager::instance()->addLayer(layerName);
        if(layerPtr)
        {
            RegionBizManager::instance()->commitLayers();
            reinitLayers();
        }
    }
}

void LayersManagerForm::slotDeleteLayer()
{
    QList<QTreeWidgetItem*> list = ui->treeWidget->selectedItems();
    if(list.isEmpty() == false)
    {
        QTreeWidgetItem* item = list.first();
        LayerItem * layerItem = dynamic_cast<LayerItem*>(item);
        if(layerItem)
            if(layerItem->getLayerType() == LayerTypes::Other)
            {
                QString text = QString::fromUtf8("Удалить слой \"") + layerItem->text(0) + QString("\" ?");
                if(QMessageBox::question(this, QString::fromUtf8("Внимание"), text, QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
                {
                    RegionBizManager::instance()->deleteLayer(layerItem->getLayerId());
                    RegionBizManager::instance()->commitLayers();
                    reinitLayers();
                }
            }
    }
}

void LayersManagerForm::slotToolButtonInPluginChecked(QVariant var)
{
    QList<QVariant> list = var.toList();
    if(list.size() != 2)
        return;

    plugin_types::PixelVisualizerButtons button = (plugin_types::PixelVisualizerButtons)list.first().toInt();
    if(button == plugin_types::GRID)
    {
        _lineItemsVisible = list.last().toBool();
        if(_lineItemsVisible)
        {
            redrawItems(_pixelDelta);
        }
        else
        {
            foreach(QGraphicsLineItem * item, _lineItems)
                delete item;
            _lineItems.clear();
        }
    }
}

LayerItem * LayersManagerForm::getTopLevelItem(LayerTypes type)
{
    if(type == LayerTypes::Base || type == LayerTypes::Etalon)
        for(int a(0); a < ui->treeWidget->topLevelItemCount(); ++a)
        {
            LayerItem * layerItem = dynamic_cast<LayerItem*>(ui->treeWidget->topLevelItem(a));
            if(layerItem)
                if(layerItem->getLayerType() == type)
                    return layerItem;
        }

    return nullptr;
}


///-----------------------------------------------

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

















