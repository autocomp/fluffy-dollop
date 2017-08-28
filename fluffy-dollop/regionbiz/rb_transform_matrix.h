#ifndef RB_TRANSFORM_MATRIX_H
#define RB_TRANSFORM_MATRIX_H

#include <memory>
#include <map>
#include <QTransform>

#include "rb_locations.h"

#define VERTEX_COUNT 4

namespace regionbiz {

typedef std::map< uint64_t, QTransform > TransformById;
class TransformMatrixManager;
typedef std::shared_ptr< TransformMatrixManager > TransformMatrixManagerPtr;

class TransformMatrixManager
{
public:
    static TransformMatrixManagerPtr instance();

    static QTransform createTransform( QPolygonF src_global, QPolygonF dst_local );
    static QTransform createTransform( double m11, double m12,
                                       double m21, double m22,
                                       double s1, double s2 );
    // first - scale, second - rotate, third - shift
    static QTransform createTransform( double scale_x, double scale_y,
                                       double rotate,
                                       double shift_x, double shift_y );

    bool isHaveTransform( uint64_t facility_id );
    bool isHaveTransform( FacilityPtr facility );
    bool setTransform( uint64_t facility_id, QTransform trans );
    bool setTransform( FacilityPtr facility, QTransform trans );
    QTransform getTransform( uint64_t facility_id );
    QTransform getTransform( FacilityPtr facility );
    void resetTransform( uint64_t facility_id );
    void resetTransform( FacilityPtr facility );

private:
    TransformMatrixManager();

    TransformById& getTransforms();
    std::mutex& getMutex();
};

}

#endif // RB_TRANSFORM_MATRIX_H
