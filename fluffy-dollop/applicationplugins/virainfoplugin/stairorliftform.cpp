#include "stairorliftform.h"
#include "ui_stairorliftform.h"

using namespace regionbiz;

StairOrLiftForm::StairOrLiftForm()
    :  ui(new Ui::StairOrLiftForm)
{
    ui->setupUi(this);

    ui->okPB->setIcon(QIcon(":/img/ok_button.png"));
    connect(ui->okPB, SIGNAL(clicked()), this, SLOT(slotApply()));

    ui->cancelPB->setIcon(QIcon(":/img/close_button.png"));
    connect(ui->cancelPB, SIGNAL(clicked()), this, SLOT(slotCloseWindow()));
}

StairOrLiftForm::~StairOrLiftForm()
{
    delete ui;
}

void StairOrLiftForm::fillFloorsList(regionbiz::FacilityPtr facilityPtr)
{
    ui->floorsTreeView->clear();

    if( ! facilityPtr)
        return;

    QMap<uint16_t, FloorTreeWidgetItem *> floorsMap;

    FloorPtrs floors = facilityPtr->getChilds();
    for( FloorPtr floorPtr: floors )
    {
        FloorTreeWidgetItem * item = new FloorTreeWidgetItem(floorPtr->getName(), floorPtr->getId(), floorPtr->getNumber());
        floorsMap.insert(floorPtr->getNumber(), item);
    }

    for(auto it = floorsMap.begin(); it != floorsMap.end(); ++it)
    {
        FloorTreeWidgetItem * item = it.value();
        ui->floorsTreeView->addTopLevelItem(item);
    }
}

void StairOrLiftForm::createEntity(uint64_t facilityId, regionbiz::Mark::MarkType type, QPointF pos)
{
    _facilityId = facilityId;
    _type = type;
    _pos = pos;

    FacilityPtr facilityPtr = BaseArea::convert< Facility >(RegionBizManager::instance()->getBaseArea(facilityId));
    fillFloorsList(facilityPtr);
}

void StairOrLiftForm::editEntity(uint64_t entityId, uint64_t facilityId)
{
    _facilityId = facilityId;
    FacilityPtr facilityPtr = BaseArea::convert< Facility >(RegionBizManager::instance()->getBaseArea(facilityId));
    fillFloorsList(facilityPtr);
}

void StairOrLiftForm::slotApply()
{
    if(_type == Mark::MT_INVALID) // редактирование
    {


    }
    else // создание
    {
//        FacilityPtr facilityPtr = BaseArea::convert< Facility >(RegionBizManager::instance()->getBaseArea(_facilityId));
//        if(facilityPtr)
//        {
//            MarkPtr markPtr = facilityPtr->addMark(_type, _pos);
//            for(int i(0); i < ui->floorsTreeView->topLevelItemCount(); ++i)
//            {
//                FloorTreeWidgetItem * item = dynamic_cast<FloorTreeWidgetItem*>(ui->floorsTreeView->topLevelItem(i));
//                if(item)
//                    if(item->checkState(0) == Qt::Checked)
//                    {
//                        LiftMarkPtr liftMarkPtr = markPtr->convert< LiftMark >(markPtr);
//                        if(liftMarkPtr)
//                            liftMarkPtr->addFloorNumber(item->_number);

//                        StairsMarkPtr stairsMarkPtr = markPtr->convert< StairsMark >(markPtr);
//                        if(stairsMarkPtr)
//                            stairsMarkPtr->addFloorNumber(item->_number);
//                    }
//            }

//            markPtr->commit();
//            facilityPtr->commit();
//        }
    }

    emit signalCloseWindow();
}

void StairOrLiftForm::slotCloseWindow()
{
    emit signalCloseWindow();
}



















