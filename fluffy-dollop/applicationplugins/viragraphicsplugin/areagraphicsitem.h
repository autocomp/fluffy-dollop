#ifndef AREAGRAPHICSITEM_H
#define AREAGRAPHICSITEM_H

#include <QObject>
#include <QGraphicsPolygonItem>
#include <QPen>
#include <QBrush>
#include "viragraphicsitem.h"

struct AreaInitData
{
    QString tooltip;
    QPen penNormal;
    QPen penHoverl;
    QBrush brushNormal;
    QBrush brushHoverl;
    double zValue = 1000;
    qulonglong id = 0;
    bool sendDoubleClick = false;
    bool isSelectableFromMap = false;
};

class AreaGraphicsItem : public ViraGraphicsItem, public QGraphicsPolygonItem
{
    Q_OBJECT
public:
    AreaGraphicsItem(const QPolygonF& polygon);
    void init(const AreaInitData& areaInitData);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);
    void setItemselected(bool on_off);
    void centerOnItem();
    quint64 getId();
    void reinit();

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

    AreaInitData _areaInitData;
};

#endif

