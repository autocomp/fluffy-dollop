#include "viravisualizer2dform.h"
#include "ui_viravisualizer2dform.h"

ViraVisualizer2dForm::ViraVisualizer2dForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViraVisualizer2dForm)
{
    ui->setupUi(this);
}

ViraVisualizer2dForm::~ViraVisualizer2dForm()
{
    delete ui;
}

QStackedWidget *ViraVisualizer2dForm::getStackedWidget()
{
    return ui->stackedWidget;
}

void ViraVisualizer2dForm::addWidgetToSplitterLeftArea(QWidget *wdg)
{
    ui->leftSplitterLayout->addWidget(wdg);
}

void ViraVisualizer2dForm::addWidgetToStatusBar(QWidget *wdg)
{
    ui->mainVerticalLayout->addWidget(wdg);
}
