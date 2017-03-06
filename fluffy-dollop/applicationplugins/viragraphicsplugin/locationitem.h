#ifndef LOCATIONITEM_H
#define LOCATIONITEM_H

#include <QObject>
#include <QGraphicsPixmapItem>

class LocationItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit LocationItem(int minZoom, QRectF bSceneRect, const QList<QGraphicsItem*>& items, QGraphicsScene* scene);
    ~LocationItem();
    void zoomChanged(int zoom);

signals:
    void setViewport(QRectF);

protected:
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    bool _isVisible = true;
    int _minZoom;
    QRectF _bSceneRect;
    QList<QGraphicsItem*> _items;
};

#endif // LOCATIONITEM_H
