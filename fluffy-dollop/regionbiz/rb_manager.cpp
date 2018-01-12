#include "rb_manager.h"

#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <functional>
#include <QPluginLoader>
#include <QFile>
#include <QJsonDocument>
#include <QVariant>
#include <QDebug>
#include <QDir>

#include "rb_entity_filter.h"
#include "rb_meta_constraints.h"

#ifdef STACKWALKER
#include "StackWalker/StackWalker.h"
#endif

using namespace regionbiz;

RegionBizManagerPtr RegionBizManager::instance()
{

#ifdef STACKWALKER
    SetUnhandledExceptionFilter(TopLevelFilter);
#endif

    static RegionBizManagerPtr instance_ptr = nullptr;
    if( !instance_ptr )
    {
        instance_ptr = RegionBizManagerPtr( new RegionBizManager() );
    }
    return instance_ptr;
}

bool RegionBizManager::init(QString &config_path)
{
    QVariantMap settings = loadJsonConfig( config_path );

    // plugins
    bool load_plugins = processPlugins( settings );

    // translators
    bool load_translators = processTranslators( settings );

    // init constraints manager
    initConstraintsManager( settings );

    return load_plugins && load_translators;
}

BaseEntityPtr RegionBizManager::getBaseEntity(uint64_t id)
{
    auto entity = BaseEntity::getEntity( id );
    if( EntityFilter::isFiltered( entity ))
        return entity;
    return nullptr;
}

bool RegionBizManager::isEntityConstraintsCorrect(uint64_t id)
{
    auto entity = getBaseEntity( id );
    if( entity )
        return isEntityConstraintsCorrect( entity );
    return false;
}

bool RegionBizManager::isEntityConstraintsCorrect( BaseEntityPtr entity )
{
    using namespace std;

    if( !entity )
        return false;

    #define CHECK_CONSTRAINTS( type_geted ) \
    auto type = type_geted; \
    /* check system metadata */ \
    auto contraints = ConstraintsManager::getConstraints( type, Constraint::CT_SYSTEM ); \
    for( Constraint& cons: contraints ) \
    { \
        if( !entity->isMetadataPresent( cons.getMetaName() )) \
        { \
            if( cons.getDefaultValue().isValid() ) \
            { \
                entity->addMetadata( cons.getMetaType(), \
                                     cons.getMetaName(), \
                                     cons.getDefaultValue() ); \
            } \
            else \
                return false; \
        } \
    } \
    /* check current metadata values */ \
    for( auto pair: entity->getMetadataMap() ) \
    { \
        BaseMetadataPtr data = pair.second; \
        if( !data->checkConstraits() ) \
            return false; \
    }

    CHECK_CONSTRAINTS( entity->getEntityType() );

    switch( type ) {
    case BaseEntity::ET_AREA:
    {
        CHECK_CONSTRAINTS( entity->convert< BaseArea >()->getType() );
        BaseAreaPtr area = entity->convert< BaseArea >();
        if( area->getType() == BaseArea::AT_ROOM )
        {
            CHECK_CONSTRAINTS( area->convert< Room >()->getRoomType() );
        }
        break;
    }

    case BaseEntity::ET_MARK:
    {
        CHECK_CONSTRAINTS( entity->convert< Mark >()->getMarkType() );
        break;
    }

    case BaseEntity::ET_GRAPH:
    {
        // NOTE don't constraints check for graph childs
        //auto nodes = entity->convert< GraphEntity >()->getNodes();
        //auto edges = entity->convert< GraphEntity >()->getEdges();
        break;
    }

    case BaseEntity::ET_GROUP:
    {
        CHECK_CONSTRAINTS( entity->convert< GroupEntity >()->getGroupType() );
        break;
    }

    case BaseEntity::ET_RELATION:
    case BaseEntity::ET_GRAPH_EDGE:
    case BaseEntity::ET_GRAPH_NODE:
    {
        // WARNING don't specific check constraints of group and relation
        break;
    }

    }

    #undef CHECK_CONSTRAINTS

    return true;
}

Constraints RegionBizManager::getConstraintsOfEntity(uint64_t id)
{
    auto entity = getBaseEntity( id );
    if( entity )
        return getConstraintsOfEntity( entity );
    return Constraints();
}

Constraints RegionBizManager::getConstraintsOfEntity( BaseEntityPtr entity )
{
    Constraints constraints;
    if( !entity )
        return constraints;

    auto type = entity->getEntityType();
    constraints = ConstraintsManager::getConstraints( type );

    switch( type ) {
    case BaseEntity::ET_AREA:
    {
        BaseAreaPtr area = entity->convert< BaseArea >();
        auto type = area->getType();
        auto cons_specific = ConstraintsManager::getConstraints( type );
        constraints.insert( constraints.end(), cons_specific.begin(), cons_specific.end() );

        if( area->getType() == BaseArea::AT_ROOM )
        {
            auto type = area->convert< Room >()->getRoomType();
            auto cons_specific = ConstraintsManager::getConstraints( type );
            constraints.insert( constraints.end(), cons_specific.begin(), cons_specific.end() );
        }
        break;
    }

    case BaseEntity::ET_MARK:
    {
        auto type = entity->convert< Mark >()->getMarkType();
        auto cons_specific = ConstraintsManager::getConstraints( type );
        constraints.insert( constraints.end(), cons_specific.begin(), cons_specific.end() );
        break;
    }

    case BaseEntity::ET_GROUP:
    {
        auto type = entity->convert< GroupEntity >()->getGroupType();
        auto cons_specific = ConstraintsManager::getConstraints( type );
        constraints.insert( constraints.end(), cons_specific.begin(), cons_specific.end() );
        break;
    }

    case BaseEntity::ET_RELATION:

    case BaseEntity::ET_GRAPH:
    case BaseEntity::ET_GRAPH_EDGE:
    case BaseEntity::ET_GRAPH_NODE:
    {
        // WARNING don't specific append constraints of group and relation
        break;
    }

    }

    return constraints;
}

Constraints RegionBizManager::getConstraintsOfEntity( uint64_t id, Constraint::ConstraintType type )
{
    auto entity = getBaseEntity( id );
    if( entity )
        return getConstraintsOfEntity( entity, type );
    return Constraints();
}

Constraints RegionBizManager::getConstraintsOfEntity( BaseEntityPtr entity, Constraint::ConstraintType type )
{
    Constraints constraints;
    if( !entity )
        return constraints;

    auto all_cons = getConstraintsOfEntity( entity );
    for( Constraint& cons: all_cons )
        if( cons.getType() == type )
            constraints.push_back( cons );

    return constraints;
}

BaseAreaPtr RegionBizManager::getBaseArea( uint64_t id )
{
    auto entity = BaseEntity::getEntity( id );

    BaseAreaPtr loc = BaseEntity::convert< BaseArea >( entity );
    return loc;
}

BaseAreaPtr RegionBizManager::getBaseArea( uint64_t id,
                                           BaseArea::AreaType type )
{
    BaseAreaPtr loc = getBaseArea( id );
    if( !loc )
        return nullptr;
    if( loc->getType() != type )
        return nullptr;

    return loc;
}

BaseAreaPtrs RegionBizManager::getAreaChildsByParent( uint64_t id )
{
    BaseAreaPtrs childs;

    BaseEntity::_mutex.lock();
    childs = getBaseAreasByParent< BaseArea >( id );
    BaseEntity::_mutex.unlock();

    return childs;
}

std::vector<RegionPtr> RegionBizManager::getRegions()
{
    return getBaseAreasByParent< Region >( 0 );
}

std::vector<LocationPtr> RegionBizManager::getLocationsByParent(uint64_t parent_id)
{
    return getBaseAreasByParent< Location >( parent_id );
}

std::vector<FacilityPtr> RegionBizManager::getFacilitysByParent(uint64_t parent_id)
{
    return getBaseAreasByParent< Facility >( parent_id );
}

std::vector<FloorPtr> RegionBizManager::getFloorsByParent( uint64_t parent_id )
{
    return getBaseAreasByParent< Floor >( parent_id );
}

std::vector<RoomPtr> RegionBizManager::getRoomsByParent(uint64_t parent_id)
{
    return getBaseAreasByParent< Room >( parent_id );
}

BaseAreaPtr RegionBizManager::addArea( BaseArea::AreaType type,
                                       uint64_t parent_id )
{
    BaseAreaPtr add_area;

    switch ( type ) {
    case BaseArea::AT_REGION:
        add_area = addArea< Region >( parent_id );
        break;
    case BaseArea::AT_LOCATION:
        add_area = addArea< Location >( parent_id );
        break;
    case BaseArea::AT_FACILITY:
        add_area = addArea< Facility >( parent_id );
        break;
    case BaseArea::AT_FLOOR:
        add_area = addArea< Floor >( parent_id );
        break;
    case BaseArea::AT_ROOM:
        add_area = addArea< Room >( parent_id );
        break;
    default:
        return nullptr;
    }

    return add_area;
}

BaseAreaPtr RegionBizManager::addArea( BaseArea::AreaType type,
                                       BaseAreaPtr parent )
{
    if( !parent )
        return nullptr;

    BaseAreaPtr add_area = addArea( type, parent->getId() );
    return add_area;
}

RoomPtr RegionBizManager::addRoom(Room::RoomType type, uint64_t parent_id)
{
    auto room = addArea< Room >( parent_id );
    if( !room )
        return false;

    auto room_conv = room->convert< Room >();
    bool seted = room_conv->setRoomType( type );
    if( !seted )
        return nullptr;

    return room_conv;
}

RoomPtr RegionBizManager::addRoom(Room::RoomType type, BaseAreaPtr parent)
{
    if( !parent )
        return nullptr;
    return addRoom( type, parent->getId() );
}

bool RegionBizManager::deleteArea(BaseAreaPtr area)
{
    if( !area )
        return false;

    // delete childs recursive
    auto childs = area->getBaseAreaChilds();
    for( BaseAreaPtr child: childs )
        deleteArea( child );

    // delete marks
    MarksHolderPtr holder = area->convert< MarksHolder >();
    if( holder )
    {
        auto marks = holder->getMarks();
        for( MarkPtr mark: marks )
            deleteMark( mark );
    }

    // emit signal
    _change_watcher.deleteEntity( area->getId() );

    // delete this one
    if( _data_translator )
    {
        bool del = _data_translator->deleteArea( area );
        return del;
    }
    return false;
}

bool RegionBizManager::deleteArea(uint64_t id)
{
    BaseAreaPtr area = getBaseArea( id );
    return deleteArea( area );
}

bool RegionBizManager::commitArea( BaseAreaPtr area )
{
    if( !isEntityConstraintsCorrect( area ))
    {
        std::cerr << "Incorrect Constraints for area: "
                  << area->getId() << std::endl;
        return false;
    }

    bool com = _data_translator->commitArea( area );
    if( com )
    {
        // emit signal
        _change_watcher.changeEntity( area->getId() );
    }
    return com;
}

bool RegionBizManager::commitArea( uint64_t id )
{
    BaseAreaPtr area = getBaseArea( id );
    return commitArea( area );
}

BaseBizRelationPtrs RegionBizManager::getBizRelationByArea(uint64_t id)
{
    BaseBizRelationPtrs relations = getBizRelationByArea( id, BaseBizRelation::RT_PROPERTY );

    BaseBizRelationPtrs relations_rent = getBizRelationByArea( id, BaseBizRelation::RT_RENT );
    for( BaseBizRelationPtr rent: relations_rent )
        relations.push_back( rent );

    return relations;
}

BaseBizRelationPtrs RegionBizManager::getBizRelationByArea( uint64_t id,
                                                            BaseBizRelation::RelationType type )
{
    BaseBizRelationPtrs relations;

    auto relations_ptrs =
            BaseEntity::getEntitysByTypeAndParent< BaseBizRelation >(
                BaseEntity::ET_RELATION, id  );

    for( BaseBizRelationPtr rel: relations_ptrs )
    {
        if( type == rel->getType() )
        {
            relations.push_back( rel );
        }
    }

    return relations;
}

BaseMetadataPtr RegionBizManager::getMetadata( uint64_t id, QString name )
{
    if( isMetadataPresent( id, name ))
    {
        BaseMetadata::getMutex().lock();
        auto data = BaseMetadata::getMetadatas()[id][name];
        BaseMetadata::getMutex().unlock();

        return data;
    }

    return EmptyMetadata::instance();
}

QVariant RegionBizManager::getMetadataValue(uint64_t id, QString name)
{
    QVariant val = getMetadata( id, name )->getValueAsVariant();
    return val;
}

MetadataByName RegionBizManager::getMetadataMap(uint64_t id)
{
    MetadataByName datas = MetadataByName();

    BaseMetadata::getMutex().lock();
    if( BaseMetadata::getMetadatas().find( id )
            != BaseMetadata::getMetadatas().end() )
    {
        datas = BaseMetadata::getMetadatas()[id];
    }
    BaseMetadata::getMutex().unlock();

    return datas;
}

bool RegionBizManager::isMetadataPresent( uint64_t id, QString name )
{
    BaseMetadata::getMutex().lock();
    bool present = ( BaseMetadata::getMetadatas().find( id )
                     != BaseMetadata::getMetadatas().end() ) &&
            ( BaseMetadata::getMetadatas()[id].find( name )
              != BaseMetadata::getMetadatas()[id].end() );
    BaseMetadata::getMutex().unlock();

    return present;
}

bool RegionBizManager::setMetadataValue( uint64_t id, QString name, QVariant val )
{
    if( isMetadataPresent( id, name ))
    {
        BaseMetadata::getMutex().lock();
        BaseMetadataPtr data = BaseMetadata::getMetadatas()[id][name];

        bool ok = data->setValueByVariant( val );
        BaseMetadata::getMutex().unlock();

        return ok;
    }

    return false;
}

bool RegionBizManager::addMetadata( uint64_t id, QString type,
                                    QString name, QVariant val )
{
    BaseMetadataPtr data = MetadataFabric::createMetadata( type, id );
    if( !data )
        return false;

    data->setName( name );
    if( !data->setValueByVariant( val ))
        return false;

    return addMetadata( data );
}

bool RegionBizManager::addMetadata( BaseMetadataPtr data )
{
    if( !isMetadataPresent( data->getParentId(), data->getName() ))
    {
        if( !data->checkConstraits() )
            return false;

        BaseMetadata::addForEntityByName( data );
        return true;
    }
    else
    {
        QString type = getMetadata( data->getParentId(), data->getName() )->getType();
        if( data->getType() != type )
        {
            std::cerr << "Can't set Metadata \""
                      << data->getName().toUtf8().data()
                      << "\" value with other type" << std::endl;
            return false;
        }

        bool set = setMetadataValue( data->getParentId(),
                                     data->getName(),
                                     data->getValueAsString() );
        return set;
    }

    return false;
}

bool RegionBizManager::deleteMetadata( uint64_t id, QString name )
{
    if( !isMetadataPresent( id, name ))
       return false;

    BaseMetadata::getMutex().lock();
    auto it = BaseMetadata::getMetadatas()[id].find( name );
    BaseMetadata::getMetadatas()[id].erase( it );
    BaseMetadata::getMutex().unlock();

    return true;
}

MarkPtr RegionBizManager::getMark( uint64_t id )
{
    auto entity = BaseEntity::getEntity( id );
    MarkPtr mark = BaseEntity::convert< Mark >( entity );
    return mark;
}

MarkPtrs RegionBizManager::getMarksByParent( uint64_t id )
{
    // get marks from entitys
    auto marks_ptrs = BaseEntity::getEntitysByTypeAndParent< Mark >(
                BaseEntity::ET_MARK, id );

    MarkPtrs marks;
    for( MarkPtr mark: marks_ptrs )
    {
        marks.push_back( mark );
    }

    return marks;
}

MarkPtrs RegionBizManager::getMarksByParent(uint64_t id, Mark::MarkType type)
{
    MarkPtrs res_marks;
    auto marks = getMarksByParent( id );
    for( MarkPtr mark: marks )
        if( mark->getMarkType() == type )
            res_marks.push_back( mark );

    return res_marks;
}

MarkPtrs RegionBizManager::getMarksByParent(MarksHolderPtr parent)
{
    return getMarksByParent( parent->getHolderId() );
}

MarkPtrs RegionBizManager::getMarksByParent(MarksHolderPtr parent, Mark::MarkType type)
{
    return getMarksByParent( parent->getHolderId(), type );
}

MarkPtrs RegionBizManager::getMarks()
{
    // get marks from entitys
    auto marks_ptrs = BaseEntity::getEntitysByType< Mark >( BaseEntity::ET_MARK );

    return marks_ptrs;
}

MarkPtr RegionBizManager::addMark( uint64_t parent_id,
                                   Mark::MarkType type,
                                   QPointF center )
{
    return addMark( parent_id, type, QPolygonF( { center } ));
}

MarkPtr RegionBizManager::addMark( uint64_t parent_id,
                                   Mark::MarkType type,
                                   QPolygonF coords )
{
    MarkPtr mark;
    BaseAreaPtr parent = getBaseArea( parent_id );
    if( !( parent->BaseEntity::convert< MarksHolder >() ))
        return mark;

    // add by type
    uint64_t id = BaseEntity::getMaxId() + 1;
    mark = MarkFabric::createByType( type, id, parent_id );
    if( mark )
    {
        mark->setCoords( coords );
        mark->setParentId( parent_id );

        // emit signal
        _change_watcher.addEntity( mark->getId() );
    }
    return mark;
}

bool RegionBizManager::commitMark(uint64_t id)
{
    MarkPtr mark = getMark( id );
    bool comm = commitMark( mark );

    return comm;
}

bool RegionBizManager::commitMark(MarkPtr mark)
{
    if( !isEntityConstraintsCorrect( mark ))
    {
        std::cerr << "Incorrect Constraints for mark: "
                  << mark->getId() << std::endl;
        return false;
    }

    bool comm = _data_translator->commitMark( mark );
    if( comm )
    {
        // emit signal
        _change_watcher.changeEntity( mark->getId() );
    }
    return comm;
}

bool RegionBizManager::deleteMark(uint64_t id)
{
    MarkPtr mark = getMark( id );
    bool del = deleteMark( mark );

    return del;
}

bool RegionBizManager::deleteMark(MarkPtr mark)
{
    if( !mark )
        return false;

    // emit signal
    _change_watcher.deleteEntity( mark->getId() );

    // delete
    bool del = _data_translator->deleteMark( mark );
    return del;
}

GroupEntityPtr RegionBizManager::getGroup(uint64_t id)
{
    auto entity = BaseEntity::getEntity( id );
    GroupEntityPtr group = BaseEntity::convert< GroupEntity >( entity );
    return group;
}

GroupEntityPtrs RegionBizManager::getGroups()
{
    // get groupd from entitys
    auto group_ptrs = BaseEntity::getEntitysByType< GroupEntity >( BaseEntity::ET_GROUP );

    return group_ptrs;
}

GroupEntityPtrs RegionBizManager::getGroups(GroupEntity::GroupType type)
{
    auto groups = getGroups();
    GroupEntityPtrs res;
    for( GroupEntityPtr gr: groups )
        if( type == gr->getGroupType() )
            res.push_back( gr );

    return res;
}

GroupEntityPtrs RegionBizManager::getGroupsOfRoomByFacility(
        FacilityPtr facility )
{
    GroupEntityPtrs res;
    for( GroupEntityPtr group: getGroups() )
    {
        if( group->isValid() && !group->isEmpty()
                && group->getType() == BaseArea::AT_ROOM )
        {
            for( BaseAreaPtr area: group->getElements() )
                if( area->getParent()->getParentId()
                        == facility->getId() )
                {
                    res.push_back( group );
                    break;
                }
        }
    }
    return res;
}

GroupEntityPtrs RegionBizManager::getGroupsOfRoomByFacility(
        FacilityPtr facility, GroupEntity::GroupType type )
{
    GroupEntityPtrs res;
    for( GroupEntityPtr gr: getGroupsOfRoomByFacility( facility ))
        if( type == gr->getGroupType() )
            res.push_back( gr );

    return res;
}

// TODO think how doing fast search
GroupEntityPtr RegionBizManager::getGroupOfEntity(uint64_t id)
{
    for( GroupEntityPtr group: getGroups() )
        for( uint64_t element_id : group->getElementsIds() )
            if( element_id == id )
                return group;

    return nullptr;
}

GroupEntityPtr RegionBizManager::addGroup( GroupEntity::GroupType type )
{
    uint64_t id = BaseEntity::getMaxId() + 1;
    GroupEntityPtr group = BaseEntity::createWithId< GroupEntity >( id );
    if( !group )
        return nullptr;

    group->setChanged();
    group->setGroupType( type );

    // emit signal
    _change_watcher.addBaseEntity( group->getId() );

    return group;
}

bool RegionBizManager::commitGroup(uint64_t id)
{
    GroupEntityPtr group = getGroup( id );
    bool comm = commitGroup( group );

    return comm;
}

bool RegionBizManager::commitGroup( GroupEntityPtr group )
{
    if( !isEntityConstraintsCorrect( group ))
    {
        std::cerr << "Incorrect Constraints for group: "
                  << group->getId() << std::endl;
        return false;
    }

    bool free_changes( true );
    std::vector< uint64_t > groups_on_commit =
            GroupWatcher::getChangedGroups( !free_changes );

    // if group in set of changed groups - cpmmit all (for sync)
    auto check_id = [ group ]( uint64_t id ){ return group->getId() == id; };
    auto iter = FIND_IF( groups_on_commit, check_id );
    if( iter != groups_on_commit.end() )
    {
        return commitGroupsChanged();
    }

    return true;
}

bool RegionBizManager::commitGroupsChanged()
{
    std::vector< uint64_t > groups_on_commit = GroupWatcher::getChangedGroups();

    GroupEntityPtrs groups;
    for( uint64_t id: groups_on_commit )
        groups.push_back( getGroup( id ));

    bool comm = _data_translator->commitGroups( groups );
    if( comm )
    {
        // signals for all groups
        for( uint64_t id: groups_on_commit )
            _change_watcher.changeEntity( id );
    }

    return comm;
}

bool RegionBizManager::deleteGroup(GroupEntityPtr group)
{
    if( !group )
        return false;

    // emit signal
    _change_watcher.deleteEntity( group->getId() );

    // delete
    bool del = _data_translator->deleteGroup( group );
    return del;
}

bool RegionBizManager::deleteGroup(uint64_t id)
{
    auto mngr = RegionBizManager::instance();
    auto group = mngr->getGroup( id );
    return deleteGroup( group );
}

LayerPtr RegionBizManager::getLayer(uint64_t id)
{
    return getLayerManager()->getLayer( id );
}

LayerPtrs RegionBizManager::getLayers()
{
    return getLayerManager()->getLayers();
}

LayerManagerPtr RegionBizManager::getLayerManager()
{
    return LayerManager::instance();
}

LayerPtr RegionBizManager::addLayer( QString name )
{
    auto layer = getLayerManager()->addLayer( name );
    if( layer )
        LayerManager::instance()->
                _signal_emiter.onLayerAdded( layer->getId() );
    return layer;
}

bool RegionBizManager::commitLayers()
{
    bool com = _data_translator->commitLayers();
    if( com )
        LayerManager::instance()->_signal_emiter.onLayersChanged();

    return com;
}

bool RegionBizManager::deleteLayer( LayerPtr layer )
{
    if( !layer )
        return false;

    // delete layer signal
    LayerManager::instance()->
            _signal_emiter.onLayerDeleted( layer->getId() );

    bool del = _data_translator->deleteLayer( layer );
    return del;
}

bool RegionBizManager::deleteLayer( uint64_t id )
{
    auto mngr = RegionBizManager::instance();
    LayerPtr layer = mngr->getLayer( id );
    if( layer )
        return deleteLayer( layer );

    return false;
}

LayerPtr RegionBizManager::getLayerOfMark(uint64_t id)
{
    auto mngr = RegionBizManager::instance();
    auto mark = mngr->getMark( id );
    return getLayerOfMark( mark );
}

LayerPtr RegionBizManager::getLayerOfMark(MarkPtr mark)
{
    if( !mark )
        return nullptr;

    for( LayerPtr layer: getLayers() )
        for( MarkPtr elem_mark: layer->getMarks() )
            if( mark->getId() == elem_mark->getId() )
                return layer;

    return nullptr;
}

LayerPtr RegionBizManager::getLayerOfFile(BaseFileKeeperPtr file)
{
    if( !file )
        return nullptr;

    for( LayerPtr layer: getLayers() )
        for( BaseFileKeeperPtr elem_file: layer->getFiles() )
            if( file->getPath() == elem_file->getPath() )
                return layer;

    return nullptr;
}

LayerPtr RegionBizManager::getLayerOfFile(QString path)
{
    auto mngr = RegionBizManager::instance();
    auto file = mngr->getFileByPath( path );
    return getLayerOfFile( file );
}

LayerPtr RegionBizManager::getLayerOfMetadataName(QString name)
{
    for( LayerPtr layer: getLayers() )
        for( QString elem_mark: layer->getMetadataNames() )
            if( name == elem_mark )
                return layer;

    return nullptr;
}

void RegionBizManager::subscribeLayerAdded(QObject *obj, const char *slot, bool queue)
{
    getLayerManager()->subscribeLayerAdded( obj, slot, queue );
}

void RegionBizManager::subscribeLayersChanged(QObject *obj, const char *slot, bool queue)
{
    getLayerManager()->subscribeLayersChanged( obj, slot, queue );
}

void RegionBizManager::subscribeLayerDeleted(QObject *obj, const char *slot, bool queue)
{
    getLayerManager()->subscribeLayerDeleted( obj, slot, queue );
}

void RegionBizManager::subscribeLayersChangedOrder(QObject *obj, const char *slot, bool queue)
{
    getLayerManager()->subscribeLayersChangedOrder( obj, slot, queue );
}

void RegionBizManager::subscribeLayerChangeShowed(QObject *obj, const char *slot, bool queue)
{
    getLayerManager()->subscribeLayerChangeShowed( obj, slot, queue );
}

TransformMatrixManagerPtr RegionBizManager::getTransformManager()
{
    return TransformMatrixManager::instance();
}

bool RegionBizManager::isHaveTransform(uint64_t facility_id)
{
    return getTransformManager()->isHaveTransform( facility_id );
}

QTransform RegionBizManager::getTransform(uint64_t facility_id)
{
    return getTransformManager()->getTransform( facility_id );
}

bool RegionBizManager::setTransform(uint64_t facility_id, QTransform transform)
{
    return getTransformManager()->setTransform( facility_id, transform );
}

void RegionBizManager::resetTransform(uint64_t facility_id)
{
    getTransformManager()->resetTransform( facility_id );
}

bool RegionBizManager::commitTransformOfFacility( uint64_t facility_id )
{
    auto facil = getBaseArea( facility_id )->convert< Facility >();
    return commitTransformOfFacility( facil );
}

bool RegionBizManager::commitTransformOfFacility(FacilityPtr facility)
{
    if( facility )
        return _data_translator->commitTransformMatrix( facility );
    return false;
}

BaseFileKeeperPtrs RegionBizManager::getFilesByEntity(uint64_t id)
{
    BaseFileKeeperPtrs files_keepers =
            BaseFileKeeper::getFileKeepersByEntity( id );
    return files_keepers;
}

BaseFileKeeperPtrs RegionBizManager::getFilesByEntity(BaseEntityPtr ptr)
{
    return getFilesByEntity( ptr->getId() );
}

BaseFileKeeperPtrs RegionBizManager::getFilesByEntity( uint64_t id, BaseFileKeeper::FileType type )
{
    BaseFileKeeperPtrs files_res;

    auto files = getFilesByEntity( id );
    for( BaseFileKeeperPtr file: files )
        if( file->getType() == type )
            files_res.push_back( file );

    return files_res;
}

BaseFileKeeperPtrs RegionBizManager::getFilesByEntity(BaseEntityPtr ptr, BaseFileKeeper::FileType type)
{
    return getFilesByEntity( ptr->getId(), type );
}

BaseFileKeeperPtr RegionBizManager::getFileByPath(QString path)
{
    return BaseFileKeeper::getFileByPath( path );
}

BaseFileKeeperPtr RegionBizManager::addFile( QString file_path,
                                             BaseFileKeeper::FileType type,
                                             uint64_t entity_id )
{
    BaseFileKeeperPtr file =
            _files_translator->addFile( file_path, type, entity_id );
    BaseFileKeeper::addFile( file, entity_id );

    return file;
}

BaseFileKeeperPtrs RegionBizManager::getFilesOnProcess()
{
    return _files_translator->getFilesOnProcess();
}

bool RegionBizManager::isHasFilesOnProcess()
{
    return _files_translator->isHasFilesOnProcess();
}

QFilePtr RegionBizManager::getLocalFile(BaseFileKeeperPtr file)
{
    QFilePtr qfile = _files_translator->getFile( file );
    return qfile;
}

BaseFileKeeper::FileState RegionBizManager::getFileState(BaseFileKeeperPtr file)
{
    auto state = _files_translator->getFileState( file );
    return state;
}

BaseFileKeeper::FileState RegionBizManager::syncFile(BaseFileKeeperPtr file)
{
    auto state = _files_translator->syncFile( file );
    return state;
}

bool RegionBizManager::deleteFile(BaseFileKeeperPtr file)
{
    bool del = _files_translator->deleteFile( file );
    if( del )
    {
        BaseFileKeeper::deleteFile( file );

        auto entity = getBaseEntity( file->getEntityId() );
        if( entity )
            // TODO commit files only
            return entity->commit();
    }
    return del;
}

bool RegionBizManager::commitFile( BaseFileKeeperPtr file )
{
    auto entity = getBaseEntity( file->getEntityId() );
    if( entity )
        // TODO commit files only
        return entity->commit();

    return false;
}

void RegionBizManager::subscribeFileSynced(QObject *obj, const char *slot)
{
    _files_translator->subscribeFileSynced( obj, slot );
}

void RegionBizManager::subscribeFileAdded(QObject *obj, const char *slot)
{
    _files_translator->subscribeFileAdded( obj, slot );
}

void RegionBizManager::subscribeFileDeleted(QObject *obj, const char *slot)
{
    _files_translator->subscribeFileDeleted( obj, slot );
}

BaseTranslatorPtr RegionBizManager::getTranslatorByName(QString name)
{
    BaseTranslatorPtr&& ptr =
            BaseTranslatorFabric::getTranslatorByName( name );
    return ptr;
}

//--------------------------------------------------

uint64_t RegionBizManager::getCurrentEntity()
{
    return _select_manager._current_entity_id;
}

void RegionBizManager::clearCurrent()
{
    setCurrentEntity( UNSELECTED_ID );
}

void RegionBizManager::setCurrentEntity(uint64_t id)
{
    _select_manager.setNewCurrentEntity( id );
}

void RegionBizManager::subscribeOnCurrentChange( QObject *obj, const char *slot, bool queue )
{
    QObject::connect( &_select_manager, SIGNAL( currentEntityChange(uint64_t,uint64_t) ),
                      obj, slot, ( queue ? Qt::QueuedConnection : Qt::DirectConnection ));
}

std::vector<uint64_t> RegionBizManager::getSelectedSet()
{
    std::vector< uint64_t > ids;
    for( uint64_t id : _select_manager._selected_set )
        ids.push_back( id );
    return ids;
}

void RegionBizManager::clearSelect()
{
    _select_manager.clearSelect();
}

void RegionBizManager::appendToSelectedSet(uint64_t id, bool force)
{
    _select_manager.appendToSeletedSet( { id }, force );
}

void RegionBizManager::appendToSelectedSet(std::vector<uint64_t> ids, bool force)
{
    _select_manager.appendToSeletedSet( ids, force );
}

void RegionBizManager::removeFromSelectedSet(uint64_t id, bool force)
{
    _select_manager.removeFromSeletedSet( { id }, force );
}

void RegionBizManager::removeFromSelectedSet(std::vector<uint64_t> ids, bool force)
{
    _select_manager.removeFromSeletedSet( ids, force );
}

void RegionBizManager::subscribeOnSelectedSetChange(QObject *obj, const char *slot, bool queue)
{
    QObject::connect( &_select_manager, SIGNAL( selectedSetChange( std::vector< uint64_t >,
                                                                   std::vector< uint64_t > )),
                      obj, slot, ( queue ? Qt::QueuedConnection : Qt::DirectConnection ));
}

void RegionBizManager::subscribeOnSelectedChange(QObject *obj, const char *slot, bool queue)
{
    QObject::connect( &_select_manager, SIGNAL( selectedSetChange() ),
                      obj, slot, ( queue ? Qt::QueuedConnection : Qt::DirectConnection ));
}

void RegionBizManager::centerOnEntity(uint64_t id)
{
    _select_manager.centerOnBaseEntity( id );
}

void RegionBizManager::subscribeCenterOn(QObject *obj, const char *slot, bool queue)
{
    QObject::connect( &_select_manager, SIGNAL( centerOnBaseEntity( uint64_t )),
                      obj, slot, ( queue ? Qt::QueuedConnection : Qt::DirectConnection ));
}

void RegionBizManager::subscribeOnChangeEntity(QObject *obj, const char *slot, bool queue)
{
    QObject::connect( &_change_watcher, SIGNAL( changeBaseEntity( uint64_t )),
                      obj, slot, ( queue ? Qt::QueuedConnection : Qt::DirectConnection ));
}

void RegionBizManager::subscribeOnDeleteEntity(QObject *obj, const char *slot, bool queue)
{
    QObject::connect( &_change_watcher, SIGNAL( deleteBaseEntity( uint64_t )),
                      obj, slot, ( queue ? Qt::QueuedConnection : Qt::DirectConnection ));
}

void RegionBizManager::subscribeOnAddEntity(QObject *obj, const char *slot, bool queue)
{
    QObject::connect( &_change_watcher, SIGNAL( addBaseEntity( uint64_t )),
                      obj, slot, ( queue ? Qt::QueuedConnection : Qt::DirectConnection ));
}

GraphEntityPtr RegionBizManager::getGraph(uint64_t id)
{
    auto entity = BaseEntity::getEntity( id );
    GraphEntityPtr graph = BaseEntity::convert< GraphEntity >( entity );
    return graph;
}

bool RegionBizManager::isAreaHasGraph(uint64_t area_id)
{
    auto area = getBaseArea( area_id );
    return isAreaHasGraph( area );
}

bool RegionBizManager::isAreaHasGraph(BaseAreaPtr area)
{
    bool has = (bool) getGraphOfArea( area );
    return has;
}

GraphEntityPtr RegionBizManager::getGraphOfArea(uint64_t area_id )
{
    auto area = getBaseArea( area_id );
    return getGraphOfArea( area );
}

GraphEntityPtr RegionBizManager::getGraphOfArea( BaseAreaPtr area )
{
    if( !area )
        return nullptr;

    GraphEntityPtr graph;

    BaseEntity::_mutex.unlock();
    for( auto& pair: BaseEntity::getEntitys() )
    {
        BaseEntityPtr ent_ch = pair.second;
        if( !ent_ch )
            continue;

        GraphEntityPtr graph_ch = BaseEntity::convert< GraphEntity >( ent_ch );
        if( !graph_ch )
            continue;

        if( area->getId() == graph_ch->getParentId() )
        {
            graph = graph_ch;
            break;
        }
    }
    BaseEntity::_mutex.unlock();

    return graph;
}

GraphEntityPtr RegionBizManager::addGraph( uint64_t area_id )
{
    using namespace std;

    auto parent = getBaseArea( area_id );
    if( !parent )
    {
        cerr << "Can't find Graph parent " << area_id << endl;
        return nullptr;
    }

    if( parent->hasGraph() )
    {
        cerr << "Graph parent " << area_id
             << " already has graph" << endl;
        return nullptr;
    }

    uint64_t id = BaseEntity::getMaxId() + 1;
    GraphEntityPtr graph = BaseEntity::createWithId< GraphEntity >( id, area_id );

    if( graph )
    {
        graph->setParentId( area_id );
        Q_EMIT _change_watcher.addBaseEntity( graph->getId() );
    }
    return graph;
}

GraphEntityPtr RegionBizManager::addGraph(BaseAreaPtr area)
{
    if( !area )
        return nullptr;
    return addGraph( area->getId() );
}

bool RegionBizManager::commitGraph(GraphEntityPtr graph)
{
    if( graph )
    {
        bool correct = isEntityConstraintsCorrect( graph );
        if( correct )
            for( auto node: graph->getNodes() )
                correct &= isEntityConstraintsCorrect( node );
        if( correct )
            for( auto edge: graph->getEdges() )
                correct &= isEntityConstraintsCorrect( edge );

        if( !correct )
        {
            std::cerr << "Incorrect Constraints for graph: "
                      << graph->getId() << std::endl;
            return false;
        }

        bool comm = _data_translator->commitGraph( graph );
        if( comm )
        {
            // emit signal
            _change_watcher.changeEntity( graph->getId() );
        }
    }
    else
        return false;

    return true;
}

bool RegionBizManager::commitGraph( uint64_t graph_id )
{
    auto ent = getBaseEntity( graph_id );
    if( ent && BaseEntity::ET_GRAPH == ent->getEntityType() )
    {
        bool res = commitGraph( ent->convert< GraphEntity >() );
        return res;
    }
    return false;
}

bool RegionBizManager::deleteGraph(uint64_t graph_id)
{
    auto ent = getBaseEntity( graph_id );
    if( ent && BaseEntity::ET_GRAPH == ent->getEntityType() )
    {
        bool res = deleteGraph( ent->convert< GraphEntity >() );
        return res;
    }
    return false;
}

bool RegionBizManager::deleteGraph( GraphEntityPtr graph )
{
    if( !graph )
        return false;

    // emit signal
    _change_watcher.deleteEntity( graph->getId() );

    // delete
    bool del = _data_translator->deleteGraph( graph );
    return del;
}

RegionBizManager::RegionBizManager()
{
    std::atexit( onExit );
}

void RegionBizManager::onExit()
{
    std::cerr << "Region Biz Manager stopped" << std::endl;
    bool clear_entitys( false );
    RegionBizManager::instance()->clearCurrentData( clear_entitys );
}

QVariantMap RegionBizManager::loadJsonConfig( QString& file_path )
{
    // replace '~' in config
    if( file_path.startsWith ( "~/" ))
        file_path.replace (0, 1, QDir::homePath());

    // open
    QString file_in;
    QFile file( file_path );
    if( file.open( QFile::ReadOnly | QFile::Text ))
    {
        file_in = file.readAll();
        file.close();
    }
    else
    {
        std::cerr << "Region Biz can't open config file: "
                  << file_path.toUtf8().data() << std::endl;
    }

    // parce json
    QJsonParseError err;
    QJsonDocument json_doc = QJsonDocument::fromJson( file_in.toUtf8(), &err );
    if( err.error != QJsonParseError::NoError )
        std::cerr << err.errorString().toUtf8().data();

    // return result
    QVariantMap settings = json_doc.toVariant().toMap();
    return settings;
}

bool RegionBizManager::processPlugins( QVariantMap settings )
{
    if( settings.contains( "plugins" ))
    {
        QVariantMap plugins_settings = settings["plugins"].toMap();
        if( plugins_settings.contains( "plugins_path" ))
        {
            QString path = plugins_settings[ "plugins_path" ].toString();
            bool load_list = true;

            if( plugins_settings.contains( "plugins_load" ))
            {
                QString plugins_load = plugins_settings[ "plugins_load" ].toString();
                if( "all" == plugins_load )
                {
                    //load_list = false;

                    bool load_all( true );
                    return loadPlugins( path, load_all );
                }
                else if( "no" == plugins_load )
                {
                    load_list = false;
                }
                else if( "list" != plugins_load )
                {
                    std::cerr << "Incorrect plugins_load parameter: "
                              << plugins_load.toUtf8().data() << std::endl;
                    return false;
                }
            }

            if( load_list )
            {
                if( plugins_settings.contains( "plugins_list" ))
                {
                    QStringList list = plugins_settings[ "plugins_list" ].toStringList();
                    bool load_all( true );
                    return loadPlugins( path, !load_all, list );
                }
            }
        }
    }
    else
        std::cout << "No plugins" << std::endl;

    return true;
}

bool RegionBizManager::processTranslators(QVariantMap settings)
{
    bool no_error = true;

    if( settings.contains( "translators" ))
    {
        QVariantList translator_settings_list = settings["translators"].toList();
        for( QVariant translator_settings_var: translator_settings_list )
        {
            QVariantMap translator_settings = translator_settings_var.toMap();

            QString translator_name = translator_settings["name"].toString();
            BaseTranslatorPtr translator = getTranslatorByName( translator_name );
            if( translator )
            {
                QVariantMap translator_params;
                if( translator_settings.contains( "params" ))
                    translator_params = translator_settings[ "params" ].toMap();

                bool init_correct = translator->init( translator_params );
                if( !init_correct )
                    no_error = false;
            }
            else
            {
                std::cerr << "Region Biz Translator \"" << translator_name.toUtf8().data()
                          << "\" don't created" << std::endl;
                no_error = false;
            }
        }
    }

    if( settings.contains( "main_translators" ))
    {
        QVariantMap translators = settings[ "main_translators" ].toMap();

        if( translators.contains( "data" ))
        {
            QString name_data_translator = translators[ "data" ].toString();
            BaseTranslatorPtr base_trans = getTranslatorByName( name_data_translator );
            if( base_trans )
                _data_translator = BaseTranslator::convert< BaseDataTranslator >( base_trans );
            if( !_data_translator )
            {
                std::cerr << "Main Data Translator not fonud" << std::endl;
                no_error = false;
            }
            else
            {
                QString error_text = "";
                if( _data_translator->checkTranslator( BaseDataTranslator::CT_READ, error_text ))
                {
                    loadDataByTranslator();
                }
                else
                {
                    std::cerr << "Error with data Translator:\n" << error_text.toUtf8().data() << std::endl;
                    _data_translator = nullptr;

                    no_error = false;
                }
            }
        }
        else
            no_error = false;

        if( translators.contains( "files" ))
        {
            QString name_files_translator = translators[ "files" ].toString();
            BaseTranslatorPtr base_trans = getTranslatorByName( name_files_translator );
            if( base_trans )
                _files_translator = BaseTranslator::convert< BaseFilesTranslator >( base_trans );
            if( !_files_translator )
            {
                std::cerr << "Main Files Translator not fonud" << std::endl;
                no_error = false;
            }
        }
        else
            no_error = false;
    }
    else no_error = false;

    return no_error;
}

bool RegionBizManager::loadPlugins( QString plugins_path, bool load_all,
                                    QStringList plugins )
{
    bool no_error = true;

    QDir plugin_dir( plugins_path );
    QFileInfoList file_infos;
    if( load_all)
        file_infos = plugin_dir.entryInfoList( QDir::Files );
    else
    {
        for( QString plugin: plugins )
        {
            QFileInfo file_info( plugins_path + QDir::separator() + plugin );
            file_infos.append( file_info );
        }
    }

    for( QFileInfo file_info: file_infos )
    {
        std::cout << "Start loading \"" << file_info.fileName().toUtf8().data()
                  << "\"" << std::endl;

        QPluginLoader loader( file_info.filePath() );
        if( loader.load() )
            std::cout << "Plugin \"" << file_info.fileName().toUtf8().data()
                      << "\" loaded" << std::endl;
        else
        {
            std::cerr << "Error while loading \"" << file_info.fileName().toUtf8().data()
                      << "\"\n" << loader.errorString().toUtf8().data() << std::endl;

            no_error = false;
        }
    }

    return no_error;
}

void RegionBizManager::loadDataByTranslator()
{
    if( !_data_translator )
        return;

    clearCurrentData();

    // regions
    _data_translator->loadRegions();
    // locations
    _data_translator->loadLocations();
    // facilitys
    _data_translator->loadFacilitys();
    // floors
    _data_translator->loadFloors();
    // rooms
    _data_translator->loadRooms();

    // TODO load rents and propertys
//    // propertys
//    auto prop_vec = _data_translator->loadPropertys();
//    for( PropertyPtr prop: prop_vec )
//        _propertys.push_back( prop );

//    // rents
//    auto rent_vec = _data_translator->loadRents();
//    for( RentPtr rent: rent_vec )
//        _rents.push_back( rent );

    // metadata
    _data_translator->loadMetadata();

    // marks
    _data_translator->loadMarks();

    // files
    _data_translator->loadFiles();

    // groups
    _data_translator->loadGroups();

    // layers
    _data_translator->loadLayers();

    // transform
    _data_translator->loadTransformMatrixes();

    // graphs
    _data_translator->loadGraphs();
}

void RegionBizManager::clearCurrentData( bool clear_entitys )
{
    //data
    BaseMetadata::getMutex().lock();
    BaseMetadata::getMetadatas().clear();
    BaseMetadata::getMutex().unlock();

    // entitys
    if( clear_entitys )
    {
        BaseEntity::_mutex.lock();
        BaseEntity::getEntitys().clear();
        BaseEntity::getChildsOfParent().clear();
        BaseEntity::_mutex.unlock();
    }
}

void RegionBizManager::initConstraintsManager( QVariantMap settings )
{
    if( !settings.contains( "constraints" ))
        return;

    QVariantMap cons_settings = settings[ "constraints" ].toMap();
    if( !cons_settings.contains( "constraints_aim" ))
        return;

    QString aim = cons_settings[ "constraints_aim" ].toString();
    QString file = cons_settings[ "constraints_file" ].toString();
    ConstraintsManager::init( aim, file );
}

template<typename LocType>
std::vector< std::shared_ptr< LocType >>
RegionBizManager::getBaseAreasByParent( uint64_t parent_id )
{
    // check parent id
    std::function< bool( BaseAreaPtr ) > check_parent_id =
            [ parent_id ]( BaseAreaPtr bl )
    {
        // if region - return true
        if( typeid( LocType ) == typeid( Region ))
            return true;
        return parent_id == bl->getParentId();
    };

    // WARNING filtered
    std::function< bool( BaseAreaPtr ) > check_filter =
            []( BaseAreaPtr bl ){ return EntityFilter::isFiltered( bl ); };

    // result
    std::vector< std::shared_ptr< LocType >> loc_childs;

    // for objects with parent
    if( parent_id )
    {
        // get areas by parent
        BaseAreaPtrs areas = BaseEntity::getEntitysByTypeAndParent< BaseArea >(
                    BaseEntity::ET_AREA, parent_id );

        // check all areas
        for( BaseAreaPtr ptr: areas )
        {
            std::shared_ptr< LocType > loc_ptr = BaseArea::convert< LocType >( ptr );
            if( !loc_ptr )
                continue;

            if( check_filter( ptr ))
                loc_childs.push_back( loc_ptr );
        }
    }
    // for other objects
    else
    {
        // get areas
        BaseAreaPtrs areas = BaseEntity::getEntitysByType< BaseArea >( BaseEntity::ET_AREA );

        // check all areas
        for( BaseAreaPtr ptr: areas )
        {
            std::shared_ptr< LocType > loc_ptr = BaseArea::convert< LocType >( ptr );
            if( !loc_ptr )
                continue;

            if( check_parent_id( ptr )
                    && check_filter( ptr ))
                loc_childs.push_back( loc_ptr );
        }
    }

    return loc_childs;
}
