#ifndef ABSTRACTTILEMAPPER_H
#define ABSTRACTTILEMAPPER_H

#include <ctrcore/provider/rasterdataprovider.h>
#include <QObject>
#include <QSharedPointer>
#include "scenedescriptor.h"

class QPolygonF;

namespace visualize_system
{

struct URL
{
    URL(int _x, int _y, int _z, QString _returnId) : x(_x), y(_y), z(_z), returnId(_returnId) {}
    int x,y,z;
    QString returnId;
};

struct ProviderData
{
    unsigned int returnId;
    double transparency;
};

class AbstractTileMapper;
class RasterLayer;

struct DataProviderProperty
{
    DataProviderProperty(AbstractTileMapper* _dataProvider)
        : dataProvider(_dataProvider), rasterLayer(0), transparency(1), percentTransparency(0), isVisible(true), clearBlackArea(false), baseCover(false) {}
    void setVisible(bool on_off) {isVisible = on_off;}
    int getPercentTransparency() {return percentTransparency;}
    void setTransparency(int _percentTransparency)
    {
        if(_percentTransparency < 0 || _percentTransparency > 100)
            return;

        percentTransparency = _percentTransparency;
        transparency = (percentTransparency == 0 ? 1 : 1 - percentTransparency / 100.);
    }

    AbstractTileMapper* dataProvider;
    RasterLayer* rasterLayer;
    double transparency;
    int percentTransparency;
    bool isVisible;
    QRectF sceneBRect;
    bool clearBlackArea;
    bool baseCover;
};


/**
 * @brief The Temp struct - временный объект для хранения указателя на матрицу тайла, отданого на заполнения провайдеру (+ исходные данные этого тайла).
 */
struct Temp
{
    Temp(image_types::TImage* _tImg, QString _task, int _x, int _y, int _z, QString _cacheKey, QPoint _shiftImageInTile)
        : tImg(_tImg), task(_task), x(_x), y(_y), z(_z), cacheKey(_cacheKey), shiftImageInTile(_shiftImageInTile) {}

    image_types::TImage* tImg;
    int x, y, z;
    QString task, cacheKey;
    QPoint shiftImageInTile;
};

struct TileCoords
{
    TileCoords(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}
    TileCoords() : x(-1), y(-1), z(-1) {}
    TileCoords(const TileCoords& other) : x(other.x), y(other.y), z(other.z) {}
    bool equival(int _x, int _y, int _z) {return x==_x && y==_y && z==_z;}
    void setCoords(int _x, int _y, int _z) {x=_x; y=_y; z=_z;}
    void reset() {x=-1; y=-1; z=-1;}

    int x,y,z;
};

/**
 * @brief The TileMapper class Класс мапинга координат
 * Класс обеспечивает доступ к провайдерам через тайловый интерфейс
 */
class AbstractTileMapper : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор класса
     * @param provider укзатель на провайдер, с которым будет работать маппер
     */
    AbstractTileMapper(uint providerId, const SceneDescriptor& sceneDescriptor);
    virtual ~AbstractTileMapper();
    virtual void abort() = 0;

    void init(QPointF shift, QRectF sceneRect, double coef);
    QSharedPointer<data_system::RasterDataProvider> provider() const;

protected:
    QSharedPointer<data_system::RasterDataProvider> raster_provider;
    const SceneDescriptor & sceneDescriptor;
    const int bytesPerPixel;

    QRectF rasterSceneRect;
    double rasterMapperCoef;
    QPointF shiftImageOnScene;

    QMap<uint, Temp> m_Map;

protected:
    image_types::TImage *normalizeTImage(image_types::TImage *src, QPoint shiftImageInTile);
    QPointF geoToScene(const QPointF& coordinateIn);
    QPointF sceneToGeo(const QPointF& point);
    QRectF getTileExtend(int x, int y, int z);
};

}

#endif // TILEMAPPER_H
