#ifndef STATEOBJECT_H
#define STATEOBJECT_H

#include <ctrcore/visual/stateinterface.h>
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QKeyEvent>

namespace visualize_system
{

class StateObject : public VisualizerStateObject
{
    Q_OBJECT
public:
    explicit StateObject();
    virtual ~StateObject();

    virtual void init(QGraphicsScene * scene, QGraphicsView * view, const int * zoom, const double * scale, double frameCoef, uint visualizerId);
    void emit_closeState();
    // при return true - выполнение в обработчике вьюхи передается дальше, при return false - выходит из обработчика
    virtual bool mouseMoveEvent(QMouseEvent* e, QPointF scenePos) {return true;}
    virtual bool mousePressEvent(QMouseEvent* e, QPointF scenePos) {return true;}
    virtual bool mouseReleaseEvent(QMouseEvent *e, QPointF scenePos) {return true;}
    virtual bool mouseDoubleClickEvent(QMouseEvent *e, QPointF scenePos) {return true;}
    virtual bool wheelEvent(QWheelEvent* e, QPointF scenePos) {return true;}
    virtual bool keyReleaseEvent(QKeyEvent * e) {return true;}
    virtual bool keyPressEvent(QKeyEvent * e) {return true;}
    virtual void zoomChanged() {}
    virtual void setData(const QString& tag, QVariant& value) {}
    virtual bool stateInited();

    virtual void statePushedToStack() = 0;
    virtual void statePoppedFromStack() = 0;

    void convertSceneToNative(QPointF & pos);
    void convertNativeToScene(QPointF & pos);

    void convertEpsgToScene(int epsgCode, QPointF & pos, bool & success);
    void convertSceneToEpsg(int epsgCode, QPointF & pos, bool & success);

    QGraphicsView * getGraphicsView();
    QGraphicsScene * getGraphicsScene();
    void setZlevel(int);
    double getCoef() const;
    double lenght(const QPointF & p1, const QPointF & p2) const;
    double lenght(const QPoint & p1, const QPoint & p2) const;
    double lenghtInViewScale(const QPointF & p1, const QPointF & p2) const;

    //! нужно ли проверять объекты под мышкой
    bool checkableObjectUnderMouse();
    //! если метод вернул "TRUE" - объект будет подсвечен, курсор вьюхи будет "cursor",
    //! если курсор не нужен - не изменяем.
    virtual bool checkObjectUnderMouse(quint64 obgectId, QCursor & cursor) {return false;}
    virtual bool objectChoiced(quint64 obgectId) {return false;}

    //! нужно ли проверять растры под мышкой
    bool checkableRasterUnderMouse();
    virtual void rasterUnderMouse(uint rasterId) {}

    //! нужно ли показывать контекстное меню при щелчке на объекте правой кнопкой мыши
    bool showContextRasterMenu();
    bool showContextObjectMenu();

    void setPrefferZoomForSceneRect(QRectF sceneRect);

    void setActiveForScene(bool on_off);

protected:
    //! вызывается перед разрушением вьюхи. При наличии у объекта-состояния указателей на графические объекты на сцене их необходимо удалить
    //! в этом слоте, так как в деструкторе объекта-состояния будут уже "битые" указатели на них (их удалит сцена при разрушении до вызова деструктора объекта-состояния).
    virtual void viewDestroyed() {}

signals:
    // испускается объектом перед удалением из деструктора
    void signalBeforeDelete();

    // вызывается из метода "emit_closeState", для удаления объекта и смены состояния на дефолтное
    void signalCloseState();

    // вызывается из метода "init" базового класса, до этого "stateInited()" возвращает "false", после "true".
    void signalStateInited();

    // сигналы соединяются со слотами "ViewController".
    void signalConvertSceneToNative(QPointF & pos);
    void signalConvertNativeToScene(QPointF & pos);
    void signalConvertEpsgToScene(int epsgCode, QPointF & pos, bool & success);
    void signalConvertSceneToEpsg(int epsgCode, QPointF & pos, bool & success);
    void signalSetZlevel(int);
    void signalSetActiveForScene(bool);
    void signalSetPrefferZoomForSceneRect(QRectF sceneRect);

protected:
    QGraphicsScene * _scene;
    QGraphicsView * _view;
    const int * _zoom;
    const double * _scale;
    double _frameCoef;
    uint _visualizerId;
    bool _checkableObjectUnderMouse;
    bool _checkableRasterUnderMouse;
    bool _showContextObjectMenu;
    bool _showContextRasterMenu;

protected slots:
    void slotViewDestroyed(QObject *);

};

}

#endif
