#include "stateobject.h"
#include <cmath>
#include <QDebug>

using namespace visualize_system;

StateObject::StateObject()
    : _scene(0)
    , _view(0)
    , _zoom(0)
    , _frameCoef(1)
    , _visualizerId(0)
    , _checkableObjectUnderMouse(false)
    , _checkableRasterUnderMouse(false)
    , _showContextRasterMenu(false)
    , _showContextObjectMenu(false)
{
}

StateObject::~StateObject()
{
    emit signalSetActiveForScene(false);
    emit signalBeforeDelete();
}

void StateObject::emit_closeState()
{
    emit signalCloseState();
}

void StateObject::init(QGraphicsScene * scene, QGraphicsView * view, const int * zoom, const double *scale, double frameCoef, uint visualizerId)
{
    _scene = scene;
    _view = view;
    _zoom = zoom;
    _scale = scale;
    _frameCoef = frameCoef;
    _visualizerId = visualizerId;
    connect(_view, SIGNAL(destroyed(QObject*)), this, SLOT(slotViewDestroyed(QObject*)));

    emit signalStateInited();
}

bool StateObject::stateInited()
{
    return _scene != 0 && _view != 0;
}

void StateObject::convertSceneToNative(QPointF & pos)
{
    emit signalConvertSceneToNative(pos);
}

void StateObject::convertNativeToScene(QPointF & pos)
{
    emit signalConvertNativeToScene(pos);
}

void StateObject::convertEpsgToScene(int epsgCode, QPointF & pos, bool & success)
{
    emit signalConvertEpsgToScene(epsgCode, pos, success);
}

void StateObject::convertSceneToEpsg(int epsgCode, QPointF & pos, bool & success)
{
    emit signalConvertSceneToEpsg(epsgCode, pos, success);
}

QGraphicsView * StateObject::getGraphicsView()
{
    return _view;
}

QGraphicsScene * StateObject::getGraphicsScene()
{
    return _scene;
}

double StateObject::getCoef() const
{
    return pow(2, ((*_zoom)-1));
}

void StateObject::setZlevel(int zVal)
{
    emit signalSetZlevel(zVal);
}

double StateObject::lenght(const QPointF & p1, const QPointF & p2) const
{
    return sqrt( (p1.x()-p2.x())*(p1.x()-p2.x()) + (p1.y()-p2.y())*(p1.y()-p2.y()) );
}
double StateObject::lenght(const QPoint & p1, const QPoint & p2) const
{
    return sqrt( (p1.x()-p2.x())*(p1.x()-p2.x()) + (p1.y()-p2.y())*(p1.y()-p2.y()) );
}

double StateObject::lenghtInViewScale(const QPointF & p1, const QPointF & p2) const
{
    return lenght(p1, p2) * getCoef();
}

void StateObject::slotViewDestroyed(QObject* obj)
{
    qDebug() << "StateObject::slotViewDestroyed, this :" << this << ", obj :" << obj;
    viewDestroyed();
}

bool StateObject::checkableObjectUnderMouse()
{
    return _checkableObjectUnderMouse;
}

bool StateObject::checkableRasterUnderMouse()
{
    return _checkableRasterUnderMouse;
}

bool StateObject::showContextRasterMenu()
{
    return _showContextRasterMenu;
}

bool StateObject::showContextObjectMenu()
{
    return _showContextObjectMenu;
}

void StateObject::setActiveForScene(bool on_off)
{
    emit signalSetActiveForScene(on_off);
}









