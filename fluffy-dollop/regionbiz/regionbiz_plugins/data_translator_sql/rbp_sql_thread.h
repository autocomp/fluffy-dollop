#ifndef RBP_SQL_THREAD_H
#define RBP_SQL_THREAD_H

#include <queue>
#include <QThread>
#include <QSqlQuery>

#include <regionbiz/rb_locations.h>
#include <regionbiz/rb_layers.h>

#define NO_ID 0

namespace regionbiz {

class ThreadSql: public QObject
{
    Q_OBJECT
public:
    enum Command
    {
        C_COMMIT_AREA,
        C_DELETE_AREA,
        C_COMMIT_MARK,
        C_DELETE_MARK,
        C_COMMIT_GROUP,
        C_DELETE_GROUP,
        C_COMMIT_LAYERS,
        C_DELETE_LAYER,
        C_COMMIT_TRANSFORM_MATRIX,
        C_COMMIT_GRAPH,
        C_DELETE_GRAPH
    };

    ThreadSql();

    //! copy exist database name for other thread
    bool copyDatabase( QString db_name );
    //! use in other thread to append command
    void appendCommand( Command cmnd, uint64_t id );

Q_SIGNALS:
    void appendNewCommand( Command cmnd, uint64_t id );

private Q_SLOTS:
    void onAppendNewCommand( Command cmnd, uint64_t id );

private:
    // process queue of command
    void processQueueOfCommand();

    // name of copyed database
    QString getBaseName();

    // main functions
    bool commitArea( BaseAreaPtr area );
    bool deleteArea(uint64_t id );
    bool commitMark( MarkPtr mark );
    bool deleteMark( uint64_t id );
    bool commitGroup( GroupEntityPtr group );
    bool deleteGroup( uint64_t id );
    bool commitLayers();
    bool deleteLayer( uint64_t id );
    bool commitTransformMatrix( uint64_t facility_id );
    bool commitGraph( GraphEntityPtr graph );
    bool deleteGraph( uint64_t id );
    bool commitNode( QSqlQuery &query, GraphNodePtr node );
    bool commitEdge( QSqlQuery &query, GraphEdgePtr edge );

    // commit small elements
    template< typename EntType >
    bool commitCoordinates( EntType entity );
    bool commitMetadate( BaseEntityPtr entity );
    bool commitFiles( BaseEntityPtr entity );

    // technick functions
    QString getStringType(BaseArea::AreaType area_type );
    QString getStringFileType(BaseFileKeeper::FileType type);
    QString getMarkType( Mark::MarkType type );

    // queue of commands
    std::queue< std::pair< Command, uint64_t >> _commands;
};

}

#endif // RBP_SQL_THREAD_H
