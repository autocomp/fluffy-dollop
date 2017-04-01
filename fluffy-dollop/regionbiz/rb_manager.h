#ifndef RB_MANAGER_H
#define RB_MANAGER_H

#include <memory>
#include <list>

#include "rb_locations.h"
#include "rb_translator.h"
#include "rb_signal_manager.h"
#include "rb_metadata.h"
#include "rb_marks.h"

#define FIND_IF( cont, func ) std::find_if( cont.begin(), cont.end(), func );

namespace regionbiz {

class RegionBizManager;
typedef std::shared_ptr< RegionBizManager > RegionBizManagerPtr;

class RegionBizManager
{
    friend class BaseTranslator;

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
    std::vector< RoomsGroupPtr > getRoomsGroupsByParent( uint64_t parent_id );
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
            appendArea( area );

            // emit signal
            _change_watcher.addEntity( area->getId() );
        }
        return area;
    }

    // delete
    bool deleteArea( BaseAreaPtr area );
    bool deleteArea( uint64_t id );

    // commit
    bool commitArea( BaseAreaPtr area );
    bool commitArea( uint64_t id );

    // biz relstions
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
    MarkPtrs getMarksByParent( MarksHolderPtr parent );
    MarkPtrs getMarks();
    MarkPtr addMark( uint64_t parent_id,
                     QPointF center = QPointF() );
    bool commitMark( uint64_t id );
    bool commitMark( MarkPtr mark );
    bool deleteMark( uint64_t id );
    bool deleteMark( MarkPtr mark );

    // tranlators
    BaseTranslatorPtr getTranslatorByName( QString name );

    // selection managment
    uint64_t getSelectedEntity();
    void selectEntity( uint64_t id );
    void subscribeOnSelect( QObject* obj,
                            const char *slot,
                            bool queue = false );
    void centerOnEntity( uint64_t id );
    void subscribeCenterOn( QObject* obj,
                            const char *slot,
                            bool queue = false );
    // other signals
    void subscribeOnChangeEntity( QObject* obj,
                                  const char *slot,
                                  bool queue = false );
    void subscribeOnDeleteEntity( QObject* obj,
                                  const char *slot,
                                  bool queue = false );
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
    void appendArea( BaseAreaPtr area );
    void removeArea( BaseAreaPtr area );
    void removeMark( MarkPtr mark );

    template< typename LocTypePtr >
    std::vector< LocTypePtr > getBaseLocationsByParent( uint64_t parent_id,
                                                        std::vector< LocTypePtr >& vector  );

    static RegionBizManagerPtr _regionbiz_mngr;
    BaseTranslatorPtr _translator = nullptr;

    // data locations
    std::vector< RegionPtr > _regions;
    std::vector< LocationPtr > _locations;
    std::vector< FacilityPtr > _facilitys;
    std::vector< FloorPtr > _floors;
    std::vector< RoomsGroupPtr > _rooms_groups;
    std::vector< RoomPtr > _rooms;

    // data relations
    std::vector< PropertyPtr > _propertys;
    std::vector< RentPtr > _rents;

    // metadata
    MetadataById _metadata;

    // marks
    MarkPtrs _marks;

    // signals
    SelectionManager _select_manager;
    ChangeEntitysWatcher _change_watcher;
};

}

#endif // RB_MANAGER_H
