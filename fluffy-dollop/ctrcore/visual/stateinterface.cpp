#include "stateinterface.h"

using namespace visualize_system;

StateInterface::StateInterface(AbstractVisualizer& abstractVisualizer)
    : VisualizerInterface(abstractVisualizer)
{
    connect(&abstractVisualizer, SIGNAL(signalStateChanged()), this, SIGNAL(signalStateChanged()));
}

bool StateInterface::setVisualizerState(QSharedPointer<visualize_system::VisualizerStateObject> state)
{
    return _abstractVisualizer.setVisualizerState(state);
}

QString StateInterface::getCurrentState()
{
    return _abstractVisualizer.getCurrentState();
}
