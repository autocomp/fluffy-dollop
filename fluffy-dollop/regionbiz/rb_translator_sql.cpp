#include "rb_translator_sql.h"

#include <iostream>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QPolygonF>
#include <QDebug>
#include <QSqlDriver>

#include "rb_manager.h"

using namespace regionbiz;

// register translator
REGISTER_TRANSLATOR(PsqlTranslator)
REGISTER_TRANSLATOR(SqliteTranslator)

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

    // marks
    _load_marks = std::bind( &SqlTranslator::loadMarks, this );
    _commit_mark = std::bind( &SqlTranslator::commitMark, this, std::placeholders::_1 );
    _delete_mark = std::bind( &SqlTranslator::deleteMark, this, std::placeholders::_1 );
}

std::vector< RegionPtr > SqlTranslator::loadRegions()
{
    return loadBaseAreas< Region >( "regions" );
}

std::vector<LocationPtr> SqlTranslator::loadLocations()
{
    return loadBaseAreas< Location >( "locations" );
}

std::vector<FacilityPtr> SqlTranslator::loadFacilitys()
{
    return loadBaseAreas< Facility >( "facilitys" );
}

std::vector<FloorPtr> SqlTranslator::loadFloors()
{
    return loadBaseAreas< Floor >( "floors" );
}

std::vector<RoomsGroupPtr> SqlTranslator::loadRoomsGroups()
{
    return loadBaseAreas< RoomsGroup >( "rooms_groups" );
}

std::vector<RoomPtr> SqlTranslator::loadRooms()
{
    return loadBaseAreas< Room >( "rooms" );
}

#define tryQuery( string ) \
    if( !query.exec( string )) \
    { \
        db.rollback(); \
        return false; \
    } \

bool SqlTranslator::deleteArea(BaseAreaPtr area)
{
    // find a type
    QString type = getStringType( area->getType() );
    if( type.isEmpty() )
        return false;

    // lock database
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    db.transaction();
    QSqlQuery query( db );

    // delete all data
    QString delete_metadata = "DELETE FROM metadate "
                              "WHERE entity_id = " + QString::number( area->getId() );
    tryQuery( delete_metadata );

    QString delete_coords = "DELETE FROM coords "
                            "WHERE id = " + QString::number( area->getId() );
    tryQuery( delete_coords );

    QString delete_area = "DELETE FROM " + type +
                          " WHERE id = " + QString::number( area->getId() );
    tryQuery( delete_area );

    // TODO delete relations

    // unlock
    db.commit();
    return true;
}

bool SqlTranslator::commitArea( BaseAreaPtr area )
{
    // find a type
    QString type = getStringType( area->getType() );
    if( type.isEmpty() )
        return false;

    // lock base
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    db.transaction();

    // check exist
    QString select = "SELECT id, parent, name, description FROM " + type +
                     " WHERE id = " + QString::number( area->getId() );
    QSqlQuery query( db );
    query.setForwardOnly( true );
    query.exec( select );

    QString insert_update;

    // room
    if( query.first() )
    {
        // update
        insert_update = "UPDATE " + type +
                        " SET id = ?, parent = ?, name = ?, description = ? "
                        "WHERE id = " + QString::number( area->getId() );
    }
    else
    {
        // insert
        insert_update = "INSERT INTO " + type + " (id, parent, name, description ) "
                        "VALUES (?, ?, ?, ?);";
    }

    query.prepare( insert_update );
    query.addBindValue( (qulonglong) area->getId() );
    query.addBindValue( (qulonglong) area->getParentId() );
    query.addBindValue( area->getName() );
    query.addBindValue( area->getDescription() );
    tryQuery()

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
    QString select = "SELECT id, x, y, parent_id, name, description FROM marks";
    QSqlQuery query( db );
    bool res = query.exec( select );
    if( res )
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( 0 ).toLongLong();
            double x = query.value( 1 ).toDouble();
            double y = query.value( 2 ).toDouble();
            uint64_t parent_id = query.value( 3 ).toLongLong();
            QString name = query.value( 4 ).toString();
            QString descr = query.value( 5 ).toString();

            MarkPtr mark = BaseEntity::createWithId< Mark >( id );
            mark->setCenter( QPointF( x, y ));
            mark->setParentId( parent_id );
            mark->setName( name );
            mark->setDesription( descr );

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
    tryQuery( delete_mark );

    QString insert_update = "INSERT INTO marks( id, x, y, parent_id, name, description ) "
                            "VALUES (?, ?, ?, ?, ?, ?)";
    query.prepare( insert_update );
    query.addBindValue( (qulonglong) mark->getId() );
    query.addBindValue( mark->getCenter().x() );
    query.addBindValue( mark->getCenter().y() );
    query.addBindValue( (qulonglong) mark->getParentId() );
    query.addBindValue( mark->getName() );
    query.addBindValue( mark->getDescription() );
    tryQuery();

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
    tryQuery( delete_mark );

    // metadate
    QString delete_coords = "DELETE FROM metadate "
                            "WHERE entity_id = " + QString::number( mark->getId() );
    tryQuery( delete_coords );

    // unlock base
    db.commit();
    return true;
}

bool SqlTranslator::commitCoordinates(BaseAreaPtr area)
{
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QSqlQuery query( db );
    query.setForwardOnly( true );
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

template<typename LocType>
std::vector< std::shared_ptr< LocType >> SqlTranslator::loadBaseAreas( QString type_name )
{
    typedef std::shared_ptr< LocType > LocTypePtr;
    std::vector< LocTypePtr > areas;

    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QString select;
    // region hasn't parent
    if( typeid( LocType ) == typeid( Region ))
        select = "SELECT id, name, description FROM " + type_name;
    else
        select = "SELECT id, name, description, parent FROM " + type_name;
    QSqlQuery query( db );

    bool plan_keeper = false;

    // boost speed
    query.setForwardOnly( true );
    bool res = query.exec( select );
    if( res )
    {
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( "id" ).toLongLong();
            uint64_t parent_id = 0;
            if( query.record().contains( "parent" ))
                parent_id = query.value( "parent" ).toLongLong();
            QString name = query.value( "name" ).toString();
            QString descr = query.value( "description" ).toString();

            LocTypePtr area_ptr = BaseEntity::createWithId< LocType >( id );
            if( !area_ptr )
                continue;

            setParentForBaseLocation( area_ptr, parent_id );
            area_ptr->setName( name );
            area_ptr->setDesription( descr );

            PlanKeeperPtr plan_keeper_ptr = BaseArea::convert< PlanKeeper >( area_ptr );
            if( plan_keeper_ptr )
                plan_keeper = true;

            areas.push_back( area_ptr );
        }
    }

    if( plan_keeper )
        loadPlans< LocTypePtr >( areas );
    loadCoordinate< LocTypePtr >( areas, type_name );

    return areas;
}

template<typename LocTypePtr>
bool SqlTranslator::loadCoordinate( std::vector< LocTypePtr > &vector,
                                       QString name )
{
    std::map< uint64_t, QPolygonF > coords;

    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QSqlQuery query( db );
    QString select_coords = "SELECT c.id, c.x, c.y, c.number FROM " + name + " as n JOIN coords as c "
                            "ON n.id = c.id ORDER by c.id, c.number";
    // boost speed of query
    query.setForwardOnly( true );
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

template< typename LocTypePtr >
bool SqlTranslator::loadPlans( std::vector< LocTypePtr >& areas )
{
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QSqlQuery query( db );
    // boost speed
    query.setForwardOnly( true );
    QString select_plans = "SELECT parent, path, scale_w, scale_h, angle, x, y FROM plans ";
    bool res = query.exec( select_plans );
    if( res )
    {
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t parent_id = query.value( 0 ).toLongLong();
            std::function< bool( BaseAreaPtr ) > check_id =
                    [ parent_id ]( BaseAreaPtr ba ){ return parent_id == ba->getId(); };

            BaseAreaPtr area;
            auto iter = FIND_IF( areas, check_id );
            if( iter != areas.end() )
                area = *iter;

            std::shared_ptr< PlanKeeper > keeper = BaseArea::convert< PlanKeeper >( area );
            if( keeper )
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
    }
    else
        return false;

    return true;
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

QString SqlTranslator::getStringType(BaseArea::AreaType area_type)
{
    QString type = "";
    switch ( area_type ) {
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

    return type;
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

QString SqliteTranslator::getTranslatorName()
{
    return "sqlite";
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

QString PsqlTranslator::getTranslatorName()
{
    return "psql";
}

QString PsqlTranslator::getBaseName()
{
    return "REGION_BIZ_PSQL";
}
