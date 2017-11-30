#include "instrumentalform.h"
#include "floorgraphmakerstate.h"
#include "ui_instrumentalform.h"
#include <ctrcore/visual/visualizermanager.h>
#include <ctrcore/visual/viewinterface.h>
#include <ctrcore/visual/stateinterface.h>
#include <QColorDialog>
#include <QPen>
#include <QTimer>
#include <QDebug>

using namespace floor_graph_maker;

InstrumentalForm::InstrumentalForm(uint visualizerId)
    : ew::EmbeddedSubIFace()
    , ui(new Ui::InstrumentalForm)
    , _visualizerId(visualizerId)
{
    ui->setupUi(this);

    ui->nodeOrEdgeButton->setIcon(QIcon(":/img/.png"));
    connect(ui->nodeOrEdgeButton, SIGNAL(clicked(bool)), this, SLOT(slotSetNodeOrEdge(bool)));

    ui->cameraButton->setIcon(QIcon(":/img/.png"));
    connect(ui->cameraButton, SIGNAL(clicked(bool)), this, SLOT(slotSetCamera(bool)));

    ui->placeholderButton->setIcon(QIcon(":/img/.png"));
    connect(ui->placeholderButton, SIGNAL(clicked(bool)), this, SLOT(slotSetPlaceHolder(bool)));

    ui->saveButton->setIcon(QIcon(":/img/.png"));
    connect(ui->saveButton, SIGNAL(pressed()), this, SLOT(slotSave()));

    ui->removeButton->setIcon(QIcon(":/img/.png"));
    connect(ui->removeButton, SIGNAL(pressed()), this, SLOT(slotRemoveElement()));

    _floorGraphMakerState = QSharedPointer<floor_graph_maker::FloorGraphMakerState>(new floor_graph_maker::FloorGraphMakerState());
    visualize_system::StateInterface * stateInterface = visualize_system::VisualizerManager::instance()->getStateInterface(_visualizerId);
    // connect(_svgEditorState.data(), SIGNAL(svgDocSaved(QString,QPointF)), this, SIGNAL(svgDocSaved(QString,QPointF)));
    if(stateInterface)
        stateInterface->setVisualizerState(_floorGraphMakerState);

    QTimer::singleShot(50,this,SLOT(slotMakeAdjustForm()));
}

InstrumentalForm::~InstrumentalForm()
{
    delete ui;

    if(_floorGraphMakerState)
    {
        _floorGraphMakerState->emit_closeState();
        _floorGraphMakerState.clear();
    }
}

QWidget *InstrumentalForm::getWidget()
{
    return this;
}

void InstrumentalForm::closed(bool *acceptFlag)
{
    if(acceptFlag)
    {
        // *acceptFlag = false;
        emit signalClosed();
    }
}

void InstrumentalForm::slotSetNodeOrEdge(bool on_off)
{
    uncheckedButtons(sender());
    if(on_off)
    {
        ui->removeWidget->hide();
        ui->edgeWidget->hide();
        _floorGraphMakerState->setMode(Mode::SetNodeOrEdgeElement);
    }
}

void InstrumentalForm::slotSetCamera(bool on_off)
{
    uncheckedButtons(sender());
    if(on_off)
    {
        ui->removeWidget->hide();
        ui->edgeWidget->hide();
        _floorGraphMakerState->setMode(Mode::SetCamera);
    }
}

void InstrumentalForm::slotSetPlaceHolder(bool on_off)
{
    uncheckedButtons(sender());
    if(on_off)
    {
        ui->removeWidget->hide();
        ui->edgeWidget->hide();
        _floorGraphMakerState->setMode(Mode::SetPlaceHolder);
    }
}

void InstrumentalForm::slotSave()
{
    _floorGraphMakerState->save();
}

void InstrumentalForm::slotMakeAdjustForm()
{
    QWidget * wgt = this;
    QSize wgtMinSize = wgt->minimumSize();
    QSize wgtMaxSize = wgt->maximumSize();
    wgt->setFixedWidth(wgt->size().width());
    adjustSize();
    wgt->resize(0,0);
    ewApp()->adjustWidgetSize(id());
    wgt->setMinimumWidth(wgtMinSize.width());
    wgt->setMaximumWidth(wgtMaxSize.width());
}

void InstrumentalForm::uncheckedButtons(QObject * button)
{
    if(button != ui->nodeOrEdgeButton && ui->nodeOrEdgeButton->isChecked())
        ui->nodeOrEdgeButton->setChecked(false);

    if(button != ui->cameraButton && ui->cameraButton->isChecked())
        ui->cameraButton->setChecked(false);

    if(button != ui->placeholderButton && ui->placeholderButton->isChecked())
        ui->placeholderButton->setChecked(false);

    _floorGraphMakerState->setMode(Mode::ScrollMode);

    ui->edgeWidget->show();
}






