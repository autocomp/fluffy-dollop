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

    // ui->nodeButton->setIcon(QIcon(":/img/.png"));
    connect(ui->nodeButton, SIGNAL(clicked(bool)), this, SLOT(slotSetNode(bool)));

    // ui-edgeButton>->setIcon(QIcon(":/img/.png"));
    connect(ui->edgeButton, SIGNAL(clicked(bool)), this, SLOT(slotSetEdge(bool)));

    // ui->doorButton->setIcon(QIcon(":/img/.png"));
    connect(ui->doorButton, SIGNAL(clicked(bool)), this, SLOT(slotSetDoor(bool)));

    // ui->windowButton->setIcon(QIcon(":/img/.png"));
    connect(ui->windowButton, SIGNAL(clicked(bool)), this, SLOT(slotSetWindow(bool)));

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

void InstrumentalForm::slotSetNode(bool on_off)
{
    uncheckedButtons(sender());
    if(on_off)
        _floorGraphMakerState->setMode(Mode::Node);
}

void InstrumentalForm::slotSetEdge(bool on_off)
{
    uncheckedButtons(sender());
    if(on_off)
        _floorGraphMakerState->setMode(Mode::Edge);
}

void InstrumentalForm::slotSetDoor(bool on_off)
{
    uncheckedButtons(sender());
    if(on_off)
        _floorGraphMakerState->setMode(Mode::Door);
}

void InstrumentalForm::slotSetWindow(bool on_off)
{
    uncheckedButtons(sender());
    if(on_off)
        _floorGraphMakerState->setMode(Mode::Window);
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
    if(button != ui->nodeButton && ui->nodeButton->isChecked())
        ui->nodeButton->setChecked(false);
    if(button != ui->edgeButton && ui->edgeButton->isChecked())
        ui->edgeButton->setChecked(false);
    if(button != ui->doorButton && ui->doorButton->isChecked())
        ui->doorButton->setChecked(false);
    if(button != ui->windowButton && ui->windowButton->isChecked())
        ui->windowButton->setChecked(false);

    _floorGraphMakerState->setMode(Mode::Scroll);
}






