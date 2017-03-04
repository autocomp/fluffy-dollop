#include "localdataprovider.h"
#include "tileloaderpool.h"
#include "math.h"
#include <QSettings>
#include <QTime>
#include <QApplication>
#include <QDomDocument>
#include <QFile>

using namespace data_system;

LocalDataProvider::LocalDataProvider()
    : TMSDataProvider()
    , _localTmsAdapter(0)
{
}

bool LocalDataProvider::open(const QUrl& url, WorkMode /*mode*/)
{
    _localTmsAdapter = new LocalTmsAdapter(url);

    m_providerURL = url;

    RasterDataProvider* p = static_cast<RasterDataProvider*>(this);
    if(p)
        p->setObjectName(url.host() + url.path());

    setStoragePolicy(AbstractDataProvider::SP_FullInMem);

    m_metadata.setProperty("FILE_PATH_URL", url);

    qDebug() << "LocalDataProvider::open, url :" << url.toString();

    QSettings settings(TileLoaderPool::instance()->settingsPath(), QSettings::NativeFormat);
    QVariant tmsDefaultPort = settings.value("Pool_DefaultPort", 80);

    QSize worldSizeInTiles = TileLoaderPool::instance()->worldSizeInTiles();
    switch(worldSizeInTiles.height())
    {
    case 1 : _localTmsAdapter->setZLevelShiftForY(-1); break;
    case 2 : _localTmsAdapter->setZLevelShiftForY(0); break;
    case 4 : _localTmsAdapter->setZLevelShiftForY(1); break;
    case 8 : _localTmsAdapter->setZLevelShiftForY(2); break;
    }

    if(worldSizeInTiles.height() == 1)
    {
        _localTmsAdapter->setZLevelShiftForY(-1);
    }

    uint _baseTileSetOrder(1);
    QVariant baseTileSetOrder = settings.value("Pool_BaseTileSetOrder");
    if(baseTileSetOrder.isValid())
    {
        bool ok;
        _baseTileSetOrder = baseTileSetOrder.toUInt(&ok);
        if( ! ok || _baseTileSetOrder > 18)
        {
            _baseTileSetOrder = 0;
            settings.setValue("Pool_BaseTileSetOrder", 1);
        }
    }
    else
        settings.setValue("Pool_BaseTileSetOrder", 1);
    _localTmsAdapter->setZLevelShift(_baseTileSetOrder - 1);

    QString serverHost = url.host();
    int serverPort = url.port(tmsDefaultPort.toInt());

    QString serverPath = url.path();
    QString testUrl;
    const QRegExp reg("/%");
    testUrl = serverPath.section(reg, 0, 0);
    qDebug() << "check url :" << testUrl << ", serverHost :" << serverHost << ", serverPort :" << serverPort;

//    QHttp http;
//    http.setHost(serverHost, serverPort);
//    QHttpRequestHeader header("GET", testUrl);
//    header.setValue("User-Agent", "Mozilla");
//    header.setValue("Host", serverHost);
//    http.request(header);

    QTime time;
    time.start();

//    QHttp::State _state(QHttp::Unconnected);
    qDebug() << "-----start-----";

    const uint TIMEOUT(TileLoaderPool::instance()->timeoutOpenProviderSec()*1000);
//    while(time.elapsed() < TIMEOUT)
//    {
//        if(_state != http.state())
//        {
//            _state = http.state();

//            switch(_state)
//            {
//            case QHttp::Unconnected : qDebug() << "----- Unconnected" << time.elapsed(); break;
//            case QHttp::HostLookup : qDebug() << "----- HostLookup" << time.elapsed(); break;
//            case QHttp::Connecting : qDebug() << "----- Connecting" << time.elapsed(); break;
//            case QHttp::Sending : qDebug() << "----- Sending" << time.elapsed(); break;
//            case QHttp::Reading : qDebug() << "----- Reading" << time.elapsed(); break;
//            case QHttp::Connected : qDebug() << "----- Connected" << time.elapsed(); break;
//            case QHttp::Closing : qDebug() << "----- Closing" << time.elapsed(); break;
//            }

//            if(_state == QHttp::Connected)
//            {
//                qDebug() << "http.lastResponse().statusCode() :" << http.lastResponse().statusCode();

//                QDomDocument doc;
//                if(doc.setContent(http.readAll()))
//                {
//                    QRectF boundingBoxRect, extentRect;

//                    QDomNode firstChild = doc.firstChild();
//                    while(!firstChild.isNull())
//                    {
//                        QDomNode n = firstChild.firstChild();
//                        while (!n.isNull())
//                        {
//                            if (n.isElement())
//                            {
//                                QDomElement e = n.toElement();
//                                const QString tagName(e.tagName());
//                                const QString text(e.text());
//                                if(tagName == QString("ScaleMode"))
//                                {
//                                    if(text == QString("fixed"))
//                                        setScaleForBottomLevels(false);
//                                }
//                                else if(tagName == QString("UpperLevel"))
//                                {
//                                    bool ok;
//                                    const uint UpperLevel(text.toUInt(&ok));
//                                    if(ok)
//                                    {
//                                        if(UpperLevel <= 25)
//                                            setMinZoomLevel(UpperLevel + 1);
//                                        else
//                                            qDebug() << "!!! UpperLevel > 25 :" << UpperLevel;
//                                    }
//                                }
//                                else if(tagName == QString("LowerLevel"))
//                                {
//                                    bool ok;
//                                    const uint LowerLevel(text.toUInt(&ok));
//                                    if(ok)
//                                    {
//                                        if(LowerLevel <= 25)
//                                            setMaxZoomLevel(LowerLevel);
//                                        else
//                                            qDebug() << "!!! LowerLevel > 25 :" << LowerLevel;
//                                    }
//                                }
//                                else if(tagName == QString("BoundingBox"))
//                                {
//                                    bool minxOk;
//                                    double minx = e.attribute("minx").toDouble(&minxOk);
//                                    bool minyOk;
//                                    double miny = e.attribute("miny").toDouble(&minyOk);
//                                    bool maxxOk;
//                                    double maxx = e.attribute("maxx").toDouble(&maxxOk);
//                                    bool maxyOk;
//                                    double maxy = e.attribute("maxy").toDouble(&maxyOk);
//                                    if(minxOk && minyOk && maxxOk && maxyOk)
//                                    {
//                                        qDebug() << "===> BoundingBox :" << minx << miny << maxx << maxy;
//                                        boundingBoxRect = QRectF(QPointF(minx, miny), QPointF(maxx, maxy));
//                                    }
//                                }
//                                else if(tagName == QString("Extent"))
//                                {
//                                    bool minxOk;
//                                    double minx = e.attribute("minx").toDouble(&minxOk);
//                                    bool minyOk;
//                                    double miny = e.attribute("miny").toDouble(&minyOk);
//                                    bool maxxOk;
//                                    double maxx = e.attribute("maxx").toDouble(&maxxOk);
//                                    bool maxyOk;
//                                    double maxy = e.attribute("maxy").toDouble(&maxyOk);
//                                    if(minxOk && minyOk && maxxOk && maxyOk)
//                                    {
//                                        qDebug() << "===> Extent :" << minx << miny << maxx << maxy;
//                                        extentRect = QRectF(QPointF(minx, miny), QPointF(maxx, maxy));
//                                    }
//                                }
//                                else if(tagName == QString("TileFormat"))
//                                {
//                                    bool widthOk;
//                                    uint width = e.attribute("width").toUInt(&widthOk);
//                                    bool heightOk;
//                                    uint height = e.attribute("height").toUInt(&heightOk);
//                                    QString extension = e.attribute("extension");
//                                    if(widthOk && heightOk && extension.isEmpty() == false)
//                                    {
//                                        qDebug() << "===> TileFormat width :" << width << ", height :" << height << ", extension :" << extension;
//                                        mTileSize = QSize(width, height);
//                                        if(_localTmsAdapter)
//                                        {
//                                            _localTmsAdapter->setTileSize(QSize(width, height));
//                                            _localTmsAdapter->setExtension(extension);
//                                        }
//                                    }
//                                }
//                                else if(tagName == QString("SRS"))
//                                {
//                                    if(text != QString("EPSG:4326"))
//                                    {
//                                        qDebug() << "!!! SRS != EPSG:4326 :" << text;
//                                    }
//                                }
//                                else if(tagName == QString("Origin"))
//                                {
//                                    bool xOk;
//                                    double x = e.attribute("x").toDouble(&xOk);
//                                    bool yOk;
//                                    double y = e.attribute("y").toDouble(&yOk);
//                                    if(xOk && yOk)
//                                    {
//                                        qDebug() << "===> Origin :" << x << y;
//                                    }
//                                }
//                                else if(tagName == QString("TileSets"))
//                                {
//                                    qDebug() << "   TileSets :";
//                                    QDomNode firstSubChild = n.firstChild();
//                                    while (!firstSubChild.isNull())
//                                    {
//                                        if (firstSubChild.isElement())
//                                        {
//                                            QDomElement e = firstSubChild.toElement();
//                                            const QString tagName(e.tagName());
//                                            if(tagName == QString("TileSet"))
//                                            {
//                                                bool orderOk;
//                                                uint order = e.attribute("order").toUInt(&orderOk);
//                                                bool unitsPerPixelOk;
//                                                double unitsPerPixel = e.attribute("units-per-pixel").toDouble(&unitsPerPixelOk);
//                                                if(orderOk && unitsPerPixelOk)
//                                                {
//                                                    qDebug() << "       order :" << order << ", unitsPerPixel :" << unitsPerPixel;
//                                                }

//                                            }
//                                        }
//                                        firstSubChild = firstSubChild.nextSibling();
//                                    }
//                                    qDebug() << "   -----------";
//                                }
//                            }
//                            n = n.nextSibling();
//                        }
//                        firstChild = firstChild.nextSibling();
//                    }

//                    if(extentRect.isEmpty() == false)
//                        m_spatialRect = extentRect;
//                    else if(boundingBoxRect.isEmpty() == false)
//                        m_spatialRect = boundingBoxRect;

//                    qDebug() << "XML read, return TRUE";
//                    return true;
//                }
//                else
//                    qDebug() << "setContent == false !!!";
//            }
//        }

//        qApp->processEvents();
//    }

//    qDebug() << "check server, TIMEOUT(sec) :" << TIMEOUT << "time.elapsed(msec) :" << time.elapsed();
//    qDebug() << "check server, state:" << http.state() << ", statusCode:" << http.lastResponse().statusCode();

    return true;
}

const TmsBaseAdapter * LocalDataProvider::getTmsAdapter()
{
    return _localTmsAdapter;
}

QString LocalDataProvider::host() const
{
    if(_localTmsAdapter)
        return _localTmsAdapter->serverHost();
    else
        return QString();
}

QString LocalDataProvider::serverPath() const
{
    if(_localTmsAdapter)
        return _localTmsAdapter->serverPath();
    else
        return QString();
}

int LocalDataProvider::serverPort() const
{
    if(_localTmsAdapter)
        return _localTmsAdapter->serverPort();
    else
        return -1;
}

QString LocalDataProvider::scheme() const
{
    if(_localTmsAdapter)
        return _localTmsAdapter->scheme();
    else
        return QString();
}

QUrl LocalDataProvider::query(int x, int y, int z) const
{
    if(_localTmsAdapter)
        return _localTmsAdapter->query(x,y,z);
    else
        return QUrl();
}

void LocalDataProvider::setMinZoomLevel(uint zoom)
{
    if(_localTmsAdapter)
        _localTmsAdapter->setMinZoomLevel(zoom);
}
void LocalDataProvider::setMaxZoomLevel(uint zoom)
{
    if(_localTmsAdapter)
        _localTmsAdapter->setMaxZoomLevel(zoom);
}
void  LocalDataProvider::setRetryLimit(uint retryLimit)
{
    if(_localTmsAdapter)
        _localTmsAdapter->setRetryLimit(retryLimit);
}
void  LocalDataProvider::setTimeoute(uint timeouteSec)
{
    if(_localTmsAdapter)
        _localTmsAdapter->setTimeoute(timeouteSec);
}
void  LocalDataProvider::setScaleForBottomLevels(bool on_off)
{
    if(_localTmsAdapter)
        _localTmsAdapter->setScaleForBottomLevels(on_off);
}
void LocalDataProvider::setLoadersLimit(int loadersLimit)
{
    if(_localTmsAdapter)
        _localTmsAdapter->setLoadersLimit(loadersLimit);
}

uint LocalDataProvider::minZoomLevel()const
{
    if(_localTmsAdapter)
        return _localTmsAdapter->minZoomLevel();
    else
        return 0;
}
uint LocalDataProvider::maxZoomLevel()const
{
    if(_localTmsAdapter)
        return _localTmsAdapter->maxZoomLevel();
    else
        return 20;
}
uint LocalDataProvider::retryLimit()const
{
    if(_localTmsAdapter)
        return _localTmsAdapter->retryLimit();
    else
        return 2;
}
uint LocalDataProvider::timeouteSec()const
{
    if(_localTmsAdapter)
        return _localTmsAdapter->timeouteSec();
    else
        return 30;
}
bool LocalDataProvider::scaleForBottomLevels()const
{
    if(_localTmsAdapter)
        return _localTmsAdapter->scaleForBottomLevels();
    else
        return true;
}
int LocalDataProvider::loadersLimit()const
{
    if(_localTmsAdapter)
        return _localTmsAdapter->loadersLimit();
    else
        return -1;
}
