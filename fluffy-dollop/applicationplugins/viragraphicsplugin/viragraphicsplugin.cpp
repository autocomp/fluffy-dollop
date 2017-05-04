#include "viragraphicsplugin.h"
#include <dmanager/embeddedstruct.h>
#include <dmanager/embeddedapp.h>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <ctrcore/visual/viewinterface.h>
#include <ctrcore/visual/datainterface.h>
#include <ctrcore/visual/visualizermanager.h>
#include <ctrcore/visual/abstractscenewidget.h>
#include <ctrcore/provider/dataproviderfactory.h>
#include <ctrcore/provider/abstractdataprovider.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <QStackedWidget>
#include <QDebug>

ViraGraphicsPlugin::ViraGraphicsPlugin()
    : _pdfEditorForm(0)
{
}

ViraGraphicsPlugin::~ViraGraphicsPlugin()
{
    if(_pdfEditorForm)
        delete _pdfEditorForm;
}

void ViraGraphicsPlugin::init(uint visualizerId, quint64 visualizerWindowId)
{
    CtrAppVisualizerPlugin::init(visualizerId, visualizerWindowId);

    visualize_system::ViewInterface * viewInterface = visualize_system::VisualizerManager::instance()->getViewInterface(visualizerId);
    _stackedWidget = dynamic_cast<QStackedWidget*>(viewInterface->stackedWidget());

    QTimer::singleShot(1000, this, SLOT(launchWorkState()));
}
void ViraGraphicsPlugin::launchWorkState()
{
    _workState = QSharedPointer<WorkState>(new WorkState());
    connect(_workState.data(), SIGNAL(showFacility(qulonglong)), this, SLOT(showFacility(qulonglong)));
    connect(_workState.data(), SIGNAL(switchOnMap()), this, SLOT(switchOnMap()));
    connect(_workState.data(), SIGNAL(switchOnEditor()), this, SLOT(switchOnEditor()));
    connect(_workState.data(), SIGNAL(setMarkOnMap(qulonglong)), this, SLOT(setMarkOnMap(qulonglong)));
    visualize_system::VisualizerManager::instance()->getStateInterface(getVisualizerId())->setVisualizerState(_workState);

    _pdfEditorForm = new ViraEditorForm;
    connect(_pdfEditorForm, SIGNAL(switchOnMap()), this, SLOT(switchOnMap()));
    //connect(_workState.data(), SIGNAL(centerEditorOn(qulonglong)), _pdfEditorForm, SLOT(centerEditorOn(qulonglong)));

    int visualizerIndex = _stackedWidget->currentIndex();
    _stackedWidget->setProperty("visualizerIndex", visualizerIndex);
    int pdfEditorFormIndex = _stackedWidget->addWidget(_pdfEditorForm);
    _stackedWidget->setProperty("pdfEditorFormIndex", pdfEditorFormIndex);
    _stackedWidget->setCurrentIndex(visualizerIndex);
}

void ViraGraphicsPlugin::showFacility(qulonglong id)
{
    _stackedWidget->setCurrentIndex(_stackedWidget->property("pdfEditorFormIndex").toInt());
}

void ViraGraphicsPlugin::switchOnMap()
{
    _stackedWidget->setCurrentIndex(_stackedWidget->property("visualizerIndex").toInt());
}

void ViraGraphicsPlugin::switchOnEditor()
{
    _stackedWidget->setCurrentIndex(_stackedWidget->property("pdfEditorFormIndex").toInt());
}

void ViraGraphicsPlugin::setMarkOnMap(qulonglong id)
{
    if(id > 0)
    {
        _stackedWidget->setCurrentIndex(_stackedWidget->property("visualizerIndex").toInt());

        _setDefectState = QSharedPointer<ChoiceAreaState>(new ChoiceAreaState(ChoiceAreaState::POINT_OR_POLYGON, QCursor(QPixmap(":/img/cursor_mark.png"), 0, 0)));
        visualize_system::VisualizerManager::instance()->getStateInterface(getVisualizerId())->setVisualizerState(_setDefectState);
        connect(_setDefectState.data(), SIGNAL(signalAreaChoiced(QPolygonF)), this, SLOT(defectStateChoiced(QPolygonF)));
        connect(_setDefectState.data(), SIGNAL(signalAbort()), this, SLOT(defectStateAborted()));
    }
    else
    {
        if(_setDefectState)
        {
            _setDefectState->emit_closeState();
            _setDefectState.clear();
        }
    }

//    _setImageState = QSharedPointer<SetImageState>(new SetImageState());
//    visualize_system::VisualizerManager::instance()->getStateInterface(getVisualizerId())->setVisualizerState(_setImageState);
}

void ViraGraphicsPlugin::defectStateChoiced(QPolygonF pol)
{
    // create or change mark !
    if(_setDefectState)
    {
        _setDefectState->emit_closeState();
        _setDefectState.clear();
    }
    QList<QVariant>list;

    QVariant type(1);
    list.append(type);

    QVariant polygon;
    polygon.setValue(pol);
    list.append(polygon);

    CommonMessageNotifier::send( (uint)visualize_system::BusTags::MarkCreated, list, QString("visualize_system"));
}

void ViraGraphicsPlugin::defectStateAborted()
{
    if(_setDefectState)
    {
        _setDefectState->emit_closeState();
        _setDefectState.clear();
    }
    QList<QVariant>list;
    CommonMessageNotifier::send( (uint)visualize_system::BusTags::MarkCreated, list, QString("visualize_system"));
}

QList<InitPluginData> ViraGraphicsPlugin::getInitPluginData()
{
    InitPluginData initDataSave;
    initDataSave.buttonName = QString("ViraGraphicsPlugin");
    initDataSave.translateButtonName = QString::fromUtf8("Switch base cover");
    initDataSave.iconForButtonOn = QIcon(":/img/119_icons_32_gl.png");
    initDataSave.isCheckable = false;
    initDataSave.tooltip = QString::fromUtf8("Switch base cover");

    return QList<InitPluginData>() << initDataSave;
}

bool ViraGraphicsPlugin::isChecked(const QString&)
{
    return false;
}

void ViraGraphicsPlugin::checked(const QString &buttonName, bool on_off)
{
    if(buttonName == QString("ViraGraphicsPlugin"))
    {
        visualize_system::DataInterface * dataInterface = visualize_system::VisualizerManager::instance()->getDataInterface(getVisualizerId());
        QList<uint>providers = dataInterface->getProviders();
        if(providers.size() == 2)
        {
            dataInterface->setProviderViewProperty(providers.at(0), "visibility", ! _showFirstBaseCover);
            dataInterface->setProviderViewProperty(providers.at(1), "visibility", _showFirstBaseCover);
        }
        _showFirstBaseCover = ! _showFirstBaseCover;
    }
}



































