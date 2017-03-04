#ifndef VISUALIZERINTERFACE_H
#define VISUALIZERINTERFACE_H

#include "abstractvisualizer.h"
#include "visualizertype.h"
#include <QObject>

namespace visualize_system
{
class VisualizerInterface : public QObject
{
    Q_OBJECT
public:
    explicit VisualizerInterface(AbstractVisualizer& abstractVisualizer);
    virtual ~VisualizerInterface();
    uint getVisualizerId();
    VisualizerType getVisualizerType();

protected:
    AbstractVisualizer & _abstractVisualizer;
};
}

#endif
