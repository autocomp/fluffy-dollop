#include "geoscenedescriptor.h"
#include <gdal/ogr_spatialref.h>
#include <QDomDocument>
#include <QRectF>
#include <QDebug>

using namespace visualize_system;

GeoSceneDescriptor::GeoSceneDescriptor(const QString & xml, const QString & gridName, uint baseTileSetOrder)
    : _isValid(false)
    , _refEPSG(0)
    , _unitsPerPixel(0)
    , _tileSize(256,256)
    , _trans(0)
    , _revTrans(0)
    , _srsGeogSrc(0)
    , _srsGeogTrg(0)
{
    QDomDocument doc;
    if(doc.setContent(xml))
    {
        qDebug() << "*********************GeoSceneDescriptor";
        qDebug() << xml;

        QDomNode firstChild = doc.firstChild();
        bool gridRead(false);
        while(!firstChild.isNull() && gridRead == false)
        {
            QDomNode n = firstChild.firstChild();
            while (!n.isNull() && gridRead == false)
            {
                QDomElement e = n.toElement();
                const QString tagName(e.tagName());
                if(tagName == QString("Grids"))
                {
                    QDomNode nn = n.firstChild();
                    while (!nn.isNull() && gridRead == false) // проходим по сеткам
                    {
                        if (nn.isElement())
                        {
                            QDomElement ee = nn.toElement();
                            const QString tagName2(ee.tagName());
                            if(tagName2 ==  QString("Grid"))
                            {
                                QDomNode nnn = nn.firstChild();
                                bool targetGrid(true);
                                while (!nnn.isNull() && targetGrid) // проходим по элементам внутри сеткам
                                {
                                    QDomElement eee = nnn.toElement();
                                    const QString tagName3(eee.tagName());
                                    const QString text3(eee.text());
                                    if(tagName3 == QString("Name"))
                                    {
                                        if(text3 == gridName)
                                        {
                                            qDebug() << "+++ gridName :" << gridName << ", text3" << text3;
                                            gridRead = true;
                                        }
                                        else
                                        {
                                            qDebug() << "--- gridName :" << gridName << ", text3" << text3;
                                            targetGrid = false;
                                            continue;
                                        }
                                    }
                                    else if(tagName3 == QString("BoundingBox"))
                                    {
                                        bool minxOk;
                                        double minx = eee.attribute("minx").toDouble(&minxOk);
                                        bool minyOk;
                                        double miny = eee.attribute("miny").toDouble(&minyOk);
                                        bool maxxOk;
                                        double maxx = eee.attribute("maxx").toDouble(&maxxOk);
                                        bool maxyOk;
                                        double maxy = eee.attribute("maxy").toDouble(&maxyOk);
                                        if(minxOk && minyOk && maxxOk && maxyOk)
                                        {
                                            qDebug() << "BoundingBox :" << minx << miny << maxx << maxy;
                                            _bbox = QRectF(QPointF(minx, miny), QPointF(maxx, maxy));
                                        }
                                    }
                                    else if(tagName3 == QString("SRSCode"))
                                    {
                                        QString refText = text3.section(':', 1);
                                        bool ok;
                                        int ref = refText.toInt(&ok);
                                        if(ok)// && ref < 100000)
                                            _refEPSG = ref;
                                        qDebug() << "SRSCode :" << text3 << ", refText :" << refText << ", REF :" << _refEPSG;
                                    }
                                    else if(tagName3 == QString("SRSProj"))
                                    {
                                        _refProj = text3;
                                        qDebug() << "SRSProj :" << text3;
                                    }
                                    else if(tagName3 == QString("Origin"))
                                    {
                                        bool xOk;
                                        double x = eee.attribute("x").toDouble(&xOk);
                                        bool yOk;
                                        double y = eee.attribute("y").toDouble(&yOk);
                                        if(xOk && yOk)
                                        {
                                            _origin = QPointF(x,y);
                                            qDebug() << "Origin :" << x << y;
                                        }
                                    }
                                    else if(tagName3 == QString("TileFormat"))
                                    {
                                        bool wOk;
                                        double W = eee.attribute("width").toInt(&wOk);
                                        bool hOk;
                                        double H = eee.attribute("height").toInt(&hOk);
                                        if(wOk && hOk)
                                        {
                                            _tileSize = QSize(W,H);
                                            qDebug() << "TileSize :" << W << H;
                                        }
                                    }
                                    else if(tagName3 == QString("TileSet"))
                                    {
                                        bool orderOk;
                                        uint order = eee.attribute("order").toUInt(&orderOk);
                                        bool unitsPerPixelOk;
                                        double unitsPerPixel = eee.attribute("units-per-pixel").toDouble(&unitsPerPixelOk);
                                        if(orderOk && unitsPerPixelOk && order == baseTileSetOrder)
                                        {
                                            _unitsPerPixel = unitsPerPixel;
                                            qDebug() << "+++ baseTileSetOrder :" << baseTileSetOrder << ", order :" << order << ", unitsPerPixel :" << _unitsPerPixel;
                                        }
//                                        else
//                                            qDebug() << "--- baseTileSetOrder :" << baseTileSetOrder << ", order :" << order << ", unitsPerPixel :" << unitsPerPixel << ", orderOk :" << orderOk;

                                    }
                                    nnn = nnn.nextSibling();
                                }
                            }
                        }
                        nn = nn.nextSibling();
                    }
                }
                n = n.nextSibling();
            }
            firstChild = firstChild.nextSibling();
        }

        if(_bbox.isEmpty() == false && _unitsPerPixel > 0)
        {
            QString W = QString::number(_bbox.width() / _unitsPerPixel / _tileSize.width(), 'f', 0);
            QString H = QString::number(_bbox.height() / _unitsPerPixel / _tileSize.height(), 'f', 0);
            _sceneInTiles = QSize(W.toInt(), H.toInt());
            qDebug() << "---> SCENE_IN_TILES :" << _sceneInTiles << QString::number(_bbox.width(), 'f', 0) << QString::number(_unitsPerPixel, 'f');
        }
        else
        {
            qDebug() << "GeoSceneDescriptor, ( _bbox.isEmpty() == false && _unitsPerPixel > 0 ) == false !!!";
            return;
        }

        if(_refEPSG != 4326)
        {
            _srsGeogSrc = new OGRSpatialReference;
            OGRErr err;
            if(_refEPSG != 0 )
                err = _srsGeogSrc->importFromEPSG(_refEPSG);
            else if(_refProj.isEmpty() == false)
                err = _srsGeogSrc->importFromProj4(_refProj.toLocal8Bit());
            else
                qDebug() << "GeoSceneDescriptor, _refEPSG == NULL && _refProj.isEmpty() !!!";

            if(err != OGRERR_NONE)
            {
                delete _srsGeogSrc;
                _srsGeogSrc = 0;
                qDebug() << "GeoSceneDescriptor, OGRErr != OGRERR_NONE !!!, err :" << err;
                return;
            }

            _srsGeogTrg = new OGRSpatialReference;
            _srsGeogTrg->importFromEPSG(4326);
            _trans = OGRCreateCoordinateTransformation(_srsGeogSrc, _srsGeogTrg);
            _revTrans = OGRCreateCoordinateTransformation(_srsGeogTrg, _srsGeogSrc);
        }

        _isValid = true;
        qDebug() << "*********************GeoSceneDescriptor is valid";
    }
    else
    {
        qDebug() << "GeoSceneDescriptor, setContent == FALSE !!!";
    }
}

GeoSceneDescriptor::~GeoSceneDescriptor()
{
    if(_trans)
    {
        OGRCoordinateTransformation::DestroyCT(_trans);
    }
    if(_revTrans)
    {
        OGRCoordinateTransformation::DestroyCT(_revTrans);
    }
    if(_srsGeogSrc)
    {
        OGRSpatialReference::DestroySpatialReference(_srsGeogSrc);
    }
    if(_srsGeogTrg)
    {
        OGRSpatialReference::DestroySpatialReference(_srsGeogTrg);
    }
}

bool GeoSceneDescriptor::convertNativeToScene(double &x, double &y) const
{
    if(_isValid == false)
        return false;

    double _X = x; // -180 / +180
    double _Y = y; // -90 / +90

    if(_refEPSG != 4326)
        _revTrans->Transform(1,&_X,&_Y);

    if(_bbox.contains(QPointF(_X,_Y)) == false)
    {
        x = -1;
        y = -1;
        return false;
    }

    x = (_X - _bbox.left()) / _unitsPerPixel;
    double origY = (_Y - _bbox.top()) / _unitsPerPixel;
    y = _sceneInTiles.height()*_tileSize.height() - origY;

    return true;
}

bool GeoSceneDescriptor::convertSceneToNative(double &x, double &y) const
{
    if(_isValid == false)
        return false;

    // переводим пиксели сцены в родные географические координаты сцены
    double geoOrigX = x * _unitsPerPixel + _bbox.left();
    double revY = (_sceneInTiles.height()*_tileSize.height() - y);
    double geoOrigY = revY * _unitsPerPixel + _bbox.top();

    if(_refEPSG == 4326)
    {
        x = geoOrigX;
        y = geoOrigY;
    }
    else
    {
        // вновь переводим родные гео в WGS координаты
        double geoWgsX = geoOrigX;
        double geoWgsY = geoOrigY;
        _trans->Transform(1,&geoWgsX,&geoWgsY); //

        // вновь переводим WGS координаты в родные гео, чтоб сравнить их
        double _geoOrigX = geoWgsX;
        double _geoOrigY = geoWgsY;
        _revTrans->Transform(1,&_geoOrigX,&_geoOrigY);

        if(_refEPSG != 0)
            if(QString::number(geoOrigX,'f',2) != QString::number(_geoOrigX,'f',2) || QString::number(geoOrigY,'f',2) != QString::number(_geoOrigY,'f',2))
            {
                x = -360;
                y = -360;
                // qDebug() << _refEPSG << "!!!" << QString::number(geoOrigX,'f',2) << "!=" << QString::number(_geoOrigX,'f',2) << " || " << QString::number(geoOrigY,'f',2) << "!=" << QString::number(_geoOrigY,'f',2);
                return false;
            }

        x = geoWgsX;
        y = geoWgsY;
    }

    return true;
}

bool GeoSceneDescriptor::convertRefSystemToScene(double &x, double &y) const
{
    if(_isValid == false)
        return false;

    double _X = x;
    double _Y = y;

    if(_bbox.contains(QPointF(_X,_Y)) == false)
    {
        x = -1;
        y = -1;
        return false;
    }

    x = (_X - _bbox.left()) / _unitsPerPixel;
    double origY = (_Y - _bbox.top()) / _unitsPerPixel;
    y = _sceneInTiles.height()*_tileSize.height() - origY;
    return true;
}

bool GeoSceneDescriptor::convertSceneToRefSystem(double &x, double &y) const
{
    if(_isValid == false)
        return false;

    // переводим пиксели сцены в родные географические координаты сцены
    double geoOrigX = x * _unitsPerPixel + _bbox.left();
    double revY = (_sceneInTiles.height()*_tileSize.height() - y);
    double geoOrigY = revY * _unitsPerPixel + _bbox.top();

    if(_refEPSG == 4326)
    {
        x = geoOrigX;
        y = geoOrigY;
        return true;
    }

    // вновь переводим родные гео в WGS координаты
    double geoWgsX = geoOrigX;
    double geoWgsY = geoOrigY;
    _trans->Transform(1,&geoWgsX,&geoWgsY); //

    // вновь переводим WGS координаты в родные гео, чтоб сравнить их
    double _geoOrigX = geoWgsX;
    double _geoOrigY = geoWgsY;
    _revTrans->Transform(1,&_geoOrigX,&_geoOrigY);

    if(_refEPSG != 0)
        if( QString::number(geoOrigX,'f',2) != QString::number(_geoOrigX,'f',2) || QString::number(geoOrigY,'f',2) != QString::number(_geoOrigY,'f',2))
        {
            x = -360;
            y = -360;
            return false;
        }

    x = geoOrigX; // geoWgsX;
    y = geoOrigY; // geoWgsY;
    return true;
}

bool GeoSceneDescriptor::convertEpsgToScene(int epsgCode, double &x, double &y) const
{
    if(_isValid == false)
        return false;

    if(epsgCode != _refEPSG)
    {
        OGRSpatialReference * inRef = new OGRSpatialReference;
        OGRErr err = inRef->importFromEPSG(epsgCode);
        if(err != OGRERR_NONE)
        {
            OGRSpatialReference::DestroySpatialReference(inRef);
            return false;
        }
        OGRCoordinateTransformation * trans = OGRCreateCoordinateTransformation(inRef, _srsGeogSrc);
        trans->Transform(1,&x,&y);
        OGRCoordinateTransformation::DestroyCT(trans);
        OGRSpatialReference::DestroySpatialReference(inRef);
    }
    return convertRefSystemToScene(x,y);
}

bool GeoSceneDescriptor::convertSceneToEpsg(int epsgCode, double &x, double &y) const
{
    if(_isValid == false)
        return false;

    if(convertSceneToRefSystem(x,y) == false)
        return false;

    if(epsgCode != _refEPSG)
    {
        OGRSpatialReference * outRef = new OGRSpatialReference;
        OGRErr err = outRef->importFromEPSG(epsgCode);
        if(err != OGRERR_NONE)
        {
            OGRSpatialReference::DestroySpatialReference(outRef);
            return false;
        }
        OGRCoordinateTransformation * trans = OGRCreateCoordinateTransformation(_srsGeogSrc, outRef);
        trans->Transform(1,&x,&y);
        OGRCoordinateTransformation::DestroyCT(trans);
        OGRSpatialReference::DestroySpatialReference(outRef);
    }
    return true;
}

int GeoSceneDescriptor::refEpsgSRS() const
{
    return _refEPSG;
}

QString GeoSceneDescriptor::refProjSRS() const
{
    return _refProj;
}

bool GeoSceneDescriptor::isValid() const
{
    return _isValid;
}

QSize GeoSceneDescriptor::tileSize() const
{
    return _tileSize;
}

QSize GeoSceneDescriptor::sceneSizeInTiles() const
{
    return _sceneInTiles;
}

QSize GeoSceneDescriptor::sceneSize() const
{
    return QSize(_sceneInTiles.width() * _tileSize.width(), _sceneInTiles.height() * _tileSize.height());
}

bool GeoSceneDescriptor::isGeoScene() const
{
    return true;
}

bool GeoSceneDescriptor::isPixelScene() const
{
    return false;
}
