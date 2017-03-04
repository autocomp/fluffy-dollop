#include "parallelcursor.h"
#include <QPainter>
#include <QGraphicsScene>

using namespace visualize_system;

ParallelCursor::ParallelCursor(QGraphicsScene* scene)
{
    scene->addItem(this);
    m_pen.setWidth(3);
    m_pen.setColor(Qt::red);
    setFlags(QGraphicsItem::ItemIgnoresTransformations);
    setZValue(10000000);
}


ParallelCursor::~ParallelCursor()
{
}

QRectF ParallelCursor::boundingRect()const
{
    return QRectF(-6,-6,15,15);
}

void ParallelCursor::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  //  qDebug("here");
    painter->setPen(m_pen);
    painter->drawLine(-5,0,5,0);
    painter->drawLine(0,-5,0,5);
}
