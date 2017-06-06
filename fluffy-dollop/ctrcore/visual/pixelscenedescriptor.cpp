#include "pixelscenedescriptor.h"
#include <QDebug>

using namespace visualize_system;

PixelSceneDescriptor::PixelSceneDescriptor(QSize _sceneInTiles_, QSize _tileSize_)
    : _isValid(false)
    , _sceneInTiles(_sceneInTiles_)
    , _tileSize(_tileSize_)
    , _coef(1)
{
}

PixelSceneDescriptor::~PixelSceneDescriptor()
{
//    _geoImageObject.clear();
}

void PixelSceneDescriptor::init(QSize rasterSize) //QSharedPointer<dpf::io::GeoImageObject> gio)
{
//    _geoImageObject.clear();
//    _geoImageObject = gio;
    QSize _rasterSize(rasterSize); // _geoImageObject->width(), _geoImageObject->height());
    if(_rasterSize.width() == 0 || _rasterSize.height() == 0)
        return;

    int baseZlevel(1);
    QSize baseSceneSize(sceneSize());
    forever
    {
        QRect currRect(0, 0, baseSceneSize.width()*pow(2,baseZlevel-1), baseSceneSize.height()*pow(2,baseZlevel-1));
        if(currRect.contains(QRect(QPoint(),_rasterSize)))
        {
            double coef(1. / pow(2,(baseZlevel - 1)) );
            double W(_rasterSize.width()*coef);
            double H(_rasterSize.height()*coef);
            _rasterSceneRect = QRectF(0,0,W,H);
            _coef = (_rasterSize.width()/_rasterSceneRect.width() + _rasterSize.height()/_rasterSceneRect.height()) / 2;
            break;
        }
        ++baseZlevel;
    }

    _isValid = true;
    //qDebug() << "*** PixelSceneDescriptor::init, _shiftRasterOnScene :" << _shiftRasterOnScene << ", _rasterSceneRect :" << _rasterSceneRect << ", _coef :" << _coef;
}

bool PixelSceneDescriptor::convertNativeToScene(double &x, double &y) const
{
    if(_isValid == false)
        return false;

    x /= _coef;
    y /= _coef;
    return true;
}

bool PixelSceneDescriptor::convertSceneToNative(double &x, double &y) const
{
    if(_isValid == false)
        return false;

    x *= _coef;
    y *= _coef;
    return true;
}

bool PixelSceneDescriptor::convertRefSystemToScene(double &x, double &y) const
{
    return false;
}

bool PixelSceneDescriptor::convertSceneToRefSystem(double &x, double &y) const
{
    return false;
}

bool PixelSceneDescriptor::convertEpsgToScene(int epsgCode, double &x, double &y) const
{
//    if(_geoImageObject.isNull())
        return false;

//    int srcEpsgCode  = _geoImageObject->getProjectionParams().epsg();
//    if(srcEpsgCode != destEpsgCode)
//    {
//        OGRSpatialReference * inRef = new OGRSpatialReference;
//        OGRSpatialReference * outRef = new OGRSpatialReference;
//        OGRErr errIn = inRef->importFromEPSG(srcEpsgCode);
//        OGRErr errOut = outRef->importFromEPSG(destEpsgCode);
//        if(errIn != OGRERR_NONE || errOut != OGRERR_NONE)
//        {
//            OGRSpatialReference::DestroySpatialReference(inRef);
//            OGRSpatialReference::DestroySpatialReference(outRef);
//            return false;
//        }
//        OGRCoordinateTransformation * trans = OGRCreateCoordinateTransformation(inRef, outRef);
//        trans->Transform(1,&x,&y);
//        OGRCoordinateTransformation::DestroyCT(trans);
//        OGRSpatialReference::DestroySpatialReference(inRef);
//        OGRSpatialReference::DestroySpatialReference(outRef);
//    }

//    dpf::geo::GeoPoint gp = _geoImageObject->pixel_to_geo(dpf::geo::PixelPoint(x, y));
//    x = gp.latitude();
//    y = gp.longitude();

//    return true;
}

bool PixelSceneDescriptor::convertSceneToEpsg(int destEpsgCode, double &x, double &y) const
{
    return false;

//    if(_isValid == false || _geoImageObject.isNull())
//        return false;

//    if(_geoImageObject->is_matched() == false)
//        return false;

//    x *= _coef;
//    y *= _coef;

//    dpf::geo::GeoPoint gp = _geoImageObject->pixel_to_geo(dpf::geo::PixelPoint(x, y));
//    x = gp.latitude();
//    y = gp.longitude();

//    //! Пока все конвертируется в ВГС, не зависимо от реально установленной системы координат в геоимиджобжекте.
//    int srcEpsgCode = 4326; //_geoImageObject->getProjectionParams().epsg();
//    if(srcEpsgCode != destEpsgCode)
//    {
//        OGRSpatialReference * inRef = new OGRSpatialReference;
//        OGRSpatialReference * outRef = new OGRSpatialReference;
//        OGRErr errIn = inRef->importFromEPSG(srcEpsgCode);
//        OGRErr errOut = outRef->importFromEPSG(destEpsgCode);
//        if(errIn != OGRERR_NONE || errOut != OGRERR_NONE)
//        {
//            OGRSpatialReference::DestroySpatialReference(inRef);
//            OGRSpatialReference::DestroySpatialReference(outRef);
//            return false;
//        }
//        OGRCoordinateTransformation * trans = OGRCreateCoordinateTransformation(inRef, outRef);
//        trans->Transform(1,&x,&y);
//        OGRCoordinateTransformation::DestroyCT(trans);
//        OGRSpatialReference::DestroySpatialReference(inRef);
//        OGRSpatialReference::DestroySpatialReference(outRef);
//    }
//    return true;
}

int PixelSceneDescriptor::refEpsgSRS() const
{
    return 0;
}

QString PixelSceneDescriptor::refProjSRS() const
{
    return QString();
}

bool PixelSceneDescriptor::isValid() const
{
    return _isValid;
}

QSize PixelSceneDescriptor::tileSize() const
{
    return _tileSize;
}

QSize PixelSceneDescriptor::sceneSizeInTiles() const
{
    return _sceneInTiles;
}

QSize PixelSceneDescriptor::sceneSize() const
{
    return QSize(_sceneInTiles.width() * _tileSize.width(), _sceneInTiles.height() * _tileSize.height());
}

bool PixelSceneDescriptor::isGeoScene() const
{
    return false;
}

bool PixelSceneDescriptor::isPixelScene() const
{
    return true;
}
