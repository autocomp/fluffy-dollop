#include <QDebug>
#include "roundrobin.h"
#include "debugwidget.h"
#include "tmsbaseadapter.h"

//uint RoundRobin::OutsideTask::OutsideTaskId = 1000000;

//RoundRobin::OutsideTask::OutsideTask(const QString& url)
//    : _outsideTaskId(++OutsideTaskId)
//    , _url(url)
//{
//}

//RoundRobin::OutsideTask::OutsideTask(const OutsideTask& other)
//    : _outsideTaskId(other._outsideTaskId)
//    , _url(other._url)
//{
//}

RoundRobin::Queue::Queue(const TmsBaseAdapter * const _tmsAdapter, uint _maxSize)
    : tmsAdapter(const_cast<TmsBaseAdapter*>(_tmsAdapter)), maxSize(_maxSize)
{}

RoundRobin::Queue::Queue()
    : tmsAdapter(0), maxSize(300)
{}

RoundRobin::Queue::Queue(const Queue& other)
    : maxSize(other.maxSize), queue(other.queue), tmsAdapter(other.tmsAdapter)
{}

TmsBaseAdapter *RoundRobin::Queue::getTmsAdapter()
{
    return tmsAdapter;
}

///////////////////////////////////////////////////////////////////////////////////////

RoundRobin::RoundRobin(uint max2DQueueSize, uint max3DQueueSize)
    : _queueMarker(_tasks.begin())
    , _maxQueue2DSize(max2DQueueSize)
    , _maxQueue3DSize(max3DQueueSize)
    , _totalTasks(0)
    , _debugWidget(0)
{
}

void RoundRobin::setMaxQueueSize(uint max2DQueueSize, uint max3DQueueSize)
{
    _maxQueue2DSize = max2DQueueSize;
    _maxQueue3DSize = max3DQueueSize;
    // в идеале необходимо пересчитать все очереди, но в нашем случае этот метод вызывается сразу после создания объекта.
}

RoundRobin::~RoundRobin()
{

}

bool RoundRobin::registerSource(uint sourceId, const TmsBaseAdapter * const tmsAdapter, TileDataProvider::TmsTaskSender tmsTaskSender)
{
    QMap< uint, Queue >::iterator it(_tasks.find(sourceId));

    if(it != _tasks.end())
        return false;
    else
    {
        uint maxQueue;
        switch (tmsTaskSender) {
        case TileDataProvider::TmsMapper2D : maxQueue = _maxQueue2DSize; break;
        case TileDataProvider::TmsMapper3D : maxQueue = _maxQueue3DSize; break;
        case TileDataProvider::AltitudeController : maxQueue = 0; break;
        }
        Queue sourceQueue(tmsAdapter, maxQueue);
        _tasks.insert(sourceId, sourceQueue);
        _queueMarker = _tasks.begin();

        if(_debugWidget)
            _debugWidget->registerSource(sourceId, tmsTaskSender);

        return true;
    }
}

bool RoundRobin::unregisterSource(uint sourceId)
{
    QMap< uint, Queue >::iterator it(_tasks.find(sourceId));

    if(it != _tasks.end())
    {
        _tasks.erase(it);
        _queueMarker = _tasks.begin();

        if(_debugWidget)
            _debugWidget->unregisterSource(sourceId);

        return true;
    }
    else
        return false;
}

TmsBaseAdapter * RoundRobin::getAdapter(uint sourceId)
{
    QMap< uint, Queue >::iterator it(_tasks.find(sourceId));
    if(it != _tasks.end())
        return it.value().getTmsAdapter();
    else
        return 0;
}

//bool RoundRobin::getTask(QString& url, uint& taskId, uint &sourceId)
//{
//    if(_totalTasks == 0)
//        return false;

//    TmsBaseAdapter * firstAdapter(0);
//    forever
//    {
//        if(_tasks.isEmpty())
//        {
//            return false;
//        }

//        if( (++_queueMarker) == _tasks.end())
//            _queueMarker = _tasks.begin();

//        TmsBaseAdapter * tmsAdapter = _queueMarker.value().getTmsAdapter();
//        if(firstAdapter == tmsAdapter)// сделали круг !
//        {
//            return false;
//        }

//        if(firstAdapter == 0)
//            firstAdapter = tmsAdapter;

//        //! FIXME - при удалении во время загрузки (например сирии) происходит обращение к не валидному tmsAdapter.
//        if(tmsAdapter->hasFreeLoaders() == false)
//        {
//            continue;
//        }

//        // сюда мы попадем если текущий адаптер позволяет запихнуть задачу в загрузчик
//        QQueue<OutsideTask>& queue(_queueMarker.value().queue);
//        if(queue.isEmpty() == false)
//        {
//            OutsideTask outsideTask(queue.dequeue());
//            url = outsideTask.url();
//            taskId = outsideTask.taskId();
//            sourceId = _queueMarker.key();
//            --_totalTasks;

//            if(_debugWidget)
//                _debugWidget->setQueueSize(sourceId, queue.size());

//            return true;
//        }
//    }
//}
bool RoundRobin::getTask(QString& url, uint &sourceId)
{
    if(_totalTasks == 0)
        return false;

    TmsBaseAdapter * firstAdapter(0);
    forever
    {
        if( (++_queueMarker) == _tasks.end())
            _queueMarker = _tasks.begin();

        TmsBaseAdapter * tmsAdapter = _queueMarker.value().getTmsAdapter();
        if(firstAdapter == tmsAdapter)// сделали круг !
        {
            return false;
        }

        if(firstAdapter == 0)
            firstAdapter = tmsAdapter;

        if(tmsAdapter->hasFreeLoaders() == false)
        {
            continue;
        }

        // сюда мы попадем если текущий адаптер позволяет запихнуть задачу в загрузчик
        QQueue<QString>& queue(_queueMarker.value().queue);
        if(queue.isEmpty() == false)
        {
            url = queue.dequeue();
            sourceId = _queueMarker.key();
            --_totalTasks;

            if(_debugWidget)
                _debugWidget->setQueueSize(sourceId, queue.size());

            return true;
        }
    }
}

//void RoundRobin::addTask(const OutsideTask &outsideTask, uint sourceId)
//{
//    uint localTasks(0);

//    QMap< uint, Queue >::iterator it(_tasks.find(sourceId));
//    if(it != _tasks.end())
//    {
//        it.value().queue.prepend(outsideTask);
//        if(it.value().maxSize != 0 && it.value().queue.size() > it.value().maxSize)
//        {
//            OutsideTask lastOutsideTask = it.value().queue.takeLast();
//            emit signal_exceptionFromQueue(lastOutsideTask.taskId(), it.key());
//        }
//        else
//            ++_totalTasks;

//        localTasks = it.value().queue.size();

//        if(_debugWidget)
//            _debugWidget->setQueueSize(sourceId, localTasks);
//    }
//    //qDebug() << "+++ source :" << source << ", localTasks :" << localTasks << ", _totalTasks :" << _totalTasks;
//}
void RoundRobin::addTask(QString url, uint sourceId)
{
    uint localTasks(0);

    QMap< uint, Queue >::iterator it(_tasks.find(sourceId));
    if(it != _tasks.end())
    {
        it.value().queue.prepend(url);
        if(it.value().maxSize != 0 && it.value().queue.size() > it.value().maxSize)
        {
            QString exceptedUrl = it.value().queue.takeLast();
            emit signal_exceptionFromQueue(exceptedUrl, sourceId);
        }
        else
            ++_totalTasks;

        localTasks = it.value().queue.size();

        if(_debugWidget)
            _debugWidget->setQueueSize(sourceId, localTasks);
    }
    //qDebug() << "+++ source :" << source << ", localTasks :" << localTasks << ", _totalTasks :" << _totalTasks;
}


bool RoundRobin::clearQueueLoadingFromSourse(uint sourceId)
{
//    qDebug() << "clearQueueLoadingFromSourse";
    if(sourceId == 0)
        return false;

    QMap< uint, Queue >::iterator it(_tasks.find(sourceId));
    if(it != _tasks.end())
    {
        it.value().queue.clear();

        _totalTasks = 0;
        for(QMap< uint, Queue >::iterator it(_tasks.begin()); it != _tasks.end(); ++it)
            _totalTasks += it.value().queue.size();

        if(_debugWidget)
            _debugWidget->setQueueSize(sourceId, 0);
//        qDebug() << "source :" << source << ", _totalTasks :" << _totalTasks;

        return true;
    }
    else
        return false;
}

void RoundRobin::setDebugMode(uint loadersCount)
{
    if(_debugWidget == 0)
    {
        _debugWidget = new DebugWidget(loadersCount, _maxQueue2DSize, _maxQueue3DSize);
        connect(_debugWidget, SIGNAL(signalShowLog()), this, SIGNAL(signal_showLog()));
        _debugWidget->show();
    }
}

void RoundRobin::loadersInWork(int count)
{
    if(_debugWidget)
        _debugWidget->setLoadersInWork(count);
}

void RoundRobin::setLog(const QString& url)
{
    if(_debugWidget)
        _debugWidget->setLog(url);
}

//bool RoundRobin::excludeTask(uint tasksId, uint sourceId)
//{
//    QMap< uint, Queue >::iterator it(_tasks.find(sourceId));
//    if(it != _tasks.end())
//    {
//        QQueue<OutsideTask>& queue(_queueMarker.value().queue);
//        for(QQueue<OutsideTask>::iterator it(queue.begin()); it != queue.end(); ++it)
//        {
//            OutsideTask outsideTask(*it);
//            if(outsideTask.taskId() == tasksId)
//            {
//                queue.erase(it);
//                return true;
//            }
//        }
//    }

//    return false;
//}
bool RoundRobin::excludeTask(QString url, uint sourceId)
{
    QMap< uint, Queue >::iterator it(_tasks.find(sourceId));
    if(it != _tasks.end())
    {
        QQueue<QString>& queue(_queueMarker.value().queue);
        for(QQueue<QString>::iterator it(queue.begin()); it != queue.end(); ++it)
        {
            if(*it == url)
            {
                queue.erase(it);
                return true;
            }
        }
    }

    return false;
}





















