#ifndef LOCATIONITEM_H
#define LOCATIONITEM_H

#include <QObject>
#include <QGraphicsPixmapItem>

class LocationItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT
public:
    explicit LocationItem(qulonglong id, int minZoom, const QList<QGraphicsItem*>& items, QGraphicsScene* scene);
    ~LocationItem();
    void setMinZoom(int minZoom);
    void zoomChanged(int zoom);
    void setItemselected(bool on_off);
    bool locationIsVisible();
    void addItem(QGraphicsItem* item);
    void removeItem(QGraphicsItem* item);

signals:
    void setViewport(QRectF);
    void signalSelectItem(qulonglong id, bool centerOnEntity);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    const qulonglong _id;
    bool _isVisible = true;
    bool _selected = false;
    int _minZoom;
    QList<QGraphicsItem*> _items;
};

#endif // LOCATIONITEM_H
