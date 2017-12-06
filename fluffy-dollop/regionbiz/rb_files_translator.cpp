#include "rb_files_translator.h"

#include <QFileInfo>
#include <QUuid>
#include <QDir>
#include <iostream>

using namespace regionbiz;

QFilePtr BaseFilesTranslator::getFile( BaseFileKeeperPtr file )
{
    if( _get_file )
    {
        QFilePtr file_by_trans = _get_file( file );
        return file_by_trans;
    }

    return QFilePtr( new QFile() );
}

BaseFileKeeper::FileState BaseFilesTranslator::getFileState( BaseFileKeeperPtr file )
{
    if( _get_file_state )
    {
        BaseFileKeeper::FileState state = _get_file_state( file );
        return state;
    }

    return BaseFileKeeper::FS_INVALID;
}

BaseFileKeeper::FileState BaseFilesTranslator::syncFile(BaseFileKeeperPtr file )
{
    if( _sync_file )
    {
        BaseFileKeeper::FileState state = _sync_file( file );
        return state;
    }

    return BaseFileKeeper::FS_INVALID;
}

BaseFileKeeperPtr BaseFilesTranslator::addFile( QString file_path,
                                                BaseFileKeeper::FileType type,
                                                uint64_t entity_id )
{
    QFileInfo info( file_path );
    if( !info.exists() )
    {
        std::cerr << "File " << file_path.toUtf8().data()
                  << " don't exists!" << std::endl;
        return nullptr;
    }

    QString uuid = QUuid::createUuid().toString();
    // WARNING without file name
    QString inner_path = uuid;// + SEPARATOR + info.fileName();

    if( _add_file )
    {
        BaseFileKeeperPtr file = _add_file( file_path, inner_path, type, entity_id );
        return file;
    }

    return nullptr;
}

bool BaseFilesTranslator::deleteFile(BaseFileKeeperPtr file)
{
    if( _delete_file )
    {
        _delete_file( file );
        return true;
    }

    return false;
}

BaseFileKeeperPtrs BaseFilesTranslator::getFilesOnProcess()
{
    if( _get_files_on_process )
    {
        auto files = _get_files_on_process();
        return files;
    }
    return BaseFileKeeperPtrs();
}

bool BaseFilesTranslator::isHasFilesOnProcess()
{
    bool has = getFilesOnProcess().size();
    return has;
}

void BaseFilesTranslator::subscribeFileSynced( QObject *obj, const char *slot )
{
    QObject::connect( &_file_syncer, SIGNAL( fileSynced( BaseFileKeeperPtr )),
                      obj, slot, Qt::QueuedConnection );
}

void BaseFilesTranslator::subscribeFileAdded(QObject *obj, const char *slot)
{
    QObject::connect( &_file_syncer, SIGNAL( fileAdded( BaseFileKeeperPtr )),
                      obj, slot, Qt::QueuedConnection );
}

void BaseFilesTranslator::subscribeFileDeleted(QObject *obj, const char *slot)
{
    QObject::connect( &_file_syncer, SIGNAL( fileDeleted( BaseFileKeeperPtr )),
                      obj, slot, Qt::QueuedConnection );
}

void BaseFilesTranslator::onSyncFile( BaseFileKeeperPtr file )
{
    _file_syncer.onSyncFile( file );
}

void BaseFilesTranslator::onAddFile(BaseFileKeeperPtr file)
{
    _file_syncer.onFileAdded( file );
}

void BaseFilesTranslator::onDeleteFile(BaseFileKeeperPtr file)
{
    _file_syncer.onFileDeleted( file );
}

FileSyncer::FileSyncer()
{
    qRegisterMetaType< regionbiz::BaseFileKeeperPtr >( "BaseFileKeeperPtr" );
}

void FileSyncer::onSyncFile( BaseFileKeeperPtr file )
{
    Q_EMIT fileSynced( file );
}

void FileSyncer::onFileAdded(BaseFileKeeperPtr file)
{
    Q_EMIT fileAdded( file );
}

void FileSyncer::onFileDeleted(BaseFileKeeperPtr file)
{
    Q_EMIT fileDeleted( file );
}

