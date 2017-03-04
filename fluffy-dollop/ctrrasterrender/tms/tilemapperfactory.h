#ifndef TILEMAPPERFACTORY_H
#define TILEMAPPERFACTORY_H

#include <ctrcore/provider/rasterdataprovider.h>
#include <ctrcore/visual/scenedescriptor.h>
#include <ctrrasterrender/tms/tilemapper.h>

namespace visualize_system {

class TileMapperFactory
{
public:
    static TileMapper* createTileMapper(uint visualizerId, uint providerId, const SceneDescriptor & sceneDescriptor);

private:
    TileMapperFactory() {}
    ~TileMapperFactory() {}
};

}

#endif
















