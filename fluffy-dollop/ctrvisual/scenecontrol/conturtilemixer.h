#ifndef CONTURTILEMIXER_H
#define CONTURTILEMIXER_H

#include "tilemixer.h"
#include "blockdialog.h"
#include <QTimer>

namespace visualize_system {

class ConturTileMixer : public TileMixer
{
    Q_OBJECT
public:
    ConturTileMixer(uint visualiserId, QGraphicsScene *scene, SceneDescriptor& sceneDescriptor);
    virtual ~ConturTileMixer();
    virtual QList<uint> addProviders(const QList<uint> & prov_list);
    virtual QList<uint> addBaseProviders(const QList<uint> & prov_list);

protected slots:
    virtual void rasterLayerActivated();
//    void slotGeoModelSelectionChanged(const QItemSelection&, const QItemSelection&);
    void slotPropertyChanged(QString title, QVariant value);
    void slotCheckLoadingCache();
    void rasterLayerClicked(bool withControl);

protected:
    virtual void rasterLayerActivated(visualize_system::RasterLayer* rasterLayer);


    QTimer m_timer;
    QList<data_system::RasterDataProvider*> m_prov_list_for_create;
    QList<TileMapper *> m_loadingCacheList;
    int ADD_RASTERS_TIMEOUT;
    BlockDialog * m_blockDialog;
    bool m_stopLoadingCache;


    double DELTA_Z_LEVEL;
    int MIN_LIST_SIZE;
    QString RASTER_CACHE_PATH;
    int m_middleMinZlevel;
    QString m_settingsPath;


};


}
#endif
