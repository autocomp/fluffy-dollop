#include "proxynetworkmanager.h"
#include <QNetworkReply>

ProxyNetworkManager::ProxyNetworkManager(QObject *parent) : QObject(parent)
{
    _thread = new QThread();
    // WARNING thread for tile pool
    //moveToThread( _thread );
    _thread->start();

    _network_mngr = new QNetworkAccessManager( this );
    // WARNING thread for tile pool
    //_network_mngr->moveToThread( _thread );

    connect( _network_mngr, SIGNAL(finished(QNetworkReply*)), SLOT(onFinish(QNetworkReply*)) );
}

ProxyNetworkManager::~ProxyNetworkManager()
{
    _thread->quit();
    _thread->wait();
    _thread->deleteLater();
}

void ProxyNetworkManager::setRequest(QString url, int source_id)
{
    Request req = { url, source_id };
    _request_queue.prepend( req );

    checkQueue();
}

void ProxyNetworkManager::abortRequest(QString url, int source_id)
{
    for( auto it = _request_queue.begin(); it != _request_queue.end(); ++it )
    {
        Request it_req = *it;
        if( it_req.url == url && it_req.source_id == source_id )
        {
            _request_queue.erase( it );
            break;
        }
    }
}

void ProxyNetworkManager::onFinish(QNetworkReply *reply)
{
    --_current_request_count;

    QString url = reply->url().toString();
    int source_id = reply->property( "source_id" ).toInt();
    if( reply->error() == QNetworkReply::NoError )
    {
        QByteArray datagram = reply->readAll();
        emit finishRequest( url, source_id, true, datagram );
    }
    else
        emit finishRequest( url, source_id, false, QByteArray() );

    checkQueue();
}

void ProxyNetworkManager::checkQueue()
{
    if( _current_request_count < _max_request_count )
    {
        if( !_request_queue.isEmpty() )
        {
            Request req = _request_queue.dequeue();
            QNetworkRequest request(QUrl( req.url ));
            request.setRawHeader("User-Agent", "Mozilla/5.0 (PC; U; Intel; Linux; en) AppleWebKit/420+ (KHTML, like Gecko)");
            QNetworkReply* reply = _network_mngr->get(request);
            reply->setProperty("source_id", req.source_id);

            ++_current_request_count;
        }
    }
}

