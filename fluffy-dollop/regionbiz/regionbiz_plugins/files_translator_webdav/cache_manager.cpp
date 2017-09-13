#include "cache_manager.h"

void regionbiz::CacheManager::setDir(QString path)
{
    _cache = QDir( path );
    if( !_cache.exists() )
        _cache.mkpath( "." );
}

quint64 regionbiz::CacheManager::getFileSize(QString file)
{
    QFileInfo info( _cache.path() + QDir::separator() + file );
    if( !info.exists() )
        return 0;

    return info.size();
}

bool regionbiz::CacheManager::isFileExist(QString file)
{
    QFileInfo info( _cache.path() + QDir::separator() + file );
    return info.exists();
}

QString regionbiz::CacheManager::getFullPath(QString file)
{
    QFileInfo info( _cache.path() + QDir::separator() + file );
    return info.absoluteFilePath();
}
