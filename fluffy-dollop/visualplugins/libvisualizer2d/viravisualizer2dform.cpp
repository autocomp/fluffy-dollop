#include "viravisualizer2dform.h"
#include "ui_viravisualizer2dform.h"
#include <ctrcore/ctrcore/ctrconfig.h>
#include <QSplitter>
#include <QStackedWidget>
#include <QTimer>
#include <QDebug>

ViraVisualizer2dForm::ViraVisualizer2dForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ViraVisualizer2dForm)
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
    connect(ui->splitter, SIGNAL(splitterMoved(int,int)), this, SLOT(slotSplitterMoved(int,int)));
}

ViraVisualizer2dForm::~ViraVisualizer2dForm()
{
    delete ui;
}

void ViraVisualizer2dForm::slotSplitterMoved(int pos,int index)
{
    QList<int> list = ui->splitter->sizes();
    if(list.size() == 2)
    {
        CtrConfig::setValueByName("application_settings.mainSplittetLeftSize_int", list.first());
        CtrConfig::setValueByName("application_settings.mainSplittetRightSize_int", list.last());
        qDebug() << "slotSplitterMoved pos:" << pos << ", index:" << index << ", sizes():" << list.first() << list.last();
    }
}

QStackedWidget *ViraVisualizer2dForm::getStackedWidget()
{
    return ui->stackedWidget;
}

void ViraVisualizer2dForm::addWidgetToSplitterLeftArea(QWidget *wdg)
{
    ui->leftSplitterLayout->addWidget(wdg);
    QTimer::singleShot(1, this, SLOT(slotMoveSplitter()));
}

void ViraVisualizer2dForm::slotMoveSplitter()
{
    QVariant splitterLeftSizeVar = CtrConfig::getValueByName("application_settings.mainSplittetLeftSize_int");
    QVariant splitterRightSizeVar = CtrConfig::getValueByName("application_settings.mainSplittetRightSize_int");
    if(splitterLeftSizeVar.isValid() && splitterRightSizeVar.isValid())
    {
        ui->splitter->setSizes(QList<int>() << splitterLeftSizeVar.toInt() << splitterRightSizeVar.toInt());
    }
}

void ViraVisualizer2dForm::addWidgetToStatusBar(QWidget *wdg)
{
    ui->mainVerticalLayout->addWidget(wdg);
}












