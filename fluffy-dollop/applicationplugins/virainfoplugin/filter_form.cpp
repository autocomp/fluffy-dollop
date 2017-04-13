#include "filter_form.h"
#include "ui_filter_form.h"

#include <iostream>
#include <QPushButton>

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
    sw->setSmallSize( QSize( 10, 80 ));
    sw->setBigSize( QSize( 60, 110 ));
    sw->updateGeometry();
    sw->setType( SlideWindow::ST_RIGHT );
    sw->setDuration( 150 );
    sw->setViewProportionsl( 0.1 );
    sw->show();

    // add buttons
    QPushButton* btn_filter = new QPushButton();
    btn_filter->setMinimumSize( 40, 40 );
    btn_filter->setMaximumSize( 40, 40 );
    btn_filter->setIcon( QIcon( ":/img/filter.png" ));
    sw->addWidget( btn_filter );

    QPushButton* btn_free_filter = new QPushButton();
    btn_free_filter->setMinimumSize( 40, 40 );
    btn_free_filter->setMaximumSize( 40, 40 );
    btn_free_filter->setIcon( QIcon( ":/img/delete_filter.png" ));
    sw->addWidget( btn_free_filter );

    // connect with buttons
    QObject::connect( btn_filter, SIGNAL( clicked( bool )), SLOT( showFilterWindow() ));
    QObject::connect( btn_free_filter, SIGNAL( clicked( bool )), SLOT( resetFilters() ));
}

void FilterForm::showFilterWindow()
{
    std::cerr << "Show filters here" << std::endl;
}

void FilterForm::resetFilters()
{
    std::cerr << "Reset filters here" << std::endl;
}
