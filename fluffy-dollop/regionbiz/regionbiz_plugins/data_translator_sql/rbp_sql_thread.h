#ifndef RBP_SQL_THREAD_H
#define RBP_SQL_THREAD_H

#include <queue>
#include <QThread>

#include <regionbiz/rb_locations.h>

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
        C_DELETE_GROUP
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
    bool deleteArea( BaseAreaPtr area );
    bool commitMark( MarkPtr mark );
    bool deleteMark( MarkPtr mark );
    bool commitGroup( GroupEntityPtr group );
    bool deleteGroup( GroupEntityPtr group );

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
