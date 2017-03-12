#include "virastatusbar.h"
#include "ui_virastatusbar.h"
#include <QDebug>
#include <regionbiz/rb_manager.h>
#include <libembeddedwidgets/embeddedstruct.h>
#include <libembeddedwidgets/embeddedapp.h>

using namespace regionbiz;

ViraStatusBar::ViraStatusBar(quint64 parentWidgetId, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViraStatusBar)
{
    ui->setupUi(this);
    connect(ui->moreInfo, SIGNAL(clicked(bool)), this, SLOT(slotShowMoreInfo(bool)));
    reset();

    auto mngr = RegionBizManager::instance();
    mngr->subscribeOnSelect(this, SLOT(slotObjectSelectionChanged(uint64_t,uint64_t)));

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
        ewApp()->createWindow(struc); //, parentWidgetId);
    }
    ewApp()->setVisible(_iface->id(), false);
    connect(_iface, SIGNAL(signalClosed()), this, SLOT(slotMoreInfoWidgetClosed()));
}

ViraStatusBar::~ViraStatusBar()
{
    delete ui;
}

void ViraStatusBar::slotObjectSelectionChanged(uint64_t prev_id, uint64_t curr_id)
{
    BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(curr_id);
    if(ptr)
    {
        ui->number->setText(QString("DB ID : ") + QString::number(ptr->getId()) );
        ui->square->setText("2323");
        ui->status->setText("Free");
        ui->owner->setText("Ivanoff");
    }
    else
        reset();
}

void ViraStatusBar::reset()
{
    ui->number->clear();
    ui->square->clear();
    ui->status->clear();
    ui->owner->clear();
}

void ViraStatusBar::slotShowMoreInfo(bool on_off)
{
    ewApp()->setVisible(_iface->id(), on_off);
}

void ViraStatusBar::slotMoreInfoWidgetClosed()
{
    disconnect(ui->moreInfo, SIGNAL(clicked(bool)), this, SLOT(slotShowMoreInfo(bool)));
    ui->moreInfo->setChecked(false);
    connect(ui->moreInfo, SIGNAL(clicked(bool)), this, SLOT(slotShowMoreInfo(bool)));
}



















