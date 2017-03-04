#include "videovisualizersinterface.h"
#include "visualizermanager.h"
#include "datainterface.h"
#include "viewinterface.h"
#include <libembeddedwidgets/embeddedstruct.h>
#include <libembeddedwidgets/embeddedapp.h>
#include <ctrcore/provider/dataproviderfactory.h>
#include <ctrcore/provider/abstractvideodataprovider.h>
#include <QFileInfo>

using namespace visualize_system;

VideoVisualizersInterface::VideoVisualizersInterface()
{
}

VideoVisualizersInterface::~VideoVisualizersInterface()
{
    for(auto it = _videoVisualizrers.begin(); it != _videoVisualizrers.end(); ++it)
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
            auto it = _videoVisualizrers.find(providerId);
            if(it != _videoVisualizrers.end())
            {
                ewApp()->removeWidget(_iface->id());
                delete _iface;
                _iface = 0;

                _videoVisualizrers.erase(it);
            }
        }
    }
    _videoVisualizrers.clear();
}

uint VideoVisualizersInterface::addVisualizer(uint providerId)
{
    uint visualizerId(0);
    auto it = _videoVisualizrers.find(providerId);
    if(it != _videoVisualizrers.end())
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

    QSharedPointer<data_system::AbstractVideoDataProvider> rdp = data_system::DataProviderFactory::instance()->getProvider(providerId).dynamicCast<data_system::AbstractVideoDataProvider>();
    if(rdp)
    {
        this->providerId = providerId;
        const QString filePath(rdp->getProviderUrl().toLocalFile());

        QList<uint> providers;
        providers.append(providerId);

        visualizerId = visualize_system::VisualizerManager::instance()->addVisualizer(visualize_system::VisualizerVideo);
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
                    quint64 widgetId = ewApp()->restoreWidget(tag, _iface);
                    if(0 == widgetId)
                    {
                        ew::EmbeddedWidgetStruct struc;
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
                        widgetId = ewApp()->createWidget(struc);
                    }
                    ///viewInterface->setVisualizerWindowId(widgetId);/// МОжно заменить на подходящий по смыслу метод
                    viewInterface->setVisualizerWindowId(0);/// МОжно заменить на подходящий по смыслу метод

                    ewApp()->setVisible(_iface->id(), true);
                    connect(_iface, SIGNAL(signalClosed()), this, SLOT(slotPixelVisualizerClosed()));

                    _videoVisualizrers.insert(providerId, _iface);
                }
            }
        }
    }
    return visualizerId;
}

bool VideoVisualizersInterface::removeVisualizerById(uint _visualizerId)
{
    for(auto it = _videoVisualizrers.begin(); it != _videoVisualizrers.end(); ++it)
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
                    auto it = _videoVisualizrers.find(providerId);
                    if(it != _videoVisualizrers.end())
                    {
                        ewApp()->removeWidget(_iface->id());
                        delete _iface;
                        _iface = 0;

                        _videoVisualizrers.erase(it);
                    }
                }
                _videoVisualizrers.erase(it);
                return true;
            }
        }
    }
    return false;
}

bool VideoVisualizersInterface::removeVisualizerByProviderId(uint providerId)
{
    auto it = _videoVisualizrers.find(providerId);
    if(it != _videoVisualizrers.end())
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
            auto it = _videoVisualizrers.find(providerId);
            if(it != _videoVisualizrers.end())
            {
                ewApp()->removeWidget(_iface->id());
                delete _iface;
                _iface = 0;

                _videoVisualizrers.erase(it);
            }
        }
        return true;
    }
    return false;
}

uint VideoVisualizersInterface::getProviderId()
{
    return providerId;
}

void VideoVisualizersInterface::slotPixelVisualizerClosed()
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
            auto it = _videoVisualizrers.find(providerId);
            if(it != _videoVisualizrers.end())
            {
                ewApp()->removeWidget(_iface->id());
                delete _iface;
                _iface = 0;

                _videoVisualizrers.erase(it);
            }
            emit visualizerDistroyed();
        }
    }
}
