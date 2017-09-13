#ifndef WEBDAV_WRAPPER_H
#define WEBDAV_WRAPPER_H

#include <mutex>

#include <qwebdavlib/qwebdav.h>
#include <qwebdavlib/qwebdavdirparser.h>
#include <qwebdavlib/qwebdavitem.h>

namespace regionbiz {

struct WebDavParams
{
    QString host = "";
    QString path = "";
    QString user = "";
    QString pass = "";
    uint port = 0;

    uint queue_size = 1;
    uint update_list_sec_delay = 10;
};

class WebDavWrapper: public QObject
{
    Q_OBJECT
public:
    void init( WebDavParams params );

    QList< QWebdavItem > getItems();
    bool isItemPresent( QString id_file );
    QWebdavItem getItem( QString id_file );

    bool readAllItems();
    bool putFile( QString id_file, QString file_path_local );
    bool getFile(QString id_file, QString file_path_local );
    bool deleteFile( QString id_file );

Q_SIGNALS:
    void filePut( QString id_file );
    void fileGet( QString id_file );
    void fileDelete( QString id_file );
    void replyFinished();
    void error();

private Q_SLOTS:
    void saveFileList();
    void onError( QString err );
    void onReplyFinished();
    void addItem( QString id_file, uint64_t size );

private:
    QWebdav _dav;
    QWebdavDirParser _dir_parser;

    QList< QWebdavItem > _items;
    std::mutex _mutex;
};

}

#endif // WEBDAV_WRAPPER_H
