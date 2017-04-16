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

using namespace regionbiz;

RegionBizManagerPtr RegionBizManager::instance()
{
    static RegionBizManagerPtr instance_ptr = nullptr;
    if( !instance_ptr )
        instance_ptr = RegionBizManagerPtr( new RegionBizManager() );
    return instance_ptr;
}

bool RegionBizManager::init(QString &config_path)
{
    QVariantMap settings = loadJsonConfig( config_path );

    // plugins
    bool load_plugins = processPlugins( settings );

    // translators
    bool load_translators = processTranslators( settings );

    return load_plugins && load_translators;
}

BaseEntityPtr RegionBizManager::getBaseEntity(uint64_t id)
{
    auto entity = BaseEntity::getEntity( id );
    if( EntityFilter::isFiltered( entity ))
        return entity;
    return nullptr;
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
    for( auto pair: BaseEntity::getEntitys() )
    {
        BaseEntityPtr ent_ch = pair.second;
        if( !ent_ch )
            continue;

        BaseAreaPtr area_ch = BaseEntity::convert< BaseArea >( ent_ch );
        if( !area_ch )
            continue;

        if( id == area_ch->getParentId() )
        {
            // WARNING filtered child
            bool filtered = EntityFilter::isFiltered( ent_ch );
            if( filtered )
                childs.push_back( area_ch );
        }
    }

    return childs;
}

std::vector<RegionPtr> RegionBizManager::getRegions()
{
    return _regions;
}

std::vector<LocationPtr> RegionBizManager::getLocationsByParent(uint64_t parent_id)
{
    return getBaseLocationsByParent< LocationPtr >( parent_id, _locations );
}

std::vector<FacilityPtr> RegionBizManager::getFacilitysByParent(uint64_t parent_id)
{
    return getBaseLocationsByParent< FacilityPtr >( parent_id, _facilitys );
}

std::vector<FloorPtr> RegionBizManager::getFloorsByParent( uint64_t parent_id )
{
    return getBaseLocationsByParent< FloorPtr >( parent_id, _floors );
}

std::vector<RoomsGroupPtr> RegionBizManager::getRoomsGroupsByParent(uint64_t parent_id)
{
    return getBaseLocationsByParent< RoomsGroupPtr >( parent_id, _rooms_groups );
}

std::vector<RoomPtr> RegionBizManager::getRoomsByParent(uint64_t parent_id)
{
    return getBaseLocationsByParent< RoomPtr >( parent_id, _rooms );
}

BaseAreaPtr RegionBizManager::addArea( BaseArea::AreaType type,
                                       uint64_t parent_id )
{
    BaseAreaPtr add_area;

    switch ( type ) {
    case BaseArea::AT_REGION:
        add_area = addArea< Region >( parent_id );
    case BaseArea::AT_LOCATION:
        add_area = addArea< Location >( parent_id );
    case BaseArea::AT_FACILITY:
        add_area = addArea< Facility >( parent_id );
    case BaseArea::AT_FLOOR:
        add_area = addArea< Floor >( parent_id );
    case BaseArea::AT_ROOMS_GROUP:
        add_area = addArea< RoomsGroup >( parent_id );
    case BaseArea::AT_ROOM:
        add_area = addArea< Room >( parent_id );
    default:
        return nullptr;
    }

    return add_area;
}

bool RegionBizManager::deleteArea(BaseAreaPtr area)
{
    // delete childs recursive
    auto childs = area->getChilds();
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

    // delete this one
    bool del = _translator->deleteArea( area );
    if( del )
    {
        // emit signal
        _change_watcher.deleteEntity( area->getId() );
    }
    return del;
}

bool RegionBizManager::deleteArea(uint64_t id)
{
    BaseAreaPtr area = getBaseArea( id );
    return deleteArea( area );
}

bool RegionBizManager::commitArea( BaseAreaPtr area )
{
    bool com = _translator->commitArea( area );
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
                                                            BaseBizRelation::RelationType type)
{
    BaseBizRelationPtrs relations;

    switch( type )
    {
    case BaseBizRelation::RT_PROPERTY:
    {
        for( PropertyPtr prop: _propertys )
            if( id == prop->getAreaId() )
                relations.push_back( prop );

        break;
    }

    case BaseBizRelation::RT_RENT:
    {
        for( RentPtr rent: _rents )
            if( id == rent->getAreaId() )
                relations.push_back( rent );

        break;
    }

    }

    return relations;
}

BaseMetadataPtr RegionBizManager::getMetadata( uint64_t id, QString name )
{
    if( isMetadataPresent( id, name ))
        return _metadata[id][name];

    return nullptr;
}

QVariant RegionBizManager::getMetadataValue(uint64_t id, QString name)
{
    QVariant val = getMetadata( id, name )->getValueAsVariant();
    return val;
}

MetadataByName RegionBizManager::getMetadataMap(uint64_t id)
{
    if( _metadata.find( id ) != _metadata.end() )
            return _metadata[id];

    return MetadataByName();
}

bool RegionBizManager::isMetadataPresent( uint64_t id, QString name )
{
    bool present = ( _metadata.find( id ) != _metadata.end() ) &&
            ( _metadata[id].find( name ) != _metadata[id].end() );

    return present;
}

bool RegionBizManager::setMetadataValue( uint64_t id, QString name, QVariant val )
{
    if( isMetadataPresent( id, name ))
    {
        BaseMetadataPtr data = _metadata[id][name];
        data->setValueByVariant( val );
        return true;
    }

    return false;
}

bool RegionBizManager::addMetadata( uint64_t id, QString type,
                                    QString name, QVariant val )
{
    BaseMetadataPtr data = MetadataFabric::createMetadata( type, id );
    data->setName( name );
    data->setValueByVariant( val );

    return addMetadata( data );
}

bool RegionBizManager::addMetadata( BaseMetadataPtr data )
{
    if( !isMetadataPresent( data->getParentId(), data->getName() ))
    {
       _metadata[data->getParentId()][data->getName()] = data;

       return true;
    }
    else
    {
        setMetadataValue( data->getParentId(), data->getName(), data->getValueAsString() );
        return true;
    }

    return false;
}

MarkPtr RegionBizManager::getMark( uint64_t id )
{
    auto entity = BaseEntity::getEntity( id );
    MarkPtr mark = BaseEntity::convert< Mark >( entity );
    return mark;
}

MarkPtrs RegionBizManager::getMarksByParent( uint64_t id )
{
    std::function< bool( MarkPtr ) > check_id =
            [ id ]( MarkPtr mark ){ return id == mark->getParentId(); };

    MarkPtrs marks;
    for( MarkPtr mark: _marks )
    {
        if( check_id( mark ))
            marks.push_back( mark );
    }

    return marks;
}

MarkPtrs RegionBizManager::getMarksByParent(MarksHolderPtr parent)
{
    return getMarksByParent( parent->getHolderId() );
}

MarkPtrs RegionBizManager::getMarks()
{
    return _marks;
}

MarkPtr RegionBizManager::addMark( uint64_t parent_id,
                                   QPointF center )
{
    MarkPtr mark;
    BaseAreaPtr parent = getBaseArea( parent_id );
    if( !( parent->BaseEntity::convert< MarksHolder >() ))
        return mark;

    mark = BaseEntity::createWithId< Mark >( BaseEntity::getMaxId() + 1 );
    if( mark )
    {
        _marks.push_back( mark );
        mark->setCenter( center );
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
    bool comm = _translator->commitMark( mark );
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
    bool del = _translator->deleteMark( mark );
    if( del )
    {
        // emit signal
        _change_watcher.deleteEntity( mark->getId() );
    }
    return del;
}

BaseTranslatorPtr RegionBizManager::getTranslatorByName(QString name)
{
    BaseTranslatorPtr&& ptr =
            BaseTranslatorFabric::getTranslatorByName( name );
    return ptr;
}

uint64_t RegionBizManager::getSelectedEntity()
{
    return _select_manager._selected_entity_id;
}

std::set<uint64_t>& RegionBizManager::getSelectedSet()
{
    return _select_manager._selected_set;
}

void RegionBizManager::selectEntity(uint64_t id)
{
    _select_manager.selectNewEntity( id );
}

void RegionBizManager::subscribeOnSelect(QObject *obj, const char *slot, bool queue)
{
    QObject::connect( &_select_manager, SIGNAL( selectBaseEntity(uint64_t,uint64_t) ),
                      obj, slot, ( queue ? Qt::QueuedConnection : Qt::DirectConnection ));
}

void RegionBizManager::clearSelect()
{
    _select_manager.clearSelect();
}

void RegionBizManager::appendToSelectedSet(uint64_t id)
{
    _select_manager.appendToSeletedSet( id );
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

void RegionBizManager::subscribeSelectedSet(QObject *obj, const char *slot, bool queue)
{
    QObject::connect( &_select_manager, SIGNAL( selectedSet( std::set< uint64_t > )),
                      obj, slot, ( queue ? Qt::QueuedConnection : Qt::DirectConnection ));
}

void RegionBizManager::subscribeClearSelect(QObject *obj, const char *slot, bool queue)
{
    QObject::connect( &_select_manager, SIGNAL( selectClear() ),
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
                    load_list = false;

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

    QString error_text = "";
    if( settings.contains( "main_translator" ))
    {
        QString name = settings[ "main_translator" ].toString();
        _translator = getTranslatorByName( name );
        if( _translator->checkTranslator( BaseTranslator::CT_READ, error_text ))
        {
            loadDataByTranslator();
        }
        else
        {
            std::cerr << "Error with Translator:\n" << error_text.toUtf8().data() << std::endl;
            _translator = nullptr;

            no_error = false;
        }
    }

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
    if( !_translator )
        return;

    clearCurrentData();

    // regions
    _translator->loadRegions();
    // locations
    _translator->loadLocations();
    // facilitys
    _translator->loadFacilitys();
    // floors
    _translator->loadFloors();
    // rooms groups
    _translator->loadRoomsGroups();
    // rooms
    _translator->loadRooms();

    // TODO load rents and propertys
//    // propertys
//    auto prop_vec = _translator->loadPropertys();
//    for( PropertyPtr prop: prop_vec )
//        _propertys.push_back( prop );

//    // rents
//    auto rent_vec = _translator->loadRents();
//    for( RentPtr rent: rent_vec )
//        _rents.push_back( rent );

    // metadate
    _translator->loadMetadata();

    // marks
    _translator->loadMarks();
}

void RegionBizManager::clearCurrentData( bool clear_entitys )
{
    // areas
    _regions.clear();
    _locations.clear();
    _facilitys.clear();
    _floors.clear();
    _rooms_groups.clear();
    _rooms.clear();

    // relations
    _rents.clear();
    _propertys.clear();

    //data
    _metadata.clear();
    _marks.clear();

    // entitys
    if( clear_entitys )
        BaseEntity::getEntitys().clear();
}

void RegionBizManager::appendArea( BaseAreaPtr area )
{
    switch ( area->getType() ) {
    case BaseArea::AT_REGION:
        _regions.push_back( BaseArea::convert< Region >( area ));
        break;
    case BaseArea::AT_LOCATION:
        _locations.push_back( BaseArea::convert< Location >( area ));
        break;
    case BaseArea::AT_FACILITY:
        _facilitys.push_back( BaseArea::convert< Facility >( area ));
        break;
    case BaseArea::AT_FLOOR:
        _floors.push_back( BaseArea::convert< Floor >( area ));
        break;
    case BaseArea::AT_ROOMS_GROUP:
        _rooms_groups.push_back( BaseArea::convert< RoomsGroup >( area ));
        break;
    case BaseArea::AT_ROOM:
        _rooms.push_back( BaseArea::convert< Room >( area ));
        break;
    default:
        break;
    }
}

void RegionBizManager::removeArea(BaseAreaPtr area)
{
    std::function< bool( BaseAreaPtr ) > check_id =
            [ area ]( BaseAreaPtr ba ){ return area->getId() == ba->getId(); };

    switch ( area->getType() ) {
    case BaseArea::AT_REGION:
    {
        auto iter = FIND_IF( _regions, check_id );
        if( iter != _regions.end() )
            _regions.erase( iter );
        break;
    }
    case BaseArea::AT_LOCATION:
    {
        auto iter = FIND_IF( _locations, check_id );
        if( iter != _locations.end() )
            _locations.erase( iter );
        break;
    }
    case BaseArea::AT_FACILITY:
    {
        auto iter = FIND_IF( _facilitys, check_id );
        if( iter != _facilitys.end() )
            _facilitys.erase( iter );
        break;
    }
    case BaseArea::AT_FLOOR:
    {
        auto iter = FIND_IF( _floors, check_id );
        if( iter != _floors.end() )
            _floors.erase( iter );
        break;
    }
    case BaseArea::AT_ROOMS_GROUP:
    {
        auto iter = FIND_IF( _rooms_groups, check_id );
        if( iter != _rooms_groups.end() )
            _rooms_groups.erase( iter );
        break;
    }
    case BaseArea::AT_ROOM:
    {
        auto iter = FIND_IF( _rooms, check_id );
        if( iter != _rooms.end() )
            _rooms.erase( iter );
        break;
    }
    default:
        break;
    }
}

void RegionBizManager::removeMark( MarkPtr mark )
{
    std::function< bool( MarkPtr ) > check_id =
            [ mark ]( MarkPtr m ){ return mark->getId() == m->getId(); };

    auto iter = FIND_IF( _marks, check_id );
    if( iter != _marks.end() )
        _marks.erase( iter );
}

template<typename LocTypePtr>
std::vector< LocTypePtr > RegionBizManager::getBaseLocationsByParent( uint64_t parent_id,
                                                                      std::vector< LocTypePtr >& vector )
{
    std::function< bool( BaseAreaPtr ) > check_parent_id =
            [ parent_id ]( BaseAreaPtr bl ){ return parent_id == bl->getParentId(); };

    // WARNING filtered
    std::function< bool( BaseAreaPtr ) > check_filter =
            []( BaseAreaPtr bl ){ return EntityFilter::isFiltered( bl ); };

    std::vector< LocTypePtr > loc_childs;
    for( LocTypePtr ptr: vector )
        if( check_parent_id( ptr )
                && check_filter( ptr ))
            loc_childs.push_back( ptr );

    return loc_childs;
}
