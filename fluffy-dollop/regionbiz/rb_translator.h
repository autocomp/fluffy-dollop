#ifndef RB_TRANSLATOR_H
#define RB_TRANSLATOR_H

#include <functional>
#include <vector>
#include <memory>
#include <QVariantMap>
#include <QSqlDatabase>

#include "rb_locations.h"
#include "rb_biz_relations.h"

namespace regionbiz {

class BaseTranslator
{
public:
    bool init(QVariantMap settings);
    bool checkTranslator( std::string& err );
    bool checkTranslator();

    // locations
    std::vector< RegionPtr > loadRegions();
    std::vector< LocationPtr > loadLocations();
    std::vector< FacilityPtr > loadFacilitys();
    std::vector< FloorPtr > loadFloors();
    std::vector< RoomsGroupPtr > loadRoomsGroups();
    std::vector< RoomPtr > loadRooms();

    // relations
    std::vector< PropertyPtr > loadPropertys();
    std::vector< RentPtr > loadRents();

protected:
    virtual void loadFunctions() = 0;
    virtual bool initBySettings( QVariantMap settings ) = 0;
    void setParentForBaseLocation( BaseAreaPtr loc, uint64_t parent_id );
    void setAreaForBaseRalation( BaseBizRelationPtr relation, uint64_t id );

    // locations
    std::function< std::vector< RegionPtr >( void ) > _load_regions;
    std::function< std::vector< LocationPtr >( void ) > _load_locations;
    std::function< std::vector< FacilityPtr >( void ) > _load_facilitys;
    std::function< std::vector< FloorPtr >( void ) > _load_floors;
    std::function< std::vector< RoomsGroupPtr >( void ) > _load_rooms_groups;
    std::function< std::vector< RoomPtr >( void ) > _load_rooms;

    // relations
    std::function< std::vector< PropertyPtr >( void ) > _load_propertys;
    std::function< std::vector< RentPtr >( void ) > _load_rents;
};
typedef std::shared_ptr< BaseTranslator > BaseTranslatorPtr;

//-----------------------------------------------------------------

class BaseTranslatorFabric
{
public:
    static BaseTranslatorPtr getTranslatorByType( std::string& type );
};

}

#endif // RB_TRANSLATOR_H
