#ifndef TMSDATAPROVIDER_H
#define TMSDATAPROVIDER_H

#include <QObject>
#include <QDebug>
#include <QBuffer>
#include <QPixmap>

#include <ctrcore/provider/t_image.h>
#include <ctrcore/provider/tiledataprovider.h>
#include "tmsbaseadapter.h"

namespace data_system
{

//struct SavedImage
//{
//    SavedImage(image_types::TImage* _img, uint _listener, uint _source) : img(_img), listener(_listener), source(_source) {}
//    image_types::TImage* img;
//    uint listener;
//    uint source;
//};

/**
 * @brief The TmsProviderTask struct - структура описывает одну задачу загрузки.
 */
struct TmsProviderTask
{
    /**
     * @brief TmsProviderTask - конструктор класса.
     * @param _img - матрица, в которую необходимо записать загруженные данные.
     * @param _taskId - идентификатор задачи.
     * @param _url - урл задачи.
     */
    TmsProviderTask(image_types::TImage* _img, uint _taskId, QString _url) : img(_img), taskId(_taskId), url(_url) {}
    TmsProviderTask(const TmsProviderTask& other) : img(other.img), taskId(other.taskId), url(other.url) {}
    TmsProviderTask() : img(0), taskId(0) {}
    image_types::TImage* img;
    uint taskId;
    QString url;
};

/**
 * @brief The TmsProviderTasks class - класс хранит в себе все задачи на загрузку.
 */
class TmsProviderTasks
{
public:
    TmsProviderTasks() {}

    /**
     * @brief insert - метод вставки задачи в контейнеры класса.
     * @param img - матрица, в которую необходимо записать загруженные данные.
     * @param taskId - идентификатор задачи.
     * @param url - урл задачи.
     */
    void insert(image_types::TImage* img, uint taskId, QString url)
    {
        TmsProviderTask * pTmsProviderTask = new TmsProviderTask(img, taskId, url);
        _taskMap.insert(taskId, pTmsProviderTask);
        _urlMap.insert(url, pTmsProviderTask);
    }

    /**
     * @brief getTask - метод возвращает задачу по идентификатору.
     * @param taskId - идентификатор задачи для поиска в контейнерах.
     * @param tmsProviderTask - при нахождении задачи в контейнерах класса найденная задача записывается в эту переменную.
     * @return - успешность выполнения. При true - задача найдена, записана в "tmsProviderTask" и удалена из контейнеров класса.
     */
    bool getTask(uint taskId, TmsProviderTask & tmsProviderTask)
    {
        auto taskIt = _taskMap.find(taskId);
        if(taskIt != _taskMap.end())
        {
            TmsProviderTask * fountTask = taskIt.value();
            _taskMap.erase(taskIt);
            tmsProviderTask.img = fountTask->img;
            tmsProviderTask.taskId = fountTask->taskId;
            tmsProviderTask.url = fountTask->url;

            auto urlIt = _urlMap.find(fountTask->url);
            if(urlIt != _urlMap.end())
                _urlMap.erase(urlIt);

            delete fountTask;

            return true;
        }
        else
            return false;
    }

    /**
     * @brief getTask - метод возвращает задачу по урлу.
     * @param url - урл задачи для поиска в контейнерах.
     * @param tmsProviderTask - при нахождении задачи в контейнерах класса найденная задача записывается в эту переменную.
     * @return - успешность выполнения. При true - задача найдена, записана в "tmsProviderTask" и удалена из контейнеров класса.
     */
    bool getTask(QString url, TmsProviderTask & tmsProviderTask)
    {
        auto urlIt = _urlMap.find(url);
        if(urlIt != _urlMap.end())
        {
            TmsProviderTask * fountTask = urlIt.value();
            _urlMap.erase(urlIt);
            tmsProviderTask.img = fountTask->img;
            tmsProviderTask.taskId = fountTask->taskId;
            tmsProviderTask.url = fountTask->url;

            auto taskIt = _taskMap.find(fountTask->taskId);
            if(taskIt != _taskMap.end())
                _taskMap.erase(taskIt);

            delete fountTask;

            return true;
        }
        else
            return false;
    }

    /**
     * @brief clear - метод очищает класс от всех задач.
     */
    void clear()
    {
        for(auto it = _taskMap.begin(); it != _taskMap.end(); ++it)
        {
            TmsProviderTask * fountTask = it.value();
            delete fountTask;
        }
        _taskMap.clear();
        _urlMap.clear();
    }

private:
    QMap<uint, TmsProviderTask*> _taskMap;
    QMap<QString, TmsProviderTask*> _urlMap;
};

/**
 *  Класс абстрактного тмс-провайдера, пердоставляющий по запросу данные с тмс-сервера.
 *  Запросы переадресуются в пул-загрузчиков.
 */

class TMSDataProvider : public TileDataProvider
{
    Q_OBJECT

public:
    TMSDataProvider();
    virtual ~TMSDataProvider();
    QMap <QString, QStringList> outKeys() const {return QMap<QString, QStringList>();}
    QList<InParam> outParameters(const QString&) const {return QList<InParam>();}
    QSize tilesize() const;

    virtual StoragePolicy storagePolicy() const
    {
        return AbstractDataProvider::SP_FullInMem;
    }

    virtual bool setStoragePolicy(StoragePolicy)
    {
        return false;
    }

    virtual ProviderType providerType() const
    {
        return AbstractDataProvider::Async;
    }

    virtual WorkMode workMode() const
    {
        return AbstractDataProvider::Read;
    }

    virtual QStringList accessType() const
    {
        QStringList list;
        list << "http";
        return list;
    }

    virtual QStringList resourceType() const
    {
        QStringList list;
        list << "tms";
        return list;
    }

    virtual bool isFinished()
    {
    return false;
    }

    //rdp
    virtual QRect boundingRect(){return QRect();}
    virtual QRectF spatialRect(){return m_spatialRect;}
    virtual QPolygonF spatialPolygon(){ QPolygonF(m_spatialRect); }
    virtual uint32_t rasterCount(){return 0;}
    virtual int readData(image_types::TImage& img, QRect source = QRect(), QSize dest = QSize(), bool async = false, uint32_t band = 1)
    {
        return -1;
    }
    virtual int writeData(image_types::TImage& img, QRect source = QRect(), QSize dest = QSize(), uint32_t band = 1)
    {
        return -1;
    }
    virtual int readRawData(image_types::TImage& img, QRect source = QRect(), QSize dest = QSize())
    {
        return -1;
    }
    virtual int readSpatialData(image_types::TImage& img, QRectF source, QRect dest = QRect(), uint32_t band = 1)
    {
        return -1;
    }
    virtual int readSpatialData(image_types::TImage &img, int epsgCode, QRectF source, QSize dest = QSize(), uint32_t band = 1)
    {
        return -1;
    }
    virtual int readSpatialData(image_types::TImage &img, const QString & proj4code, QRectF source, QSize dest = QSize(), uint32_t band = 1)
    {
        return -1;
    }
    virtual int registerDataListener(image_types::TImage& img, QMutex &lock)
    {
        return -1;
    }
    virtual void destroyDataListener(int num){}
    virtual qreal updateInterval() {return 0;}
    virtual void abort() {}
    virtual void abort(int task) {}

    //tdp

    /**
     * @brief registerSource метод предназначенный для регистрации отправителя запросов на получение тайлов от провайдера.
     * @param taskSender тип отправителя запросов согласно перечислению "TileDataProvider::TmsTaskSender"
     * @return присвоенный идентификатор отправителя запросов, необходимо запомнить и использовать в методе "tile" и прочих.
     */
    uint registerSource(TileDataProvider::TmsTaskSender taskSender);

    /**
     * @brief unregisterSource метод предназначенный для удаления отправителя запросов.
     * @param source идентификатор удаляемого отправителя запросов.
     * @return успешность удаления из системы.
     */
    bool unregisterSource(uint sourceId);

    /**
     * @brief tile Метод, предназначенный для записи тайла по заданным координатам в матрицу image_types::TImage.
     * @param img
     * @param x
     * @param y
     * @param z
     * @param sourceId - некий идентификатор источника запроса, необходим для распаралеливания очередей(стеков) загрузки от разных источников в пуле загрузчиков.
     */
    virtual int tile(image_types::TImage &img, int x, int y, int z, uint sourceId);

    virtual void abortAllTaskFromSource(uint sourceId);
    virtual void abortTaskFromSource(uint tasksId, uint sourceId);
    virtual const TmsBaseAdapter * getTmsAdapter() = 0;

private:
    virtual void loadRaster(){}
    virtual void saveRaster(){}


protected:
    virtual QString host() const = 0;
    virtual QString serverPath() const = 0;
    virtual QString scheme() const = 0;
    virtual int serverPort() const = 0;
    virtual QUrl query(int x, int y, int z) const = 0;

    QSize mTileSize;
    QRectF m_spatialRect;

private slots:
    //void slot_requestFinished(int inKey, uint sourceId, QByteArray * src, TileDataProvider::Result result);
    void slot_requestFinished(QString url, uint sourceId, QByteArray * src, TileDataProvider::Result result);

private:

    //QMap<uint, QMap<int, SavedImage> > m_map;
    QMap<uint, TmsProviderTasks> m_taskFromSourece;
    data_system::MetaData m_metaData;

signals:
    void imageReceived();
    void loadingFinished();
    void signal_recivedImage(const QPixmap&, uint providerId, uint returnId);
    //////////////////////////////////////////////////////////////////////////////
public:
signals:
    void signal_receivedImage(const QPixmap pixmap, const QString& url);
    //////////////////////////////////////////////////////////////////////////////
};

}
#endif
