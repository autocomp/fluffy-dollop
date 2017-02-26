#include "abstracttilemapper.h"

#include <QDebug>
#include <QPolygonF>
#include <math.h>

#include <ctrcore/provider/dataproviderfactory.h>

using namespace visualize_system;

AbstractTileMapper::AbstractTileMapper(uint providerId, const SceneDescriptor &_sceneDescriptor)
    : sceneDescriptor(_sceneDescriptor)
    , bytesPerPixel(32/8)
    , rasterMapperCoef(1)
{
    QSharedPointer<data_system::AbstractDataProvider> ptr = data_system::DataProviderFactory::instance()->getProvider(providerId);
    raster_provider = ptr.dynamicCast<data_system::RasterDataProvider>();
}

AbstractTileMapper::~AbstractTileMapper()
{
}

void AbstractTileMapper::init(QPointF shift, QRectF sceneRect, double coef)
{
    shiftImageOnScene = shift;
    rasterSceneRect = sceneRect;
    rasterMapperCoef = coef;
}

QSharedPointer<data_system::RasterDataProvider> AbstractTileMapper::provider() const
{
    return raster_provider;
}

image_types::TImage * AbstractTileMapper::normalizeTImage(image_types::TImage *src, QPoint shiftImageInTile)
{
    if(shiftImageInTile == QPoint(-1,-1))
        shiftImageInTile = QPoint(0,0);

    image_types::TImage *dest = new image_types::TImage;
    dest->depth = 32;
    dest->w = sceneDescriptor.tileSize().width();
    dest->h = sceneDescriptor.tileSize().height();
    dest->prt = new char[dest->w * dest->h * bytesPerPixel];
    memset(dest->prt, 0, dest->w * dest->h * bytesPerPixel);

    int destLine = dest->w * bytesPerPixel;
    int destShiftX = bytesPerPixel * shiftImageInTile.x();
    int destShiftY = destLine * shiftImageInTile.y();

    for (int y = 0; y < src->h; y++)
        for (int x = 0; x < src->w; x++)
            memcpy(dest->prt + destShiftY + destLine*y + destShiftX + x*bytesPerPixel,
                   src->prt + y * src->w * bytesPerPixel + x * bytesPerPixel,
                   bytesPerPixel);

    return dest;
}


/////////////////////////////////////////////////////////////////////////////////
QPointF AbstractTileMapper::geoToScene(const QPointF& coordinateIn)
{
    const uint W(sceneDescriptor.sceneSize().width());
    const uint H(sceneDescriptor.sceneSize().height());

    QPointF point;
    point.setX( (coordinateIn.y() +180 )*W/360.0 ); // long
    point.setY( ( -coordinateIn.x()  + 90)*( H)/180.0 ); //latt
    return point;
}


QPointF AbstractTileMapper::sceneToGeo(const QPointF& point)
{
    const uint W(sceneDescriptor.sceneSize().width());
    const uint H(sceneDescriptor.sceneSize().height());

    double longitude = (point.x()*(360. / W))-180.;
    double latitude = -(point.y()*(180. / H))+90.;

    return QPointF(latitude, longitude);
}

QRectF AbstractTileMapper::getTileExtend(int x, int y, int z)
{
    double POW(pow(2,z-1));
    double tileWidthOnView(sceneDescriptor.tileSize().width() / POW);
    double tileHeightOnView(sceneDescriptor.tileSize().height() / POW);

    QRectF tileSceneRect(tileWidthOnView*x, tileHeightOnView*y, tileWidthOnView, tileHeightOnView);
    QPolygonF sceneRectPolygon(tileSceneRect), geoRectPolygon;
    foreach(QPointF p, sceneRectPolygon)
    {
        double x(p.x()), y(p.y());
        sceneDescriptor.convertSceneToRefSystem(x,y);
        geoRectPolygon.append(QPointF(x,y));
        // geoRectPolygon.append(sceneToGeo(p));
    }

    return geoRectPolygon.boundingRect();
}







