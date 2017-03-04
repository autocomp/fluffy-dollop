#ifndef GEOSCENEDESCRIPTOR_H
#define GEOSCENEDESCRIPTOR_H

#include "scenedescriptor.h"
#include <QRectF>
#include <QString>

class OGRCoordinateTransformation;
class OGRSpatialReference;

namespace visualize_system
{

/**
 * @brief The GeoSceneDescriptor class Класс обеспечивает конвертацию между координатами WGS84 и координаты сцены
 */
class GeoSceneDescriptor : public SceneDescriptor
{
public:
    GeoSceneDescriptor(const QString & xml, const QString &gridName, uint baseTileSetOrder = 1);
    virtual ~GeoSceneDescriptor();
    /**
     * @brief convertNativeToScene - конвертация координат из WGS84 в координаты сцены.
     * @param x - на входе : для геосцены - долгота. На выходе : координата X сцены.
     * @param y - на входе : для геосцены - широта. На выходе : координата Y сцены.
     * @return - успешность выполнения преобразования.
     */
    virtual bool convertNativeToScene(double &x, double &y) const;
    /**
     * @brief convertSceneToNative - конвертация координат из координат сцены в координаты WGS84.
     * @param x - на входе : координата X сцены. На выходе : для геосцены - долгота.
     * @param y - на входе : координата Y сцены. На выходе : для геосцены - широта.
     * @return - успешность выполнения преобразования.
     */
    virtual bool convertSceneToNative(double &x, double &y) const;
    /**
     * @brief convertFromRefSystemToScene - конвертация координат из системы "refEpsgSystemCoord()" в координат сцены.
     * @param x - на входе : координата в системе "refEpsgSystemCoord()". На выходе : координата X сцены.
     * @param y - на входе : координата в системе "refEpsgSystemCoord()". На выходе : координата Y сцены.
     * @return - успешность выполнения преобразования.
     */
    virtual bool convertRefSystemToScene(double &x, double &y) const;
    /**
     * @brief convertSceneToRefSystem - конвертация координат из координат сцены в координаты системы "refEpsgSystemCoord()".
     * @param x - на входе : координата X сцены. На выходе : координата в системе "refEpsgSystemCoord()".
     * @param y - на входе : координата Y сцены. На выходе : координата в системе "refEpsgSystemCoord()".
     * @return - успешность выполнения преобразования.
     */
    virtual bool convertSceneToRefSystem(double &x, double &y) const;
    /**
     * @brief convertEpsgToScene - конвертация координат из системы "epsgCode" в координат сцены.
     * @param x - на входе : координата в системе "epsgCode". На выходе : координата X сцены.
     * @param y - на входе : координата в системе "epsgCode". На выходе : координата Y сцены.
     * @return - успешность выполнения преобразования.
     */
    virtual bool convertEpsgToScene(int epsgCode, double &x, double &y) const;
    /**
     * @brief convertSceneToEpsg - конвертация координат из координат сцены в координаты системы "epsgCode".
     * @param x - на входе : координата X сцены. На выходе : координата в системе "epsgCode".
     * @param y - на входе : координата Y сцены. На выходе : координата в системе "epsgCode".
     * @return - успешность выполнения преобразования.
     */
    virtual bool convertSceneToEpsg(int epsgCode, double &x, double &y) const;
    /**
     * @brief refSystemCoord - систма координат в которой работает адаптер (EPSG).
     * @return - систма координат (EPSG).
     */
    virtual int refEpsgSRS() const;
    /**
     * @brief refProjSRS - систма координат в которой работает адаптер (Proj).
     * @return - систма координат (Proj).
     */
    virtual QString refProjSRS() const;
    /**
     * @brief isValid - метод возвращает валидность дескриптора.
     * @return - валидность дескриптора.
     */
    virtual bool isValid() const;
    virtual QSize tileSize() const;
    virtual QSize sceneSizeInTiles() const;
    virtual QSize sceneSize() const;
    virtual bool isGeoScene() const;
    virtual bool isPixelScene() const;

private:
    bool _isValid;
    int _refEPSG;
    QString _refProj;
    QRectF _bbox;
    QPointF _origin;
    double _unitsPerPixel;
    QSize _sceneInTiles, _tileSize;
    OGRCoordinateTransformation * _trans, * _revTrans;
    OGRSpatialReference * _srsGeogSrc, * _srsGeogTrg;
};

}
#endif
