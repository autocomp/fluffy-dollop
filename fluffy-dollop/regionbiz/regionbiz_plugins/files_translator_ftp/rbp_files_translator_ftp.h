#ifndef RB_FILES_TRANSLATOR_FTP_H
#define RB_FILES_TRANSLATOR_FTP_H

#include <map>

#include <regionbiz/rb_files_translator.h>
#include <regionbiz/rb_plugin_register.h>

#include "rbp_ftp_wrapper.h"

#define DEFAULT_CACHE_DIR "ftp_cache"
#define DEFAULT_TREE_FILE "tree_file.json"
#define DEFAULT_UPDATE_DELAY 10000

namespace regionbiz {

REGISTER_PLUGIN( files_tranlator_ftp )

class FilesTranslatorFtp: public QObject, public BaseFilesTranslator
{
    Q_OBJECT
public:
    FilesTranslatorFtp();

    QString getTranslatorName() override;

protected:
    void loadFunctions() override;
    bool initBySettings( QVariantMap settings ) override;

private:
    QFilePtr getFile( BaseFileKeeperPtr file_ptr );
    BaseFileKeeper::FileState getFileState( BaseFileKeeperPtr file );
    BaseFileKeeper::FileState syncFile( BaseFileKeeperPtr file );
    BaseFileKeeperPtr addFile( QString local_path, QString inner_path,
                               BaseFileKeeper::FileType type , uint64_t entity_id);
    void deleteFile( BaseFileKeeperPtr file );

    FtpWrapper _ftp_wrapper;
};

}

#endif // RB_FILES_TRANSLATOR_FTP_H
