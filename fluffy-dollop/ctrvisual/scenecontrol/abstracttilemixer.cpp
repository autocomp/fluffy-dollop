#include "abstracttilemixer.h"

using namespace visualize_system;

AbstractTileMixer::AbstractTileMixer(uint visualiserId, QGraphicsScene *scene, SceneDescriptor &sceneDescriptor)
    : m_visualiserId(visualiserId),
      m_scene(scene),
      m_sceneDescriptor(sceneDescriptor)
{
}

void AbstractTileMixer::abortLoading()
{
    concrete_abortLoading();
}

void AbstractTileMixer::getTiles(const QList<visualize_system::URL> list, int currentZ, QRectF tilesOnViewport)
{
    concrete_getTiles(list, currentZ, tilesOnViewport);
}

void AbstractTileMixer::removeTiles(const QStringList &list)
{
    concrete_removeTiles(list);
}















