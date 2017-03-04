#ifndef VIDEOVISUALIZERSINTERFACE_H
#define VIDEOVISUALIZERSINTERFACE_H

#include <QObject>
#include <ctrcore/plugin/embifacenotifier.h>

namespace visualize_system
{

class VideoVisualizersInterface : public QObject
{
    Q_OBJECT
    friend class VisualizerManager;
public:
    ~VideoVisualizersInterface();
    uint addVisualizer(uint providerId);
    bool removeVisualizerById(uint visualizerId);
    bool removeVisualizerByProviderId(uint providerId);
    uint getProviderId();

private slots:
    void slotPixelVisualizerClosed();

private:
    VideoVisualizersInterface();
    QMap<uint,EmbIFaceNotifier*> _videoVisualizrers;
    uint providerId = 0;

signals:
    void visualizerDistroyed();

};

}

#endif // PIXELVISUALIZERSINTERFACE_H
