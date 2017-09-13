#include "webdav_wrapper.h"

#include <iostream>
#include <QFileInfo>

#define OPEN_FILE( read_type ) \
    QFile* file = new QFile( file_path_local ); \
    bool open = file->open( QFile::read_type ); \
    if( !open ) \
    { \
        delete file; \
        return false; \
    }

#define CREATE_REPLY_2( type, file ) \
    auto reply = _dav.type( "/" + id_file, file ); \
    reply->setProperty( "id_file", id_file ); \
    reply->setProperty( "type", ""#type ); \
    if( ""#type == QString( "put" )) \
        reply->setProperty( "size", QFileInfo( *file ).size() ); \
    connect( reply, SIGNAL( finished() ), SLOT( onReplyFinished() ));

#define CREATE_REPLY( type ) \
    auto reply = _dav.type( "/" + id_file ); \
    reply->setProperty( "id_file", id_file ); \
    reply->setProperty( "type", ""#type ); \
    connect( reply, SIGNAL( finished() ), SLOT( onReplyFinished() ));

using namespace regionbiz;

void WebDavWrapper::init( WebDavParams params )
{
    _dav.setConnectionSettings( QWebdav::HTTPS, params.host,
                                params.path, params.user,
                                params.pass, params.port );

    connect(&_dir_parser, SIGNAL( finished() ), this, SLOT( saveFileList() ));
    connect(&_dir_parser, SIGNAL( errorChanged( QString )), this, SLOT( onError( QString )));
    connect(&_dav, SIGNAL( errorChanged( QString )), this, SLOT( onError( QString )));
}

bool WebDavWrapper::readAllItems()
{
    bool correct = _dir_parser.listDirectory( &_dav, "/" );
    return correct;
}

QList<QWebdavItem> WebDavWrapper::getItems()
{
    _mutex.lock();
    QList<QWebdavItem> list = _items;
    _mutex.unlock();

    return list;
}

bool WebDavWrapper::isItemPresent(QString id_file)
{
    bool has = false;

    _mutex.lock();
    {
        for( QWebdavItem& item: _items )
            if( item.name() == id_file )
            {
                has = true;
                break;
            }
    }
    _mutex.unlock();

    return has;
}

QWebdavItem WebDavWrapper::getItem(QString id_file)
{
    QWebdavItem item;

    _mutex.lock();
    {
        for( QWebdavItem& item_in: _items )
            if( item_in.name() == id_file )
            {
                item = item_in;
                break;
            }
    }
    _mutex.unlock();

    return item;
}

bool WebDavWrapper::putFile( QString id_file, QString file_path_local )
{
    OPEN_FILE( ReadOnly )
    CREATE_REPLY_2( put, file )

    return true;
}

bool WebDavWrapper::getFile( QString id_file, QString file_path_local )
{
    OPEN_FILE( WriteOnly )
    CREATE_REPLY_2( get, file )

    return true;
}

bool WebDavWrapper::deleteFile(QString id_file)
{
    CREATE_REPLY( remove )

    return true;
}

void WebDavWrapper::saveFileList()
{
    _mutex.lock();
    _items = _dir_parser.getList();
    _mutex.unlock();

    //for( QWebdavItem item: _items )
    //    qDebug() << item.name() << item.size();

    Q_EMIT replyFinished();
}

void WebDavWrapper::onError( QString err )
{
    using namespace std;
    cerr << "WebDAV error: "
         << err.toLocal8Bit().data() << endl;

    _mutex.lock();
    _items.clear();
    _mutex.unlock();


    Q_EMIT error();
}

void WebDavWrapper::onReplyFinished()
{
    QNetworkReply* reply = (QNetworkReply*) sender();
    QString id_file = reply->property( "id_file" ).toString();
    QString type = reply->property( "type" ).toString();

    if( "put" == type )
    {
        // add item to list on succesfull put
        uint64_t size = reply->property( "type" ).toULongLong();
        addItem( id_file, size );
        Q_EMIT filePut( id_file );
    }
    else if( "get" == type )
        Q_EMIT fileGet( id_file );
    else if( "remove" == type )
        Q_EMIT fileDelete( id_file );
    else
        Q_EMIT replyFinished();
}

void WebDavWrapper::addItem( QString id_file, uint64_t size )
{
    _mutex.lock();
    _items.push_back( QWebdavItem( id_file, id_file, "",
                                   false, QDateTime::currentDateTime(), size ));
    _mutex.unlock();
}
