#ifndef RB_TRANSLATOR_SQLITE_H
#define RB_TRANSLATOR_SQLITE_H

#include "rb_data_translator.h"

namespace regionbiz {

class SqlTranslator: public BaseDataTranslator
{
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

    // delete locations
    bool deleteArea( BaseAreaPtr area );

    // commit locations
    bool commitArea( BaseAreaPtr area );

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

    // support functions
    template< typename LocTypePtr >
    bool loadCoordinate( std::vector< LocTypePtr >& vector );
    template< typename LocType >
    std::vector< std::shared_ptr< LocType >> loadBaseAreas( QString type_name );
    template< typename EntType >
    bool commitCoordinates( EntType area );
    bool commitMetadate(BaseEntityPtr area );
    bool commitFiles(BaseEntityPtr entity );
    bool loadDocuments( BaseBizRelationPtr relation );
    bool loadPayments( RentPtr rent );
    QString getStringType(BaseArea::AreaType area_type );
    QString getStringFileType( BaseFileKeeper::FileType type );
    BaseFileKeeper::FileType getFileTypeByString( QString type );
    MarkPtr getMarkByTypeString(QString type , uint64_t id);
    QString getMarkType( Mark::MarkType type );
};

//------------------------------------------------------------

class SqliteTranslator: public SqlTranslator
{
private:
    bool initBySettings( QVariantMap settings ) override;
    QString getTranslatorName() override;
    QString getBaseName() override;
};

//------------------------------------------------------------

class PsqlTranslator: public SqlTranslator
{
private:
    bool initBySettings( QVariantMap settings ) override;
    QString getTranslatorName() override;
    QString getBaseName() override;
};

}

#endif // RB_TRANSLATOR_SQLITE_H
