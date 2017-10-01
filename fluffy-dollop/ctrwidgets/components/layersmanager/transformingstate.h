#ifndef TRANSFORMINGSTATE_H
#define TRANSFORMINGSTATE_H

#include <ctrvisual/state/scrollbasestate.h>
#include <QGraphicsLineItem>
#include <QPixmap>
#include <QStack>
#include "commontypes.h"

namespace transform_state
{

enum class StateMode
{
    ScrollMap,
    TransformImage,
    CropImage,
    GetColorOnImage,
    GetAreaOnImage
};

enum class HandleType
{
    Invalid = 0,
    TopLeft,
    TopCenter,
    TopRight,
    RightCenter,
    BottomRight,
    BottomCenter,
    BottomLeft,
    LeftCenter,
    Rotater,
    Anchor,
    UserHandle
};

struct UndoAct               // структара описывает состояние системы для восстановления при откате выполненной операции.
{
    QString filePath;        // путь к текущему изображению.
    QPointF scenePos;        // позиция левого верхнего угла изображения на сцене.
    double scaleW=1;         // расстяжение изображения по оси X на сцене.
    double scaleH=1;         // расстяжение изображения по оси Y на сцене.
    double rotation=0;       // угол поворота изображения на сцене.
    QPointF anchorPos;       // если есть значение, устанавливаем хендл якоря в эту точку (в координатах элемента). Если нет - устанавливаем в центр.
    // double rotaterLenght=-1; // если >0, устанавливаем хендл поворота на это расстояние от хендла якоря. Если <0 - устанавливаем на 0.75 от хендла якоря.
};

class TransformingItem;
class HandleItem;
class AnchorHandleItem;
class RotaterHandleItem;
class LineItem;
class RotaterLineItem;
class FogItem;

class TransformingState : public ScrollBaseState
{
    Q_OBJECT
public:
    TransformingState(const QPixmap & pixmap, const QString &rasterFileBaseName, QPointF scenePos, int zValue, double originalScale = -1);
    TransformingState(const QPixmap & pixmap, const QString &rasterFileBaseName, QPointF scenePos, double scW, double scH, double rotation, int zValue);

    TransformingState(const QPolygonF & polygon, const QRectF &bRectOnMapScene, const QTransform &transformer, int zValue);

    TransformingState(const QString &svgFilePath, QPointF scenePos, int zValue, double originalScale = -1);
    TransformingState(const QString &svgFilePath, QPointF scenePos, double scW, double scH, double rotation, int zValue);

    ~TransformingState();
    virtual void init(QGraphicsScene *scene, QGraphicsView *view, const int *zoom, const double *scale, double frameCoef, uint visualizerId);
    virtual bool wheelEvent(QWheelEvent* e, QPointF scenePos);
    virtual bool mouseMoveEvent(QMouseEvent *e, QPointF scenePos);
    virtual bool mousePressEvent(QMouseEvent *e, QPointF scenePos);
    virtual bool mouseReleaseEvent(QMouseEvent *e, QPointF scenePos);
    virtual bool mouseDoubleClickEvent(QMouseEvent *e, QPointF scenePos);
    virtual QString stateName();
    virtual void statePushedToStack();
    virtual void statePoppedFromStack();

    /**
     * @brief changeImageColor - изменение или заливка цветом изображения (или его области, при наличии).
     * @param inColor - устанавливаемый цвет, при inColor = QColor() производится замена на полностью прозрачный черный пиксель.
     * @param outColor - замещаемый цвет, при outColor = QColor() производится заливка всей области цветом inColor,
     *        если (inColor = QColor() && outColor = QColor()) - производится заливка всей области полностью прозрачным черным пикселем.
     * @param sensitivity - порог чувствительности, максимальное расстояние между точкам входного и выходного цвета в координатах трёх векторов цветов (RGB), задается в диапазоне от [1..50].
     */
    void changeImageColor(QColor inColor, QColor outColor, int sensitivity = 1);
    int getTransformingItemType() const;
    void setGlobalMode(GlobalMode mode);
    GlobalMode getGlobalMode();
    void setMode(StateMode stateMode);
    StateMode mode();
    void setTransparentBackgroundForPixmapItem(bool on_off);
    void setOpacity(double val);
    void setTransparency(int percent);
    QPolygonF getPixmapCorners();
    bool changed();
    QRectF getCropArea();
    bool cropItem();
    void clearAreaOnImage();
    void setAreaOnImagefinish();
    void undoAction();
    UndoAct getCurrentParams();
    void getCurrentVertex(QPolygonF &vertex, double &pixmapWidth, double &pixmapHeight);

    // все методы ниже вызываются только из хендлов !
    void pressHandle(HandleType handleType);
    void anchorMoved();
    void anchorReplaced();
    void createUserHandler(QPointF scenePos);
    void resendColor(QColor color);
    void pixmapMoved();

signals:
    void signalItemChanged();
    void signalSendColor(QColor color);
    void signalAreaSetted();
    
public slots:

protected:
    double lenght(QPointF p1, QPointF p2);
    double lenght(QColor c1, QColor c2);
    void setHandlesVisible(bool on_off);
    LineItem* createAreaLineItem(QPointF p1, QPointF p2);

    const int _transformingItemType;
    GlobalMode _globalMode=GlobalMode::AllAction;
    StateMode _stateMode=StateMode::TransformImage;
    QPixmap _pixmap;
    QPolygonF _polygon;
    QString _svgFilePath;
    const QString _rasterFileBaseName;
    QPointF _scenePos;
    const int _zValue;
    bool _blockWheelEvent=false;
    TransformingItem * _transformingItem=nullptr;
    HandleItem * _handleItemBottomRight, * _handleItemTopCenter, * _handleItemTopLeft, * _handleItemRightCenter, * _handleItemTopRight, * _handleItemBottomCenter, * _handleItemLeftCenter, * _handleItemBottomLeft;
    AnchorHandleItem * _handleItemAnchor;
    RotaterHandleItem * _handleItemRotater;
    RotaterLineItem * _rotaterLine;
    LineItem * _topLine = nullptr;
    LineItem * _bottomLine = nullptr;
    LineItem * _leftLine = nullptr;
    LineItem * _rightLine = nullptr;
    QPointF _lastScenePos, _userHandlePos;
    HandleType _currentHandleType = HandleType::Invalid;
    QList<HandleItem*> _handleItems;
    double _originalScale=-1, _scW=-1, _scH=-1, _rotation=0, _scWmax, _scHmax, _originW=1, _originH=1, _userHandleLenght, _userHandleSCW, _userHandleSCH;
    QRectF _cropArea;
    FogItem * _fogItem=nullptr;
    QList<LineItem*> _areaLineItems;
    QPolygonF _areaOnImage;
    QStack<UndoAct> _undoStack;
    QRectF _bRectOnMapScene;
    QTransform _transformer;
};

}

#endif







