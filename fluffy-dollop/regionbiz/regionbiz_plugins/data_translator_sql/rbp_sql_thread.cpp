#include "rbp_sql_thread.h"

#include <iostream>
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

#define tryBatch() \
    if( !query.execBatch() ) \
    { \
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
    //std::cerr << "Append new SQL command: " << cmnd
    //          << " (" << id << ")" << std::endl;

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
            no_error = deleteArea( id );
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
            no_error = deleteMark( id );
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
            no_error = deleteGroup( id );
            break;
        }

        case C_COMMIT_LAYERS:
        {
            no_error = commitLayers();
            break;
        }

        case C_DELETE_LAYER:
            no_error = deleteLayer( id );
            break;

        case C_COMMIT_TRANSFORM_MATRIX:
            no_error = commitTransformMatrix( id );
            break;

        case C_COMMIT_GRAPH:
        {
            auto graph = mngr->getGraph( id );
            if( !graph )
                return;
            no_error = commitGraph( graph );
            break;
        }

        case C_DELETE_GRAPH:
            no_error = deleteGraph( id );
            break;

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

bool ThreadSql::deleteArea( uint64_t id )
{
    // lock database
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    db.transaction();
    QSqlQuery query( db );

    // delete all data
    QString delete_metadata = "DELETE FROM metadata "
                              "WHERE entity_id = " + QString::number( id );
    tryQuery( delete_metadata );

    QString delete_coords = "DELETE FROM coords "
                            "WHERE id = " + QString::number( id );
    tryQuery( delete_coords );

    QString delete_entity = "DELETE FROM entitys "
                          " WHERE id = " + QString::number( id );
    tryQuery( delete_entity );

    QString delete_area = "DELETE FROM areas "
                          " WHERE id = " + QString::number( id );
    tryQuery( delete_area );

    QString delete_files = "DELETE FROM files "
                           " WHERE entity_id = " + QString::number( id );
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

bool ThreadSql::deleteMark(uint64_t id)
{
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    // lock base
    db.transaction();

    // delete ent
    QString delete_ent = "DELETE FROM entitys "
                          "WHERE id = " + QString::number( id );
    QSqlQuery query( db );
    tryQuery( delete_ent );

    // delete mark
    QString delete_mark = "DELETE FROM marks "
                          "WHERE id = " + QString::number( id );
    tryQuery( delete_mark );

    // metadata
    QString delete_meta = "DELETE FROM metadata "
                          "WHERE entity_id = " + QString::number( id );
    tryQuery( delete_meta );

    // coords
    QString delete_coords = "DELETE FROM coords "
                            "WHERE id = " + QString::number( id );
    tryQuery( delete_coords );

    // files
    QString delete_files = "DELETE FROM files "
                           "WHERE entity_id = " + QString::number( id );
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

bool ThreadSql::deleteGroup(uint64_t id)
{
    // lock database
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    db.transaction();
    QSqlQuery query( db );

    // delete group
    QString delete_group = "DELETE FROM entitys "
                           "WHERE id = " + QString::number( id );
    tryQuery( delete_group );

    // metadata
    QString delete_meta = "DELETE FROM metadata "
                          "WHERE entity_id = " + QString::number( id );
    tryQuery( delete_meta );

    // TODO delete files

    // unlock
    db.commit();
    return true;
}

bool ThreadSql::commitLayers()
{
    // get all layers
    auto mngr = RegionBizManager::instance();
    LayerPtrs layers = mngr->getLayers();

    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    // lock
    db.transaction();
    QSqlQuery query( db );

    // delete layers
    QString delete_layers = "DELETE FROM layers;"
                            "DELETE FROM layers_elements;";
    tryQuery( delete_layers );

    // insert layers in main table
    QString insert_layers = "INSERT INTO layers ( id, priority, name ) "
                            "VALUES ( ?, ?, ? );";
    query.prepare( insert_layers );
    // prepare data
    QVariantList ids, prioritys, names;
    for( LayerPtr layer: layers )
    {
        ids.push_back( (qulonglong) layer->getId() );
        prioritys.push_back( (qulonglong) layer->getNumber() );
        names.push_back( layer->getName() );
    }
    // bind data
    query.addBindValue( ids );
    query.addBindValue( prioritys );
    query.addBindValue( names );

    if( !query.execBatch() )
    {
        db.rollback();
        return false;
    }

    // insert elements - marks
    QString insert_marks = "INSERT INTO layers_elements ( layer_id, element.id, type ) "
                           "VALUES ( ?, ?, 'mark' );";
    query.prepare( insert_marks );
    {
        QVariantList layer_ids, element_ids;
        for( LayerPtr layer: layers  )
        {
            for( MarkPtr element_mark: layer->getMarks() )
            {
                layer_ids.push_back( (qulonglong) layer->getId() );
                element_ids.push_back( (qulonglong) element_mark->getId() );
            }
        }
        query.addBindValue( layer_ids );
        query.addBindValue( element_ids );
    }
    tryBatch();

    // insert elements - files
    QString insert_files = "INSERT INTO layers_elements ( layer_id, element.name, type ) "
                           "VALUES ( ?, ?, 'file' );";
    query.prepare( insert_files );
    {
        QVariantList layer_ids, element_ids;
        for( LayerPtr layer: layers  )
        {
            for( BaseFileKeeperPtr element_file: layer->getFiles() )
            {
                layer_ids.push_back( (qulonglong) layer->getId() );
                element_ids.push_back( element_file->getPath() );
            }
        }
        query.addBindValue( layer_ids );
        query.addBindValue( element_ids );
    }
    tryBatch();

    // insert elements - meta
    QString insert_meta = "INSERT INTO layers_elements ( layer_id, element.name, type ) "
                          "VALUES ( ?, ?, 'metadata' );";
    query.prepare( insert_meta );
    {
        QVariantList layer_ids, element_ids;
        for( LayerPtr layer: layers  )
        {
            for( QString element_meta: layer->getMetadataNames() )
            {
                layer_ids.push_back( (qulonglong) layer->getId() );
                element_ids.push_back( element_meta );
            }
        }
        query.addBindValue( layer_ids );
        query.addBindValue( element_ids );
    }
    tryBatch();

    // unlock
    db.commit();
    return true;
}

bool ThreadSql::deleteLayer(uint64_t id )
{
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    // lock
    db.transaction();
    QSqlQuery query( db );

    // delete layers
    QString id_str = QString::number( id );
    QString delete_layer = "DELETE FROM layers WHERE id = " + id_str + ";"
                           "DELETE FROM layers_elements WHERE layer_id = " + id_str + ";";
    tryQuery( delete_layer );

    // unlock
    db.commit();
    return true;
}

bool ThreadSql::commitTransformMatrix( uint64_t facility_id )
{
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    // lock
    db.transaction();
    QSqlQuery query( db );

    // delete transform
    QString id_str = QString::number( facility_id );
    QString delete_layer = "DELETE FROM transform_matrix WHERE facility_id = " + id_str + ";";
    tryQuery( delete_layer );

    // get transform
    auto mngr = RegionBizManager::instance();
    bool have = mngr->isHaveTransform( facility_id );
    if( have )
    {
        QString insert_coords = "INSERT INTO transform_matrix (facility_id, "
                                "rotate_scale.m11, rotate_scale.m12, "
                                "rotate_scale.m21, rotate_scale.m22, "
                                "shift.s1, shift.s2) "
                                "VALUES( ?,?,?,?,?,?,? )";
        query.prepare( insert_coords );

        QVariant id = (quint64) facility_id;

        QTransform trans = mngr->getTransform( facility_id );
        QVariant m11 = trans.m11();
        QVariant m12 = trans.m12();
        QVariant m21 = trans.m21();
        QVariant m22 = trans.m22();
        QVariant s1 = trans.dx();
        QVariant s2 = trans.dy();

        query.bindValue( 0, id );
        query.bindValue( 1, m11 );
        query.bindValue( 2, m12 );
        query.bindValue( 3, m21 );
        query.bindValue( 4, m22 );
        query.bindValue( 5, s1 );
        query.bindValue( 6, s2 );

        tryQuery()
    }

    // unlock
    db.commit();
    return true;
}

bool ThreadSql::commitGraph(GraphEntityPtr graph)
{
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    // lock
    db.transaction();
    QSqlQuery query( db );

    // check exist
    QString select = "SELECT id, parent_id, name, description FROM entitys "
                     " WHERE id = " + QString::number( graph->getId() );
    query.setForwardOnly( true );
    query.exec( select );
    bool has_graph = query.first();

    QString insert_update;
    if( has_graph )
    {
        // update
        insert_update = "UPDATE entitys "
                        "SET id = ?, parent_id = ?, name = ?, description = ? "
                        "WHERE id = " + QString::number( graph->getId() ) + ";";
    }
    else
    {
        // insert
        insert_update = "INSERT INTO entitys (id, parent_id, name, description) "
                        "VALUES (?, ?, ?, ?);";
    }

    query.prepare( insert_update );
    query.addBindValue( (qulonglong) graph->getId() );
    query.addBindValue( (qulonglong) graph->getParentId() );
    query.addBindValue( graph->getName() );
    query.addBindValue( graph->getDescription() );

    tryQuery()

    if( !has_graph )
    {
        // find a type
        QString insert_graph = "INSERT INTO graphs (id, type)"
                               "VALUES (?, ?);";
        query.prepare( insert_graph );
        query.addBindValue( (qulonglong) graph->getId() );
        query.addBindValue( "graph" );
        tryQuery()
    }

    // delete points and edges
    QString id_str = QString::number( graph->getId() );
    QString delete_nodes = "DELETE FROM entitys WHERE id IN "
                           "( SELECT id FROM entitys WHERE parent_id = " + id_str + ")";
    tryQuery( delete_nodes );

    // commit points
    for( GraphNodePtr node: graph->getNodes() )
    {
        if( !commitNode( query, node ))
        {
            db.rollback();
            return false;
        }
    }

    // commit edges
    for( GraphEdgePtr edge: graph->getEdges() )
    {
        if( !commitEdge( query, edge ))
        {
            db.rollback();
            return false;
        }
    }

    // commit metadata
    if( !commitMetadate( graph ))
    {
        db.rollback();
        return false;
    }

    db.commit();
    return true;
}

bool ThreadSql::deleteGraph(uint64_t id)
{
    QSqlDatabase db = QSqlDatabase::database( getBaseName() );
    // lock
    db.transaction();
    QSqlQuery query( db );

    // delete points
    QString id_str = QString::number( id );
    QString delete_query = "DELETE FROM entitys WHERE id IN "
                           "( SELECT id FROM entitys WHERE parent_id = " + id_str + ")";
    tryQuery( delete_query );

    delete_query = "DELETE FROM graphs WHERE id = " + id_str + ";";
    tryQuery( delete_query );

    delete_query = "DELETE FROM entitys WHERE id = " + id_str + ";";
    tryQuery( delete_query );

    db.commit();
    return true;
}

bool ThreadSql::commitNode( QSqlQuery& query, GraphNodePtr node )
{
    // insert into entity table
    QString insert_update = "INSERT INTO entitys (id, parent_id, name, description) "
                            "VALUES (?, ?, ?, ?);";

    query.prepare( insert_update );
    query.addBindValue( (qulonglong) node->getId() );
    query.addBindValue( (qulonglong) node->getParentGraph()->getId() );
    query.addBindValue( node->getName() );
    query.addBindValue( node->getDescription() );
    if( !query.exec())
    {
        qDebug() << query.lastError();
        return false;
    }

    // insert into graph table
    QString insert_node = "INSERT INTO graphs (id, type)"
                          "VALUES (?, ?)";
    query.prepare( insert_node );
    query.addBindValue( (qulonglong) node->getId() );
    query.addBindValue( "node" );
    if( !query.exec())
    {
        qDebug() << query.lastError();
        return false;
    }

    // commit metadata
    if( !commitMetadate( node ))
        return false;

    // commit coordinates
    if( !commitCoordinates( node ))
        return false;

    return true;
}

bool ThreadSql::commitEdge(QSqlQuery &query, GraphEdgePtr edge)
{
    // insert into entity table
    QString insert_update = "INSERT INTO entitys (id, parent_id, name, description) "
                            "VALUES (?, ?, ?, ?);";

    query.prepare( insert_update );
    query.addBindValue( (qulonglong) edge->getId() );
    query.addBindValue( (qulonglong) edge->getParentGraph()->getId() );
    query.addBindValue( edge->getName() );
    query.addBindValue( edge->getDescription() );
    if( !query.exec())
    {
        qDebug() << query.lastError();
        return false;
    }

    // insert into graph table
    QString insert_node = "INSERT INTO graphs (id, type)"
                          "VALUES (?, ?)";
    query.prepare( insert_node );
    query.addBindValue( (qulonglong) edge->getId() );
    query.addBindValue( "edge" );
    if( !query.exec())
    {
        qDebug() << query.lastError();
        return false;
    }

    // insert into graph table
    insert_node = "INSERT INTO graph_edges (id, first_node, second_node)"
                  "VALUES (?, ?, ?)";
    query.prepare( insert_node );
    query.addBindValue( (qulonglong) edge->getId() );
    query.addBindValue( (qulonglong) edge->getFirstPoint()->getId() );
    query.addBindValue( (qulonglong) edge->getSecondPoint()->getId() );
    if( !query.exec())
    {
        qDebug() << query.lastError();
        return false;
    }

    // commit metadata
    if( !commitMetadate( edge ))
        return false;

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
    case BaseFileKeeper::FT_PLAN_RASTER:
        type_str = "plan_raster";
        break;
    case BaseFileKeeper::FT_PLAN_VECTOR:
        type_str = "plan_vector";
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
        return "photo_3d";
    case Mark::MT_PLACEHOLDER:
        return "place_holder";
    case Mark::MT_INVALID:
        static_assert( true, "Get inbalid type of Mark" );
    }

    return "";
}
