#include "tilesceneasinc.h"
#include <QtConcurrent/QtConcurrentRun> //qtconcurrentrun.h>
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QTime>
#include "unistd.h"
#include <functional>

using namespace visualize_system;

TileSceneAsinc::TileSceneAsinc()
    : _isActive(false)
    , _zLevel(0)
    , _asincMode(false)
{
    connect(&_futureWatcher, SIGNAL(finished()), this, SLOT(renderFinished()));
    _scene.setBackgroundBrush(QBrush(Qt::gray));
}

bool TileSceneAsinc::isActive()
{
    return _isActive;
}

void TileSceneAsinc::addItem(QGraphicsItem* item)
{
    if(_isActive)
        _addItems << item;
    else
        _scene.addItem(item);
}

void TileSceneAsinc::addItems(QList<QGraphicsItem*> list)
{
    if(_isActive)
        _addItems << list;
    else
        foreach(QGraphicsItem* item, list)
            _scene.addItem(item);
}

void TileSceneAsinc::removeItem(QGraphicsItem* item)
{
    if(_isActive)
        _removeItems << item;
    else
        {
            _scene.removeItem(item);
            delete item;
        }
}

void TileSceneAsinc::removeItems(QList<QGraphicsItem*> list)
{
    if(_isActive)
        _removeItems << list;
    else
        foreach(QGraphicsItem* item, list)
        {
            _scene.removeItem(item);
            delete item;
        }
}

bool TileSceneAsinc::startRender(QSize sizeImage, const QRectF &sceneRect, int zLevel)
{
    if(_isActive)
        return false;

    _isActive = true;
    _sizeImage = sizeImage;
    _sceneRect = sceneRect;
    _zLevel = zLevel;

    _time.restart();
    //qDebug() << "TileSceneAsinc::startRender, sizeImage :" << sizeImage << ", sceneRect :" << sceneRect << ", zLevel :" << zLevel;

    if(_asincMode)
        _futureWatcher.setFuture(QtConcurrent::run(std::bind(&TileSceneAsinc::run, this)));
    else
        sendImage(run());

    return true;
}

QImage* TileSceneAsinc::run()
{
    QImage *image = new QImage(_sizeImage, QImage::Format_ARGB32);
    image->fill(Qt::transparent);

    QPainter painter(image);
    QRectF imageRect(0, 0, _sizeImage.width(), _sizeImage.height() );
    _scene.render(&painter, imageRect, _sceneRect);
    if(painter.end() == false)
        qDebug() << "&&& TileSceneAsinc::run(), painter.end() == false !!!";
    return image;
}

void TileSceneAsinc::renderFinished()
{
    QFutureWatcher<QImage*>* watcher = dynamic_cast<QFutureWatcher<QImage*>*>(sender());
    if(watcher == 0)
        return;

    QImage* image = watcher->result();
    sendImage(image);
}


void TileSceneAsinc::sendImage(QImage * image)
{
    foreach(QGraphicsItem* item, _addItems)
        _scene.addItem(item);
    _addItems.clear();

    foreach(QGraphicsItem* item, _removeItems)
    {
        _scene.removeItem(item);
        delete item;
    }
    _removeItems.clear();

    _isActive = false;

    //qDebug() << "TileSceneAsinc::renderFinished, asincMode :" << _asincMode << ", elapsed :" << _time.elapsed() << "image size :" << image->size(); // << ", items on asinc scene :" << _scene.items().size();;
    emit finishRender(image, _sceneRect,_zLevel );
}

void TileSceneAsinc::setAsincMode(bool on_off)
{
    _asincMode = on_off;
}






