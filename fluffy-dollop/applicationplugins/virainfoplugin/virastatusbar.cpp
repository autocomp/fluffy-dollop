#include "virastatusbar.h"
#include "ui_virastatusbar.h"

#include <QDebug>

#include <libembeddedwidgets/embeddedstruct.h>
#include <libembeddedwidgets/embeddedapp.h>
#include <ctrcore/tempinputdata/tempdatatypies.h>
#include <ctrcore/tempinputdata/tempdatacontroller.h>

#include "virainfowidget.h"

using namespace regionbiz;

ViraStatusBar::ViraStatusBar( quint64 parentWidgetId, QWidget *parent ):
    QWidget(parent),
    ui(new Ui::ViraStatusBar),
    _parentWidgetId(parentWidgetId)
{
    ui->setupUi(this);
    connect(ui->moreInfo, SIGNAL(clicked(bool)), this, SLOT(slotShowMoreInfo(bool)));
    reset();

    auto mngr = RegionBizManager::instance();
    mngr->subscribeOnSelect(this, SLOT(slotObjectSelectionChanged(uint64_t,uint64_t)));

    auto moreInfoWidget = new ViraInfoWidget();
    _iface = new EmbIFaceNotifier( moreInfoWidget );
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
        headStr.windowTitle = QString::fromUtf8("Информация");
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

void ViraStatusBar::slotObjectSelectionChanged( uint64_t /*prev_id*/, uint64_t curr_id )
{
    reset();

    BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(curr_id);
    if(! ptr)
        return;

    auto data = TestDataGetter::getData( curr_id );

    switch( ptr->getType() ) {
    case BaseArea::AT_REGION:
    case BaseArea::AT_LOCATION:
    case BaseArea::AT_FACILITY:
    case BaseArea::AT_FLOOR:
    {
        showAddres( ptr );
        showTasks( data );
        showArendators( data, false );
        showAreas( data, false );
        showDebt( data );

        break;
    }

    case BaseArea::AT_ROOMS_GROUP:
    case BaseArea::AT_ROOM:
    {
        showName( ptr );
        showAreas( data, true );
        showArendators( data, true );
        showTasks( data );

        break;
    }
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
}

void ViraStatusBar::showTasks( TestData data )
{
    QLabel* work_task = getTask( TT_WORK, data );
    QLabel* lost_task = getTask( TT_LOST, data );
    QLabel* new_task = getTask( TT_NEW, data );

    ui->horizontalLayout_widgets->addWidget( work_task );
    ui->horizontalLayout_widgets->addWidget( lost_task );
    ui->horizontalLayout_widgets->addWidget( new_task );
    ui->horizontalLayout_widgets->addStretch();
}

void ViraStatusBar::showArendators( TestData data, bool one )
{
    if( !one )
    {
        QLabel* arend_good = getArendator( AT_GOOD );
        QLabel* arend_good_count = new QLabel( QString::number(  data.arendators_good ));
        QLabel* arend_bad = getArendator( AT_BAD );
        QLabel* arend_bad_count = new QLabel( QString::number(  data.arendators_bad ));

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

        QLabel* arend = new QLabel( arend_name );
        if( !arend_name.isEmpty() )
            ui->horizontalLayout_widgets->addWidget( man );
        ui->horizontalLayout_widgets->addWidget( arend );
    }

    ui->horizontalLayout_widgets->addStretch();
}

void ViraStatusBar::showAreas( TestData data, bool one )
{
    if( !one )
    {
        QLabel* area_rent = getArea( AT_RENT, data );
        QLabel* area_free = getArea( AT_FREE, data );

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

        QLabel* status_lbl = new QLabel( status );
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
}

void ViraStatusBar::showDebt(TestData data)
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
        color = "#aaffaa";
        break;
    }
    case AT_BAD:
    {
        color = "#ffaaaa";
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

QLabel *ViraStatusBar::getTask(ViraStatusBar::TaskType type, TestData data)
{
    QString color;
    int tasks;
    switch( type )
    {
    case TT_LOST:
    {
        tasks = data.task_lost;
        color = "#ffaaaa";
        break;
    }
    case TT_WORK:
    {
        tasks = data.task_work;
        color = "#aaffaa";
        break;
    }
    case TT_NEW:
    {
        tasks = data.task_new;
        color = "#ffffaa";
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

QLabel *ViraStatusBar::getArea(ViraStatusBar::AreaType type, TestData data)
{
    QString color;
    int area_cnt;
    switch( type )
    {
    case AT_RENT:
    {
        area_cnt = data.area_rent;
        color = "#ffffaa";
        break;
    }
    case AT_FREE:
    {
        area_cnt = data.area_free;
        color = "#aaffaa";
        break;
    }
    case AT_PROP:
    {
        area_cnt = data.area_property;
        color = "#aaaaff";
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
    if( !_iface )
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



















