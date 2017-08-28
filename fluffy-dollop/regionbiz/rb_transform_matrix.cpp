#include "rb_transform_matrix.h"

#include <iostream>

#include "rb_manager.h"

using namespace regionbiz;

TransformMatrixManagerPtr TransformMatrixManager::instance()
{
    static TransformMatrixManagerPtr mngr =
            TransformMatrixManagerPtr( new TransformMatrixManager() );
    return mngr;
}

QTransform TransformMatrixManager::createTransform( QPolygonF src_global,
                                                    QPolygonF dst_local )
{
    using namespace std;

    QTransform trans;
    if( src_global.size() != VERTEX_COUNT
            || dst_local.size() != VERTEX_COUNT )
        cerr << "Warning: Incorrect polygon size "
                "on create transform" << endl;

    bool ok = QTransform::quadToQuad( src_global, dst_local, trans );
    if( ok )
        return trans;
    else
    {
        cerr << "Can't create transform matrix" << endl;
        return QTransform();
    }
}

QTransform TransformMatrixManager::createTransform( double m11, double m12,
                                                    double m21, double m22,
                                                    double s1, double s2 )
{
    QTransform trans( m11, m12, m21, m22, s1, s2 );
    return trans;
}

QTransform TransformMatrixManager::createTransform( double scale_x, double scale_y,
                                                    double rotate,
                                                    double shift_x, double shift_y )
{
    QTransform mat = QTransform::fromTranslate( shift_x, shift_y );
    mat.rotate( rotate );
    mat.scale( scale_x, scale_y );

    return mat;
}

bool TransformMatrixManager::isHaveTransform(uint64_t facility_id)
{
    auto mngr = RegionBizManager::instance();

    auto facil = mngr->getBaseArea( facility_id )->convert< Facility >();
    if( !facil )
        return false;

    bool have = isHaveTransform( facil );
    return have;
}

bool TransformMatrixManager::isHaveTransform(FacilityPtr facility)
{
    if( !facility )
        return false;

    uint64_t id = facility->getId();
    bool have;
    getMutex().lock();
    {
        have = getTransforms().find( id ) != getTransforms().end();
    }
    getMutex().unlock();

    return have;
}

bool TransformMatrixManager::setTransform(uint64_t facility_id, QTransform trans)
{
    auto mngr = RegionBizManager::instance();

    auto facil = mngr->getBaseArea( facility_id )->convert< Facility >();
    if( !facil )
        return false;

    bool seted = setTransform( facil, trans );
    return seted;
}

bool TransformMatrixManager::setTransform( FacilityPtr facility, QTransform trans )
{
    if( !facility )
        return false;

    uint64_t id = facility->getId();
    getMutex().lock();
    {
        if( getTransforms().find( id ) != getTransforms().end() )
            getTransforms()[ id ] = trans;
        else
            getTransforms().insert( std::make_pair( id, trans ));
    }
    getMutex().unlock();

    return true;
}

QTransform TransformMatrixManager::getTransform(uint64_t facility_id )
{
    auto mngr = RegionBizManager::instance();

    auto facil = mngr->getBaseArea( facility_id )->convert< Facility >();
    if( !facil )
        return QTransform();

    QTransform trans = getTransform( facil );
    return trans;
}

QTransform TransformMatrixManager::getTransform( FacilityPtr facility )
{
    if( !facility )
        return QTransform();

    uint64_t id = facility->getId();
    QTransform trans = QTransform();
    getMutex().lock();
    {
        if( getTransforms().find( id ) != getTransforms().end() )
            trans = getTransforms()[ id ];
    }
    getMutex().unlock();

    return trans;
}

void TransformMatrixManager::resetTransform(uint64_t facility_id)
{
    auto mngr = RegionBizManager::instance();

    auto facil = mngr->getBaseArea( facility_id )->convert< Facility >();
    if( !facil )
        return;

    resetTransform( facil );
}

void TransformMatrixManager::resetTransform(FacilityPtr facility)
{
    if( !facility )
        return;

    uint64_t id = facility->getId();
    getMutex().lock();
    {
        auto finded = getTransforms().find( id );
        if( finded != getTransforms().end() )
            getTransforms().erase( finded );
    }
    getMutex().unlock();
}

TransformMatrixManager::TransformMatrixManager()
{}

TransformById &TransformMatrixManager::getTransforms()
{
    static TransformById transforms;
    return transforms;
}

std::mutex &TransformMatrixManager::getMutex()
{
    static std::mutex mutex;
    return mutex;
}
