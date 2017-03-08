#include "viragraphicsplugin.h"
#include <libembeddedwidgets/embeddedstruct.h>
#include <libembeddedwidgets/embeddedapp.h>
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
    visualize_system::VisualizerManager::instance()->getStateInterface(getVisualizerId())->setVisualizerState(_workState);

    _pdfEditorForm = new ViraEditorForm;
    connect(_pdfEditorForm, SIGNAL(switchOnMap()), this, SLOT(switchOnMap()));
    connect(_workState.data(), SIGNAL(centerEditorOn(qulonglong)), _pdfEditorForm, SLOT(centerEditorOn(qulonglong)));

    int visualizerIndex = _stackedWidget->currentIndex();
    _stackedWidget->setProperty("visualizerIndex", visualizerIndex);
    int pdfEditorFormIndex = _stackedWidget->addWidget(_pdfEditorForm);
    _stackedWidget->setProperty("pdfEditorFormIndex", pdfEditorFormIndex);
    _stackedWidget->setCurrentIndex(visualizerIndex);
}

void ViraGraphicsPlugin::showFacility(qulonglong id)
{
    _pdfEditorForm->reinit(id);
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



































