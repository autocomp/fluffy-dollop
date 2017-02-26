#ifndef SCENEINTHREAD_H
#define SCENEINTHREAD_H

#include <QGraphicsScene>
#include <QThread>
#include <QSemaphore>
#include <QImage>
#include <QMap>
#include <QString>
#include <QGraphicsPixmapItem>

namespace visualize_system {

class SceneInThread : public QThread
{
    Q_OBJECT

    class AsincScene : public QGraphicsScene
    {
    public:
        AsincScene() {}
    protected:
        virtual void drawBackground(QPainter *painter, const QRectF &rect) {}
    };

public:
    struct ItemData
    {
        QString key;
        QImage img;
        QPointF pos;
        int z;
        double scale;
    };
    struct SceneData
    {
        QSize imageSize;
        QRectF sceneRect;
        int zLevel;
        bool handled;
    };

    explicit SceneInThread();
    bool startRender(QSize imageSize, const QRectF& sceneRect, int zLevel);
    void addItem(QString key, QImage img, QPointF pos, int z, double scale);
    void removeItems(QStringList keys);
    bool isActive();

signals:
    void signalFinished(QImage *image);
    void finishRender(QImage* image, QRectF sceneRect, int zLevel);

protected:
    void run();

private slots:
    void slotFinished(QImage *image);

private:
    QSemaphore _semaphore;
    QMap<QString,ItemData> _addItems;
    QList<ItemData> _addItemsInRun;
    QStringList _removeItems, _removeItemsInRun;
    SceneData _sceneData, _sceneDataInRun;
    bool _threadInWork;
};

}

#endif // SCENEINTHREAD_H
