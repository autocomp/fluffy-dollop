#include "rbp_files_translator_ftp.h"

#include <iostream>
#include <QTimer>

using namespace regionbiz;

// register translator
REGISTER_TRANSLATOR(FilesTranslatorFtp)

FilesTranslatorFtp::FilesTranslatorFtp()
{
    _ftp_wrapper._callback_add = std::bind( &FilesTranslatorFtp::onAddFile, this, std::placeholders::_1 );
    _ftp_wrapper._callback_sync = std::bind( &FilesTranslatorFtp::onSyncFile, this, std::placeholders::_1 );
    _ftp_wrapper._callback_del = std::bind( &FilesTranslatorFtp::onDeleteFile, this, std::placeholders::_1 );
}

QString FilesTranslatorFtp::getTranslatorName()
{
    return "ftp";
}

void FilesTranslatorFtp::loadFunctions()
{
    _get_file = std::bind( &FilesTranslatorFtp::getFile, this, std::placeholders::_1 );
    _get_file_state = std::bind( &FilesTranslatorFtp::getFileState, this, std::placeholders::_1 );
    _sync_file = std::bind( &FilesTranslatorFtp::syncFile, this, std::placeholders::_1 );
    _add_file = std::bind( &FilesTranslatorFtp::addFile, this,
                           std::placeholders::_1, std::placeholders::_2,
                           std::placeholders::_3, std::placeholders::_4 );
    _delete_file = std::bind( &FilesTranslatorFtp::deleteFile, this,
                              std::placeholders::_1 );
}

bool regionbiz::FilesTranslatorFtp::initBySettings(QVariantMap settings)
{
    if( settings.contains( "cache_dir" ))
    {
        QString cache_dir_str = settings[ "cache_dir" ].toString();
        _ftp_wrapper._cache_dir = QDir( cache_dir_str );
    }
    else
        _ftp_wrapper._cache_dir = QDir( DEFAULT_CACHE_DIR );

    if( settings.contains( "tree_file" ))
        _ftp_wrapper._tree_file = settings[ "tree_file" ].toString();
    else
        _ftp_wrapper._tree_file = DEFAULT_TREE_FILE;

    if( !settings.contains( "url" ))
        return false;

    QUrl url( settings[ "url" ].toString() );
    if ( url.isValid() )
    {
        _ftp_wrapper._url = url;
        _ftp_wrapper.connectByUrl( url );

        // NOTE it's not a real image of FTP (shift by time)
        // load sync data from file
        _ftp_wrapper.loadTreeFromFile();

        uint delay = DEFAULT_UPDATE_DELAY;
        if( settings.contains( "update_delay" ))
            delay = settings[ "update_delay" ].toUInt();

        QTimer* timer = new QTimer( this );
        QObject::connect( timer, SIGNAL( timeout() ),
                          &_ftp_wrapper, SLOT( restartSync() ));
        timer->start( delay );
    }

    return true;
}

QFilePtr FilesTranslatorFtp::getFile( BaseFileKeeperPtr file_ptr )
{
    bool local( true );
    QString abs_path = _ftp_wrapper.getFullPath( file_ptr, local );
    QFileInfo file_info( abs_path );
    if( !file_info.exists() )
        return QFilePtr( new QFile() );
    else
    {
        QFile* file_star =  new QFile( abs_path );
        QFilePtr file_by_name = QFilePtr( file_star );
        return file_by_name;
    }
}

BaseFileKeeper::FileState FilesTranslatorFtp::getFileState( BaseFileKeeperPtr file )
{
    // check for valid
    if( !_ftp_wrapper.isValid() )
        return BaseFileKeeper::FS_INVALID;

    // find in tree
    bool local( true );
    QString ftp_path = _ftp_wrapper.getFullPath( file, !local );
    FtpTreeNodePtr cur_node = _ftp_wrapper.getNodeByPath( ftp_path );

    // check that file
    if( !cur_node
            || !cur_node->getInfo().isFile() )
       return BaseFileKeeper::FS_INVALID;

    // get file on local
    QString local_path = _ftp_wrapper.getFullPath( file, local );
    QFileInfo file_info( local_path );
    if( !file_info.exists() )
        return BaseFileKeeper::FS_UNSYNC;

    // get last modifed
    QDateTime last_modified_on_ftp = cur_node->getInfo().lastModified();
    QDateTime last_modified_local = file_info.lastModified();
    uint64_t size_on_ftp = cur_node->getInfo().size();
    uint64_t size_local = file_info.size();
    if( last_modified_on_ftp <= last_modified_local
            && size_local == size_on_ftp )
        return BaseFileKeeper::FS_SYNC;
    else
        return BaseFileKeeper::FS_UNSYNC;
}

BaseFileKeeper::FileState FilesTranslatorFtp::syncFile( BaseFileKeeperPtr file )
{
    auto state = getFileState( file );
    switch( state )
    {
    case BaseFileKeeper::FS_INVALID:
        break;

    case BaseFileKeeper::FS_SYNC:
    {
        onSyncFile( file );
        break;
    }

    case BaseFileKeeper::FS_UNSYNC:
    {
        _ftp_wrapper.getFile( file );
        break;
    }

    }

    return state;
}

BaseFileKeeperPtr FilesTranslatorFtp::addFile( QString local_path, QString inner_path,
                                               BaseFileKeeper::FileType type, uint64_t entity_id )
{
    auto file = FileKeeperFabric::createFile( inner_path, entity_id, type );
    if( file )
    {
        bool local( true );
        QString inner_local_path = _ftp_wrapper.getFullPath( file, local );
        QFile local_file( local_path );
        QFileInfo inner_info( inner_local_path );
        if( !inner_info.dir().exists() )
            inner_info.dir().mkpath( "." );
        if( local_file.copy( inner_local_path ))
            _ftp_wrapper.putFile( file, type );
    }
    return file;
}

void FilesTranslatorFtp::deleteFile( BaseFileKeeperPtr file )
{
    // delete folder
    bool local( true );
    QString local_path = _ftp_wrapper.getFullPath( file, local );
    QFileInfo local_file( local_path );
    if( local_file.exists() )
        local_file.dir().removeRecursively();

    // delete on ftp
    _ftp_wrapper.deleteFile( file );
}

