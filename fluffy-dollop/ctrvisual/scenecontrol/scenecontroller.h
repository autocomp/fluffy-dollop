#ifndef SCENECONTROLLER_H
#define SCENECONTROLLER_H

#include <QObject>
#include <QSize>
#include <QList>
#include <QTimer>
//#include <dpf/geo/convertion.h>
#include <ctrcore/visual/abstracttilemapper.h>
#include <ctrcore/visual/scenedescriptor.h>

#include "tilescene.h"
#include "viewercontroller.h"
#include "reperpoint.h"
#include "tilesceneasinc.h"
#include "sceneinthread.h"

namespace visualize_system {

struct UserAction
{
    UserAction()
    {
        type = QEvent::None;
        buttons = Qt::NoButton;
        modifiers = Qt::NoModifier;
        posf = QPointF(0,0);
        epsgCode = 4326;
        sceneId = 0;
        sceneType = 0;
        keybKey = Qt::Key_unknown;
    }

    QEvent::Type type;
    Qt::MouseButtons buttons;
    Qt::KeyboardModifiers modifiers;

    QPointF posf;
    int epsgCode;

    /*****/
    int sceneId;
    int sceneType;
    Qt::Key keybKey;

};

class ParallelCursor;

class SceneController : public QObject
{
    Q_OBJECT

public:
    SceneController(uint visualizerId, const SceneDescriptor& sceneDescriptor, QObject *parent = 0);
    virtual ~SceneController();

    virtual bool frameIsMatching();
    virtual void moveParallelCursorTo(QPointF wgs84grad);
    virtual void moveMouseInView(const QPointF& scenePos, int zLevel);
    virtual void moveParallelCursorInView(const QPointF& scenePos);

    void getCoordsForMark(quint64 objBrowserId, quint64 markId, QString iconPath);
    QString m_firstProviderUrl;
    ViewerController* addView();
    void addView(ViewerController* viewerController);
    void removeView(ViewerController *);
    QGraphicsScene* getScene() {return &m_Scene;}
    TileSceneAsinc* getTileSceneAsinc() {return &m_TileSceneAsinc;}
//    SceneInThread* getSceneInThread() {return m_SceneInThread;}
    TileScene* getTileScene() {return &m_TileScene;}
    QGraphicsScene* getMinimapScene() {return &m_MinimapScene;}
    QSize tileSize()const {return m_tileSize;}
    QSize tilesCount()const {return m_tilesCount;}
    QPointF geoToScene(const QPointF& coordice);
    QPointF sceneToGeo(const QPointF& point);
    uint zoomMin()const {return m_zoomMin;}
    uint zoomMax()const {return m_zoomMax;}
    double deg_rad(double x)const;
    double rad_deg(double x)const;
    void centerViewsOn(QPointF scenePos);
    void getTiles(const QList<visualize_system::URL>&, int currentZ, QRectF viewportArea, QRect tilesOnViewport);
    void removeTiles(const QStringList &);
    void abortLoading();
    void mousePressEventInView(QMouseEvent const * const e, QPointF scenePos);
    qreal getIntepixelDistance();
    bool sceneIsGeo() {return m_isGeo;}
    int getBaseZlevel() {return m_BaseZlevel;}
    int getFrameId() {return m_FrameId;}
//    bool startSelectingPolygonalArea(QCursor *cursor);
//    void finishSelectingPolygonalArea();
//    bool getPoint(QCursor *cursor);
//    void finish_getPoint();
    void setEditMode(bool on_off, QPixmap pm);
    void setSceneActive(bool on_off);
//    void setReperPointsState(bool on_off);
//    void setReperPointsOnMaster(bool on_off, bool onMaster, int masterFrameId);
//    SceneControl::ReperPoint *createReperPoints(QPointF framePos);
    bool isGeo();

    QPointF sceneToGeo2(const QPointF& pixPos);
    double getDistanceWGS84(QPointF firstWGS84Point, QPointF secondWGS84Point);
    qreal getInterMeterDistanceWGS84();

    void setFirstProviderUrl(QString url);
    QString getProviderUrl();

    qreal getIntepixelDistance(qreal m_scale);

    void setHighLightBaseObject(BaseSceneLayer* highLightBaseObject = 0);
    BaseSceneLayer* highLightBaseObject() {return m_HighLightBaseObject;}
    void getSceneParamsForSave(QPointF& scenePos, int& zoomLevel);
    void setSceneParams(QPointF scenePos, int zoomLevel);
    void setViewCenterOnObject(quint64 id);
    void setViewOptimization() {m_viewOptimization = true;}
    bool viewOptimization() {return m_viewOptimization;}
    void setTilesOnViewport(uint tiles) {m_addTileOnViewport = tiles;}
    uint addTileOnViewport() {return m_addTileOnViewport;}
    void initView(QColor backGroundColor, Qt::ScrollBarPolicy policy = Qt::ScrollBarAsNeeded);
    double getFrameCoef() {return m_Koef;}
    void emit_DubleClickOnScene(QPointF pos) {emit signal_doubleClickOnScene(pos);}
    void emit_ClickRBOnRaster(uint id, QPointF geoPos, bool controlModifier) {emit signal_clickRBOnRaster(id, geoPos, controlModifier);}
    void emit_ClickRBOnObject(quint64 id) {emit signal_clickRBOnObject(id);}
    uint getVisualizerId() {return m_visualizerId;}
    QRectF getViewportRect();
    QRectF getViewportSceneRect();
    void emit_signalUserViewControlAction(UserAction userAction);

public slots:
    void slotMainViewScaleChanged(qreal scale, int zLevel);
    void slotMainViewRotateAngleChanged(qreal angle);
    void updateContent(bool deepRepaint);
    void updateContent();
    void slotSetMainViewCenterOnGeoPoint(quint64 id, QRectF geoRect, int minVizZLevel, int maxVizZLevel, bool useZoom, bool force = false);
    void slotObjectCreated(QList<QGraphicsItem*>* list);
    void slotConvertPixelToGeo(QPointF pxPoint, QPointF* pGeoPoint);
    void slotConvertGeoToPixel(QPointF geoPoint, QPointF* pPxPoint);
    void slotGetInterpixelDistance(qreal* res);
    void initPixelScene(QPointF deltaPixFrame, double baseZlevel, int frameId, int baseGeoZlevel, QString path);
    void sceneToRealGeo(QPointF& point);
    void sceneToFrame(QPointF& point);
    void convertEpsgToScene(int epsgCode, QPointF&pos, bool &success);
    void convertSceneToEpsg(int epsgCode, QPointF&pos, bool &success);
    void realGeoToScene(QPointF&);
    void frameToScene(QPointF&);
	void slotGetInterMeterDistance(qreal*dist);
    void slotBlockSceneScroll(bool isBlocked);
    void slotBindLayer(quint64 id, bool isBind);
    void slotCurCoordSystChanged(int curCoordSyst);
    void setSceneRect(QRectF sceneRect);
    void setZoomLevels(int minZlevel, int maxZlevel, int baseZlevel);
    void setAsincLoadingOnGeoMap(bool on_off);
    void getViewportRect(QRectF & r);
    void slotConvertSceneCoordToGlobal(QPointF &);
    void slotSetActiveForScene(bool on_off);

protected slots:
    void slotAltReload();

protected:
    const uint m_visualizerId;
    const SceneDescriptor& m_sceneDescriptor;
    const QSize m_tileSize;
    const QSize m_tilesCount;
    int m_zoomMin, m_zoomMax;
    QString m_filePath;
    bool m_blockAbortLoading;
    TileScene m_Scene, m_TileScene, m_MinimapScene;
    TileSceneAsinc m_TileSceneAsinc;
//    SceneInThread * m_SceneInThread;
    ParallelCursor* m_parallelCursor;
    QList<ViewerController*> m_viewList;
    uint m_viewCounter;
    const bool m_isGeo;
    QPointF m_DeltaPixFrame;
    double m_Koef;
    int m_BaseZlevel, m_FrameId, m_BaseGeoZlevel;
    BaseSceneLayer* m_HighLightBaseObject;
    int m_curCoordSyst;
    //QPointF m_curCoordPos; // текущая позиция для отображения в виджете
//    dpf::geo::AngleConvertion angleConvertion;
//    dpf::geo::Convertion systemConvert;
    int m_currentZ;
    QRect m_tilesOnViewport;

    bool m_viewOptimization;
    uint m_addTileOnViewport;
    QTimer m_altReloadTimer;
    QPointF m_lastMousePosInView;
    int m_lastZoomInView;
    Qt::ScrollBarPolicy m_policy;

//    dpf::geo::CoordinateSystem getCoordinateType();
    QString coordToString(double data);
    void coordToString2(double dataX, double dataY, QString &xStr, QString &yStr);

protected slots:
    void slotBaseObjectDestroyed();

signals:
    void signalMainViewScaleChanged(qreal, int);
    void signalMainViewAngleChanged(qreal);
    void signal_recivedImage(const QPixmap&, const QString&);
    void signal_mousePressEvent(QMouseEvent const * const e, QPointF scenePos, QPointF wgs84grad);
    void loadingFinished();
    void sigShowCoords(qreal , qreal, int, double);
    void signalShowCoords(QString,QString,QString);
    void signal_getTiles(const QList<visualize_system::URL>& list, int currentZ, QRectF tilesOnViewport);
    void signal_removeTiles(const QStringList& list);
    void signal_abortLoading();
    void readyDraw(QList<QGraphicsItem*>* _list );
    void signalUserViewControlAction(UserAction);
    void signal_doubleClickOnScene(QPointF);
    void signal_clickRBOnRaster(uint id, QPointF geoPos, bool controlModifier);
    void signal_clickRBOnObject(quint64 id);
};

}

#endif



