#include "pdfimportplugin.h"
#include <libembeddedwidgets/embeddedstruct.h>
#include <libembeddedwidgets/embeddedapp.h>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <ctrcore/visual/viewinterface.h>
#include <ctrcore/visual/datainterface.h>
#include <ctrcore/visual/visualizermanager.h>
#include <ctrcore/visual/abstractscenewidget.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>
#include <QDebug>

PdfImportPlugin::PdfImportPlugin()
    : _isChecked(false)
    , _pdfEditorForm(0)
    , _iface(0)
{
}

PdfImportPlugin::~PdfImportPlugin()
{
    if(_pdfEditorForm)
        delete _pdfEditorForm;
}

void PdfImportPlugin::init(uint visualizerId, quint64 visualizerWindowId)
{
    CtrAppVisualizerPlugin::init(visualizerId, visualizerWindowId);
}

QList<InitPluginData> PdfImportPlugin::getInitPluginData()
{
    InitPluginData initDataSave;
    initDataSave.buttonName = QString("PdfImportPlugin");
    initDataSave.translateButtonName = QString::fromUtf8("Импорт PDF-файла");
    initDataSave.iconForButtonOn = QIcon("://img/061_icons_32_tools_pdf.png");
    initDataSave.isCheckable = true;
    initDataSave.tooltip = QString::fromUtf8("Импорт PDF-файла");

    return QList<InitPluginData>() << initDataSave;
}

bool PdfImportPlugin::isChecked(const QString&)
{
    return _isChecked;
}

void PdfImportPlugin::checked(const QString &buttonName, bool on_off)
{
    if(buttonName == QString("PdfImportPlugin"))
    {
        _isChecked = on_off;
        if(on_off)
        {
            loadPDF();
        }
        else
        {
            ewApp()->setVisible(_iface->id(), false);
            if(_pdfEditorForm)
            {
                delete _pdfEditorForm;
                _pdfEditorForm = 0;
            }
            if(_iface)
            {
                delete _iface;
                _iface = 0;
            }
        }
    }
}

void PdfImportPlugin::loadPDF()
{
    visualize_system::ViewInterface * inter = visualize_system::VisualizerManager::instance()->getViewInterface(getVisualizerId());
    QString pdfFilePath = QFileDialog::getOpenFileName(inter ? inter->getAbstractSceneWidget() : 0,
                                                QString::fromUtf8("Выберите PDF-файл для импорта"),
                                                QString(""),
                                                QString("*.pdf *.PDF"));
    if(pdfFilePath.isEmpty())
    {
        emit setChecked(QString("PdfImportPlugin"), false);
    }
    else
    {
        if( ! _pdfEditorForm)
        {
            _pdfEditorForm = new PdfEditorForm(pdfFilePath);
            _iface = new EmbIFaceNotifier(_pdfEditorForm);
            QString tag = QString("PdfImportPluginForm");
            quint64 widgetId = ewApp()->restoreWindow(tag, _iface);
            if(0 == widgetId)
            {
                ew::EmbeddedWindowStruct struc;
                ew::EmbeddedHeaderStruct headStr;
                headStr.hasCloseButton = true;
                headStr.hasMinMaxButton = true;
                headStr.hasCollapseButton = true;
                headStr.headerPixmap = ":/img/061_icons_32_tools_pdf.png";
                headStr.windowTitle = QString::fromUtf8("Импорт PDF-файла");
                struc.header = headStr;
                struc.iface = _iface;
                struc.widgetTag = tag;
                struc.minSize = QSize(300,300);
                ewApp()->createWindow(struc);
            }
            connect(_iface, SIGNAL(signalClosed()), this, SLOT(slotEditorClosed()));
            // _pdfEditorForm->setParentWindowId(_iface->id());
        }
        ewApp()->setVisible(_iface->id(), true);
    }
}

void PdfImportPlugin::slotEditorClosed()
{
    emit setChecked(QString("PdfImportPlugin"), false);
}


Q_EXPORT_PLUGIN2(PdfImportPlugin, PdfImportPluginInterface)
































