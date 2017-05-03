#ifndef RB_FILES_TRANSLATOR_H
#define RB_FILES_TRANSLATOR_H

#include <memory>
#include <functional>
#include <QObject>
#include <QVariantMap>
#include <QFile>

#include "rb_files.h"
#include "rb_translator.h"

#define SEPARATOR "/"

Q_DECLARE_METATYPE( regionbiz::BaseFileKeeperPtr )

namespace regionbiz {

class FileSyncer: public QObject
{
    Q_OBJECT
public:
    FileSyncer();
    void onSyncFile( BaseFileKeeperPtr file );
    void onFileAdded( BaseFileKeeperPtr file );

Q_SIGNALS:
    void fileSynced( BaseFileKeeperPtr file );
    void fileAdded( BaseFileKeeperPtr file );
};

class BaseFilesTranslator: public BaseTranslator
{
public:
    QFilePtr getFile( BaseFileKeeperPtr file );
    BaseFileKeeper::FileState getFileState( BaseFileKeeperPtr file );
    BaseFileKeeper::FileState syncFile( BaseFileKeeperPtr file );
    BaseFileKeeperPtr addFile(QString file_path , BaseFileKeeper::FileType type, uint64_t entity_id);

    void subscribeFileSynced( QObject* obj,
                              const char *slot );
    void subscribeFileAdded( QObject* obj,
                             const char *slot );

protected:
    void onSyncFile( BaseFileKeeperPtr file );
    void onAddFile( BaseFileKeeperPtr file );
    void appendFile( BaseFileKeeperPtr file );

    std::function< QFilePtr( BaseFileKeeperPtr ) > _get_file;
    std::function< BaseFileKeeper::FileState( BaseFileKeeperPtr ) > _get_file_state;
    std::function< BaseFileKeeper::FileState( BaseFileKeeperPtr ) > _sync_file;
    std::function< BaseFileKeeperPtr( QString local_path, QString inner_path,
                                      BaseFileKeeper::FileType type,
                                      uint64_t entity_id ) > _add_file;

private:
    FileSyncer _file_syncer;
};
typedef std::shared_ptr< BaseFilesTranslator > BaseFilesTranslatorPtr;
typedef std::vector< BaseFilesTranslatorPtr > BaseFilesTranslatorPtrs;

}

#endif // RB_FILES_TRANSLATOR_H
