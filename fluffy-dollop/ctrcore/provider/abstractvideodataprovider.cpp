#include "abstractvideodataprovider.h"

using namespace data_system;

AbstractVideoDataProvider::AbstractVideoDataProvider()
{}
AbstractVideoDataProvider::~AbstractVideoDataProvider()
{}
void AbstractVideoDataProvider::setSourceProvider(AbstractVideoDataProvider *pSource)
{}
AbstractDataProvider::WorkMode AbstractVideoDataProvider::workMode() const
{
    return AbstractDataProvider::Read;
}

QStringList AbstractVideoDataProvider::accessType() const
{
    return QStringList();
}

//QStringList AbstractVideoDataProvider::resourceType() const
//{
//    return QStringList();
//}

bool AbstractVideoDataProvider::toFormat(const QUrl &url, const QString &type)
{
    return false;
}

/// RASTERDATAPROVIDER REIMPMEMENTED UNUSE METHODS
QRect AbstractVideoDataProvider::boundingRect()
{
    return QRect();
}

QRectF AbstractVideoDataProvider::spatialRect()
{
    return QRectF();
}

uint32_t AbstractVideoDataProvider::rasterCount()
{
    return -1;
}

int AbstractVideoDataProvider::readRawData(image_types::TImage &img, QRect source, QSize dest)
{
    return -1;
}

int AbstractVideoDataProvider::readSpatialData(image_types::TImage &img, QRectF source, QRect dest, uint32_t band)
{
    return -1;
}

int AbstractVideoDataProvider::readSpatialData(image_types::TImage &img, int epsgCode, QRectF source, QSize dest, uint32_t band)
{
    return -1;
}

int AbstractVideoDataProvider::readSpatialData(image_types::TImage &img, const QString &proj4code, QRectF source, QSize dest, uint32_t band)
{
    return -1;
}

int AbstractVideoDataProvider::registerDataListener(image_types::TImage &img, QMutex &lock)
{
    return -1;
}

void AbstractVideoDataProvider::destroyDataListener(int num)
{}

qreal AbstractVideoDataProvider::updateInterval()
{
    return -1;
}

void AbstractVideoDataProvider::abort()
{}
void AbstractVideoDataProvider::abort(int task)
{}
void AbstractVideoDataProvider::loadRaster()
{}
void AbstractVideoDataProvider::saveRaster()
{}

/// DATAPROVIDER REIMPMEMENTED UNUSE METHODS
QMap<QString, QStringList> AbstractVideoDataProvider::outKeys() const
{
    return QMap<QString, QStringList>();
}

QList<InParam> AbstractVideoDataProvider::outParameters(const QString &) const
{
    return QList<InParam>();
}

AbstractDataProvider::StoragePolicy AbstractVideoDataProvider::storagePolicy() const
{
    return SP_None;
}

bool AbstractVideoDataProvider::setStoragePolicy(AbstractDataProvider::StoragePolicy)
{
    return false;
}

AbstractDataProvider::ProviderType AbstractVideoDataProvider::providerType() const
{
    return Async;
}

bool AbstractVideoDataProvider::isFinished()
{
    return false;
}

