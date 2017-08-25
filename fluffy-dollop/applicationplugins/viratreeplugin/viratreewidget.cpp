#include "viratreewidget.h"
#include "delegate.h"
#include "area_treewidget_items.h"
#include "addbaseareaform.h"
#include <QDebug>
#include <QHeaderView>
#include <regionbiz/rb_manager.h>
#include <iostream>
#include <ctrcore/tempinputdata/tempdatatypies.h>
#include <ctrcore/tempinputdata/tempdatacontroller.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <map>
#include <QMenu>
#include <QAction>
#include <QCursor>
#include <QMessageBox>

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

    setExpandsOnDoubleClick(false);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setColumnCount(ColumnCount);
    for(int i(0); i<ColumnCount; ++i)
        setColumnWidth(i, 200);

    setColumnWidth((int)ColumnTitle::BASE_RENT, 150);
    setColumnWidth((int)ColumnTitle::STATUS, 150);

    headerItem()->setText((int)ColumnTitle::NAME, QString::fromUtf8(""));
    headerItem()->setText((int)ColumnTitle::SQUARE, QString::fromUtf8("Площадь"));
//    headerItem()->setText((int)ColumnTitle::BASE_RENT, QString::fromUtf8("Арендная ставка"));
//    headerItem()->setText((int)ColumnTitle::CALC_RENT, QString::fromUtf8("Арендная плата"));
//    headerItem()->setText((int)ColumnTitle::STATUS, QString::fromUtf8("Статус"));
//    headerItem()->setText((int)ColumnTitle::RENTER, QString::fromUtf8("Арендатор"));
    headerItem()->setText((int)ColumnTitle::TASKS, QString::fromUtf8("Задачи"));
    headerItem()->setText((int)ColumnTitle::COMMENT, QString::fromUtf8("Комментарий"));
    headerItem()->setText((int)ColumnTitle::ID, QString::fromUtf8("ID"));
    for(int i(0); i<ColumnCount; ++ i)
        headerItem()->setTextAlignment(i, Qt::AlignCenter);

    LineEditDelegate * lineEditDelegate = new LineEditDelegate(this);
    connect(lineEditDelegate, SIGNAL(saveItemToDb(QModelIndex)), this, SLOT(slotSaveItemToDb(QModelIndex)));
    setItemDelegateForColumn((int)ColumnTitle::NAME, lineEditDelegate);
    setItemDelegateForColumn((int)ColumnTitle::RENTER, lineEditDelegate);
    setItemDelegateForColumn((int)ColumnTitle::COMMENT, lineEditDelegate);

    SpinBoxDelegate * spinBoxDelegate = new SpinBoxDelegate(this);
    connect(spinBoxDelegate, SIGNAL(resaclArea(QModelIndex)), this, SLOT(slotResaclArea(QModelIndex)));
    connect(spinBoxDelegate, SIGNAL(saveItemToDb(QModelIndex)), this, SLOT(slotSaveItemToDb(QModelIndex)));
    setItemDelegateForColumn((int)ColumnTitle::SQUARE, spinBoxDelegate);
    setItemDelegateForColumn((int)ColumnTitle::BASE_RENT, spinBoxDelegate);
    setItemDelegateForColumn((int)ColumnTitle::CALC_RENT, spinBoxDelegate);

    ComboBoxDelegate * comboBoxDelegate = new ComboBoxDelegate(this);
    connect(comboBoxDelegate, SIGNAL(resaclArea(QModelIndex)), this, SLOT(slotResaclArea(QModelIndex)));
    connect(comboBoxDelegate, SIGNAL(saveItemToDb(QModelIndex)), this, SLOT(slotSaveItemToDb(QModelIndex)));
    setItemDelegateForColumn((int)ColumnTitle::STATUS, comboBoxDelegate);

    StatusDelegate * statusDelegate = new StatusDelegate(this);
    setItemDelegateForColumn((int)ColumnTitle::TASKS, statusDelegate);

    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotItemSelectionChanged()));
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), this, SLOT(slotItemDoubleClicked(QTreeWidgetItem*,int)));

    auto mngr = RegionBizManager::instance();
    mngr->subscribeOnCurrentChange(this, SLOT(slotObjectSelectionChanged(uint64_t,uint64_t)));
    mngr->subscribeOnChangeEntity(this, SLOT(slotObjectChanged(uint64_t)));
    mngr->subscribeOnAddEntity(this, SLOT(slotAddObject(uint64_t)));
    mngr->subscribeOnDeleteEntity(this, SLOT(slotDeleteObject(uint64_t)));

    regionbiz::EntityFilter::subscribeOnFilterChacnge(this, SLOT(reinit()));

    connect(header(), SIGNAL(sectionClicked(int)), this, SLOT(slotHeaderSectionClicked(int)));
    header()->setSortIndicatorShown(true);
    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);

    //reinit();
}


void ViraTreeWidget::reinit()
{
    _items.clear();
    clear();

    std::vector<RegionPtr> regions = RegionBizManager::instance()->getRegions();
    for( RegionPtr regionPtr: regions )
    {
        QTreeWidgetItem * regionItem = new QTreeWidgetItem(this);
        QString name = regionPtr->getName();
        if(name.isEmpty())
            name = regionPtr->getDescription();
        regionItem->setText((int)ColumnTitle::NAME, name);
        const qulonglong id(regionPtr->getId());
        regionItem->setText((int)ColumnTitle::ID, QString::number(id));
        for(int i(0); i<ColumnCount; ++i) regionItem->setData(i, ID, id);
        for(int i(0); i<ColumnCount; ++i) regionItem->setData(i, TYPE, (int)ItemType::ItemTypeRegion);
        if(regionPtr->getCoords().isEmpty())
            for(int i(0); i < ColumnCount; ++i) regionItem->setTextColor(i, _noCoordColor);
        else
            for(int i(0); i < ColumnCount; ++i) regionItem->setTextColor(i, _defaultColor);
        _items.insert(id, regionItem);

        std::vector<BaseAreaPtr> locations = regionPtr->getChilds( Region::RCF_LOCATIONS );
        for( BaseAreaPtr ptr: locations )
        {
            LocationPtr locationPtr = BaseArea::convert< Location >( ptr );
            if(locationPtr)
            {
                QTreeWidgetItem * locationItem = new QTreeWidgetItem(regionItem);
                name = locationPtr->getName();
                if(name.isEmpty())
                    name = locationPtr->getDescription();
                locationItem->setText((int)ColumnTitle::NAME, name);
                const qulonglong id(locationPtr->getId());
                locationItem->setText((int)ColumnTitle::ID, QString::number(id));
                for(int i(0); i<ColumnCount; ++i) locationItem->setData(i, ID, id);
                for(int i(0); i<ColumnCount; ++i) locationItem->setData(i, TYPE, (int)ItemType::ItemTypeLocation);
                if(locationPtr->getCoords().isEmpty())
                    for(int i(0); i < ColumnCount; ++i) locationItem->setTextColor(i, _noCoordColor);
                else
                    for(int i(0); i < ColumnCount; ++i) locationItem->setTextColor(i, _defaultColor);
                _items.insert(id, locationItem);

                std::vector< FacilityPtr > facilities = locationPtr->getChilds();
                for( FacilityPtr facilityPtr: facilities )
                {
                    QTreeWidgetItem * facilityItem = new QTreeWidgetItem(locationItem);
                    facilityItem->setFlags(facilityItem->flags() | Qt::ItemIsEditable);
                    name = facilityPtr->getName();
                    if(name.isEmpty())
                        name = facilityPtr->getDescription();
                    facilityItem->setText((int)ColumnTitle::NAME, name);
                    const qulonglong id(facilityPtr->getId());
                    facilityItem->setText((int)ColumnTitle::ID, QString::number(id));
                    for(int i(0); i<ColumnCount; ++i) facilityItem->setData(i, ID, id);
                    for(int i(0); i<ColumnCount; ++i) facilityItem->setData(i, TYPE, (int)ItemType::ItemTypeFacility);
                    if(facilityPtr->getCoords().isEmpty())
                        for(int i(0); i < ColumnCount; ++i) facilityItem->setTextColor(i, _noCoordColor);
                    else
                        for(int i(0); i < ColumnCount; ++i) facilityItem->setTextColor(i, _defaultColor);
                    _items.insert(id, facilityItem);

                    FloorPtrs floors = facilityPtr->getChilds();
                    for( FloorPtr floorPtr: floors )
                    {
                        QTreeWidgetItem* floorItem = new FloorTreeWidgetItem(facilityItem);
                        floorItem->setText((int)ColumnTitle::NAME, floorPtr->getName());
                        if( !floorPtr->getDescription().isEmpty() )
                            floorItem->setToolTip( 0, floorPtr->getDescription() );
                        const qulonglong id(floorPtr->getId());
                        floorItem->setText((int)ColumnTitle::ID, QString::number(id));
                        floorItem->setFlags(floorItem->flags() | Qt::ItemIsEditable);
                        for(int i(0); i<ColumnCount; ++i) floorItem->setData(i, ID, id);
                        for(int i(0); i<ColumnCount; ++i) floorItem->setData(i, TYPE, (int)ItemType::ItemTypeFloor);
                        floorItem->setTextAlignment(1, Qt::AlignCenter);
                        _items.insert(id, floorItem);

                        int floor_tasks_new(0);
                        int floor_tasks_in_work(0);
                        int floor_tasks_for_check(0);
                        // WARNING check marks of floor
                        MarkPtrs marks_of_floor = floorPtr->getMarks();
                        for( MarkPtr mark: marks_of_floor )
                            if(mark->getMarkType() == Mark::MT_DEFECT)
                            {
                                BaseMetadataPtr status = mark->getMetadata("status");
                                if(status)
                                {
                                    QString statusStr = status->getValueAsString();
                                    if(statusStr == QString::fromUtf8("новый"))
                                        ++floor_tasks_new;
                                    else if(statusStr == QString::fromUtf8("в работе"))
                                        ++floor_tasks_in_work;
                                    else if(statusStr == QString::fromUtf8("на проверку"))
                                        ++floor_tasks_for_check;
                                }
                            }
                        floorItem->setData((int)ColumnTitle::TASKS, TASKS_NEW, floor_tasks_new);
                        floorItem->setData((int)ColumnTitle::TASKS, TASKS_IN_WORK, floor_tasks_in_work);
                        floorItem->setData((int)ColumnTitle::TASKS, TASKS_FOR_CHECK, floor_tasks_for_check);

                        RoomPtrs rooms = floorPtr->getChilds();
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

                                for(int i(0); i<ColumnCount; ++i) roomItem->setData(i, TYPE, (int)ItemType::ItemTypeRoom);
                                for(int i(0); i<ColumnCount; ++i) roomItem->setData(i, ID, id);
                                if(room->getCoords().isEmpty())
                                    for(int i(0); i < ColumnCount; ++i) roomItem->setTextColor(i, _noCoordColor);
                                else
                                    for(int i(0); i < ColumnCount; ++i) roomItem->setTextColor(i, _defaultColor);

                                roomItem->setText((int)ColumnTitle::NAME, room->getName());
                                if( !room->getDescription().isEmpty() )
                                    roomItem->setToolTip( 0, room->getDescription() );
                                roomItem->setText((int)ColumnTitle::ID, QString::number(id));

                                BaseMetadataPtr areaPtr = room->getMetadata("area");
                                if(areaPtr)
                                {
                                    double area = areaPtr->getValueAsVariant().toDouble();
                                    roomItem->setText((int)ColumnTitle::SQUARE, QString::number(area, 'f', 1));
                                }

                                BaseMetadataPtr baseRentPtr = room->getMetadata("base_rent");
                                if(baseRentPtr)
                                {
                                    double baseRent = baseRentPtr->getValueAsVariant().toDouble();
                                    roomItem->setText((int)ColumnTitle::BASE_RENT, QString::number(baseRent, 'f', 2));
                                }

                                BaseMetadataPtr rentorPtr = room->getMetadata("rentor");
                                if(rentorPtr)
                                    roomItem->setText((int)ColumnTitle::RENTER, rentorPtr->getValueAsVariant().toString());

                                BaseMetadataPtr statusPtr = room->getMetadata("status");
                                if(statusPtr)
                                {
                                    QString status = statusPtr->getValueAsVariant().toString();
                                    if(status == QString::fromUtf8("Свободно"))
                                        roomItem->setText((int)ColumnTitle::STATUS, status);
                                    else if(status == QString::fromUtf8("В аренде"))
                                        roomItem->setText((int)ColumnTitle::STATUS, status);
                                    else
                                    {
                                        roomItem->setText((int)ColumnTitle::STATUS, QString::fromUtf8("Недоступно"));
                                        qDebug() << QString::fromUtf8("Недоступно --->") << statusPtr->getValueAsVariant();
                                    }
                                }

                                BaseMetadataPtr commentPtr = room->getMetadata("comment");
                                if(commentPtr)
                                    roomItem->setText((int)ColumnTitle::COMMENT, commentPtr->getValueAsVariant().toString());

                                int tasks_new(0);
                                int tasks_in_work(0);
                                int tasks_for_check(0);
                                MarkPtrs marks_of_room = room->getMarks();
                                for( MarkPtr mark: marks_of_room )
                                    if(mark->getMarkType() == Mark::MT_DEFECT)
                                    {
                                        BaseMetadataPtr status = mark->getMetadata("status");
                                        if(status)
                                        {
                                            QString statusStr = status->getValueAsString();
                                            if(statusStr == QString::fromUtf8("новый"))
                                                ++tasks_new;
                                            else if(statusStr == QString::fromUtf8("в работе"))
                                                ++tasks_in_work;
                                            else if(statusStr == QString::fromUtf8("на проверку"))
                                                ++tasks_for_check;
                                        }
                                    }
                                roomItem->setData((int)ColumnTitle::TASKS, TASKS_NEW, tasks_new);
                                roomItem->setData((int)ColumnTitle::TASKS, TASKS_IN_WORK, tasks_in_work);
                                roomItem->setData((int)ColumnTitle::TASKS, TASKS_FOR_CHECK, tasks_for_check);

                                _items.insert(id, roomItem);
                            }
                        }
                        //floorItem->setExpanded(true);
                    }
                    facilityItem->setExpanded(true);
                    recalcAreaInFacility(facilityItem);
                    recalcTasksInFacility(facilityItem);
                }
                locationItem->setExpanded(true);
            }
        }
        regionItem->setExpanded(true);
    }
}

void ViraTreeWidget::checkMarks(QTreeWidgetItem * item)
{
    qulonglong id = item->data(0, ID).toULongLong();
    auto areaPtr = RegionBizManager::instance()->getBaseArea(id);
    if(areaPtr)
    {
        auto holder = areaPtr->convert<MarksHolder>();
        if(holder)
        {
            int tasks_new(0);
            int tasks_in_work(0);
            int tasks_for_check(0);
            MarkPtrs marks = holder->getMarks();
            for( MarkPtr mark: marks )
                if(mark->getMarkType() == Mark::MT_DEFECT)
                {
                    BaseMetadataPtr status = mark->getMetadata("status");
                    if(status)
                    {
                        QString statusStr = status->getValueAsString();
                        if(statusStr == QString::fromUtf8("новый"))
                            ++tasks_new;
                        else if(statusStr == QString::fromUtf8("в работе"))
                            ++tasks_in_work;
                        else if(statusStr == QString::fromUtf8("на проверку"))
                            ++tasks_for_check;
                    }
                }
            item->setData((int)ColumnTitle::TASKS, TASKS_NEW, tasks_new);
            item->setData((int)ColumnTitle::TASKS, TASKS_IN_WORK, tasks_in_work);
            item->setData((int)ColumnTitle::TASKS, TASKS_FOR_CHECK, tasks_for_check);
        }
    }
}

void ViraTreeWidget::recalcAreaInFacility(QTreeWidgetItem * facilityItem)
{
    double facilityRealRent = 0;
    double facilityPossibleRent = 0;
    double facilityRentedArea = 0;
    double facilityTotalArea = 0;

    for(int i(0); i < facilityItem->childCount(); ++i)
    {
        double floorRealRent = 0;
        double floorPossibleRent = 0;
        double floorRentedArea = 0;
        double floorTotalArea = 0;
        QTreeWidgetItem * floorItem = facilityItem->child(i);
        for(int i(0); i < floorItem->childCount(); ++i)
        {
            QTreeWidgetItem * roomItem = floorItem->child(i);
            double area = roomItem->text((int)ColumnTitle::SQUARE).toDouble();
            floorTotalArea += area;
            bool rented = roomItem->text((int)ColumnTitle::STATUS) == QString::fromUtf8("В аренде");
            if(rented)
                floorRentedArea += area;

            bool baseRentOk;
            double baseRent = roomItem->text((int)ColumnTitle::BASE_RENT).toDouble(&baseRentOk);
            if(baseRentOk)
            {
                double rent = baseRent * area;
                floorPossibleRent += rent;
                roomItem->setText((int)ColumnTitle::CALC_RENT, QString::number(rent, 'f', 2));
                if(rented)
                    floorRealRent += rent;
            }
        }
        floorItem->setData((int)ColumnTitle::SQUARE, RENTED_AREA, floorRentedArea);
        floorItem->setData((int)ColumnTitle::SQUARE, TOTAL_AREA, floorTotalArea);
        facilityTotalArea += floorTotalArea;
        facilityRentedArea += floorRentedArea;

        floorItem->setData((int)ColumnTitle::CALC_RENT, REAL_RENT, floorRealRent);
        floorItem->setData((int)ColumnTitle::CALC_RENT, POSSIBLE_RENT, floorPossibleRent);
        facilityRealRent += floorRealRent;
        facilityPossibleRent += floorPossibleRent;
    }
    facilityItem->setData((int)ColumnTitle::SQUARE, RENTED_AREA, facilityRentedArea);
    facilityItem->setData((int)ColumnTitle::SQUARE, TOTAL_AREA, facilityTotalArea);

    facilityItem->setData((int)ColumnTitle::CALC_RENT, REAL_RENT, facilityRealRent);
    facilityItem->setData((int)ColumnTitle::CALC_RENT, POSSIBLE_RENT, facilityPossibleRent);
}

void ViraTreeWidget::recalcTasksInFacility(QTreeWidgetItem * facilityItem)
{
    int facility_tasks_new(0);
    int facility_tasks_in_work(0);
    int facility_tasks_for_check(0);
    for(int i(0); i < facilityItem->childCount(); ++i)
    {
        QTreeWidgetItem * floorItem = facilityItem->child(i);
        int floor_tasks_new(0);
        int floor_tasks_in_work(0);
        int floor_tasks_for_check(0);
        // WARNING check marks of floor
        uint64_t floor_id = floorItem->data( 0, ID ).toULongLong();
        auto areaPtr = RegionBizManager::instance()->getBaseArea( floor_id );
        FloorPtr floorPtr = BaseArea::convert< Floor >( areaPtr );
        if( floorPtr )
        {
            MarkPtrs marks_of_floor = floorPtr->getMarks();
            for( MarkPtr mark: marks_of_floor )
                if(mark->getMarkType() == Mark::MT_DEFECT)
                {
                    BaseMetadataPtr status = mark->getMetadata("status");
                    if(status)
                    {
                        QString statusStr = status->getValueAsString();
                        if(statusStr == QString::fromUtf8("новый"))
                            ++floor_tasks_new;
                        else if(statusStr == QString::fromUtf8("в работе"))
                            ++floor_tasks_in_work;
                        else if(statusStr == QString::fromUtf8("на проверку"))
                            ++floor_tasks_for_check;
                    }
                }
        }

        for(int i(0); i < floorItem->childCount(); ++i)
        {
            QTreeWidgetItem * roomItem = floorItem->child(i);
            checkMarks(roomItem);

            floor_tasks_new += roomItem->data((int)ColumnTitle::TASKS, TASKS_NEW).toInt();
            floor_tasks_in_work += roomItem->data((int)ColumnTitle::TASKS, TASKS_IN_WORK).toInt();
            floor_tasks_for_check += roomItem->data((int)ColumnTitle::TASKS, TASKS_FOR_CHECK).toInt();
        }
        floorItem->setData((int)ColumnTitle::TASKS, TASKS_NEW, floor_tasks_new);
        floorItem->setData((int)ColumnTitle::TASKS, TASKS_IN_WORK, floor_tasks_in_work);
        floorItem->setData((int)ColumnTitle::TASKS, TASKS_FOR_CHECK, floor_tasks_for_check);

        facility_tasks_new += floor_tasks_new;
        facility_tasks_in_work += floor_tasks_in_work;
        facility_tasks_for_check += floor_tasks_for_check;
    }

    uint64_t facility_id = facilityItem->data( 0, ID ).toULongLong();
    auto facility_ptr = RegionBizManager::instance()->getBaseArea( facility_id );
    FacilityPtr facilityPtr = BaseArea::convert< Facility >( facility_ptr );
    if( facilityPtr )
    {
        MarkPtrs marks_of_facility = facilityPtr->getMarks();
        for( MarkPtr mark: marks_of_facility )
            if(mark->getMarkType() == Mark::MT_DEFECT)
            {
                BaseMetadataPtr status = mark->getMetadata("status");
                if(status)
                {
                    QString statusStr = status->getValueAsString();
                    if(statusStr == QString::fromUtf8("новый"))
                        ++facility_tasks_new;
                    else if(statusStr == QString::fromUtf8("в работе"))
                        ++facility_tasks_in_work;
                    else if(statusStr == QString::fromUtf8("на проверку"))
                        ++facility_tasks_for_check;
                }
            }
    }
    facilityItem->setData((int)ColumnTitle::TASKS, TASKS_NEW, facility_tasks_new);
    facilityItem->setData((int)ColumnTitle::TASKS, TASKS_IN_WORK, facility_tasks_in_work);
    facilityItem->setData((int)ColumnTitle::TASKS, TASKS_FOR_CHECK, facility_tasks_for_check);

    if(facilityItem->parent())
        recalcTasksInLocation(facilityItem->parent());
}

void ViraTreeWidget::recalcTasksInLocation(QTreeWidgetItem * locationItem)
{
    int location_tasks_new(0);
    int location_tasks_in_work(0);
    int location_tasks_for_check(0);
    for(int i(0); i < locationItem->childCount(); ++i)
    {
        QTreeWidgetItem * _facilityItem = locationItem->child(i);
        if( ! _facilityItem) continue;

        location_tasks_new += _facilityItem->data((int)ColumnTitle::TASKS, TASKS_NEW).toInt();
        location_tasks_in_work += _facilityItem->data((int)ColumnTitle::TASKS, TASKS_IN_WORK).toInt();
        location_tasks_for_check += _facilityItem->data((int)ColumnTitle::TASKS, TASKS_FOR_CHECK).toInt();
    }
    uint64_t location_id = locationItem->data( 0, ID ).toULongLong();
    auto location_ptr = RegionBizManager::instance()->getBaseArea( location_id );
    LocationPtr locationPtr = BaseArea::convert< Location >( location_ptr );
    if( locationPtr )
    {
        MarkPtrs marks_of_location = locationPtr->getMarks();
        for( MarkPtr mark: marks_of_location )
            if(mark->getMarkType() == Mark::MT_DEFECT)
            {
                BaseMetadataPtr status = mark->getMetadata("status");
                if(status)
                {
                    QString statusStr = status->getValueAsString();
                    if(statusStr == QString::fromUtf8("новый"))
                        ++location_tasks_new;
                    else if(statusStr == QString::fromUtf8("в работе"))
                        ++location_tasks_in_work;
                    else if(statusStr == QString::fromUtf8("на проверку"))
                        ++location_tasks_for_check;
                }
            }
    }
    locationItem->setData((int)ColumnTitle::TASKS, TASKS_NEW, location_tasks_new);
    locationItem->setData((int)ColumnTitle::TASKS, TASKS_IN_WORK, location_tasks_in_work);
    locationItem->setData((int)ColumnTitle::TASKS, TASKS_FOR_CHECK, location_tasks_for_check);
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
            BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(id);
            if(ptr)
            {
                QTreeWidgetItem* roomItem = it.value();
                if(ptr->getCoords().isEmpty() == false)
                    for(int i(0); i < 8; ++i)
                        roomItem->setTextColor(i, _defaultColor);
            }
        }
    }
}

void ViraTreeWidget::slotResaclArea(const QModelIndex &index)
{
    QTreeWidgetItem * item = itemFromIndex(index);
    if( ! item) return;

    int itemType = item->data((int)ColumnTitle::NAME, TYPE).toInt();
    while(itemType != (int)ItemType::ItemTypeFacility)
    {
        item = item->parent();
        if( ! item)
            break;
        itemType = item->data((int)ColumnTitle::NAME, TYPE).toInt();
    }

    if(item)
        recalcAreaInFacility(item);
}

void ViraTreeWidget::slotSaveItemToDb(const QModelIndex &index)
{
//    qDebug() << "---> slotSaveItemToDb" << index.column() << index.row();
    QTreeWidgetItem * item = itemFromIndex(index);
    if( ! item) return;

    const qulonglong id(item->data((int)ColumnTitle::NAME, ID).toULongLong());
    BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(id);
    if( ! ptr) return;
    switch(item->data((int)ColumnTitle::NAME, TYPE).toInt())
    {
    case (int)ItemType::ItemTypeRoom :
    {
        if(item->text((int)ColumnTitle::SQUARE).isEmpty() == false)
            ptr->addMetadata("double", "area", item->text((int)ColumnTitle::SQUARE).toDouble());

        if(item->text((int)ColumnTitle::BASE_RENT).isEmpty() == false)
            ptr->addMetadata("double", "base_rent", item->text((int)ColumnTitle::BASE_RENT).toDouble());

        if(item->text((int)ColumnTitle::STATUS).isEmpty() == false)
            ptr->addMetadata("string", "status", item->text((int)ColumnTitle::STATUS));

        ptr->addMetadata("string", "rentor", item->text((int)ColumnTitle::RENTER));

        ptr->addMetadata("string", "comment", item->text((int)ColumnTitle::COMMENT));
    }break;
    case (int)ItemType::ItemTypeFloor : {
//        ptr->addMetadata("string", "comment", item->text(5));

    }break;
    case (int)ItemType::ItemTypeFacility : {
//        ptr->addMetadata("string", "comment", item->text(5));

    }break;
    case (int)ItemType::ItemTypeLocation : {
//        ptr->addMetadata("string", "comment", item->text(5));

    }break;
    case (int)ItemType::ItemTypeRegion : {
//        ptr->addMetadata("string", "comment", item->text(5));

    }break;

    default:
        return;
    };
    bool res = ptr->commit();
    qDebug() << "ID :" << id << ", commit:" << res;
//    qDebug() << ptr->getName() << "---------------";
//    MetadataByName map = ptr->getMetadataMap();
//    for(auto it=map.begin(); it != map.end(); ++it)
//    {
//        QString tag = it->first;
//        BaseMetadataPtr _ptr = it->second;
//        qDebug() << tag << _ptr->getValueAsVariant();
//    }
//    qDebug() << "===================================";
}

void ViraTreeWidget::slotObjectChanged(uint64_t id)
{
    MarkPtr markPtr = RegionBizManager::instance()->getMark(id);
    if(markPtr)
    {
        BaseAreaPtr parentPtr = RegionBizManager::instance()->getBaseArea(markPtr->getParentId());
        if(! parentPtr) return;

        QTreeWidgetItem * item(0);
        auto it = _items.find(parentPtr->getId());
        if(it != _items.end())
            item = it.value();
        else
            return;

        int itemType = item->data((int)ColumnTitle::NAME, TYPE).toInt();
        if(itemType == (int)ItemType::ItemTypeLocation)
        {
            recalcTasksInLocation(item);
        }
        else
        {
            while(itemType != (int)ItemType::ItemTypeFacility)
            {
                item = item->parent();
                if( ! item)
                    break;
                itemType = item->data((int)ColumnTitle::NAME, TYPE).toInt();
            }
            if(item)
                recalcTasksInFacility(item);
        }
    }
    else
    {
        BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(id);
        if(! ptr)
            return;

        auto it = _items.find(id);
        if(it == _items.end())
            return;

        QTreeWidgetItem * item = it.value();

        switch(ptr->getType())
        {
        case BaseArea::AT_REGION :
        case BaseArea::AT_LOCATION :
        case BaseArea::AT_FACILITY :
        case BaseArea::AT_FLOOR : {
            QString description = ptr->getDescription();
            QString name = ptr->getName();
            if(name.isEmpty())
                name = description;
            item->setText((int)ColumnTitle::NAME, name);
            if( ! description.isEmpty() )
                item->setToolTip( 0, description );
            item->setText((int)ColumnTitle::ID, QString::number(id));
        }break;

        case BaseArea::AT_ROOM : {
            item->setText((int)ColumnTitle::NAME, ptr->getName());
            if( !ptr->getDescription().isEmpty() )
                item->setToolTip( 0, ptr->getDescription() );
            item->setText((int)ColumnTitle::ID, QString::number(id));

            BaseMetadataPtr areaPtr = ptr->getMetadata("area");
            if(areaPtr)
            {
                double area = areaPtr->getValueAsVariant().toDouble();
                item->setText((int)ColumnTitle::SQUARE, QString::number(area, 'f', 1));
            }

            BaseMetadataPtr baseRentPtr = ptr->getMetadata("base_rent");
            if(baseRentPtr)
            {
                double baseRent = baseRentPtr->getValueAsVariant().toDouble();
                item->setText((int)ColumnTitle::BASE_RENT, QString::number(baseRent, 'f', 2));
            }

            BaseMetadataPtr rentorPtr = ptr->getMetadata("rentor");
            if(rentorPtr)
                item->setText((int)ColumnTitle::RENTER, rentorPtr->getValueAsVariant().toString());

            BaseMetadataPtr statusPtr = ptr->getMetadata("status");
            if(statusPtr)
            {
                QString status = statusPtr->getValueAsVariant().toString();
                if(status == QString::fromUtf8("Свободно"))
                    item->setText((int)ColumnTitle::STATUS, status);
                else if(status == QString::fromUtf8("В аренде"))
                    item->setText((int)ColumnTitle::STATUS, status);
                else
                    item->setText((int)ColumnTitle::STATUS, QString::fromUtf8("Недоступно"));
            }

            BaseMetadataPtr commentPtr = ptr->getMetadata("comment");
            if(commentPtr)
                item->setText((int)ColumnTitle::COMMENT, commentPtr->getValueAsVariant().toString());
        }break;
        }

        qDebug() << "slotObjectChanged, id :" << id << ", points :" << ptr->getCoords().size();
    }
}

void ViraTreeWidget::slotAddObject(uint64_t id)
{
    qDebug() << "ViraTreeWidget::slotAddObject, id:" << id;

    BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(id);
    if(! ptr)
        return;

    QTreeWidgetItem * parentItem(nullptr);
    uint64_t parentId = ptr->getParentId();
    auto it = _items.find(parentId);
    if(it != _items.end())
        parentItem = it.value();

    QString name;
    if( ! ptr->getName().isEmpty() )
        name = ptr->getName();
    else if( ! ptr->getDescription().isEmpty() )
        name = ptr->getDescription();

    ItemType itemType;
    switch(ptr->getType())
    {
    case BaseArea::AT_REGION : {
        if(name.isEmpty())
            name = QString::fromUtf8("регион");
        itemType = ItemType::ItemTypeRegion;
    }break;
    case BaseArea::AT_LOCATION : {
        if(name.isEmpty())
            name = QString::fromUtf8("локация");
        itemType = ItemType::ItemTypeLocation;
    }break;
    case BaseArea::AT_FACILITY : {
        if(name.isEmpty())
            name = QString::fromUtf8("здание");
        itemType = ItemType::ItemTypeFacility;
    }break;
    case BaseArea::AT_FLOOR : {
        if(name.isEmpty())
            name = QString::fromUtf8("этаж");
        itemType = ItemType::ItemTypeFloor;
    }break;
    case BaseArea::AT_ROOM : {
        if(name.isEmpty())
            name = QString::fromUtf8("комната");
        itemType = ItemType::ItemTypeRoom;
    }break;
    }

    QTreeWidgetItem * item = (parentItem ? new QTreeWidgetItem(parentItem) : new QTreeWidgetItem(this));
    const qulonglong _id(id);
    item->setText((int)ColumnTitle::NAME, name);
    item->setText((int)ColumnTitle::ID, QString::number(_id));
    for(int i(0); i<ColumnCount; ++i)
    {
        item->setData(i, ID, _id);
        item->setData(i, TYPE, (int)itemType);
        if(ptr->getType() != BaseArea::AT_FLOOR)
            item->setTextColor(i, _noCoordColor);
    }
    _items.insert(_id, item);

    clearSelection();
    RegionBizManager::instance()->setCurrentEntity(id);
    scrollToItem(item);
}

void ViraTreeWidget::slotDeleteObject(uint64_t id)
{
    qDebug() << "ViraTreeWidget::slotDeleteObject, id:" << id;

    auto it = _items.find(id);
    if(it != _items.end())
    {
        QTreeWidgetItem * item = it.value();
        _items.erase(it);

        QTreeWidgetItem * parentItem = item->parent();
        delete item;
        item = nullptr;
        if(parentItem)
        {
            int itemType = parentItem->data((int)ColumnTitle::NAME, TYPE).toInt();
            if(itemType == (int)ItemType::ItemTypeLocation)
            {
                recalcTasksInLocation(parentItem);
            }
            else
            {
                while(itemType != (int)ItemType::ItemTypeFacility)
                {
                    parentItem = parentItem->parent();
                    if( ! parentItem)
                        break;
                    itemType = parentItem->data((int)ColumnTitle::NAME, TYPE).toInt();
                }
                if(parentItem)
                    recalcTasksInFacility(parentItem);
            }
        }
    }
}

void ViraTreeWidget::slotItemSelectionChanged()
{
    qulonglong id(0);
    QList<QTreeWidgetItem*>list = selectedItems();
    if( ! list.isEmpty())
    {
        QTreeWidgetItem* item = list.first();
        id = item->data((int)ColumnTitle::NAME, ID).toULongLong();
    }
    RegionBizManager::instance()->setCurrentEntity(id);
    qDebug() << "slotItemSelectionChanged, ID :" << id;
}

void ViraTreeWidget::slotItemDoubleClicked(QTreeWidgetItem * item, int)
{
    if(item)
    {
        const qulonglong id(item->data((int)ColumnTitle::NAME, ID).toULongLong());
        RegionBizManager::instance()->centerOnEntity(id);
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

void ViraTreeWidget::mousePressEvent(QMouseEvent *event)
{
    QTreeWidget::mousePressEvent(event);

    bool layerEditMode(false);
    QVariant layerEditModeVar = CtrConfig::getValueByName("application_settings.editMode");
    if(layerEditModeVar.isValid())
        layerEditMode = layerEditModeVar.toBool();
    else
        CtrConfig::setValueByName("application_settings.editMode", false);

    if(event->buttons()&Qt::RightButton && layerEditMode)
    {
        uint64_t id = RegionBizManager::instance()->getCurrentEntity();
        BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(id);
        if(ptr)
        {
            QMenu menu(this);
            QAction * addRegionAction(nullptr);
            QAction * addSubAction(nullptr);
            QAction * delAction(nullptr);
            QString name = ptr->getName();
            if(name.isEmpty())
                name = ptr->getDescription();
            switch(ptr->getType())
            {
            case BaseArea::AT_REGION : {
                addRegionAction = menu.addAction(QString::fromUtf8("Добавить новый регион"));
                menu.addSeparator();
                addSubAction = menu.addAction(QString::fromUtf8("Добавить локацию в регион \"") + name + QString("\""));
                delAction = menu.addAction(QString::fromUtf8("Удалить регион \"") + name + QString("\""));
            }break;
            case BaseArea::AT_LOCATION : {
                addSubAction = menu.addAction(QString::fromUtf8("Добавить здание в локацию \"") + name + QString("\""));
                delAction = menu.addAction(QString::fromUtf8("Удалить локацию \"") + name + QString("\""));
            }break;
            case BaseArea::AT_FACILITY : {
                addSubAction = menu.addAction(QString::fromUtf8("Добавить этаж в здание \"") + name + QString("\""));
                delAction = menu.addAction(QString::fromUtf8("Удалить здание \"") + name + QString("\""));
            }break;
            case BaseArea::AT_FLOOR : {
                addSubAction = menu.addAction(QString::fromUtf8("Добавить комнату в этаж \"") + name + QString("\""));
                delAction = menu.addAction(QString::fromUtf8("Удалить этаж \"") + name + QString("\""));
            }break;
            case BaseArea::AT_ROOM : {
                delAction = menu.addAction(QString::fromUtf8("Удалить комнату \"") + name + QString("\""));
            }break;
            }
            QAction * res = menu.exec(QCursor::pos());
            if(res)
            {
                if(res == addRegionAction || res == addSubAction)
                {
                    BaseArea::AreaType type;
                    QString addWindowTitle;
                    if(res == addRegionAction)
                    {
                        type = BaseArea::AT_REGION;
                        addWindowTitle = QString::fromUtf8("Добавить регион");
                    }
                    else
                        switch(ptr->getType())
                        {
                        case BaseArea::AT_REGION :
                            type = BaseArea::AT_LOCATION;
                            addWindowTitle = QString::fromUtf8("Добавить локацию");
                            break;
                        case BaseArea::AT_LOCATION :
                            type = BaseArea::AT_FACILITY;
                            addWindowTitle = QString::fromUtf8("Добавить здание");
                            break;
                        case BaseArea::AT_FACILITY :
                            type = BaseArea::AT_FLOOR;
                            addWindowTitle = QString::fromUtf8("Добавить этаж");
                            break;
                        case BaseArea::AT_FLOOR :
                            type = BaseArea::AT_ROOM;
                            addWindowTitle = QString::fromUtf8("Добавить комнату");
                            break;
                        case BaseArea::AT_ROOM : return;
                        }

                    AddBaseAreaForm addBaseAreaForm(type, id);
                    ew::EmbeddedWidgetStruct struc;
                    ew::EmbeddedHeaderStruct headStr;
                    headStr.hasCloseButton = true;
                    headStr.windowTitle = addWindowTitle;
                    headStr.headerPixmap = QString(":/img/icon_edit_image.png");
                    struc.widgetTag = "AddBaseAreaForm";
                    struc.minSize = QSize(100,100);
                    struc.maxSize = QSize(500,500);
                    //struc.size = QSize(400,25);
                    struc.header = headStr;
                    struc.iface = &addBaseAreaForm;
                    struc.topOnHint = true;
                    struc.isModal = true;
                    ewApp()->createWidget(struc);

                    ewApp()->removeWidget(addBaseAreaForm.id());
                }
                else if(res == delAction)
                {
                    if(QMessageBox::Yes == QMessageBox::question(this, QString::fromUtf8("Внимание"), delAction->text() + QString(" ?"), QMessageBox::Yes, QMessageBox::No))
                    {
                        bool res = RegionBizManager::instance()->deleteArea(ptr);
                        qDebug() << "deleteArea, id:" << id << ", res:" << res;
                        clearSelection();
                    }
                }
            }
        }
    }
}




















