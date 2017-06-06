#ifndef RB_MANAGER_H
#define RB_MANAGER_H

#include <memory>
#include <list>

#include "rb_locations.h"
#include "rb_data_translator.h"
#include "rb_files_translator.h"
#include "rb_signal_manager.h"
#include "rb_metadata.h"
#include "rb_marks.h"
#include "rb_group.h"

#define FIND_IF( cont, func ) std::find_if( cont.begin(), cont.end(), func );

namespace regionbiz {

class RegionBizManager;
typedef std::shared_ptr< RegionBizManager > RegionBizManagerPtr;

class RegionBizManager
{
public:
    virtual ~RegionBizManager(){}

    // init
    static RegionBizManagerPtr instance();
    bool init( QString& config_path );

    // get entity
    BaseEntityPtr getBaseEntity( uint64_t id );

    // locations
    // getters
    BaseAreaPtr getBaseArea( uint64_t id );
    BaseAreaPtr getBaseArea( uint64_t id, BaseArea::AreaType type );
    BaseAreaPtrs getAreaChildsByParent( uint64_t id );

    // typed getters
    std::vector< RegionPtr > getRegions();
    std::vector< LocationPtr > getLocationsByParent( uint64_t parent_id );
    std::vector< FacilityPtr > getFacilitysByParent( uint64_t parent_id );
    std::vector< FloorPtr > getFloorsByParent( uint64_t parent_id );
    std::vector< RoomPtr > getRoomsByParent( uint64_t parent_id );

    // add
    BaseAreaPtr addArea( BaseArea::AreaType type,
                         uint64_t parent_id );
    template< typename Type >
    BaseAreaPtr addArea( uint64_t parent_id )
    {
        // TODO check parent type
        BaseAreaPtr area = BaseEntity::createWithId< Type >( BaseEntity::getMaxId() + 1 );
        if( area )
        {
            area->setParent( parent_id );

            // emit signal
            _change_watcher.addEntity( area->getId() );
        }
        return area;
    }
    BaseAreaPtr addArea( BaseArea::AreaType type,
                         BaseAreaPtr parent );
    template< typename Type >
    BaseAreaPtr addArea( BaseAreaPtr parent )
    {
        if( !parent )
            return nullptr;

        BaseAreaPtr add_area = addArea< Type >( parent->getId() );
        return add_area;
    }

    // delete
    bool deleteArea( BaseAreaPtr area );
    bool deleteArea( uint64_t id );

    // commit
    bool commitArea( BaseAreaPtr area );
    bool commitArea( uint64_t id );

    // biz relations
    BaseBizRelationPtrs getBizRelationByArea( uint64_t id );
    BaseBizRelationPtrs getBizRelationByArea( uint64_t id, BaseBizRelation::RelationType type );

    // metadata
    bool isMetadataPresent( uint64_t id, QString name );
    QVariant getMetadataValue( uint64_t id, QString name );
    BaseMetadataPtr getMetadata( uint64_t id, QString name );
    MetadataByName getMetadataMap( uint64_t id );
    bool setMetadataValue(uint64_t id, QString name , QVariant val);
    bool  addMetadata( uint64_t id, QString type,
                      QString name, QVariant val = QVariant() );
    bool addMetadata( BaseMetadataPtr data );

    // marks
    MarkPtr getMark( uint64_t id );
    MarkPtrs getMarksByParent( uint64_t id );
    MarkPtrs getMarksByParent( uint64_t id, Mark::MarkType type );
    MarkPtrs getMarksByParent( MarksHolderPtr parent );
    MarkPtrs getMarksByParent( MarksHolderPtr parent, Mark::MarkType type );
    MarkPtrs getMarks();
    MarkPtr addMark(uint64_t parent_id, Mark::MarkType type,
                     QPointF center = QPointF() );
    MarkPtr addMark(uint64_t parent_id, Mark::MarkType type,
                     QPolygonF coords );
    bool commitMark( uint64_t id );
    bool commitMark( MarkPtr mark );
    bool deleteMark( uint64_t id );
    bool deleteMark( MarkPtr mark );

    // groups
    GroupEntityPtr getGroup( uint64_t id );
    GroupEntityPtrs getGroups();
    GroupEntityPtr getGroupOfEntity( uint64_t id );
    GroupEntityPtr addGroup();
    bool commitGroup( uint64_t id );
    bool commitGroup( GroupEntityPtr group );
    bool commitGroupsChanged();
    bool deleteGroup( GroupEntityPtr group );
    bool deleteGroup( uint64_t id );

    // files
    BaseFileKeeperPtrs getFilesByEntity( uint64_t id );
    BaseFileKeeperPtrs getFilesByEntity( BaseEntityPtr ptr );
    BaseFileKeeperPtrs getFilesByEntity( uint64_t id, BaseFileKeeper::FileType type );
    BaseFileKeeperPtrs getFilesByEntity( BaseEntityPtr ptr, BaseFileKeeper::FileType type );
    BaseFileKeeperPtr addFile( QString file_path ,
                               BaseFileKeeper::FileType type,
                               uint64_t entity_id );

    // through file tranlator
    QFilePtr getLocalFile( BaseFileKeeperPtr file );
    BaseFileKeeper::FileState getFileState( BaseFileKeeperPtr file );
    BaseFileKeeper::FileState syncFile( BaseFileKeeperPtr file );
    bool deleteFile( BaseFileKeeperPtr file );
    bool commitFile( BaseFileKeeperPtr file );

    // signals of file processing subscribe
    //! signal on file synced. Signature:
    //! slot( BaseFileKeeperPtr )
    void subscribeFileSynced( QObject* obj,
                              const char *slot );
    //! signal on file added. Signature:
    //! slot( BaseFileKeeperPtr )
    void subscribeFileAdded( QObject* obj,
                             const char *slot );
    //! signal on file deleted. Signature:
    //! slot( BaseFileKeeperPtr )
    void subscribeFileDeleted( QObject* obj,
                               const char *slot );

    // tranlators
    BaseTranslatorPtr getTranslatorByName( QString name );

    // selection managment
    uint64_t getCurrentEntity();
    void setCurrentEntity( uint64_t id );
    void clearCurrent();
    //! change current. Signature:
    //! slot( uint64_t prev_id, uint64_t new_id )
    void subscribeOnCurrentChange( QObject* obj,
                                   const char *slot,
                                   bool queue = false );

    std::vector< uint64_t > getSelectedSet();
    void appendToSelectedSet( uint64_t id, bool force = true );
    void appendToSelectedSet( std::vector< uint64_t > ids, bool force = true );
    void removeFromSelectedSet( uint64_t id, bool force = true );
    void removeFromSelectedSet( std::vector< uint64_t > ids, bool force = true );
    void clearSelect();
    //! change selected set. Signature:
    //! slot( std::vector< uint64_t > selected, std::vector< uint64_t > deselected );
    void subscribeOnSelectedSetChange( QObject* obj,
                                       const char *slot,
                                       bool queue = false );
    //! change selected by anyway. Signature:
    //! slot()
    void subscribeOnSelectedChange( QObject* obj,
                                    const char *slot,
                                    bool queue = false );

    void centerOnEntity( uint64_t id );
    //! center on some. Signature:
    //! slot( uint64_t id )
    void subscribeCenterOn( QObject* obj,
                            const char *slot,
                            bool queue = false );

    // other signals
    //! if some entity was chacged (commited). Signature:
    //! slot( uint64_t id )
    void subscribeOnChangeEntity( QObject* obj,
                                  const char *slot,
                                  bool queue = false );
    //! if some entity was deleted. Signature:
    //! slot( uint64_t id )
    void subscribeOnDeleteEntity( QObject* obj,
                                  const char *slot,
                                  bool queue = false );
    //! if some entity was added. Signature:
    //! slot( uint64_t id )
    void subscribeOnAddEntity( QObject* obj,
                               const char *slot,
                               bool queue = false );

private:
    RegionBizManager();

    static void onExit();
    QVariantMap loadJsonConfig( QString &file_path );
    bool processPlugins(QVariantMap settings);
    bool processTranslators( QVariantMap settings );
    bool loadPlugins( QString plugins_path, bool load_all,
                      QStringList plugins = QStringList() );
    void loadDataByTranslator();
    void clearCurrentData(bool clear_entitys = true );

    template< typename LocType >
    std::vector< std::shared_ptr< LocType >>
    getBaseAreasByParent( uint64_t parent_id );

    BaseDataTranslatorPtr _data_translator = nullptr;
    BaseFilesTranslatorPtr _files_translator = nullptr;

    // data relations
    //    std::vector< PropertyPtr > _propertys;
    //    std::vector< RentPtr > _rents;

    // signals
    SelectionManager _select_manager;
    ChangeEntitysWatcher _change_watcher;
};

}

#endif // RB_MANAGER_H
