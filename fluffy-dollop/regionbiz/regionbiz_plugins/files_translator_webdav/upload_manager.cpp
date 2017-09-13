#include "upload_manager.h"

void UploadManager::addFileId(QString id)
{
    if( isFileOnUpload( id ))
        return;

    _ids.push_back( id );
}

bool UploadManager::isFileOnUpload(QString id)
{
    auto it = std::find( _ids.begin(), _ids.end(), id );
    bool inside = it != _ids.end();

    return inside;
}

void UploadManager::eraseFile(QString id)
{
    auto it = std::find( _ids.begin(), _ids.end(), id );
    bool inside = it != _ids.end();
    if( inside )
        _ids.erase( it );
}

void UploadManager::clear()
{
    _ids.clear();
}
