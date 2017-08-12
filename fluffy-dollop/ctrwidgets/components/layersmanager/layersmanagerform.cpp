#include "layersmanagerform.h"
#include "ui_layersmanagerform.h"
#include "layerinstrumentalform.h"
#include <QFile>
#include <QDir>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QGraphicsPixmapItem>
#include <ctrcore/visual/visualizermanager.h>
#include <ctrcore/visual/viewinterface.h>
#include <ctrcore/visual/stateinterface.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <dmanager/embeddedstruct.h>
#include <dmanager/embeddedapp.h>
#include <QDebug>

using namespace regionbiz;
using namespace layers_manager_form;

LayersManagerForm::LayersManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LayersManagerForm)
{
    auto mngr = RegionBizManager::instance();
    mngr->subscribeFileSynced( this, SLOT(slotFileLoaded(regionbiz::BaseFileKeeperPtr)));

    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::BlockGUI, this, SLOT(slotBlockGUI(QVariant)),
                                      qMetaTypeId< bool >(),
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
    QVariant layerEditModeVar = CtrConfig::getValueByName("layers_manager.layerEditMode");
    if(layerEditModeVar.isValid())
        layerEditMode = layerEditModeVar.toBool();
    else
        CtrConfig::setValueByName("layers_manager.layerEditMode", false);

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

    LayerItem * baseLayerItem = new LayerItem(ui->treeWidget);
    reinitLayer(baseLayerItem);
    _layers.insert(0, baseLayerItem);

    foreach(LayerPtr layerPtr, RegionBizManager::instance()->getLayers())
    {
        QString layerName = layerPtr->getName();
        LayerItem * layerItem = new LayerItem(ui->treeWidget, layerName, layerPtr->getId());
        reinitLayer(layerItem);
        _layers.insert(layerPtr->getId(), layerItem);
    }

    connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));

//    _defectLayer = new QTreeWidgetItem(_baseLayer, QStringList() << QString::fromUtf8("дефекты"), (int)ItemTypes::Defect);
//    QVariant defectBaseLayerVisible = CtrConfig::getValueByName("application_settings.defectBaseLayerVisible", true, true);
//    _defectLayer->setCheckState(0, defectBaseLayerVisible.toBool() ? Qt::Checked : Qt::Unchecked);

//    _photoLayer = new QTreeWidgetItem(_baseLayer, QStringList() << QString::fromUtf8("фотографии"));
//    QVariant photoBaseLayerVisible = CtrConfig::getValueByName("application_settings.photoBaseLayerVisible", true, true);
//    _photoLayer->setCheckState(0, photoBaseLayerVisible.toBool() ? Qt::Checked : Qt::Unchecked);

//    _photo3dLayer = new QTreeWidgetItem(_baseLayer, QStringList() << QString::fromUtf8("панорамные фотографии"));
//    QVariant photo3dBaseLayerVisible = CtrConfig::getValueByName("application_settings.photo3dBaseLayerVisible", true, true);
//    _photo3dLayer->setCheckState(0, photo3dBaseLayerVisible.toBool() ? Qt::Checked : Qt::Unchecked);

/*
    _axisLayer = new QTreeWidgetItem(ui->treeWidget, QStringList() << QString::fromUtf8("оси"));
    QVariant axisRasterVisible = CtrConfig::getValueByName("application_settings.axisRasterVisible", true, true);
    _axisLayer->setCheckState(0, axisRasterVisible.toBool() ? Qt::Checked : Qt::Unchecked);

    _sizesLayer = new QTreeWidgetItem(ui->treeWidget, QStringList() << QString::fromUtf8("размер"));
    QVariant sizesRasterVisible = CtrConfig::getValueByName("application_settings.sizesRasterVisible", true, true);
    _sizesLayer->setCheckState(0, sizesRasterVisible.toBool() ? Qt::Checked : Qt::Unchecked);

    _waterDisposalLayer = new QTreeWidgetItem(ui->treeWidget, QStringList() << QString::fromUtf8("водоотведение"));
    QVariant waterDisposalRasterVisible = CtrConfig::getValueByName("application_settings.waterDisposalRasterVisible", true, true);
    _waterDisposalLayer->setCheckState(0, waterDisposalRasterVisible.toBool() ? Qt::Checked : Qt::Unchecked);

    _waterSupplyLayer = new QTreeWidgetItem(ui->treeWidget, QStringList() << QString::fromUtf8("водоснабжение"));
    QVariant waterSupplyRasterVisible = CtrConfig::getValueByName("application_settings.waterSupplyRasterVisible", true, true);
    _waterSupplyLayer->setCheckState(0, waterSupplyRasterVisible.toBool() ? Qt::Checked : Qt::Unchecked);

    _heatingLayer = new QTreeWidgetItem(ui->treeWidget, QStringList() << QString::fromUtf8("отопление"));
    QVariant heatingRasterVisible = CtrConfig::getValueByName("application_settings.heatingRasterVisible", true, true);
    _heatingLayer->setCheckState(0, heatingRasterVisible.toBool() ? Qt::Checked : Qt::Unchecked);

    _electricityLayer = new QTreeWidgetItem(ui->treeWidget, QStringList() << QString::fromUtf8("электрика"));
    QVariant electricityRasterVisible = CtrConfig::getValueByName("application_settings.electricityRasterVisible", true, true);
    _electricityLayer->setCheckState(0, electricityRasterVisible.toBool() ? Qt::Checked : Qt::Unchecked);

    _doorsLayer = new QTreeWidgetItem(ui->treeWidget, QStringList() << QString::fromUtf8("двери"));
    QVariant doorsRasterVisible = CtrConfig::getValueByName("application_settings.doorsRasterVisible", true, true);
    _doorsLayer->setCheckState(0, doorsRasterVisible.toBool() ? Qt::Checked : Qt::Unchecked);
*/

}

void LayersManagerForm::setEmbeddedWidgetId(quint64 id)
{
    _embeddedWidgetId = id;
}

void LayersManagerForm::reset()
{
    _currAreaId = 0;
    _currLayerId = 0;
    _loadingRasters.clear();
    for(int a(0); a < ui->treeWidget->topLevelItemCount(); ++a)
    {
        LayerItem * layerItem = dynamic_cast<LayerItem*>(ui->treeWidget->topLevelItem(a));
        if(layerItem)
            layerItem->clearData();
    }

    /*

    setDisabled(true);

    QList<QTreeWidgetItem *> list;
    for(int a(0); a<ui->treeWidget->topLevelItemCount() ; ++a)
    {
        QTreeWidgetItem * item = ui->treeWidget->topLevelItem(a);
        if(item == _baseLayer)
            for(int b(0); b<item->childCount() ; ++b)
            {
                QTreeWidgetItem * subItem = item->child(b);
                if(subItem != _defectLayer && subItem != _photoLayer  && subItem != _photo3dLayer)
                    list.append(subItem);
            }
        else
        {
            for(int b(0); b<item->childCount() ; ++b)
                list.append(item->child(b));
        }
    }
    foreach(QTreeWidgetItem * item, list)
        delete item;

    */
}

void LayersManagerForm::slotFileLoaded(regionbiz::BaseFileKeeperPtr baseFileKeeperPtr)
{
    PlanFileKeeperPtr planFileKeeperPtr = BaseFileKeeper::convert<PlanFileKeeper>(baseFileKeeperPtr);
    if(planFileKeeperPtr)
    {
        auto it = _loadingRasters.find(baseFileKeeperPtr->getPath());
        if(it == _loadingRasters.end())
            return;

        RasterItem * rasterTreeWidgetItem = it.value();
        _loadingRasters.erase(it);

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
            QGraphicsPixmapItem * rasterItem = new QGraphicsPixmapItem(pm);
            rasterItem->setTransformationMode(Qt::SmoothTransformation);
            PlanFileKeeper::PlanParams planParams = planFileKeeperPtr->getPlanParams();
            rasterItem->setPos(QPointF(planParams.x, planParams.y));
            rasterItem->setTransform(QTransform().scale(planParams.scale_w,planParams.scale_h).rotate(planParams.rotate));

            LayerItem * layerItem = dynamic_cast<LayerItem*>(rasterTreeWidgetItem->parent()->parent());
            if(layerItem)
                rasterItem->setZValue(layerItem->isBaseLayer() ? -100 : -10);

            QGraphicsScene * _scene = (_isGeoScene ? _geoView->scene() : _pixelView->scene());
            _scene->addItem(rasterItem);

            rasterTreeWidgetItem->setRaster(rasterItem);
        }
    }
}

void LayersManagerForm::reload(BaseAreaPtr ptr, bool isGeoScene)
{
    reset();
    setDisabled(false);
    _currAreaId = ptr->getId();
    _isGeoScene = isGeoScene;
//    QGraphicsScene * _scene = (_isGeoScene ? _geoView->scene() : _pixelView->scene());
    auto mngr = RegionBizManager::instance();

    BaseFileKeeperPtrs baseFileKeeperPtrs = ptr->getFilesByType(BaseFileKeeper::FT_PLAN);
    qDebug() << "LayersManagerForm::reload, baseFileKeeperPtrs.size :" << baseFileKeeperPtrs.size();
    foreach (BaseFileKeeperPtr baseFileKeeperPtr, baseFileKeeperPtrs)
    {
        uint64_t ID = baseFileKeeperPtr->getEntityId();
//        bool res = mngr->deleteFile(baseFileKeeperPtr);
//        qDebug() << "ptr->getId :" << ptr->getId() << "deleteFile ID :" << ID << ", res:" << res;

////        uint64_t ID = baseFileKeeperPtr->getEntityId();
////        if(ID == )
////        {
////            bool res = mngr->deleteFile(baseFileKeeperPtr);
////            qDebug() << "deleteFile ID :" << ID << ", res:" << res;
////        }

        PlanFileKeeperPtr planFileKeeperPtr = BaseFileKeeper::convert<PlanFileKeeper>(baseFileKeeperPtr);
        if(planFileKeeperPtr)
        {
            QString suffix = QFileInfo(planFileKeeperPtr->getPath()).suffix();
            QTreeWidgetItem * parentItem(nullptr);
            uint64_t layerId(0);
            LayerPtr layerPtr = planFileKeeperPtr->getLayer();
            if(layerPtr)
                layerId = layerPtr->getId();
            auto it = _layers.find(layerId);
            if(it != _layers.end())
            {
                if(suffix == QString("svg"))
                    parentItem = it.value()->vectorItems();
                else
                    parentItem = it.value()->rasterItems();
            }
            if( ! parentItem)
                continue;

            qDebug() << "ID:" << ID << ", suffix:" << suffix << ", getPath:" << planFileKeeperPtr->getPath();

            RasterItem * rasterItem = new RasterItem(parentItem, baseFileKeeperPtr->getEntityId(), planFileKeeperPtr->getPath());
            _loadingRasters.insert(planFileKeeperPtr->getPath(), rasterItem);

            BaseFileKeeper::FileState fileState = planFileKeeperPtr->getFileState();
            switch(fileState)
            {
            case BaseFileKeeper::FS_UNSYNC : {
                qDebug() << "reload, FS_UNSYNC, planFileKeeperPtr ID :" << planFileKeeperPtr->getEntityId();
                planFileKeeperPtr->syncFile();
            }break;
            case BaseFileKeeper::FS_SYNC : {
                qDebug() << "reload, FS_SYNC, planFileKeeperPtr ID :" << planFileKeeperPtr->getEntityId();
                slotFileLoaded(planFileKeeperPtr);
            }break;
            }
        }
    }
    qDebug() << "----------";

/*
    QString destPath;
    QVariant regionBizInitJson_Path = CtrConfig::getValueByName("application_settings.regionBizFilesPath",
                                                                "./data", true);
    if(regionBizInitJson_Path.isValid())
         destPath = regionBizInitJson_Path.toString() + QDir::separator();

    disconnect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
    QString path = destPath + QString::number(ptr->getId()) + QDir::separator();
    {
        QString filePath;
        if(QFile::exists(path + QString("area.tiff")))
            filePath = path + QString("area.tiff");
        else if(QFile::exists(path + QString("base.tiff")))
            filePath = path + QString("base.tiff");

        if(filePath.isEmpty() == false)
        {
            QPixmap pm(filePath);
            QGraphicsPixmapItem * _baseRasterItem = new QGraphicsPixmapItem(pm);
            _baseRasterItem->setTransformationMode(Qt::SmoothTransformation);
            _scene->addItem(_baseRasterItem);

            RasterItem * subItem = new RasterItem(_baseLayer, QString("base.tiff"), _baseRasterItem, false);
        }
    }

    if(QFile::exists(path + QString("axis.tiff")))
    {
        QPixmap pm(path + QString("axis.tiff"));
        QGraphicsPixmapItem * _axisRasterItem = new QGraphicsPixmapItem(pm);
        _axisRasterItem->setTransformationMode(Qt::SmoothTransformation);
        _scene->addItem(_axisRasterItem);

        RasterItem * subItem = new RasterItem(_axisLayer, QString("axis.tiff"), _axisRasterItem);
    }
    if(QFile::exists(path + QString("sizes.tiff")))
    {
        QPixmap pm(path + QString("sizes.tiff"));
        QGraphicsPixmapItem * _sizesRasterItem = new QGraphicsPixmapItem(pm);
        _sizesRasterItem->setTransformationMode(Qt::SmoothTransformation);
        _scene->addItem(_sizesRasterItem);

        RasterItem * subItem = new RasterItem(_sizesLayer, QString("sizes.tiff"), _sizesRasterItem);
    }
    if(QFile::exists(path + QString("water_disposal.tiff")))
    {
        QPixmap pm(path + QString("water_disposal.tiff"));
        QGraphicsPixmapItem * _waterDisposalRasterItem = new QGraphicsPixmapItem(pm);
        _waterDisposalRasterItem->setTransformationMode(Qt::SmoothTransformation);
        _scene->addItem(_waterDisposalRasterItem);

        RasterItem * subItem = new RasterItem(_waterDisposalLayer, QString("water_disposal.tiff"), _waterDisposalRasterItem);
    }
    if(QFile::exists(path + QString("water_supply.tiff")))
    {
        QPixmap pm(path + QString("water_supply.tiff"));
        QGraphicsPixmapItem * _waterSupplyRasterItem = new QGraphicsPixmapItem(pm);
        _waterSupplyRasterItem->setTransformationMode(Qt::SmoothTransformation);
        _scene->addItem(_waterSupplyRasterItem);

        RasterItem * subItem = new RasterItem(_waterSupplyLayer, QString("water_supply.tiff"), _waterSupplyRasterItem);
    }
    if(QFile::exists(path + QString("heating.tiff")))
    {
        QPixmap pm(path + QString("heating.tiff"));
        QGraphicsPixmapItem * _heatingRasterItem = new QGraphicsPixmapItem(pm);
        _heatingRasterItem->setTransformationMode(Qt::SmoothTransformation);
        _scene->addItem(_heatingRasterItem);

        RasterItem * subItem = new RasterItem(_heatingLayer, QString("heating.tiff"), _heatingRasterItem);
    }
    if(QFile::exists(path + QString("electricity.tiff")))
    {
        QPixmap pm(path + QString("electricity.tiff"));
        QGraphicsPixmapItem * _electricityRasterItem = new QGraphicsPixmapItem(pm);
        _electricityRasterItem->setTransformationMode(Qt::SmoothTransformation);
        _scene->addItem(_electricityRasterItem);

        RasterItem * subItem = new RasterItem(_electricityLayer, QString("electricity.tiff"), _electricityRasterItem);
    }
    if(QFile::exists(path + QString("doors.tiff")))
    {
        QPixmap pm(path + QString("doors.tiff"));
        QGraphicsPixmapItem * _doorsRasterItem = new QGraphicsPixmapItem(pm);
        _doorsRasterItem->setTransformationMode(Qt::SmoothTransformation);
        _scene->addItem(_doorsRasterItem);

        RasterItem * subItem = new RasterItem(_doorsLayer, QString("doors.tiff"), _doorsRasterItem);
    }
*/

    ///_pixelView->setSceneRect(_pixelView->scene()->itemsBoundingRect());

//    connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
}

void LayersManagerForm::slotItemChanged(QTreeWidgetItem *item, int /*column*/)
{
    /*
    bool setVisible(item->checkState(0) == Qt::Checked);
    RasterItem * subItem = dynamic_cast<RasterItem *>(item);
    if( ! subItem)
        syncChechState(item, setVisible);

    bool processed(true);
    if(item == _defectLayer)
    {

    }
    else if(item == _photoLayer)
    {

    }
    else if(item == _photo3dLayer)
    {

    }
    else
        processed = false;

    if(processed)
        return;

    if(subItem)
    {
        subItem->_pixmapItem->setVisible(setVisible);
    }
    else
    {
        for(int a(0); a<item->childCount() ; ++a)
        {
            subItem = dynamic_cast<RasterItem *>(item->child(a));
            if(subItem)
            {
                subItem->setCheckState(0, setVisible ? Qt::Checked : Qt::Unchecked);
                //subItem->_pixmapItem->setVisible(setVisible);
            }
        }
    }
    */
}

void LayersManagerForm::slotAddEntity()
{
    QList<QTreeWidgetItem*> list = ui->treeWidget->selectedItems();
    if(list.isEmpty())
        return;

    QTreeWidgetItem* item = list.first();
    if(item->type() == (int)ItemTypes::Rasters)
    {
        QString filePath = QFileDialog::getOpenFileName(this, QString::fromUtf8("Выберите изображение"));
        if(filePath.isEmpty() == false)
        {
            QPixmap pixmap(filePath);
            if(pixmap.isNull())
            {
                QMessageBox::information(this, QString::fromUtf8("Внимание"), QString::fromUtf8("Выбранный файл не является изображением !"));
            }
            else
            {
                _currLayerId = 0;
                LayerItem * layerItem = dynamic_cast<LayerItem*>(item->parent());
                if(layerItem)
                    if(layerItem->isBaseLayer() == false)
                        _currLayerId = layerItem->getLayerId();

                _instrumentalForm = new LayerInstrumentalForm( (_isGeoScene ? _geoVisId : _pixelVisId ), pixmap);
                connect(_instrumentalForm, SIGNAL(signalSaved(QString,QPointF,double,double,double)),
                        this, SLOT(slotSaved(QString,QPointF,double,double,double)));

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
                connect(_ifaceInstrumentalForm, SIGNAL(signalClosed()), this, SLOT(slotLayerInstrumentalFormClose()));
                ewApp()->setVisible(_ifaceInstrumentalForm->id(), true);
                ewApp()->setVisible(_embeddedWidgetId, false);

                bool block(true);
                CommonMessageNotifier::send( (uint)visualize_system::BusTags::BlockGUI, QVariant(block), QString("visualize_system"));
            }
        }
    }
    else if(item->type() == (int)ItemTypes::Vectors)
    {
        QMessageBox::information(this, QString::fromUtf8("Внимание"), QString::fromUtf8("Ввод векторных данных в разработке ! :)"));
    }
}

void LayersManagerForm::slotSaved(QString filePath, QPointF scenePos, double scaleW, double scaleH, double rotate)
{
    BaseFileKeeperPtr basePlan = RegionBizManager::instance()->addFile(filePath, BaseFileKeeper::FT_PLAN, _currAreaId);
    PlanFileKeeperPtr plan = BaseFileKeeper::convert<PlanFileKeeper>(basePlan);
    if(plan)
    {
        PlanFileKeeper::PlanParams planParams;
        planParams.scale_w = scaleW;
        planParams.scale_h = scaleH;
        planParams.rotate = rotate;
        planParams.x = scenePos.x();
        planParams.y = scenePos.y();
        plan->setPlanParams(planParams);

        LayerPtr layer = RegionBizManager::instance()->getLayer(_currLayerId);
        if(layer)
            plan->moveToLayer(layer);

        bool commitRes = plan->commit();
        qDebug() << "PlanFileKeeper commitRes:" << commitRes;
    }

    slotLayerInstrumentalFormClose();
}

void LayersManagerForm::slotEditEntity()
{
    QMessageBox::information(this, QString::fromUtf8("Внимание"), QString::fromUtf8("Редактирование данных в разработке ! :)"));
}

void LayersManagerForm::slotDeleteEntity()
{
    QList<QTreeWidgetItem*> list = ui->treeWidget->selectedItems();
    if(list.isEmpty() == false)
    {
        QTreeWidgetItem* item = list.first();
        if(item->type() == (int)ItemTypes::Raster) // || item->type() == (int)ItemTypes::Vector)
        {
            RasterItem * rasterItem = dynamic_cast<RasterItem*>(item);
            if(rasterItem)
            {
                auto mngr = RegionBizManager::instance();
                BaseEntityPtr ptr = mngr->getBaseEntity(rasterItem->getEntityId());
                if(ptr)
                {
                    BaseFileKeeperPtrs baseFileKeeperPtrs = ptr->getFilesByType(BaseFileKeeper::FT_PLAN);
                    foreach(BaseFileKeeperPtr baseFileKeeperPtr, baseFileKeeperPtrs)
                        if(baseFileKeeperPtr->getPath() == rasterItem->getPath())
                        {
                            bool res = mngr->deleteFile(baseFileKeeperPtr);
                            qDebug() << "slotDeleteEntity, getPath :" << rasterItem->getPath() << ", res:" << res;
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
    if(list.isEmpty() == false)
    {
        QTreeWidgetItem* item = list.first();

        LayerItem * layerItem = dynamic_cast<LayerItem*>(item);
        if(layerItem)
            if(layerItem->isBaseLayer() == false)
                ui->deleteLayer->setEnabled(true);

        if(item->type() == (int)ItemTypes::Rasters || item->type() == (int)ItemTypes::Vectors)
            ui->addEntity->setEnabled(true);

        if(item->type() == (int)ItemTypes::Raster || item->type() == (int)ItemTypes::Vector)
        {
            ui->editEntity->setEnabled(true);
            ui->deleteEntity->setEnabled(true);
        }
    }
}

void LayersManagerForm::slotBlockGUI(QVariant var)
{
    setDisabled(var.toBool());
}

void LayersManagerForm::slotLayerInstrumentalFormClose()
{
    if(_ifaceInstrumentalForm)
    {
        ewApp()->removeWidget(_ifaceInstrumentalForm->id());
        _instrumentalForm->deleteLater();
        _instrumentalForm = nullptr;

        delete _ifaceInstrumentalForm;
        _ifaceInstrumentalForm = nullptr;
    }

    bool block(false);
    _currLayerId = 0;
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::BlockGUI, QVariant(block), QString("visualize_system"));
    ewApp()->setVisible(_embeddedWidgetId, true);
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
            if(layerItem->isBaseLayer() == false)
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

///-----------------------------------------------

LayerItem::LayerItem(QTreeWidget * parent)
    : QTreeWidgetItem(parent, QStringList() << QString::fromUtf8("базовый"), (int)ItemTypes::Layer)
    , _layerId(0)
{
}

LayerItem::LayerItem(QTreeWidget * parent, QString name, uint64_t layerId)
    : QTreeWidgetItem(parent, QStringList() << name, (int)ItemTypes::Layer)
    , _layerId(layerId)
{
}

uint64_t LayerItem::getLayerId()
{
    return _layerId;
}

bool LayerItem::isBaseLayer()
{
    return _layerId == 0;
}

void LayerItem::clearData()
{
    for(int i(0); i < childCount(); ++i)
    {
        QTreeWidgetItem * children = child(i);
        if(children->type() == (int)ItemTypes::Rasters || children->type() == (int)ItemTypes::Vectors)
        {
            QList<QTreeWidgetItem*> items;
            for(int i(0); i < children->childCount(); ++i)
                items.append(children->child(i));

            foreach(QTreeWidgetItem* item, items)
                delete item;
        }
    }
}

QTreeWidgetItem *LayerItem::rasterItems()
{
    for(int i(0); i<childCount(); ++i)
        if(child(i)->text(0) == QString::fromUtf8("изображения"))
            return child(i);
    return nullptr;
}

QTreeWidgetItem *LayerItem::vectorItems()
{
    for(int i(0); i<childCount(); ++i)
        if(child(i)->text(0) == QString::fromUtf8("векторные данные"))
            return child(i);
    return nullptr;
}

///-----------------------------------------------

RasterItem::RasterItem(QTreeWidgetItem *parentItem, uint64_t entityId, QString path)
    : QTreeWidgetItem(parentItem, QStringList() << QString::fromUtf8("... загрузка ..."), (int)ItemTypes::Raster)
    , _entityId(entityId)
    , _path(path)
{
//    if(syncCheckState)
//    {
//        setCheckState(0, parentItem->checkState(0));
//        if(_pixmapItem)
//            _pixmapItem->setVisible(parentItem->checkState(0) == Qt::Checked);
//    }
}

RasterItem::~RasterItem()
{
    if(_pixmapItem)
        delete _pixmapItem;
}

void RasterItem::setRaster(QGraphicsPixmapItem *item, QString name)
{
    _pixmapItem = item;
    if(name.isEmpty())
    {
        if(parent()->text(0) == QString::fromUtf8("изображения"))
            setText(0, QString::fromUtf8("изображение"));
        else
            setText(0, QString::fromUtf8("данные"));
    }
    else
        setText(0, name);
}

QGraphicsPixmapItem *RasterItem::getRaster()
{
    return _pixmapItem;
}

uint64_t RasterItem::getEntityId()
{
    return _entityId;
}

QString RasterItem::getPath()
{
    return _path;
}






















