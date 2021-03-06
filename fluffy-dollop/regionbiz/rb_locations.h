#ifndef RB_LOCATIONS_H
#define RB_LOCATIONS_H

#include <memory>
#include <vector>
#include <QFile>

//#include "rb_manager.h"

namespace regionbiz {

class Coord
{
public:
    Coord( double x_, double y_ ):
        x( x_ ), y( y_ )
    {}

    double x = 0;
    double y = 0;
};
typedef std::vector< Coord > Coords;

class BaseArea;
typedef std::shared_ptr< BaseArea > BaseAreaPtr;
typedef std::vector< BaseAreaPtr > BaseAreaPtrs;

//class RegionBizManager;

// TODO rename to area
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

    Coords getCoords();
    void setCoords( Coords coord );
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
    Coords _coords;
    uint64_t _id = 0;
    uint64_t _parent_id = 0;

private:
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

    std::string getPlanPath();
    void setPlanPath( std::string path );
    QFilePtr getPlanFile();
    PlanParams getPlanParams();
    void setPlanParams( PlanParams params );

    // TODO file db
    // TODO file attributes

private:
    std::string _plan_path = "";
    PlanParams _params;
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
    std::string getDescription();
    void setDesription( std::string descr );

    // getters
    std::vector< BaseAreaPtr > getChilds( RegionChildFilter filter = RCF_ALL );

private:
    std::string _description = "";
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
    std::string getDescription();
    void setDesription( std::string descr );
    std::string getAddress();
    void setAddress( std::string address );

    // getters
    std::vector< FacilityPtr > getChilds();

private:
    std::string _description = "";
    std::string _address = "";
};
typedef std::shared_ptr< Location > LocationPtr;
typedef std::vector< LocationPtr > LocationPtrs;

//----------------------------------------------

class Floor;
typedef std::shared_ptr< Floor > FloorPtr;

class Facility: public BaseArea
{
public:
    Facility( uint64_t id );
    virtual ~Facility(){}
    AreaType getType() override;

    // params
    std::string getDescription();
    void setDesription( std::string descr );
    std::string getAddress();
    void setAddress( std::string address );
    std::string getCadastralNumber();
    void setCadastralNumber( std::string number );

    // getters
    std::vector< FloorPtr > getChilds();

private:
    std::string _description = "";
    std::string _address = "";
    std::string _cadastral_number = "";
};
typedef std::vector< FacilityPtr > FacilityPtrs;

//----------------------------------------------

class Floor: public BaseArea, public PlanKeeper
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
    std::string getName();
    void setName( std::string name );
    uint16_t getNumber();
    void setNumber( uint16_t number );

    // getters
    BaseAreaPtrs getChilds( FloorChildFilter filter = FCF_ALL );

private:
    std::string _name = "";
    int16_t _number = 0;

};
typedef std::vector< FloorPtr > FloorPtrs;

//-----------------------------------------------

class Room;
typedef std::shared_ptr< Room > RoomPtr;

class RoomsGroup: public BaseArea, public PlanKeeper
{
public:
    RoomsGroup( uint64_t id );
    virtual ~RoomsGroup(){}
    AreaType getType() override;

    // params
    std::string getAddress();
    void setAddress( std::string address );
    std::string getCadastralNumber();
    void setCadastralNumber( std::string number );

    // getters
    std::vector< RoomPtr > getChilds();

private:
    std::string _address = "";
    std::string _cadastral_number = "";
};
typedef std::shared_ptr< RoomsGroup > RoomsGroupPtr;
typedef std::vector< RoomsGroupPtr > RoomsGroupPtrs;

//-------------------------------------------------

class Room: public BaseArea, public PlanKeeper
{
public:
    Room( uint64_t id );
    virtual ~Room(){}
    AreaType getType() override;

    // params
    std::string getName();
    void setName( std::string name );

private:
    std::string _name = "";
};
typedef std::vector< RoomPtr > RoomPtrs;

}

#endif // RB_LOCATIONS_H
