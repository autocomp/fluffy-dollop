#include "handledirectionitem.h"
#include <QPen>
#include <QColor>
#include <QPainter>
#include <QCursor>
#include <QDebug>
#include <QtMath>

HandleDirectionItem::HandleDirectionItem(QGraphicsItem * parent, bool isMovable)
    : QGraphicsPixmapItem(parent)
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, isMovable);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    setFlag(QGraphicsItem::ItemStacksBehindParent, true);

    if(isMovable)
        setCursor(QCursor(Qt::CrossCursor));

    QPixmap pm(11,11);
    pm.fill(Qt::transparent);

    QPen pen;
    QColor colPen;
    colPen.setRgba(qRgba(0,0,0,255)); // qRgba(0x09,0x61,0xD3,150));
    pen.setColor(colPen);
    pen.setWidth(2);
    pen.setCosmetic(true);
    pen.setJoinStyle(Qt::RoundJoin);

    QBrush brush;
    QColor colBr;
    colBr.setRgba(qRgba(0xF3,0xF0,0x0F,255));
    brush.setColor(colBr);
    brush.setStyle(Qt::SolidPattern);

    QPainter pr(&pm);
    pr.setPen(pen);
    pr.setBrush(brush);
    pr.drawEllipse(1, 1, pm.width()-2, pm.height()-2);

    setPixmap(pm);
    setOffset(-5, -5);

}

HandleDirectionItem::~HandleDirectionItem()
{
}

QVariant HandleDirectionItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemPositionChange)
    {
        QPointF newPos = value.toPointF();
        QLineF line(QPointF(0,0), newPos);
        double angle = line.angle();
        qDebug() << "angle :" << angle;
        QPointF pos(_radius * cos( -angle * M_PI / 180 ), _radius * sin( -angle * M_PI / 180 ) );
        emit handleDirectionReplaced(pos, angle);
        return QGraphicsPixmapItem::itemChange(change, pos);
    }
    else
        return QGraphicsPixmapItem::itemChange(change, value);
}
