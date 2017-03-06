#include "virapageslistwidget.h"
#include<QResizeEvent>
#include<QDir>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <regionbiz/rb_manager.h>

using namespace regionbiz;

ViraPagesListWidget::ViraPagesListWidget(qulonglong facilityId)
    : _coef(1)
{
    setViewMode(QListView::IconMode);
    setFlow(QListView::TopToBottom);
    setWrapping(false);
    //setIconSize(QSize(200,100));

    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragDropMode(QAbstractItemView::DragOnly);
    setDragEnabled(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    reinit(facilityId);

    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
    auto mngr = RegionBizManager::instance();
    mngr->subscribeOnSelect(this, SLOT(slotSelectionItemsChanged(uint64_t,uint64_t)));
}

void ViraPagesListWidget::reinit(qulonglong facilityId)
{
    _items.clear();
    clear();
    QString destPath;
    QVariant regionBizInitJson_Path = CtrConfig::getValueByName(QString("application_settings.regionBizInitJson_Path"));
    if(regionBizInitJson_Path.isValid())
         destPath = regionBizInitJson_Path.toString();

    BaseAreaPtr ptr = RegionBizManager::instance()->getBaseLoation(facilityId, BaseArea::AT_FACILITY);
    FacilityPtr facilityPtr = BaseArea::convert< Facility >(ptr);
    if(facilityPtr)
    {
        FloorPtrs floors = facilityPtr->getChilds();
        for( FloorPtr floorPtr: floors )
        {
            QString pixmapPath = destPath + QString::fromStdString(floorPtr->getPlanPath());
            QPixmap pm(pixmapPath);
            _coef = (double)pm.height() / (double)pm.width();
            QListWidgetItem * item = new QListWidgetItem(QIcon(pm), QString::fromStdString(floorPtr->getName()), this);
            item->setData(Qt::UserRole, (qulonglong)floorPtr->getId());
            _items.insert((qulonglong)floorPtr->getId(), item);
        }
    }
}

void ViraPagesListWidget::resizeEvent(QResizeEvent * e)
{
    int W = e->size().width();
    _iconSize = QSize(W-2, (W * _coef)-2);
    setIconSize(_iconSize);
    QListWidget::resizeEvent(e);
}

void ViraPagesListWidget::slotSelectionChanged()
{
    if(selectedItems().isEmpty() == false)
    {
        _block = true;
        QListWidgetItem * item = selectedItems().first();
        RegionBizManager::instance()->selectArea(item->data(Qt::UserRole).toULongLong());
        emit setFloor(item->data(Qt::UserRole).toULongLong());
        _block = false;
    }
}

void ViraPagesListWidget::slotSelectionItemsChanged(uint64_t prev_id, uint64_t curr_id)
{
    if(_block)
        return;

    disconnect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
    if(prev_id > 0)
    {
        auto it = _items.find(prev_id);
        if(it != _items.end())
            it.value()->setSelected(false);
    }
    if(curr_id > 0)
    {
        auto it = _items.find(curr_id);
        if(it != _items.end())
        {
            it.value()->setSelected(true);
            emit setFloor(curr_id);
        }
        else
        {
            BaseAreaPtr ptr = RegionBizManager::instance()->getBaseLoation(curr_id, BaseArea::AT_ROOM);
            RoomPtr roomPtr = BaseArea::convert< Room >(ptr);
            if(roomPtr)
            {
                auto it = _items.find(roomPtr->getParentId());
                if(it != _items.end())
                {
                    it.value()->setSelected(true);
                    emit setFloor(roomPtr->getParentId());
                }
            }
        }
    }
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
}























