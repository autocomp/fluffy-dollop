#include "rbp_file_load_manager.h"

#include <functional>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

using namespace regionbiz;

void FileLoadManager::addFileOnLoad( BaseFileKeeperPtr file )
{
    _files_on_load.push_back( file->getPath() );
    _types[ file->getPath() ] = file->getType();
    saveFileList();
}

bool FileLoadManager::isFileOnLoad( BaseFileKeeperPtr file )
{
    auto it = std::find( _files_on_load.begin(), _files_on_load.end(), file->getPath() );
    bool finded = it != _files_on_load.end();

    return finded;
}

void FileLoadManager::stopFileLoad( BaseFileKeeperPtr file )
{
    auto it = std::find( _files_on_load.begin(), _files_on_load.end(), file->getPath() );
    bool finded = it != _files_on_load.end();

    if( finded )
        _files_on_load.erase( it );

    saveFileList();
}

std::vector<BaseFileKeeperPtr> FileLoadManager::getFilesForLoad()
{
    std::vector<BaseFileKeeperPtr> files;
    for( QString path: _files_on_load )
    {
        auto file = FileKeeperFabric::createFile( path, 0, _types[ path ] );
        if( file )
            files.push_back( file );
    }

    return files;
}

void FileLoadManager::init( QString file_list, QString cache_dir )
{
    _load_file = file_list;
    _cache_dir = QDir( cache_dir );
    loadFileList();
}

void FileLoadManager::loadFileList()
{
    // read data from file
    QString file_path = _cache_dir.path() + QDir::separator() + _load_file;
    QFile json_file( file_path );
    if( !json_file.open( QFile::ReadOnly ))
        return;
    QByteArray json_data = json_file.readAll();
    json_file.close();

    // read json
    QJsonDocument json_doc = QJsonDocument::fromJson( json_data );
    QJsonObject obj = json_doc.object();
    if( obj.contains( "files" ))
    {
        for( QJsonValue file: obj[ "files" ].toArray() )
        {
            auto file_obj = file.toObject();
            QString name = file_obj[ "name" ].toString();
            BaseFileKeeper::FileType type
                    = (BaseFileKeeper::FileType) file_obj[ "type" ].toInt();

            _files_on_load.push_back( name );
            _types[ name ] = type;
        }
    }
}

void FileLoadManager::saveFileList()
{
    // append all files
    QJsonObject obj;
    QJsonArray files;
    for( QString file: _files_on_load )
    {
        QJsonObject fl_obj;
        fl_obj[ "name" ] = file;
        fl_obj[ "type" ] = (int) _types[ file ];
        files.append( fl_obj );
    }
    obj[ "files" ] = files;

    QJsonDocument doc;
    doc.setObject( obj );

    // create folder for file
    if( !_cache_dir.exists() )
        _cache_dir.mkpath( "." );

    // write to file
    QByteArray json_data = doc.toJson();
    QFile json_file( _cache_dir.path() + QDir::separator() + _load_file );
    if( json_file.open( QFile::WriteOnly ))
    {
        json_file.write( json_data );
        json_file.flush();
        json_file.close();
    }
}
