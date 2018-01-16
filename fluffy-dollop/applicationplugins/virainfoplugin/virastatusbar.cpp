#include "virastatusbar.h"
#include "ui_virastatusbar.h"
#include "markform.h"
#include "infoform.h"
#include "imageviewer.h"
#include "photo360form.h"
#include "stairorliftform.h"

#include <QDebug>

#include <dmanager/embeddedstruct.h>
#include <dmanager/embeddedapp.h>
#include <ctrcore/tempinputdata/tempdatatypies.h>
#include <ctrcore/tempinputdata/tempdatacontroller.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>
#include <ctrcore/ctrcore/ctrconfig.h>
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
    ui->addDefect->setIcon(QIcon(":/img/mark_button"));
    ui->deleteObject->setIcon(QIcon(":/img/garbg"));
    ui->addFoto->setIcon(QIcon(":/img/foto_button"));
    ui->addFoto360->setIcon(QIcon(":/img/foto360_button"));
    ui->addStairs->setIcon(QIcon(":/img/stairs_button"));
    ui->addLift->setIcon(QIcon(":/img/lift_button"));
    ui->addPlaceholder->setIcon(QIcon(":/img/placeholder_button"));

    connect(ui->moreInfo, SIGNAL(clicked()), this, SLOT(slotShowMoreInfo()));
    connect(ui->editObject, SIGNAL(clicked(bool)), this, SLOT(slotEditAreaGeometry(bool)));
    connect(ui->addDefect, SIGNAL(clicked(bool)), this, SLOT(slotAddDefect(bool)));
    connect(ui->addFoto, SIGNAL(clicked(bool)), this, SLOT(slotAddFoto(bool)));
    connect(ui->addFoto360, SIGNAL(clicked(bool)), this, SLOT(slotAddFoto360(bool)));
    connect(ui->addPlaceholder, SIGNAL(clicked(bool)), this, SLOT(slotAddPlaceholder(bool)));
    connect(ui->addStairs, SIGNAL(clicked(bool)), this, SLOT(slotAddStairs(bool)));
    connect(ui->addLift, SIGNAL(clicked(bool)), this, SLOT(slotAddLift(bool)));
    connect(ui->deleteObject, SIGNAL(clicked()), this, SLOT(slotDeleteObject()));
    reset();

    auto mngr = RegionBizManager::instance();
    mngr->subscribeOnCurrentChange(this, SLOT(slotObjectSelectionChanged(uint64_t,uint64_t)));
    mngr->subscribeCenterOn(this, SLOT(slotObjectCenterOn(uint64_t)));

    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::EditModeFinish, this, SLOT(slotEditObjectGeometryFinish(QVariant)),
                                      qMetaTypeId< quint64 >(),
                                      QString("visualize_system") );

    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::MarkCreated,
                                      this, SLOT( slotMarkCreated( QVariant )),
                                      qMetaTypeId< QVariantList >(),
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
        showMarkInfoWidgwt(ptr->getId());
    }
}

void ViraStatusBar::slotObjectSelectionChanged( uint64_t /*prev_id*/, uint64_t curr_id )
{
    reset();

    // WARNING check id for 0. MATSUBADZU
    if( curr_id == 0 )
        return;

    // get entity
    BaseEntityPtr entity = RegionBizManager::instance()->getBaseEntity(curr_id);
    if( !entity )
        return;

    switch( entity->getEntityType() )
    {
    // process marks
    case BaseEntity::ET_MARK:
    {
        MarkPtr markPtr = entity->convert< Mark >();
        if( !markPtr )
            return;

        ui->deleteObject->show();

        showMarkName( markPtr->getName() );

        MetadataByName values = markPtr->getMetadataMap();
        if( markPtr->isMetadataPresent( "status" ))
            showMarkStatus( values.at("status")->getValueAsVariant().toString() );
        if( markPtr->isMetadataPresent( "priority" ))
            showMarkPriority( values.at("priority")->getValueAsVariant().toString() );
        if( markPtr->isMetadataPresent( "worker" ))
            showMarkWorker( values.at("worker")->getValueAsVariant().toString() );
        if( markPtr->isMetadataPresent( "category" ))
            showMarkCategory( values.at("category")->getValueAsVariant().toString() );

        break;
    }

    // process areas
    case BaseEntity::ET_AREA:
    {
        BaseAreaPtr ptr = entity->convert< BaseArea >();
        if(! ptr)
            return;

        auto data = DataGetter::getData( curr_id );

        switch( ptr->getType() ) {
        case BaseArea::AT_FLOOR:
//            ui->addStairs->show();
//            ui->addLift->show();
            //! without break !!!
        case BaseArea::AT_LOCATION:
        case BaseArea::AT_FACILITY:
            ui->addFoto->show();
            ui->addFoto360->show();
            ui->addDefect->show();
            ui->addPlaceholder->show();
            //! without break !!!
        case BaseArea::AT_REGION:
        {
            showAddres( ptr );
            showTasks( data );
            // WARNING don't show arenda's attrs
            //showArendators( data, false );
            // WARNING false -> true to show only one area
            showAreas( data, true );
            //showDebt( data );

            ui->moreInfo->show();
            break;
        }

        case BaseArea::AT_ROOM:
        {
            showName( ptr );
            showAreas( data, true );
            showArendators( data, true );
            showTasks( data );

            ui->addPlaceholder->show();
            ui->moreInfo->show();
            ui->addDefect->show();
            ui->addFoto->show();
            ui->addFoto360->show();
            ui->editObject->show();
            ui->editObject->setDisabled(false);
            ui->editObject->setToolTip(QString::fromUtf8("Редактировать контуры комнаты"));
            break;
        }
        }

        switch( ptr->getType() ) {
        case BaseArea::AT_REGION:
            ui->editObject->show();
            ui->editObject->setDisabled(false);
            ui->editObject->setToolTip(QString::fromUtf8("Редактировать контуры региона"));
            break;
        case BaseArea::AT_LOCATION: {
            ui->editObject->setToolTip(QString::fromUtf8("Редактировать контуры локации"));
            ui->editObject->show();
            ui->editObject->setDisabled(true);
            if(ptr->getCoords().isEmpty() == false)
                ui->editObject->setDisabled(false);
            else if(ptr->getParent())
            {
                if(ptr->getParent()->getCoords().isEmpty() == false)
                    ui->editObject->setDisabled(false);
            }
        }break;
        case BaseArea::AT_FACILITY: {
            ui->editObject->setToolTip(QString::fromUtf8("Редактировать контуры здания"));
            ui->editObject->show();
            ui->editObject->setDisabled(true);
            if(ptr->getCoords().isEmpty() == false)
                ui->editObject->setDisabled(false);
            else if(ptr->getParent())
            {
                if(ptr->getParent()->getCoords().isEmpty() == false)
                    ui->editObject->setDisabled(false);
            }
        }break;
        }
    }

    }
}

void ViraStatusBar::slotDeleteObject()
{
    uint64_t id = RegionBizManager::instance()->getCurrentEntity();
    if(id > 0)
    {
        if(QMessageBox::Ok == QMessageBox::warning(this, QString::fromUtf8("Внимание"), QString::fromUtf8("Вы хотите удалить метку ?")))
        {
            RegionBizManager::instance()->deleteMark(id);
            reset();

            QVariant regionBizInitJson_Path = CtrConfig::getValueByName("application_settings.regionBizFilesPath");
            if(regionBizInitJson_Path.isValid())
            {
                QString destPath = regionBizInitJson_Path.toString();
                QDir dir(destPath);

                destPath = destPath + QDir::separator() + QString::number(id);
                QDir subdir(destPath);
                QStringList fileList = subdir.entryList(QDir::Files);
                foreach(QString fileName, fileList)
                    QFile::remove(destPath + QDir::separator() + fileName);

                dir.rmdir(QString::number(id));
            }
        }
    }
}

void ViraStatusBar::slotAddDefect(bool on_off)
{
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::BlockGUI, QVariant(on_off), QString("visualize_system"));

    quint64 type(on_off ? Mark::MT_DEFECT : Mark::MT_INVALID);
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::SetMarkPosition, QVariant(type), QString("visualize_system"));

    ui->moreInfo->setDisabled(on_off);
    ui->editObject->setDisabled(on_off);
}

void ViraStatusBar::slotAddFoto(bool on_off)
{
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::BlockGUI, QVariant(on_off), QString("visualize_system"));

    quint64 type(on_off ? Mark::MT_PHOTO : Mark::MT_INVALID);
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::SetMarkPosition, QVariant(type), QString("visualize_system"));

    ui->moreInfo->setDisabled(on_off);
    ui->editObject->setDisabled(on_off);
}

void ViraStatusBar::slotAddFoto360(bool on_off)
{
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::BlockGUI, QVariant(on_off), QString("visualize_system"));

    quint64 type(on_off ? Mark::MT_PHOTO_3D : Mark::MT_INVALID);
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::SetMarkPosition, QVariant(type), QString("visualize_system"));

    ui->moreInfo->setDisabled(on_off);
    ui->editObject->setDisabled(on_off);
}

void ViraStatusBar::slotAddPlaceholder(bool on_off)
{
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::BlockGUI, QVariant(on_off), QString("visualize_system"));

    quint64 type(on_off ? Mark::MT_PLACEHOLDER : Mark::MT_INVALID);
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::SetMarkPosition, QVariant(type), QString("visualize_system"));

    ui->moreInfo->setDisabled(on_off);
    ui->editObject->setDisabled(on_off);
}

void ViraStatusBar::slotAddLift(bool on_off)
{
//    CommonMessageNotifier::send( (uint)visualize_system::BusTags::BlockGUI, QVariant(on_off), QString("visualize_system"));

//    quint64 type(on_off ? Mark::MT_LIFT : Mark::MT_INVALID);
//    CommonMessageNotifier::send( (uint)visualize_system::BusTags::SetMarkPosition, QVariant(type), QString("visualize_system"));

//    ui->moreInfo->setDisabled(on_off);
//    ui->editObject->setDisabled(on_off);
}

void ViraStatusBar::slotAddStairs(bool on_off)
{
//    CommonMessageNotifier::send( (uint)visualize_system::BusTags::BlockGUI, QVariant(on_off), QString("visualize_system"));

//    quint64 type(on_off ? Mark::MT_STAIRS : Mark::MT_INVALID);
//    CommonMessageNotifier::send( (uint)visualize_system::BusTags::SetMarkPosition, QVariant(type), QString("visualize_system"));

//    ui->moreInfo->setDisabled(on_off);
//    ui->editObject->setDisabled(on_off);
}

void ViraStatusBar::slotEditAreaGeometry(bool on_off)
{
    uint64_t id = RegionBizManager::instance()->getCurrentEntity();
    BaseAreaPtr ptr = regionbiz::RegionBizManager::instance()->getBaseArea(id);
    if( ! ptr)
        return;

    quint64 _id_(on_off ? id : 0);
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::BlockGUI, ((bool)_id_), QString("visualize_system"));

    switch(ptr->getType())
    {
    case BaseArea::AT_REGION :
    case BaseArea::AT_LOCATION :
    case BaseArea::AT_FACILITY : {
        CommonMessageNotifier::send( (uint)visualize_system::BusTags::EditAreaGeometryOnMap, QVariant(_id_), QString("visualize_system"));
    }break;
    case BaseArea::AT_FLOOR :
    case BaseArea::AT_ROOM : {
        CommonMessageNotifier::send( (uint)visualize_system::BusTags::EditAreaGeometryOnPlan, QVariant(_id_), QString("visualize_system"));
    }break;
    }
    ui->moreInfo->setDisabled(on_off);
    //ui->addDefect->setDisabled(on_off);
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
        if(ui->addDefect->isChecked())
        {
            ui->addDefect->blockSignals(true);
            ui->addDefect->setChecked(false);
            ui->addDefect->blockSignals(false);

            showMarkInfoWidgwt(id);
        }

        CommonMessageNotifier::send( (uint)visualize_system::BusTags::BlockGUI, QVariant(false), QString("visualize_system"));
        RegionBizManager::instance()->setCurrentEntity(id);
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
    ui->addDefect->hide();
    ui->editObject->hide();
    ui->deleteObject->hide();
    ui->addFoto->hide();
    ui->addFoto360->hide();
    ui->addStairs->hide();
    ui->addLift->hide();
    ui->addPlaceholder->hide();
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
//        if( data.area_free > 1e-5 )
//        {
//            area = getArea( AT_FREE, data );
//            status = QString::fromUtf8( "Свободно" );
//        }
//        else if( data.area_rent > 1e-5 )
//        {
//            area = getArea( AT_RENT, data );
//            status = QString::fromUtf8( "В аренде" );
//        }
//        else if( data.area_property > 1e-5)
//        {
//            area = getArea( AT_PROP, data );
//            status = QString::fromUtf8( "Собственность" );
//        }
//        else
//        {
//            area = getArea( AT_UNAVAILABLE, data );
//            status = QString::fromUtf8( "Недоступно" );
//        }
//        area->setToolTip( QString::fromUtf8( "Площадь" ));
//        QLabel* status_lbl = new QLabel( status );
//        status_lbl->setToolTip( QString::fromUtf8( "Статус" ));
//        status_lbl->setStyleSheet( area->styleSheet() );

        // WARNING commemts arenda's things
        area = getArea( AT_ALL, data );
        status = QString::fromUtf8( "Площадь" );
        area->setToolTip( QString::fromUtf8( "Площадь" ));

        QLabel* status_lbl = new QLabel( QString::fromUtf8( "Площадь" ));
        status_lbl->setToolTip( QString::fromUtf8( "Площадь" ));
        status_lbl->setStyleSheet( area->styleSheet() );

        ui->horizontalLayout_widgets->addWidget( area );
        ui->horizontalLayout_widgets->addWidget( status_lbl );
    }
    ui->horizontalLayout_widgets->addStretch();
}

void ViraStatusBar::showAddres(BaseAreaPtr ptr)
{
    QString addr = recursiveGetName( ptr );
    ui->name->setText( addr );
    ui->name->setToolTip( QString::fromUtf8( "Адрес" ));
}

void ViraStatusBar::showName(BaseAreaPtr ptr)
{
    QString name = recursiveGetName( ptr );
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

void ViraStatusBar::showMarkName(QString name)
{
    ui->name->setText( name );
    ui->name->setToolTip( QString::fromUtf8( "Имя" ));
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
    case AT_UNAVAILABLE:
    {
        area_cnt = data.area_unavailable;
        color = "#666666";
        break;
    }
    case AT_ALL:
    {
        area_cnt = data.area;
        color = "#F0E68C";
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

void ViraStatusBar::showMarkStatus(QString status)
{
    QString color;
    if( QString::fromUtf8( "на проверку" ) == status )
        color = "#3CE63C";
    else if( QString::fromUtf8( "в работе" ) == status )
        color = "#F0E68C";
    else if( QString::fromUtf8( "новый" ) == status )
        color = "#FF8C00";

    QLabel* task = new QLabel;
    task->setStyleSheet( "background-color: " + color + ";"
                         "border-color: rgb(255, 255, 255);"
                         "border-radius: 10px;"
                         "color: rgb(0, 0, 0);"
                         "border-width: 2px;"
                         "border-style: solid;"
                         );
    task->setText( status );
    task->setToolTip( QString::fromUtf8( "Статус" ));

    ui->horizontalLayout_widgets->addWidget( task );
    ui->horizontalLayout_widgets->addStretch();
}

void ViraStatusBar::showMarkWorker(QString worker)
{
    QLabel* task = new QLabel;
    task->setText( worker );
    task->setToolTip( QString::fromUtf8( "Ответственный" ));

    ui->horizontalLayout_widgets->addWidget( task );
    ui->horizontalLayout_widgets->addStretch();
}

void ViraStatusBar::showMarkCategory(QString category)
{
    QLabel* task = new QLabel;
    task->setText( category );
    task->setToolTip( QString::fromUtf8( "Категория" ));
    task->setStyleSheet( "border-color: rgb(255, 255, 255);"
                         "border-width: 1px;"
                         "border-style: solid;"
                         );

    ui->horizontalLayout_widgets->addWidget( task );
    ui->horizontalLayout_widgets->addStretch();
}

void ViraStatusBar::showMarkPriority(QString priority)
{
    QLabel* task = new QLabel;
    task->setText( priority );
    task->setToolTip( QString::fromUtf8( "Приоритет" ));
    task->setStyleSheet( "border-color: rgb(255, 255, 255);"
                         "border-width: 1px;"
                         "border-style: solid;"
                         );

    ui->horizontalLayout_widgets->addWidget( task );
    ui->horizontalLayout_widgets->addStretch();
}

QString ViraStatusBar::recursiveGetName(BaseAreaPtr area)
{
    QString name;
    switch( area->getType() )
    {
    case BaseArea::AT_LOCATION:
    case BaseArea::AT_FACILITY:
    {
        name = area->getName(); // getDescription();
        break;
    }

    case BaseArea::AT_REGION:
    case BaseArea::AT_FLOOR:
    case BaseArea::AT_ROOM:
    {
        name = area->getName();
        break;
    }

    }

    if( BaseArea::AT_REGION == area->getType() )
        return name;
    if( BaseArea::AT_FLOOR == area->getType() )
        name = '\n' + name;

    QString that_name = recursiveGetName( area->getParent() ) + " / " + name;
    return  that_name;
}

void ViraStatusBar::slotShowMoreInfo()
{
    quint64 id = RegionBizManager::instance()->getCurrentEntity();
    MarkPtr markPtr = RegionBizManager::instance()->getMark(id);
    if(markPtr)
    {
        showMarkInfoWidgwt(id);
    }
    else
    {
        showMoreInfo(id);
    }

    /*
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
    */
}

void ViraStatusBar::slotMoreInfoWidgetClosed()
{
    disconnect(ui->moreInfo, SIGNAL(clicked(bool)), this, SLOT(slotShowMoreInfo(bool)));
    ui->moreInfo->setChecked(false);
    connect(ui->moreInfo, SIGNAL(clicked(bool)), this, SLOT(slotShowMoreInfo(bool)));
}

void ViraStatusBar::showMoreInfo(quint64 id)
{
    if( !_ifaceInfoWidget )
    {
        _infoForm = new InfoForm;
        connect(_infoForm, SIGNAL(signalCloseWindow()), this, SLOT(slotCloseMoreInfoForm()));
        connect(_infoForm, SIGNAL(signalShowMarkInfoWidget(quint64)), this, SLOT(showMarkInfoWidgwt(quint64)));
        _infoForm->showWidget(id);

        _ifaceInfoWidget = new EmbIFaceNotifier(_infoForm);
        QString tag = QString("ViraStatusBar_MoreInfoForm");
        quint64 widgetId = ewApp()->restoreWidget(tag, _ifaceInfoWidget);
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
            struc.iface = _ifaceInfoWidget;
            struc.widgetTag = tag;
            struc.minSize = QSize(300,300);
            struc.topOnHint = true;
            struc.isModal = true;
            ewApp()->createWidget(struc, _parentWidgetId);
        }
    }
    else
    {
        _infoForm->showWidget(id);
        ewApp()->setVisible(_ifaceInfoWidget->id(), true);
    }
}

void ViraStatusBar::slotCloseMoreInfoForm()
{
    ewApp()->setVisible(_ifaceInfoWidget->id(), false);
}

void ViraStatusBar::showMarkInfoWidgwt(quint64 id)
{
    MarkPtr ptr = RegionBizManager::instance()->getMark(id);
    if( ! ptr ) return;

    switch(ptr->getMarkType())
    {
    case Mark::MT_DEFECT : {
        if( !_ifaceInfoMarkWidget )
        {
            _markForm = new MarkForm;
            connect(_markForm, SIGNAL(signalCloseWindow()), this, SLOT(slotCloseMarkWindow()));
            _markForm->showWidget(id);

            _ifaceInfoMarkWidget = new EmbIFaceNotifier(_markForm);
            QString tag = QString("ViraStatusBar_MarkInfoWidget");
            quint64 widgetId = ewApp()->restoreWidget(tag, _ifaceInfoMarkWidget);
            if(0 == widgetId)
            {
                ew::EmbeddedWidgetStruct struc;
                ew::EmbeddedHeaderStruct headStr;
                headStr.hasCloseButton = false;
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
            ewApp()->setWidgetTitle(_ifaceInfoMarkWidget->id(), QString::fromUtf8("Информация о дефекте"));
            ewApp()->setWidgetIcon(_ifaceInfoMarkWidget->id(), ":/img/mark_button.png");
            ewApp()->setVisible(_ifaceInfoMarkWidget->id(), true);
        }
    }break;
    case Mark::MT_PHOTO : {
        QList<QPixmap> pixmaps;
        QVariant regionBizInitJson_Path = CtrConfig::getValueByName("application_settings.regionBizFilesPath");
        if(regionBizInitJson_Path.isValid())
        {
             QString destPath = regionBizInitJson_Path.toString() + QDir::separator() + QString::number(id);
             QDir dir(destPath);
             QStringList list = dir.entryList(QDir::Files);
             foreach(QString fileName,list)
             {
                 fileName.prepend(destPath + QDir::separator());
                 QPixmap pm(fileName);
                 if(pm.isNull() == false)
                     pixmaps.append(pm);
             }
        }
        if(pixmaps.isEmpty() == false)
        {
            ImageViewer imageViewer(pixmaps);
            imageViewer.showImageViewer(ptr->getName());
        }
    }break;
    case Mark::MT_PHOTO_3D : {
        if( !_ifacePhoto360Widget )
        {
            _photo360Form = new Photo360Form;
            _photo360Form->showWidget(id);
            connect(_photo360Form, SIGNAL(signalCloseWindow()), this, SLOT(slotClosePhoto360Window()));

            _ifacePhoto360Widget = new EmbIFaceNotifier(_photo360Form);
            QString tag = QString("ViraStatusBar_Photo360Widget");
            quint64 widgetId = ewApp()->restoreWidget(tag, _ifacePhoto360Widget);
            if(0 == widgetId)
            {
                ew::EmbeddedWidgetStruct struc;
                ew::EmbeddedHeaderStruct headStr;
                headStr.hasCloseButton = false;
                headStr.hasMinMaxButton = false;
                headStr.hasCollapseButton = false;
                headStr.headerPixmap = ":/img/foto360_button.png";
                headStr.windowTitle = QString::fromUtf8("Панорамная фотография");
                struc.header = headStr;
                struc.iface = _ifacePhoto360Widget;
                struc.widgetTag = tag;
                struc.minSize = QSize(300,300);
                struc.topOnHint = true;
                struc.isModal = true;
                ewApp()->createWidget(struc, _parentWidgetId);
            }
        }
        else
        {
            _photo360Form->showWidget(id);
            ewApp()->setVisible(_ifacePhoto360Widget->id(), true);
        }
    }break;
//    case Mark::MT_LIFT :
//    case Mark::MT_STAIRS :
//    {

//    }break;
    }
}

void ViraStatusBar::slotClosePhoto360Window()
{
    ewApp()->setVisible(_ifacePhoto360Widget->id(), false);
}

void ViraStatusBar::slotCloseStairOrLiftWindow()
{
    ewApp()->setVisible(_ifaceStairOrLiftWidget->id(), false);
}

void ViraStatusBar::slotMarkCreated(QVariant var)
{
    if(ui->addDefect->isChecked())
    {
        ui->addDefect->setChecked(false);
        slotAddDefect(false);
    }
    if(ui->addFoto->isChecked())
    {
        ui->addFoto->setChecked(false);
        slotAddFoto(false);
    }
    if(ui->addFoto360->isChecked())
    {
        ui->addFoto360->setChecked(false);
        slotAddFoto360(false);
    }
    if(ui->addStairs->isChecked())
    {
        ui->addStairs->setChecked(false);
        slotAddStairs(false);
    }
    if(ui->addLift->isChecked())
    {
        ui->addLift->setChecked(false);
        slotAddLift(false);
    }

    quint64 parentId = RegionBizManager::instance()->getCurrentEntity();
    if(parentId == 0)
        return;

    QList<QVariant> list = var.toList();
    if(list.size() >= 2)
    {
        QVariant typeVar = list.at(0);
        Mark::MarkType type = (Mark::MarkType)typeVar.toInt();
        switch(type)
        {
        case Mark::MT_DEFECT :
        {
            quint64 parentId = RegionBizManager::instance()->getCurrentEntity();
            QPolygonF areaMark = list.at(1).value<QPolygonF>();
            qDebug() << "parentId" << parentId << "pos size :" << areaMark.size() << ", slotMarkCreated, pol :" << areaMark;

            if( !_ifaceInfoMarkWidget )
            {
                _markForm = new MarkForm;
                connect(_markForm, SIGNAL(signalCloseWindow()), this, SLOT(slotCloseMarkWindow()));
                _markForm->showWidgetAndCreateMark(Mark::MT_DEFECT, parentId, areaMark);

                _ifaceInfoMarkWidget = new EmbIFaceNotifier(_markForm);
                QString tag = QString("ViraStatusBar_MarkInfoWidget");
                quint64 widgetId = ewApp()->restoreWidget(tag, _ifaceInfoMarkWidget);
                if(0 == widgetId)
                {
                    ew::EmbeddedWidgetStruct struc;
                    ew::EmbeddedHeaderStruct headStr;
                    headStr.hasCloseButton = false;
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
                _markForm->showWidgetAndCreateMark(Mark::MT_DEFECT, parentId, areaMark);
                ewApp()->setWidgetTitle(_ifaceInfoMarkWidget->id(), QString::fromUtf8("Информация о дефекте"));
                ewApp()->setWidgetIcon(_ifaceInfoMarkWidget->id(), ":/img/mark_button.png");
                ewApp()->setVisible(_ifaceInfoMarkWidget->id(), true);
            }
        }break;
        case Mark::MT_PHOTO :
        case Mark::MT_PHOTO_3D :
        {
            QPointF pos = list.at(1).toPointF();
            double direction = 0;
            QString windowTitle = QString::fromUtf8("Панорамная фотография");
            QString iconPath = ":/img/foto360_button.png";
            if(type == Mark::MT_PHOTO)
            {
                windowTitle = QString::fromUtf8("Фотография");
                iconPath =
                direction = list.at(2).toDouble();
            }
            qDebug() << "---> pos" << pos << "direction" << direction;

            if( !_ifaceInfoMarkWidget )
            {
                _markForm = new MarkForm;
                connect(_markForm, SIGNAL(signalCloseWindow()), this, SLOT(slotCloseMarkWindow()));
                _markForm->showWidgetAndCreateMark(type, parentId, QPolygonF()<<pos, direction);

                _ifaceInfoMarkWidget = new EmbIFaceNotifier(_markForm);
                QString tag = QString("ViraStatusBar_MarkInfoWidget");
                quint64 widgetId = ewApp()->restoreWidget(tag, _ifaceInfoMarkWidget);
                if(0 == widgetId)
                {
                    ew::EmbeddedWidgetStruct struc;
                    ew::EmbeddedHeaderStruct headStr;
                    headStr.hasCloseButton = true;
                    headStr.hasMinMaxButton = false;
                    headStr.hasCollapseButton = false;
                    headStr.headerPixmap = iconPath;
                    headStr.windowTitle = windowTitle;
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
                _markForm->showWidgetAndCreateMark(type, parentId, QPolygonF()<<pos, direction);
                ewApp()->setWidgetTitle(_ifaceInfoMarkWidget->id(), windowTitle);
                ewApp()->setWidgetIcon(_ifaceInfoMarkWidget->id(), iconPath);
                ewApp()->setVisible(_ifaceInfoMarkWidget->id(), true);
            }
        }break;
        case Mark::MT_PLACEHOLDER :
        {
            quint64 entityId = RegionBizManager::instance()->getCurrentEntity();
            BaseAreaPtr areaPtr = RegionBizManager::instance()->getBaseArea(entityId);
            if(areaPtr)
            {
                MarkPtr markPtr;
                auto holder = areaPtr->convert<MarksHolder>();
                if(holder)
                {
                    QPointF pos = list.at(1).toPointF();
                    markPtr = holder->addMark(regionbiz::Mark::MT_PLACEHOLDER, QPolygonF() << pos);
                }
                if(markPtr)
                {
                    bool commitRes = markPtr->commit();
                    qDebug() << "PLACEHOLDER COMMITED : " << commitRes;
                }
            }
        }break;
//        case Mark::MT_LIFT :
//        case Mark::MT_STAIRS :
//        {
//            quint64 entityId = RegionBizManager::instance()->getCurrentEntity();
//            BaseAreaPtr facilityPtr;
//            BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(entityId);
//            while(ptr)
//            {
//                if(ptr->getType() == BaseArea::AT_FACILITY)
//                {
//                    facilityPtr = ptr;
//                    break;
//                }
//                ptr = ptr->getParent();
//            }
//            if(facilityPtr)
//            {
//                uint64_t facilityId = facilityPtr->getId();
//                QPointF pos = list.at(1).toPointF();
//                //qDebug() << "parentId" << parentId << "pos size :" << areaMark.size() << ", slotMarkCreated, pol :" << areaMark;

//                QString windowTitle(type == Mark::MT_LIFT ? QString::fromUtf8("Информация о лифте") : QString::fromUtf8("Информация о лестнице"));
//                QString headerPixmap(type == Mark::MT_LIFT ? ":/img/lift_button.png" : ":/img/stairs_button.png");

//                if( !_ifaceStairOrLiftWidget )
//                {
//                    _stairOrLiftForm = new StairOrLiftForm;
//                    connect(_stairOrLiftForm, SIGNAL(signalCloseWindow()), this, SLOT(slotCloseStairOrLiftWindow()));
//                    _stairOrLiftForm->createEntity(facilityId, type, pos);

//                    _ifaceStairOrLiftWidget = new EmbIFaceNotifier(_stairOrLiftForm);
//                    QString tag = QString("ViraStatusBar_StairOrLiftWidget");
//                    quint64 widgetId = ewApp()->restoreWidget(tag, _ifaceStairOrLiftWidget);
//                    if(0 == widgetId)
//                    {
//                        ew::EmbeddedWidgetStruct struc;
//                        ew::EmbeddedHeaderStruct headStr;
//                        headStr.hasCloseButton = false;
//                        headStr.hasMinMaxButton = false;
//                        headStr.hasCollapseButton = false;
//                        headStr.headerPixmap = headerPixmap;
//                        headStr.windowTitle = windowTitle;
//                        struc.header = headStr;
//                        struc.iface = _ifaceStairOrLiftWidget;
//                        struc.widgetTag = tag;
//                        struc.minSize = QSize(300,300);
//                        struc.topOnHint = true;
//                        struc.isModal = true;
//                        ewApp()->createWidget(struc, _parentWidgetId);
//                    }
//                }
//                else
//                {
//                    _stairOrLiftForm->createEntity(facilityId, type, pos); // _stairOrLiftForm->editEntity(facilityId, entityId);
//                    ewApp()->setWidgetTitle(_ifaceStairOrLiftWidget->id(), windowTitle);
//                    ewApp()->setWidgetIcon(_ifaceStairOrLiftWidget->id(), headerPixmap);
//                    ewApp()->setVisible(_ifaceStairOrLiftWidget->id(), true);
//                }
//            }

//        }break;
        }
    }
}

void ViraStatusBar::slotCloseMarkWindow()
{
    ewApp()->setVisible(_ifaceInfoMarkWidget->id(), false);
}

