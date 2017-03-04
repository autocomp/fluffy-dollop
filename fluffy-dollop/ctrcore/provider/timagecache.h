#ifndef TIMAGECACHE_H
#define TIMAGECACHE_H

#include <QString>
#include <QMap>
#include <QQueue>
#include <ctrcore/provider/t_image.h>
class TImageCache
{
public:
    static TImageCache *instance()
    {
        if(!m_Instance)
        {
            m_Instance = new TImageCache;
            m_Instance->m_isSingleton = true;
        }

        return m_Instance;
    }

    TImageCache();
    ~TImageCache();

    bool insert(const QString &key, const image_types::TImage *const img);
    bool find(const QString &key, image_types::TImage *img);
    bool contains(const QString &key);
    void setCacheLimit(unsigned int images);
    unsigned int cacheLimit() {return m_images;}
    int size() {return m_map.size();}
    void clear();
    void clearProviderData(QString firstPartOfKey);

private:
    void checkSize();
    static TImageCache *m_Instance;
    bool m_isSingleton;
    unsigned int m_images;
    QMap<QString, image_types::TImage *> m_map;
    QQueue<QString> m_queue;
};

#endif // ifndef TIMAGECACHE_H

