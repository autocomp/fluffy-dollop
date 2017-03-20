#include "virastatusbar.h"
#include "ui_virastatusbar.h"
#include <QDebug>
#include <regionbiz/rb_manager.h>
#include <libembeddedwidgets/embeddedstruct.h>
#include <libembeddedwidgets/embeddedapp.h>
#include <ctrcore/tempinputdata/tempdatatypies.h>
#include <ctrcore/tempinputdata/tempdatacontroller.h>

using namespace regionbiz;

ViraStatusBar::ViraStatusBar(quint64 parentWidgetId, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViraStatusBar),
    _parentWidgetId(parentWidgetId)
{
    ui->setupUi(this);
    connect(ui->moreInfo, SIGNAL(clicked(bool)), this, SLOT(slotShowMoreInfo(bool)));
    reset();

    auto mngr = RegionBizManager::instance();
    mngr->subscribeOnSelect(this, SLOT(slotObjectSelectionChanged(uint64_t,uint64_t)));
}

ViraStatusBar::~ViraStatusBar()
{
    delete ui;
}

void ViraStatusBar::slotObjectSelectionChanged(uint64_t prev_id, uint64_t curr_id)
{
    reset();
    BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(curr_id);
    if(! ptr)
        return;

    switch(ptr->getType())
    {
    case BaseArea::AT_REGION :
    {

    }break;
    case BaseArea::AT_LOCATION :
    {

    }break;
    case BaseArea::AT_FACILITY :
    {
        FacilityPtr facility = BaseArea::convert< Facility >(ptr);
        if(facility)
        {
            ui->tl1->setText(QString::fromUtf8("Здание : ") + facility->getDescription());
            temp_data::AreaInfo areaInfo;
            if(TempDataController::instance()->getAreaInfo(facility->getId(), areaInfo))
            {
                ui->tl2->setText(QString::fromUtf8("Площадь : в аренде - ") + QString::number(areaInfo.squareInTenant, 'f', 0) +
                                 QString::fromUtf8(", свободно - ")  + QString::number(areaInfo.squareFree, 'f', 0));

                ui->tl3->setText(QString::fromUtf8("Всего арендаторов : ") + QString::number(areaInfo.tenants.size()));

                if(areaInfo.debt > 0)
                    ui->tl4->setText(QString::fromUtf8("Задолженность : ") + QString::number(areaInfo.debt));
            }
        }
    }break;
    case BaseArea::AT_FLOOR :
    {
        FloorPtr floor = BaseArea::convert< Floor >(ptr);
        if(floor)
        {
            ui->tl1->setText(QString::fromUtf8("Этаж : ") + floor->getName());
            temp_data::AreaInfo areaInfo;
            if(TempDataController::instance()->getAreaInfo(floor->getId(), areaInfo))
            {
                ui->tl2->setText(QString::fromUtf8("Площадь : в аренде - ") + QString::number(areaInfo.squareInTenant, 'f', 0) +
                                 QString::fromUtf8(", свободно - ")  + QString::number(areaInfo.squareFree, 'f', 0));

                ui->tl3->setText(QString::fromUtf8("Всего арендаторов : ") + QString::number(areaInfo.tenants.size()));

                if(areaInfo.debt > 0)
                    ui->tl4->setText(QString::fromUtf8("Задолженность : ") + QString::number(areaInfo.debt));
            }
        }
    }break;
    case BaseArea::AT_ROOM :
    {
        RoomPtr room = BaseArea::convert< Room >(ptr);
        if(room)
        {
//            ui->moreInfo->setVisible(true);
            ui->tl1->setText(QString::fromUtf8("Номер : ") + room->getName());
            temp_data::RoomInfo roomInfo;
            if(TempDataController::instance()->getRoomInfo(room->getId(), roomInfo))
            {
                ui->tl2->setText(QString::fromUtf8("Площадь : ") + QString::number(roomInfo.square, 'f', 1));
                QString status = QString::fromUtf8("Статус : ");
                switch (roomInfo.state)
                {
                case temp_data::RoomState::Free : status.append(QString::fromUtf8("свободно")); break;
                case temp_data::RoomState::InTenant : status.append(QString::fromUtf8("в аренде")); break;
                case temp_data::RoomState::Reconstruction : status.append(QString::fromUtf8("ремонт")); break;
                case temp_data::RoomState::Sale : status.append(QString::fromUtf8("продажа")); break;
                }
                ui->tl3->setText(status);
                temp_data::Tenant tenant;
                if(TempDataController::instance()->getTenant(roomInfo.tenant, tenant))
                {
                    ui->tl4->setText(QString::fromUtf8("Арендатор : ") + tenant.name);
                }
            }
        }
    }break;
    }
}

void ViraStatusBar::reset()
{
//    ui->moreInfo->setVisible(false);
    ui->tl1->clear();
    ui->tl2->clear();
    ui->tl3->clear();
    ui->tl4->clear();
}

void ViraStatusBar::slotShowMoreInfo(bool on_off)
{
    if( ! _iface)
    {
        QLabel * testMoreInfoWidget = new QLabel("Hellow! I am more info widget :)");
        _iface = new EmbIFaceNotifier(testMoreInfoWidget);
        QString tag = QString("ViraStatusBar_MoreInfoWidget");
        quint64 widgetId = ewApp()->restoreWindow(tag, _iface);
        if(0 == widgetId)
        {
            ew::EmbeddedWindowStruct struc;
            ew::EmbeddedHeaderStruct headStr;
            headStr.hasCloseButton = true;
            headStr.hasMinMaxButton = false;
            headStr.hasCollapseButton = false;
            //headStr.headerPixmap = ":/img/061_icons_32_tools_pdf.png";
            headStr.windowTitle = QString::fromUtf8("More info");
            struc.header = headStr;
            struc.iface = _iface;
            struc.widgetTag = tag;
            struc.minSize = QSize(300,300);
            struc.topOnHint = true;
            ewApp()->createWindow(struc, _parentWidgetId);
        }
        ewApp()->setVisible(_iface->id(), false);
        connect(_iface, SIGNAL(signalClosed()), this, SLOT(slotMoreInfoWidgetClosed()));
    }

    ewApp()->setVisible(_iface->id(), on_off);
}

void ViraStatusBar::slotMoreInfoWidgetClosed()
{
    disconnect(ui->moreInfo, SIGNAL(clicked(bool)), this, SLOT(slotShowMoreInfo(bool)));
    ui->moreInfo->setChecked(false);
    connect(ui->moreInfo, SIGNAL(clicked(bool)), this, SLOT(slotShowMoreInfo(bool)));
}



















