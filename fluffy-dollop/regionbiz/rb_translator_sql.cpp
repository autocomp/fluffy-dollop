#include "rb_translator_sql.h"

#include <iostream>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QPolygonF>
#include <QDebug>
#include <QSqlDriver>

#include "rb_manager.h"
#include "rb_files.h"

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

    // files
    _load_files = std::bind( &SqlTranslator::loadFiles, this );
}

std::vector< RegionPtr > SqlTranslator::loadRegions()
{
    return loadBaseAreas< Region >( "region" );
}

std::vector<LocationPtr> SqlTranslator::loadLocations()
{
    return loadBaseAreas< Location >( "location" );
}

std::vector<FacilityPtr> SqlTranslator::loadFacilitys()
{
    return loadBaseAreas< Facility >( "facility" );
}

std::vector<FloorPtr> SqlTranslator::loadFloors()
{
    return loadBaseAreas< Floor >( "floor" );
}

std::vector<RoomPtr> SqlTranslator::loadRooms()
{
    return loadBaseAreas< Room >( "room" );
}

#define tryQuery( string ) \
    if( !query.exec( string )) \
    { \
        qDebug() << query.lastError(); \
        db.rollback(); \
        return false; \
    } \

bool SqlTranslator::deleteArea(BaseAreaPtr area)
{
    // lock database
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    db.transaction();
    QSqlQuery query( db );

    // delete all data
    QString delete_metadata = "DELETE FROM metadata "
                              "WHERE entity_id = " + QString::number( area->getId() );
    tryQuery( delete_metadata );

    QString delete_coords = "DELETE FROM coords "
                            "WHERE id = " + QString::number( area->getId() );
    tryQuery( delete_coords );

    QString delete_entity = "DELETE FROM entitys "
                          " WHERE id = " + QString::number( area->getId() );
    tryQuery( delete_entity );

    QString delete_area = "DELETE FROM areas "
                          " WHERE id = " + QString::number( area->getId() );
    tryQuery( delete_area );

    // TODO delete relations
    // TODO delete files

    // unlock
    db.commit();
    return true;
}

bool SqlTranslator::commitArea( BaseAreaPtr area )
{
    // lock base
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    db.transaction();

    // check exist
    QString select = "SELECT id, parent_id, name, description FROM entitys "
                     " WHERE id = " + QString::number( area->getId() );
    QSqlQuery query( db );
    query.setForwardOnly( true );
    query.exec( select );
    bool has_area = query.first();

    QString insert_update;

    // room
    if( has_area )
    {
        // update
        insert_update = "UPDATE entitys "
                        "SET id = ?, parent_id = ?, name = ?, description = ? "
                        "WHERE id = " + QString::number( area->getId() );
    }
    else
    {
        // insert
        insert_update = "INSERT INTO entitys (id, parent_id, name, description) "
                        "VALUES (?, ?, ?, ?);";
    }

    query.prepare( insert_update );
    query.addBindValue( (qulonglong) area->getId() );
    query.addBindValue( (qulonglong) area->getParentId() );
    query.addBindValue( area->getName() );
    query.addBindValue( area->getDescription() );

    tryQuery();

    if( !has_area )
    {
        // find a type
        QString type = getStringType( area->getType() );
        if( type.isEmpty() )
            return false;

        QString insert_area = "INSERT INTO areas (id, type)"
                              "VALUES (?, ?)";
        query.prepare( insert_area );
        query.addBindValue( (qulonglong) area->getId() );
        query.addBindValue( type );
        tryQuery()
    }

    // coordinates
    if( !commitCoordinates( area ))
    {
        db.rollback();
        return false;
    }

    // commit metadata
    if( !commitMetadate( area ))
    {
        db.rollback();
        return false;
    }

    // commit files
    if( !commitFiles( area ))
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
    QString select = "SELECT entity_id, type, name, value FROM metadata";
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
            if( !mark )
                continue;
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

    // commit metadata
    if( !commitMetadate( mark ))
    {
        db.rollback();
        return false;
    }

    // commit files
    if( !commitFiles( mark ))
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

    // metadata
    QString delete_coords = "DELETE FROM metadata "
                            "WHERE entity_id = " + QString::number( mark->getId() );
    tryQuery( delete_coords );

    // TODO delete files

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
    QString delete_coords = "DELETE FROM metadata "
                            "WHERE entity_id = " + QString::number( area->getId() );
    if( !query.exec( delete_coords ))
        return false;

    QString insert_coords = "INSERT INTO metadata ( entity_id, type, name, value ) "
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

bool SqlTranslator::commitFiles(BaseEntityPtr entity)
{
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QSqlQuery query( db );
    QString delete_files = "DELETE FROM files "
                           "WHERE entity_id = " + QString::number( entity->getId() );
    if( !query.exec( delete_files ))
        return false;

    // insert all files
    QString insert_files = "INSERT INTO files( entity_id, path, type ) VALUES ( ?, ?, ? );";
    query.prepare( insert_files );
    // prepare data
    QVariantList ids, paths, types;
    for( auto file: entity->getFiles() )
    {
        ids.push_back( (qulonglong) entity->getId() );
        paths.push_back( file->getPath() );
        types.push_back( getStringFileType( file->getType() ));
    }
    // bind data
    query.addBindValue( ids );
    query.addBindValue( paths );
    query.addBindValue( types );

    if ( !query.execBatch() )
        return false;

    // insert plans
    QString insert_plans = "INSERT INTO plans( path, scale_w, scale_h, angle, x, y) VALUES ( ?, ?, ?, ?, ?, ? );";
    query.prepare( insert_plans );
    // prepare data
    QVariantList plan_paths, scales_w, scales_h,
            angles, xs, ys;
    for( auto file: entity->getFiles() )
    {
        PlanFileKeeperPtr plan = BaseFileKeeper::convert< PlanFileKeeper >( file );
        if( !plan )
            continue;

        PlanFileKeeper::PlanParams params = plan->getPlanParams();
        plan_paths.push_back( file->getPath() );
        scales_w.push_back( params.scale_w );
        scales_h.push_back( params.scale_h );
        angles.push_back( params.rotate );
        xs.push_back( params.x );
        ys.push_back( params.y );
    }
    // bind data
    query.addBindValue( plan_paths );
    query.addBindValue( scales_w );
    query.addBindValue( scales_h );
    query.addBindValue( angles );
    query.addBindValue( xs );
    query.addBindValue( ys );

    return query.execBatch();
}

//------------------------------------------------------

template<typename LocType>
std::vector< std::shared_ptr< LocType >> SqlTranslator::loadBaseAreas( QString type_name )
{
    typedef std::shared_ptr< LocType > LocTypePtr;
    std::vector< LocTypePtr > areas;

    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QString select = "SELECT name, e.id, description, parent_id "
                     "FROM entitys as e JOIN areas as a "
                     "on (a.id = e.id AND a.type = \'" + type_name + "\')";

    QSqlQuery query( db );

    // boost speed
    query.setForwardOnly( true );
    bool res = query.exec( select );
    if( res )
    {
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( "id" ).toLongLong();
            uint64_t parent_id = query.value( "parent_id" ).toLongLong();
            QString name = query.value( "name" ).toString();
            QString descr = query.value( "description" ).toString();

            LocTypePtr area_ptr = BaseEntity::createWithId< LocType >( id );
            if( !area_ptr )
                continue;

            setParentForBaseLocation( area_ptr, parent_id );
            area_ptr->setName( name );
            area_ptr->setDesription( descr );

            areas.push_back( area_ptr );
        }
    }

    loadCoordinate< LocTypePtr >( areas );

    return areas;
}

template<typename LocTypePtr>
bool SqlTranslator::loadCoordinate( std::vector< LocTypePtr > &vector )
{
    std::map< uint64_t, QPolygonF > coords;

    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QSqlQuery query( db );
    QString select_coords = "SELECT c.id, c.x, c.y, c.number FROM entitys as e JOIN coords as c "
                            "ON e.id = c.id ORDER by c.id, c.number";
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

BaseFileKeeperPtrs SqlTranslator::loadFiles()
{
    BaseFileKeeperPtrs files;

    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QSqlQuery query( db );
    // boost speed
    query.setForwardOnly( true );

    std::map< QString, PlanFileKeeper::PlanParams > plan_params;
    QString select_plans = "SELECT path, scale_w, scale_h, angle, x, y FROM plans ";
    bool res_plans = query.exec( select_plans );
    if( res_plans )
    {
        for( query.first(); query.isValid(); query.next() )
        {
            QString path = query.value( 0 ).toString();

            PlanFileKeeper::PlanParams params;
            params.scale_w = query.value( 1 ).toDouble();
            params.scale_h = query.value( 2 ).toDouble();
            params.rotate = query.value( 3 ).toDouble();
            params.x = query.value( 4 ).toDouble();
            params.y = query.value( 5 ).toDouble();

            plan_params[ path ] = params;
        }
    }

    QString select_files = "SELECT entity_id, path, type FROM files";
    bool res = query.exec( select_files );
    if( res )
    {
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t parent_id = query.value( 0 ).toLongLong();
            QString path = query.value( 1 ).toString();
            QString type_str = query.value( 2 ).toString();
            BaseFileKeeper::FileType type = getFileTypeByString( type_str );

            BaseFileKeeperPtr file_ptr = FileKeeperFabric::createFile( path, parent_id, type );

            if( BaseFileKeeper::FT_PLAN == type )
            {
                auto plan = BaseFileKeeper::convert< PlanFileKeeper >( file_ptr );
                if( !res_plans
                        || !plan )
                    continue;

                plan->setPlanParams( plan_params[ path ] );
            }

            files.push_back( file_ptr );
        }
    }

    return files;
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
        type = "region";
        break;
    case BaseArea::AT_LOCATION:
        type = "location";
        break;
    case BaseArea::AT_FACILITY:
        type = "facility";
        break;
    case BaseArea::AT_FLOOR:
        type = "floor";
        break;
    case BaseArea::AT_ROOM:
        type = "room";
        break;
    }

    return type;
}

QString SqlTranslator::getStringFileType(BaseFileKeeper::FileType type)
{
    QString type_str;
    switch( type )
    {
    case BaseFileKeeper::FT_PLAN:
    {
        type_str = "plan";
        break;
    }

    case BaseFileKeeper::FT_DOCUMENT:
    {
        type_str = "document";
        break;
    }

    case BaseFileKeeper::FT_IMAGE:
    {
        type_str = "image";
        break;
    }
    case BaseFileKeeper::FT_NONE:
        break;
    }

    return type_str;
}

BaseFileKeeper::FileType SqlTranslator::getFileTypeByString(QString type)
{
    if( "plan" == type )
        return BaseFileKeeper::FT_PLAN;
    if( "document" == type )
        return BaseFileKeeper::FT_DOCUMENT;
    if( "image" == type )
        return BaseFileKeeper::FT_IMAGE;

    return BaseFileKeeper::FT_NONE;
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
