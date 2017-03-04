#ifndef VISUALIZERMANAGER_H
#define VISUALIZERMANAGER_H

#include "abstractvisualizer.h"
#include "visualizertype.h"

#include <QObject>
#include <QMap>
#include <QSet>
#include <QList>
#include <QVariant>

class RasterDataProvider;
class VisualizerStateObject;

namespace visualize_system
{
class ViewInterface;
class DataInterface;
class VectorDataInterface;
class StateInterface;
class PixelVisualizersInterface;
class VideoVisualizersInterface;

class VisualizerManager : public QObject
{
    Q_OBJECT

public:
    static VisualizerManager* instance();
    ~VisualizerManager();

    // создание заданного визуализатора
    uint addVisualizer(VisualizerType type);

    // удаление заданного визуализатора
    bool removeVisualizer(uint visualizerId);

    // получение идентификаторов всех визуализаторов.
    QList<uint> getVisualizersId();

    // получение интерфейса для работы с пиксельными визуализаторами как отдельными окнами в оконной системе ЭмбедедВиджетс.
    PixelVisualizersInterface * getPixelVisualizersInterface();

    // получение интерфейса для работы с видео визуализаторами как отдельными окнами в оконной системе ЭмбедедВиджетс.
    VideoVisualizersInterface * getVideoVisualizersInterface();

    // получение интерфейсов заданного визуализатора.
    ViewInterface * getViewInterface(uint visualizerId);
    DataInterface * getDataInterface(uint visualizerId);
    VectorDataInterface * getVectorInterface(uint visualizerId);
    StateInterface * getStateInterface(uint visualizerId);
    SelectionModelInterface * getSelectionModelInterface(uint visualizerId);

    // получение идентификаторов провайдеров для копирования.
    QList<uint> getProvidersForCopying();

    // установка идентификаторов провайдеров для копирования.
    void setProvidersForCopying(const QList<uint> &providerUrls);

    // методы конвертации типа визуализатора в строку и обратно.
    static QString convertVisualizerType(VisualizerType type);
    static VisualizerType convertVisualizerType(QString type);

signals:
    // уведомление об создание визуализатора
    void signalVisualizerAdded(uint visualizerId);

    // уведомление об удаление визуализатора
    void signalVisualizerRemoveed(uint visualizerId);

    // уведомление об изменении идентификаторов провайдеров для копирования.
    void signalProviderForCopyingChanged();

    // сигналы присоединены к таким же сигналам каждого визуализатора.
    void signalMoveParallelCursor(uint visualizerId, QPointF wgsCoords);

private:
    explicit VisualizerManager();
    AbstractVisualizer* visualizerFactory(uint visualizerId, VisualizerType type);
    QSet<VisualizerType> getSingleVisualizerTypies();

private:
    static VisualizerManager* _instance;
    uint _visualizerCounter;
    QMap<uint, AbstractVisualizer*> _visualizers;
    QSet<VisualizerType> _singleVisualizerTypies;
    QList<uint> _providersForCopying;
    PixelVisualizersInterface * _pixelVisualizersInterface;
    VideoVisualizersInterface * _videoVisualizersInterface;
};
}
#endif
























