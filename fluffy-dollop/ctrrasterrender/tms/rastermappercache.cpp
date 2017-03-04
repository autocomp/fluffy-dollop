#include "rastermappercache.h"
#include <QDebug>
#include <QDir>
#include <QCryptographicHash>
#include <QImage>

RasterMapperCache::RasterMapperCache(QString providerUrl)
    : _providerUrl(providerUrl)
{
}

void RasterMapperCache::setSavePath(QString savePath)
{
    // QString providerDir = //QCryptographicHash::hash(_providerUrl.toLocal8Bit(), QCryptographicHash::Md5).toHex();
    QString providerDir = _providerUrl;
    providerDir.replace(QString(QDir::separator()), QString("_"));
    providerDir.replace(QString("."), QString("_"));

    QDir saveDir(savePath + QDir::separator() + providerDir);
    if(saveDir.exists() == false)
    {
        QDir dir(savePath);
        if(dir.mkdir(providerDir))
        {
            _savePath = savePath + QDir::separator() + providerDir;
        }
        else
        {
            qDebug() << "RasterMapperCache::setSavePath - dont make dir :" << QString(savePath + QDir::separator() + providerDir);
        }
    }
    else
    {
        // проверка в этой папке.
        _savePath = savePath + QDir::separator() + providerDir;

//        qDebug() << "RasterMapperCache::setSavePath, Files in dir :" << _savePath;
        foreach(QString fileName, saveDir.entryList())
            if(fileName != QString(".") && fileName != QString(".."))
            {
                m_savedTiles.insert(fileName);
//                qDebug() << "--->" << fileName;
            }
//        qDebug() << "---------------------";
    }
}


bool RasterMapperCache::insert(const QString& key, const image_types::TImage* const src, bool rewrite)
{
//    qDebug() << "RasterMapperCache::insert, key :" << key << ", savePath :" << _savePath;

    if(_savePath.isEmpty() == false)
    {
        QString fileName(key + ".png");
        auto it = m_savedTiles.find(fileName);
        if(it == m_savedTiles.end()) // || (it == m_savedTiles.end() && rewrite) )
        {
            QImage img( ( (const uchar*)(src->prt) ), src->w, src->h, QImage::Format_ARGB32);
            if(img.save(QString(_savePath + QDir::separator() + fileName), "PNG"))
            {
                m_savedTiles.insert(fileName);
                return true;
            }
        }
        else if(rewrite)
        {
            QImage img( ( (const uchar*)(src->prt) ), src->w, src->h, QImage::Format_ARGB32);
            if(img.save(QString(_savePath + QDir::separator() + fileName), "PNG"))
                return true;
        }
        else
            return true;
    }

    return false;
}

bool RasterMapperCache::contains(const QString& key)
{
    if(_savePath.isEmpty())
        return false;

    QString fileName(key + ".png");
    auto it = m_savedTiles.find(fileName);
    return (it != m_savedTiles.end());
}

bool RasterMapperCache::find(const QString& key, image_types::TImage* dest)
{
//    qDebug() << "---> RasterMapperCache::find key:" << key << ", savePath:" << _savePath;
//    foreach(QString filename, m_savedTiles)
//        qDebug() << "filename :" << filename;
//    qDebug() << "---------";

    if(_savePath.isEmpty() == false)
    {
        QString fileName(key + ".png");
        auto it = m_savedTiles.find(fileName);
        if(it != m_savedTiles.end())
        {
            QImage img(QString(_savePath + QDir::separator() + fileName), "PNG");
            delete []dest->prt;
            dest->depth = img.depth();
            dest->w = img.width();
            dest->h = img.height();
            dest->prt = new char[dest->w * dest->h * (dest->depth / 8)];
            memcpy(dest->prt, img.constBits(), dest->w * dest->h * (dest->depth / 8));
            return true;
        }
    }

    return false;
}

void RasterMapperCache::clear()
{
    m_savedTiles.clear();
    if(_savePath.isEmpty() == false)
    {
        QDir dir(_savePath);
        foreach(QString fileName, dir.entryList())
            if(fileName != QString(".") && fileName != QString(".."))
            {
                qDebug() << "RasterMapperCache::clear--->" << fileName << dir.remove(fileName);
            }
    }
}



