#include "rb_manager.h"

#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <functional>
#include <QFile>
#include <QJsonDocument>
#include <QVariant>
#include <QDebug>
#include <QDir>

#define FIND_IF( cont, func ) std::find_if( cont.begin(), cont.end(), func );

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
    if( settings.contains( "translator" ))
    {
        QVariantMap translator_settings = settings["translator"].toMap();

        std::string translator_type = translator_settings["type"].toString().toStdString();
        _translator = BaseTranslatorFabric::getTranslatorByType( translator_type );
        if( _translator )
        {
            _translator->init( translator_settings );

            std::string error_text = "";
            if( _translator->checkTranslator( error_text ))
            {
                loadDataByTranslator();
            }
            else
            {
                std::cerr << "Error with Translator:\n" << error_text << std::endl;
                _translator = nullptr;

                return false;
            }
        }
        else
        {
            std::cerr << "Region Biz Translator don't created" << std::endl;
            return false;
        }
    }

    return true;
}

BaseAreaPtr RegionBizManager::getBaseArea( uint64_t id )
{
    auto entity = BaseEntity::getEntity( id );
    // TODO thin about static and dynamic cast
    BaseAreaPtr loc = std::static_pointer_cast< BaseArea >( entity );
    return loc;
}

BaseAreaPtr RegionBizManager::getBaseArea( uint64_t id,
                                           BaseArea::AreaType type )
{
    BaseAreaPtr loc;

    std::function< bool( BaseAreaPtr ) > check_id =
            [ id ]( BaseAreaPtr bl ){ return id == bl->getId(); };

    switch (type) {
    case BaseArea::AT_REGION:
    {
        auto iter = FIND_IF( _regions, check_id );
        if( iter != _regions.end() )
            loc = *iter;

        break;
    }

    case BaseArea::AT_LOCATION:
    {
        auto iter = FIND_IF( _locations, check_id );
        if( iter != _locations.end() )
            loc = *iter;

        break;
    }

    case BaseArea::AT_FACILITY:
    {
        auto iter = FIND_IF( _facilitys, check_id );
        if( iter != _facilitys.end() )
            loc = *iter;

        break;
    }

    case BaseArea::AT_FLOOR:
    {
        auto iter = FIND_IF( _floors, check_id );
        if( iter != _floors.end() )
            loc = *iter;

        break;
    }

    case BaseArea::AT_ROOMS_GROUP:
    {
        auto iter = FIND_IF( _rooms_groups, check_id );
        if( iter != _rooms_groups.end() )
            loc = *iter;

        break;
    }

    case BaseArea::AT_ROOM:
    {
        auto iter = FIND_IF( _rooms, check_id );
        if( iter != _rooms.end() )
            loc = *iter;

        break;
    }

    default:
        break;
    }

    return loc;
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
    switch ( type ) {
    case BaseArea::AT_REGION:
        addArea< Region >( parent_id );
        break;
    case BaseArea::AT_LOCATION:
        addArea< Location >( parent_id );
        break;
    case BaseArea::AT_FACILITY:
        addArea< Facility >( parent_id );
        break;
    case BaseArea::AT_FLOOR:
        addArea< Floor >( parent_id );
        break;
    case BaseArea::AT_ROOMS_GROUP:
        addArea< RoomsGroup >( parent_id );
        break;
    case BaseArea::AT_ROOM:
        addArea< Room >( parent_id );
        break;
    default:
        break;
    }
}

bool RegionBizManager::commitArea( BaseAreaPtr area )
{
    return _translator->commitArea( area );
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

BaseMetadataPtr RegionBizManager::getAreaMetadata(uint64_t id, QString name)
{
    if( _metadata.find( id ) != _metadata.end() )
        if( _metadata[id].find( name ) != _metadata[id].end() )
            return _metadata[id][name];

    return nullptr;
}

MetadataByName RegionBizManager::getAreaMetadataMap(uint64_t id)
{
    if( _metadata.find( id ) != _metadata.end() )
            return _metadata[id];

    return MetadataByName();
}

uint64_t RegionBizManager::getSelectedArea()
{
    return _select_manager._selected_area_id;
}

void RegionBizManager::selectArea(uint64_t id)
{
    _select_manager.selectNewArea( id );
}

void RegionBizManager::subscribeOnSelect(QObject *obj, const char *slot, bool queue)
{
    QObject::connect( &_select_manager, SIGNAL( selectBaseArea(uint64_t,uint64_t) ),
                      obj, slot, ( queue ? Qt::QueuedConnection : Qt::DirectConnection ));
}

void RegionBizManager::centerOnArea(uint64_t id)
{
    _select_manager.centerOnBaseArea( id );
}

void RegionBizManager::subscribeCenterOn(QObject *obj, const char *slot, bool queue)
{
    QObject::connect( &_select_manager, SIGNAL( centerOnBaseArea( uint64_t )),
                      obj, slot, ( queue ? Qt::QueuedConnection : Qt::DirectConnection ));
}

RegionBizManager::RegionBizManager()
{
    std::atexit( onExit );
}

void RegionBizManager::onExit()
{
    std::cerr << "Region Biz Manager stopped" << std::endl;
    RegionBizManager::instance()->clearCurrentData();
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
        std::cerr << err.errorString().toStdString();

    // return result
    QVariantMap settings = json_doc.toVariant().toMap();
    return settings;
}

void RegionBizManager::loadDataByTranslator()
{
    if( !_translator )
        return;

    clearCurrentData();

    // regions
    auto regions_vec = _translator->loadRegions();
    for( RegionPtr reg: regions_vec )
    {
        // TODO check id
        _regions.push_back( reg );
    }

    // locations
    auto locations_vec = _translator->loadLocations();
    for( LocationPtr loc: locations_vec )
    {
        // TODO check id and parent id
        _locations.push_back( loc );
    }

    // facilitys
    auto facilitys_vec = _translator->loadFacilitys();
    for( FacilityPtr fac: facilitys_vec )
    {
        // TODO check id and parent id
        _facilitys.push_back( fac );
    }

    // floors
    auto floors_vec = _translator->loadFloors();
    for( FloorPtr flo: floors_vec )
    {
        // TODO check id and parent id
        _floors.push_back( flo );
    }

    // rooms groups
    auto rooms_groups_vec = _translator->loadRoomsGroups();
    for( RoomsGroupPtr rg: rooms_groups_vec )
    {
        // TODO check id and parent id
        _rooms_groups.push_back( rg );
    }

    // rooms
    auto rooms_vec = _translator->loadRooms();
    for( RoomPtr room: rooms_vec )
    {
        // TODO check id and parent id
        _rooms.push_back( room );
    }

    //--------------------------------

    // propertys
    auto prop_vec = _translator->loadPropertys();
    for( PropertyPtr prop: prop_vec )
    {
        // TODO check area id
        _propertys.push_back( prop );
    }

    // rents
    auto rent_vec = _translator->loadRents();
    for( RentPtr rent: rent_vec )
    {
        // TODO check area id
        _rents.push_back( rent );
    }

    //---------------------------------
    auto metadata_vec = _translator->loadMetadata();
    for( BaseMetadataPtr data: metadata_vec )
    {
        // TODO check id

        // add by parent_id / name of metadata
        _metadata[ data->getParentId() ][ data->getName() ] = data;
    }
}

void RegionBizManager::clearCurrentData()
{
    _regions.clear();
    _locations.clear();
    _facilitys.clear();
    _floors.clear();
    _rooms_groups.clear();
    _rooms.clear();
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

template<typename LocTypePtr>
std::vector< LocTypePtr > RegionBizManager::getBaseLocationsByParent( uint64_t parent_id,
                                                                      std::vector< LocTypePtr >& vector )
{
    std::function< bool( BaseAreaPtr ) > check_parent_id =
            [ parent_id ]( BaseAreaPtr bl ){ return parent_id == bl->getParentId(); };

    std::vector< LocTypePtr > loc_childs;
    for( LocTypePtr ptr: vector )
        if( check_parent_id( ptr) )
            loc_childs.push_back( ptr );

    return loc_childs;
}
