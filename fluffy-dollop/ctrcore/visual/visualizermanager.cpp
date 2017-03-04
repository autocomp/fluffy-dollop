#include "visualizermanager.h"
#include <QDir>
#include <QDebug>
#include "ctrcore/plugin/ctrpluginloader.h"
#include "pixelvisualizersinterface.h"
#include "videovisualizersinterface.h"
using namespace visualize_system;

VisualizerManager* VisualizerManager::_instance = 0;

VisualizerManager* VisualizerManager::instance()
{
    if(!_instance)
        _instance = new VisualizerManager;
    return _instance;
}

VisualizerManager::VisualizerManager()
    : _visualizerCounter(0)
{
    _singleVisualizerTypies = getSingleVisualizerTypies();
    _pixelVisualizersInterface = new PixelVisualizersInterface;

    _videoVisualizersInterface = new VideoVisualizersInterface;
}

VisualizerManager::~VisualizerManager()
{
    delete _pixelVisualizersInterface;
    delete _videoVisualizersInterface;
}

QSet<VisualizerType> VisualizerManager::getSingleVisualizerTypies()
{
    QSet<VisualizerType> singleVisualizerTypies;
    //! тут необходимо получить из вне (например из конфига) те типы визуализаторов, которые будут представлены в приложении в единичном экземпляре.
    singleVisualizerTypies.insert(Visualizer2D);
    singleVisualizerTypies.insert(Visualizer3D);

    return singleVisualizerTypies;
}

uint VisualizerManager::addVisualizer(VisualizerType type)
{
    if(_singleVisualizerTypies.contains(type))
        for(auto it = _visualizers.begin(); it != _visualizers.end(); ++it)
            if(it.value()->getVisualizerType() == type)
                return it.key();

    const uint visualizerId(_visualizerCounter += 1);
    AbstractVisualizer* vis = visualizerFactory(_visualizerCounter, type);
    if(vis)
    {
        connect(vis, SIGNAL(signalMoveParallelCursor(uint,QPointF)), this, SIGNAL(signalMoveParallelCursor(uint,QPointF)));

        _visualizers.insert(visualizerId, vis);
        emit signalVisualizerAdded(visualizerId);
        return visualizerId;
    }
    else
    {
        _visualizerCounter -= 1;
        return 0;
    }
}

bool VisualizerManager::removeVisualizer(uint visualizerId)
{
    auto it = _visualizers.find(visualizerId);
    if(it != _visualizers.end())
    {
        it.value()->deleteLater(); //! пока не понятно что с этим делать ?!
        _visualizers.erase(it);
        emit signalVisualizerRemoveed(visualizerId);
        return true;
    }
    else
        return false;
}

QList<uint> VisualizerManager::getVisualizersId()
{
    QList<uint> list;
    for(auto it = _visualizers.begin(); it != _visualizers.end(); ++it)
        list.append(it.key());
    return list;
}

PixelVisualizersInterface * VisualizerManager::getPixelVisualizersInterface()
{
    return _pixelVisualizersInterface;
}

VideoVisualizersInterface *VisualizerManager::getVideoVisualizersInterface()
{
    return _videoVisualizersInterface;
}

ViewInterface * VisualizerManager::getViewInterface(uint visualizerId)
{
    auto it = _visualizers.find(visualizerId);
    if(it != _visualizers.end())
        return it.value()->getViewInterface();
    else
        return 0;
}

DataInterface * VisualizerManager::getDataInterface(uint visualizerId)
{
    auto it = _visualizers.find(visualizerId);
    if(it != _visualizers.end())
        return it.value()->getDataInterface();
    else
        return 0;
}

VectorDataInterface * VisualizerManager::getVectorInterface(uint visualizerId)
{
    auto it = _visualizers.find(visualizerId);
    if(it != _visualizers.end())
        return it.value()->getVectorInterface();
    else
        return 0;
}

StateInterface * VisualizerManager::getStateInterface(uint visualizerId)
{
    auto it = _visualizers.find(visualizerId);
    if(it != _visualizers.end())
        return it.value()->getStateInterface();
    else
        return 0;
}

SelectionModelInterface * VisualizerManager::getSelectionModelInterface(uint visualizerId)
{
    auto it = _visualizers.find(visualizerId);
    if(it != _visualizers.end())
        return it.value()->getSelectionModelInterface();
    else
        return 0;
}

QList<uint> VisualizerManager::getProvidersForCopying()
{
    return _providersForCopying;
}

void VisualizerManager::setProvidersForCopying(const QList<uint> & providerUrls)
{
    _providersForCopying = providerUrls;
    emit signalProviderForCopyingChanged();
}

AbstractVisualizer* VisualizerManager::visualizerFactory(uint visualizerId, VisualizerType type)
{
    AbstractVisualizer* needVisualizer(nullptr);
    CtrPluginLoader* loader = CtrPluginLoader::instance();
    QList<CtrPlugin*> pluginList = loader->getPluginList(ctrplugin::TYPE_VISUALIZER);
    foreach(CtrPlugin* ctrPlugin, pluginList)
    {
        if(needVisualizer == nullptr)
        {
            AbstractVisualizer* visPlugin = dynamic_cast<AbstractVisualizer*>(ctrPlugin);
            if(visPlugin)
            {
                QString visType_ = VisualizerManager::convertVisualizerType(visPlugin->getVisualizerType());
                if(visPlugin->getVisualizerType() == type)
                {
                    visPlugin->init(visualizerId);
                    needVisualizer = visPlugin;
                    continue;
                }
            }
        }
        delete ctrPlugin;
    }

    return needVisualizer;
}

QString VisualizerManager::convertVisualizerType(VisualizerType type)
{
    switch (type)
    {
    case VisualizerType::Visualizer2D: return QString("Visualizer2D");
    case VisualizerType::Visualizer3D: return QString("Visualizer3D");
    case VisualizerType::VisualizerPixel: return QString("VisualizerPixel");
    case VisualizerType::VisualizerVideo: return QString("VisualizerVideo");
    case VisualizerType::Invalid :
    default:
        return QString();
    }
}

VisualizerType VisualizerManager::convertVisualizerType(QString type)
{
    if(type == QString("Visualizer2D"))
        return VisualizerType::Visualizer2D;
    else if(type == QString("Visualizer3D"))
        return VisualizerType::Visualizer3D;
    else if(type == QString("VisualizerPixel"))
        return VisualizerType::VisualizerPixel;
    else if(type == QString("VisualizerVideo"))
        return VisualizerType::VisualizerVideo;
    else
        return VisualizerType::Invalid;
}









