#ifndef INTERSECTSSCENE_H
#define INTERSECTSSCENE_H

#include <QGraphicsScene>

namespace visualize_system {

class IntersectsScene : public QGraphicsScene
{
    enum DataPolygonItem {ID, IS_BASE_COVER, TRANSPARENCY, VISIBILITY, REAL_Z_VALUE};

public:
    IntersectsScene();
    void addPolygon(uint id, QPolygonF scenePolygon, bool isBaseCover);
    void setPolygon(uint id, QPolygonF scenePolygon);
    void removePolygon(uint id);
    QList<uint> check(const QRectF & sceneRect, bool BASE_COVER_ONLY, uint maxRastersInArea);
    QList<uint> check(const QPointF & scenePos);
    void setProviderOnTop(uint id);
    void setTransparency(uint id, uint persents);
    void setVisibility(uint id, bool on_off);
    void clearScene();
    void getProvidersInTile(const QRectF & tileSceneRect, QList<uint> & baseProviders, QList<uint> & otherProviders, uint maxRastersInArea);
    void setZvalue(uint id, uint zValue);

private:
    const uint Z_LEVEL_FOR_TOP_RASTER;
    QList<QGraphicsPolygonItem*> _itemsByZlevel;
    uint _itemOnTop;
};

}
#endif
