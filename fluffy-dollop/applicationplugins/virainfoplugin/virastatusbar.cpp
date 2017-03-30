#include "virastatusbar.h"
#include "ui_virastatusbar.h"
#include "markform.h"

#include <QDebug>

#include <libembeddedwidgets/embeddedstruct.h>
#include <libembeddedwidgets/embeddedapp.h>
#include <ctrcore/tempinputdata/tempdatatypies.h>
#include <ctrcore/tempinputdata/tempdatacontroller.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>
#include "virainfowidget.h"

using namespace regionbiz;

ViraStatusBar::ViraStatusBar( quint64 parentWidgetId, QWidget *parent ):
    QWidget(parent),
    ui(new Ui::ViraStatusBar),
    _parentWidgetId(parentWidgetId)
{
    ui->setupUi(this);
    ui->editObject->setIcon(QIcon(":/img/edit_mode"));
    ui->moreInfo->setIcon(QIcon(":/img/info_button"));
    ui->addMark->setIcon(QIcon(":/img/mark_button"));

    connect(ui->moreInfo, SIGNAL(clicked(bool)), this, SLOT(slotShowMoreInfo(bool)));
    connect(ui->editObject, SIGNAL(clicked(bool)), this, SLOT(slotEditAreaGeometry(bool)));
    connect(ui->addMark, SIGNAL(clicked(bool)), this, SLOT(slotAddMark(bool)));
    reset();

    auto mngr = RegionBizManager::instance();
    mngr->subscribeOnSelect(this, SLOT(slotObjectSelectionChanged(uint64_t,uint64_t)));
    mngr->subscribeCenterOn(this, SLOT(slotObjectCenterOn(uint64_t)));

    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::EditModeFinish, this, SLOT(slotEditObjectGeometryFinish(QVariant)),
                                      qMetaTypeId< quint64 >(),
                                      QString("visualize_system") );
}

ViraStatusBar::~ViraStatusBar()
{
    delete ui;
}

void ViraStatusBar::slotObjectCenterOn(uint64_t id)
{
    MarkPtr ptr = RegionBizManager::instance()->getMark(id);
    if(ptr)
    {
        showMarkInfoWidgwt(false, ptr->getId());
    }
}

void ViraStatusBar::slotObjectSelectionChanged( uint64_t /*prev_id*/, uint64_t curr_id )
{
    reset();

    if(curr_id == 0)
        return;

    MarkPtr markPtr = RegionBizManager::instance()->getMark(curr_id);
    if(markPtr)
    {
        return;
    }

    BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(curr_id);
    if(! ptr)
        return;

    auto data = DataGetter::getData( curr_id );

    switch( ptr->getType() ) {
    case BaseArea::AT_FLOOR:
    case BaseArea::AT_REGION:
    case BaseArea::AT_LOCATION:
    case BaseArea::AT_FACILITY:
    {
        showAddres( ptr );
        showTasks( data );
        showArendators( data, false );
        showAreas( data, false );
        //showDebt( data );

        ui->moreInfo->show();
        break;
    }
    case BaseArea::AT_ROOMS_GROUP:
    case BaseArea::AT_ROOM:
    {
        showName( ptr );
        showAreas( data, true );
        showArendators( data, true );
        showTasks( data );

        ui->editObject->show();
        ui->moreInfo->show();
        ui->addMark->show();
        ui->editObject->setToolTip(QString::fromUtf8("Редактировать контуры комнаты"));
        break;
    }
    }
}

void ViraStatusBar::slotAddMark(bool on_off)
{
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::BlockGUI, QVariant(on_off), QString("visualize_system"));

    quint64 id(on_off ? RegionBizManager::instance()->getSelectedArea() : 0);
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::SetMarkPosition, QVariant(id), QString("visualize_system"));

    if(on_off && ui->moreInfo->isChecked())
    {
        ui->moreInfo->setChecked(false);
        slotShowMoreInfo(false);
    }
    ui->moreInfo->setDisabled(on_off);
    ui->editObject->setDisabled(on_off);
}

void ViraStatusBar::slotEditAreaGeometry(bool on_off)
{
    quint64 id(on_off ? RegionBizManager::instance()->getSelectedArea() : 0);

    CommonMessageNotifier::send( (uint)visualize_system::BusTags::BlockGUI, ((bool)id), QString("visualize_system"));

    CommonMessageNotifier::send( (uint)visualize_system::BusTags::EditAreaGeometry, QVariant(id), QString("visualize_system"));

    if(on_off && ui->moreInfo->isChecked())
    {
        ui->moreInfo->setChecked(false);
        slotShowMoreInfo(false);
    }
    ui->moreInfo->setDisabled(on_off);
//    ui->addMark->setDisabled(on_off);
}

void ViraStatusBar::slotEditObjectGeometryFinish(QVariant var)
{
    quint64 id = var.toUInt();
    if(id > 0)
    {
        if(ui->editObject->isChecked())
        {
            ui->editObject->blockSignals(true);
            ui->editObject->setChecked(false);
            ui->editObject->blockSignals(false);
        }
        if(ui->addMark->isChecked())
        {
            ui->addMark->blockSignals(true);
            ui->addMark->setChecked(false);
            ui->addMark->blockSignals(false);

            showMarkInfoWidgwt(true, id);
        }

        CommonMessageNotifier::send( (uint)visualize_system::BusTags::BlockGUI, QVariant(false), QString("visualize_system"));
        RegionBizManager::instance()->selectArea(id);
    }
}

void ViraStatusBar::reset()
{
    while ( ui->horizontalLayout_widgets->count() )
    {
        auto item = ui->horizontalLayout_widgets->itemAt(0);
        ui->horizontalLayout_widgets->removeItem( item );
        if ( item->widget() )
            delete item->widget();
        delete item;
    }
    ui->name->clear();
    ui->moreInfo->hide();
    ui->addMark->hide();
    ui->editObject->hide();
}

void ViraStatusBar::showTasks( AreaData data )
{
    QLabel* work_task = getTask( TT_WORK, data );
    work_task->setToolTip(QString::fromUtf8("Задачи в работе"));
    QLabel* check_task = getTask( TT_CHECK, data );
    check_task->setToolTip(QString::fromUtf8("Задачи на проверку"));
    QLabel* new_task = getTask( TT_NEW, data );
    new_task->setToolTip(QString::fromUtf8("Новые задачи"));

    ui->horizontalLayout_widgets->addWidget( new_task );
    ui->horizontalLayout_widgets->addWidget( work_task );
    ui->horizontalLayout_widgets->addWidget( check_task );
    ui->horizontalLayout_widgets->addStretch();
}

void ViraStatusBar::showArendators( AreaData data, bool one )
{
    if( !one )
    {
        QLabel* arend_good = getArendator( AT_GOOD );
        arend_good->setToolTip( QString::fromUtf8( "Арендаторы" ));
        QLabel* arend_good_count = new QLabel( QString::number( data.arendators_good ));
        arend_good_count->setToolTip( QString::fromUtf8( "Арендаторы" ));
        QLabel* arend_bad = getArendator( AT_BAD );
        arend_bad->setToolTip( QString::fromUtf8( "Должники" ));
        QLabel* arend_bad_count = new QLabel( QString::number(  data.arendators_bad ));
        arend_bad_count->setToolTip( QString::fromUtf8( "Должники" ));

        ui->horizontalLayout_widgets->addWidget( arend_good );
        ui->horizontalLayout_widgets->addWidget( arend_good_count );
        ui->horizontalLayout_widgets->addWidget( arend_bad );
        ui->horizontalLayout_widgets->addWidget( arend_bad_count );
    }
    else
    {
        QString arend_name = "";
        QLabel* man;
        if( data.arendators_bad )
        {
            arend_name = ( data.arendators_bad_set.isEmpty() ?
                               "" : *( data.arendators_bad_set.begin() ));
            man = getArendator( AT_BAD );
        }
        else
        {
            arend_name = ( data.arendators_good_set.isEmpty() ?
                               "" : *( data.arendators_good_set.begin() ));
            man = getArendator( AT_GOOD );
        }
        man->setToolTip( QString::fromUtf8( "Арендатор" ));

        QLabel* arend = new QLabel( arend_name );
        arend->setToolTip( QString::fromUtf8( "Арендатор" ));
        if( !arend_name.isEmpty() )
            ui->horizontalLayout_widgets->addWidget( man );
        ui->horizontalLayout_widgets->addWidget( arend );
    }

    ui->horizontalLayout_widgets->addStretch();
}

void ViraStatusBar::showAreas( AreaData data, bool one )
{
    if( !one )
    {
        QLabel* area_rent = getArea( AT_RENT, data );
        area_rent->setToolTip( QString::fromUtf8( "Площадь в аренде" ));
        QLabel* area_free = getArea( AT_FREE, data );
        area_free->setToolTip( QString::fromUtf8( "Свободная площадь" ));

        ui->horizontalLayout_widgets->addWidget( area_rent );
        ui->horizontalLayout_widgets->addWidget( area_free );
    }
    else
    {
        QString status;
        QLabel* area;
        if( data.area_free > 1e-5 )
        {
            area = getArea( AT_FREE, data );
            status = QString::fromUtf8( "Свободно" );
        }
        else if( data.area_rent > 1e-5 )
        {
            area = getArea( AT_RENT, data );
            status = QString::fromUtf8( "В аренде" );
        }
        else
        {
            area = getArea( AT_PROP, data );
            status = QString::fromUtf8( "Собственность" );
        }
        area->setToolTip( QString::fromUtf8( "Площадь" ));

        QLabel* status_lbl = new QLabel( status );
        status_lbl->setToolTip( QString::fromUtf8( "Статус" ));
        status_lbl->setStyleSheet( area->styleSheet() );

        ui->horizontalLayout_widgets->addWidget( area );
        ui->horizontalLayout_widgets->addWidget( status_lbl );
    }
    ui->horizontalLayout_widgets->addStretch();
}

void ViraStatusBar::showAddres(BaseAreaPtr ptr)
{
    QString addr = "";

    switch( ptr->getType() )
    {
    case BaseArea::AT_REGION:
        addr = BaseArea::convert< Region >( ptr )->getDescription();
        break;
    case BaseArea::AT_LOCATION:
        addr = BaseArea::convert< Location >( ptr )->getAddress();
        break;
    case BaseArea::AT_FACILITY:
        addr = BaseArea::convert< Facility >( ptr )->getAddress();
        break;
    case BaseArea::AT_FLOOR:
        addr = BaseArea::convert< Facility >( ptr->getParent() )->getAddress();
        break;
    }

    ui->name->setText( addr );
    ui->name->setToolTip( QString::fromUtf8( "Адрес" ));
}

void ViraStatusBar::showName(BaseAreaPtr ptr)
{
    QString name = "";

    switch( ptr->getType() )
    {
    case BaseArea::AT_ROOMS_GROUP:
        name = BaseArea::convert< RoomsGroup >( ptr )->getCadastralNumber();
        break;
    case BaseArea::AT_ROOM:
        name = BaseArea::convert< Room >( ptr )->getName();
        break;
    }

    ui->name->setText( name );
    ui->name->setToolTip( QString::fromUtf8( "Название" ));
}

void ViraStatusBar::showDebt(AreaData data)
{
    QLabel* debt = new QLabel;
    debt->setStyleSheet( "background-color: #ffaaaa;"
                         "border-color: rgb(0, 0, 0);"
                         "border-radius: 5px;"
                         "color: rgb(0, 0, 0);"
                         "border-width: 1px;"
                         "border-style: solid;"
                         );
    debt->setText( QString::number( (int) data.debt ));

    ui->horizontalLayout_widgets->addWidget( debt );
    ui->horizontalLayout_widgets->addStretch();
}

QLabel *ViraStatusBar::getArendator(ArendatorType type )
{
    QString color;
    switch( type )
    {
    case AT_GOOD:
    {
        color = "#3CE63C";
        break;
    }
    case AT_BAD:
    {
        color = "#FF6347";
        break;
    }
    }

    QLabel* arend = new QLabel;
    arend->setStyleSheet("background-color: " + color + ";"
                         "border-color: rgb(255, 255, 255);"
                         "color: rgb(0, 0, 0);"
                         "border-radius: 5px;" );

    QPixmap px(":/img/standing_man.png");
    arend->setPixmap( px.scaled( 32, 32 ));
    arend->resize( 32, 32 );
    return arend;
}

QLabel *ViraStatusBar::getTask(ViraStatusBar::TaskType type, AreaData data)
{
    QString color;
    int tasks;
    switch( type )
    {
    case TT_CHECK:
    {
        tasks = data.task_check;
        color = "#3CE63C";
        break;
    }
    case TT_WORK:
    {
        tasks = data.task_work;
        color = "#F0E68C";
        break;
    }
    case TT_NEW:
    {
        tasks = data.task_new;
        color = "#FF8C00";
        break;
    }
    }

    QLabel* task = new QLabel;
    task->setStyleSheet( "background-color: " + color + ";"
                         "border-color: rgb(255, 255, 255);"
                         "border-radius: 10px;"
                         "color: rgb(0, 0, 0);"
                         "border-width: 2px;"
                         "border-style: solid;"
                         );
    task->setText( QString::number( tasks ));
    return task;
}

QLabel *ViraStatusBar::getArea(ViraStatusBar::AreaType type, AreaData data)
{
    QString color;
    int area_cnt;
    switch( type )
    {
    case AT_RENT:
    {
        area_cnt = data.area_rent;
        color = "#F0E68C";
        break;
    }
    case AT_FREE:
    {
        area_cnt = data.area_free;
        color = "#3CE63C";
        break;
    }
    case AT_PROP:
    {
        area_cnt = data.area_property;
        color = "#6495ED";
        break;
    }
    }

    QLabel* area = new QLabel;
    area->setStyleSheet( "background-color: " + color + ";"
                         "border-color: rgb(255, 255, 255);"
                         //"border-radius: 15px;"
                         "color: rgb(0, 0, 0);"
                         "border-width: 2px;"
                         "border-style: solid;"
                         );
    area->setText( QString::number( area_cnt ));
    return area;
}

void ViraStatusBar::slotShowMoreInfo(bool on_off)
{
    quint64 id = RegionBizManager::instance()->getSelectedArea();
    MarkPtr markPtr = RegionBizManager::instance()->getMark(id);
    if(markPtr)
    {
        disconnect(ui->moreInfo, SIGNAL(clicked(bool)), this, SLOT(slotShowMoreInfo(bool)));
        ui->moreInfo->setChecked(false);
        connect(ui->moreInfo, SIGNAL(clicked(bool)), this, SLOT(slotShowMoreInfo(bool)));

        showMarkInfoWidgwt(false, id);

        return;
    }

    if( !_ifaceInfoAreaWidget )
    {
        auto moreInfoWidget = new ViraInfoWidget();
        _ifaceInfoAreaWidget = new EmbIFaceNotifier( moreInfoWidget );
        QString tag = QString("ViraStatusBar_MoreInfoWidget");
        quint64 widgetId = ewApp()->restoreWidget(tag, _ifaceInfoAreaWidget);
        if(0 == widgetId)
        {
            ew::EmbeddedWidgetStruct struc;
            ew::EmbeddedHeaderStruct headStr;
            headStr.hasCloseButton = true;
            headStr.hasMinMaxButton = false;
            headStr.hasCollapseButton = false;
            headStr.headerPixmap = ":/img/info_button.png";
            headStr.windowTitle = QString::fromUtf8("Информация");
            struc.header = headStr;
            struc.iface = _ifaceInfoAreaWidget;
            struc.widgetTag = tag;
            struc.minSize = QSize(300,300);
            struc.topOnHint = true;
            ewApp()->createWidget(struc, _parentWidgetId);
        }
        ewApp()->setVisible(_ifaceInfoAreaWidget->id(), false);
        connect(_ifaceInfoAreaWidget, SIGNAL(signalClosed()), this, SLOT(slotMoreInfoWidgetClosed()));
    }

    ewApp()->setVisible(_ifaceInfoAreaWidget->id(), on_off);
}

void ViraStatusBar::slotMoreInfoWidgetClosed()
{
    disconnect(ui->moreInfo, SIGNAL(clicked(bool)), this, SLOT(slotShowMoreInfo(bool)));
    ui->moreInfo->setChecked(false);
    connect(ui->moreInfo, SIGNAL(clicked(bool)), this, SLOT(slotShowMoreInfo(bool)));
}

void ViraStatusBar::slotMarkWidgetClosed()
{
    qDebug() << "slotMarkWidgetClosed";
}

void ViraStatusBar::slotCloseMarkWindow()
{
    ewApp()->setVisible(_ifaceInfoMarkWidget->id(), false);
}

void ViraStatusBar::showMarkInfoWidgwt(bool isEditMode, qulonglong id)
{
    if( !_ifaceInfoMarkWidget )
    {
        _markForm = new MarkForm;
        connect(_markForm, SIGNAL(signalCloseWindow()), this, SLOT(slotCloseMarkWindow()));
        connect(_markForm, SIGNAL(signalUpdateMark(quint64)), this, SLOT(slotUpdateMark(quint64)));
        _markForm->showWidget(id);

        _ifaceInfoMarkWidget = new EmbIFaceNotifier(_markForm);
        connect(_ifaceInfoMarkWidget, SIGNAL(signalClosed()), this, SLOT(slotMarkWidgetClosed()));
        QString tag = QString("ViraStatusBar_MarkInfoWidget");
        quint64 widgetId = ewApp()->restoreWidget(tag, _ifaceInfoMarkWidget);
        if(0 == widgetId)
        {
            ew::EmbeddedWidgetStruct struc;
            ew::EmbeddedHeaderStruct headStr;
            headStr.hasCloseButton = true;
            headStr.hasMinMaxButton = false;
            headStr.hasCollapseButton = false;
            headStr.headerPixmap = ":/img/mark_button.png";
            headStr.windowTitle = QString::fromUtf8("Информация о дефекте");
            struc.header = headStr;
            struc.iface = _ifaceInfoMarkWidget;
            struc.widgetTag = tag;
            struc.minSize = QSize(300,300);
            struc.topOnHint = true;
            struc.isModal = true;
            ewApp()->createWidget(struc, _parentWidgetId);
        }
    }
    else
    {
        _markForm->showWidget(id);
        ewApp()->setVisible(_ifaceInfoMarkWidget->id(), true);
    }
}

void ViraStatusBar::slotUpdateMark(quint64 id)
{
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::UpdateMark, QVariant(id), QString("visualize_system"));
}
















