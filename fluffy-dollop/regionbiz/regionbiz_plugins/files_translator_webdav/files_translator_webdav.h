#ifndef FILES_TRANSLATOR_WEBDAV_H
#define FILES_TRANSLATOR_WEBDAV_H

#include <map>

#include <regionbiz/rb_files_translator.h>
#include <regionbiz/rb_plugin_register.h>

#include "webdav_queue.h"
#include "cache_manager.h"
#include "upload_manager.h"

#define DEFAULT_CACHE_DIR "webdav_cache"
#define DEFAULT_UPDATE_DELAY 10

namespace regionbiz {

REGISTER_PLUGIN( files_tranlator_webdav )

class FilesTranslatorWebDav: public QObject, public BaseFilesTranslator
{
    Q_OBJECT
public:
    QString getTranslatorName() override;

protected:
    void loadFunctions() override;
    bool initBySettings( QVariantMap settings ) override;

private Q_SLOTS:
    void onPutFile( QString file_id );
    void onGetFile( QString file_id );

    void onError();

private:
    QFilePtr getFile( BaseFileKeeperPtr file_ptr );
    BaseFileKeeper::FileState getFileState( BaseFileKeeperPtr file );
    BaseFileKeeper::FileState syncFile( BaseFileKeeperPtr file );
    BaseFileKeeperPtr addFile( QString local_path, QString inner_path,
                               BaseFileKeeper::FileType type , uint64_t entity_id);
    void deleteFile( BaseFileKeeperPtr file );
    BaseFileKeeperPtrs getFilesOnProcess();

    void initConnects();

    WebDavQueue* _webdav;
    CacheManager _cache;
    UploadManager _upload_manager;
};

}

#endif // FILES_TRANSLATOR_WEBDAV_H
