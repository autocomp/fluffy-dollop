#ifndef TILELOADERPOOL_H
#define TILELOADERPOOL_H

#include <QObject>
#include <QQueue>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include "roundrobin.h"
#include <ctrcore/provider/tiledataprovider.h>
//#include "tileloader.h"

class TmsBaseAdapter;

using namespace data_system;

/*
 *  Класс организующий одновременную работу N-заказчиков тайлов (ТМС-мапперы через ТМС-провайдеров) и N-загрузчиков данных с сервера.
 *  Представлен в приложении в еденичном экземпляре.
 **/

class TileLoaderPool : public QObject
{
    Q_OBJECT

    struct LoadedTask
    {
        LoadedTask(uint _providerTaskId) : times(0) {providerTaskId = _providerTaskId;}
        uint times, providerTaskId;
    };

public:
    struct Reply
    {
        Reply(QNetworkReply* _reply, uint _sourceId = 0)
            : reply(_reply), sourceId(_sourceId), startTime(QDateTime::currentMSecsSinceEpoch()) {}
        QNetworkReply* reply;
        uint sourceId;
        qint64 startTime;
    };

    static TileLoaderPool* instance()
    {
        if(!m_Instance)
        {
            m_Instance = new TileLoaderPool;
        }
        return m_Instance;
    }

    ~TileLoaderPool();
    void init(uint totalLoaders = 16, int defaultPort = 80, uint maxQueue2DSize = 300, uint maxQueue3DSize = 300);
    void setSettingsPath(const QString& settingsPath);
    void setDebugMode();
    bool isDebugMode();
    void serWorldSizeInTiles(QSize worldSizeInTiles);
    QSize worldSizeInTiles()const;
    void setTimeoutOpenProviderSec(uint _timeoutOpenProviderSec_);
    uint timeoutOpenProviderSec()const;
    uint registerSource(const TmsBaseAdapter * const tmsAdapter, TileDataProvider::TmsTaskSender taskSender);
    bool unregisterSource(uint sourceId);
    void setRequest(const QString& url, uint sourceId);
    uint totalLoaders()const;
    QString settingsPath() const;
    void emit_reload();

    void abortAllTaskFromSource(uint sourceId);
    //void abortTaskFromSource(uint tasksId, uint sourceId);
    void abortTaskFromSource(QString url, uint sourceId);

private:
    TileLoaderPool();

    static TileLoaderPool* m_Instance;
    uint _sourcesIdCounter, _timeoutOpenProviderSec;
    //QList<TileLoader*> _loaders;

    RoundRobin _roundRobin;
    //QQueue<TileLoader*> _queueLoaders;

    //QMap<int, LoadedTask> _map_HttpTaskId_loadedTask;
    uint _maxQueue2DSize, _maxQueue3DSize, _totalLoaders;
    QString _settingsPath, _dirForTempFiles;
    //QTimer _loadersTimeoutTimer;
    QSize _worldSizeInTiles;
    QNetworkAccessManager _manager;
    QMap<QString, Reply> _map_Url_Reply;
    bool _debugMode;

private slots:
//    void slot_requestFinished(int taskId, QByteArray * src);
//    void slot_exceptionFromQueue(uint taskId, uint sourceId);
    void slot_exceptionFromQueue(QString url, uint sourceId);
    void slot_checkLoadersTimeout();
    void slot_showLog();
    void slot_finished(QNetworkReply*networkReply);

signals:
    //void signal_requestFinished(int taskId, uint sourceId, QByteArray * src, TileDataProvider::Result result);
    void signal_requestFinished(QString url, uint sourceId, QByteArray * src, TileDataProvider::Result result);
    void signal_reload();
};

#endif // TILELOADERPOOL_H









