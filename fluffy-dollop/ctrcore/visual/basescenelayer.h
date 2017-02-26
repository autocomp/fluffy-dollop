#ifndef BASESCENELAYER_H
#define BASESCENELAYER_H

#include <QGraphicsRectItem>
#include <QMouseEvent>

namespace visualize_system
{

// класс для взаимодействия некой абстракции - логического слоя (содержащего в себе реальные QGraphicsItem'ы) с графической сценой.

class BaseSceneLayer : public QObject, public QGraphicsRectItem
{
    friend class SceneController;
    friend class LayerBase;

    Q_OBJECT

public:
    enum SceneLayerType
    {
        SLT_UNKNOWN,
        SLT_VIEWRECT,
        SLT_POINT_OBJECT
    };


private:
    QRectF m_rect;
    const quint64 ID;
    quint64 parentId;
    SceneLayerType m_layerType;

public:

    // создается извне сцены, если на момент создания не известен размер слоя на сцене - задается позже методом "setRect"
    BaseSceneLayer(QRectF rect, quint64 id);
    ~BaseSceneLayer();


    BaseSceneLayer::SceneLayerType getLayerType();
    // вызывается извне сцены, если ценр слоя находится вне вьюпорта - вьюпорт позиционируется по центру этого слоя
    void setLayerToCenterViewport();
    // вызывается сценой для установки\снятия режима подсветки у слоя
    void setHighLightObject(bool on_off);
    // вызывается сценой для проверки возможности клика на элементе в данной позиции
    bool isClickable(const QMouseEvent * const e, QPointF scenePos, double sceneScale);
    // вызывается сценой для проверки выделения элемента внутри выделенной области на сцене
    bool isSelectable(QRectF sceneRect, QPointF *intersectPoint = 0);
    bool isSelectable(QPolygonF scenePoly, QPointF *intersectPoint = 0);
    // вызывается сценой в режиме редактирования для передачи всех событий мыши со сцены редактируемому объекту
    void setMouseEventInEditMode(const QMouseEvent * const e, QPointF scenePos);
    // возвращает идентификатор слоя для выделения в репрезент-сервере (при наличии родителя - выделится родитель).
    quint64 getId() {return parentId ? parentId : ID;}
    // возвращает идентификатор логического слоя в репрезент-сервере.
    quint64 getMyId() {return ID;}
    // устанавливает идентификатор родителя логического слоя, для более гибкого поведения объектов
    quint64 setParentId(quint64 _parentId) {parentId = _parentId;}
    QRectF getRect();
    void setRect(const QRectF &rect);
    void setSceneLayerType(SceneLayerType _type);

public slots:
    // вызывается логическим слоем, в режиме редактирования объект получает все события мыши со сцены
    void setCreatingMode(bool on_off);

    //! эти сигналы необходимо подключить к логическому слою для интерактивного взаимодействия сцены с ним
signals:
    // сигнал для подключения к объекту логического слоя и определения им возможности клика в заданной точке сцены
    void signalIsClickable(bool& yes_no, const QMouseEvent * const e, QPointF scenePos, double sceneScale);
    // сигнал для подключения к объекту логического слоя и определения им возможности селектирования в заданном прямоугольнике сцены
    void signalIsSelectable(bool& yes_no, QRectF sceneRect);
    void signalGetIntersectPoint(bool& yes_no, QPolygonF sceneRect, QPointF * intersectPoint);
    // сигнал для подключения к объекту логического слоя и получения всех мышиных событий от сцены в режиме редактирования
    void signalMouseEventInEditMode(const QMouseEvent * const e, QPointF scenePos);
    // сигнал для установки\снятия режима подсветки у слоя
    void signalHighLightObject(bool on_off);
    // сигнал отправляется из деструктора
    void signalDestroyed();

signals:
    // сигнал для подключения сцены к объекту и уведомления её об начале\остановки режима редактирования
    void signalSetCreatingMode(bool on_off);
    // сигнал для подключения сцены к объекту для позиционирования вьюпорта по центру слоя
    void signalSetLayerToCenterViewport(QPointF);

public:
    // глушим рисовалку так как объекта как слоя реально не существует
    virtual void paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = 0);
};

}

#endif
