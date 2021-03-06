#include "rb_translator_sqlite.h"

#include <iostream>
#include <QSqlQuery>

using namespace regionbiz;

void SqliteTranslator::loadFunctions()
{
    _load_regions = std::bind( &SqliteTranslator::loadRegions, this );
    _load_locations = std::bind( &SqliteTranslator::loadLocations, this );
    _load_facilitys = std::bind( &SqliteTranslator::loadFacilitys, this );
    _load_floors = std::bind( &SqliteTranslator::loadFloors, this );
    _load_rooms_groups = std::bind( &SqliteTranslator::loadRoomsGroups, this );
    _load_rooms = std::bind( &SqliteTranslator::loadRooms, this );
}

bool SqliteTranslator::initBySettings( QVariantMap settings )
{
    QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE" );
    QString path = settings["file_path"].toString();
    db.setDatabaseName( path );

    if (db.open())
    {
        std::cout << "Database: connection ok" << std::endl;
        return true;
    }
    else
    {
        std::cerr << "Error: connection with database fail" << std::endl;
        return false;
    }
}

std::vector< RegionPtr > SqliteTranslator::loadRegions()
{
    std::vector< RegionPtr > regions;

    QSqlDatabase db = QSqlDatabase::database();
    QString select = "SELECT id, description FROM regions";
    QSqlQuery query( db );
    bool res = query.exec( select );
    if( res )
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( 0 ).toLongLong();
            std::string descr = query.value( 1 ).toString().toUtf8().data();

            RegionPtr reg = RegionPtr( new Region( id ));
            reg->setDesription( descr );
            setParentForBaseLocation( reg, 0 );

            regions.push_back( reg );
        }

    loadCoordinate< RegionPtr >( regions, "regions" );

    return regions;
}

std::vector<LocationPtr> SqliteTranslator::loadLocations()
{
    std::vector< LocationPtr > locations;

    QSqlDatabase db = QSqlDatabase::database();
    QString select = "SELECT id, parent, description, address FROM locations";
    QSqlQuery query( db );
    bool res = query.exec( select );
    if( res )
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( 0 ).toLongLong();
            uint64_t parent_id = query.value( 1 ).toLongLong();
            std::string descr = query.value( 2 ).toString().toUtf8().data();
            std::string addr = query.value( 3 ).toString().toUtf8().data();

            LocationPtr loc = LocationPtr( new Location( id ));
            setParentForBaseLocation( loc, parent_id );
            loc->setDesription( descr );
            loc->setAddress( addr );
            loadPlans( loc );

            locations.push_back( loc );
        }

    loadCoordinate< LocationPtr >( locations, "locations" );

    return locations;
}

std::vector<FacilityPtr> SqliteTranslator::loadFacilitys()
{
    std::vector< FacilityPtr > facilitys;

    QSqlDatabase db = QSqlDatabase::database();
    QString select = "SELECT id, parent, description, address, cad_number FROM facilitys";
    QSqlQuery query( db );
    bool res = query.exec( select );
    if( res )
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( 0 ).toLongLong();
            uint64_t parent_id = query.value( 1 ).toLongLong();
            std::string descr = query.value( 2 ).toString().toUtf8().data();
            std::string addr = query.value( 3 ).toString().toUtf8().data();
            std::string cad_number = query.value( 4 ).toString().toUtf8().data();

            FacilityPtr fac = FacilityPtr( new Facility( id ));
            setParentForBaseLocation( fac, parent_id );
            fac->setDesription( descr );
            fac->setAddress( addr );
            fac->setCadastralNumber( cad_number );

            facilitys.push_back( fac );
        }

    loadCoordinate< FacilityPtr >( facilitys, "facilitys" );

    return facilitys;
}

std::vector<FloorPtr> SqliteTranslator::loadFloors()
{
    std::vector< FloorPtr > floors;

    QSqlDatabase db = QSqlDatabase::database();
    QString select = "SELECT id, parent, number, name FROM floors;";
    QSqlQuery query( db );
    bool res = query.exec( select );
    if( res )
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( 0 ).toLongLong();
            uint64_t parent_id = query.value( 1 ).toLongLong();
            uint16_t number = query.value( 2 ).toInt();
            std::string name = query.value( 3 ).toString().toUtf8().data();

            FloorPtr flo = FloorPtr( new Floor( id ));
            setParentForBaseLocation( flo, parent_id );
            flo->setName( name );
            flo->setNumber( number );
            loadPlans( flo );

            floors.push_back( flo );
        }

    loadCoordinate< FloorPtr >( floors, "floors" );

    return floors;
}

std::vector<RoomsGroupPtr> SqliteTranslator::loadRoomsGroups()
{
    std::vector< RoomsGroupPtr > rooms_groups;

    QSqlDatabase db = QSqlDatabase::database();
    QString select = "SELECT id, parent, address, cad_number FROM rooms_groups";
    QSqlQuery query( db );
    bool res = query.exec( select );
    if( res )
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( 0 ).toLongLong();
            uint64_t parent_id = query.value( 1 ).toLongLong();
            std::string addr = query.value( 2 ).toString().toUtf8().data();
            std::string cad_number = query.value( 3 ).toString().toUtf8().data();

            RoomsGroupPtr rg = RoomsGroupPtr( new RoomsGroup( id ));
            setParentForBaseLocation( rg, parent_id );
            rg->setAddress( addr );
            rg->setCadastralNumber( cad_number );
            loadPlans( rg );

            rooms_groups.push_back( rg );
        }

    loadCoordinate< RoomsGroupPtr >( rooms_groups, "rooms_groups" );

    return rooms_groups;
}

std::vector<RoomPtr> SqliteTranslator::loadRooms()
{
    std::vector< RoomPtr > rooms;

    QSqlDatabase db = QSqlDatabase::database();
    QString select = "SELECT id, parent, name FROM rooms;";
    QSqlQuery query( db );
    bool res = query.exec( select );
    if( res )
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( 0 ).toLongLong();
            uint64_t parent_id = query.value( 1 ).toLongLong();
            std::string name = query.value( 2 ).toString().toUtf8().data();

            RoomPtr roo = RoomPtr( new Room( id ));
            setParentForBaseLocation( roo, parent_id );
            roo->setName( name );
            loadPlans( roo );

            rooms.push_back( roo );
        }

    loadCoordinate< RoomPtr >( rooms, "rooms" );

    return rooms;
}

template<typename LocTypePtr>
bool SqliteTranslator::loadCoordinate( std::vector< LocTypePtr > &vector,
                                       QString name )
{
    std::map< uint64_t, Coords > coords;

    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query( db );
    QString select_coords = "SELECT c.id, c.x, c.y, c.number FROM " + name + " as n JOIN coords as c "
                            "ON n.id = c.id ORDER by c.id, c.number";
    bool res = query.exec( select_coords );
    if( res )
    {
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( 0 ).toLongLong();
            double x = query.value( 1 ).toDouble();
            double y = query.value( 2 ).toDouble();

            coords[ id ].push_back( Coord( x, y ));
        }

        for( LocTypePtr loc: vector )
        {
            loc->setCoords( coords[loc->getId()] );
        }
    }

    return res;
}

bool SqliteTranslator::loadPlans( BaseAreaPtr area )
{
    std::shared_ptr< PlanKeeper > keeper = BaseArea::convert< PlanKeeper >( area );
    if( keeper )
    {
        QSqlDatabase db = QSqlDatabase::database();
        QSqlQuery query( db );
        QString select_plans = "SELECT parent, path, scale_w, scale_h, angle, x, y FROM plans "
                               "WHERE parent = " + QString::number( area->getId() );
        bool res = query.exec( select_plans );
        if( res )
        {
            for( query.first(); query.isValid(); query.next() )
            {
                std::string path = query.value( 1 ).toString().toUtf8().data();

                PlanKeeper::PlanParams params;
                params.scale_w = query.value( 2 ).toDouble();
                params.scale_h = query.value( 3 ).toDouble();
                params.rotate = query.value( 4 ).toDouble();
                params.x = query.value( 5 ).toDouble();
                params.y = query.value( 6 ).toDouble();

                keeper->setPlanParams( params );
                keeper->setPlanPath( path );
            }
        }
        else
            return false;

        return true;
    }

    return false;
}
