#include "geographicutils.h"
#include <GeographicLib/PolygonArea.hpp>
#include <GeographicLib/Geodesic.hpp>
#include <GeographicLib/Constants.hpp>
#include <math.h>

double GeographicUtils::SceneW = 512;

double GeographicUtils::SceneH = 512;

double GeographicUtils::getDistanceBetweenWgs84Points(const QPointF &firstWgs84Point, const QPointF &secondWgs84Point)
{
    GeographicLib::Geodesic geod(GeographicLib::Constants::WGS84_a(), GeographicLib::Constants::WGS84_f());

    double lon1 = firstWgs84Point.x(); // долгота, ось X
    double lat1 = firstWgs84Point.y(); // широта, ось Y

    double lon2 = secondWgs84Point.x(); // долгота, ось X
    double lat2 = secondWgs84Point.y(); // широта, ось Y

    double s12, azi1, azi2;

    geod.Inverse(lat1, lon1, lat2, lon2, s12, azi1, azi2);
    return s12;
}

QPointF GeographicUtils::convertSceneToWgs84(const QPointF &scenePos)
{
    double lon = (scenePos.x()*(360/SceneW))-180;
    double lat = rad_deg(atan(sinh((1-scenePos.y()*(2/SceneH))*M_PI))); // ( scenePos.y() * (180. / SceneH) ) + 90;
    return QPointF(lon, lat);
}

QPointF GeographicUtils::convertWgs84ToScene(const QPointF &wgs84Pos)
{
    double coordX = (wgs84Pos.x()+180) * SceneW/360.;
    double coordY = (1-(log(tan(M_PI/4+deg_rad(wgs84Pos.y())/2)) /M_PI)) /2  * SceneH; // ( (90. - wgs84Pos.y()) * SceneH ) / 180.;
    return QPointF(coordX, coordY);
}

double GeographicUtils::deg_rad(double x)
{
    return x * (M_PI/180.);
}
double GeographicUtils::rad_deg(double x)
{
    return x * (180./M_PI);
}

double GeographicUtils::meterInSceneCoord(const QPointF &scenePos)
{
    double deltaByScene = 0.001;
    QPointF left = convertSceneToWgs84(scenePos);
    QPointF right = convertSceneToWgs84(QPointF(scenePos.x() + deltaByScene, scenePos.y()));
    double distanceByHorInMeter = getDistanceBetweenWgs84Points(left, right);
    return deltaByScene / distanceByHorInMeter;
}








