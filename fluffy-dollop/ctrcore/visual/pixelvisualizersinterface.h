#ifndef PIXELVISUALIZERSINTERFACE_H
#define PIXELVISUALIZERSINTERFACE_H

#include <QObject>
#include <ctrcore/plugin/embifacenotifier.h>

namespace visualize_system
{

class PixelVisualizersInterface : public QObject
{
    Q_OBJECT
    friend class VisualizerManager;
public:
    ~PixelVisualizersInterface();
    uint addVisualizer(uint providerId, bool checkProviderOpenedInPixelVis = true);
    bool removeVisualizerById(uint visualizerId);
    bool removeVisualizerByProviderId(uint providerId);
    bool setWindowTitle(uint visualizerId, const QString &text);

private slots:
    void slotPixelVisualizerClosed();

private:
    PixelVisualizersInterface();
    QMap<uint,EmbIFaceNotifier*> _pixelVisualizrers;

};

}

#endif // PIXELVISUALIZERSINTERFACE_H
