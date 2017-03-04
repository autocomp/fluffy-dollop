#include "ctrappvisualizerplugin.h"

CtrAppVisualizerPlugin::CtrAppVisualizerPlugin()
    :_visualizerId(0)
    , _visualizerWindowId(0)
{
}

void CtrAppVisualizerPlugin::init(uint visualizerId, quint64 visualizerWindowId)
{
    _visualizerId = visualizerId;
    _visualizerWindowId = visualizerWindowId;
}

uint CtrAppVisualizerPlugin::getVisualizerId() const
{
    return _visualizerId;
}

quint64 CtrAppVisualizerPlugin::getVisualizerWindowId() const
{
    return _visualizerWindowId;
}


CtrAppVisualizerPlugin::~CtrAppVisualizerPlugin()
{
}
