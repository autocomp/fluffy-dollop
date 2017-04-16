#include "filter_form.h"
#include "ui_filter_form.h"

#include <iostream>
#include <QPushButton>

#include <dmanager/embeddedstruct.h>
#include <dmanager/embeddedapp.h>
#include <regionbiz/rb_entity_filter.h>

#include "slide_window.h"

FilterForm::FilterForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FilterForm)
{
    ui->setupUi(this);
}

FilterForm::~FilterForm()
{
    delete ui;
}

FilterForm &FilterForm::instance()
{
    static FilterForm form;
    return form;
}

void FilterForm::registrateToSlideWindow()
{
    // set panel propertys
    SlideWindow* sw = SlideWindow::mainInstance();

    // add buttons
    QPushButton* btn_filter = new QPushButton();
    btn_filter->setMinimumSize( 40, 40 );
    btn_filter->setMaximumSize( 40, 40 );
    btn_filter->setIcon( QIcon( ":/img/filter.png" ));
    btn_filter->setToolTip( QString::fromUtf8( "Установить фильтр" ));
    sw->addWidget( btn_filter );

    QPushButton* btn_free_filter = new QPushButton();
    btn_free_filter->setMinimumSize( 40, 40 );
    btn_free_filter->setMaximumSize( 40, 40 );
    btn_free_filter->setIcon( QIcon( ":/img/delete_filter.png" ));
    btn_free_filter->setToolTip( QString::fromUtf8( "Сбросить фильтр" ));
    sw->addWidget( btn_free_filter );

    // connect with buttons
    QObject::connect( btn_filter, SIGNAL( clicked( bool )), SLOT( showFilterWindow() ));
    QObject::connect( btn_free_filter, SIGNAL( clicked( bool )), SLOT( resetFilters() ));
}

void FilterForm::on_pushButton_reset_clicked()
{
    ui->checkBox_after->setChecked( false );
    ui->doubleSpinBox_after->setValue( 0 );
    ui->checkBox_before->setChecked( false );
    ui->doubleSpinBox_before->setValue( 0 );

    ui->comboBox_status->setCurrentIndex( 0 );
    ui->lineEdit_rentor->setText( "" );
}

void FilterForm::on_pushButton_apply_clicked()
{
    using namespace regionbiz;

    // make filter
    BaseFilterParamPtrs filters;

    bool set_area_filter = ui->checkBox_after->isChecked()
            || ui->checkBox_before->isChecked();
    if( set_area_filter )
    {
        auto area_filter = BaseFilterParamFabric::createFilter< DoubleFilterParam >( "area" );
        if( ui->checkBox_after->isChecked() )
            area_filter->setMin( ui->doubleSpinBox_after->value() );
        if( ui->checkBox_before->isChecked() )
            area_filter->setMax( ui->doubleSpinBox_before->value() );
        area_filter->setGetEmptyMetadate( false );
        filters.push_back( area_filter );
    }

    if( ui->comboBox_status->currentIndex() )
    {
        auto status_filter = BaseFilterParamFabric::createFilter< StringFilterParam >( "status" );
        status_filter->setRegExp( ui->comboBox_status->currentText() );
        filters.push_back( status_filter );
    }

    QString rentor_str = ui->lineEdit_rentor->text();
    if( !rentor_str.isEmpty() )
    {
        auto rentor_filter = BaseFilterParamFabric::createFilter< StringFilterParam >( "rentor" );
        rentor_filter->setRegExp( "*" + rentor_str + "*" );
        filters.push_back( rentor_filter );
    }

    // set filter
    EntityFilter::setFilters( filters );
}

void FilterForm::on_pushButton_ok_clicked()
{
    // apply
    on_pushButton_apply_clicked();

    // hide window
    hideWindow();
}

void FilterForm::hideWindow()
{
    ewApp()->setVisible(_ifaceInfoWidget->id(), false);
}

void FilterForm::resetFilters()
{
    on_pushButton_reset_clicked();
    regionbiz::EntityFilter::clearFilters();
}

void FilterForm::showFilterWindow()
{
    if( !_ifaceInfoWidget )
    {
        _ifaceInfoWidget = new EmbIFaceNotifier( ui->widgetFilter );
        QString tag = QString("ViraFilter_FilterForm");
        quint64 widgetId = ewApp()->restoreWidget(tag, _ifaceInfoWidget);
        if(0 == widgetId)
        {
            ew::EmbeddedWidgetStruct struc;
            ew::EmbeddedHeaderStruct headStr;
            headStr.hasCloseButton = true;
            headStr.hasMinMaxButton = false;
            headStr.hasCollapseButton = false;
            headStr.headerPixmap = ":/img/filter.png";
            headStr.windowTitle = QString::fromUtf8("Фильтрация");
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
        ewApp()->setVisible(_ifaceInfoWidget->id(), true);
}

