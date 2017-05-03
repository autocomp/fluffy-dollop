#ifndef RB_FILES_TRANSLATOR_FTP_H
#define RB_FILES_TRANSLATOR_FTP_H

#include <map>
#include <QObject>
#include <QUrl>
#include <QDir>

#include <qftp/qftp.h>
#include <regionbiz/rb_files_translator.h>
#include <regionbiz/rb_plugin_register.h>

#define DEFAULT_CACHE_DIR "ftp_cache"
#define DEFAULT_UPDATE_DELAY 60000

namespace regionbiz {

REGISTER_PLUGIN( files_tranlator_ftp )

class FtpTreeNode;
typedef std::shared_ptr< FtpTreeNode > FtpTreeNodePtr;

class FtpTreeNode
{
public:
    void appendUrlInfo(QUrlInfo info , FtpTreeNodePtr current_node);
    FtpTreeNodePtr getParent();
    FtpTreeNodePtr getNextChild();
    QUrlInfo getInfo();
    std::vector< FtpTreeNodePtr >* getChilds();

private:
    QUrlInfo _info;
    std::vector< FtpTreeNodePtr > _childs;
    uint _current_child_num = 0;
    FtpTreeNodePtr _parent = nullptr;
};

class FilesTranslatorFtp: public QObject, public BaseFilesTranslator
{
    Q_OBJECT
public:
    QString getTranslatorName() override;

protected:
    void loadFunctions() override;
    bool initBySettings( QVariantMap settings ) override;

private Q_SLOTS:
    void ftpCommandFinished(int id, bool error );
    void addToList( QUrlInfo info );
    void restartSync();

private:
    QFilePtr getFile( BaseFileKeeperPtr file_ptr );
    BaseFileKeeper::FileState getFileState( BaseFileKeeperPtr file );
    BaseFileKeeper::FileState syncFile( BaseFileKeeperPtr file );
    BaseFileKeeperPtr addFile(QString local_path , QString inner_path,
                               BaseFileKeeper::FileType type , uint64_t entity_id);
    QString getFullPath( FtpTreeNodePtr node );
    void loginByUrl( QUrl url );
    void connectByUrl( QUrl url );
    void closeConnection();
    QString getFileCategoryByType( BaseFileKeeper::FileType type );
    QString getFileCategoryByType( BaseFileKeeperPtr file );
    QString getFullPath(BaseFileKeeperPtr file_ptr, bool local );

    QFtp _ftp;
    QUrl _url;
    FtpTreeNodePtr _root_node = nullptr;
    FtpTreeNodePtr _current_node = nullptr;
    uint _current_node_num = 0;
    std::map< int, QFtp::Command > _commands;
    std::map< int, BaseFileKeeperPtr > _put_get_files;
    QDir _cache_dir;
    bool _block_update = false;
    QFile _input_file;
};

}

#endif // RB_FILES_TRANSLATOR_FTP_H
