#ifndef CACHE_MANAGER_H
#define CACHE_MANAGER_H

#include <QString>
#include <QDir>

namespace regionbiz
{

class CacheManager
{
public:
    void setDir(QString path);
    quint64 getFileSize( QString file );
    bool isFileExist( QString file );
    QString getFullPath( QString file );

private:
    QDir _cache = QDir();
};

}

#endif // CACHE_MANAGER_H
