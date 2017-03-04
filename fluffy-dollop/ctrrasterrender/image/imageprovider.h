#ifndef IMAGEPROVIDER1_H
#define IMAGEPROVIDER1_H

#include <QObject>
#include <QDebug>
#include <QBuffer>
#include <QPixmap>

#include <ctrcore/provider/t_image.h>
#include <ctrcore/provider/rasterdataprovider.h>

class WMSLoader;

namespace data_system
{

class ImageProvider : public RasterDataProvider
{
Q_OBJECT

public:
    ImageProvider();
    virtual ~ImageProvider();
    QMap<QString, QStringList> outKeys() const
    {
        return QMap<QString, QStringList>();
    }

    QList<InParam> outParameters(const QString &) const
    {
        return QList<InParam>();
    }

    static AbstractDataProvider *createProvider()
    {
        return new ImageProvider;
    }

    bool toFormat(const QUrl &url, const QString &type)
    {
        return true;
    }

    virtual StoragePolicy storagePolicy() const
    {
        return AbstractDataProvider::SP_FullInMem;
    }

    virtual bool setStoragePolicy(StoragePolicy)
    {
        return false;
    }

    virtual ProviderType providerType() const
    {
        return AbstractDataProvider::Async;
    }

    virtual WorkMode workMode() const
    {
        return AbstractDataProvider::Read;
    }

    QStringList accessType() const
    {
        QStringList list;
        list << "file";
        return list;
    }

    QStringList resourceType() const
    {
        QStringList list;
        list << "rsw";
        list << "mtw";
        list << "tif";
        list << "tiff";
        list << "map";
        list << "img";
        list << "jpeg";
        list << "jpg";
        list << "bmp";
        list << "psp";
        list << "png";
        list << "jp2";
        list << "j2k";
        return list;
    }

    virtual bool open(const QUrl &url, WorkMode mode = Both);
    virtual bool reOpen(const QUrl &url, WorkMode mode = Both) {return false;}
    virtual bool saveAs(const QUrl &url, WorkMode mode = Both) {return false;}

    virtual bool isFinished()
    {
        return false;
    }

    //rdp
    virtual QRect boundingRect()
    {
        return QRect();
    }

    virtual QRectF spatialRect();
    virtual QPolygonF spatialPolygon();
    virtual uint32_t rasterCount()
    {
        return 0;
    }

    virtual int readData(image_types::TImage &img, QRect source = QRect(), QSize dest = QSize(), bool async = false, uint32_t band = 1)
    {
        return 0;
    }

    virtual int writeData(image_types::TImage &img, QRect source = QRect(), QSize dest = QSize(), uint32_t band = 1)
    {
        return 0;
    }

    virtual int readRawData(image_types::TImage &img, QRect source = QRect(), QSize dest = QSize())
    {
        return 0;
    }

    virtual int readSpatialData(image_types::TImage &img, QRectF source, QRect dest = QRect(), uint32_t band = 1);
    virtual int readSpatialData(image_types::TImage &img, int epsgCode, QRectF source, QSize dest = QSize(), uint32_t band = 1);
    virtual int readSpatialData(image_types::TImage &img, const QString & proj4code, QRectF source, QSize dest = QSize(), uint32_t band = 1);
    virtual int registerDataListener(image_types::TImage &img, QMutex &lock)
    {
        return 0;
    }

    virtual void destroyDataListener(int num){}
    virtual qreal updateInterval()
    {
        return 0;
    }

    virtual void abort();
    virtual void abort(int task){}

private:
    virtual void loadRaster(){}
    virtual void saveRaster(){}

protected:
    QString m_layerName;
    QRectF m_boundingRect;

private:

signals:
    void imageReceived();
    void loadingFinished();
    void signal_recivedImage(const QPixmap &, uint providerId, uint returnId);
    //////////////////////////////////////////////////////////////////////////////
public:
signals:
    void dataReady(int listener);
    void signal_receivedImage(const QPixmap pixmap, const QString &url);
    //////////////////////////////////////////////////////////////////////////////
};

}

#endif
