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

class BaseArea: public BaseEntity
{
    friend class BaseTranslator;
    friend class RegionBizManager;

public:
    enum AreaType
    {
        AT_REGION,
        AT_LOCATION,
        AT_FACILITY,
        AT_FLOOR,
        AT_ROOMS_GROUP,
        AT_ROOM
    };

    BaseArea( uint64_t id );
    virtual ~BaseArea(){}

    // coords
    QPolygonF getCoords();
    void setCoords(QPolygonF coord );

    // parent - child
    uint64_t getParentId();
    BaseAreaPtr getParent( AreaType parent_type );
    BaseAreaPtr getParent();
    BaseAreaPtrs getChilds();
    virtual AreaType getType() = 0;

    bool commit();

    // params
    QString getDescription();
    void setDesription( QString descr );
    QString getName();
    void setName( QString name );

    template< typename Type >
    static std::shared_ptr< Type > convert( BaseAreaPtr base )
    {
        return std::dynamic_pointer_cast< Type >( base );
    }

    // WARNING slow converter
    template< typename Type >
    std::shared_ptr< Type > convert()
    {
        BaseAreaPtr ptr = BaseEntity::convert< BaseArea >( getItself() );
        return std::dynamic_pointer_cast< Type >( ptr );
    }

protected:
    QPolygonF _coords;
    uint64_t _parent_id = 0;
    QString _description = "";
    QString _name = "";

private:
    // private, because we need protect it
    void setParent( uint64_t id );

    // for RTTI
    void rttiFunction() const override {}
};

//----------------------------------------

typedef std::shared_ptr< QFile > QFilePtr;
class PlanKeeper
{
public:
    struct PlanParams
    {
        double scale_w = 1;
        double scale_h = 1;
        double rotate = 0;
        double x = 0;
        double y = 0;
    };

    PlanKeeper(){}
    virtual ~PlanKeeper(){}

    QString getPlanPath();
    void setPlanPath( QString path );
    QFilePtr getPlanFile();
    PlanParams getPlanParams();
    void setPlanParams( PlanParams params );

    // TODO file db

private:
    QString _plan_path = "";
    PlanParams _params;
};
typedef std::shared_ptr< PlanKeeper > PlanKeeperPtr;

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

    MarkPtr addMark( QPointF center = QPointF() );
    bool commitMarks();
    bool deleteMarks();

private:
    bool checkHolderId();

    uint64_t _holder_id;
};
typedef std::shared_ptr< MarksHolder > MarksHolderPtr;

//----------------------------------------------

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

class Facility;
typedef std::shared_ptr< Facility > FacilityPtr;

class Location: public BaseArea, public PlanKeeper, public MarksHolder
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

class Facility: public BizRelationKepper
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
};
typedef std::vector< FacilityPtr > FacilityPtrs;

//----------------------------------------------

class Floor: public BizRelationKepper, public PlanKeeper, public MarksHolder
{
public:
    enum FloorChildFilter
    {
        FCF_ALL,
        FCF_GROUPS,
        FCF_ROOMS,
        FCF_ALL_ROOMS
    };

    Floor( uint64_t id );
    virtual ~Floor(){}
    AreaType getType() override;


    uint16_t getNumber();
    void setNumber( uint16_t number );

    // getters
    BaseAreaPtrs getChilds( FloorChildFilter filter = FCF_ALL );

private:
};
typedef std::vector< FloorPtr > FloorPtrs;

//-----------------------------------------------

class Room;
typedef std::shared_ptr< Room > RoomPtr;

class RoomsGroup: public BizRelationKepper, public PlanKeeper
{
public:
    RoomsGroup( uint64_t id );
    virtual ~RoomsGroup(){}
    AreaType getType() override;

    // params
    QString getAddress();
    void setAddress( QString address );
    QString getCadastralNumber();
    void setCadastralNumber( QString number );

    // getters
    std::vector< RoomPtr > getChilds();
};
typedef std::shared_ptr< RoomsGroup > RoomsGroupPtr;
typedef std::vector< RoomsGroupPtr > RoomsGroupPtrs;

//-------------------------------------------------

class Room: public BizRelationKepper, public PlanKeeper, public MarksHolder
{
public:
    Room( uint64_t id );
    virtual ~Room(){}
    AreaType getType() override;
};
typedef std::vector< RoomPtr > RoomPtrs;

}

#endif // RB_LOCATIONS_H
