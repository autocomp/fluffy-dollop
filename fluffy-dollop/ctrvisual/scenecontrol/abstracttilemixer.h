#ifndef ABSTRACTTILEMIXER_H
#define ABSTRACTTILEMIXER_H

#include <QObject>
#include <QVariant>
#include <ctrcore/provider/t_image.h>
#include <ctrcore/visual/abstracttilemapper.h>
#include <ctrcore/visual/scenedescriptor.h>

class QPixmap;
class QGraphicsPolygonItem;
class QGraphicsScene;
class QItemSelection;
class QGraphicsItem;
class AbstractTileMapper;

namespace visualize_system
{

class AbstractTileMixer : public QObject
{
    Q_OBJECT

public:
    AbstractTileMixer(uint visualiserId, QGraphicsScene *scene, SceneDescriptor& sceneDescriptor);
    virtual ~AbstractTileMixer() {}

public slots:
    void abortLoading();
    void getTiles(const QList<visualize_system::URL> list, int currentZ, QRectF tilesOnViewport);
    void removeTiles(const QStringList &list);

protected:
    QList<visualize_system::DataProviderProperty> m_providers;
    const uint m_visualiserId;
    SceneDescriptor& m_sceneDescriptor;
    QGraphicsScene * m_scene;

protected:
    virtual void concrete_removeTiles(const QStringList &list) = 0;
    virtual void concrete_getTiles(const QList<visualize_system::URL> list, int currentZ, QRectF tilesOnViewport) = 0;
    virtual void concrete_abortLoading() = 0;

signals:
    void drawTile(const QPixmap&, const QString&);
    void loadingFinished();
    void updateContent(bool deepRepaint);
    void signalPixelCursorCreated(uint32_t, uint32_t);
    void initPixelScene(QPointF, double, int, int);
    void initPixelScene(QPointF deltaPixFrame, double baseZlevel, int frameId, int baseGeoZlevel, QString path);
    void setSceneRect(QRectF sceneRect);
    void setZoomLevels(uint minZlevel, uint maxZlevel, uint baseZlevel);
    void signalItemsForDraw(QList<QGraphicsItem*>*);
};

}
#endif
