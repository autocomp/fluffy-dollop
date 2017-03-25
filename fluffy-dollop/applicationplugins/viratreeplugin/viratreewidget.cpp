#include "viratreewidget.h"
#include <QDebug>
#include <QHeaderView>
#include <regionbiz/rb_manager.h>
#include <iostream>
#include <ctrcore/tempinputdata/tempdatatypies.h>
#include <ctrcore/tempinputdata/tempdatacontroller.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>

using namespace regionbiz;

ViraTreeWidget::ViraTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::EditObjectGeometry, this, SLOT(slotEditObjectGeometry(QVariant)),
                                      qMetaTypeId< quint64 >(),
                                      QString("visualize_system") );

    setExpandsOnDoubleClick(false);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setColumnCount(6);
    setColumnWidth(0, 370);
    headerItem()->setText(0, QString::fromUtf8(""));
    headerItem()->setText(1, QString::fromUtf8("Площадь"));
    headerItem()->setText(2, QString::fromUtf8("Аренда"));
    headerItem()->setText(3, QString::fromUtf8("Задачи"));
    headerItem()->setText(4, QString::fromUtf8("Арендатор"));
    headerItem()->setText(5, QString::fromUtf8("ID"));

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
        regionItem->setText(5, QString::number(id));
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
                locationItem->setText(5, QString::number(id));
                locationItem->setData(0, ID, id);
                _items.insert(id, locationItem);

                std::vector< FacilityPtr > facilities = locationPtr->getChilds();
                for( FacilityPtr facilityPtr: facilities )
                {
                    QTreeWidgetItem * facilityItem = new QTreeWidgetItem(locationItem);
                    facilityItem->setText(0, facilityPtr->getDescription());
                    const qulonglong id(facilityPtr->getId());
                    facilityItem->setText(5, QString::number(id));
                    facilityItem->setData(0, ID, id);
                    _items.insert(id, facilityItem);

                    temp_data::AreaInfo facilityInfo;

                    FloorPtrs floors = facilityPtr->getChilds();
                    for( FloorPtr floorPtr: floors )
                    {
                        QTreeWidgetItem * floorItem = new QTreeWidgetItem(facilityItem);
                        floorItem->setText(0, floorPtr->getName());
                        const qulonglong id(floorPtr->getId());
                        floorItem->setText(5, QString::number(id));
                        floorItem->setData(0, ID, id);
                        _items.insert(id, floorItem);

                        temp_data::AreaInfo floorInfo;

//                        floorItem->setText(2, floorPtr->getPlanPath());
//                        QPixmap pm("/home/sergey/contour_ng/0.tiff");
//                        floorItem->setIcon(0, QIcon(pm));

                        BaseAreaPtrs rooms = floorPtr->getChilds( Floor::FCF_ALL_ROOMS );
                        for( BaseAreaPtr room_ptr: rooms )
                        {
                            RoomPtr room = BaseArea::convert< Room >( room_ptr );
                            if(room)
                            {
                                QTreeWidgetItem * roomItem = new QTreeWidgetItem(floorItem);
                                if(room->getCoords().isEmpty())
                                    for(int i(0); i < 6; ++i)
                                    {
                                        _defaultColor = roomItem->textColor(i);
                                        roomItem->setTextColor(i, QColor(Qt::yellow));
                                    }

                                roomItem->setText(0, room->getName());
                                const qulonglong id(room->getId());
                                roomItem->setText(5, QString::number(id));
                                roomItem->setData(0, ID, id);

                                temp_data::RoomInfo roomInfo;
                                if(TempDataController::instance()->getRoomInfo(room->getId(), roomInfo))
                                {
                                    roomItem->setText(1, QString::number(roomInfo.square, 'f', 1));
                                    switch (roomInfo.state)
                                    {
                                    case temp_data::RoomState::Free :
                                        roomItem->setText(2, QString::fromUtf8("Свободно"));
                                        floorInfo.squareFree += roomInfo.square;
                                        break;
                                    case temp_data::RoomState::InTenant :
                                        roomItem->setText(2, QString::fromUtf8("В аренде"));
                                        floorInfo.squareInTenant += roomInfo.square;
                                        break;
                                    case temp_data::RoomState::Reconstruction :
                                        roomItem->setText(2, QString::fromUtf8("Ремонт"));
                                        floorInfo.squareFree += roomInfo.square;
                                        break;
                                    case temp_data::RoomState::Sale :
                                        roomItem->setText(2, QString::fromUtf8("Продажа"));
                                        floorInfo.squareFree += roomInfo.square;
                                        break;
                                    }

                                    temp_data::Tenant tenant;
                                    if(TempDataController::instance()->getTenant(roomInfo.tenant, tenant))
                                    {
                                        roomItem->setText(4, tenant.name);
                                        floorInfo.tenants.insert(roomInfo.tenant);
                                        facilityInfo.tenants.insert(roomInfo.tenant);
                                    }
                                }

                                _items.insert(id, roomItem);
                            }
                        }
                        //floorItem->setExpanded(true);
                        facilityInfo.squareFree += floorInfo.squareFree;
                        facilityInfo.squareInTenant += floorInfo.squareInTenant;
                        QString text1 = QString::number(floorInfo.squareInTenant) + QString(" / ") + QString::number(floorInfo.squareFree);
                        floorItem->setText(1, text1);

                        foreach(qulonglong id, floorInfo.tenants)
                        {
                            temp_data::Tenant tenant;
                            if(TempDataController::instance()->getTenant(id, tenant))
                            {
                                if(tenant.debt > 0)
                                    floorInfo.debt += tenant.debt;
                            }
                        }

                        QString text2 = QString::number(floorInfo.tenants.size()) + QString(" / ") + QString::number(floorInfo.debt);
                        floorItem->setText(2, text2);

                        TempDataController::instance()->setAreaInfo(floorPtr->getId(), floorInfo);
                    }
                    facilityItem->setExpanded(true);

                    QString text1 = QString::number(facilityInfo.squareInTenant) + QString(" / ") + QString::number(facilityInfo.squareFree);
                    facilityItem->setText(1, text1);

                    foreach(qulonglong id, facilityInfo.tenants)
                    {
                        temp_data::Tenant tenant;
                        if(TempDataController::instance()->getTenant(id, tenant))
                        {
                            if(tenant.debt > 0)
                                facilityInfo.debt += tenant.debt;
                        }
                    }
                    QString text2 = QString::number(facilityInfo.tenants.size()) + QString(" / ") + QString::number(facilityInfo.debt);
                    facilityItem->setText(2, text2);

                    TempDataController::instance()->setAreaInfo(facilityPtr->getId(), facilityInfo);
                }
                locationItem->setExpanded(true);
            }
        }
        regionItem->setExpanded(true);
    }

    connect(header(), SIGNAL(sectionClicked(int)), this, SLOT(slotHeaderSectionClicked(int)));
    header()->setSortIndicatorShown(true);
    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
}

void ViraTreeWidget::slotEditObjectGeometry(QVariant var)
{
    quint64 id = var.toUInt();
    setEnabled(id == 0);
    if(id == 0)
    {
        auto it = _items.find(_editObjectGeometry);
        if(it != _items.end())
        {
            BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(_editObjectGeometry, BaseArea::AT_ROOM);
            RoomPtr roomPtr = BaseArea::convert< Room >(ptr);
            if(roomPtr)
            {
                QTreeWidgetItem* roomItem = it.value();
                if(roomPtr->getCoords().isEmpty() == false)
                    for(int i(0); i < 6; ++i)
                        roomItem->setTextColor(i, _defaultColor);
            }
        }
    }
    _editObjectGeometry = id;
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
        {
            it.value()->setSelected(true);
            scrollToItem(it.value());
        }
        else
            clearSelection();
    }
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotItemSelectionChanged()));
}

void ViraTreeWidget::slotHeaderSectionClicked(int index)
{
    sortByColumn(index);
}

























