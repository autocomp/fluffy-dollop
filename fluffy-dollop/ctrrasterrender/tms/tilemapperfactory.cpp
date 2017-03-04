#include "tilemapperfactory.h"
#include "tmstilemapper.h"
#include "rastertilemapper.h"
#include <ctrcore/provider/tiledataprovider.h>
#include <ctrcore/provider/dataproviderfactory.h>
#include <QSharedPointer>

using namespace visualize_system;
using namespace data_system;

TileMapper* TileMapperFactory::createTileMapper(uint visualizerId, uint providerId, const SceneDescriptor & sceneDescriptor)
{
    TileMapper* tileMapper = 0;

    QSharedPointer<AbstractDataProvider> dp = data_system::DataProviderFactory::instance()->getProvider(providerId);
    QSharedPointer<TileDataProvider> tms_provider = dp.dynamicCast<TileDataProvider>();
    if(tms_provider)
        tileMapper = new TmsTileMapper(visualizerId, providerId, sceneDescriptor);
    else
        tileMapper = new RasterTileMapper(visualizerId, providerId, sceneDescriptor);

    return tileMapper;
}


