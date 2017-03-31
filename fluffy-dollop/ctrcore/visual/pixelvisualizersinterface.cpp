#include "pixelvisualizersinterface.h"
#include "visualizermanager.h"
#include "datainterface.h"
#include "viewinterface.h"
#include <dmanager/embeddedstruct.h>
#include <dmanager/embeddedapp.h>
#include <ctrcore/provider/dataproviderfactory.h>
#include <ctrcore/provider/rasterdataprovider.h>
#include <QFileInfo>

using namespace visualize_system;

PixelVisualizersInterface::PixelVisualizersInterface()
{
}

PixelVisualizersInterface::~PixelVisualizersInterface()
{
    for(auto it = _pixelVisualizrers.begin(); it != _pixelVisualizrers.end(); ++it)
    {
        EmbIFaceNotifier * _iface = it.value();
        bool ok(false);
        uint visualizerId = _iface->property("visualizerId").toUInt(&ok);
        if(ok && visualizerId > 0)
        {
            visualize_system::VisualizerManager::instance()->removeVisualizer(visualizerId);
        }

        uint providerId = _iface->property("providerId").toUInt(&ok);
        if(ok && providerId > 0)
        {
            auto it = _pixelVisualizrers.find(providerId);
            if(it != _pixelVisualizrers.end())
            {
                ewApp()->removeWidget(_iface->id());
                delete _iface;
                _iface = 0;

                _pixelVisualizrers.erase(it);
            }
        }
    }
    _pixelVisualizrers.clear();
}

uint PixelVisualizersInterface::addVisualizer(uint providerId, bool checkProviderOpenedInPixelVis)
{
    uint visualizerId(0);

    if(checkProviderOpenedInPixelVis)
    {
        auto it = _pixelVisualizrers.find(providerId);
        if(it != _pixelVisualizrers.end())
        {
            EmbIFaceNotifier * _iface = it.value();
            bool ok(false);
            visualizerId = _iface->property("visualizerId").toUInt(&ok);
            if(ok && visualizerId > 0)
            {
                return visualizerId;
            }
            return 0;
        }
    }

    QSharedPointer<data_system::RasterDataProvider> rdp = data_system::DataProviderFactory::instance()->getProvider(providerId).dynamicCast<data_system::RasterDataProvider>();
    if(rdp)
    {
        const QString filePath(rdp->getProviderUrl().toLocalFile());

        QList<uint> providers;
        providers.append(providerId);

        visualizerId = visualize_system::VisualizerManager::instance()->addVisualizer(visualize_system::VisualizerPixel);
        if(visualizerId > 0)
        {
            visualize_system::DataInterface * dataInterface = visualize_system::VisualizerManager::instance()->getDataInterface(visualizerId);
            if(dataInterface)
            {
                dataInterface->addProviders(providers);
            }

            visualize_system::ViewInterface * viewInterface = visualize_system::VisualizerManager::instance()->getViewInterface(visualizerId);
            if(viewInterface)
            {
                QWidget * pixelVisWidget = viewInterface->widget();
                if(pixelVisWidget)
                {
                    EmbIFaceNotifier * _iface = new EmbIFaceNotifier(pixelVisWidget);
                    _iface->setProperty("providerId", providerId);
                    _iface->setProperty("visualizerId", visualizerId);
                    QString tag = filePath;
                    quint64 widgetId = ewApp()->restoreWindow(tag, _iface);
                    if(0 == widgetId)
                    {
                        ew::EmbeddedWindowStruct struc;
                        ew::EmbeddedHeaderStruct headStr;
                        headStr.hasCloseButton = true;
                        headStr.headerPixmap = ":/img/008_icons_42_img.png";
                        headStr.windowTitle = QFileInfo(filePath).fileName();
                        headStr.hasCollapseButton = true;
                        headStr.hasMinMaxButton = true;
                        struc.header = headStr;
                        struc.iface = _iface;
                        struc.widgetTag = tag;
                        struc.minSize = QSize(300,200);
                        struc.size = QSize(350,350);
                        widgetId = ewApp()->createWindow(struc);
                    }
                    viewInterface->setVisualizerWindowId(widgetId);

                    ewApp()->setVisible(_iface->id(), true);
                    connect(_iface, SIGNAL(signalClosed()), this, SLOT(slotPixelVisualizerClosed()));

                    _pixelVisualizrers.insert(providerId, _iface);
                }
            }
        }
    }
    return visualizerId;
}

bool PixelVisualizersInterface::removeVisualizerById(uint _visualizerId)
{
    for(auto it = _pixelVisualizrers.begin(); it != _pixelVisualizrers.end(); ++it)
    {
        EmbIFaceNotifier * _iface = it.value();
        bool ok(false);
        uint visualizerId = _iface->property("visualizerId").toUInt(&ok);
        if(ok && visualizerId > 0)
        {
            if(_visualizerId == visualizerId)
            {
                visualize_system::VisualizerManager::instance()->removeVisualizer(visualizerId);

                uint providerId = _iface->property("providerId").toUInt(&ok);
                if(ok && providerId > 0)
                {
                    auto it = _pixelVisualizrers.find(providerId);
                    if(it != _pixelVisualizrers.end())
                    {
                        ewApp()->removeWidget(_iface->id());
                        delete _iface;
                        _iface = 0;

                        _pixelVisualizrers.erase(it);
                    }
                }
                _pixelVisualizrers.erase(it);
                return true;
            }
        }
    }
    return false;
}

bool PixelVisualizersInterface::removeVisualizerByProviderId(uint providerId)
{
    auto it = _pixelVisualizrers.find(providerId);
    if(it != _pixelVisualizrers.end())
    {
        EmbIFaceNotifier * _iface = dynamic_cast<EmbIFaceNotifier*>(sender());
        bool ok(false);
        uint visualizerId = _iface->property("visualizerId").toUInt(&ok);
        if(ok && visualizerId > 0)
        {
            visualize_system::VisualizerManager::instance()->removeVisualizer(visualizerId);
        }

        uint providerId = _iface->property("providerId").toUInt(&ok);
        if(ok && providerId > 0)
        {
            auto it = _pixelVisualizrers.find(providerId);
            if(it != _pixelVisualizrers.end())
            {
                ewApp()->removeWidget(_iface->id());
                delete _iface;
                _iface = 0;

                _pixelVisualizrers.erase(it);
            }
        }
        return true;
    }
    return false;
}

bool PixelVisualizersInterface::setWindowTitle(uint visualizerId, const QString &text)
{
    for(auto it = _pixelVisualizrers.begin(); it != _pixelVisualizrers.end(); ++it)
    {
        EmbIFaceNotifier * _iface = it.value();
        bool ok(false);
        uint currVisualizerId = _iface->property("visualizerId").toUInt(&ok);
        if(ok && currVisualizerId > 0)
            if(currVisualizerId == visualizerId)
            {
                ewApp()->setWidgetTitle(_iface->id(), text);
                return true;
            }
    }
    return false;
}

void PixelVisualizersInterface::slotPixelVisualizerClosed()
{
    EmbIFaceNotifier * _iface = dynamic_cast<EmbIFaceNotifier*>(sender());
    if(_iface)
    {
        bool ok(false);
        uint visualizerId = _iface->property("visualizerId").toUInt(&ok);
        if(ok && visualizerId > 0)
        {
            visualize_system::VisualizerManager::instance()->removeVisualizer(visualizerId);
        }

        uint providerId = _iface->property("providerId").toUInt(&ok);
        if(ok && providerId > 0)
        {
            auto it = _pixelVisualizrers.find(providerId);
            if(it != _pixelVisualizrers.end())
            {
                ewApp()->removeWidget(_iface->id());
                delete _iface;
                _iface = 0;

                _pixelVisualizrers.erase(it);
            }
        }
    }
}



























