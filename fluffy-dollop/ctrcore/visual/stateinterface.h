#ifndef STATEINTERFACE_H
#define STATEINTERFACE_H

#include "visualizerinterface.h"

namespace visualize_system
{
class VisualizerStateObject : public QObject
{
public:
    VisualizerStateObject() {}
    virtual ~VisualizerStateObject() {}
    virtual QString stateName() = 0;
};

class StateInterface : public VisualizerInterface
{
    Q_OBJECT
public:
    explicit StateInterface(AbstractVisualizer& abstractVisualizer);
    bool setVisualizerState(QSharedPointer<visualize_system::VisualizerStateObject> state);
    QString getCurrentState();

signals:
    void signalStateChanged();
};
}

#endif // STATEINTERFACE_H
