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
#include "rb_layers.h"
#include "rb_transform_matrix.h"
#include "rb_graph.h"

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

    // constraints
    bool isEntityConstraintsCorrect( uint64_t id );
    bool isEntityConstraintsCorrect( BaseEntityPtr entity );
    Constraints getConstraintsOfEntity( uint64_t id );
    Constraints getConstraintsOfEntity( BaseEntityPtr entity );
    Constraints getConstraintsOfEntity( uint64_t id, Constraint::ConstraintType type );
    Constraints getConstraintsOfEntity( BaseEntityPtr entity, Constraint::ConstraintType type );
    template< typename Type >
    bool addUserConstraint( Type type, QString name,
                            QString meta_type, QString cons_str,
                            QString showed_name = "", QVariant default_val = QVariant() )
    {
        if( ConstraintsManager::isConstraintPresent( type, name ))
        {
            Constraint cons = ConstraintsManager::getConstraint( type, name );
            if( cons.getType() != Constraint::CT_USER )
                return false;
        }

        Constraint cons = { Constraint::CT_USER, name, meta_type, cons_str,
                          showed_name, default_val };
        bool set = ConstraintsManager::addConstraint( type, cons );
        return set;
    }

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
        BaseAreaPtr area = BaseEntity::createWithId< Type >(
                    BaseEntity::getMaxId() + 1, parent_id);
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

    // specific room
    RoomPtr addRoom( Room::RoomType type, uint64_t parent_id );
    RoomPtr addRoom( Room::RoomType type, BaseAreaPtr parent );

    // commit
    bool commitArea( BaseAreaPtr area );
    bool commitArea( uint64_t id );
    // delete
    bool deleteArea( BaseAreaPtr area );
    bool deleteArea( uint64_t id );

    // biz relations
    BaseBizRelationPtrs getBizRelationByArea( uint64_t id );
    BaseBizRelationPtrs getBizRelationByArea( uint64_t id, BaseBizRelation::RelationType type );

    // metadata
    bool isMetadataPresent( uint64_t id, QString name );
    QVariant getMetadataValue( uint64_t id, QString name );
    BaseMetadataPtr getMetadata( uint64_t id, QString name );
    MetadataByName getMetadataMap( uint64_t id );
    bool setMetadataValue(uint64_t id, QString name , QVariant val);
    bool addMetadata( uint64_t id, QString type,
                      QString name, QVariant val = QVariant() );
    bool addMetadata( BaseMetadataPtr data );
    bool deleteMetadata( uint64_t id, QString name );

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
    GroupEntityPtrs getGroups( GroupEntity::GroupType type );
    GroupEntityPtrs getGroupsOfRoomByFacility( FacilityPtr facility );
    GroupEntityPtrs getGroupsOfRoomByFacility( FacilityPtr facility, GroupEntity::GroupType type );
    GroupEntityPtr getGroupOfEntity( uint64_t id );
    GroupEntityPtr addGroup( GroupEntity::GroupType type );
    bool commitGroup( uint64_t id );
    bool commitGroup( GroupEntityPtr group );
    bool commitGroupsChanged();
    bool deleteGroup( GroupEntityPtr group );
    bool deleteGroup( uint64_t id );

    // layers
    LayerPtr getLayer( uint64_t id );
    LayerPtrs getLayers();
    LayerManagerPtr getLayerManager();
    LayerPtr addLayer( QString name );
    bool commitLayers();
    bool deleteLayer( LayerPtr layer );
    bool deleteLayer( uint64_t id );

    // layers get by data
    LayerPtr getLayerOfMark( uint64_t id );
    LayerPtr getLayerOfMark( MarkPtr mark );
    LayerPtr getLayerOfFile( BaseFileKeeperPtr file );
    LayerPtr getLayerOfFile( QString path );
    LayerPtr getLayerOfMetadataName( QString name );

    // layers signals
    //! signal on layer added. Signature:
    //! slot( uint64_t )
    void subscribeLayerAdded( QObject* obj,
                              const char *slot,
                              bool queue = false);
    //! signal on layers changed. Signature:
    //! slot( )
    void subscribeLayersChanged( QObject* obj,
                                 const char *slot,
                                 bool queue = false);
    //! signal on layer deleted. Signature:
    //! slot( uint64_t )
    void subscribeLayerDeleted( QObject* obj,
                                const char *slot,
                                bool queue = false);
    //! signal on layers change order. Signature:
    //! slot( )
    void subscribeLayersChangedOrder( QObject* obj,
                                      const char *slot,
                                      bool queue = false);
    //! signal on layer change showed. Signature:
    //! slot( uint64_t id, bool showed )
    void subscribeLayerChangeShowed( QObject* obj,
                                     const char *slot,
                                     bool queue = false);

    // transform matrixes
    static TransformMatrixManagerPtr getTransformManager();
    bool isHaveTransform( uint64_t facility_id );
    QTransform getTransform( uint64_t facility_id );
    bool setTransform( uint64_t facility_id, QTransform transform );
    void resetTransform( uint64_t facility_id );
    bool commitTransformOfFacility( uint64_t facility_id );
    bool commitTransformOfFacility( FacilityPtr facility );

    // files
    BaseFileKeeperPtrs getFilesByEntity( uint64_t id );
    BaseFileKeeperPtrs getFilesByEntity( BaseEntityPtr ptr );
    BaseFileKeeperPtrs getFilesByEntity( uint64_t id, BaseFileKeeper::FileType type );
    BaseFileKeeperPtrs getFilesByEntity( BaseEntityPtr ptr, BaseFileKeeper::FileType type );
    BaseFileKeeperPtr getFileByPath( QString path );
    BaseFileKeeperPtr addFile( QString file_path ,
                               BaseFileKeeper::FileType type,
                               uint64_t entity_id );
    BaseFileKeeperPtrs getFilesOnProcess();
    bool isHasFilesOnProcess();

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

    // graph
    GraphEntityPtr getGraph( uint64_t id );
    bool isAreaHasGraph( uint64_t area_id );
    bool isAreaHasGraph( BaseAreaPtr area );
    GraphEntityPtr getGraphOfArea( uint64_t area_id );
    GraphEntityPtr getGraphOfArea( BaseAreaPtr area );
    GraphEntityPtr addGraph( uint64_t area_id );
    GraphEntityPtr addGraph( BaseAreaPtr area );
    bool commitGraph( GraphEntityPtr graph );
    bool commitGraph( uint64_t graph_id );
    bool deleteGraph( uint64_t graph_id );
    bool deleteGraph( GraphEntityPtr graph );

private:
    RegionBizManager();

    static void onExit();
    QVariantMap loadJsonConfig( QString &file_path );
    bool processPlugins(QVariantMap settings);
    bool processTranslators( QVariantMap settings );
    void initConstraintsManager(QVariantMap settings );
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
