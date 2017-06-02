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
    case BaseArea::AT_LOCATION:
        add_area = addArea< Location >( parent_id );
    case BaseArea::AT_FACILITY:
        add_area = addArea< Facility >( parent_id );
    case BaseArea::AT_FLOOR:
        add_area = addArea< Floor >( parent_id );
    case BaseArea::AT_ROOM:
        add_area = addArea< Room >( parent_id );
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

bool RegionBizManager::deleteArea(BaseAreaPtr area)
{
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

    // delete this one
    bool del = _data_translator->deleteArea( area );
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
            BaseEntity::getEntitysByType< BaseBizRelation >( BaseEntity::ET_RELATION );

    for( BaseBizRelationPtr rel: relations_ptrs )
    {
        if( type == rel->getType()
                && id == rel->getAreaId() )
        {
            relations.push_back( rel );
        }
    }

    return relations;
}

BaseMetadataPtr RegionBizManager::getMetadata( uint64_t id, QString name )
{
    if( isMetadataPresent( id, name ))
        return _metadata[id][name];

    return EmptyMetadata::instance();
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

    // get marks from entitys
    auto marks_ptrs = BaseEntity::getEntitysByType< Mark >( BaseEntity::ET_MARK );

    MarkPtrs marks;
    for( MarkPtr mark: marks_ptrs )
    {
        if( check_id( mark ))
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
    mark = MarkFabric::createByType( type, id );
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
    bool del = _data_translator->deleteMark( mark );
    if( del )
    {
        // emit signal
        _change_watcher.deleteEntity( mark->getId() );
    }
    return del;
}

BaseFileKeeperPtrs RegionBizManager::getFilesByEntity(uint64_t id)
{
    auto& files = BaseFileKeeper::getFiles();
    if( files.find( id ) != files.end() )
        return files[ id ];

    return BaseFileKeeperPtrs();
}

BaseFileKeeperPtrs RegionBizManager::getFilesByEntity(BaseEntityPtr ptr)
{
    return getFilesByEntity( ptr->getId() );
}

BaseFileKeeperPtrs RegionBizManager::getFilesByEntity(uint64_t id, BaseFileKeeper::FileType type)
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

BaseFileKeeperPtr RegionBizManager::addFile( QString file_path,
                                             BaseFileKeeper::FileType type,
                                             uint64_t entity_id )
{
    BaseFileKeeperPtr file =
            _files_translator->addFile( file_path, type, entity_id );
    BaseFileKeeper::getFiles()[ entity_id ].push_back( file );

    return file;
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

void RegionBizManager::subscribeFileSynced(QObject *obj, const char *slot)
{
    _files_translator->subscribeFileSynced( obj, slot );
}

void RegionBizManager::subscribeFileAdded(QObject *obj, const char *slot)
{
    _files_translator->subscribeFileAdded( obj, slot );
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
}

void RegionBizManager::clearCurrentData( bool clear_entitys )
{
    //data
    _metadata.clear();

    // entitys
    if( clear_entitys )
        BaseEntity::getEntitys().clear();
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

    // get areas
    BaseAreaPtrs areas = BaseEntity::getEntitysByType< BaseArea >( BaseEntity::ET_AREA );

    // check all areas
    std::vector< std::shared_ptr< LocType >> loc_childs;
    for( BaseAreaPtr ptr: areas )
    {
        std::shared_ptr< LocType > loc_ptr = BaseArea::convert< LocType >( ptr );
        if( !loc_ptr )
            continue;

        if( check_parent_id( ptr )
                && check_filter( ptr ))
            loc_childs.push_back( loc_ptr );
    }

    return loc_childs;
}
