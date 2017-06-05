#ifndef RBP_FTP_WRAPPER_H
#define RBP_FTP_WRAPPER_H

#include <memory>
#include <functional>
#include <QFile>
#include <QDir>
#include <QUrl>

#include <qftp/qurlinfo.h>
#include <qftp/qftp.h>
#include <regionbiz/rb_files.h>

namespace regionbiz {

class FtpTreeNode;
typedef std::shared_ptr< FtpTreeNode > FtpTreeNodePtr;

class FtpTreeNode
{
public:
    void appendUrlInfo( QUrlInfo info, FtpTreeNodePtr current_node );
    FtpTreeNodePtr getParent();
    FtpTreeNodePtr getNextChild();
    QUrlInfo getInfo();
    std::vector< FtpTreeNodePtr >* getChilds();
    void startUpdateChilds();
    void freeNonUpdatedChilds();

private:
    QUrlInfo _info;
    std::vector< FtpTreeNodePtr > _childs;
    uint _current_child_num = 0;
    FtpTreeNodePtr _parent = nullptr;
    bool _updated = true;
};

class FtpWrapper: public QObject
{
    Q_OBJECT
public:
    FtpWrapper();

    QString getFullPath(BaseFileKeeperPtr file_ptr, bool local );
    void connectByUrl( QUrl url );
    void loadTreeFromFile();
    void saveTreeToFile();
    bool isValid();
    FtpTreeNodePtr getNodeByPath( QString ftp_path );
    void getFile( BaseFileKeeperPtr file );
    void putFile( BaseFileKeeperPtr file, BaseFileKeeper::FileType type );
    void deleteFile( BaseFileKeeperPtr file );

    std::function< void( BaseFileKeeperPtr ) > _callback_add;
    std::function< void( BaseFileKeeperPtr ) > _callback_sync;
    std::function< void( BaseFileKeeperPtr ) > _callback_del;

    QDir _cache_dir;
    QString _tree_file;
    QUrl _url;

public Q_SLOTS:
    void restartSync();

private Q_SLOTS:
    void ftpCommandFinished(int id, bool error );
    void addToList( QUrlInfo info );

private:
    QString getFullPath( FtpTreeNodePtr node );
    void loginByUrl( QUrl url );
    void closeConnection();
    QString getFileCategoryByType( BaseFileKeeper::FileType type );
    QString getFileCategoryByType( BaseFileKeeperPtr file );

    void recursiveReadFromFile(QJsonObject obj, FtpTreeNodePtr node );
    void recursiveSaveToFile( QJsonObject& obj, FtpTreeNodePtr node );

    QFtp _ftp;
    FtpTreeNodePtr _root_node = nullptr;
    FtpTreeNodePtr _current_node = nullptr;
    uint _current_node_num = 0;
    std::map< int, QFtp::Command > _commands;
    std::map< int, BaseFileKeeperPtr > _put_get_files;
    bool _block_update = false;
    QFile _input_file;
};

}

#endif // RBP_FTP_WRAPPER_H
