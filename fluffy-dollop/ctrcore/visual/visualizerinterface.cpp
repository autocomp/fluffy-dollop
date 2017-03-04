#include "visualizerinterface.h"

using namespace visualize_system;

VisualizerInterface::VisualizerInterface(AbstractVisualizer& abstractVisualizer)
     : _abstractVisualizer(abstractVisualizer)
{
}

VisualizerInterface::~VisualizerInterface()
{
}

uint VisualizerInterface::getVisualizerId()
{
    return _abstractVisualizer.getVisualizerId();
}

VisualizerType VisualizerInterface::getVisualizerType()
{
    return _abstractVisualizer.getVisualizerType();
}












