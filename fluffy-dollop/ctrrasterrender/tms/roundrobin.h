#ifndef ROUNDROBIN_H
#define ROUNDROBIN_H

#include <QObject>
#include <QQueue>
#include <QMap>
#include <ctrcore/provider/tiledataprovider.h>

class TmsBaseAdapter;

using namespace data_system;

/* Класс предназначен для организации последовательного доступа к некоторому количеству очередей, хранящих задачи загрузки.
 *
 *
 **/

class DebugWidget;

class RoundRobin : public QObject
{
    Q_OBJECT

public:

//    class OutsideTask
//    {
//    public:
//        OutsideTask(const QString& url);
//        OutsideTask(const OutsideTask& other);
//        uint taskId()const {return _outsideTaskId;}
//        QString url()const {return _url;}

//    private:
//        static uint OutsideTaskId;
//        uint _outsideTaskId;
//        QString _url;
//    };

protected:
    struct Queue
    {
        Queue(const TmsBaseAdapter * const _tmsAdapter, uint _maxSize = 300);
        Queue();
        Queue(const Queue& other);
        TmsBaseAdapter * getTmsAdapter();

        uint maxSize;
        //QQueue<OutsideTask> queue;
        QQueue<QString> queue;

private:
        TmsBaseAdapter * tmsAdapter;
    };

public:

    RoundRobin(uint max2DQueueSize = 300, uint max3DQueueSize = 100);
    ~RoundRobin();

    bool registerSource(uint sourceId, const TmsBaseAdapter * const tmsAdapter, TileDataProvider::TmsTaskSender tmsTaskSender);
    bool unregisterSource(uint sourceId);
    bool clearQueueLoadingFromSourse(uint sourceId);
    //void addTask(const OutsideTask& outsideTask, uint sourceId);
    //bool getTask(QString& url, uint& taskId, uint& sourceId);
    void addTask(QString url, uint sourceId);
    bool getTask(QString& url, uint& sourceId);
    TmsBaseAdapter *getAdapter(uint sourceId);
    void setMaxQueueSize(uint max2DQueueSize, uint max3DQueueSize);
    bool isEmpty() {return _totalTasks == 0;}
    //bool excludeTask(uint tasksId, uint sourceId);
    bool excludeTask(QString url, uint sourceId);

    // методы для работы с виджетом отладки
    void setDebugMode(uint loadersCount);
    void loadersInWork(int count);
    void setLog(const QString& url);

private:
    QMap< uint, Queue > _tasks;
    QMap< uint, Queue >::iterator _queueMarker;
    uint _maxQueue2DSize, _maxQueue3DSize, _totalTasks;
    DebugWidget * _debugWidget;

signals:
    //void signal_exceptionFromQueue(uint taskId, uint sourceId);
    void signal_exceptionFromQueue(QString url, uint sourceId);
    void signal_showLog();
};

#endif









