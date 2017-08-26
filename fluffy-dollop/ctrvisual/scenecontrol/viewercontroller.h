#ifndef VIEWERCONTROLLER_H
#define VIEWERCONTROLLER_H

#include <QGraphicsView>
#include <QWheelEvent>
#include <QPushButton>
#include "tilescene.h"
#include "viewportcontroller.h"
#include "minimapviewer.h"
#include "parallelcursor.h"
#include <QPointF>
#include <QGraphicsItem>
#include <ctrvisual/state/stateobject.h>
#include <ctrcore/visual/basescenelayer.h>
#include <QStack>
#include <QTimer>

class AngleGraphicsItem;
class RouteScaleGraphicsItem;
class SquareGraphicsItem;
class SearchRectGraphicsItem;
class ReachGraphicsItem;
class LineResolutionItem;

namespace visualize_system {

class SceneController;

enum ViewerState
{
    ScrollOnly,
    CenterOnObject, // attach - центрирование на объекте
    DenyScrollOverAndSelect, // BindObject
    SelectLayersAndScroll,
    SelectScroll,
    SelectLayers_RectArea,
    EditObjects,
    MeasureAngles,
    MeasureLength,
    MeasureRoute,
    MeasureSquare,
    MeasureLineResolution,
    MeasureReach,
    StateObjectHandlerEvents // универсальное состояние при котором все события отдаются на обработку некому внешнему объекту унаследованному от "StateObject".
};

enum MoveMapSignalSource
{
    MMSS_NO_SENDER,
    MMSS_KOMPAS, // заказчик перемещения - компачс
    //MMSS_SCREEN_BOUND, // заказчик перемещения - наведение мыши на границы экрана
    MMSS_KEYS
};

class TestObj : public QGraphicsRectItem
{
public:
    TestObj(QPointF pos, QGraphicsScene* scene)
        : QGraphicsRectItem(QRectF(0,0,10,10))
    {
        scene->addItem(this);
        setPos(pos);
        QPen pen(Qt::red);
        pen.setWidth(1);
        pen.setCosmetic(true);
        setPen(pen);
        setBrush(QBrush(Qt::gray));
        setFlag(QGraphicsItem::ItemIsSelectable, true);
        setFlag(QGraphicsItem::ItemIsMovable, true);
        setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
        setZValue(1000000);
    }

    virtual int type()const {return 666666;}
};

class BackgroundItem : public QGraphicsPixmapItem
{
public:
    BackgroundItem(QGraphicsScene *scene) : image(QSize(), QImage::Format_ARGB32)
    {
        scene->addItem(this);
    }
    QRectF sceneRect;
    int zLevel;
    QImage image;

//protected:
//    void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
//    {
//        QGraphicsPixmapItem::paint(painter, option, widget);
//        QPen pen(Qt::red);
//        pen.setWidth(3);
//        pen.setCosmetic(true);
//        painter->setPen(pen);
//        painter->drawRect( 0, 0, boundingRect().width(), boundingRect().height() );
//    }
};


class ViewerController : public QGraphicsView
{
    class SelectRectArea : public QGraphicsPolygonItem
    {
    public:
        enum ObjectSelection
        {
            RastersLayers,
            ObjreprLayers
        };

    private:
        const ObjectSelection _objectSelection;
        QRect _rect;

    public:
        SelectRectArea(ObjectSelection objectSelection) : _objectSelection(objectSelection) {}
        ObjectSelection objectSelection() {return _objectSelection;}
        void setSelectRect(QRect rect_) {_rect = rect_;}
        QRect getSelectRect() {return _rect;}

        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0)
        {
            painter->save();
            QPen pen(Qt::black);
            pen.setCosmetic(true);
            pen.setWidth(2);
            painter->setPen(pen);
            painter->setBrush(Qt::NoBrush);
            painter->drawPolygon(polygon());
            pen.setColor(Qt::yellow);
            pen.setStyle(Qt::DotLine);
            painter->setPen(pen);
            painter->drawPolygon(polygon());
            painter->restore();
        }
    };

    struct ViewPortArea
    {
        QPixmap pixmap;
        QRectF rect;
    };

    Q_OBJECT

    bool m_allowBoundMoveMap;
    const int m_middleZoomLevel;
    QList<QGraphicsItem*> m_selectedEditPoints;
    MoveMapSignalSource m_moveSource;
    int m_cutterTaskId, m_percent;
    QString m_cuttedFilename;

    quint64 m_selectedFromMapid;
    QRect m_screenRectWithoutBounds;
    QRect m_leftBoundRect, m_rightBoundRect, m_topBoundRect, m_bottomBoundRect;
    int m_screenBounds;

    quint64 m_browserId, m_markId;
    int m_zoomMax, m_zoomMin;
    SceneController * m_sceneControlle;
    ViewPortController * m_viewPortController;
    BaseSceneLayer * pEditedBaseSceneLayer;
    BaseSceneLayer* m_pBindedLayer;
    BaseSceneLayer * m_pViewRectPartLayer;
    double m_scale;
    int m_zoom;
    qreal  m_angle;
    MiniMapViewer* m_miniView;
    const uint MINI_VIEW_Z;
    const uint m_viewNumber;
    bool m_mapMovingFromKompas;
    QTimer * m_continuousMoveTimer, *m_changeZoomDelayTimer;
    int m_zoomForSet; // зум, который надо установить. Если -1 - установка не требуется
    bool m_changeZoomOnCenter;

    QPen m_boundsPen;
    QBrush m_boundsBrush;

    QRectF getViewportRectPart(int dx, int dy);
    QPolygonF getViewportPolygon();
    void updateMiniViewer(const QRectF& r);
    //QPushButton * btn_minimap, *btn_autoScroll, *btn_screenShot, *btn_rowScale, *btn_coordBar;
    bool m_autoScroll, m_centerViewOn;
    QPointF pFirstPointPos, pSecondPointPos;
    ParallelCursor* pParallelCursor1;
    ParallelCursor* pParallelCursor2;
    bool m_blockRotate, m_blockRotateFromKompas, m_blockScrollAndSelect, m_blockSelectAndScrollOver;
    ViewerState m_viewerState, m_prevState;
    const ViewerState m_defaultMode;
    QList<QPointF> m_selectList;
    QPointF m_LeftButPressPos;
    QList<quint64> m_prevObjectsUnderMouse;
    QList<quint64> m_objectsUnderMouseForSelect;

    /*измерения*/

    AngleGraphicsItem * m_pCalcAnglesItem;
    QPolygonF m_calcAnglePoly, m_calcRoutePoly, m_calcSquarePoly, m_calcLengthPoly, m_calcLineResolutionPoly;
    bool m_calcRouteShowTempWay, m_calcLengthShowTempWay;
    RouteScaleGraphicsItem* m_pCalcRouteItem, *m_pCalcLengthItem;
    SquareGraphicsItem* m_pCalcSquareItem;
    LineResolutionItem *m_pCalcLineResolutionItem;

    QDialog * m_pAskRadiusDlg;
    qreal m_reachRadius;
    qreal m_ellipseSelectRadius;
    qreal m_ellipseSelectRadWithRad;
    ReachGraphicsItem * m_pReachGraphicsItem, *m_pSelectEllipseAreaItem, *m_pSelectEllipseAreaWithSetRadItem;
    QList<quint64> m_choosedFromEllipseSelectIds, m_pchoosedFromEllipseAreaWithSetRadIds;

    /*зона поиска OSM*/
    SearchRectGraphicsItem* m_pSearchRectItem;
    QPolygonF m_searchRectPoints;

    //QTimer * m_pSendEventTimer;
    //UserAction m_action;
    bool m_waitMoveTimerCompleted;
    QPoint m_pressedMousePos;
    QCursor m_addVectorObjectCursor;
    bool m_leftButtonPressedWithoutMods;
    QTimer * m_pWaitMapMoveTimer;
    /*выделение зоны поиска*/
    SelectRectArea* m_pSelectAreaRect;
    QRectF m_areaFroSearchRect;

    QList<QGraphicsLineItem*> m_SelectArea_LineList, m_SelectArea_BlackLineList;
    QList<QPointF> m_SelectArea_PointList;
    QPointF m_lastGoodViewportPosForBind, m_bindingLayerPrevCenterPos;
    QPointF m_bindingLayerIntersectPoint ;
    qreal m_lenBetweenLayerAndViewCenter;
    const static int viewportBindingPartPx = 50;

    ViewPortArea m_ViewPortArea;
    QTimer m_timer;
    const uint TIME_CONST;
    BackgroundItem * _backgroundItem;
    QSharedPointer<StateObject> m_stateObject;
    QStack< QSharedPointer<StateObject> > m_stateObjectStack;

public:
    ViewerController(SceneController *sceneControlle, uint viewNumber, QWidget* parent = 0);
    virtual ~ViewerController();
    void allowBoundMoveMap(bool res);
    MiniMapViewer* getMinimap() {return m_miniView;}
    void updateContent(bool deepRepaint);
    void centerViewOn(QPointF scenePos);
    void centerViewOnWithZoom(quint64 id, QRectF rect, int minVizZoom, int maxVizZoom, bool withZoom, bool force = false);
    bool setViewCenterOnObject(quint64 id);
    QGraphicsScene* getScene();
    void setPreview(bool on_off);
    SceneController* const getSceneController() {return m_sceneControlle;}
    QString getViewName() {return tr("View") + " " + QString::number(m_viewNumber);}
    qreal getIntepixelDistance();
    void centerViewOnWithCorrection(QPointF scenePos);
    void getPoint(QCursor *cursor);
    void finish_getPoint();
    void setEditMode(bool on_off, QPixmap pm);
    int getPrefferZoomForSceneRect(QRectF rect, int minVizZoom, int maxVizZoom, BaseSceneLayer::SceneLayerType slt = BaseSceneLayer::SLT_UNKNOWN);
    void setMinMaxZoom(int zoomMin, int zoomMax);
    int getCurrentZoomeLevel() {return m_zoom;}
    qreal getScaleForZoom(int zoomLevel);
    QRectF getViewportRect();
    static void convertMetersToNormString(qreal meters, QString &valueRes, QString &dimensionStr);
    static void convertMetersSquareToNormString(qreal meters, QString& valueRes, QString& dimensionStr);
    void setPrefferZoomForGeoRect(QRectF rect);
    bool setState(QSharedPointer<visualize_system::VisualizerStateObject> stateObject);

private:
    void moveMap(double partOfScreen);
    void selectArea(QPolygonF pol);
    //void selectReachArea(QPolygonF poly);
    void selectEllipseArea(QPolygonF poly);
    void selectEllipseSetRadArea(QPolygonF poly);
    bool moveBindedLayerInViewRect();
    void blockRotate(bool);
    void setItemsScale();

    /**
     * @brief Вызывается при переключении режима. Определением нужного режима занимается функция slotDetectMode;
     *  она же и вызывает slotSwitchMode;
     * @param mode - режим, устанавливается функцией slotDetectMode
     * @param params - параметры. паттерн соглашение.
     * @param modeOnOff - включение или выключение режима. Если режим однопроходовый(снимается по окончанию slotSwitchMode),
     *  то значение данного параметра не учитывается.
     */
    void switchViewerStateMode(ViewerState mode, bool onOff, QVariant params = QVariant() );

protected:
    virtual void scrollContentsBy(int dx, int dy);
    virtual void resizeEvent(QResizeEvent* e);
    virtual void wheelEvent(QWheelEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent *e);
    virtual void mouseDoubleClickEvent(QMouseEvent *e);
    virtual void keyReleaseEvent(QKeyEvent * e);
    virtual void keyPressEvent(QKeyEvent * e);
    virtual void closeEvent(QCloseEvent * e);
    virtual void showEvent(QShowEvent *e);
//    virtual void drawForeground(QPainter *painter, const QRectF &rect);


    bool eventFilter(QObject *obj, QEvent *event);

    /**
     * @brief Функция для проверки положения мыши и определения необходимости прокрутки карты
     * @param pos - позиция мыши в координатах виджета
     * @param orient - вычисленное направление перемещения
     */
//    bool checkMousePos(QPoint pos, Kompas::KompasOrientation & orient);

public slots:
    void slotBlockScrollAndSelect(bool isBlocked);
    void slotBindLayer(quint64 layerId, bool isBinded);

    /**
     * @brief Сюда прилетают хотелки на установку необходимого режима(не конкретного ViewerState, а не, например, ID нажатой кнопки, ).
     * Функция должна определить необходимый
     * режим и вызвать switchViewerStateMode. В случае, если switchViewerStateMode возвращает true, то смена режима произведена успешно,
     *  иначе slotDetectMode должна сообщить о невозможности перехода в запрашиваемый режим источнику.
     */
    void slotDetectViewerMode(QString cmd, QVariant var = QVariant());


    /**
     * @brief Слот для установки Z уровня, при котором расстояние между двуми соседними пикселями соответсвует iPixelDistance метрам
     * @param iPixelDistance - количество метров между двумя соседними пикселями
     */
    void slotSetCurrentInterpixelDistance(qreal iPixelDistance);
    void changeZoom(int, bool withDelay = true);
    void autoScroll(QPointF scenePos);
    void viewCenterOn(QPointF scenePos);

    /**
     * @brief Change mode from VectorRender
     * @param cursorId - objrepr cursor id
     * @param isEditMode - true, if edit mode enabled
     * @param pm - pixmap for cursor
     */
    void slotChangeEntityMode(quint32 cursorId, bool isEditMode, QPixmap pm);
    void setPrefferZoomForSceneRect(QRectF rect);

protected slots:
    void slotWaitReleaseMouseTimeout();
    void slot_sceneCenterOn(QPointF);
   // void zoomIn();
   // void zoomOut();
    void slotBlockRotateFromKompas(bool blockRotate);
    void slotRotateFromKompas(qreal angle);
    void slotSetAutoScroll();
    void slotChangeZoom(int);
    void slotChangeZoomTimeout();
//    void slotScreenShot();
//    void slotOneStepMoveMap(Kompas::KompasOrientation);
//    void slotContinuousMoveMap(Kompas::KompasOrientation);
    void slotStopMoveMap();
    void slotMoveTimerTimeout();
    void slotDebugOut();
    void slotCreatingMode(bool on_off);
    void slotSceneWidgetButtonClicked(int buttonId, bool isPressed);
    void slotSetLayerToCenterViewport(QPointF pos);
//    void slotSaveViewportImage();
    void slotGeoImageCutted(int cutterTaskId, int percent);

    /**
     * @brief Слот, куда попадают WGS координаты зоны измерения линейного разрешения
     * @param polyPoints - точки полигона(WGS)
     * @param square - измеренное значение должно быть записано сюда
     */
    void slotGetLineResolution(QPolygonF polyPoints,qreal* square);

    /**
     * @brief Считает длину линии в м для измерителя расстояний
     * @param line - линия в координатах сцены
     * @param dist - переменная в которую будет записан результат
     */
    void slotGetRealDistanceMeters(QLineF line ,qreal* dist);

    /**
     * @brief Слот для подсчета площади полигона
     * @param polyPoints - точки полигона в координатах сцены
     * @param square - переменная для записи результата
     */
    void slotGetRealSquareMeters(QPolygonF polyPoints,qreal* square);

    /**
     * @brief Слот считает угол между прямыми
     * @param Первая прямая(базовая) в координатах сцены. Принимается за 0 ;
     * @param Вторая прямая в координатах сцены.
     * @param angle - переменная для записи результата
     */
    void slotGetRealAngle(QLineF l1, QLineF l2, qreal* angle);

    /**
     * @brief Слот вызывается при завершении построения области поиска пользователем
     * @param Область поиска в координатах сцены
     */
    void slotSearchRectCreationCompleted(QRectF searchRect, quint64 brId);

    void slotCheckUpdate();
    void slotUpdateBackgroundItem(QRect);
    void slotFinishRender(QImage* image, QRectF sceneRect, int zLevel);
    void slotWheelEvent(QWheelEvent* e);
    void slotStateObjectHandlerEventsOff();

signals:
    void updateFromScroll(int zoom, QRectF visibleRect, bool);
    void updateFromResize(int zoom, QRectF visibleRect, bool);
    void updateFromZoom(int zoom, QRectF visibleRect, bool);
    void signalSetAngle(qreal);
    void signalScaleChanged(qreal, int);
    void signal_mousePressEvent(QMouseEvent const * const, QPointF, QPointF);
    void signalScaleChanged(int, int,int);
    void curDistanceUpdateted(bool showDistance, double distValue);
    void sigSetInterpixelDistance(qreal mInPixel);
    void signalRotateBlocked(bool blockRotate);
    void signalResized(QSize);
    void signalSelectObject(quint64);
    void signalSwitchOffButton( int buttonId);
    void signalViewportRectChanged(QRectF);
    void signalSetSelectedEditPoints(QList<QGraphicsItem*>);

    void signalSendMarkCoords(QPointF geoCoords, quint64 browserId, quint64 markId);
    /**
     * @brief Сигнал об окончании выделения области поиска пользователем
     * @param wgsRect
     */
    void signalFindOSMObjectsInRect(QRectF wgsRect, bool isPoint, quint64, bool);
};

}

struct StateObjectInQVariant
{
    QSharedPointer<visualize_system::StateObject> stateObject;
};

Q_DECLARE_METATYPE(StateObjectInQVariant)

#endif
