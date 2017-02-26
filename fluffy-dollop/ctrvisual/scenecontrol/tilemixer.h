#ifndef TILEMIXER_H
#define TILEMIXER_H

#include "abstracttilemixer.h"
#include "tilemixercell.h"
#include "matchingstate.h"
#include "rasterlayer.h"
#include <ctrcore/provider/t_image.h>
#include <ctrcore/visual/selectionmodelinterface.h>
#include <ctrrasterrender/tms/tilemapper.h>
#include <QPixmap>
#include <QMultiMap>
#include <QVariant>
#include <QPen>
#include <QTimer>
#include <QMenu>

class QGraphicsPolygonItem;
class QGraphicsScene;
class QItemSelection;
class QGraphicsItem;

namespace visualize_system {

class SceneDescriptor;
class RasterControlWidget;
class MatchingWidget;
class IntersectsScene;

class TileMixer : public AbstractTileMixer
{
    Q_OBJECT
public:
    TileMixer(uint visualiserId, QGraphicsScene *scene, SceneDescriptor& sceneDescriptor);
    virtual ~TileMixer();
    virtual QList<uint> addProviders(const QList<uint> & providers);
    virtual QList<uint> addBaseProviders(const QList<uint> & providers);
    virtual QList<uint> removeProviders(const QList<uint> & providers);
    //virtual QList<data_system::RasterDataProvider*> removeProviders(QList<data_system::RasterDataProvider*> &prov_list);
//    virtual void removeProviders();
    virtual QList<uint> getProviders(ProviderType providerType);
    virtual void addAltMatrix(const QString &path);
    virtual void removeAltMatrix(const QString &path);
    virtual QList<QString> getAltMatrices();
    virtual void setPenForAltMatrix(const QString &path, const QVariant &value);
    virtual void propertyChanged(QString, QVariant, data_system::RasterDataProvider *prov);
    virtual void addActionToObjectMenu(QList<QAction*> actions, QString groupName, uint groupWeight, QString subMenuName);
    virtual void addActionToRasterMenu(QList<QAction*> actions, QString groupName, uint groupWeight, QString subMenuName);

public slots:
    void paintTile(uint providerId, const QString &OUTKey, QPixmap& pixmap, int tileType);
    void setSelectedPen(QPen pen);
    void setDefaultPen(QPen pen);
    void slot_DubleClickOnScene(QPointF);
    void slot_ClickRBOnObject(quint64 id);
    void slot_ClickRBOnRaster(uint id, QPointF geoPos, bool controlModifier);

signals:
//    void signalProvidersAdded();
    void signalProviderMatched(uint id);
    void signalGetViewportRect(QRectF & r);
    void signalSetWidgetsEnabled(bool on_off);
    void signalConvertSceneCoordToGlobal(QPointF &);
    void signalInitObjectMenu(QMenu & objectMenu, quint64 objectId);

protected:
    virtual void concrete_removeTiles(const QStringList &list);
    virtual void concrete_getTiles(const QList<visualize_system::URL> list, int currentZ, QRectF tilesOnViewport);
    virtual void concrete_abortLoading();
    virtual void dataLoaded();
    virtual void matchingProvider(uint id);

//    QPointF geoToScene(const QPointF& coordinateIn);

protected slots:
    void slot_dataLoaded();
    virtual void rasterLayerActivated();
    void slot_reloadViewport();
    void slot_setVisibleForRaster(uint rasterShow, uint rasterHide);
    void slot_ControlWidgetClosed();
    void slot_providerChanged();
    void slot_getRasterParams(uint rasterId, QPixmap& pixmap, int& zLevel, QPolygonF& vertexInSceneCoords);
    void slot_applyTransformation(uint rasterId, const QPolygonF& polygonInSceneCoords);
    void slot_getPolygonInSceneCoords(uint rasterId, QPolygonF &polygonInSceneCoords);
//    void slot_applyMatching(uint rasterId, QList<MatchingState::MatchingPoint> matchingPoints, dpf::geo::PolinomialGeoData &dst_data);
//    void slotSelectedProvidersChanged(QList<uint> selectedProviders);
    void slotSelectedProvidersChanged(const QList<uint> & providerSelected, const QList<uint> & providerDeselected);
    void slotCurrentProviderChanged(uint cur, uint prev);
    void slotMarkedProvidersChanged(const QList<uint> & providerMarked, const QList<uint> & providerDemarked);
    void slotProviderPropertyChanged(uint providerId, QString propertyName);
    void slotProviderProcessingSettingsChanged(uint providerId, QString processingSystemName);
    void slotDefaultRasterSelectionModeChanged(visualize_system::DefaultRasterSelectionMode);
    void slotMatchProvider(QVariant);
    void slotProviderChanged(uint id);

protected :
    virtual void rasterLayerActivated(visualize_system::RasterLayer* rasterLayer);
    QRectF tileSceneRect(int x, int y, int z);
    void setActionProperty(QMap<QString, QVariant> map, QMenu * menu);
    void loadingRasterLayerInitData();

protected:
    visualize_system::RasterLayerInitData _rasterLayerInitData;
    bool m_blockFinishLoadind, m_onlyResultPixmap;
    QMap<QString,visualize_system::RasterLayer *> m_altMatrixes;
    IntersectsScene * m_intersectsScene;
    int MAX_RASTER_PROVIDERS_IN_TAIL;
    const int UPDATE_VIEWPORT_BY_TIMER_MSEC;
    QMap<QString,TileMixerCell> m_Cells;
    QMap<uint,visualize_system::DataProviderProperty> m_Providers;
    QTimer m_Timer;
    int m_lastCurrentZ;
    RasterControlWidget * m_rasterControlWidget;
    MatchingWidget * m_matchingWidget;
    QMenu * m_rasterMenu;
    //QMenu * m_objectMenu;
    DefaultRasterSelectionMode _defaultRasterSelectionMode;
};

}
#endif





















