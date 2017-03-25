#ifndef RB_TRANSLATOR_H
#define RB_TRANSLATOR_H

#include <functional>
#include <vector>
#include <memory>
#include <QVariantMap>
#include <QSqlDatabase>

#include "rb_locations.h"
#include "rb_biz_relations.h"
#include "rb_metadata.h"
#include "rb_marks.h"

namespace regionbiz {

class BaseTranslator
{
public:
    bool init(QVariantMap settings);
    // TODO think about different checkers
    bool checkTranslator( QString& err );
    bool checkTranslator();

    // get name
    virtual QString getTranslatorName() = 0;

    // locations
    RegionPtrs loadRegions();
    LocationPtrs loadLocations();
    FacilityPtrs loadFacilitys();
    FloorPtrs loadFloors();
    RoomsGroupPtrs loadRoomsGroups();
    RoomPtrs loadRooms();

    // TODO check all inside functions

    // commit locations
    bool commitArea(BaseAreaPtr area);

    // delete locations
    bool deleteArea(BaseAreaPtr area);

//    // relations
//    std::vector< PropertyPtr > loadPropertys();
//    std::vector< RentPtr > loadRents();

    // metadata
    BaseMetadataPtrs loadMetadata();

    // marks
    MarkPtrs loadMarks();
    bool commitMark( MarkPtr mark );
    bool deleteMark( MarkPtr mark );

protected:
    virtual void loadFunctions() = 0;
    virtual bool initBySettings(QVariantMap /*settings*/ );
    void setParentForBaseLocation( BaseAreaPtr loc, uint64_t parent_id );
    void setAreaForBaseRalation( BaseBizRelationPtr relation, uint64_t id );

    // locations
    std::function< std::vector< RegionPtr >( void ) > _load_regions;
    std::function< std::vector< LocationPtr >( void ) > _load_locations;
    std::function< std::vector< FacilityPtr >( void ) > _load_facilitys;
    std::function< std::vector< FloorPtr >( void ) > _load_floors;
    std::function< std::vector< RoomsGroupPtr >( void ) > _load_rooms_groups;
    std::function< std::vector< RoomPtr >( void ) > _load_rooms;

    // delete location
    std::function< bool( BaseAreaPtr ) > _delete_area;

    // commit locations
    std::function< bool( BaseAreaPtr ) > _commit_area;

    // relations
    std::function< std::vector< PropertyPtr >( void ) > _load_propertys;
    std::function< std::vector< RentPtr >( void ) > _load_rents;

    // metadata
    std::function< std::vector< BaseMetadataPtr >( void ) > _load_metadata;

    // marks
    std::function< std::vector< MarkPtr >( void ) > _load_marks;
    std::function< bool( MarkPtr ) > _delete_mark;
    std::function< bool( MarkPtr ) > _commit_mark;

private:
    template< typename Return, typename Func >
    Return loadAreas( Func function );
};
typedef std::shared_ptr< BaseTranslator > BaseTranslatorPtr;
typedef std::vector< BaseTranslatorPtr > BaseTranslatorPtrs;

//-----------------------------------------------------------------

class BaseTranslatorFabric
{
    template< typename Translator >
    friend class BaseTranslatorFabricRegister;

public:
    static BaseTranslatorPtr getTranslatorByName(QString &name );

private:
    static void addTranslator(BaseTranslatorPtr translator );
    static BaseTranslatorPtrs& getTranslators();
};

//------------------------------------------------------------------

/**
 * This macro must using ones per Traslator class
 * becase it's create global register object with class name.
 * It's involve header dependencys
 **/
#define REGISTER_TRANSLATOR( trans ) \
    BaseTranslatorFabricRegister< trans > base_register_ ## trans;

template< typename Translator >
class BaseTranslatorFabricRegister
{
public:
    BaseTranslatorFabricRegister()
    {
        BaseTranslatorPtr ptr = BaseTranslatorPtr( new Translator() );
        BaseTranslatorFabric::addTranslator( BaseTranslatorPtr( ptr ));
    }
};

}

#endif // RB_TRANSLATOR_H
