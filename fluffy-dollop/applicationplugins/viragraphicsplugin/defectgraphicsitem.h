#ifndef DEFECTGRAPHICSITEM_H
#define DEFECTGRAPHICSITEM_H

#include "viragraphicsitem.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>

class DefectAreaItem;
class DefectPreviewItem;

class DefectGraphicsItem : public ViraGraphicsItem, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    DefectGraphicsItem(qulonglong id);
    ~DefectGraphicsItem();
    void setItemselected(bool on_off);
    void setItemEnabled(bool on_off);
    void setItemVisible(bool on_off);
    void centerOnItem();
    quint64 getId();
    ItemType getItemType();
    void reinit();

protected slots:
    void hoverEnterEventInPreview(bool on_off);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    const qulonglong _id;
    QPixmap _pixmap;
    DefectPreviewItem * _preview = nullptr;
    DefectAreaItem * _area = nullptr;
};

class DefectAreaItem : public QGraphicsPolygonItem
{
public:
    DefectAreaItem(const QPolygonF &polygon);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);
};

class DefectPreviewItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    DefectPreviewItem(QPixmap pixmap, QGraphicsItem * parent);

signals:
    void hoverEnterEvent(bool on_off);

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
};

#endif // MARKGRAPHICSITEM_H
