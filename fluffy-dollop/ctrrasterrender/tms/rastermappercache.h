#ifndef RASTERMAPPERCACHE_H
#define RASTERMAPPERCACHE_H

#include <QString>
#include <QSet>
#include <ctrcore/provider/t_image.h>

class RasterMapperCache
{
public:
    RasterMapperCache(QString providerUrl);

    void setSavePath(QString savePath);
    bool insert(const QString &key, const image_types::TImage *const img, bool rewrite = false);
    bool find(const QString &key, image_types::TImage *img);
    bool contains(const QString &key);
    int size() {return m_savedTiles.size();}
    void clear();

private:
    QString _savePath;
    const QString _providerUrl;
    QSet<QString> m_savedTiles;
};

#endif //

