#include "imageprovider.h"
#include <ctrcore/provider/uniqueidfactory.h>
#include <QPainter>
#include <QFileInfo>

data_system::ImageProvider::ImageProvider()
{}

QRectF data_system::ImageProvider::spatialRect()
{
    return m_boundingRect;
}

QPolygonF data_system::ImageProvider::spatialPolygon()
{
    return QPolygonF(m_boundingRect);
}

bool data_system::ImageProvider::open(const QUrl &url, WorkMode mode)
{
    QFileInfo fi(url.toLocalFile());
    QString path = fi.absoluteFilePath() + QString(".xml");
    if(QFile::exists(path) == false)
        return false;

    m_providerURL = url;

    QString name, title;
    double minx, miny, maxx, maxy;

    m_layerName = name;
    m_boundingRect = QRectF(QPointF(miny, minx), QPointF(maxy, maxx));

    qDebug() << "*** name" << name << "minx" << minx << "maxx" << maxx << "miny" << miny << "maxy" << maxy;

    setStoragePolicy(AbstractDataProvider::SP_FullInMem);

    if(!title.isEmpty())
    {
        name = title;
    }

    if(!name.isEmpty())
    {
        setObjectName(name);
    }

    m_metadata.setProperty("FILE_PATH_URL", url);

    return true;
}

data_system::ImageProvider::~ImageProvider()
{
}

void data_system::ImageProvider::abort()
{
}

int data_system::ImageProvider::readSpatialData(image_types::TImage &img, QRectF source, QRect dest, uint32_t band)
{
    return -1;
}

int data_system::ImageProvider::readSpatialData(image_types::TImage &img, int epsgCode, QRectF source, QSize dest, uint32_t band)
{
    return -1;
}

int data_system::ImageProvider::readSpatialData(image_types::TImage &img, const QString &proj4code, QRectF source, QSize dest, uint32_t band)
{
    return -1;
}
