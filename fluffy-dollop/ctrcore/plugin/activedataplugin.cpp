#include "activedataplugin.h"
#include <ctrcore/visual/visualizermanager.h>
#include <ctrcore/visual/selectionmodelinterface.h>
#include <ctrcore/visual/datainterface.h>
#include <ctrcore/provider/dataproviderfactory.h>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <dmanager/embeddedstruct.h>
#include <dmanager/embeddedapp.h>
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>

#include <QSharedPointer>
#include <QLabel>
#include <QVBoxLayout>
#include <QFileInfo>
#include <QCheckBox>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>

ActiveDataPlugin::ActiveDataPlugin()
    : _activeProvider(0)
    , _widget(0)
    , _iface(0)
    , _isChecked(false)
    , _applyForImage(0)
    , _prevCheck(false)
{
}

ActiveDataPlugin::~ActiveDataPlugin()
{
    delete _widget;
}

QList<InitPluginData> ActiveDataPlugin::getInitPluginData()
{
    InitPluginData initData;
    initData.buttonName = getSystemName();
    initData.translateButtonName = getUserName();
    initData.iconForButtonOn = getIconOn();
    initData.iconForButtonOff = getIconOff();
    initData.isCheckable = true;
    initData.tooltip = getToolTip();

    return QList<InitPluginData>() << initData;
}

bool ActiveDataPlugin::isChecked(const QString & buttonName)
{
    return (getSystemName() == buttonName) ? _isChecked : false;
}

void ActiveDataPlugin::checked(const QString & buttonName, bool on_off)
{
    if(getSystemName() == buttonName)
    {
        _isChecked = on_off;
        if(on_off)
        {
            visualize_system::SelectionModelInterface * selInt = visualize_system::VisualizerManager::instance()->getSelectionModelInterface(getVisualizerId());
            if(selInt)
            {
                slotCurrentProviderChanged(selInt->getCurrentProvider(), 0);
            }
            if(_iface)
            {
                qDebug() << "ActiveDataPlugin::checked " << _iface->id() << " " << buttonName;
                ewApp()->setVisible(_iface->id(), true);
            }
        }
        else
        {
            _activeProvider = 0;
            slotCurrentProviderChanged(0, 0);
            if(_iface)
                ewApp()->setVisible(_iface->id(), false);
        }
    }
}

void ActiveDataPlugin::init(uint visualizerId, quint64 visualizerWindowId)
{
    CtrAppVisualizerPlugin::init(visualizerId, visualizerWindowId);

    visualize_system::SelectionModelInterface * selInt = visualize_system::VisualizerManager::instance()->getSelectionModelInterface(visualizerId);
    if(selInt)
    {
        connect(selInt, SIGNAL(signalCurrentProviderChanged(uint,uint)), this, SLOT(slotCurrentProviderChanged(uint,uint)));
    }

    visualize_system::DataInterface * dataInt = visualize_system::VisualizerManager::instance()->getDataInterface(getVisualizerId());
    if(dataInt)
        connect(dataInt, SIGNAL(signalProvidersRemoved(QList<uint>)), this, SLOT(slotProvidersRemoved(QList<uint>)));

    QWidget * processingWidget = getProcessingWidget();

    if(processingWidget)
    {
        _widget = new QWidget;

        QVBoxLayout * vLayout = new QVBoxLayout(_widget);
        vLayout->setContentsMargins(0,0,0,0);

        _fileNameTL = new QLabel(_widget);
        vLayout->addWidget(_fileNameTL, 0, Qt::AlignLeft);

        _topFrame = new QFrame(_widget);
        _topFrame->setFrameShape(QFrame::HLine);
        _topFrame->setFrameShadow(QFrame::Sunken );
        vLayout->addWidget(_topFrame);

        QSpacerItem * spacer = new QSpacerItem(0, 5, QSizePolicy::Minimum, QSizePolicy::Preferred);
        vLayout->addSpacerItem(spacer);

        _applyRuntime = new QCheckBox(tr("Apply"), _widget);
        connect(_applyRuntime, SIGNAL(toggled(bool)), this, SLOT(slotApplyRuntime(bool)));
        vLayout->addWidget(_applyRuntime);

        spacer = new QSpacerItem(0, 5, QSizePolicy::Minimum, QSizePolicy::Preferred);
        vLayout->addSpacerItem(spacer);

        vLayout->addWidget(processingWidget, 10);

        spacer = new QSpacerItem(0, 5, QSizePolicy::Minimum, QSizePolicy::Preferred);
        vLayout->addSpacerItem(spacer);

        _bottomFrame = new QFrame(_widget);
        _bottomFrame->setFrameShape(QFrame::HLine);
        _bottomFrame->setFrameShadow(QFrame::Sunken );
        vLayout->addWidget(_bottomFrame);
        _bottomFrame->hide();

        //    spacer = new QSpacerItem(0, 500, QSizePolicy::Minimum, QSizePolicy::Preferred);
        //    vLayout->addSpacerItem(spacer);

        QSize size(32,32);
        _applyForImage = new QToolButton;
        _applyForImage->setFixedSize(size);
        _applyForImage->setIconSize(size);
        _applyForImage->setAutoRaise(true);
        _applyForImage->setToolTip(tr("Start processing for whole image"));
        _applyForImage->setIcon(QIcon(":/img/start_processing.png"));
        connect(_applyForImage, SIGNAL(clicked()), this, SLOT(slotApplyForImage()));
        vLayout->addWidget(_applyForImage, -1, Qt::AlignCenter);
        _applyForImage->hide();

        _iface = new EmbIFaceNotifier(_widget);
        quint64 widgetId = ewApp()->restoreWidget(getSystemName(), _iface);
        if(0 == widgetId)
        {
            ew::EmbeddedHeaderStruct headStr;
            headStr.hasCloseButton = true;
            headStr.windowTitle = getUserName();
            headStr.headerPixmap = getIconOnPath();
            ew::EmbeddedWidgetStruct struc;
            struc.widgetTag = getSystemName();
            struc.header = headStr;
            struc.addHided = true;
            struc.iface = _iface;
            struc.minSize = getMinimumWidgetSize();
            ewApp()->createWidget(struc, visualizerWindowId);
        }
        connect(_iface, SIGNAL(signalClosed()), this, SLOT(slotWidgetClosed()));
    }

    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::DisableComponent, this, SLOT(slotDisableWidget(QVariant)),
                                      qMetaTypeId< QString >(),
                                      QString("visualize_system") );
    CommonMessageNotifier::subscribe( (uint)visualize_system::BusTags::EnableComponent, this, SLOT(slotEnabledWidget(QVariant)),
                                      qMetaTypeId< QString >(),
                                      QString("visualize_system") );
}

uint ActiveDataPlugin::getActiveProvider()
{
    return _activeProvider;
}


void ActiveDataPlugin::slotCurrentProviderChanged(uint curr, uint prev)
{
    if(_widget)
        if(_isChecked)
        {
            if(curr > 0)
            {
                _activeProvider = curr;
                _widget->setDisabled(false);
                _fileNameTL->setText("");

                QSharedPointer<data_system::RasterDataProvider>rdp = data_system::DataProviderFactory::instance()->getProvider(_activeProvider).dynamicCast<data_system::RasterDataProvider>();
                if(rdp.isNull() == false)
                {
                    QFileInfo fi(rdp->getProviderUrl().toLocalFile());
                    _fileNameTL->setText(fi.fileName());
                }
            }
            else
            {
                _activeProvider = 0;
                _widget->setDisabled(true);
                _fileNameTL->setText("");
            }

            providerActivated(_activeProvider);
        }
}

void ActiveDataPlugin::slotWidgetClosed()
{
    if(_isChecked)
    {
        _isChecked = false;
        _activeProvider = 0;
        emit setChecked(getSystemName(), false);
    }
}

void ActiveDataPlugin::slotProvidersRemoved(QList<uint> ids)
{
    if(_activeProvider > 0 && ids.contains(_activeProvider))
    {
        slotCurrentProviderChanged(0, 0);
    }
}

QString ActiveDataPlugin::getSavePath(const QString& filePath)
{
    QString tempFileDir;
    QVariant tempFileDir_Path = CtrConfig::getValueByName(QString("application_settings.tempFileDir_Path"));
    if(tempFileDir_Path.isValid())
    {
        tempFileDir = tempFileDir_Path.toString();

        QDir dir(tempFileDir);
        QString dest_dir = QDateTime::currentDateTime().toString("dd_MM_yy__hh_mm_ss_z");
        dir.mkdir(dest_dir);

        QFileInfo fi(filePath);
        QString savePath = tempFileDir + QDir::separator() + dest_dir + QDir::separator() + fi.baseName() + QString(".tif");

        return savePath;
    }
    else
    {
        QMessageBox::critical(0, tr("Attention"), tr("You haven't set path for directory with temporary files!"), QMessageBox::Close);
        return QString();
    }
}

QSize ActiveDataPlugin::getMinimumWidgetSize()
{
    return QSize(50,25);
}

QSize ActiveDataPlugin::getMaximumWidgetSize()
{
    return QSize(400,800);
}

void ActiveDataPlugin::slotDisableWidget(QVariant var)
{
    if(_isChecked)
    {
        _prevCheck = true;
        emit setChecked(getSystemName(), false);
    }
    emit setEnabled(getSystemName(), false);
}

void ActiveDataPlugin::slotEnabledWidget(QVariant var)
{
    if(_prevCheck)
    {
        _prevCheck = false;
        ewApp()->setVisible(_iface->id(), true);
        emit setChecked(getSystemName(), true);
    }
    emit setEnabled(getSystemName(), true);
}









