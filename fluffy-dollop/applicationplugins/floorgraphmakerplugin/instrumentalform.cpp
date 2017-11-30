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

InstrumentalForm::InstrumentalForm(uint visualizerId, uint64_t floorId)
    : ew::EmbeddedSubIFace()
    , ui(new Ui::InstrumentalForm)
{
    ui->setupUi(this);

    ui->nodeOrEdgeButton->setIcon(QIcon(":/img/floorgraphstate.png"));
    connect(ui->nodeOrEdgeButton, SIGNAL(clicked(bool)), this, SLOT(slotSetNodeOrEdge(bool)));

    ui->saveButton->setIcon(QIcon(":/img/icon_save.png"));
    connect(ui->saveButton, SIGNAL(pressed()), this, SLOT(slotSave()));

    connectInit();

    _floorGraphMakerState = QSharedPointer<floor_graph_maker::FloorGraphMakerState>(new floor_graph_maker::FloorGraphMakerState(floorId));
    visualize_system::StateInterface * stateInterface = visualize_system::VisualizerManager::instance()->getStateInterface(visualizerId);
    if(stateInterface)
        stateInterface->setVisualizerState(_floorGraphMakerState);

    ui->removeWidget->setVisible(false);
    ui->edgeWidget->setVisible(false);
    ui->wallRB->setChecked(true);
    ui->wallWidget->setVisible(true);
    ui->doorWidget->setVisible(false);
    ui->windowWidget->setVisible(false);

    // чтение параметров из конфига

    QTimer::singleShot(50,this,SLOT(slotMakeAdjustForm()));
}

InstrumentalForm::InstrumentalForm(uint elementId, ElementType elementType)
    : ew::EmbeddedSubIFace()
    , ui(new Ui::InstrumentalForm)
    , _elementId(elementId)
{
    ui->setupUi(this);

    ui->removeButton->setIcon(QIcon(":/img/icon_delete.png"));
    connect(ui->removeButton, SIGNAL(pressed()), this, SLOT(slotRemoveElement()));

    connectInit();

    ui->stateButtonWidget->setVisible(false);
    ui->removeWidget->setVisible(true);

    setCurrentElementType(elementType);

    QTimer::singleShot(50,this,SLOT(slotMakeAdjustForm()));
}

void InstrumentalForm::connectInit()
{
    connect(ui->wallRB, SIGNAL(clicked(bool)), this, SLOT(slotEdgeTypeChanged(bool)));
    connect(ui->doorRB, SIGNAL(clicked(bool)), this, SLOT(slotEdgeTypeChanged(bool)));
    connect(ui->windowRB, SIGNAL(clicked(bool)), this, SLOT(slotEdgeTypeChanged(bool)));
    connect(ui->wallWidth, SIGNAL(valueChanged(double)), this, SLOT(doubleValueChanged(double)));
    connect(ui->wallHeight, SIGNAL(valueChanged(double)), this, SLOT(doubleValueChanged(double)));
    connect(ui->windowHeightUnderFloor, SIGNAL(valueChanged(double)), this, SLOT(doubleValueChanged(double)));
    connect(ui->windowHeight, SIGNAL(valueChanged(double)), this, SLOT(doubleValueChanged(double)));
    connect(ui->doorState, SIGNAL(currentIndexChanged(int)), this, SLOT(intValueChanged(int)));

}

InstrumentalForm::~InstrumentalForm()
{
    if(_floorGraphMakerState)
    {
        // запись параметров в конфига, если это главное окно плагина

        _floorGraphMakerState->emit_closeState();
        _floorGraphMakerState.clear();
    }

    delete ui;
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

void InstrumentalForm::setCurrentElementType(ElementType elementType)
{
    switch(elementType)
    {
    case ElementType::None :
        ui->removeWidget->hide();
        ui->edgeWidget->hide();
        break;
    case ElementType::Node :
        ui->removeWidget->show();
        ui->edgeWidget->hide();
        break;
    case ElementType::Edge :
        ui->removeWidget->show();
        ui->edgeWidget->show();
        break;
    }
}

void InstrumentalForm::setCurrentProperty(EdgeProperty property)
{
    ui->wallRB->setChecked(property.type == EdgeType::Wall);
    ui->doorRB->setChecked(property.type == EdgeType::Door);
    ui->windowRB->setChecked(property.type == EdgeType::Window);

    ui->wallWidget->setVisible(ui->wallRB->isChecked());
    ui->doorWidget->setVisible(ui->doorRB->isChecked());
    ui->windowWidget->setVisible(ui->windowRB->isChecked());

    ui->wallWidth->setValue(property.wallWidth);
    ui->wallHeight->setValue(property.wallHeight);
    ui->windowHeightUnderFloor->setValue(property.windowHeightUnderFloor);
    ui->windowHeight->setValue(property.windowHeight);

    ui->doorState->setCurrentText(property.state == DoorState::Open ? QString::fromUtf8("открыто") : QString::fromUtf8("закрыто"));
}

void InstrumentalForm::slotSetNodeOrEdge(bool on_off)
{
    if(on_off)
    {
        ui->removeWidget->hide();
        ui->edgeWidget->show();
        if(_floorGraphMakerState)
            _floorGraphMakerState->setMode(Mode::SetNodeOrEdgeElement);
    }
    else
    {
        ui->removeWidget->hide();
        ui->edgeWidget->hide();
        if(_floorGraphMakerState)
            _floorGraphMakerState->setMode(Mode::ScrollMode);
    }
    QTimer::singleShot(50,this,SLOT(slotMakeAdjustForm()));
}

void InstrumentalForm::slotSave()
{
    if(_floorGraphMakerState)
        _floorGraphMakerState->save();
}

void InstrumentalForm::slotRemoveElement()
{
    emit signalRemoveElement(_elementId);
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

void InstrumentalForm::slotEdgeTypeChanged(bool)
{
    ui->wallWidget->setVisible(ui->wallRB->isChecked());
    ui->doorWidget->setVisible(ui->doorRB->isChecked());
    ui->windowWidget->setVisible(ui->windowRB->isChecked());
    sendCurrentProperty();
    QTimer::singleShot(50,this,SLOT(slotMakeAdjustForm()));
}

void InstrumentalForm::doubleValueChanged(double)
{
    sendCurrentProperty();
}

void InstrumentalForm::intValueChanged(int)
{
    sendCurrentProperty();
}

void InstrumentalForm::sendCurrentProperty()
{
    EdgeProperty property;
    if(ui->wallRB->isChecked())
        property.type = EdgeType::Wall;
    else if (ui->doorRB->isChecked())
        property.type = EdgeType::Door;
    else if (ui->windowRB->isChecked())
        property.type = EdgeType::Window;

    property.wallWidth = ui->wallWidth->value();
    property.wallHeight = ui->wallHeight->value();
    property.windowHeightUnderFloor = ui->windowHeightUnderFloor->value();
    property.windowHeight = ui->windowHeight->value();
    property.state = ui->doorState->currentText() == QString::fromUtf8("открыто") ? DoorState::Open : DoorState::Close;

    if(_floorGraphMakerState)
        _floorGraphMakerState->edgeStateChanged(property);
    else
        emit signalEdgeStateChanged(_elementId, property);
}


















