#ifndef RB_LOCATIONS_H
#define RB_LOCATIONS_H

#include <memory>
#include <vector>
#include <QFile>
#include <QPointF>
#include <QPolygon>

//#include "rb_manager.h"
#include "rb_biz_relations.h"

namespace regionbiz {

class BaseArea;
typedef std::shared_ptr< BaseArea > BaseAreaPtr;
typedef std::vector< BaseAreaPtr > BaseAreaPtrs;

//class RegionBizManager;

class BaseArea
{
    friend class BaseTranslator;

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

    QPolygonF getCoords();
    void setCoords(QPolygonF coord );
    uint64_t getId();
    uint64_t getParentId();
    BaseAreaPtr getParent( AreaType parent_type );
    BaseAreaPtr getParent();
    virtual AreaType getType() = 0;

    template< typename Type >
    static std::shared_ptr< Type > convert( BaseAreaPtr base )
    {
        return std::dynamic_pointer_cast< Type >( base );
    }

    // TODO think how to read
//    template< typename Type >
//    std::shared_ptr< Type > convert()
//    {
//        auto mngr = RegionBizManager::instance();
//        BaseLocationPtr ptr = mngr->getBaseLoation( _id, getType() );
//        return std::dynamic_pointer_cast< Type >( ptr );
//    }

protected:
    QPolygonF _coords;
    uint64_t _id = 0;
    uint64_t _parent_id = 0;

private:
    // private, becouse we need protect it
    void setParent( uint64_t id );
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

    // params
    QString getDescription();
    void setDesription( QString descr );

    // getters
    std::vector< BaseAreaPtr > getChilds( RegionChildFilter filter = RCF_ALL );

private:
    QString _description = "";
};
typedef std::shared_ptr< Region > RegionPtr;
typedef std::vector< RegionPtr > RegionPtrs;

//----------------------------------------------

class Facility;
typedef std::shared_ptr< Facility > FacilityPtr;

class Location: public BaseArea, public PlanKeeper
{
public:
    Location( uint64_t id );
    virtual ~Location(){}
    AreaType getType() override;

    // params
    QString getDescription();
    void setDesription( QString descr );
    QString getAddress();
    void setAddress( QString address );

    // getters
    std::vector< FacilityPtr > getChilds();

private:
    QString _description = "";
    QString _address = "";
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
    QString getDescription();
    void setDesription( QString descr );
    QString getAddress();
    void setAddress( QString address );
    QString getCadastralNumber();
    void setCadastralNumber( QString number );

    // getters
    std::vector< FloorPtr > getChilds();

private:
    QString _description = "";
    QString _address = "";
    QString _cadastral_number = "";
};
typedef std::vector< FacilityPtr > FacilityPtrs;

//----------------------------------------------

class Floor: public BizRelationKepper, public PlanKeeper
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

    // params
    QString getName();
    void setName( QString name );
    uint16_t getNumber();
    void setNumber( uint16_t number );

    // getters
    BaseAreaPtrs getChilds( FloorChildFilter filter = FCF_ALL );

private:
    QString _name = "";
    int16_t _number = 0;

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

private:
    QString _address = "";
    QString _cadastral_number = "";
};
typedef std::shared_ptr< RoomsGroup > RoomsGroupPtr;
typedef std::vector< RoomsGroupPtr > RoomsGroupPtrs;

//-------------------------------------------------

class Room: public BizRelationKepper, public PlanKeeper
{
public:
    Room( uint64_t id );
    virtual ~Room(){}
    AreaType getType() override;

    // params
    QString getName();
    void setName( QString name );

private:
    QString _name = "";
};
typedef std::vector< RoomPtr > RoomPtrs;

}

#endif // RB_LOCATIONS_H
