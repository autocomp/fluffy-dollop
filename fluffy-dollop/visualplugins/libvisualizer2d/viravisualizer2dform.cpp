#include "viravisualizer2dform.h"
#include "ui_viravisualizer2dform.h"
#include <ctrcore/ctrcore/ctrconfig.h>

ViraVisualizer2dForm::ViraVisualizer2dForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViraVisualizer2dForm)
{
    ui->setupUi(this);

    int handleWidth(10);
    QVariant handleWidthVar = CtrConfig::getValueByName("application_settings.mainSplittetHandleWidth_Pixels", 10, true);
    if(handleWidthVar.isValid())
    {
        if(handleWidthVar.toUInt() > 0 && handleWidthVar.toUInt() <= 20)
            handleWidth = handleWidthVar.toUInt();
        else
            CtrConfig::setValueByName("application_settings.mainSplittetHandleWidth_Pixels", 10);
    }
    ui->splitter->setHandleWidth(handleWidth);
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
