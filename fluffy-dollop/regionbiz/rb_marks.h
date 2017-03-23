#ifndef RB_MARKS_H
#define RB_MARKS_H

#include <QPointF>

#include "rb_base_entity.h"

namespace regionbiz {

class Mark: public BaseEntity
{
public:
    Mark( uint64_t id );

    // center
    QPointF getCenter();
    void setCenter( QPointF center );

    // asociated object
    uint64_t getParentId();
    void setParentId( uint64_t id );

    // base comunication
    bool commit();

private:
    QPointF _center;
    uint64_t _parent_id;

    // for RTTI
    void rttiFunction() const override {}
};

typedef std::shared_ptr< Mark > MarkPtr;
typedef std::vector< MarkPtr > MarkPtrs;

}

#endif // RB_MARKS_H
