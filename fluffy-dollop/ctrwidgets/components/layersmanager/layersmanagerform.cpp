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
}

void LayersManagerForm::setEmbeddedWidgetId(quint64 id)
{
    _embeddedWidgetId = id;
}

void LayersManagerForm::reset()
{
    _currentData.areaId = 0;
    _currentData.clear();
    _loadingRasters.clear();

    disconnect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
    for(int a(0); a < ui->treeWidget->topLevelItemCount(); ++a)
    {
        LayerItem * layerItem = dynamic_cast<LayerItem*>(ui->treeWidget->topLevelItem(a));
        if(layerItem)
            layerItem->clearData();
    }
    connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
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
            QGraphicsPixmapItem * pixmapRasterItem = new QGraphicsPixmapItem(pm);
            pixmapRasterItem->setTransformationMode(Qt::SmoothTransformation);
            PlanFileKeeper::PlanParams planParams = planFileKeeperPtr->getPlanParams();
            pixmapRasterItem->setPos(QPointF(planParams.x, planParams.y));
            pixmapRasterItem->setTransform(QTransform().scale(planParams.scale_w,planParams.scale_h).rotate(planParams.rotate));

            LayerItem * layerItem = dynamic_cast<LayerItem*>(rasterTreeWidgetItem->parent()->parent());
            if(layerItem)
                pixmapRasterItem->setZValue(layerItem->isBaseLayer() ? -100 : -10);

            QGraphicsScene * _scene = (_isGeoScene ? _geoView->scene() : _pixelView->scene());
            _scene->addItem(pixmapRasterItem);

            disconnect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
            rasterTreeWidgetItem->setRaster(pixmapRasterItem, planParams.scale_w, planParams.scale_h, planParams.rotate);
            connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
        }
    }
}

void LayersManagerForm::reload(BaseAreaPtr ptr, bool isGeoScene)
{
    reset();
    setDisabled(false);
    _currentData.areaId = ptr->getId();
    _isGeoScene = isGeoScene;
    //    QGraphicsScene * _scene = (_isGeoScene ? _geoView->scene() : _pixelView->scene());

    BaseFileKeeperPtrs baseFileKeeperPtrs = ptr->getFilesByType(BaseFileKeeper::FT_PLAN);
    qDebug() << "LayersManagerForm::reload, baseFileKeeperPtrs.size :" << baseFileKeeperPtrs.size();
    foreach (BaseFileKeeperPtr baseFileKeeperPtr, baseFileKeeperPtrs)
    {
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

            qDebug() << "EntityId:" << baseFileKeeperPtr->getEntityId() << ", suffix:" << suffix << ", getPath:" << planFileKeeperPtr->getPath();

            RasterItem * rasterTreeWidgetItem = new RasterItem(parentItem, baseFileKeeperPtr->getEntityId(), planFileKeeperPtr->getPath());

            BaseFileKeeper::FileState fileState = planFileKeeperPtr->getFileState();
            switch(fileState)
            {
            case BaseFileKeeper::FS_UNSYNC : {
                qDebug() << "reload, FS_UNSYNC";
                _loadingRasters.insert(planFileKeeperPtr->getPath(), rasterTreeWidgetItem);
                planFileKeeperPtr->syncFile();
            }break;
            case BaseFileKeeper::FS_SYNC : {
                qDebug() << "reload, FS_SYNC";
                _loadingRasters.insert(planFileKeeperPtr->getPath(), rasterTreeWidgetItem);
                slotFileLoaded(planFileKeeperPtr);
            }break;
            case BaseFileKeeper::FS_INVALID : {
                qDebug() << "reload, FS_INVALID";

//                QString filePath = QString("/home/sergey/projects/vira_02/bin/ftp_cache/") + planFileKeeperPtr->getPath();
//                QPixmap pm(filePath);
//                qDebug() << "reload, FS_INVALID, filePath:" << filePath << ", pm.isNull() :" << pm.isNull();
//                if(pm.isNull() == false)
//                {
//                    qDebug() << "reload, FS_INVALID, HENDLE LOADING !!!";

//                    QGraphicsPixmapItem * pixmapRasterItem = new QGraphicsPixmapItem(pm);
//                    pixmapRasterItem->setTransformationMode(Qt::SmoothTransformation);
//                    PlanFileKeeper::PlanParams planParams = planFileKeeperPtr->getPlanParams();
//                    pixmapRasterItem->setPos(QPointF(planParams.x, planParams.y));
//                    pixmapRasterItem->setTransform(QTransform().scale(planParams.scale_w,planParams.scale_h).rotate(planParams.rotate));

//                    LayerItem * layerItem = dynamic_cast<LayerItem*>(rasterTreeWidgetItem->parent()->parent());
//                    if(layerItem)
//                        pixmapRasterItem->setZValue(layerItem->isBaseLayer() ? -100 : -10);

//                    QGraphicsScene * _scene = (_isGeoScene ? _geoView->scene() : _pixelView->scene());
//                    _scene->addItem(pixmapRasterItem);

//                    disconnect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
//                    rasterTreeWidgetItem->setRaster(pixmapRasterItem);
//                    connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
//                }
            }break;
            }
        }
    }
    qDebug() << "----------";

    QTimer::singleShot(10, this, SLOT(slotSyncMarks()));
}
void LayersManagerForm::slotSyncMarks()
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
                    switch(item->checkState(0))
                    {
                    case Qt::Checked : break;
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
        case (int)ItemTypes::Rasters : {
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
        case (int)ItemTypes::Raster : {
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
                item->child(a)->setCheckState(0, setVisible ? Qt::Checked : Qt::Unchecked);
                // ...
            }
            if(item->childCount() > 0)
                recalcLayer = true;
        }break;
        case (int)ItemTypes::Vector : {
            // ...

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
                _currentData.clear();
                _currentData.state = CurrentData::Create;
                _currentData.object = CurrentData::Raster;
                LayerItem * layerItem = dynamic_cast<LayerItem*>(item->parent());
                if(layerItem)
                    if(layerItem->isBaseLayer() == false)
                        _currentData.layerId = layerItem->getLayerId();

                _instrumentalForm = new LayerInstrumentalForm( (_isGeoScene ? _geoVisId : _pixelVisId ), pixmap, _currentData.layerId != 0);
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

    BaseFileKeeperPtr basePlan = RegionBizManager::instance()->addFile(filePath, BaseFileKeeper::FT_PLAN, _currentData.areaId);
    PlanFileKeeperPtr plan = BaseFileKeeper::convert<PlanFileKeeper>(basePlan);
    if( ! plan)
        return;

    PlanFileKeeper::PlanParams planParams;
    planParams.scale_w = scaleW;
    planParams.scale_h = scaleH;
    planParams.rotate = rotate;
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

    RasterItem * rasterTreeWidgetItem(nullptr);
    if(_currentData.state == CurrentData::Create)
    {
        rasterTreeWidgetItem = new RasterItem(rastersNode, plan->getEntityId(), plan->getPath());
    }
    else if(_currentData.state == CurrentData::Edit)
    {
        BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(_currentData.areaId);
        BaseFileKeeperPtrs baseFileKeeperPtrs = ptr->getFilesByType(BaseFileKeeper::FT_PLAN);
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
    slotLayerInstrumentalFormClose();

    if(rasterTreeWidgetItem)
    {
        rastersNode->setExpanded(true);
        rasterTreeWidgetItem->setSelected(true);

        BaseFileKeeper::FileState fileState = plan->getFileState();
        switch(fileState)
        {
        case BaseFileKeeper::FS_UNSYNC : {
            qDebug() << "slotSaved, FS_UNSYNC";
            _loadingRasters.insert(plan->getPath(), rasterTreeWidgetItem);
            plan->syncFile();
        }break;
        case BaseFileKeeper::FS_SYNC : {
            qDebug() << "slotSaved, FS_SYNC";
            _loadingRasters.insert(plan->getPath(), rasterTreeWidgetItem);
            slotFileLoaded(plan);
        }break;
        case BaseFileKeeper::FS_INVALID : {
            qDebug() << "slotSaved, FS_INVALID";
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
    if(item->type() == (int)ItemTypes::Raster)
    {
        RasterItem * rasterItem = dynamic_cast<RasterItem*>(item);
        if( ! rasterItem)
            return;

        QGraphicsPixmapItem * graphicsPixmapItem = rasterItem->getRaster();
        if( ! graphicsPixmapItem)
            return;

        graphicsPixmapItem->hide();

        _currentData.clear();
        _currentData.state = CurrentData::Edit;
        _currentData.object = CurrentData::Raster;
        _currentData.planPath = rasterItem->getPath();
        LayerItem * layerItem = dynamic_cast<LayerItem*>(item->parent());
        if(layerItem)
            if(layerItem->isBaseLayer() == false)
                _currentData.layerId = layerItem->getLayerId();

        _instrumentalForm = new LayerInstrumentalForm( (_isGeoScene ? _geoVisId : _pixelVisId ), graphicsPixmapItem->pixmap(), graphicsPixmapItem->scenePos(), rasterItem->getScW(), rasterItem->getScH(), rasterItem->getRotate(), _currentData.layerId != 0);
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

    bool block(false);
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
            if(children->checkState(0) == Qt::PartiallyChecked)
                children->setCheckState(0, Qt::Checked);

            QList<QTreeWidgetItem*> items;
            for(int i(0); i < children->childCount(); ++i)
                items.append(children->child(i));

            foreach(QTreeWidgetItem* item, items)
                delete item;
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
        if(parent()->text(0) == QString::fromUtf8("изображения"))
            setText(0, QString::fromUtf8("изображение"));
        else
            setText(0, QString::fromUtf8("данные"));
    }
    else
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






















