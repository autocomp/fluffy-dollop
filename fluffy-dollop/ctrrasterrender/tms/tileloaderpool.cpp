#include "tileloaderpool.h"
//#include "tmsaltcontroller.h"
#include <QSettings>
#include <QDir>

TileLoaderPool* TileLoaderPool::m_Instance = 0;

TileLoaderPool::TileLoaderPool()
    : _sourcesIdCounter(0)
    //, _loadersTimeoutTimer(this)
    , _worldSizeInTiles(2,1)
    //, _timeoutOpenProviderSec(3)
    //, _totalLoaders(16)
    //, _debugMode(false)
{
    //connect(&_roundRobin, SIGNAL(signal_exceptionFromQueue(uint,uint)), this, SLOT(slot_exceptionFromQueue(uint,uint)));
    //connect(&_roundRobin, SIGNAL(signal_exceptionFromQueue(QString,uint)), this, SLOT(slot_exceptionFromQueue(QString,uint)));
    //connect(&_roundRobin, SIGNAL(signal_showLog()), this, SLOT(slot_showLog()));
    //connect(&_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slot_finished(QNetworkReply*)));
    //connect(&_loadersTimeoutTimer, SIGNAL(timeout()), this, SLOT(slot_checkLoadersTimeout()));

    _proxyNetworkManager = new ProxyNetworkManager();
    connect(_proxyNetworkManager, SIGNAL(finishRequest(QString,int,bool,QByteArray)),
            this, SLOT(finishRequest(QString,int,bool,QByteArray)), Qt::QueuedConnection);
    connect(this, SIGNAL(signal_setRequest(QString,int)),
            _proxyNetworkManager, SLOT(setRequest(QString,int)), Qt::QueuedConnection);
    connect(this, SIGNAL(signal_abortRequest(QString,int)),
            _proxyNetworkManager, SLOT(abortRequest(QString,int)), Qt::QueuedConnection);

    //init();
    //_loadersTimeoutTimer.start(1000);
}

QString TileLoaderPool::settingsPath()const
{
    return _settingsPath;
}

uint TileLoaderPool::totalLoaders()const
{
    return 4; //_totalLoaders; //_loaders.count();
}

void TileLoaderPool::setSettingsPath(const QString& settingsPath)
{
//    _settingsPath = settingsPath;
//    QSettings settings(_settingsPath, QSettings::NativeFormat);

//    QVariant tmsMax2DQueueSize = settings.value("Pool_Max2dQueueSize");
//    if(tmsMax2DQueueSize.isValid() == false)
//        settings.setValue("Pool_Max2dQueueSize", _maxQueue2DSize);
//    else
//        _maxQueue2DSize = tmsMax2DQueueSize.toUInt();

//    QVariant tmsMax3DQueueSize = settings.value("Pool_Max3dQueueSize");
//    if(tmsMax3DQueueSize.isValid() == false)
//        settings.setValue("Pool_Max3dQueueSize", _maxQueue3DSize);
//    else
//        _maxQueue3DSize = tmsMax3DQueueSize.toUInt();

//    QVariant tmsLoadersCount = settings.value("Pool_LoadersCount");
//    if(tmsLoadersCount.isValid() == false || tmsLoadersCount.toUInt() == 0)
//    {
//        tmsLoadersCount = 16;
//        settings.setValue("Pool_LoadersCount", QString("16"));
//    }
//    QVariant tmsLoadersCount = settings.value("Pool_LoadersCount");
//    if(tmsLoadersCount.isValid() == false || tmsLoadersCount.toUInt() == 0)
//        settings.setValue("Pool_LoadersCount", _totalLoaders);
//    else
//        _totalLoaders = tmsLoadersCount.toUInt();

//    QVariant tmsDefaultPort = settings.value("Pool_DefaultPort");
//    if(tmsDefaultPort.isValid() == false)
//    {
//        tmsDefaultPort = 80;
//        settings.setValue("Pool_DefaultPort", tmsDefaultPort);
//    }

//    QVariant timeoutOpenProviderSecVar = settings.value("Pool_TimeoutOpenProviderSec");
//    if(timeoutOpenProviderSecVar.isValid())
//    {
//        _timeoutOpenProviderSec = timeoutOpenProviderSecVar.toUInt();
//        if(_timeoutOpenProviderSec > 60)
//        {
//            _timeoutOpenProviderSec = 3;
//            settings.setValue("Pool_TimeoutOpenProviderSec", _timeoutOpenProviderSec);
//        }
//    }
//    else
//        settings.setValue("Pool_TimeoutOpenProviderSec", _timeoutOpenProviderSec);

//    QVariant tmsDebugMode = settings.value("Pool_DebugMode");
//    if(tmsDebugMode.isValid() && tmsDebugMode.toBool())
//        setDebugMode();
//    else
//        settings.setValue("Pool_DebugMode", false);
}

void TileLoaderPool::init(uint totalLoaders, int defaultPort, uint maxQueue2DSize, uint maxQueue3DSize)
{
//    _maxQueue2DSize = maxQueue2DSize;
//    _maxQueue3DSize = maxQueue3DSize;
//    _roundRobin.setMaxQueueSize(3000,3000); // maxQueue2DSize, maxQueue3DSize);
//    _totalLoaders = totalLoaders;

//    for(auto it = _map_Url_Reply.begin(); it != _map_Url_Reply.end(); ++it)
//    {
//        Reply reply = it.value();
//        reply.reply->abort();
//        delete reply.reply;
//    }
//    _map_Url_Reply.clear();

    /*
    foreach(TileLoader* tileLoader, _loaders)
    {
        tileLoader->abortLoading();
        delete tileLoader;
    }

    _loaders.clear();
    _queueLoaders.clear();
    _map_HttpTaskId_loadedTask.clear();

    for(uint i(0); i<totalLoaders; ++i)
    {
        TileLoader* tileLoader = new TileLoader(defaultPort);
        _loaders.append(tileLoader);
        _queueLoaders.enqueue(tileLoader);
        connect(tileLoader, SIGNAL(signal_requestFinished(int,QByteArray*)), this, SLOT(slot_requestFinished(int,QByteArray*)));
    }
    */
}

void TileLoaderPool::setDebugMode()
{
//    _debugMode = true;
//    _roundRobin.setDebugMode(_totalLoaders); //_loaders.size());
}

bool TileLoaderPool::isDebugMode()
{
    return false;//_debugMode;
}

void TileLoaderPool::serWorldSizeInTiles(QSize worldSizeInTiles)
{
    _worldSizeInTiles = worldSizeInTiles;
}

QSize TileLoaderPool::worldSizeInTiles()const
{
    return _worldSizeInTiles;
}

void TileLoaderPool::setTimeoutOpenProviderSec(uint _timeoutOpenProviderSec_)
{
    //_timeoutOpenProviderSec = _timeoutOpenProviderSec_;
}

uint TileLoaderPool::timeoutOpenProviderSec()const
{
    return 3; // _timeoutOpenProviderSec;
}

uint TileLoaderPool::registerSource(const TmsBaseAdapter * const tmsAdapter, TileDataProvider::TmsTaskSender taskSender)
{
    uint sourceId(++_sourcesIdCounter);
    //_roundRobin.registerSource(sourceId, tmsAdapter, taskSender);
    return sourceId;
}

bool TileLoaderPool::unregisterSource(uint sourceId)
{
    return true; //return _roundRobin.unregisterSource(sourceId);
}

TileLoaderPool::~TileLoaderPool()
{
//    for(auto it = _map_Url_Reply.begin(); it != _map_Url_Reply.end(); ++it)
//    {
//        Reply reply = it.value();
//        reply.reply->abort();
//        delete reply.reply;
//    }
//    _map_Url_Reply.clear();

//    foreach(TileLoader* tileLoader, _loaders)
//    {
//        tileLoader->abortLoading();
//        delete tileLoader;
//    }
}

void TileLoaderPool::slot_exceptionFromQueue(QString url, uint sourceId)
{
    emit signal_requestFinished(url, sourceId, 0, TileDataProvider::Aborted);
}

//void TileLoaderPool::slot_exceptionFromQueue(uint insideTaskId, uint sourceId)
//{
//    QMap<int, LoadedTask>::iterator it(_map_HttpTaskId_loadedTask.find(insideTaskId));
//    if(it != _map_HttpTaskId_loadedTask.end())
//        _map_HttpTaskId_loadedTask.erase(it);

//    emit signal_requestFinished(insideTaskId, sourceId, 0, TileDataProvider::Aborted);
//}


void TileLoaderPool::setRequest(const QString& url, uint sourceId)
{
    emit signal_setRequest(url, sourceId);

//    if(_map_Url_Reply.size() > _totalLoaders)
//    {
//        _roundRobin.addTask(url, sourceId);
//        //qDebug() << "setRequest, queueOutsideTask.size :" << _queueOutsideTask.size();
//    }
//    else
//    {
//        QNetworkRequest request(url);
//        request.setRawHeader("User-Agent", "Mozilla/5.0 (PC; U; Intel; Linux; en) AppleWebKit/420+ (KHTML, like Gecko)");
//        QNetworkReply* networkReply = _manager.get(request);
//        Reply reply(networkReply, sourceId);
//        _map_Url_Reply.insert(url, reply);

//        _roundRobin.setLog(url);
//        _roundRobin.loadersInWork(_map_Url_Reply.size());
//    }
}

void TileLoaderPool::abortTaskFromSource(QString url, uint sourceId)
{
    emit signal_abortRequest(url, sourceId);

//    auto it = _map_Url_Reply.find(url);
//    if(it != _map_Url_Reply.end())
//    {
//        Reply reply = it.value();
//        reply.reply->abort();
//        delete reply.reply;
//        _map_Url_Reply.erase(it);

//        // если есть невыполненные задачи - отдаем загрузчику первую из очереди невыполненных задач.
//        QString _url;
//        uint _source;
//        bool success = _roundRobin.getTask(_url, _source);
//        if(success)
//        {
//            QNetworkRequest request(_url);
//            request.setRawHeader("User-Agent", "Mozilla/5.0 (PC; U; Intel; Linux; en) AppleWebKit/420+ (KHTML, like Gecko)");
//            QNetworkReply* networkReply = _manager.get(request);
//            Reply reply(networkReply, sourceId);
//            _map_Url_Reply.insert(_url, reply);
//        }
//        _roundRobin.loadersInWork(_map_Url_Reply.size());
//    }
//    else
//    {
//        // раз задачи нет в мапе - значит она еще не загружается и находится в очереди на загрузку.
//        _roundRobin.excludeTask(url, sourceId);
//    }
}

/*
void TileLoaderPool::slot_finished(QNetworkReply* networkReply)
{
    const QString URL(networkReply->request().url().toString());
    auto it = _map_Url_Reply.find(URL);
    if(it != _map_Url_Reply.end())
    {
        Reply reply = it.value();
        qint64 timeMSec = QDateTime::currentMSecsSinceEpoch() - reply.startTime;
        QNetworkReply::NetworkError err = networkReply->error();
        if(networkReply->error() == QNetworkReply::NoError)
        {
//            qDebug() << "TileLoaderPool::slot_finished, Loaded, timeMSec:" << timeMSec << ", URL:" << URL << ", size:" << _map_Url_Reply.size() << ", totalLoaders:" << _totalLoaders;
            QByteArray byteArray(networkReply->readAll());
            emit signal_requestFinished(URL, reply.sourceId, &byteArray, TileDataProvider::Loaded);
        }
        else
        {
//            qDebug() << "TileLoaderPool::slot_finished, NotFound, timeMSec:" << timeMSec << ", URL:" << URL << ", size:" << _map_Url_Reply.size() << ", err:" << err;
            emit signal_requestFinished(URL, reply.sourceId, 0, TileDataProvider::NotFound);
        }
        _map_Url_Reply.erase(it);

        // если есть невыполненные задачи - отдаем загрузчику первую из очереди невыполненных задач.
        QString _url;
        uint _source;
        bool success = _roundRobin.getTask(_url, _source);
        if(success)
        {
            QNetworkRequest request(_url);
            request.setRawHeader("User-Agent", "Mozilla/5.0 (PC; U; Intel; Linux; en) AppleWebKit/420+ (KHTML, like Gecko)");
            QNetworkReply* networkReply = _manager.get(request);
            Reply reply(networkReply, _source);
            _map_Url_Reply.insert(_url, reply);
        }
        _roundRobin.loadersInWork(_map_Url_Reply.size());
    }
//    else
    //        qDebug() << "TileLoaderPool::slot_finished, no url in map ! URL:" << URL << ", totalLoaders:" << _totalLoaders;
}
*/

void TileLoaderPool::finishRequest(QString url, int sourceId, bool res, QByteArray src)
{
    if(res)
        emit signal_requestFinished(url, sourceId, &src, TileDataProvider::Loaded);
    else
        emit signal_requestFinished(url, sourceId, 0, TileDataProvider::NotFound);
}

void TileLoaderPool::emit_reload()
{
    emit signal_reload();
}

void TileLoaderPool::slot_showLog()
{
//    foreach(TileLoader* tileLoader, _loaders)
//        if(tileLoader->inWork())
//            _roundRobin.setLog("Url in loader :" + tileLoader->getUrl());
}

//! ПАДАЕТ, нужно раскомментировать и отследить !!!
void TileLoaderPool::abortAllTaskFromSource(uint sourceId)
{
    // найти очередь заказчика и очистить
//    _roundRobin.clearQueueLoadingFromSourse(sourceId);

//    QStringList urlForDeleting;
//    for(auto it = _map_Url_Reply.begin(); it != _map_Url_Reply.end(); ++it)
//    {
//        Reply reply = it.value();
//        if(reply.sourceId == sourceId)
//        {
//            urlForDeleting.append(it.key());
//            reply.reply->abort();
//            delete reply.reply;
//        }
//    }
//    foreach(QString url, urlForDeleting)
//        _map_Url_Reply.remove(url);

//    _roundRobin.loadersInWork(_map_Url_Reply.size());

///------------------------------------------------------------------------

//    // найти всех загрузчиков работающих с данным заказчиком и остановить
//    foreach(TileLoader* tileLoader, _loaders)
//    {
//        if(tileLoader->source() == sourceId && tileLoader->taskId() != -1)
//        {
////            qDebug() << "===> source tileLoader, taskId :" << tileLoader->taskId();

//            int taskId = tileLoader->taskId();
//            tileLoader->abortLoading();
//            QMap<int, LoadedTask>::iterator it(_map_HttpTaskId_loadedTask.find(taskId));
//            if(it != _map_HttpTaskId_loadedTask.end())
//                _map_HttpTaskId_loadedTask.erase(it);

//            // если есть невыполненные задачи - отдаем загрузчику первую из очереди невыполненных задач.
//            QString _url;
//            uint _taskId;
//            uint _source;
//            bool success = _roundRobin.getTask(_url, _taskId, _source);
//            if(success)
//            {
//                TmsBaseAdapter * tmsAdapter = _roundRobin.getAdapter(_source);
//                int httpTaskId = tileLoader->setRequest(_source, _taskId, _url, tmsAdapter);
//                _map_HttpTaskId_loadedTask.insert(httpTaskId, LoadedTask(_taskId));
//            }
//            else
//            {
//                // если очередь невыполненных задач пуста - укладываем загрузчика в очередь свободных загрузчиков.
//                _queueLoaders.enqueue(tileLoader);
//                _roundRobin.loadersInWork(_loaders.size() - _queueLoaders.size());
//            }
//        }
//    }
}

void TileLoaderPool::slot_checkLoadersTimeout()
{
//    if(_queueLoaders.size() != _loaders.size())
//    {
////        qDebug() << "TileLoaderPool::slot_checkLoadersTimeout()";
//        foreach(TileLoader* tileLoader, _loaders)
//            if(tileLoader->inWork())
//                if(tileLoader->stopByTimeout())
//                {
////                    qDebug() << "need stop :" << tileLoader->getUrl();

//                    QMap<int, LoadedTask>::iterator it(_map_HttpTaskId_loadedTask.find(tileLoader->taskId()));
//                    if(it != _map_HttpTaskId_loadedTask.end())
//                    {
//                        LoadedTask loadedTask(it.value());
//                        _map_HttpTaskId_loadedTask.erase(it);

//                        TmsBaseAdapter * adapter = tileLoader->adapter();
//                        uint sourceId = tileLoader->source();
//                        uint outsideKey = tileLoader->outsideKey();
//                        QString url = tileLoader->getUrl();
//                        tileLoader->abortLoading();

//                        loadedTask.times += 1;
//                        if(loadedTask.times < adapter->retryLimit())
//                        {
//                            // по уму надо сформировать задачу и отдать в карусель, а на выполнение взять из карусели очередную задачу согласно приоритету !!!
//                            int httpTaskId = tileLoader->setRequest(sourceId, outsideKey, url, adapter);
//                            _map_HttpTaskId_loadedTask.insert(httpTaskId, loadedTask);
//                            continue;
//                        }

//                        emit signal_requestFinished(loadedTask.providerTaskId, sourceId, 0, TileDataProvider::NotFound);
//                    }
//                    else
//                    {
//                        qDebug() << "TileLoaderPool, error 01 !!!";
//                        return;
//                    }

//                    // если есть невыполненные задачи - отдаем загрузчику первую из очереди невыполненных задач.
//                    QString _url;
//                    uint _taskId;
//                    uint _source;
//                    bool success = _roundRobin.getTask(_url, _taskId, _source);
//                    if(success)
//                    {
//                        TmsBaseAdapter * tmsAdapter = _roundRobin.getAdapter(_source);
//                        int httpTaskId = tileLoader->setRequest(_source, _taskId, _url, tmsAdapter);
//                        _map_HttpTaskId_loadedTask.insert(httpTaskId, LoadedTask(_taskId));
//                    }
//                    else
//                    {
//                        // если очередь невыполненных задач пуста - укладываем загрузчика в очередь свободных загрузчиков.
//                        _queueLoaders.enqueue(tileLoader);
//                        _roundRobin.loadersInWork(_loaders.size() - _queueLoaders.size());
//                    }
//                }
//    }
}









