#include "rbp_sql_thread.h"

#include <iostream>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlError>

#include <regionbiz/rb_manager.h>

#define tryQuery( string ) \
    if( !query.exec( string )) \
    { \
        qDebug() << query.lastError(); \
        db.rollback(); \
        return false; \
    } \

using namespace regionbiz;

ThreadSql::ThreadSql()
{
    // connect for queue work
    qRegisterMetaType< regionbiz::ThreadSql::Command >( "Command" );
    QObject::connect( this, SIGNAL( appendNewCommand( Command, uint64_t )),
                      this, SLOT( onAppendNewCommand( Command, uint64_t )),
                      Qt::QueuedConnection );
}

bool ThreadSql::copyDatabase(QString db_name)
{
    auto new_db = QSqlDatabase::cloneDatabase( QSqlDatabase::database( db_name ),
                                               getBaseName() );
    if( new_db.open() )
        return true;
    else
    {
        std::cerr << "Database of thread don't connected" << std::endl;
        return false;
    }
}

void ThreadSql::appendCommand(ThreadSql::Command cmnd, uint64_t id)
{
    Q_EMIT appendNewCommand( cmnd, id );
}

void ThreadSql::onAppendNewCommand( ThreadSql::Command cmnd, uint64_t id )
{
    std::cerr << "Append new SQL command: " << cmnd
              << " (" << id << ")" << std::endl;

    _commands.push( { cmnd, id } );

    if( !_commands.empty() )
        processQueueOfCommand();
}

void ThreadSql::processQueueOfCommand()
{
    while( !_commands.empty() )
    {
        auto pair = _commands.front();
        _commands.pop();
        Command cmnd = pair.first;
        uint64_t id = pair.second;

        auto mngr = RegionBizManager::instance();
        bool no_error = true;

        switch( cmnd )
        {
        case C_COMMIT_AREA:
        {
            auto area = mngr->getBaseArea( id );
            no_error = commitArea( area );
            break;
        }
        case C_DELETE_AREA:
        {
            auto area = mngr->getBaseArea( id );
            no_error = deleteArea( area );
            break;
        }

        case C_COMMIT_MARK:
        {
            auto mark = mngr->getMark( id );
            no_error = commitMark( mark );
            break;
        }
        case C_DELETE_MARK:
        {
            auto mark = mngr->getMark( id );
            no_error = deleteMark( mark );
            break;
        }

        case C_COMMIT_GROUP:
        {
            auto mark = mngr->getGroup( id );
            no_error = commitGroup( mark );
            break;
        }
        case C_DELETE_GROUP:
        {
            auto mark = mngr->getGroup( id );
            no_error = deleteGroup( mark );
            break;
        }

        }

        if( !no_error )
            std::cerr << "Somthing wrong with SQL Command: "
                      << cmnd << std::endl;
    }
}

QString ThreadSql::getBaseName()
{
    return "REGION_BIZ_PSQL_THREAD";
}

bool ThreadSql::commitArea(BaseAreaPtr area)
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

bool ThreadSql::deleteArea(BaseAreaPtr area)
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

    QString delete_files = "DELETE FROM files "
                           " WHERE id = " + QString::number( area->getId() );
    tryQuery( delete_files );

    // TODO delete relations

    // unlock
    db.commit();
    return true;
}

bool ThreadSql::commitMark(MarkPtr mark)
{
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    // lock base
    db.transaction();

    // update mark
    QString delete_ent = "DELETE FROM entitys "
                          "WHERE id = " + QString::number( mark->getId() );
    QSqlQuery query( db );
    tryQuery( delete_ent );

    QString delete_mark = "DELETE FROM marks "
                          "WHERE id = " + QString::number( mark->getId() );
    tryQuery( delete_mark );

    QString insert_update = "INSERT INTO entitys( id, parent_id, name, description ) "
                            "VALUES (?, ?, ?, ?)";
    query.prepare( insert_update );
    query.addBindValue( (qulonglong) mark->getId() );
    query.addBindValue( (qulonglong) mark->getParentId() );
    query.addBindValue( mark->getName() );
    query.addBindValue( mark->getDescription() );
    tryQuery();

    QString insert_mark = "INSERT INTO marks( id, type ) "
                          "VALUES (?, ?)";
    query.prepare( insert_mark );
    query.addBindValue( (qulonglong) mark->getId() );
    query.addBindValue( getMarkType( mark->getMarkType() ));
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

    // commit coordinates
    if( !commitCoordinates( mark ))
    {
        db.rollback();
        return false;
    }

    // unlock base
    db.commit();
    return true;
}

bool ThreadSql::deleteMark(MarkPtr mark)
{
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    // lock base
    db.transaction();

    // delete ent
    QString delete_ent = "DELETE FROM entitys "
                          "WHERE id = " + QString::number( mark->getId() );
    QSqlQuery query( db );
    tryQuery( delete_ent );

    // delete mark
    QString delete_mark = "DELETE FROM marks "
                          "WHERE id = " + QString::number( mark->getId() );
    tryQuery( delete_mark );

    // metadata
    QString delete_meta = "DELETE FROM metadata "
                          "WHERE entity_id = " + QString::number( mark->getId() );
    tryQuery( delete_meta );

    // coords
    QString delete_coords = "DELETE FROM coords "
                            "WHERE id = " + QString::number( mark->getId() );
    tryQuery( delete_coords );

    // files
    QString delete_files = "DELETE FROM files "
                           "WHERE id = " + QString::number( mark->getId() );
    tryQuery( delete_files );

    // unlock base
    db.commit();
    return true;
}

bool ThreadSql::commitGroup( GroupEntityPtr group )
{
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    // lock base
    db.transaction();

    // update group like entity
    QString insert_update = "INSERT INTO entitys( id, parent_id, name, description ) "
                            "VALUES (?, ?, ?, ?)";
    QSqlQuery query( db );
    query.prepare( insert_update );
    query.addBindValue( (qulonglong) group->getId() );
    // NOTE group has't parent
    query.addBindValue( (qulonglong) UNSELECTED_ID );
    query.addBindValue( group->getName() );
    query.addBindValue( group->getDescription() );
    tryQuery();

    // update group consist
    QString insert_coords = "INSERT INTO groups ( group_id, element_id ) "
                            "VALUES ( ?, ? );";
    query.prepare( insert_coords );
    // prepare data
    QVariantList group_ids, element_ids;
    for( auto element_id: group->getElementsIds() )
    {
        group_ids.push_back( (qulonglong) group->getId() );
        element_ids.push_back( (qulonglong) element_id );
    }
    // bind data
    query.addBindValue( group_ids );
    query.addBindValue( element_ids );

    if( !query.execBatch() )
    {
        db.rollback();
        return false;
    }

    // commit metadata
    if( !commitMetadate( group ))
    {
        db.rollback();
        return false;
    }

    // commit files
    if( !commitFiles( group ))
    {
        db.rollback();
        return false;
    }

    // unlock base
    db.commit();
    return true;
}

bool ThreadSql::deleteGroup(GroupEntityPtr group)
{
    // lock database
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    db.transaction();
    QSqlQuery query( db );

    // delete group
    QString delete_group = "DELETE FROM entitys "
                           "WHERE id = " + QString::number( group->getId() );
    tryQuery( delete_group );

    // metadata
    QString delete_meta = "DELETE FROM metadata "
                          "WHERE entity_id = " + QString::number( group->getId() );
    tryQuery( delete_meta );

    // TODO delete files

    // unlock
    db.commit();
    return true;
}

bool ThreadSql::commitMetadate(BaseEntityPtr entity)
{
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QSqlQuery query( db );
    QString delete_coords = "DELETE FROM metadata "
                            "WHERE entity_id = " + QString::number( entity->getId() );
    if( !query.exec( delete_coords ))
        return false;

    QString insert_coords = "INSERT INTO metadata ( entity_id, type, name, value ) "
                            "VALUES ( ?, ?, ?, ? );";
    query.prepare( insert_coords );
    // prepare data
    QVariantList ids, types, names, values;
    for( auto data_pair: entity->getMetadataMap() )
    {
        BaseMetadataPtr data = data_pair.second;

        ids.push_back( (qulonglong) entity->getId() );
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

bool ThreadSql::commitFiles(BaseEntityPtr entity)
{
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QSqlQuery query( db );
    QString delete_files = "DELETE FROM files "
                           "WHERE entity_id = " + QString::number( entity->getId() );
    if( !query.exec( delete_files ))
        return false;

    // insert all files
    QString insert_files = "INSERT INTO files( entity_id, path, type, name ) VALUES ( ?, ?, ?, ? );";
    query.prepare( insert_files );
    // prepare data
    QVariantList ids, paths, types, names;
    for( auto file: entity->getFiles() )
    {
        ids.push_back( (qulonglong) entity->getId() );
        paths.push_back( file->getPath() );
        types.push_back( getStringFileType( file->getType() ));
        names.push_back( file->getName() );
    }
    // bind data
    query.addBindValue( ids );
    query.addBindValue( paths );
    query.addBindValue( types );
    query.addBindValue( names );

    if ( !query.execBatch() )
        return false;

    // insert plans
    QString insert_plans = "INSERT INTO plans( path, scale_w, scale_h, angle, x, y, opacity ) VALUES ( ?, ?, ?, ?, ?, ?, ? );";
    query.prepare( insert_plans );
    // prepare data
    QVariantList plan_paths, scales_w, scales_h,
            angles, xs, ys, opacitys;
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
        opacitys.push_back( params.opacity );
    }
    // bind data
    query.addBindValue( plan_paths );
    query.addBindValue( scales_w );
    query.addBindValue( scales_h );
    query.addBindValue( angles );
    query.addBindValue( xs );
    query.addBindValue( ys );
    query.addBindValue( opacitys );

    return query.execBatch();
}

QString ThreadSql::getStringType(BaseArea::AreaType area_type)
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

template<typename EntType>
bool ThreadSql::commitCoordinates(EntType entity )
{
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    QSqlQuery query( db );
    query.setForwardOnly( true );
    QString delete_coords = "DELETE FROM coords "
                            "WHERE id = " + QString::number( entity->getId() );
    if( !query.exec( delete_coords ))
        return false;

    QString insert_coords = "INSERT INTO coords ( id, x, y, number ) VALUES (?, ?, ?, ?)";
    query.prepare( insert_coords );
    // prepare data
    QVariantList ids, xs, ys, numbers;
    uint num = 0;
    for( QPointF pnt: entity->getCoords() )
    {
        ids.push_back( (qulonglong) entity->getId() );
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

QString ThreadSql::getStringFileType(BaseFileKeeper::FileType type)
{
    QString type_str;
    switch( type )
    {
    case BaseFileKeeper::FT_PLAN:
        type_str = "plan";
        break;
    case BaseFileKeeper::FT_DOCUMENT:
        type_str = "document";
        break;
    case BaseFileKeeper::FT_IMAGE:
        type_str = "image";
        break;
    case BaseFileKeeper::FT_NONE:
        break;
    }

    return type_str;
}

QString ThreadSql::getMarkType(Mark::MarkType type)
{
    switch( type )
    {
    case Mark::MT_DEFECT:
        return "defect";
    case Mark::MT_PHOTO:
        return "photo";
    case Mark::MT_PHOTO_3D:
        return "photo3d";
    case Mark::MT_INVALID:
        static_assert( true, "Get inbalid type of Mark" );
    }

    return "";
}
