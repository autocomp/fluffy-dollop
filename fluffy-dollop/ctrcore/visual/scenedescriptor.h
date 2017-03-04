#ifndef SCENEDESCRIPTOR_H
#define SCENEDESCRIPTOR_H

#include <QSize>

namespace visualize_system
{

/**
 * @brief The SceneDescriptor class - интерфейсный класс описывающий гео или пиксельную тайловую сцену (размер тайла, количество тайлов на минимальнои уровне отображения, референсная система координат).
 * Так же класс производит конвертацию координат между кьютовой графической сценой и гео-координатами или пикселями матрицы.
 */
class SceneDescriptor
{
public:
    SceneDescriptor() {}
    virtual ~SceneDescriptor() {}

    /**
     * @brief convertNativeToScene - конвертация координат из "родной" системы в координаты сцены.
     * @param x - на входе : для геосцены - долгота WGS84, для пиксельной сцены - координата X матрицы изображения. На выходе : координата X сцены.
     * @param y - на входе : для геосцены - широта WGS84, для пиксельной сцены - координата Y матрицы изображения. На выходе : координата Y сцены.
     * @return - успешность выполнения преобразования.
     */
    virtual bool convertNativeToScene(double &x, double &y) const = 0;

    /**
     * @brief convertSceneToNative - конвертация координат из координат сцены в "родные" координаты.
     * @param x - на входе : координата X сцены. На выходе : для геосцены - долгота WGS84, для пиксельной сцены - координата X матрицы изображения.
     * @param y - на входе : координата Y сцены. На выходе : для геосцены - широта WGS84, для пиксельной сцены - координата Y матрицы изображения.
     * @return - успешность выполнения преобразования.
     */
    virtual bool convertSceneToNative(double &x, double &y) const = 0;

    /**
     * @brief convertFromRefSystemToScene - конвертация координат из системы с кодом "refEpsgSystemCoord()" в координат сцены.
     * @param x - на входе : координата в системе с кодом "refEpsgSystemCoord()". На выходе : координата X сцены.
     * @param y - на входе : координата в системе с кодом "refEpsgSystemCoord()". На выходе : координата Y сцены.
     * @return - успешность выполнения преобразования.
     */
    virtual bool convertRefSystemToScene(double &x, double &y) const = 0;

    /**
     * @brief convertSceneToRefSystem - конвертация координат из координат сцены в координаты системы с кодом "refEpsgSystemCoord()".
     * @param x - на входе : координата X сцены. На выходе : координата в системе с кодом "refEpsgSystemCoord()".
     * @param y - на входе : координата Y сцены. На выходе : координата в системе с кодом "refEpsgSystemCoord()".
     * @return - успешность выполнения преобразования.
     */
    virtual bool convertSceneToRefSystem(double &x, double &y) const = 0;

    /**
     * @brief convertEpsgToScene - конвертация координат из системы с кодом  "epsgCode" в координат сцены.
     * @param x - на входе : координата в системе с кодом "epsgCode". На выходе : координата X сцены.
     * @param y - на входе : координата в системе с кодом  "epsgCode". На выходе : координата Y сцены.
     * @return - успешность выполнения преобразования.
     */
    virtual bool convertEpsgToScene(int epsgCode, double &x, double &y) const = 0;

    /**
     * @brief convertSceneToEpsg - конвертация координат из координат сцены в координаты системы с кодом "epsgCode".
     * @param x - на входе : координата X сцены. На выходе : координата в системе с кодом "epsgCode".
     * @param y - на входе : координата Y сцены. На выходе : координата в системе с кодом "epsgCode".
     * @return - успешность выполнения преобразования.
     */
    virtual bool convertSceneToEpsg(int epsgCode, double &x, double &y) const = 0;

    /**
     * @brief refEpsgSRS - систма координат в которой работает дескриптор (EPSG), валидна при ( projectionAdapterIsGeo() || !sceneDescriptorIsPixel()).
     * @return - систма координат (EPSG).
     */
    virtual int refEpsgSRS() const = 0;

    /**
     * @brief refProjSRS - систма координат в которой работает адаптер (Proj), валидна при ( projectionAdapterIsGeo() || !sceneDescriptorIsPixel()).
     * @return - систма координат (Proj).
     */
    virtual QString refProjSRS() const = 0;

    /**
     * @brief tileSize - метод возвращает размер тайла в пикселях для кьютовой графической сцены.
     * @return - размер тайла в пикселях.
     */
    virtual QSize tileSize() const = 0;

    /**
     * @brief sceneSizeInTiles - метод возвращает размер сцены в тайлах.
     * @return - размер сцены в тайлах.
     */
    virtual QSize sceneSizeInTiles() const = 0;

    /**
     * @brief sceneSize - метод возвращает размер сцены (точнее тайлового мира) в еденицах сцены.
     * @return - размер сцены (точнее тайлового мира) в еденицах сцены.
     */
    virtual QSize sceneSize() const = 0;

    /**
     * @brief isGeoScene - метод показывает, может ли дескриптор работать с географическими координатами на входе и выходе.
     * @return - при "TRUE" - "родные"(native) координаты географические, все методы пересчета работают.
     *           при "FALSE"- "родные"(native) координаты пиксельные, работают только методы пересчета : "convertNativeToScene" и "convertSceneToNative".
     */
    virtual bool isGeoScene() const = 0;

    /**
     * @brief isPixelScene - метод показывает, может ли дескриптор работать с географическими координатами на входе и выходе.
     * @return - при "TRUE" - "родные"(native) координаты пиксельные, работают только методы пересчета : "convertNativeToScene" и "convertSceneToNative".
     *           при "FALSE"- "родные"(native) координаты географические, все методы пересчета работают.
     */
    virtual bool isPixelScene() const = 0;

    /**
     * @brief isValid - метод возвращает валидность дескриптора.
     * @return - валидность дескриптора.
     */
    virtual bool isValid() const = 0;
};

}
#endif
