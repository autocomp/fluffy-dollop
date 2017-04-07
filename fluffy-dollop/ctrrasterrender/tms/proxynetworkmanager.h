#ifndef PROXY_NETWORK_MANAGER_H
#define PROXY_NETWORK_MANAGER_H

#include <QObject>
#include <QThread>
#include <QQueue>
#include <QNetworkAccessManager>

class ProxyNetworkManager : public QObject
{
    Q_OBJECT
public:
    explicit ProxyNetworkManager(QObject *parent = 0);
    ~ProxyNetworkManager();

public slots:
    void setRequest( QString url, int source_id );
    void abortRequest( QString url, int source_id );

signals:
    void finishRequest( QString, int, bool, QByteArray );

private slots:
    void onFinish( QNetworkReply* reply );

private:
    struct Request
    {
        QString url;
        int source_id;
    };

    void checkQueue();

    QThread* _thread;
    QNetworkAccessManager* _network_mngr;
    QQueue< Request > _request_queue;

    uint _current_request_count = 0;
    uint _max_request_count = 20;
};

#endif // PROXY_NETWORK_MANAGER_H
