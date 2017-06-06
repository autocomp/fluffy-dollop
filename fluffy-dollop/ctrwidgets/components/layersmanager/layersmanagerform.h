#ifndef LAYERSMANAGERFORM_H
#define LAYERSMANAGERFORM_H

#include <QWidget>
#include <QTreeWidgetItem>
#include <regionbiz/rb_manager.h>
#include "inputlayerstate.h"
class QGraphicsView;
class QGraphicsPixmapItem;

namespace Ui {
class LayersManagerForm;
}

class LayersManagerForm : public QWidget
{
    Q_OBJECT

public:
    explicit LayersManagerForm(QWidget *parent = 0);
    ~LayersManagerForm();
    void reset();
    void reload(regionbiz::BaseAreaPtr ptr);

private slots:
    void slotItemChanged(QTreeWidgetItem *item, int column);
    void slotAddLayer();
    void slotEditLayer();
    void slotDeleteLayer();
    void slotSelectionChanged();
    void slotBlockGUI(QVariant);
    void slotEditStateAborted();

private:
    void syncChechState(QTreeWidgetItem *item, bool setVisible);

    Ui::LayersManagerForm *ui;
    QGraphicsView * _geoView = nullptr;
    QGraphicsView * _pixelView = nullptr;
    uint _geoVisId = 0;
    uint _pixelVisId = 0;
    QSharedPointer<InputLayerState> _inputLayerState;

    QTreeWidgetItem * _baseLayer, * _defectLayer, * _photoLayer, * _photo3dLayer ;
    QTreeWidgetItem * _axisLayer;
    QTreeWidgetItem * _sizesLayer;
    QTreeWidgetItem * _waterDisposalLayer;
    QTreeWidgetItem * _waterSupplyLayer;
    QTreeWidgetItem * _heatingLayer;
    QTreeWidgetItem * _electricityLayer;
    QTreeWidgetItem * _doorsLayer;
};

class SublayersItem : public QTreeWidgetItem
{
    public:
    SublayersItem(QTreeWidgetItem * parentItem, QString name, QGraphicsPixmapItem * pixmapItem, bool syncCheckState = true);
    ~SublayersItem();

    QGraphicsPixmapItem * _pixmapItem;
};


#endif // LAYERSMANAGERFORM_H
