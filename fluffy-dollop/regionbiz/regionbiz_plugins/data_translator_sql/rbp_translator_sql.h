#ifndef RB_TRANSLATOR_SQLITE_H
#define RB_TRANSLATOR_SQLITE_H

#include <QSqlDriver>

#include <regionbiz/rb_data_translator.h>
#include <regionbiz/rb_plugin_register.h>

#include "rbp_sql_thread.h"

namespace regionbiz {

REGISTER_PLUGIN( data_tranlator_sql )

class SqlTranslator: public BaseDataTranslator
{
public:
    SqlTranslator();
    virtual ~SqlTranslator();

protected:
    ThreadSql _thread_sql;

private:
    void loadFunctions() override;
    virtual bool initBySettings(QVariantMap settings) = 0;

    // get name
    virtual QString getTranslatorName() override = 0;

    // base name (for work with other QDatabase)
    virtual QString getBaseName() = 0;

    // read locations
    std::vector< RegionPtr > loadRegions();
    std::vector< LocationPtr > loadLocations();
    std::vector< FacilityPtr > loadFacilitys();
    std::vector< FloorPtr > loadFloors();
    std::vector< RoomPtr > loadRooms();

    // locations
    bool commitArea( BaseAreaPtr area );
    bool deleteArea( BaseAreaPtr area );

    // read relations
    std::vector< PropertyPtr > loadPropertys();
    std::vector< RentPtr > loadRents();

    // metadata
    BaseMetadataPtrs loadMetadata();

    // marks
    MarkPtrs loadMarks();
    bool commitMark(MarkPtr mark);
    bool deleteMark(MarkPtr mark);

    // files
    BaseFileKeeperPtrs loadFiles();

    // groups
    GroupEntityPtrs loadGroups();
    bool commitGroups( GroupEntityPtrs groups );
    bool deleteGroup( GroupEntityPtr group );

    // layers
    LayerPtrs loadLayers();
    bool commitLayers();
    bool deleteLayer( LayerPtr layer );

    // transform matrixes
    TransformById loadTransformMatrixes();
    bool commitTransformMatrix( FacilityPtr facility );

    // support functions
    template< typename LocTypePtr >
    bool loadCoordinate( std::vector< LocTypePtr >& vector );
    template< typename LocType >
    std::vector< std::shared_ptr< LocType >> loadBaseAreas( QString type_name );
    bool loadDocuments( BaseBizRelationPtr relation );
    bool loadPayments( RentPtr rent );
    QString getStringFileType( BaseFileKeeper::FileType type );
    BaseFileKeeper::FileType getFileTypeByString( QString type );
    MarkPtr getMarkByTypeString(QString type , uint64_t id);

    // sql thread for long db operations
    QThread* _self_thread = nullptr;
};

//------------------------------------------------------------

class SqliteTranslator: public SqlTranslator
{
public:
    virtual ~SqliteTranslator(){}

private:
    bool initBySettings( QVariantMap settings ) override;
    QString getTranslatorName() override;
    QString getBaseName() override;
};

//------------------------------------------------------------

class PsqlTranslator: public QObject, public SqlTranslator
{
    // FIXME tmp solution of notify check ---
    Q_OBJECT
public:
    ~PsqlTranslator(){}

private Q_SLOTS:
    void onNewNotify(const QString &name,
                      QSqlDriver::NotificationSource source,
                      const QVariant& payload );
    //--------------------------------------

private:
    bool initBySettings( QVariantMap settings ) override;
    QString getTranslatorName() override;
    QString getBaseName() override;
};

}

#endif // RB_TRANSLATOR_SQLITE_H
