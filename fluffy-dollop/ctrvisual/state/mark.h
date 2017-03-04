#ifndef MARK_H
#define MARK_H

#include <QGraphicsEllipseItem>
#include <math.h>

namespace global_state
{
class Mark : public QGraphicsEllipseItem
{
private:
    const int TYPE;
public:
    Mark(int _type, QGraphicsItem* parent) : QGraphicsEllipseItem(parent), TYPE(_type) {}
    int type() {return TYPE;}
    bool check(double coef, QPointF p1)
    {
        QPointF p2(scenePos());
        return sqrt( (p1.x() - p2.x()) * (p1.x() - p2.x()) +  (p1.y() - p2.y()) * (p1.y() - p2.y()) ) <= coef;
    }
};
}
#endif // MARK_H
