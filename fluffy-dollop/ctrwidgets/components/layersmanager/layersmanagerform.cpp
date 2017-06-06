#include "layersmanagerform.h"
#include "ui_layersmanagerform.h"
#include <QFile>
#include <QDir>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QFileDialog>
#include <QMessageBox>
#include <QGraphicsPixmapItem>
#include <ctrcore/visual/visualizermanager.h>
#include <ctrcore/visual/viewinterface.h>
#include <ctrcore/visual/stateinterface.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>
#include <ctrcore/ctrcore/ctrconfig.h>

using namespace regionbiz;

LayersManagerForm::LayersManagerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LayersManagerForm)
{
    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::BlockGUI, this, SLOT(slotBlockGUI(QVariant)),
                                      qMetaTypeId< bool >(),
                                      QString("visualize_system") );

    ui->setupUi(this);
    ui->addTb->setIcon(QIcon(":/img/add_image.png"));
    connect(ui->addTb, SIGNAL(clicked()), this, SLOT(slotAddLayer()));

    ui->editTb->setIcon(QIcon(":/img/edit_mode.png"));
    connect(ui->editTb, SIGNAL(clicked()), this, SLOT(slotEditLayer()));

    ui->transparentTb->hide();

    ui->deleteTb->setIcon(QIcon(":/img/close_button.png"));
    connect(ui->deleteTb, SIGNAL(clicked()), this, SLOT(slotDeleteLayer()));

    ui->upTb->hide();
    ui->downTb->hide();

    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));

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

    _baseLayer = new QTreeWidgetItem(ui->treeWidget, QStringList() << QString::fromUtf8("базовый"));

    _defectLayer = new QTreeWidgetItem(_baseLayer, QStringList() << QString::fromUtf8("дефекты"));
    QVariant defectBaseLayerVisible = CtrConfig::getValueByName("application_settings.defectBaseLayerVisible", true, true);
    _defectLayer->setCheckState(0, defectBaseLayerVisible.toBool() ? Qt::Checked : Qt::Unchecked);

    _photoLayer = new QTreeWidgetItem(_baseLayer, QStringList() << QString::fromUtf8("фотографии"));
    QVariant photoBaseLayerVisible = CtrConfig::getValueByName("application_settings.photoBaseLayerVisible", true, true);
    _photoLayer->setCheckState(0, photoBaseLayerVisible.toBool() ? Qt::Checked : Qt::Unchecked);

    _photo3dLayer = new QTreeWidgetItem(_baseLayer, QStringList() << QString::fromUtf8("панорамные фотографии"));
    QVariant photo3dBaseLayerVisible = CtrConfig::getValueByName("application_settings.photo3dBaseLayerVisible", true, true);
    _photo3dLayer->setCheckState(0, photo3dBaseLayerVisible.toBool() ? Qt::Checked : Qt::Unchecked);

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

    connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
}

LayersManagerForm::~LayersManagerForm()
{
    delete ui;
}

void LayersManagerForm::reset()
{
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
}

void LayersManagerForm::reload(BaseAreaPtr ptr)
{
    reset();
    setDisabled(false);

    QGraphicsScene * _scene = _pixelView->scene(); /// <---- !!! set geo or pixel scene !!!

    BaseFileKeeperPtrs baseFileKeeperPtrs = ptr->getFilesByType(BaseFileKeeper::FT_PLAN);
    foreach (BaseFileKeeperPtr baseFileKeeperPtr, baseFileKeeperPtrs)
    {
        PlanFileKeeperPtr planFileKeeperPtr = BaseFileKeeper::convert<PlanFileKeeper>(baseFileKeeperPtr);
        if(planFileKeeperPtr)
        {
            baseFileKeeperPtr->getName();
            planFileKeeperPtr->getPlanParams().x;
            planFileKeeperPtr->getPlanParams().y;
        }
    }


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

            SublayersItem * subItem = new SublayersItem(_baseLayer, QString("base.tiff"), _baseRasterItem, false);
        }
    }
    if(QFile::exists(path + QString("axis.tiff")))
    {
        QPixmap pm(path + QString("axis.tiff"));
        QGraphicsPixmapItem * _axisRasterItem = new QGraphicsPixmapItem(pm);
        _axisRasterItem->setTransformationMode(Qt::SmoothTransformation);
        _scene->addItem(_axisRasterItem);

        SublayersItem * subItem = new SublayersItem(_axisLayer, QString("axis.tiff"), _axisRasterItem);
    }
    if(QFile::exists(path + QString("sizes.tiff")))
    {
        QPixmap pm(path + QString("sizes.tiff"));
        QGraphicsPixmapItem * _sizesRasterItem = new QGraphicsPixmapItem(pm);
        _sizesRasterItem->setTransformationMode(Qt::SmoothTransformation);
        _scene->addItem(_sizesRasterItem);

        SublayersItem * subItem = new SublayersItem(_sizesLayer, QString("sizes.tiff"), _sizesRasterItem);
    }
    if(QFile::exists(path + QString("water_disposal.tiff")))
    {
        QPixmap pm(path + QString("water_disposal.tiff"));
        QGraphicsPixmapItem * _waterDisposalRasterItem = new QGraphicsPixmapItem(pm);
        _waterDisposalRasterItem->setTransformationMode(Qt::SmoothTransformation);
        _scene->addItem(_waterDisposalRasterItem);

        SublayersItem * subItem = new SublayersItem(_waterDisposalLayer, QString("water_disposal.tiff"), _waterDisposalRasterItem);
    }
    if(QFile::exists(path + QString("water_supply.tiff")))
    {
        QPixmap pm(path + QString("water_supply.tiff"));
        QGraphicsPixmapItem * _waterSupplyRasterItem = new QGraphicsPixmapItem(pm);
        _waterSupplyRasterItem->setTransformationMode(Qt::SmoothTransformation);
        _scene->addItem(_waterSupplyRasterItem);

        SublayersItem * subItem = new SublayersItem(_waterSupplyLayer, QString("water_supply.tiff"), _waterSupplyRasterItem);
    }
    if(QFile::exists(path + QString("heating.tiff")))
    {
        QPixmap pm(path + QString("heating.tiff"));
        QGraphicsPixmapItem * _heatingRasterItem = new QGraphicsPixmapItem(pm);
        _heatingRasterItem->setTransformationMode(Qt::SmoothTransformation);
        _scene->addItem(_heatingRasterItem);

        SublayersItem * subItem = new SublayersItem(_heatingLayer, QString("heating.tiff"), _heatingRasterItem);
    }
    if(QFile::exists(path + QString("electricity.tiff")))
    {
        QPixmap pm(path + QString("electricity.tiff"));
        QGraphicsPixmapItem * _electricityRasterItem = new QGraphicsPixmapItem(pm);
        _electricityRasterItem->setTransformationMode(Qt::SmoothTransformation);
        _scene->addItem(_electricityRasterItem);

        SublayersItem * subItem = new SublayersItem(_electricityLayer, QString("electricity.tiff"), _electricityRasterItem);
    }
    if(QFile::exists(path + QString("doors.tiff")))
    {
        QPixmap pm(path + QString("doors.tiff"));
        QGraphicsPixmapItem * _doorsRasterItem = new QGraphicsPixmapItem(pm);
        _doorsRasterItem->setTransformationMode(Qt::SmoothTransformation);
        _scene->addItem(_doorsRasterItem);

        SublayersItem * subItem = new SublayersItem(_doorsLayer, QString("doors.tiff"), _doorsRasterItem);
    }

    _pixelView->setSceneRect(_pixelView->scene()->itemsBoundingRect());

    connect(ui->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)), this, SLOT(slotItemChanged(QTreeWidgetItem*,int)));
}

void LayersManagerForm::slotItemChanged(QTreeWidgetItem *item, int /*column*/)
{
    bool setVisible(item->checkState(0) == Qt::Checked);
    SublayersItem * subItem = dynamic_cast<SublayersItem *>(item);
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
            subItem = dynamic_cast<SublayersItem *>(item->child(a));
            if(subItem)
            {
                subItem->setCheckState(0, setVisible ? Qt::Checked : Qt::Unchecked);
                //subItem->_pixmapItem->setVisible(setVisible);
            }
        }
    }
}

void LayersManagerForm::slotAddLayer()
{
    QString filePath = QFileDialog::getOpenFileName(this);
    if(filePath.isEmpty() == false)
    {
        QPixmap pm(filePath);
        if(pm.isNull())
        {
            QMessageBox::information(this, QString::fromUtf8("Внимание"), QString::fromUtf8("Выбранный файл не является изображением !"));
        }
        else
        {
            _inputLayerState = QSharedPointer<InputLayerState>(new InputLayerState(pm));
            connect(_inputLayerState.data(), SIGNAL(signalStateAborted()), this, SLOT(slotEditStateAborted()));

            visualize_system::StateInterface * stateInterface = visualize_system::VisualizerManager::instance()->getStateInterface(_pixelVisId);
            stateInterface->setVisualizerState(_inputLayerState);

            bool block(true);
            CommonMessageNotifier::send( (uint)visualize_system::BusTags::BlockGUI, QVariant(block), QString("visualize_system"));
        }
    }
}

void LayersManagerForm::slotEditLayer()
{

}

void LayersManagerForm::slotDeleteLayer()
{

}

void LayersManagerForm::slotSelectionChanged()
{
    ui->addTb->setDisabled(true);
    ui->editTb->setDisabled(true);
    ui->deleteTb->setDisabled(true);

    QList<QTreeWidgetItem*> list = ui->treeWidget->selectedItems();
    if(list.isEmpty() == false)
    {
        QTreeWidgetItem* item = list.first();
        if(item->parent() == nullptr)
        {
            ui->addTb->setDisabled(false);
        }
        else
        {
            if(dynamic_cast<SublayersItem *>(item))
            {
                ui->editTb->setDisabled(false);
                ui->deleteTb->setDisabled(false);
            }
        }
    }
}

void LayersManagerForm::slotBlockGUI(QVariant var)
{
    setDisabled(var.toBool());
}

void LayersManagerForm::slotEditStateAborted()
{
    _inputLayerState->emit_closeState();
    _inputLayerState.clear();

    bool block(false);
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::BlockGUI, QVariant(block), QString("visualize_system"));
}

void LayersManagerForm::syncChechState(QTreeWidgetItem *item, bool setVisible)
{
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
}


///-----------------------------------------------


SublayersItem::SublayersItem(QTreeWidgetItem *parentItem, QString name, QGraphicsPixmapItem *pixmapItem, bool syncCheckState)
    : QTreeWidgetItem(parentItem, QStringList()<<name)
    , _pixmapItem(pixmapItem)
{
    if(syncCheckState)
    {
        setCheckState(0, parentItem->checkState(0));
        pixmapItem->setVisible(parentItem->checkState(0) == Qt::Checked);
    }
}

SublayersItem::~SublayersItem()
{
    delete _pixmapItem;
}






















