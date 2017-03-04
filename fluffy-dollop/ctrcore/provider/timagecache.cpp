#include "timagecache.h"
#include <QStringList>
#include <QRegExp>
#include <QDebug>

TImageCache* TImageCache::m_Instance = 0;

TImageCache::TImageCache()
    : m_images(4000)
    , m_isSingleton(false)
{}

TImageCache::~TImageCache()
{
    if(m_isSingleton)
        if(m_Instance)
        {
            delete m_Instance;
            m_Instance = 0;
        }
}


bool TImageCache::insert(const QString& key, const image_types::TImage* const src)
{
    QMap<QString, image_types::TImage*>::iterator it(m_map.find(key));
    if(it != m_map.end())
    {
        image_types::TImage* dest = it.value();
        if(dest->depth == src->depth && dest->w == src->w && dest->h == src->h)
        {
            memcpy(dest->prt, src->prt, dest->w * dest->h * (dest->depth / 8));
        }
        else
        {
            dest->depth = src->depth;
            dest->w = src->w;
            dest->h = src->h;
            delete []dest->prt;
            dest->prt = new char[dest->w * dest->h * (dest->depth / 8)];
            memcpy(dest->prt, src->prt, dest->w * dest->h * (dest->depth / 8));
        }
    }
    else
    {
        image_types::TImage* dest = new image_types::TImage;
        dest->depth = src->depth;
        dest->w = src->w;
        dest->h = src->h;
        dest->prt = new char[dest->w * dest->h * (dest->depth / 8)];
        memcpy(dest->prt, src->prt, dest->w * dest->h * (dest->depth / 8));
        m_map.insert(key, dest);
        m_queue.enqueue(key);
        checkSize();
    }
    return true;
}


void TImageCache::checkSize()
{
    while(m_queue.count() > m_images)
    {
        const QString key(m_queue.dequeue());
        QMap<QString, image_types::TImage*>::iterator it(m_map.find(key));
        if(it != m_map.end())
        {
            image_types::TImage* img = it.value();
            delete []img->prt;
            img->prt = 0;
            delete img;

            m_map.erase(it);
        }
    }
}

bool TImageCache::contains(const QString& key)
{
    QMap<QString, image_types::TImage*>::iterator it(m_map.find(key));
    return it != m_map.end();
}

bool TImageCache::find(const QString& key, image_types::TImage* dest)
{
    QMap<QString, image_types::TImage*>::iterator it(m_map.find(key));
    if(it != m_map.end())
    {
        if(dest->prt)
            delete []dest->prt;

        image_types::TImage* src = it.value();
        dest->depth = src->depth;
        dest->w = src->w;
        dest->h = src->h;
        dest->prt = new char[dest->w * dest->h * (dest->depth / 8)];
        memcpy(dest->prt, src->prt, dest->w * dest->h * (dest->depth / 8));
        return true;
    }
    else
    {
        return false;
    }
}


void TImageCache::setCacheLimit(unsigned int images)
{
    m_images = images;
    checkSize();
}


void TImageCache::clear()
{
    for(QMap<QString, image_types::TImage*>::iterator it(m_map.begin()); it != m_map.end(); ++it)
    {
        image_types::TImage* img = it.value();
        delete []img->prt;
        img->prt = 0;
        delete img;
    }
    m_map.clear();
    m_queue.clear();
}


void TImageCache::clearProviderData(QString firstPartOfKey)
{
    QStringList deletingItems;
    for(auto it = m_queue.begin(); it != m_queue.end(); ++it)
    {
        QString key = *it;
        QString _firstPartOfKey = key.section(QRegExp("_"), 0, 0);
        if(firstPartOfKey == _firstPartOfKey)
        {
            auto mapIt = m_map.find(key);
            if(mapIt != m_map.end())
            {
                image_types::TImage* img = mapIt.value();
                delete []img->prt;
                img->prt = 0;
                delete img;

                m_map.erase(mapIt);
            }
            deletingItems.append(key);
        }
    }
    foreach(QString deletingItem, deletingItems)
        m_queue.removeAll(deletingItem);
}

