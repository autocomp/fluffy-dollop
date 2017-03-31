#include "rastercontrolwidget.h"
#include <QHBoxLayout>
#include <QCheckBox>
#include <QFileInfo>
#include <QCursor>
#include "parallelcursor.h"
#include <ctrcore/provider/dataproviderfactory.h>
#include <ctrcore/provider/rasterdataprovider.h>
#include <ctrcore/visual/visualizermanager.h>
#include <ctrcore/visual/datainterface.h>
#include <ctrcore/visual/viewinterface.h>
#include <ctrcore/visual/selectionmodelinterface.h>
#include <dmanager/embeddedapp.h>

using namespace visualize_system;

RasterControlWidget::RasterControlWidget(uint visualizerId, QList<uint> rasters, ParallelCursor *parallelCursor)
    : _visualizerId(visualizerId)
    , _rasters(rasters)
    , _parallelCursor(parallelCursor)
    , _currentRaster(_rasters.size() - 1)
{
    QHBoxLayout * hbLayout = new QHBoxLayout(this);

    _tbDown = new QToolButton(this);
    connect(_tbDown, SIGNAL(clicked()), this, SLOT(slotDown()));
    _tbDown->setIcon(QIcon(":/img/ctrvisual_arrowleft.png"));
//    _tbDown->setToolTip(QString::fromUtf8("Вниз"));
    _tbDown->setFixedSize(32,32);
    _tbDown->setIconSize(QSize(32,32));
    _tbDown->setAutoRaise(true);
    hbLayout->addWidget(_tbDown);

    _tlInfo = new QLabel(this);
    hbLayout->addWidget(_tlInfo);

    _tbDelete = new QToolButton(this);
    connect(_tbDelete, SIGNAL(clicked()), this, SLOT(slotDelete()));
    _tbDelete->setIcon(QIcon(":/img/ctrvisual_close_or_delete.png"));
    _tbDelete->setToolTip(QString::fromUtf8("Удалить"));
    _tbDelete->setFixedSize(32,32);
    _tbDelete->setIconSize(QSize(32,32));
    _tbDelete->setAutoRaise(true);
    hbLayout->addWidget(_tbDelete);

    _tbMark = new QToolButton(this);
    connect(_tbMark, SIGNAL(clicked(bool)), this, SLOT(slotSetMark(bool)));
    _tbMark->setIcon(QIcon(":/img/ctrvisual_check.png"));
    _tbMark->setToolTip(QString::fromUtf8("Пометить"));
    _tbMark->setFixedSize(32,32);
    _tbMark->setIconSize(QSize(32,32));
    _tbMark->setAutoRaise(true);
    _tbMark->setCheckable(true);
    hbLayout->addWidget(_tbMark);

    _tbUp = new QToolButton(this);
    connect(_tbUp, SIGNAL(clicked()), this, SLOT(slotUp()));
    _tbUp->setIcon(QIcon(":/img/ctrvisual_arrowright.png"));
//    _tbUp->setToolTip(QString::fromUtf8("Вверх"));
    _tbUp->setFixedSize(32,32);
    _tbUp->setIconSize(QSize(32,32));
    _tbUp->setAutoRaise(true);
    hbLayout->addWidget(_tbUp);

    quint64 visualizerWindowId(0);
    visualize_system::ViewInterface * interface = visualize_system::VisualizerManager::instance()->getViewInterface(visualizerId);
    if(interface)
        visualizerWindowId = interface->getVisualizerWindowId();

    _iface = new EmbIFaceNotifier(this);
    QString tag = QString("RasterControlWidget");
    quint64 widgetId = ewApp()->restoreWidget(tag, _iface);
    if(0 == widgetId)
    {
        ew::EmbeddedWidgetStruct struc;
        ew::EmbeddedHeaderStruct headStr;
        headStr.hasCloseButton = true;
        headStr.windowTitle = QString::fromUtf8("");
        struc.header = headStr;
        struc.iface = _iface;
        struc.widgetTag = tag;
        struc.minSize = QSize(200,12);
        struc.topOnHint = true;
        ewApp()->createWidget(struc, visualizerWindowId);
    }
    ewApp()->setVisible(_iface->id(), true);
    connect(_iface, SIGNAL(signalClosed()), this, SLOT(widgetClose()));

    check();
    update();
}

RasterControlWidget::~RasterControlWidget()
{
    delete _parallelCursor;
}

void RasterControlWidget::check()
{
    if(_rasters.isEmpty())
    {
        ewApp()->setVisible(_iface->id(), false);
        emit signalRasterControlWidgetClosed();
        return;
    }

    if(_currentRaster >= _rasters.size())
        _currentRaster = _rasters.size() - 1;

    _tlInfo->setText( QString::number(_currentRaster + 1) + QString::fromUtf8(" из ") + QString::number(_rasters.size()) );

    _tbDown->setEnabled(_currentRaster > 0);
    _tbUp->setEnabled(_currentRaster < _rasters.size() - 1);

    uint providerId = _rasters.at(_currentRaster);
    QSharedPointer<data_system::RasterDataProvider> rdp = data_system::DataProviderFactory::instance()->getProvider(providerId).dynamicCast<data_system::RasterDataProvider>();
    if(rdp)
    {
        QFileInfo fi(rdp->getProviderUrl().toLocalFile());
        ewApp()->setWidgetTitle(_iface->id(), fi.fileName());
    }

    visualize_system::SelectionModelInterface * selectionModelInterface = visualize_system::VisualizerManager::instance()->getSelectionModelInterface(_visualizerId);
    if(selectionModelInterface)
    {
        QList<uint> providers;
        providers.append(providerId);
        selectionModelInterface->clearSelectProviders();
        selectionModelInterface->setSelectProviders(true, providers);
        selectionModelInterface->setCurrentProvider(providerId);

        bool checked = selectionModelInterface->getMarkedProviders().contains(providerId);
        _tbMark->blockSignals(true);
        _tbMark->setChecked(checked);
        _tbMark->blockSignals(false);
    }
}

void RasterControlWidget::slotDown()
{
    if(_currentRaster > 0)
    {
        uint hideRaser = _rasters.at(_currentRaster);
        --_currentRaster;
        uint showRaser = _rasters.at(_currentRaster);
        emit signalSetVisibleForRaster(showRaser, hideRaser);
        check();
    }
}

void RasterControlWidget::slotUp()
{
    if(_currentRaster < _rasters.size())
    {
        uint hideRaser = _rasters.at(_currentRaster);
        ++_currentRaster;
        uint showRaser = _rasters.at(_currentRaster);
        emit signalSetVisibleForRaster(showRaser, hideRaser);
        check();
    }
}

void RasterControlWidget::slotDelete()
{
    if(QMessageBox::question(this, QString::fromUtf8("Внимание"), QString::fromUtf8("Удалить изображение ?"), QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
    {
        visualize_system::DataInterface * dataInterface = visualize_system::VisualizerManager::instance()->getDataInterface(_visualizerId);
        if(dataInterface)
        {
            QList<uint> providers;
            providers.append(_rasters.at(_currentRaster));
            dataInterface->removeProviders(providers);

            _rasters.removeAt(_currentRaster);
            if(_rasters.isEmpty())
            {
                ewApp()->setVisible(_iface->id(), false);
                emit signalRasterControlWidgetClosed();
                return;
            }
            if(_currentRaster >= _rasters.size())
                _currentRaster = _rasters.size() - 1;

            uint showRaser = _rasters.at(_currentRaster);
            emit signalSetVisibleForRaster(showRaser, 0);

            check();
        }
    }
}

void RasterControlWidget::slotSetMark(bool on_off)
{
    visualize_system::SelectionModelInterface * selectionModelInterface = visualize_system::VisualizerManager::instance()->getSelectionModelInterface(_visualizerId);
    if(selectionModelInterface)
    {
        QList<uint> providers;
        uint providerId = _rasters.at(_currentRaster);
        providers.append(providerId);
        selectionModelInterface->setMarkProviders(on_off, providers);
    }
}

void RasterControlWidget::widgetClose()
{
    emit signalRasterControlWidgetClosed();
}


//-------------------------------------------------------


//void RasterControlWidget::slotSetTransparency(bool on_off)
//{
//    visualize_system::DataInterface * dataInterface = visualize_system::VisualizerManager::instance()->getDataInterface(_visualizerId);
//    if(dataInterface)
//        dataInterface->setProviderViewProperty(_rasters.at(_currentRaster), QString("transparency"), (on_off ? 50 : 0));
//}

//void RasterControlWidget::slotValueChanged(int currentRaster)
//{
//    uint hideRaser = ( (_currentRaster == currentRaster) ? 0 : _rasters.at(_currentRaster) );
//    _currentRaster = currentRaster;
//    uint showRaser = _rasters.at(_currentRaster);
//    emit signalSetVisibleForRaster(showRaser, hideRaser);
//}

















