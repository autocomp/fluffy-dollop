#include "sincxmlloader.h"
#include <QDebug>
//#include <QHttp>
#include <QTime>
//#include <QApplication>

bool SincXmlLoader::loadXml(const QUrl &url, QByteArray &xml, int defaultPort, uint msecTimeout)
{
//    QString serverHost = url.host();
//    int serverPort = url.port(defaultPort);

//    QString serverPath = url.path();
//    qDebug() << "SincXmlLoader::loadXml, serverPath :" << serverHost << ", url :" << serverPath;

//    QHttp http;
//    http.setHost(serverHost, serverPort);
//    QHttpRequestHeader header("GET", serverPath);
//    header.setValue("User-Agent", "Mozilla");
//    header.setValue("Host", serverHost);
//    http.request(header);

//    QTime time;
//    time.start();

//    QHttp::State _state(QHttp::Unconnected);
//    qDebug() << "SincXmlLoader::loadXml,-----start-----";

//    while(time.elapsed() < msecTimeout)
//    {
//        if(_state != http.state())
//        {
//            _state = http.state();

//            switch(_state)
//            {
//            case QHttp::Unconnected : qDebug() << "SincXmlLoader::loadXml,----- Unconnected" << time.elapsed(); break;
//            case QHttp::HostLookup : qDebug() << "SincXmlLoader::loadXml,----- HostLookup" << time.elapsed(); break;
//            case QHttp::Connecting : qDebug() << "SincXmlLoader::loadXml,----- Connecting" << time.elapsed(); break;
//            case QHttp::Sending : qDebug() << "SincXmlLoader::loadXml,----- Sending" << time.elapsed(); break;
//            case QHttp::Reading : qDebug() << "SincXmlLoader::loadXml,----- Reading" << time.elapsed(); break;
//            case QHttp::Connected : qDebug() << "SincXmlLoader::loadXml,----- Connected" << time.elapsed(); break;
//            case QHttp::Closing : qDebug() << "SincXmlLoader::loadXml,----- Closing" << time.elapsed(); break;
//            }

//            if(_state == QHttp::Connected)
//            {
//                xml = http.readAll();
//                qDebug() << "SincXmlLoader::loadXml, LOADED, statusCode() :" << http.lastResponse().statusCode() << ", elapsed :" << time.elapsed() << " msec";;
//                return true;
//            }
//        }

//        qApp->processEvents();
//    }

//    qDebug() << "SincXmlLoader::loadXml,check server, time.elapsed() > " << msecTimeout << " msec !";
//    qDebug() << "SincXmlLoader::loadXml,check server, state:" << http.state() << ", statusCode:" << http.lastResponse().statusCode();

    return false;
}

