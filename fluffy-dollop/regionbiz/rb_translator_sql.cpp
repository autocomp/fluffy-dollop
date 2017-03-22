#include "rb_translator_sql.h"

#include <iostream>
#include <QSqlQuery>
#include <QSqlError>
#include <QPolygonF>
#include <QDebug>

using namespace regionbiz;

void SqlTranslator::loadFunctions()
{
    // locations
    _load_regions = std::bind( &SqlTranslator::loadRegions, this );
    _load_locations = std::bind( &SqlTranslator::loadLocations, this );
    _load_facilitys = std::bind( &SqlTranslator::loadFacilitys, this );
    _load_floors = std::bind( &SqlTranslator::loadFloors, this );
    _load_rooms_groups = std::bind( &SqlTranslator::loadRoomsGroups, this );
    _load_rooms = std::bind( &SqlTranslator::loadRooms, this );

    // delete locations
    _delete_area = std::bind( &SqlTranslator::deleteArea, this, std::placeholders::_1 );

    // commit locations
    _commit_area = std::bind( &SqlTranslator::commitArea, this, std::placeholders::_1 );

    // relations
    _load_propertys = std::bind( &SqlTranslator::loadPropertys, this );
    _load_rents = std::bind( &SqlTranslator::loadRents, this );

    // metadata
    _load_metadata = std::bind( &SqlTranslator::loadMetadata, this );
    _commit_mark = std::bind( &SqlTranslator::commitMark, this, std::placeholders::_1 );
    _delete_mark = std::bind( &SqlTranslator::deleteMark, this, std::placeholders::_1 );
}

std::vector< RegionPtr > SqlTranslator::loadRegions()
{
    std::vector< RegionPtr > regions;

    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QString select = "SELECT id, description FROM regions";
    QSqlQuery query( db );
    bool res = query.exec( select );
    if( res )
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( 0 ).toLongLong();
            QString descr = query.value( 1 ).toString();

            RegionPtr reg = BaseEntity::createWithId< Region >( id );
            reg->setDesription( descr );
            setParentForBaseLocation( reg, 0 );

            regions.push_back( reg );
        }

    loadCoordinate< RegionPtr >( regions, "regions" );

    return regions;
}

std::vector<LocationPtr> SqlTranslator::loadLocations()
{
    std::vector< LocationPtr > locations;

    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QString select = "SELECT id, parent, description, address FROM locations";
    QSqlQuery query( db );
    bool res = query.exec( select );
    if( res )
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( 0 ).toLongLong();
            uint64_t parent_id = query.value( 1 ).toLongLong();
            QString descr = query.value( 2 ).toString();
            QString addr = query.value( 3 ).toString();

            LocationPtr loc = BaseEntity::createWithId< Location >( id );
            setParentForBaseLocation( loc, parent_id );
            loc->setDesription( descr );
            loc->setAddress( addr );
            loadPlans( loc );

            locations.push_back( loc );
        }

    loadCoordinate< LocationPtr >( locations, "locations" );

    return locations;
}

std::vector<FacilityPtr> SqlTranslator::loadFacilitys()
{
    std::vector< FacilityPtr > facilitys;

    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QString select = "SELECT id, parent, description, address, cad_number FROM facilitys";
    QSqlQuery query( db );
    bool res = query.exec( select );
    if( res )
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( 0 ).toLongLong();
            uint64_t parent_id = query.value( 1 ).toLongLong();
            QString descr = query.value( 2 ).toString();
            QString addr = query.value( 3 ).toString();
            QString cad_number = query.value( 4 ).toString();

            FacilityPtr fac = BaseEntity::createWithId< Facility >( id );
            setParentForBaseLocation( fac, parent_id );
            fac->setDesription( descr );
            fac->setAddress( addr );
            fac->setCadastralNumber( cad_number );

            facilitys.push_back( fac );
        }

    loadCoordinate< FacilityPtr >( facilitys, "facilitys" );

    return facilitys;
}

std::vector<FloorPtr> SqlTranslator::loadFloors()
{
    std::vector< FloorPtr > floors;

    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QString select = "SELECT id, parent, number, name FROM floors;";
    QSqlQuery query( db );
    bool res = query.exec( select );
    if( res )
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( 0 ).toLongLong();
            uint64_t parent_id = query.value( 1 ).toLongLong();
            uint16_t number = query.value( 2 ).toInt();
            QString name = query.value( 3 ).toString();

            FloorPtr flo = BaseEntity::createWithId< Floor >( id );
            setParentForBaseLocation( flo, parent_id );
            flo->setName( name );
            flo->setNumber( number );
            loadPlans( flo );

            floors.push_back( flo );
        }

    loadCoordinate< FloorPtr >( floors, "floors" );

    return floors;
}

std::vector<RoomsGroupPtr> SqlTranslator::loadRoomsGroups()
{
    std::vector< RoomsGroupPtr > rooms_groups;

    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QString select = "SELECT id, parent, address, cad_number FROM rooms_groups";
    QSqlQuery query( db );
    bool res = query.exec( select );
    if( res )
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( 0 ).toLongLong();
            uint64_t parent_id = query.value( 1 ).toLongLong();
            QString addr = query.value( 2 ).toString();
            QString cad_number = query.value( 3 ).toString();

            RoomsGroupPtr rg = BaseEntity::createWithId< RoomsGroup >( id );
            setParentForBaseLocation( rg, parent_id );
            rg->setAddress( addr );
            rg->setCadastralNumber( cad_number );
            loadPlans( rg );

            rooms_groups.push_back( rg );
        }

    loadCoordinate< RoomsGroupPtr >( rooms_groups, "rooms_groups" );

    return rooms_groups;
}

std::vector<RoomPtr> SqlTranslator::loadRooms()
{
    std::vector< RoomPtr > rooms;

    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QString select = "SELECT id, parent, name FROM rooms;";
    QSqlQuery query( db );
    bool res = query.exec( select );
    if( res )
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( 0 ).toLongLong();
            uint64_t parent_id = query.value( 1 ).toLongLong();
            QString name = query.value( 2 ).toString();

            RoomPtr roo = BaseEntity::createWithId< Room >( id );
            setParentForBaseLocation( roo, parent_id );
            roo->setName( name );
            loadPlans( roo );

            rooms.push_back( roo );
        }

    loadCoordinate< RoomPtr >( rooms, "rooms" );

    return rooms;
}

bool SqlTranslator::deleteArea(BaseAreaPtr area)
{
    // find a type
    QString type = "";
    switch ( area->getType() ) {
    case BaseArea::AT_REGION:
        type = "regions";
        break;
    case BaseArea::AT_LOCATION:
        type = "locations";
        break;
    case BaseArea::AT_FACILITY:
        type = "facilitys";
        break;
    case BaseArea::AT_FLOOR:
        type = "floors";
        break;
    case BaseArea::AT_ROOMS_GROUP:
        type = "rooms_groups";
        break;
    case BaseArea::AT_ROOM:
        type = "rooms";
        break;
    }
    if( type.isEmpty() )
        return false;

    // lock database
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    db.transaction();
    QSqlQuery query( db );

    // delete all data
    QString delete_metadata = "DELETE FROM metadate "
                              "WHERE entity_id = " + QString::number( area->getId() );
    if( !query.exec( delete_metadata ))
    {
        db.rollback();
        return false;
    }

    QString delete_coords = "DELETE FROM coords "
                            "WHERE id = " + QString::number( area->getId() );
    if( !query.exec( delete_coords ))
    {
        db.rollback();
        return false;
    }

    QString delete_area = "DELETE FROM " + type +
                          " WHERE id = " + QString::number( area->getId() );
    if( !query.exec( delete_area ))
    {
        db.rollback();
        return false;
    }

    // TODO delete relations

    // unlock
    db.commit();
    return true;
}

bool SqlTranslator::commitArea( BaseAreaPtr area )
{
    // FIXME all area types
    if( BaseArea::AT_ROOM != area->getType() )
        return false;

    // check exist
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QString select = "SELECT id, parent, name FROM rooms "
                     "WHERE id = " + QString::number( area->getId() );
    QSqlQuery query( db );
    query.exec( select );

    QString insert_update;

    // lock base
    db.transaction();

    // room
    if( query.first() )
    {
        // update
        insert_update = "UPDATE rooms SET id = ?, parent = ?, name = ? "
                        "WHERE id = " + QString::number( area->getId() );
    }
    else
    {
        // insert
        insert_update = "INSERT INTO rooms (id, parent, name) VALUES (?, ?, ?);";
    }

    query.prepare( insert_update );
    query.addBindValue( (qulonglong) area->getId() );
    query.addBindValue( (qulonglong) area->getParentId() );
    query.addBindValue( BaseArea::convert< Room >( area )->getName() );
    if( !query.exec() )
    {
        db.rollback();
        return false;
    }

    // coordinates
    if( !commitCoordinates( area ))
    {
        db.rollback();
        return false;
    }

    // commit metadate
    if( !commitMetadate( area ))
    {
        db.rollback();
        return false;
    }

    // unlock base
    db.commit();
    return true;
}

//------------------------------------------------------

std::vector<PropertyPtr> SqlTranslator::loadPropertys()
{
    std::vector<PropertyPtr> propertys;

    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QString select = "SELECT id, area_id, register_date, encumbrances FROM propertys";
    QSqlQuery query( db );
    bool res = query.exec( select );
    if( res )
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( 0 ).toLongLong();
            uint64_t area_id = query.value( 1 ).toLongLong();
            QDate date_register = query.value( 2 ).toDate();

            // TODO create by base entity
            PropertyPtr prop = PropertyPtr( new Property( id ));
            setAreaForBaseRalation( prop, area_id );
            prop->setDateOfRegistration( date_register );

            loadDocuments( prop );

            propertys.push_back( prop );
        }

    return propertys;
}

std::vector<RentPtr> SqlTranslator::loadRents()
{
    std::vector<RentPtr> rents;

    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QString select = "SELECT id, area_id, start_date, finish_date FROM rents";
    QSqlQuery query( db );
    bool res = query.exec( select );
    if( res )
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( 0 ).toLongLong();
            uint64_t area_id = query.value( 1 ).toLongLong();
            QDate date_start = query.value( 2 ).toDate();
            QDate date_finish = query.value( 2 ).toDate();

            // TODO create by base entity
            RentPtr rent = RentPtr( new Rent( id ));
            setAreaForBaseRalation( rent, area_id );
            rent->setDateOfStart( date_start );
            rent->setDateOfFinish( date_finish );

            loadDocuments( rent );
            loadPayments( rent );

            rents.push_back( rent );
        }

    return rents;
}

BaseMetadataPtrs SqlTranslator::loadMetadata()
{
    BaseMetadataPtrs metadata;

    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QString select = "SELECT entity_id, type, name, value FROM metadate";
    QSqlQuery query( db );
    bool res = query.exec( select );
    if( res )
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t area_id = query.value( 0 ).toLongLong();
            QString type = query.value( 1 ).toString();
            QString name = query.value( 2 ).toString();
            QString value_str = query.value( 3 ).toString();

            BaseMetadataPtr data = MetadataFabric::createMetadata( type, area_id );
            data->setValueByString( value_str );
            data->setName( name );

            metadata.push_back( data );
        }

    return metadata;
}

MarkPtrs SqlTranslator::loadMarks()
{
    MarkPtrs marks;

    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QString select = "SELECT id, x, y, parent_id FROM marks";
    QSqlQuery query( db );
    bool res = query.exec( select );
    if( res )
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( 0 ).toLongLong();
            double x = query.value( 1 ).toDouble();
            double y = query.value( 2 ).toDouble();
            uint64_t parent_id = query.value( 3 ).toLongLong();

            MarkPtr mark = BaseEntity::createWithId< Mark >( id );
            mark->setCenter( QPointF( x, y ));
            mark->setParentId( parent_id );

            marks.push_back( mark );
        }

    return marks;
}

bool SqlTranslator::commitMark( MarkPtr mark )
{
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    // lock base
    db.transaction();

    // update mark
    QString delete_mark = "DELETE FROM marks "
                          "WHERE id = " + QString::number( mark->getId() );
    QSqlQuery query( db );
    bool res = query.exec( delete_mark );
    if( !res )
    {
        db.rollback();
        return false;
    }

    QString insert_update = "INSERT INTO marks( id, x, y, parent_id ) VALUES (?, ?, ?, ?)";
    query.prepare( insert_update );
    query.addBindValue( (qulonglong) mark->getId() );
    query.addBindValue( mark->getCenter().x() );
    query.addBindValue( mark->getCenter().y() );
    query.addBindValue( (qulonglong) mark->getParentId() );

    res = query.exec();
    if( !res )
    {
        db.rollback();
        return false;
    }

    // commit metadate
    if( !commitMetadate( mark ))
    {
        db.rollback();
        return false;
    }

    // unlock base
    db.commit();
    return true;
}

bool SqlTranslator::deleteMark( MarkPtr mark )
{
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    // lock base
    db.transaction();

    // update mark
    QString delete_mark = "DELETE FROM marks "
                          "WHERE id = " + QString::number( mark->getId() );
    QSqlQuery query( db );
    bool res = query.exec( delete_mark );
    if( !res )
    {
        db.rollback();
        return false;
    }

    // metadate
    QString delete_coords = "DELETE FROM metadate "
                            "WHERE entity_id = " + QString::number( mark->getId() );
    res = query.exec( delete_coords );
    if( !res )
    {
        db.rollback();
        return false;
    }

    // unlock base
    db.commit();
    return true;
}

bool SqlTranslator::commitCoordinates(BaseAreaPtr area)
{
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QSqlQuery query( db );
    QString delete_coords = "DELETE FROM coords "
                            "WHERE id = " + QString::number( area->getId() );
    if( !query.exec( delete_coords ))
        return false;

    QString insert_coords = "INSERT INTO coords ( id, x, y, number ) VALUES (?, ?, ?, ?)";
    query.prepare( insert_coords );
    // prepare data
    QVariantList ids, xs, ys, numbers;
    uint num = 0;
    for( QPointF pnt: area->getCoords() )
    {
        ids.push_back( (qulonglong) area->getId() );
        xs.push_back( pnt.x() );
        ys.push_back( pnt.y() );
        numbers.push_back( num++ );
    }
    // bind data
    query.addBindValue( ids );
    query.addBindValue( xs );
    query.addBindValue( ys );
    query.addBindValue( numbers );

    return query.execBatch();
}

bool SqlTranslator::commitMetadate(BaseEntityPtr area)
{
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QSqlQuery query( db );
    QString delete_coords = "DELETE FROM metadate "
                            "WHERE entity_id = " + QString::number( area->getId() );
    if( !query.exec( delete_coords ))
        return false;

    QString insert_coords = "INSERT INTO metadate ( entity_id, type, name, value ) "
                            "VALUES ( ?, ?, ?, ? );";
    query.prepare( insert_coords );
    // prepare data
    QVariantList ids, types, names, values;
    for( auto data_pair: area->getMetadataMap() )
    {
        BaseMetadataPtr data = data_pair.second;

        ids.push_back( (qulonglong) area->getId() );
        types.push_back( data->getType() );
        names.push_back( data->getName() );
        values.push_back( data->getValueAsString() );
    }
    // bind data
    query.addBindValue( ids );
    query.addBindValue( types );
    query.addBindValue( names );
    query.addBindValue( values );

    return query.execBatch();
}

//------------------------------------------------------

template<typename LocTypePtr>
bool SqlTranslator::loadCoordinate( std::vector< LocTypePtr > &vector,
                                       QString name )
{
    std::map< uint64_t, QPolygonF > coords;

    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
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

            coords[ id ].push_back( QPointF( x, y ));
        }

        for( LocTypePtr loc: vector )
        {
            loc->setCoords( coords[loc->getId()] );
        }
    }

    return res;
}

bool SqlTranslator::loadPlans( BaseAreaPtr area )
{
    std::shared_ptr< PlanKeeper > keeper = BaseArea::convert< PlanKeeper >( area );
    if( keeper )
    {
        QSqlDatabase db = QSqlDatabase::database( getBaseName() );
        QSqlQuery query( db );
        QString select_plans = "SELECT parent, path, scale_w, scale_h, angle, x, y FROM plans "
                               "WHERE parent = " + QString::number( area->getId() );
        bool res = query.exec( select_plans );
        if( res )
        {
            for( query.first(); query.isValid(); query.next() )
            {
                QString path = query.value( 1 ).toString();

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

bool SqlTranslator::loadDocuments( BaseBizRelationPtr /*relation*/ )
{
    // TODO load documents
    return true;
}

bool SqlTranslator::loadPayments(RentPtr /*rent*/)
{
    // TODO load payments
    return true;
}

bool SqliteTranslator::initBySettings(QVariantMap settings)
{
    QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE", getBaseName() );
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

QString SqliteTranslator::getBaseName()
{
    return "REGION_BIZ_SQLITE";
}

bool PsqlTranslator::initBySettings(QVariantMap settings)
{
    QSqlDatabase db = QSqlDatabase::addDatabase( "QPSQL", getBaseName() );
    QString host = settings["host"].toString();
    QString db_name = settings["db_name"].toString();
    QString user = settings["user"].toString();
    QString pass = settings["pass"].toString();

    db.setHostName( host );
    db.setDatabaseName( db_name );
    db.setUserName( user );
    db.setPassword( pass );

    if (db.open())
    {
        std::cout << "Database: connection ok" << std::endl;
        return true;
    }
    else
    {
        std::cerr << "Error: connection with database fail: "
                  << db.lastError().text().toUtf8().data() << std::endl;
        return false;
    }
}

QString PsqlTranslator::getBaseName()
{
    return "REGION_BIZ_PSQL";
}
