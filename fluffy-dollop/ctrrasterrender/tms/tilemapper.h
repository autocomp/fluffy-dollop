#ifndef TILEMAPPER_H
#define TILEMAPPER_H

#include <ctrcore/visual/abstracttilemapper.h>
#include <QObject>
#include <QPolygonF>
#include <QSet>

//#include <ctrcore/processing/processingpool.h>

//using namespace processing_system;

namespace visualize_system {

struct Area
{
    Area(int _z = 0) : z(_z) {}
    int z;
    QRect rect;
};

struct ProcessingArea
{
    ProcessingArea() : task(-1), z(0){}
    uint task;
    int z;
    QRect rect;
};

enum class MapperViewMode
{
    SHOW_BORDER_AND_RASTER,
    SHOW_ONLY_RASTER,
    SHOW_ONLY_BORDER
};

/**
 * @brief The TileMapper class Класс мапинга координат
 * Базовый абстрактный класс, обеспечивает доступ к тайловому и растровому провайдерам через тайловый интерфейс
 */
class TileMapper : public AbstractTileMapper
{
    Q_OBJECT

protected:
    QRect tileBoundingRect;

    QRect rasterPixBRect;
    double baseZlevel;

    Area queryArea;
    ProcessingArea processingArea;
    uint mapperKey;
    int tasksInProcessingsPool;
    const uint64_t processingPoolSenderId;
    // кэш-ключи для асинхронно загружаемых задач
    QSet<QString> loadingTaskKey;
    QMap<int, QRect> tileAreaForZlevel;
    const int MAX_Z_LEVEL;
    int MIN_Z_LEVEL;
    MapperViewMode _mapperViewMode;

    QPolygonF _boundingScenePolygone;
    QRectF _boundingSceneRect;
    QString PROVIDER_URL;
    const uint PROVIDER_ID;
    const uint VISUALIZER_ID;
    bool _hasProcessing;

public:
    enum TileType{
        INVALID = 0,
        ORIGINAL,
        SCALED  // тайл получен растягиванием тайла с более нижнего уровня, отправлен для предпросмотра во время ожидания оригинального тайла.
    };

    TileMapper(uint visualizerId, uint providerId, const SceneDescriptor & _sceneDescriptor);
    virtual ~TileMapper();
    virtual bool isTMSProviderMapper() = 0;
    virtual bool isRasterProviderMapper() = 0;
    virtual void abort(QString taskKey) = 0;
    virtual bool cacheLoaded() = 0;
    virtual bool dataLoaded() = 0;
    virtual QPolygonF getBoundingPolygone() = 0;
    virtual void loadCache(const QString & path) = 0;
    virtual void loadTiles(const QList<visualize_system::URL> list) = 0;
    virtual void setCurrentViewParams(int currentZ, QRectF tilesOnViewport);
    void setMinZlevel(int minZlevel);
    int getMinZlevel();
    void setMapperViewMode(MapperViewMode mapperViewMode);
    MapperViewMode mapperViewMode();
//    bool setHisto(const QString& title, const QVariant& value);
    double getBaseZlevel() {return baseZlevel;}
    QPointF getFrameDelta() {return shiftImageOnScene;}
    QPolygonF getBoundingScenePolygone() {return _boundingScenePolygone;}
    QPoint getTile(const QPointF wgsPos, int Z);
    QRectF tileSceneRect(int x, int y, int z);
    QString getProviderUrl();
    uint getProviderId();
    uint getVisualizerId();
    void setHasProcessing(bool hasProcessing);

protected slots:
//    void slotTaskCompleted(RuntimeTask *task);

signals:
    void signal_paintTile(uint providerID, const QString& OUTKey, QPixmap& pixmap, int tileType);
    void signal_dataLoaded();
    void signal_providerChanged();
};

}

#endif // TILEMAPPER_H
















