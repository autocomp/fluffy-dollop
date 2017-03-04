#include "imagecache.h"
#include <QDebug>
#include <QFile>
#include <QDir>

ImageCache::ImageCache(const QString & cacheDir, uint cacheSize)
    : m_cacheDir(cacheDir)
    , m_cacheSize(cacheSize)
    , m_fileNumber(0)
{
    QDir dir(cacheDir);
    if( ! dir.exists())
        dir.mkdir(cacheDir);

    QStringList list(dir.entryList());
    foreach(QString filename, list)
        dir.remove(filename);
}

QString ImageCache::getFileNameInCacheDir(QString ext)
{
    QString fileName(m_cacheDir + QDir::separator() + QString::number(++m_fileNumber) + "." + ext);
    return fileName;
}

void ImageCache::insert(const QString& key, QByteArray * byteArray)
{
    if(byteArray->size() > 0 && m_map.contains(key) == false)
    {
        QString fileName(m_cacheDir + QDir::separator() + QString::number(++m_fileNumber) + ".tiff");
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly))
        {
            file.write(*byteArray);
            file.close();

            m_map.insert(key, fileName);
            m_queue.enqueue(key);
            checkSize();

//            qDebug() << "insert, ImageCache::insert, key :" << key << ", m_queue.count() :" << m_queue.count() << ", fileName :" << fileName;
        }
    }
}


void ImageCache::checkSize()
{
    while(m_queue.count() > m_cacheSize)
    {
        const QString key(m_queue.dequeue());
        QMap<QString, QString>::iterator it(m_map.find(key));
        if(it != m_map.end())
        {
            QDir dir(m_cacheDir);
            QFileInfo fi(it.value());
            bool ok = dir.remove(fi.fileName());
//            qDebug() << "ImageCache::remove :" << ok << fi.fileName() << it.value();
            m_map.erase(it);
        }
//        else
//            qDebug() << "ImageCache::No removed !!! :" << key;
    }
//    qDebug() << "ImageCache::checkSize :" << m_queue.count();
}

bool ImageCache::contains(const QString& key)
{
    QMap<QString, QString>::iterator it(m_map.find(key));
    return it != m_map.end();
}

QString ImageCache::find(const QString& key, bool& succsess)
{
//    qDebug() << "ImageCache :";
//    for(QMap<QString, QString>::iterator it(m_map.begin()); it != m_map.end(); ++it)
//        qDebug() << "key :" << it.key() << ", value :" << it.value();
//    qDebug() << "==============";

    QMap<QString, QString>::iterator it(m_map.find(key));
    if(it != m_map.end())
    {
        succsess = true;
        return it.value();
    }

    succsess = false;
    return QString();
}


void ImageCache::setCacheLimit(unsigned int images)
{
    m_cacheSize = images;
    checkSize();
}


void ImageCache::clear()
{
    QDir dir(m_cacheDir);
    QStringList list(dir.entryList());
    foreach(QString filename, list)
        dir.remove(filename);

    m_map.clear();
    m_queue.clear();
}



