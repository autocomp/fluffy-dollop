#include "viratreewidget.h"
#include <QDebug>
#include <regionbiz/rb_manager.h>
#include <iostream>

using namespace regionbiz;

ViraTreeWidget::ViraTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    setExpandsOnDoubleClick(false);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setColumnCount(1);
    headerItem()->setText(0, QString::fromUtf8("Имя"));
//    headerItem()->setText(1, QString("Id"));
//    headerItem()->setText(2, QString("File"));

//    setIconSize(QSize(300,100));

    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotItemSelectionChanged()));
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(slotItemDoubleClicked(QTreeWidgetItem*,int)));

    auto mngr = RegionBizManager::instance();
    mngr->subscribeOnSelect(this, SLOT(slotObjectSelectionChanged(uint64_t,uint64_t)));
    std::vector<RegionPtr> regions = mngr->getRegions();
    for( RegionPtr regionPtr: regions )
    {
        QTreeWidgetItem * regionItem = new QTreeWidgetItem(this);
        regionItem->setText(0, regionPtr->getDescription());
        const qulonglong id(regionPtr->getId());
        regionItem->setText(1, QString::number(id));
        regionItem->setData(0, ID, id);
        _items.insert(id, regionItem);

        std::vector<BaseAreaPtr> locations = regionPtr->getChilds( Region::RCF_LOCATIONS );
        for( BaseAreaPtr ptr: locations )
        {
            LocationPtr locationPtr = BaseArea::convert< Location >( ptr );
            if(locationPtr)
            {
                QTreeWidgetItem * locationItem = new QTreeWidgetItem(regionItem);
                locationItem->setText(0, locationPtr->getDescription());
                const qulonglong id(locationPtr->getId());
                locationItem->setText(1, QString::number(id));
                locationItem->setData(0, ID, id);
                _items.insert(id, locationItem);

//                locationItem->setText(2, QString::fromStdString(locationPtr->getPlanPath()));

                std::vector< FacilityPtr > facilities = locationPtr->getChilds();
                for( FacilityPtr facilityPtr: facilities )
                {
                    QTreeWidgetItem * facilityItem = new QTreeWidgetItem(locationItem);
                    facilityItem->setText(0, facilityPtr->getDescription());
                    const qulonglong id(facilityPtr->getId());
                    facilityItem->setText(1, QString::number(id));
                    facilityItem->setData(0, ID, id);
                    _items.insert(id, facilityItem);

                    FloorPtrs floors = facilityPtr->getChilds();
                    for( FloorPtr floorPtr: floors )
                    {
                        QTreeWidgetItem * floorItem = new QTreeWidgetItem(facilityItem);
                        floorItem->setText(0, floorPtr->getName());
                        const qulonglong id(floorPtr->getId());
                        floorItem->setText(1, QString::number(id));
                        floorItem->setData(0, ID, id);
                        _items.insert(id, floorItem);

//                        floorItem->setText(2, QString::fromStdString(floorPtr->getPlanPath()));
//                        QPixmap pm("/home/sergey/contour_ng/0.tiff");
//                        floorItem->setIcon(0, QIcon(pm));

                        BaseAreaPtrs rooms = floorPtr->getChilds( Floor::FCF_ALL_ROOMS );
                        for( BaseAreaPtr room_ptr: rooms )
                        {
                            RoomPtr room = BaseArea::convert< Room >( room_ptr );
                            if(room)
                            {
                                QTreeWidgetItem * roomItem = new QTreeWidgetItem(floorItem);
                                roomItem->setText(0, room->getName());
                                const qulonglong id(room->getId());
                                roomItem->setText(1, QString::number(id));
                                roomItem->setData(0, ID, id);
                                _items.insert(id, roomItem);

                                roomItem->setText(2, room->getPlanPath());
                            }
                        }
                        floorItem->setExpanded(true);
                    }
                    facilityItem->setExpanded(true);
                }
                locationItem->setExpanded(true);
            }
        }
        regionItem->setExpanded(true);
    }
}

void ViraTreeWidget::slotItemSelectionChanged()
{
    qulonglong id(0);
    QList<QTreeWidgetItem*>list = selectedItems();
    if( ! list.isEmpty())
    {
        QTreeWidgetItem* item = list.first();
        id = item->data(0, ID).toULongLong();
    }
    RegionBizManager::instance()->selectArea(id);
    qDebug() << "slotItemSelectionChanged, ID :" << id;
}

void ViraTreeWidget::slotItemDoubleClicked(QTreeWidgetItem * item, int)
{
    if(item)
    {
        const qulonglong id(item->data(0, ID).toULongLong());
        RegionBizManager::instance()->centerOnArea(id);
        qDebug() << "slotItemDoubleClicked, ID :" << id;
    }
}

void ViraTreeWidget::slotObjectSelectionChanged(uint64_t prev_id, uint64_t curr_id)
{
    disconnect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotItemSelectionChanged()));
    clearSelection();
    if(curr_id > 0)
    {
        auto it = _items.find(curr_id);
        if(it != _items.end())
            it.value()->setSelected(true);
        else
            clearSelection();
    }
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotItemSelectionChanged()));
}

























