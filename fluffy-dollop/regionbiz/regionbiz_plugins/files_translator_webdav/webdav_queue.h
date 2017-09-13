#ifndef WEBDAV_QUEUE_H
#define WEBDAV_QUEUE_H

#include <QSemaphore>

#include "webdav_wrapper.h"

namespace regionbiz {

enum OperationType
{
    OT_CHECK_DIR,
    OT_PUT,
    OT_GET,
    OT_DELETE
};

struct Operation
{
    OperationType type = OT_CHECK_DIR;
    QString id = "";
    QString file = "";

    Operation( OperationType tp,
               QString id_str,
               QString file_str ):
        type( tp ),
        id( id_str ),
        file( file_str )
    {}
};

class WebDavQueue: public QObject
{
    Q_OBJECT
public:
    WebDavQueue( QObject* parent );

    void init( WebDavParams params );
    void addOperation(Operation oper);
    bool isItemPresent( QString id_file );
    bool isFileSynced(QString id_file, uint64_t size );

Q_SIGNALS:
    void filePut( QString id_file );
    void fileGet( QString id_file );
    void fileDelete( QString id_file );

    void error();

private Q_SLOTS:
    void onPut( QString id );
    void onGet( QString id );
    void onDelete( QString id );
    void onReplyFinished();
    void onError();

    void loadFilesList();

private:
    void process();

    WebDavWrapper _wrapper;
    QSemaphore* _semaphore;
    std::list< Operation > _operations;
};

}

#endif // WEBDAV_QUEUE_H
