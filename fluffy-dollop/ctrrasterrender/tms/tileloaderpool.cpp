#include "tileloaderpool.h"
//#include "tmsaltcontroller.h"
#include <QSettings>
#include <QDir>
#include <QDebug>

TileLoaderPool* TileLoaderPool::m_Instance = 0;

TileLoaderPool::TileLoaderPool()
    : _sourcesIdCounter(0)
    //, _loadersTimeoutTimer(this)
    , _worldSizeInTiles(2,1)
    , _timeoutOpenProviderSec(3)
    , _totalLoaders(16)
    , _debugMode(false)
{
    //connect(&_roundRobin, SIGNAL(signal_exceptionFromQueue(uint,uint)), this, SLOT(slot_exceptionFromQueue(uint,uint)));
    connect(&_roundRobin, SIGNAL(signal_exceptionFromQueue(QString,uint)), this, SLOT(slot_exceptionFromQueue(QString,uint)));
    connect(&_roundRobin, SIGNAL(signal_showLog()), this, SLOT(slot_showLog()));
    connect(&_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(slot_finished(QNetworkReply*)));
    //connect(&_loadersTimeoutTimer, SIGNAL(timeout()), this, SLOT(slot_checkLoadersTimeout()));

    init();
    //_loadersTimeoutTimer.start(1000);
}

QString TileLoaderPool::settingsPath()const
{
    return _settingsPath;
}

uint TileLoaderPool::totalLoaders()const
{
    return _totalLoaders; //_loaders.count();
}

void TileLoaderPool::setSettingsPath(const QString& settingsPath)
{
    _settingsPath = settingsPath;
    QSettings settings(_settingsPath, QSettings::NativeFormat);

    QVariant tmsMax2DQueueSize = settings.value("Pool_Max2dQueueSize");
    if(tmsMax2DQueueSize.isValid() == false)
        settings.setValue("Pool_Max2dQueueSize", _maxQueue2DSize);
    else
        _maxQueue2DSize = tmsMax2DQueueSize.toUInt();

    QVariant tmsMax3DQueueSize = settings.value("Pool_Max3dQueueSize");
    if(tmsMax3DQueueSize.isValid() == false)
        settings.setValue("Pool_Max3dQueueSize", _maxQueue3DSize);
    else
        _maxQueue3DSize = tmsMax3DQueueSize.toUInt();

//    QVariant tmsLoadersCount = settings.value("Pool_LoadersCount");
//    if(tmsLoadersCount.isValid() == false || tmsLoadersCount.toUInt() == 0)
//    {
//        tmsLoadersCount = 16;
//        settings.setValue("Pool_LoadersCount", QString("16"));
//    }
    QVariant tmsLoadersCount = settings.value("Pool_LoadersCount");
    if(tmsLoadersCount.isValid() == false || tmsLoadersCount.toUInt() == 0)
        settings.setValue("Pool_LoadersCount", _totalLoaders);
    else
        _totalLoaders = tmsLoadersCount.toUInt();

    QVariant tmsDefaultPort = settings.value("Pool_DefaultPort");
    if(tmsDefaultPort.isValid() == false)
    {
        tmsDefaultPort = 80;
        settings.setValue("Pool_DefaultPort", tmsDefaultPort);
    }

    QVariant timeoutOpenProviderSecVar = settings.value("Pool_TimeoutOpenProviderSec");
    if(timeoutOpenProviderSecVar.isValid())
    {
        _timeoutOpenProviderSec = timeoutOpenProviderSecVar.toUInt();
        if(_timeoutOpenProviderSec > 60)
        {
            _timeoutOpenProviderSec = 3;
            settings.setValue("Pool_TimeoutOpenProviderSec", _timeoutOpenProviderSec);
        }
    }
    else
        settings.setValue("Pool_TimeoutOpenProviderSec", _timeoutOpenProviderSec);

    QVariant tmsDebugMode = settings.value("Pool_DebugMode");
    if(tmsDebugMode.isValid() && tmsDebugMode.toBool())
        setDebugMode();
    else
        settings.setValue("Pool_DebugMode", false);

    /*
    foreach(TileLoader* tileLoader, _loaders)
    {
        tileLoader->abortLoading();
        delete tileLoader;
    }

    _loaders.clear();
    _queueLoaders.clear();
    _map_HttpTaskId_loadedTask.clear();

    for(uint i(0); i < tmsLoadersCount.toInt(); ++i)
    {
        TileLoader* tileLoader = new TileLoader(tmsDefaultPort.toInt());
        _loaders.append(tileLoader);
        _queueLoaders.enqueue(tileLoader);
        connect(tileLoader, SIGNAL(signal_requestFinished(int,QByteArray*)), this, SLOT(slot_requestFinished(int,QByteArray*)));
    }

    QVariant tmsDebugMode = settings.value("Pool_DebugMode");
    if(tmsDebugMode.isValid() && tmsDebugMode.toBool())
        TileLoaderPool::instance()->setDebugMode();
    else
        settings.setValue("Pool_DebugMode", false);
    */

//    _coverCutter.init(settingsPath);

//    std::string fileName;
//    std::string url("http://innovationg/mapproxy/tms/1.0.0/Bing/EPSG4326/%1/%2/%3.png");
//    std::string url("http://innovationg/mapproxy/tms/1.0.0/OSM_Planet/EPSG4326/%1/%2/%3.png");
//    _coverCutter.cut(59.55, 30, 59.98, 30.55, 3000, 3000, fileName, url, dpf::BGRA32, 1, 12);
}

void TileLoaderPool::init(uint totalLoaders, int defaultPort, uint maxQueue2DSize, uint maxQueue3DSize)
{
    _maxQueue2DSize = maxQueue2DSize;
    _maxQueue3DSize = maxQueue3DSize;
    _roundRobin.setMaxQueueSize(maxQueue2DSize, maxQueue3DSize);
    _totalLoaders = totalLoaders;

    for(auto it = _map_Url_Reply.begin(); it != _map_Url_Reply.end(); ++it)
    {
        Reply reply = it.value();
        reply.reply->abort();
        delete reply.reply;
    }
    _map_Url_Reply.clear();

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
    _debugMode = true;
    _roundRobin.setDebugMode(_totalLoaders); //_loaders.size());
}

bool TileLoaderPool::isDebugMode()
{
    return _debugMode;
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
    _timeoutOpenProviderSec = _timeoutOpenProviderSec_;
}

uint TileLoaderPool::timeoutOpenProviderSec()const
{
    return _timeoutOpenProviderSec;
}

uint TileLoaderPool::registerSource(const TmsBaseAdapter * const tmsAdapter, TileDataProvider::TmsTaskSender taskSender)
{
    uint sourceId(++_sourcesIdCounter);
    _roundRobin.registerSource(sourceId, tmsAdapter, taskSender);
    return sourceId;
}

bool TileLoaderPool::unregisterSource(uint sourceId)
{
    return _roundRobin.unregisterSource(sourceId);
}

TileLoaderPool::~TileLoaderPool()
{
    for(auto it = _map_Url_Reply.begin(); it != _map_Url_Reply.end(); ++it)
    {
        Reply reply = it.value();
        reply.reply->abort();
        delete reply.reply;
    }
    _map_Url_Reply.clear();

//    foreach(TileLoader* tileLoader, _loaders)
//    {
//        tileLoader->abortLoading();
//        delete tileLoader;
//    }
}

//void TileLoaderPool::slot_requestFinished(int insideTaskId, QByteArray * src)
//{
////    qDebug() << "TileLoaderPool, map_HttpTaskId_loadedTask.size() :" << _map_HttpTaskId_loadedTask.size();

//    TileLoader* tileLoader = (TileLoader*)(sender());

//    QMap<int, LoadedTask>::iterator it(_map_HttpTaskId_loadedTask.find(insideTaskId));
//    if(it != _map_HttpTaskId_loadedTask.end())
//    {
//        LoadedTask loadedTask(it.value());
//        _map_HttpTaskId_loadedTask.erase(it);
//        TmsBaseAdapter * adapter = tileLoader->adapter();
//        if(src)
//        {
//            emit signal_requestFinished(loadedTask.providerTaskId, tileLoader->source(), src, TileDataProvider::Loaded);
//        }
//        else
//        {
//            loadedTask.times += 1;
//            if(loadedTask.times < adapter->retryLimit())
//            {
//                // по уму надо сформировать задачу и отдать в карусель, а на выполнение взять из карусели очередную задачу согласно приоритету !!!
//                QString _url = tileLoader->getUrl();
//                uint _taskId = tileLoader->taskId();
//                uint _source = tileLoader->source();
//                int httpTaskId = tileLoader->setRequest(_source, _taskId, _url, adapter);
//                _map_HttpTaskId_loadedTask.insert(httpTaskId, loadedTask);
//                return;
//            }

//            emit signal_requestFinished(loadedTask.providerTaskId, tileLoader->source(), 0, TileDataProvider::NotFound);
//        }
//    }

////    qDebug() << "slot_requestFinished, queueOutsideTask.size :" << _queueOutsideTask.size();

//    // если есть невыполненные задачи - отдаем загрузчику первую из очереди невыполненных задач.
//    QString _url;
//    uint _taskId;
//    uint _source;
//    bool success = _roundRobin.getTask(_url, _taskId, _source);
//    if(success)
//    {
//        TmsBaseAdapter * tmsAdapter = _roundRobin.getAdapter(_source);
//        int httpTaskId = tileLoader->setRequest(_source, _taskId, _url, tmsAdapter);
//        _map_HttpTaskId_loadedTask.insert(httpTaskId, LoadedTask(_taskId));
//    }
//    else
//    {
//        // если очередь невыполненных задач пуста - укладываем загрузчика в очередь свободных загрузчиков.
//        _queueLoaders.enqueue(tileLoader);
//        _roundRobin.loadersInWork(_loaders.size() - _queueLoaders.size());
//    }
//}

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
    if(_map_Url_Reply.size() > _totalLoaders)
    {
        _roundRobin.addTask(url, sourceId);
        //qDebug() << "setRequest, queueOutsideTask.size :" << _queueOutsideTask.size();
    }
    else
    {
        QNetworkReply* networkReply = _manager.get(QNetworkRequest(url));
        Reply reply(networkReply, sourceId);
        _map_Url_Reply.insert(url, reply);

        _roundRobin.setLog(url);
        _roundRobin.loadersInWork(_map_Url_Reply.size());
    }
}
//int TileLoaderPool::setRequest(const QString& url, uint sourceId)
//{
//    RoundRobin::OutsideTask outsideTask(url);
//    _roundRobin.setLog(url);

//    if(_queueLoaders.isEmpty())
//    {
//        _roundRobin.addTask(outsideTask, sourceId);
////        qDebug() << "setRequest, queueOutsideTask.size :" << _queueOutsideTask.size();
//    }
//    else
//    {
//        TmsBaseAdapter * adapter = _roundRobin.getAdapter(sourceId);
//        if(adapter->hasFreeLoaders())
//        {
////            qDebug() << "getTask, loaders:" << it.value().loaders << ", limit:" << it.value().limit;

//            TileLoader* tileLoader = _queueLoaders.dequeue();
//            int insideTaskId = tileLoader->setRequest(sourceId, outsideTask.taskId(), url, adapter);
//            _map_HttpTaskId_loadedTask.insert(insideTaskId, LoadedTask(outsideTask.taskId()));

//            _roundRobin.loadersInWork(_loaders.size() - _queueLoaders.size());
//        }
//        else
//            _roundRobin.addTask(outsideTask, sourceId);
//    }

//    return outsideTask.taskId();
//}

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

    _roundRobin.loadersInWork(_map_Url_Reply.size());

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


void TileLoaderPool::abortTaskFromSource(QString url, uint sourceId)
{
    auto it = _map_Url_Reply.find(url);
    if(it != _map_Url_Reply.end())
    {
        Reply reply = it.value();
        reply.reply->abort();
        delete reply.reply;
        _map_Url_Reply.erase(it);

        // если есть невыполненные задачи - отдаем загрузчику первую из очереди невыполненных задач.
        QString _url;
        uint _source;
        bool success = _roundRobin.getTask(_url, _source);
        if(success)
        {
            QNetworkReply* networkReply = _manager.get(QNetworkRequest(_url));
            Reply reply(networkReply, sourceId);
            _map_Url_Reply.insert(_url, reply);
        }
        _roundRobin.loadersInWork(_map_Url_Reply.size());
    }
    else
    {
        // раз задачи нет в мапе - значит она еще не загружается и находится в очереди на загрузку.
        _roundRobin.excludeTask(url, sourceId);
    }
}
//void TileLoaderPool::abortTaskFromSource(uint tasksId, uint sourceId)
//{
//    for(QMap<int, LoadedTask>::iterator it(_map_HttpTaskId_loadedTask.begin()); it != _map_HttpTaskId_loadedTask.end(); ++it)
//        if(it.value().providerTaskId == tasksId) // так как она в мапе - значит она УЖЕ загружается
//        {
//            _map_HttpTaskId_loadedTask.erase(it);

//            foreach(TileLoader* tileLoader, _loaders)
//                if(tileLoader->outsideKey() == tasksId)
//                {
//                    tileLoader->abortLoading();

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
//            return;
//        }

//    // раз задачи нет в мапе - значит она еще не загружается и находится в очереди на загрузку.
//    _roundRobin.excludeTask(tasksId, sourceId);
//}

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
            qDebug() << "TileLoaderPool::slot_finished, Loaded, timeMSec:" << timeMSec << ", URL:" << URL << ", size:" << _map_Url_Reply.size() << ", totalLoaders:" << _totalLoaders;
            QByteArray byteArray(networkReply->readAll());
            emit signal_requestFinished(URL, reply.sourceId, &byteArray, TileDataProvider::Loaded);
        }
        else
        {
            qDebug() << "TileLoaderPool::slot_finished, NotFound, timeMSec:" << timeMSec << ", URL:" << URL << ", size:" << _map_Url_Reply.size() << ", totalLoaders:" << _totalLoaders;
            emit signal_requestFinished(URL, reply.sourceId, 0, TileDataProvider::NotFound);
        }
        _map_Url_Reply.erase(it);

        // если есть невыполненные задачи - отдаем загрузчику первую из очереди невыполненных задач.
        QString _url;
        uint _source;
        bool success = _roundRobin.getTask(_url, _source);
        if(success)
        {
            QNetworkReply* networkReply = _manager.get(QNetworkRequest(_url));
            Reply reply(networkReply, _source);
            _map_Url_Reply.insert(_url, reply);
        }
        _roundRobin.loadersInWork(_map_Url_Reply.size());
    }
//    else
//        qDebug() << "TileLoaderPool::slot_finished, no url in map ! URL:" << URL << ", totalLoaders:" << _totalLoaders;
}


///----------------------------------------------------------------------------------------------------------------------------------------------


//bool TileLoaderPool::getAltitude(double latMin, double lonMin, double latMax, double lonMax, uint pixWidth, uint pixHeight, std::string & file_path)
//{
//    file_path = std::string(_imageCache.getFileNameInCacheDir().toLocal8Bit());
//    return getAltitudeInFile(latMin, lonMin, latMax, lonMax, pixWidth, pixHeight, file_path);
//}


//// lat - широта, lon - долгота
//bool TileLoaderPool::getAltitudeInFile(double latMin, double lonMin, double latMax, double lonMax, uint pixWidth, uint pixHeight, const std::string & file_path)
//{
//    uint W(_tileSize.width()*_tilesCount.width());
//    uint H(_tileSize.height()*_tilesCount.height());
//    double depthW(W/360.);
//    double depthH(H/180.);

//    double topLeft_sceneX = depthW * (lonMin + 180);
//    double topLeft_sceneY = depthH * (90 - latMax);
//    double bottomRight_sceneX = depthW * (lonMax + 180);
//    double bottomRight_sceneY = depthH * (90 - latMin);
//    double currAreaWidth = bottomRight_sceneX - topLeft_sceneX;
//    double currAreaHeight = bottomRight_sceneY - topLeft_sceneY;

//    QRect areaInTiles;
//    int zoom(1);

//    qDebug() << "==========================";
//    forever
//    {
//        double POW(pow(2,zoom-1));
//        double sceneWidth(_tileSize.width() / POW);
//        double sceneHeight(_tileSize.height() / POW);
//        int MAX_WIDTH(_tilesCount.width()*POW);
//        int MAX_HEIGHT(_tilesCount.height()*POW);

//        int leftX = topLeft_sceneX / sceneWidth;
//        int rightX = bottomRight_sceneX / sceneWidth;
//        if(rightX > MAX_WIDTH) rightX = MAX_WIDTH;
//        int topY = topLeft_sceneY / sceneHeight;
//        int bottomY = bottomRight_sceneY / sceneHeight;
//        if(bottomY > MAX_HEIGHT) bottomY = MAX_HEIGHT;

//        areaInTiles = QRect(QPoint(leftX, topY), QPoint(rightX, bottomY));
//        qDebug() << zoom << "zoom---TmsAltController::getAltitudeInFile, zoom" << zoom << "areaInTiles" << areaInTiles << "currAreaWidth" << currAreaWidth << "currAreaHeight" << currAreaHeight << "pixSize" << QSize(pixWidth,pixHeight);

//        if(currAreaWidth >= pixWidth && currAreaHeight >= pixHeight)
//            break;

//        if(++zoom > _maxZlevel)
//            break;

//        currAreaWidth *= 2;
//        currAreaHeight *= 2;
//    }
//    qDebug() << "==========================";

//    if(zoom < _minZlevel)
//    {
//        qDebug() << "zoom < _minZlevel !!! zoom:" << zoom << "_minZlevel:" << _minZlevel;
//    }

//    QSettings settings(TileLoaderPool::instance()->settingsPath(), QSettings::NativeFormat);
//    QVariant timeoutSecVar = settings.value("AltitudeTmsController_Timeout_sec");
//    uint timeoutMSec(1000);
//    if(timeoutSecVar.isValid() && timeoutSecVar.toUInt() <= 30)
//        timeoutMSec = timeoutSecVar.toUInt() * 1000;

//    currAreaWidth = areaInTiles.width() * _tileSize.width();
//    currAreaHeight = areaInTiles.height() * _tileSize.height();

//    dpf::io::GeoImageObject object = dpf::io::create_geoimage(file_path, 100000 /*currAreaWidth*/, 100000 /*currAreaHeight*/, dpf::BGRA32); // BGRA32 // ABGR32 // Gray32U

//    LocalTmsAdapter localTmsAdapter(QUrl("http://innovationg/mapproxy/tms/1.0.0/OSM_Planet/EPSG4326/%1/%2/%3.png"));

//    bool success(true);
//    QDir dir;
//    int Z(zoom-1);
//    for(int X(areaInTiles.x()); X < areaInTiles.x() + areaInTiles.width(); ++X)
//        for(int Y(areaInTiles.y()); Y < areaInTiles.y() + areaInTiles.height(); ++Y)
//        {
//            const QString url(localTmsAdapter.query(X,Y,Z));
//            QHttp http;
//            http.setHost(localTmsAdapter.serverHost(), localTmsAdapter.serverPort());
//            QHttpRequestHeader header("GET", url);
//            header.setValue("User-Agent", "Mozilla");
//            header.setValue("Host", localTmsAdapter.serverHost());
//            http.request(header);

//            QTime time;
//            time.start();

//            int st = -1;
//            while(http.state() != QHttp::Connected && time.elapsed() < timeoutMSec)
//            {
//                qApp->processEvents();

//                if(st != http.state())
//                {
//                    qDebug() << "http.state() :" << http.state();
//                    st = http.state();
//                }
//            }

//            if(http.lastResponse().statusCode() == 200)
//            {
//                QString fileName = _imageCache.getFileNameInCacheDir("png");
//                QFile file(fileName);
//                if (file.open(QIODevice::WriteOnly))
//                {
//                    file.write(http.readAll());
//                    file.close();

//                    dpf::Image img_tile = dpf::io::read_image(fileName.toStdString(), dpf::BGRA32);
//                    int pixelOffsetX( (X - areaInTiles.x())*_tileSize.width() );
//                    int pixelOffsetY( (Y - areaInTiles.y())*_tileSize.height() );

//                    dpf::Rect roi_to_write(pixelOffsetX, pixelOffsetY, _tileSize.width(), _tileSize.height());

//                    if(!img_tile.isNull())
//                        object.write_image(img_tile, roi_to_write);
//                    else
//                        qDebug() << "image is null";

//                    qDebug() << "fileName" << fileName << "pixelOffsetX" << pixelOffsetX << "pixelOffsetY" << pixelOffsetY;
//                    qDebug() << "+++ checkServer, statusCode 200, " << url << "time.elapsed() :" << time.elapsed() << "http.state()" << http.state();
//                }
//                else
//                {
//                    qDebug() << "+++ checkServer, statusCode 200, file no opened !";
//                    success = false;
//                }
////                dir.remove(fileName);
//            }
//            else
//            {
//                qDebug() << "--- checkServer, statusCode :" << http.lastResponse().statusCode() << url;
//                success = false;
//            }

//            if(success == false)
//                break;
//        }

//    if(success == false)
//    {
//        dir.remove(QString::fromUtf8(file_path.c_str()));
//        return tms_alt_controller::NotFound;
//    }
//    else
//    {
//        double POW(pow(2,Z-1));
//        double topRightAreaX((areaInTiles.x() + areaInTiles.width())*_tileSize.width() / POW);
//        double topRightAreaY(areaInTiles.y()*_tileSize.height()  / POW);
//        double bottomLeftAreaX(areaInTiles.x()*_tileSize.width()  / POW);
//        double bottomLeftAreaY((areaInTiles.y() + areaInTiles.height())*_tileSize.height() / POW);

//        QPointF topRight = QPointF( (-(topRightAreaY*(180. / H))+90.), ((topRightAreaX*(360. / W))-180.));
//        QPointF bottomLeft = QPointF( (-(bottomLeftAreaY*(180. / H))+90.), ((bottomLeftAreaX*(360. / W))-180.));

//        dpf::geo::GCP_DATA geo_data;
//        geo_data.setImageSize(currAreaWidth, currAreaHeight);

//        geo_data.addPoint(dpf::geo::GCP(dpf::geo::GCP_pixel(0.0, currAreaHeight - 1),
//                                        dpf::geo::GCP_geo(bottomLeft.x(),
//                                                          bottomLeft.y(),
//                                                          0.0)));

//        geo_data.addPoint(dpf::geo::GCP(dpf::geo::GCP_pixel(currAreaWidth - 1,0.0),
//                                        dpf::geo::GCP_geo(topRight.x(),
//                                                          topRight.y(),
//                                                          0.0)));

//        geo_data.set_polynomial_params(1);
//        object.set_geodata(geo_data);
//        object.write_metadata();

//        qDebug() << "==========================, file :" << file_path.c_str();
//        return tms_alt_controller::Loaded;
//    }
//}











