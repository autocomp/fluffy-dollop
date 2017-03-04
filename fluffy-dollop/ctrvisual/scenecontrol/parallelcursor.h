#ifndef PARALLELCURSOR_H
#define PARALLELCURSOR_H

#include <QAbstractGraphicsShapeItem>
#include <QPen>

namespace visualize_system {

class ParallelCursor : public QAbstractGraphicsShapeItem
{
public:
    ParallelCursor(QGraphicsScene *scene);
    ~ParallelCursor();
    virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget);
    virtual QRectF boundingRect()const;

private:
    QPen m_pen;
};

}

#endif
