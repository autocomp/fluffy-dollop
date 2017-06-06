#include "rbp_translator_sql.h"

#include <iostream>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QPolygonF>
#include <QDebug>
#include <QSqlDriver>

#include <regionbiz/rb_manager.h>
#include <regionbiz/rb_files.h>

using namespace regionbiz;

// register translator
REGISTER_TRANSLATOR(PsqlTranslator)
REGISTER_TRANSLATOR(SqliteTranslator)

SqlTranslator::SqlTranslator()
{
    // start thread
    _self_thread = new QThread();
    _thread_sql.moveToThread( _self_thread );
    _self_thread->start();
}

SqlTranslator::~SqlTranslator()
{
    // stop thread
    _self_thread->quit();
    _self_thread->wait();
    delete _self_thread;
}

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

    // groups
    _load_groups = std::bind( &SqlTranslator::loadGroups, this );
    _commit_groups = std::bind( &SqlTranslator::commitGroups, this, std::placeholders::_1 );
    _delete_group = std::bind( &SqlTranslator::deleteGroup, this, std::placeholders::_1 );
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
    // WARNING test multithread commit
    _thread_sql.appendCommand( ThreadSql::C_DELETE_AREA, area->getId() );
    return true;
}

bool SqlTranslator::commitArea( BaseAreaPtr area )
{
    // WARNING test multithread commit
    _thread_sql.appendCommand( ThreadSql::C_COMMIT_AREA, area->getId() );
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
    std::map< uint64_t, QPolygonF > coords;

    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    // lock
    db.transaction();

    QSqlQuery query( db );
    // select coords of marks
    QString select_coords = "SELECT c.id, c.x, c.y, c.number FROM marks as m JOIN coords as c "
                            "ON m.id = c.id ORDER by c.id, c.number";
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
    }

    // select marks
    QString select = "SELECT e.id, e.parent_id, e.name, e.description, m.type "
                     "FROM entitys as e JOIN marks as m ON (e.id = m.id)";
    res = query.exec( select );
    if( res )
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( 0 ).toLongLong();
            uint64_t parent_id = query.value( 1 ).toLongLong();
            QString name = query.value( 2 ).toString();
            QString descr = query.value( 3 ).toString();
            QString type = query.value( 4 ).toString();

            MarkPtr mark = getMarkByTypeString( type, id );
            if( !mark )
                continue;

            mark->setParentId( parent_id );
            mark->setName( name );
            mark->setDesription( descr );
            mark->setCoords( coords[ id ] );

            marks.push_back( mark );
        }

    // unlock
    db.commit();
    return marks;
}

bool SqlTranslator::commitMark( MarkPtr mark )
{
    // WARNING test multithread commit
    _thread_sql.appendCommand( ThreadSql::C_COMMIT_MARK, mark->getId() );
    return true;
}

bool SqlTranslator::deleteMark( MarkPtr mark )
{
    // WARNING test multithread commit
    _thread_sql.appendCommand( ThreadSql::C_DELETE_MARK, mark->getId() );
    return true;
}

GroupEntityPtrs SqlTranslator::loadGroups()
{
    // lock database
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );

    // lock
    db.transaction();
    QSqlQuery query( db );

    // select group like entitys
    QString select_groups = "SELECT id FROM entitys WHERE id IN "
                            "( SELECT DISTINCT group_id FROM groups )";
    query.setForwardOnly( true );
    bool res = query.exec( select_groups );
    std::map< uint64_t, GroupEntityPtr > groups_by_id;
    if( res )
    {
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id = query.value( 0 ).toLongLong();
            groups_by_id[ id ] = BaseEntity::createWithId< GroupEntity >( id );
        }
    }

    // select all elements
    QString select_groups_and_elements = "SELECT group_id, element_id FROM groups";
    query.setForwardOnly( true );
    res = query.exec( select_groups_and_elements );
    if( res )
    {
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t id_group = query.value( 0 ).toLongLong();
            uint64_t id_element = query.value( 1 ).toLongLong();

            // check group existence
            if( groups_by_id.find( id_group ) != groups_by_id.end() )
            {
                // add element to group
                groups_by_id[ id_group ]->addElement( id_element );
            }
            else
                std::cerr << "Group " << id_group
                          << " doesn't exist" << std::endl;
        }
    }

    // clear list of changed groups for commit
    freeChangedGroups();

    // make vector of groups
    GroupEntityPtrs groups;
    for( auto pair: groups_by_id )
    {
        GroupEntityPtr group = pair.second;
        if( group->getElements().size() )
            groups.push_back( group );
        else
            std::cerr << "Empty group finded: "
                      << group->getId() << std::endl;
    }

    // unlock
    db.commit();
    return groups;
}

bool SqlTranslator::commitGroups( GroupEntityPtrs groups )
{
    // WARNING test multithread commit
    for( GroupEntityPtr group: groups )
        if( group->getCount() )
            _thread_sql.appendCommand( ThreadSql::C_COMMIT_GROUP,
                                       group->getId() );

    return true;
}

bool SqlTranslator::deleteGroup(GroupEntityPtr group)
{
    // WARNING test multithread commit
    _thread_sql.appendCommand( ThreadSql::C_DELETE_GROUP, group->getId() );
    return true;
}

//------------------------------------------------------

template<typename LocType>
std::vector< std::shared_ptr< LocType >> SqlTranslator::loadBaseAreas( QString type_name )
{
    typedef std::shared_ptr< LocType > LocTypePtr;
    std::vector< LocTypePtr > areas;

    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    // lock
    db.transaction();

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

    // unlock
    db.commit();
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
    // lock
    db.transaction();

    QSqlQuery query( db );
    // boost speed
    query.setForwardOnly( true );

    std::map< QString, PlanFileKeeper::PlanParams > plan_params;
    QString select_plans = "SELECT path, scale_w, scale_h, angle, x, y, opacity FROM plans";
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
            params.opacity = query.value( 6 ).toDouble();

            plan_params[ path ] = params;
        }
    }

    QString select_files = "SELECT entity_id, path, type, name FROM files";
    bool res = query.exec( select_files );
    if( res )
    {
        for( query.first(); query.isValid(); query.next() )
        {
            uint64_t parent_id = query.value( 0 ).toLongLong();
            QString path = query.value( 1 ).toString();
            QString type_str = query.value( 2 ).toString();
            QString name = query.value( 3 ).toString();
            BaseFileKeeper::FileType type = getFileTypeByString( type_str );

            BaseFileKeeperPtr file_ptr = FileKeeperFabric::createFile( path, parent_id, type );
            file_ptr->setName( name );

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

    // unlock
    db.commit();
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

MarkPtr SqlTranslator::getMarkByTypeString(QString type, uint64_t id)
{
    if( "defect" == type )
        return BaseEntity::createWithId< DefectMark >( id );
    if( "photo" == type )
        return BaseEntity::createWithId< PhotoMark >( id );
    if( "photo_3d" == type )
        return BaseEntity::createWithId< Photo3dMark >( id );

    return nullptr;
}

bool SqliteTranslator::initBySettings(QVariantMap settings)
{
    QSqlDatabase db = QSqlDatabase::addDatabase( "QSQLITE", getBaseName() );
    QString path = settings["file_path"].toString();
    db.setDatabaseName( path );

    if (db.open())
    {
        std::cout << "Database: connection ok" << std::endl;

        bool thread_conn = _thread_sql.copyDatabase( getBaseName() );
        return thread_conn;
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

// FIXME tmp solution of notify check ---
void PsqlTranslator::onNewNotify( const QString& name,
                                  QSqlDriver::NotificationSource source,
                                  const QVariant &payload)
{
    qDebug() << "From"
             << ( QSqlDriver::SelfSource == source ? "this" : "other" );
    qDebug() << "Notify:" << name << "with:" << payload;
}
//-------------------------------------------

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
        // TODO check version of database

        // FIXME tmp solution of notify check -------
        db.driver()->subscribeToNotification( "area_add" );
        QObject::connect( db.driver(), SIGNAL( notification( const QString&,
                                                             QSqlDriver::NotificationSource,
                                                             const QVariant& )),
                          this, SLOT( onNewNotify( const QString&,
                                                   QSqlDriver::NotificationSource,
                                                   const QVariant& )));
        //-------------------------------------------

        std::cout << "Database: connection ok" << std::endl;

        bool thread_conn = _thread_sql.copyDatabase( getBaseName() );
        return thread_conn;
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
