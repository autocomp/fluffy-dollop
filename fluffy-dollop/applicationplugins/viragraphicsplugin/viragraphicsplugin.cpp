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
    QString xmlFilePath;
    QVariant xmlFilePath_Var = CtrConfig::getValueByName(QString("application_settings.launchXml_Path"));
    if(xmlFilePath_Var.isValid())
        xmlFilePath = xmlFilePath_Var.toString();

    if(QFile::exists(xmlFilePath))
    {
        _workState = QSharedPointer<WorkState>(new WorkState(xmlFilePath));
        connect(_workState.data(), SIGNAL(showFacility(qulonglong)), this, SLOT(showFacility(qulonglong)));
        visualize_system::VisualizerManager::instance()->getStateInterface(getVisualizerId())->setVisualizerState(_workState);
    }
    else
    {
        QMessageBox::critical(0, "Ahtung", "You must set launchXml_Path to application_settings.xml !");
    }
}

void ViraGraphicsPlugin::showFacility(qulonglong id)
{
    if(_pdfEditorForm)
    {
        _pdfEditorForm->reinit(id); //xmlFilePath);
        _stackedWidget->setCurrentIndex(_stackedWidget->property("pdfEditorFormIndex").toInt());
    }
    else
    {
        _pdfEditorForm = new ViraEditorForm(id);//xmlFilePath);
        connect(_pdfEditorForm, SIGNAL(switchOnMap()), this, SLOT(switchOnMap()));

        int visualizerIndex = _stackedWidget->currentIndex();
        _stackedWidget->setProperty("visualizerIndex", visualizerIndex);
        int pdfEditorFormIndex = _stackedWidget->addWidget(_pdfEditorForm);
        _stackedWidget->setProperty("pdfEditorFormIndex", pdfEditorFormIndex);
        _stackedWidget->setCurrentIndex(pdfEditorFormIndex);

//        _iface = new EmbIFaceNotifier(_pdfEditorForm);
//        QString tag = QString("ViraGraphicsPluginForm");
//        quint64 widgetId = ewApp()->restoreWindow(tag, _iface);
//        if(0 == widgetId)
//        {
//            ew::EmbeddedWindowStruct struc;
//            ew::EmbeddedHeaderStruct headStr;
//            headStr.hasCloseButton = true;
//            headStr.hasMinMaxButton = true;
//            headStr.hasCollapseButton = true;
//            headStr.headerPixmap = ":/img/061_icons_32_tools_pdf.png";
//            headStr.windowTitle = QString::fromUtf8("Импорт PDF-файла");
//            struc.header = headStr;
//            struc.iface = _iface;
//            struc.widgetTag = tag;
//            struc.minSize = QSize(300,300);
//            ewApp()->createWindow(struc);
//        }
//        connect(_iface, SIGNAL(signalClosed()), this, SLOT(slotEditorClosed()));
//        _pdfEditorForm->setParentWindowId(_iface->id());
    }
    //ewApp()->setVisible(_iface->id(), true);
}

void ViraGraphicsPlugin::switchOnMap()
{
    _stackedWidget->setCurrentIndex(_stackedWidget->property("visualizerIndex").toInt());
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

//        dataInterface->removeProviders(providers);

//        QVariant baseTmsLayer_Url = CtrConfig::getValueByName(_showFirstBaseCover ? QString("application_settings.baseTmsLayer2_Url") : QString("application_settings.baseTmsLayer1_Url"));
//        if(baseTmsLayer_Url.isValid())
//        {
//            QUrl url(baseTmsLayer_Url.toString());
//            QSharedPointer<data_system::AbstractDataProvider>dp = data_system::DataProviderFactory::instance()->createProvider(url, QString("tms"));
//            if(dp)
//            {
//                dp->open(url);
//                dataInterface->addBaseProviders(QList<uint>() << dp->getProviderId());
//            }
//        }
        _showFirstBaseCover = ! _showFirstBaseCover;

//        _isChecked = on_off;
//        if(on_off)
//        {
//            startEditor();
//        }
//        else
//        {
//            ewApp()->setVisible(_iface->id(), false);
//            if(_pdfEditorForm)
//            {
//                delete _pdfEditorForm;
//                _pdfEditorForm = 0;
//            }
//            if(_iface)
//            {
//                delete _iface;
//                _iface = 0;
//            }
//        }
    }
}

void ViraGraphicsPlugin::startEditor()
{
//    visualize_system::ViewInterface * inter = visualize_system::VisualizerManager::instance()->getViewInterface(getVisualizerId());
//    QString path = QFileDialog::getOpenFileName(inter ? inter->getAbstractSceneWidget() : 0,
//                                                QString::fromUtf8("Выберите PDF-файл для импорта"),
//                                                QString(""),
//                                                QString("facility.xml"));
//    if(path.isEmpty())
//    {
//        emit setChecked(QString("ViraGraphicsPlugin"), false);
//    }
//    else
//    {
//        if( ! _pdfEditorForm)
//        {
//            _pdfEditorForm = new PdfEditorForm(path);
//            _iface = new EmbIFaceNotifier(_pdfEditorForm);
//            QString tag = QString("ViraGraphicsPluginForm");
//            quint64 widgetId = ewApp()->restoreWindow(tag, _iface);
//            if(0 == widgetId)
//            {
//                ew::EmbeddedWindowStruct struc;
//                ew::EmbeddedHeaderStruct headStr;
//                headStr.hasCloseButton = true;
//                headStr.hasMinMaxButton = true;
//                headStr.hasCollapseButton = true;
//                headStr.headerPixmap = ":/img/061_icons_32_tools_pdf.png";
//                headStr.windowTitle = QString::fromUtf8("Импорт PDF-файла");
//                struc.header = headStr;
//                struc.iface = _iface;
//                struc.widgetTag = tag;
//                struc.minSize = QSize(300,300);
//                ewApp()->createWindow(struc);
//            }
//            connect(_iface, SIGNAL(signalClosed()), this, SLOT(slotEditorClosed()));
//            _pdfEditorForm->setParentWindowId(_iface->id());
//        }
//        ewApp()->setVisible(_iface->id(), true);
//    }
}

void ViraGraphicsPlugin::slotEditorClosed()
{
//    emit setChecked(QString("ViraGraphicsPlugin"), false);
}


//Q_EXPORT_PLUGIN2(ViraGraphicsPlugin, ViraGraphicsPluginInterface)
































