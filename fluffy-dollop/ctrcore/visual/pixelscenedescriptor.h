#ifndef PIXELSCENEDESCRIPTOR_H
#define PIXELSCENEDESCRIPTOR_H

#include "scenedescriptor.h"
#include <QRectF>
#include <QString>
#include <QSharedPointer>
//#include <dpf/geo/geoimage_object.h>

namespace visualize_system
{

/**
 * @brief The PixelSceneDescriptor class Класс обеспечивает конвертацию между координатами из матрицы изображения и координаты сцены
 */
class PixelSceneDescriptor : public SceneDescriptor
{
public:
    PixelSceneDescriptor(QSize _sceneInTiles = QSize(2,2), QSize _tileSize = QSize(256,256));
    virtual ~PixelSceneDescriptor();
    /**
     * @brief init - метод вызывается из миксера после добавления в него провайдера.
     * @param gio - объект предоставляющий доступ к файлу, в том числе для перевода координат пиксель\гео и наоборот.
     */
    void init(QSize rasterSize); //QSharedPointer<dpf::io::GeoImageObject> gio);
    /**
     * @brief convertNativeToScene - конвертация координат из матрицы изображения в координаты сцены.
     * @param x - на входе : координата X матрицы изображения. На выходе : координата X сцены.
     * @param y - на входе : координата Y матрицы изображения. На выходе : координата Y сцены.
     * @return - успешность выполнения преобразования.
     */
    virtual bool convertNativeToScene(double &x, double &y) const;
    /**
     * @brief convertSceneToNative - конвертация координат из координат сцены в координаты матрицы изображения.
     * @param x - на входе : координата X сцены. На выходе : координата X матрицы изображения.
     * @param y - на входе : координата Y сцены. На выходе : координата Y матрицы изображения.
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
     * @brief refSystemCoord - метод не валиден.
     * @return - ноль.
     */
    virtual int refEpsgSRS() const;

    /**
     * @brief refSystemCoord - метод не валиден.
     * @return - QString().
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
    // QSharedPointer<dpf::io::GeoImageObject> _geoImageObject;
    QSize _sceneInTiles, _tileSize;
    QRectF _rasterSceneRect;
    double _coef;
};

}
#endif
