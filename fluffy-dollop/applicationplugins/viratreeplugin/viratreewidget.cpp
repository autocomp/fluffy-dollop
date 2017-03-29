#include "viratreewidget.h"
#include "delegate.h"
#include <QDebug>
#include <QHeaderView>
#include <regionbiz/rb_manager.h>
#include <iostream>
#include <ctrcore/tempinputdata/tempdatatypies.h>
#include <ctrcore/tempinputdata/tempdatacontroller.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>
#include <map>

using namespace regionbiz;

ViraTreeWidget::ViraTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{
    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::BlockGUI, this, SLOT(slotBlockGUI(QVariant)),
                                      qMetaTypeId< bool >(),
                                      QString("visualize_system") );

    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::EditModeFinish, this, SLOT(slotEditModeFinish(QVariant)),
                                      qMetaTypeId< quint64 >(),
                                      QString("visualize_system") );

    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::UpdateMark, this, SLOT(slotUpdateMark(QVariant)),
                                      qMetaTypeId< quint64 >(),
                                      QString("visualize_system") );

    setExpandsOnDoubleClick(false);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setColumnCount(7);
    setColumnWidth(0, 200);
    setColumnWidth(1, 200);
    setColumnWidth(2, 200);
    setColumnWidth(3, 200);
    setColumnWidth(4, 200);
    headerItem()->setText(0, QString::fromUtf8(""));
    headerItem()->setText(1, QString::fromUtf8("Площадь"));
    headerItem()->setText(2, QString::fromUtf8("Статус"));
    headerItem()->setText(3, QString::fromUtf8("Арендатор"));
    headerItem()->setText(4, QString::fromUtf8("Задачи"));
    headerItem()->setText(5, QString::fromUtf8("Комментарий"));
    headerItem()->setText(6, QString::fromUtf8("ID"));
    for(int i(0); i<6; ++ i)
        headerItem()->setTextAlignment(i, Qt::AlignCenter);

    LineEditDelegate * lineEditDelegate = new LineEditDelegate(this);
    connect(lineEditDelegate, SIGNAL(saveItemToDb(QModelIndex)), this, SLOT(slotSaveItemToDb(QModelIndex)));
    setItemDelegateForColumn(0, lineEditDelegate);
    setItemDelegateForColumn(3, lineEditDelegate);
    setItemDelegateForColumn(4, lineEditDelegate);
    setItemDelegateForColumn(5, lineEditDelegate);

    SpinBoxDelegate * spinBoxDelegate = new SpinBoxDelegate(this);
    connect(spinBoxDelegate, SIGNAL(resaclArea(QModelIndex)), this, SLOT(slotResaclArea(QModelIndex)));
    connect(spinBoxDelegate, SIGNAL(saveItemToDb(QModelIndex)), this, SLOT(slotSaveItemToDb(QModelIndex)));
    setItemDelegateForColumn(1, spinBoxDelegate);

    ComboBoxDelegate * comboBoxDelegate = new ComboBoxDelegate(this);
    connect(comboBoxDelegate, SIGNAL(resaclArea(QModelIndex)), this, SLOT(slotResaclArea(QModelIndex)));
    connect(comboBoxDelegate, SIGNAL(saveItemToDb(QModelIndex)), this, SLOT(slotSaveItemToDb(QModelIndex)));
    setItemDelegateForColumn(2, comboBoxDelegate);

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
//        regionItem->setText(5, QString::number(id));
        for(int i(0); i<6; ++i) regionItem->setData(i, ID, id);
        for(int i(0); i<6; ++i) regionItem->setData(i, TYPE, (int)ItemTypeOther);
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
                locationItem->setText(6, QString::number(id));
//                locationItem->setText(5, QString::number(id));
                for(int i(0); i<6; ++i) locationItem->setData(i, ID, id);
                for(int i(0); i<6; ++i) locationItem->setData(i, TYPE, (int)ItemTypeOther);
                _items.insert(id, locationItem);

                std::vector< FacilityPtr > facilities = locationPtr->getChilds();
                for( FacilityPtr facilityPtr: facilities )
                {
                    QTreeWidgetItem * facilityItem = new QTreeWidgetItem(locationItem);
                    facilityItem->setFlags(facilityItem->flags() | Qt::ItemIsEditable);
                    facilityItem->setText(0, facilityPtr->getDescription());
                    const qulonglong id(facilityPtr->getId());
                    facilityItem->setText(6, QString::number(id));
                    for(int i(0); i<6; ++i) facilityItem->setData(i, ID, id);
                    for(int i(0); i<6; ++i) facilityItem->setData(i, TYPE, (int)ItemTypeFacility);
                    _items.insert(id, facilityItem);

                    FloorPtrs floors = facilityPtr->getChilds();
                    for( FloorPtr floorPtr: floors )
                    {
                        QTreeWidgetItem * floorItem = new QTreeWidgetItem(facilityItem);
                        floorItem->setText(0, floorPtr->getName());
                        const qulonglong id(floorPtr->getId());
                        floorItem->setText(6, QString::number(id));
                        floorItem->setFlags(floorItem->flags() | Qt::ItemIsEditable);
                        for(int i(0); i<6; ++i) floorItem->setData(i, ID, id);
                        for(int i(0); i<6; ++i) floorItem->setData(i, TYPE, (int)ItemTypeFloor);
                        floorItem->setTextAlignment(1, Qt::AlignCenter);
                        floorItem->setData(1, RENTED_AREA, 234.4);
                        floorItem->setData(1, TOTAL_AREA, 373.6);
                        _items.insert(id, floorItem);

                        BaseAreaPtrs rooms = floorPtr->getChilds( Floor::FCF_ALL_ROOMS );
                        for( BaseAreaPtr room_ptr: rooms )
                        {
                            RoomPtr room = BaseArea::convert< Room >( room_ptr );
                            if(room)
                            {
                                const qulonglong id(room->getId());
                                QTreeWidgetItem * roomItem = new QTreeWidgetItem(floorItem);
                                roomItem->setFlags(roomItem->flags() | Qt::ItemIsEditable);
                                roomItem->setTextAlignment(1, Qt::AlignCenter);
                                roomItem->setTextAlignment(2, Qt::AlignCenter);
                                roomItem->setTextAlignment(3, Qt::AlignCenter);
                                roomItem->setTextAlignment(4, Qt::AlignCenter);

                                for(int i(0); i<6; ++i) roomItem->setData(i, TYPE, (int)ItemTypeRoom);
                                for(int i(0); i<6; ++i) roomItem->setData(i, ID, id);
                                if(room->getCoords().isEmpty())
                                    for(int i(0); i < 6; ++i) roomItem->setTextColor(i, _noCoordColor);
                                else
                                    for(int i(0); i < 6; ++i) roomItem->setTextColor(i, _defaultColor);

                                roomItem->setText(0, room->getName());
                                roomItem->setText(6, QString::number(id));

                                BaseMetadataPtr areaPtr = room->getMetadata("area");
                                if(areaPtr)
                                {
                                    double area = areaPtr->getValueAsVariant().toDouble();
                                    roomItem->setText(1, QString::number(area, 'f', 1));
                                }

                                BaseMetadataPtr rentorPtr = room->getMetadata("rentor");
                                if(rentorPtr)
                                    roomItem->setText(3, rentorPtr->getValueAsVariant().toString());

                                BaseMetadataPtr statusPtr = room->getMetadata("status");
                                if(statusPtr)
                                {
                                    QString status = statusPtr->getValueAsVariant().toString();
                                    if(status == QString::fromUtf8("Cвободно") || status == QString::fromUtf8("Свободно"))
                                        roomItem->setText(2, QString::fromUtf8("Свободно"));
                                    else if(status == QString::fromUtf8("В аренде"))
                                        roomItem->setText(2, status);
                                    else
                                    {
                                        roomItem->setText(2, QString::fromUtf8("Недоступно"));
                                        qDebug() << QString::fromUtf8("Недоступно --->") << statusPtr->getValueAsVariant();
                                    }
                                }

                                BaseMetadataPtr commentPtr = room->getMetadata("comment");
                                if(commentPtr)
                                    roomItem->setText(5, commentPtr->getValueAsVariant().toString());



                                _items.insert(id, roomItem);
                            }
                        }
                        //floorItem->setExpanded(true);
                    }
                    facilityItem->setExpanded(true);
                    recalcAreaInFacility(facilityItem);
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

void ViraTreeWidget::recalcAreaInFacility(QTreeWidgetItem * facilityItem)
{
    double facilityRentedArea = 0;
    double facilityTotalArea = 0;

    for(int i(0); i < facilityItem->childCount(); ++i)
    {
        double floorRentedArea = 0;
        double floorTotalArea = 0;
        QTreeWidgetItem * floorItem = facilityItem->child(i);
        for(int i(0); i < floorItem->childCount(); ++i)
        {
            QTreeWidgetItem * roomItem = floorItem->child(i);
            double area = roomItem->text(1).toDouble();
            floorTotalArea += area;
            if(roomItem->text(2) == QString::fromUtf8("В аренде"))
                floorRentedArea += area;
        }
        floorItem->setData(1, RENTED_AREA, floorRentedArea);
        floorItem->setData(1, TOTAL_AREA, floorTotalArea);

        facilityTotalArea += floorTotalArea;
        facilityRentedArea += floorRentedArea;
    }
    facilityItem->setData(1, RENTED_AREA, facilityRentedArea);
    facilityItem->setData(1, TOTAL_AREA, facilityTotalArea);
}

void ViraTreeWidget::slotBlockGUI(QVariant var)
{
    setDisabled(var.toBool());
}

void ViraTreeWidget::slotEditModeFinish(QVariant var)
{
    quint64 id = var.toUInt();
    if(id != 0)
    {
        auto it = _items.find(id);
        if(it != _items.end())
        {
            BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(id, BaseArea::AT_ROOM);
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
}

void ViraTreeWidget::slotResaclArea(const QModelIndex &index)
{
    QTreeWidgetItem * item = itemFromIndex(index);
    if( ! item) return;

    int itemType = item->data(0, TYPE).toInt();
    while(itemType != ItemTypeFacility)
    {
        item = item->parent();
        if( ! item)
            break;
        itemType = item->data(0, TYPE).toInt();
    }

    if(item)
        recalcAreaInFacility(item);
}

void ViraTreeWidget::slotSaveItemToDb(const QModelIndex &index)
{
    qDebug() << "---> slotSaveItemToDb" << index.column() << index.row();
    QTreeWidgetItem * item = itemFromIndex(index);
    if( ! item) return;

    const qulonglong id(item->data(0, ID).toULongLong());
    BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(id);
    if( ! ptr) return;
    switch(item->data(0, TYPE).toInt())
    {
    case ItemTypeRoom :
    {
        if(item->text(1).isEmpty() == false)
            ptr->addMetadata("double", "area", item->text(1).toDouble());

        if(item->text(2).isEmpty() == false)
            ptr->addMetadata("string", "status", item->text(2));

        if(item->text(3).isEmpty() == false)
            ptr->addMetadata("string", "rentor", item->text(3));

        //if(item->text(5).isEmpty() == false)
            ptr->addMetadata("string", "comment", item->text(5));
    }break;
    case ItemTypeFloor : {
//        ptr->addMetadata("string", "comment", item->text(5));

    }break;
    case ItemTypeFacility : {
//        ptr->addMetadata("string", "comment", item->text(5));

    }break;
    case ItemTypeOther : {
//        ptr->addMetadata("string", "comment", item->text(5));

    }break;
    default:
        return;
    };
    bool res = ptr->commit();
    qDebug() << "ID :" << id << ", commit:" << res;
    qDebug() << ptr->getName() << "---------------";
    MetadataByName map = ptr->getMetadataMap();
    for(auto it=map.begin(); it != map.end(); ++it)
    {
        QString tag = it->first;
        BaseMetadataPtr _ptr = it->second;
        qDebug() << tag << _ptr->getValueAsVariant();
    }
    qDebug() << "===================================";
}

void ViraTreeWidget::slotUpdateMark(QVariant var)
{
    MarkPtr markPtr = RegionBizManager::instance()->getMark(var.toUInt());
    if(! markPtr) return;
    BaseAreaPtr parentPtr = RegionBizManager::instance()->getBaseArea(markPtr->getParentId());
    if(! parentPtr) return;


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

























