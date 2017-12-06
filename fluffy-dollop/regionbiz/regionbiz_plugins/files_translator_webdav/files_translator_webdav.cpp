#include "files_translator_webdav.h"

#include <regionbiz/rb_manager.h>

using namespace regionbiz;

// register translator
REGISTER_TRANSLATOR(FilesTranslatorWebDav)

QString FilesTranslatorWebDav::getTranslatorName()
{
    return "webdav";
}

void FilesTranslatorWebDav::loadFunctions()
{
    _get_file = std::bind( &FilesTranslatorWebDav::getFile, this, std::placeholders::_1 );
    _get_file_state = std::bind( &FilesTranslatorWebDav::getFileState, this, std::placeholders::_1 );
    _sync_file = std::bind( &FilesTranslatorWebDav::syncFile, this, std::placeholders::_1 );
    _add_file = std::bind( &FilesTranslatorWebDav::addFile, this,
                           std::placeholders::_1, std::placeholders::_2,
                           std::placeholders::_3, std::placeholders::_4 );
    _delete_file = std::bind( &FilesTranslatorWebDav::deleteFile, this,
                              std::placeholders::_1 );
    _get_files_on_process = std::bind( &FilesTranslatorWebDav::getFilesOnProcess, this );
}

bool FilesTranslatorWebDav::initBySettings(QVariantMap settings)
{
    // init cache
    QString cache_dir_str;
    if( settings.contains( "cache_dir" ))
    {
        cache_dir_str = settings[ "cache_dir" ].toString();
    }
    else
        cache_dir_str = DEFAULT_CACHE_DIR;
    _cache.setDir( cache_dir_str );

    // get params
    WebDavParams params;
    if( settings.contains( "host" )
            && settings.contains( "path" )
            && settings.contains( "user" )
            && settings.contains( "pass" )
            && settings.contains( "port" ))
    {
        params.host = settings[ "host" ].toString();
        params.path = settings[ "path" ].toString();
        params.user = settings[ "user" ].toString();
        params.pass = settings[ "pass" ].toString();
        params.port = settings[ "port" ].toUInt();
    }
    else
        return false;

    if( settings.contains( "queue_size" ))
    {
        params.queue_size = settings[ "queue_size" ].toUInt();
    }
    else
        params.queue_size = 1;

    if( settings.contains( "updaete_delay_sec" ))
    {
        params.update_list_sec_delay = settings[ "updaete_delay_sec" ].toUInt();
    }
    else
        params.update_list_sec_delay = DEFAULT_UPDATE_DELAY;

    // init
    _webdav = new WebDavQueue( this );
    initConnects();
    _webdav->init( params );

    // return
    return true;
}

void FilesTranslatorWebDav::onPutFile( QString file_id )
{
    _upload_manager.eraseFile( file_id );

    auto mngr = RegionBizManager::instance();
    auto file = mngr->getFileByPath( file_id );
    if( file )
        onAddFile( file );
}

void FilesTranslatorWebDav::onGetFile( QString file_id )
{
    auto mngr = RegionBizManager::instance();
    auto file = mngr->getFileByPath( file_id );
    if( file )
        onSyncFile( file );
}

void FilesTranslatorWebDav::onError()
{
    // TODO webdav error behavior
    _upload_manager.clear();
}

QFilePtr FilesTranslatorWebDav::getFile( BaseFileKeeperPtr file_ptr )
{
    QString id = file_ptr->getId();
    if( _cache.isFileExist( id ))
    {
        QString path = _cache.getFullPath( id );
        QFile* file_star =  new QFile( path );
        QFilePtr file_by_name = QFilePtr( file_star );
        return file_by_name;
    }

    return nullptr;
}

BaseFileKeeper::FileState FilesTranslatorWebDav::getFileState(BaseFileKeeperPtr file)
{
    QString id = file->getId();

    if( _upload_manager.isFileOnUpload( id ))
        return BaseFileKeeper::FS_UPLOAD;

    if( !_webdav->isItemPresent( id ))
        return BaseFileKeeper::FS_INVALID;

    uint size = _cache.getFileSize( id );
    if( _webdav->isFileSynced( id, size ))
        return BaseFileKeeper::FS_SYNC;

    return BaseFileKeeper::FS_UNSYNC;
}

BaseFileKeeper::FileState FilesTranslatorWebDav::syncFile(BaseFileKeeperPtr file)
{
    auto state = getFileState( file );

    switch (state)
    {
    case BaseFileKeeper::FS_INVALID:
    case BaseFileKeeper::FS_UPLOAD:
        break;

    case BaseFileKeeper::FS_SYNC:
        onSyncFile( file );
        break;

    case BaseFileKeeper::FS_UNSYNC:
    {
        QString id = file->getId();
        QString local_file = _cache.getFullPath( id );
        _webdav->addOperation( { OT_GET, id, local_file } );
        break;
    }

    }

    return state;
}

BaseFileKeeperPtr FilesTranslatorWebDav::addFile(QString local_path, QString inner_path, BaseFileKeeper::FileType type, uint64_t entity_id)
{
    auto file = FileKeeperFabric::createFile( inner_path, entity_id, type );
    if( file )
    {
        QString id = file->getId();
        QString inner_local_path = _cache.getFullPath( id );

        QFile local_file( local_path );
        if( local_file.copy( inner_local_path ))
        {
            _webdav->addOperation( { OT_PUT, id, inner_local_path } );
            _upload_manager.addFileId( id );
        }
    }
    return file;
}

void FilesTranslatorWebDav::deleteFile(BaseFileKeeperPtr file)
{
    // delete file
    QString id = file->getId();
    QString local_path = _cache.getFullPath( id );

    QFileInfo local_file( local_path );
    if( local_file.exists() )
        local_file.dir().remove( local_path );

    // delete on ftp
    _webdav->addOperation( { OT_DELETE, id, "" } );

    onDeleteFile( file );
}

BaseFileKeeperPtrs FilesTranslatorWebDav::getFilesOnProcess()
{
    auto mngr = RegionBizManager::instance();
    BaseFileKeeperPtrs res;

    auto files = _upload_manager.getFilesIds();
    for( QString file_id: files )
    {
        auto file = mngr->getFileByPath( file_id );
        if( file )
            res.push_back( file );
    }

    return res;
}

void FilesTranslatorWebDav::initConnects()
{
    connect( _webdav, SIGNAL( filePut( QString )), SLOT( onPutFile( QString )));
    connect( _webdav, SIGNAL( fileGet( QString )), SLOT( onGetFile( QString )));

    connect( _webdav, SIGNAL( error() ), SLOT( onError() ));
}
