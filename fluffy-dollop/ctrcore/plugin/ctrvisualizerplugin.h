#ifndef CTRVISUALIZERPLUGIN_H
#define CTRVISUALIZERPLUGIN_H

#include "ctrplugin.h"

using namespace ctrplugin;

class CtrVisualizerPlugin : public CtrPlugin
{
public:
    CtrVisualizerPlugin();
    virtual ~CtrVisualizerPlugin();

    virtual TYPE_PLUGIN getPluginType() { return TYPE_VISUALIZER;}

};

#endif // CTRVISUALIZERPLUGIN_H
