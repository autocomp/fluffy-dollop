#include "rb_manager.h"

#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <functional>
#include <QFile>
#include <QJsonDocument>
#include <QVariant>
#include <QDebug>

#define FIND_IF( cont, func ) std::find_if( cont.begin(), cont.end(), func );

using namespace regionbiz;

RegionBizManagerPtr RegionBizManager::instance()
{
    static RegionBizManagerPtr instance_ptr = nullptr;
    if( !instance_ptr )
        instance_ptr = RegionBizManagerPtr( new RegionBizManager() );
    return instance_ptr;
}

bool RegionBizManager::init(std::string& config_path)
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
            }
        }
        else
            std::cerr << "Region Biz Translator don't created" << std::endl;
    }
}

BaseAreaPtr RegionBizManager::getBaseLoation( uint64_t id )
{
    BaseAreaPtr loc;

    loc = getBaseLoation( id, BaseArea::AT_REGION );
    if( loc )
        return loc;
}

BaseAreaPtr RegionBizManager::getBaseLoation( uint64_t id,
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

RegionBizManager::RegionBizManager()
{
    std::atexit( onExit );
}

void RegionBizManager::onExit()
{
    std::cerr << "Region Biz Manager stopped" << std::endl;
    RegionBizManager::instance()->clearCurrentData();
}

QVariantMap RegionBizManager::loadJsonConfig( std::string& file_path )
{
    QString file_in;

    QFile file( file_path.c_str() );
    if( file.open( QFile::ReadOnly | QFile::Text ))
    {
        file_in = file.readAll();
        file.close();
    }
    else
    {
        std::cerr << "Region Biz can't open config file: "
                  << file_path << std::endl;
    }

    QJsonParseError err;
    QJsonDocument json_doc = QJsonDocument::fromJson( file_in.toUtf8(), &err );
    if( err.error != QJsonParseError::NoError )
        std::cerr << err.errorString().toStdString();

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
