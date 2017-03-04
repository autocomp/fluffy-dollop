#ifndef TMSTILEMAPPER_H
#define TMSTILEMAPPER_H

#include "tilemapper.h"
#include <ctrcore/provider/tiledataprovider.h>
#include <QSharedPointer>

using namespace data_system;

namespace visualize_system {

struct OtherLevelData
{
    OtherLevelData(const TileCoords& _tileCoords, int _targetZlevel) : tileCoords(_tileCoords), targetZlevel(_targetZlevel), tImg(0) /*, loaded(false)*/ {}
    TileCoords tileCoords;
    QList<Temp> list;
    image_types::TImage* tImg;
    int targetZlevel;
};

/**
 * @brief The TmsTileMapper class Класс мапинга координат
 * Класс обеспечивает доступ к тайловому провайдерам через тайловый интерфейс
 */
class TmsTileMapper : public TileMapper
{
    Q_OBJECT

private:
    QSharedPointer<TileDataProvider> tms_provider;
    // контейнер для ожидания загрузки тайлов Z-уровнем ниже чем было заказано тайловым провайдером
    QMap<uint, OtherLevelData> m_MapForOtherTiles;

public:
    TmsTileMapper(uint visualizerId, uint providerId, const SceneDescriptor & _sceneDescriptor);
    ~TmsTileMapper();
    virtual bool isTMSProviderMapper() {return true;}
    virtual bool isRasterProviderMapper() {return false;}
    virtual void loadCache(const QString & path) {}
    void loadTiles(const QList<visualize_system::URL> list);
    void abort();
    void abort(QString taskKey);
    QPolygonF getBoundingPolygone();
    bool cacheLoaded() {return true;}
    bool dataLoaded();

protected:
    void analizeNotFoundTask(Temp temp);
    void cutTile(OtherLevelData& otherLevelData);

protected slots:
    void slotDataReady(int task, uint sourceId, TileDataProvider::Result result);

signals:

};

}

#endif
