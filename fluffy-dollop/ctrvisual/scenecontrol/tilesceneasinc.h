#ifndef TILESCENEASINC_H
#define TILESCENEASINC_H

#include <QGraphicsScene>
#include <QFutureWatcher>
#include <QTime>
#include <QImage>

namespace visualize_system {

class TileSceneAsinc : public QObject
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
    TileSceneAsinc();
    bool startRender(QSize sizeImage, const QRectF& sceneRect, int zLevel);
    bool isActive();
    void addItem(QGraphicsItem* item);
    void addItems(QList<QGraphicsItem*> list);
    void removeItem(QGraphicsItem* item);
    void removeItems(QList<QGraphicsItem*> list);
    void setAsincMode(bool on_off);

signals:
    void finishRender(QImage* image, QRectF sceneRect, int zLevel);

private:
    AsincScene _scene;
    QFutureWatcher<QImage*> _futureWatcher;
    QList<QGraphicsItem*> _addItems;
    QList<QGraphicsItem*> _removeItems;
    bool _isActive;
    QSize _sizeImage;
    QRectF _sceneRect;
    int _zLevel;
    bool _asincMode;
    QTime _time;

    QImage *run();
    void sendImage(QImage *);

private slots:
    void renderFinished();
};
}

#endif // SCENE_H
