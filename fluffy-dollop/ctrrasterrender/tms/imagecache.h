#ifndef IMAGECACHE_H
#define IMAGECACHE_H

#include <QString>
#include <QMap>
#include <QQueue>
#include <QByteArray>

class ImageCache
{
public:
    ImageCache(const QString &cacheDir, uint cacheSize);

    void insert(const QString &key, QByteArray *img);
    QString find(const QString& key, bool &ok);
    bool contains(const QString& key);
    void setCacheLimit(unsigned int images);
    void clear();

    //! метод генерирует и возвращает имя файла в каталоге с кэшем. При последующей загрузке все файлы будет удален.
    QString getFileNameInCacheDir(QString ext = "tiff");

private:
    void checkSize();
    unsigned int m_cacheSize;
    const QString m_cacheDir;
    uint m_fileNumber;
    QMap<QString, QString> m_map;
    QQueue<QString> m_queue;
};

#endif

