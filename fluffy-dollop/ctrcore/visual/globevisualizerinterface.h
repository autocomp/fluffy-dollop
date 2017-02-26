#ifndef GLOBEVISUALIZERINTERFACE_H
#define GLOBEVISUALIZERINTERFACE_H

#include "visualizerinterface.h"

namespace visualize_system
{

class GlobeVisualizerInterface : public VisualizerInterface
{
public:
    GlobeVisualizerInterface(AbstractVisualizer& abstractVisualizer);

    void setStarsVisible(bool on_off);
    void setCloudsVisible(bool on_off);

    void centerOnEarth();
    void centerOnMoon();
    void centerOnMars();
};

}

#endif // GLOBEVISUALIZERINTERFACE_H
