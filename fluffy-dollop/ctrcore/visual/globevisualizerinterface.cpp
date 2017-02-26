#include "globevisualizerinterface.h"
#include "visualizertype.h"
#include "abstractvisualizer.h"

using namespace visualize_system;

GlobeVisualizerInterface::GlobeVisualizerInterface(AbstractVisualizer& abstractVisualizer)
    : VisualizerInterface(abstractVisualizer)
{
}

void GlobeVisualizerInterface::setStarsVisible(bool on_off)
{

}

void GlobeVisualizerInterface::setCloudsVisible(bool on_off)
{

}

void GlobeVisualizerInterface::centerOnEarth()
{

}

void GlobeVisualizerInterface::centerOnMoon()
{

}

void GlobeVisualizerInterface::centerOnMars()
{

}
