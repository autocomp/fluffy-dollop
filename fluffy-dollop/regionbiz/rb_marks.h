#ifndef RB_MARKS_H
#define RB_MARKS_H

#include <QPointF>
#include <QPolygonF>

#include "rb_base_entity.h"

namespace regionbiz {

class Mark: public BaseEntity
{
public:
    Mark( uint64_t id );
    EntityType getEntityType() override;

    // center
    QPointF getCenter();
    QPolygonF getCoords();
    void setCenter( QPointF center );
    void setCoords( QPolygonF coords );

    // asociated object
    uint64_t getParentId();
    void setParentId( uint64_t id );

    // base comunication
    bool commit();

private:
    QPolygonF _coords;
    uint64_t _parent_id;
};

typedef std::shared_ptr< Mark > MarkPtr;
typedef std::vector< MarkPtr > MarkPtrs;

}

#endif // RB_MARKS_H
