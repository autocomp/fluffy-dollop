#include "tmsbaseadapter.h"
#include <QDebug>

TmsBaseAdapter::TmsBaseAdapter(const QUrl& url)
    : _serverUrl(url.toString())
    , mMinZoomLevel(0)
    , mMaxZoomLevel(20) //18)
    , mRetryLimit(2)
    , mTimeoute(30)
    , mScaleForBottomLevels(true)
    , mLoadersLimit(-1)
    , mLoadersInWork(0)
    , mExtension(QString("png"))
    , mTileSize(QSize(256,256))
    , mZLevelShift(-1)
    , mZLevelShiftForY(0)
{
    loc.setNumberOptions(QLocale::OmitGroupSeparator);

    _serverHost = url.host();
    _serverPath = url.path();
    _serverPort = url.port(80);
    _scheme = url.scheme();

    int param1 = _serverPath.indexOf("%1");
    int param2 = _serverPath.indexOf("%2");
    int param3 = _serverPath.indexOf("%3");

    int min = param1 < param2 ? param1 : param2;
    min = param3 < min ? param3 : min;

    int max = param1 > param2 ? param1 : param2;
    max = param3 > max ? param3 : max;

    int middle = param1+param2+param3-min-max;

    order[0][0] = min;
    if (min == param1)
        order[0][1] = 0;
    else if (min == param2)
        order[0][1] = 1;
    else
        order[0][1] = 2;

    order[1][0] = middle;
    if (middle == param1)
        order[1][1] = 0;
    else if (middle == param2)
        order[1][1] = 1;
    else
        order[1][1] = 2;

    order[2][0] = max;
    if (max == param1)
        order[2][1] = 0;
    else if(max == param2)
        order[2][1] = 1;
    else
        order[2][1] = 2;
}

TmsBaseAdapter::~TmsBaseAdapter()
{

}

QString TmsBaseAdapter::scheme() const
{
    return _scheme;
}

QString TmsBaseAdapter::serverHost() const
{
    return _serverHost;
}

QString TmsBaseAdapter::serverPath() const
{
    return _serverPath;
}

int TmsBaseAdapter::serverPort() const
{
    return _serverPort;
}

QString TmsBaseAdapter::serverUrl() const
{
    return _serverUrl;
}

QUrl TmsBaseAdapter::query(int x, int y, int z) const
{
    int a[3] = {z, x, y};
    QString queryString = _serverPath;
    queryString.replace(order[2][0],2, loc.toString(a[order[2][1]]));
    queryString.replace(order[1][0],2, loc.toString(a[order[1][1]]));
    queryString.replace(order[0][0],2, loc.toString(a[order[0][1]]));

    QUrl url;
    url.setHost(serverHost());
    url.setScheme(scheme());
    url.setPort(serverPort());
    url.setPath(queryString);

    return url;
}

void TmsBaseAdapter::setMinZoomLevel(uint zoom)
{
    mMinZoomLevel = zoom;
}

void TmsBaseAdapter::setMaxZoomLevel(uint zoom)
{
    mMaxZoomLevel = zoom;
}

void  TmsBaseAdapter::setRetryLimit(uint retryLimit)
{
    mRetryLimit = retryLimit;
}

void  TmsBaseAdapter::setTimeoute(uint timeouteSec)
{
    mTimeoute = timeouteSec;
}

void TmsBaseAdapter::setScaleForBottomLevels(bool on_off)
{
    mScaleForBottomLevels = on_off;
}

void TmsBaseAdapter::setLoadersLimit(int loadersLimit)
{
    mLoadersLimit = loadersLimit;
}

void TmsBaseAdapter::setExtension(const QString &extension)
{
    mExtension = extension;
}

void TmsBaseAdapter::setTileSize(const QSize &tileSize)
{
    mTileSize = tileSize;
}

void TmsBaseAdapter::setZLevelShift(int zLevelShift)
{
    mZLevelShift = zLevelShift;
}

void TmsBaseAdapter::setZLevelShiftForY(int zLevelShiftForY)
{
    mZLevelShiftForY = zLevelShiftForY;
}

uint TmsBaseAdapter::minZoomLevel() const
{
    return mMinZoomLevel;
}

uint TmsBaseAdapter::maxZoomLevel() const
{
    return mMaxZoomLevel;
}

uint TmsBaseAdapter::retryLimit() const
{
    return mRetryLimit;
}

uint TmsBaseAdapter::timeouteSec() const
{
    return mTimeoute;
}
bool TmsBaseAdapter::scaleForBottomLevels() const
{
    return mScaleForBottomLevels;
}
int TmsBaseAdapter::loadersLimit()const
{
    return mLoadersLimit;
}
QString TmsBaseAdapter::extension()const
{
    return mExtension;
}
QSize TmsBaseAdapter::tileSize()const
{
    return mTileSize;
}
int TmsBaseAdapter::zLevelShift()const
{
    return mZLevelShift;
}

int TmsBaseAdapter::zLevelShiftForY()const
{
    return mZLevelShiftForY;
}

void TmsBaseAdapter::incLoadersInWork()
{
    if(mLoadersLimit > 0)
    {
        uint _loadersLimit(mLoadersLimit);
        if(mLoadersInWork < _loadersLimit)
            ++mLoadersInWork;
        else
            qDebug() << "!!! TmsBaseAdapter, ERROR in incLoadersInWork, mLoadersLimit :" << mLoadersLimit << ", mLoadersInWork :" << mLoadersInWork;
    }

}
void TmsBaseAdapter::decLoadersInWork()
{
    if(mLoadersLimit > 0)
    {
        if(mLoadersInWork == 0)
            qDebug() << "!!! TmsBaseAdapter, ERROR in decLoadersInWork, mLoadersLimit :" << mLoadersLimit << ", mLoadersInWork :" << mLoadersInWork;
        else
            --mLoadersInWork;
    }
}
uint TmsBaseAdapter::loadersInWork()const
{
    return mLoadersInWork;
}
bool TmsBaseAdapter::hasFreeLoaders() const
{
    if(mLoadersLimit > 0)
    {
        return mLoadersInWork < mLoadersLimit;
    }
    else
    {
        return true;
    }
}









