#ifndef GEOGRAPHICUTILS_H
#define GEOGRAPHICUTILS_H

#include <QPointF>

/**
 * @brief The GeographicUtils class - класс для перевода географических координат в координаты гео-сцены и обратно,
 * а так же различных измерений в координатной системе WGS84.
 * Во всех методах географические координаты описаны следующим образом :
 *  wgs84Point.x() - долгота, ось X,
 *  wgs84Point.y() - широта, ось Y.
 *  SceneW и SceneH - ширина и высота "мира" на гео-сцене в еденицах сцены.
 */
class GeographicUtils
{
public:

    static double getDistanceBetweenWgs84Points(const QPointF &firstWgs84Point, const QPointF &secondWgs84Point);

    static QPointF convertSceneToWgs84(const QPointF &scenePos);

    static QPointF convertWgs84ToScene(const QPointF &wgs84Pos);

    static double deg_rad(double x);

    static double rad_deg(double x);

    static double meterInSceneCoord(const QPointF &scenePos);

private:

    static double SceneW;
    static double SceneH;
};

#endif // GEOGRAPHICUTILS_H
