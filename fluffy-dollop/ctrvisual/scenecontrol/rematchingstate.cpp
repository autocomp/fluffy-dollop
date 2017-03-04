#include "rematchingstate.h"
#include <QDebug>
#include <QMessageBox>
#include <QGraphicsLineItem>
#include <QPen>
#include <cmath>

using namespace visualize_system;

RematchingState::RematchingState(uint rasterId, QPolygonF sceneVertex)
    : _rasterId(rasterId)
    , _cursor(Qt::ArrowCursor)
    , _state(DONT_EDIT)
    , _sceneVertex(sceneVertex)
    , _isChanged(false)
{
}

RematchingState::~RematchingState()
{
}

void RematchingState::setRasterScenePolygon(const QPolygonF& polygonInSceneCoords)
{
    _sceneVertex = polygonInSceneCoords;
}

uint RematchingState::getRasterId()
{
    return _rasterId;
}

QPointF RematchingState::convertPolarToDecar(double const & r, double const & fi, const QPointF center)const
{
    return QPointF( center.x() + r * cos( fi * M_PI / 180 ), center.y() + r * sin( fi * M_PI / 180 ) );
}

void RematchingState::convertDecartToPolar(QPointF const & p, double& r, double& fi)const
{
    r = sqrt( p.x() * p.x() +  p.y() * p.y() );
    fi = atan2(p.y(), p.x()) * 180 / M_PI;
    if (fi < 0 )
        fi += 360;
}

double RematchingState::getLenght(const QPointF& p1, const QPointF& p2)const
{
    return sqrt( (p1.x() - p2.x()) * (p1.x() - p2.x()) +  (p1.y() - p2.y()) * (p1.y() - p2.y()) );
}

int RematchingState::normalizeDegree(int degree)
{
    if (degree < 0)
        degree += 360;
    else
        if (degree > 359)
            degree -= 360;

    return degree;
}

bool RematchingState::isChanged()
{
    return _isChanged;
}

bool RematchingState::check(double coef, QPointF p1, QPointF p2)
{
    return sqrt( (p1.x() - p2.x()) * (p1.x() - p2.x()) +  (p1.y() - p2.y()) * (p1.y() - p2.y()) ) <= coef;
}







