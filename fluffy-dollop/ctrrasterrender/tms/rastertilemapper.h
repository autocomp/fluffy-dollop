#ifndef RASTERTILEMAPPER_H
#define RASTERTILEMAPPER_H

#include "tilemapper.h"
#include "rastermappercache.h"

namespace visualize_system {

/**
 * @brief The RasterTileMapper class Класс мапинга координат
 * Класс обеспечивает доступ к растровому провайдерам через тайловый интерфейс
 */
class RasterTileMapper : public TileMapper
{
    Q_OBJECT

public:
    RasterTileMapper(uint visualizerId, uint providerId, const SceneDescriptor & _sceneDescriptor);
    ~RasterTileMapper();
    virtual bool isTMSProviderMapper() {return false;}
    virtual bool isRasterProviderMapper() {return true;}
    virtual void loadCache(const QString & path);
    int loadTile(image_types::TImage *img, int x, int y, int z, QPoint &shiftImageInTile);
    void loadTiles(const QList<visualize_system::URL> list);
    void abort();
    void abort(QString taskKey);
    QPolygonF getBoundingPolygone();
    bool cacheLoaded();
    bool dataLoaded();
    void getPixmap(int zLevel, QPixmap & pixmap, QPolygonF& vertexInSceneCoords);

protected:
    RasterMapperCache _rasterMapperCache;
    QMap<uint, Temp> _loadingBaseTImageMap;
    const uint DELTA_Z_LEVEL_FOR_INSERT_TO_CACHE;
    const uint DOWN_DELTA_Z_LEVEL;
    const uint MAX_Z;
    void scaleTile(visualize_system::URL url);
    QPoint tileUnderCurrent(int x, int y, int z);

protected slots:
    void slotRasterDataReady(int task);
    void slotProviderChanged();

signals:

};

}

#endif
