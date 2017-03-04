#include "sceneinthread.h"

#include <QPainter>
#include <QDebug>

using namespace visualize_system;

SceneInThread::SceneInThread()
    : _threadInWork(false)
{
    connect(this, SIGNAL(signalFinished(QImage*)), this, SLOT(slotFinished(QImage*)), Qt::QueuedConnection);
    start();
}

bool SceneInThread::isActive()
{
    return _threadInWork;
}

bool SceneInThread::startRender(QSize imageSize, const QRectF& sceneRect, int zLevel)
{
    _sceneData.imageSize = imageSize;
    _sceneData.sceneRect = sceneRect;
    _sceneData.zLevel = zLevel;
    _sceneData.handled = false;

    if(_threadInWork == false)
    {
        _threadInWork = true;

        _sceneData.handled = true;
        _sceneDataInRun = _sceneData;
        _removeItemsInRun = _removeItems;
        _removeItems.clear();
        _addItemsInRun = _addItems.values();
        _addItems.clear();

        _semaphore.release(1);
        return true;
    }
    else
        return false;
}

void SceneInThread::addItem(QString key, QImage img, QPointF pos, int z, double scale)
{
    ItemData data;
    data.key = key;
    data.img = img;
    data.pos = pos;
    data.z = z;
    data.scale = scale;
    _addItems.insert(key, data);
}

void SceneInThread::removeItems(QStringList keys)
{
    foreach(QString key, keys)
    {
        auto it = _addItems.find(key);
        if(it != _addItems.end())
            _addItems.erase(it);
        else
        _removeItems.append(key);
    }
}

void SceneInThread::run()
{
    AsincScene scene;
    QMap<QString, QGraphicsPixmapItem*> items;

    forever
    {
        qDebug() << "run 111";

        //  1) залипаем пока во вне не вызовут _semaphore.release(1)
        _semaphore.acquire(1);

        qDebug() << "run 222";

        //  2) блокируем, чтоб при следеющей итерации снова залипнуть
        _semaphore.acquire(_semaphore.available());

        //  3) удаляем элементы
        foreach(QString key, _removeItemsInRun)
        {
            auto it = items.find(key);
            if(it != items.end())
            {
                delete it.value();
                items.erase(it);
            }
        }
        _removeItemsInRun.clear();


        //  4) добавляем элементы
        foreach(ItemData data, _addItemsInRun)
        {
            auto it = items.find(data.key);
            if(it != items.end())
            {
                QGraphicsPixmapItem * item = it.value();
                item->setPixmap(QPixmap::fromImage(data.img));
            }
            else
            {
                QGraphicsPixmapItem * item = new QGraphicsPixmapItem(QPixmap::fromImage(data.img));
                item->setTransform(QTransform().scale(data.scale, data.scale));
                item->setZValue(data.z);
                item->setPos(data.pos);
                items.insert(data.key, item);
                scene.addItem(item);
            }
        }
        _addItemsInRun.clear();


        //  5) рендерим
        QImage *image = new QImage(_sceneDataInRun.imageSize, QImage::Format_ARGB32);
        image->fill(Qt::transparent);

        QPainter painter(image);
        QRectF imageRect(0, 0, _sceneDataInRun.imageSize.width(), _sceneDataInRun.imageSize.height() );
        scene.render(&painter, imageRect, _sceneDataInRun.sceneRect);


        //  6) уведомляем
        emit signalFinished(image);
    }
}

void SceneInThread::slotFinished(QImage *image)
{
    // отправляем картинку с позицией и зет-фактором заказчику
    emit finishRender(image, _sceneDataInRun.sceneRect, _sceneDataInRun.zLevel);

    if(_sceneData.handled == false)
    {
        _sceneData.handled = true;
        _sceneDataInRun = _sceneData;
        _removeItemsInRun = _removeItems;
        _removeItems.clear();
        _addItemsInRun = _addItems.values();
        _addItems.clear();

        _semaphore.release(1);
    }
    else
    {
        _threadInWork = false;
    }
}





















