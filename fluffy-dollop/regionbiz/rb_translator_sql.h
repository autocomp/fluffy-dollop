#ifndef RB_TRANSLATOR_SQLITE_H
#define RB_TRANSLATOR_SQLITE_H

#include "rb_translator.h"

namespace regionbiz {

class SqlTranslator: public BaseTranslator
{
private:
    void loadFunctions() override;
    virtual bool initBySettings(QVariantMap settings) = 0;

    // read locations
    // TODO write template method
    std::vector< RegionPtr > loadRegions();
    std::vector< LocationPtr > loadLocations();
    std::vector< FacilityPtr > loadFacilitys();
    std::vector< FloorPtr > loadFloors();
    std::vector< RoomsGroupPtr > loadRoomsGroups();
    std::vector< RoomPtr > loadRooms();

    // read relations
    std::vector< PropertyPtr > loadPropertys();
    std::vector< RentPtr > loadRents();

    // support functions
    template< typename LocTypePtr >
    bool loadCoordinate( std::vector< LocTypePtr >& vector, QString name );
    bool loadPlans( BaseAreaPtr area );
    bool loadDocuments( BaseBizRelationPtr relation );
    bool loadPayments( RentPtr rent );
};

//------------------------------------------------------------

class SqliteTranslator: public SqlTranslator
{
    bool initBySettings( QVariantMap settings ) override;
};

//------------------------------------------------------------

class PsqlTranslator: public SqlTranslator
{
    bool initBySettings( QVariantMap settings ) override;
};

}

#endif // RB_TRANSLATOR_SQLITE_H
