#include "webdav_queue.h"

#include <QTimer>

using namespace regionbiz;

WebDavQueue::WebDavQueue(QObject *parent):
    QObject( parent )
{}

void WebDavQueue::init(WebDavParams params)
{
    // init data
    _semaphore = new QSemaphore( params.queue_size );
    _wrapper.init( params );

    // connects
    connect( &_wrapper, SIGNAL( filePut( QString )),
             SLOT( onPut( QString )));
    connect( &_wrapper, SIGNAL( fileGet( QString )),
             SLOT( onGet( QString )));
    connect( &_wrapper, SIGNAL( fileDelete( QString )),
             SLOT( onDelete( QString )));
    connect( &_wrapper, SIGNAL( replyFinished() ),
             SLOT( onReplyFinished() ));
    connect( &_wrapper, SIGNAL( error() ),
             SLOT( onError() ));

    // for init auth and load files
    loadFilesList();

    // init auto update
    QTimer* timer = new QTimer( this );
    connect( timer, SIGNAL( timeout() ), SLOT( loadFilesList() ));
    timer->start( params.update_list_sec_delay * 1000 );
}

void WebDavQueue::addOperation( Operation oper )
{
    _operations.push_back( oper );
    process();
}

bool WebDavQueue::isItemPresent( QString id_file )
{
    return _wrapper.isItemPresent( id_file );
}

bool WebDavQueue::isFileSynced(QString id_file, uint64_t size)
{
    if( !isItemPresent( id_file ))
        return false;

    auto item = _wrapper.getItem( id_file );
    bool sync = ( !item.isDir() && size == item.size() );
    return sync;
}

void WebDavQueue::onPut(QString id)
{
    Q_EMIT filePut( id );
    _semaphore->release( 1 );
    process();
}

void WebDavQueue::onGet(QString id)
{
    Q_EMIT fileGet( id );
    _semaphore->release( 1 );
    process();
}

void WebDavQueue::onDelete(QString id)
{
    Q_EMIT fileDelete( id );
    _semaphore->release( 1 );
    process();
}

void WebDavQueue::onReplyFinished()
{
    _semaphore->release( 1 );
    process();
}

void WebDavQueue::onError()
{
    _operations.clear();
    loadFilesList();

    Q_EMIT error();
}

void WebDavQueue::loadFilesList()
{
    addOperation( { OT_CHECK_DIR, "", "" } );
}

void WebDavQueue::process()
{
    // if not have any operations
    if( _operations.empty() )
        return;

    // if blocked - return
    if( !_semaphore->tryAcquire( 1 ))
        return;

    // get first operation
    auto oper = _operations.front();
    _operations.pop_front();

    switch ( oper.type ) {
    case OT_CHECK_DIR:
        _wrapper.readAllItems();
        break;
    case OT_GET:
        _wrapper.getFile( oper.id, oper.file );
        break;
    case OT_PUT:
        _wrapper.putFile( oper.id, oper.file );
        break;
    case OT_DELETE:
        _wrapper.deleteFile( oper.id );
        break;
    }
}
