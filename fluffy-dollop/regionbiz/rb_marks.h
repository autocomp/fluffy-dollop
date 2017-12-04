#ifndef RB_MARKS_H
#define RB_MARKS_H

#include <QPointF>
#include <QPolygonF>

#include "rb_base_entity.h"

namespace regionbiz {

class Layer;
typedef std::shared_ptr< Layer > LayerPtr;

class Room;
typedef std::shared_ptr< Room > RoomPtr;
typedef std::vector< RoomPtr > RoomPtrs;

class Mark: public BaseEntity
{
public:
    enum MarkType
    {
        MT_INVALID,
        MT_DEFECT,
        MT_PHOTO,
        MT_PHOTO_3D,
        MT_PLACEHOLDER,
        MT_LIFT,
        MT_STAIRS
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

    // layer
    LayerPtr getLayer();
    void moveToLayer( LayerPtr layer );
    void leaveLayer();

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

//-----------------------------------------

class PlaceHolderMark: public Mark
{
public:
    PlaceHolderMark( uint64_t id );
    MarkType getMarkType() override;
};
typedef std::shared_ptr< PlaceHolderMark > PlaceHolderMarkPtr;
typedef std::vector< PlaceHolderMarkPtr > PlaceHolderMarkPtrs;

//------------------------------------------

class VerticalWay: public Mark
{
public:
    VerticalWay( uint64_t id );
    virtual MarkType getMarkType() override = 0;

    // force - remove other room from same floor
    bool addRoom( RoomPtr room, bool force = false );
    bool addRoom( uint64_t id, bool force = false );
    bool removeRoom( RoomPtr room );
    bool removeRoom( uint64_t id );
    bool isPresentOnRoom( RoomPtr room );
    bool isPresentOnRoom( uint64_t id );
    std::vector< uint64_t > getRoomIds();
    RoomPtrs getRooms();

private:
    bool checkRoomInsideFloor( uint64_t id, bool force );
};

//-----------------------------------------

class LiftMark: public VerticalWay
{
public:
    LiftMark( uint64_t id );
    MarkType getMarkType() override;
};
typedef std::shared_ptr< LiftMark > LiftMarkPtr;
typedef std::vector< LiftMarkPtr > LiftMarkPtrs;

//--------------------------------------------

class StairsMark: public VerticalWay
{
public:
    StairsMark( uint64_t id );
    MarkType getMarkType() override;
};
typedef std::shared_ptr< StairsMark > StairsMarkPtr;
typedef std::vector< StairsMarkPtr > StairsMarkPtrs;

}

#endif // RB_MARKS_H
