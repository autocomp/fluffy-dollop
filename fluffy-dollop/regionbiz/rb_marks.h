#ifndef RB_MARKS_H
#define RB_MARKS_H

#include <QPointF>
#include <QPolygonF>

#include "rb_base_entity.h"

namespace regionbiz {

class Mark: public BaseEntity
{
public:
    enum MarkType
    {
        MT_INVALID,
        MT_DEFECT,
        MT_PHOTO,
        MT_PHOTO_3D
    };

    // base comunication
    Mark( uint64_t id );
    EntityType getEntityType() override;
    virtual MarkType getMarkType() = 0;
    bool commit() override;

    // center
    QPointF getCenter();
    QPolygonF getCoords();
    void setCenter( QPointF center );
    void setCoords( QPolygonF coords );

    // asociated object
    uint64_t getParentId();
    void setParentId( uint64_t id );

private:
    QPolygonF _coords;
    uint64_t _parent_id;
};

typedef std::shared_ptr< Mark > MarkPtr;
typedef std::vector< MarkPtr > MarkPtrs;

//---------------------------------------

class MarkFabric
{
public:
    static MarkPtr createByType(Mark::MarkType type , uint64_t id);
};

//---------------------------------------

class DefectMark: public Mark
{
public:
    DefectMark( uint64_t id );
    MarkType getMarkType() override;
};
typedef std::shared_ptr< DefectMark > DefectMarkPtr;
typedef std::vector< DefectMarkPtr > DefectMarkPtrs;

//---------------------------------------

class PhotoMark: public Mark
{
public:
    PhotoMark( uint64_t id );
    MarkType getMarkType() override;
};
typedef std::shared_ptr< PhotoMark > PhotoMarkPtr;
typedef std::vector< PhotoMarkPtr > PhotoMarkPtrs;

//---------------------------------------

class Photo3dMark: public Mark
{
public:
    Photo3dMark( uint64_t id );
    MarkType getMarkType() override;
};
typedef std::shared_ptr< Photo3dMark > Photo3dMarkPtr;
typedef std::vector< Photo3dMarkPtr > Photo3dMarkPtrs;

}

#endif // RB_MARKS_H
