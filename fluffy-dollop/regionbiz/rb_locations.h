#ifndef RB_LOCATIONS_H
#define RB_LOCATIONS_H

#include <memory>
#include <vector>
#include <QFile>
#include <QPointF>
#include <QPolygon>

#include "rb_biz_relations.h"
#include "rb_base_entity.h"
#include "rb_marks.h"

namespace regionbiz {

class BaseArea;
typedef std::shared_ptr< BaseArea > BaseAreaPtr;
typedef std::vector< BaseAreaPtr > BaseAreaPtrs;

class RegionBizManager;
class GroupEntity;
typedef std::shared_ptr< GroupEntity > GroupEntityPtr;

class MarksHolder;
typedef std::shared_ptr< MarksHolder > MarksHolderPtr;

class LocalTransformKeeper;
typedef std::shared_ptr< LocalTransformKeeper > LocalTransformKeeperPtr;

class BaseArea: public BaseEntity
{
    friend class BaseDataTranslator;
    friend class RegionBizManager;

public:
    enum AreaType
    {
        AT_REGION,
        AT_LOCATION,
        AT_FACILITY,
        AT_FLOOR,
        AT_ROOM
    };

    // main
    BaseArea( uint64_t id );
    virtual ~BaseArea(){}
    virtual AreaType getType() = 0;
    EntityType getEntityType() override;
    bool commit() override;

    // coords
    QPolygonF getCoords();
    void setCoords(QPolygonF coord );

    // parent - child
    uint64_t getParentId();
    BaseAreaPtr getParent( AreaType parent_type );
    BaseAreaPtr getParent();
    BaseAreaPtrs getBaseAreaChilds();

    // groups
    GroupEntityPtr getGroup();
    bool moveToGroup( GroupEntityPtr group );
    bool leaveGroup();

    // converts
    MarksHolderPtr convertToMarksHolder();
    LocalTransformKeeperPtr convertToLocalTransformHolder();

    template< typename Type >
    static std::shared_ptr< Type > convert( BaseAreaPtr base )
    {
        return std::dynamic_pointer_cast< Type >( base );
    }

    template< typename Type >
    std::shared_ptr< Type > convert()
    {
        BaseAreaPtr ptr = BaseEntity::convert< BaseArea >( getItself() );
        return std::dynamic_pointer_cast< Type >( ptr );
    }

protected:
    QPolygonF _coords;
    uint64_t _parent_id = 0;

private:
    // private, because we need protect it
    void setParent( uint64_t id );
};

//----------------------------------------------

class BizRelationKepper: public BaseArea
{
public:
    BizRelationKepper( uint64_t id );

    BaseBizRelationPtrs getBizRelations();
    PropertyPtrs getPropertys();
    RentPtrs getRents();
};

//----------------------------------------------

class MarksHolder
{
public:
    MarksHolder( uint64_t id );

    uint64_t getHolderId();
    MarkPtrs getMarks();
    MarkPtrs getMarks( Mark::MarkType type );

    MarkPtr addMark(Mark::MarkType type, QPointF center = QPointF() );
    MarkPtr addMark(Mark::MarkType type, QPolygonF coords );
    bool commitMarks();
    bool deleteMarks();

private:
    bool checkHolderId();

    uint64_t _holder_id;
};

//----------------------------------------

class Facility;
typedef std::shared_ptr< Facility > FacilityPtr;

class LocalTransformKeeper
{
public:
    // need id
    LocalTransformKeeper( uint64_t id );

    // transform matrixes
    bool isHaveTransform();
    bool setTransform( QTransform transform );
    QTransform getTransform();
    void resetTransform();
    bool commitTransformMatrix();

private:
    FacilityPtr getFacilityParent();

    uint64_t _holder_id;
};

//---------------------------------------------

class Region: public BaseArea
{
public:
    enum RegionChildFilter
    {
        RCF_ALL,
        RCF_LOCATIONS,
        RCF_FACILITYS,
        RCF_ALL_FACILITYS
    };

    Region( uint64_t id );
    virtual ~Region(){}
    AreaType getType() override;

    // getters
    std::vector< BaseAreaPtr > getChilds( RegionChildFilter filter = RCF_ALL );
};
typedef std::shared_ptr< Region > RegionPtr;
typedef std::vector< RegionPtr > RegionPtrs;

//----------------------------------------------

class Location: public BaseArea, public MarksHolder
{
public:
    Location( uint64_t id );
    virtual ~Location(){}
    AreaType getType() override;

    // params
    QString getAddress();
    void setAddress( QString address );

    // getters
    std::vector< FacilityPtr > getChilds();
};
typedef std::shared_ptr< Location > LocationPtr;
typedef std::vector< LocationPtr > LocationPtrs;

//----------------------------------------------

class Floor;
typedef std::shared_ptr< Floor > FloorPtr;

class Facility: public BizRelationKepper, public MarksHolder
{
public:
    Facility( uint64_t id );
    virtual ~Facility(){}
    AreaType getType() override;

    // params
    QString getAddress();
    void setAddress( QString address );
    QString getCadastralNumber();
    void setCadastralNumber( QString number );

    // getters
    std::vector< FloorPtr > getChilds();

    // transform matrixes
    bool isHaveTransform();
    bool setTransform( QTransform transform );
    QTransform getTransform();
    void resetTransform();
    bool commitTransformMatrix();

    // etalon image
    bool isHaveEtalonPlan();
    BaseFileKeeperPtr getEtalonPlan();
    bool setEtalonPlan( BaseFileKeeperPtr file );
    void resetEtalonFile();
};
typedef std::vector< FacilityPtr > FacilityPtrs;

//----------------------------------------------

class Room;
typedef std::shared_ptr< Room > RoomPtr;

class Floor: public BizRelationKepper,
        public MarksHolder,
        public LocalTransformKeeper
{
public:
    Floor( uint64_t id );
    virtual ~Floor(){}
    AreaType getType() override;


    uint16_t getNumber();
    void setNumber( uint16_t number );

    // getters
    std::vector< RoomPtr > getChilds();

private:
};
typedef std::vector< FloorPtr > FloorPtrs;

//-----------------------------------------------

class Room: public BizRelationKepper,
        public MarksHolder,
        public LocalTransformKeeper
{
public:
    Room( uint64_t id );
    virtual ~Room(){}
    AreaType getType() override;
};
typedef std::vector< RoomPtr > RoomPtrs;

//----------------------------------------------

}

#endif // RB_LOCATIONS_H
