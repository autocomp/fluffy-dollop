#include "rbp_xlsx_translator.h"

#include <regionbiz/rb_manager.h>
#include <qtxlsx/xlsx/xlsxdocument.h>

using namespace regionbiz;

REGISTER_TRANSLATOR( XlsxTranlsator )

XlsxTranlsator::XlsxTranlsator()
{
    auto mngr = RegionBizManager::instance();
    mngr->subscribeOnSelect( this, SLOT( onObjectSelected( uint64_t, uint64_t )));
}

bool XlsxTranlsator::initBySettings( QVariantMap settings )
{
    if( settings.contains( "file_path" ))
    {
        QString file_path = settings[ "file_path" ].toString();
        loadData( file_path );
    }
    if( settings.contains( "template" ))
    {
         QVariantMap template_settings = settings[ "template" ].toMap();

         if( template_settings.contains( "columns" ))
            _col_template = template_settings[ "columns" ].toString();
         if( template_settings.contains( "first_row" ))
             _start_row = template_settings[ "first_row" ].toUInt();
    }

    return true;
}

void XlsxTranlsator::loadFunctions()
{
    // load functions xlsx
    _load_floors = std::bind( &XlsxTranlsator::loadFloors, this );
    _load_rooms = std::bind( &XlsxTranlsator::loadRooms, this );
}

void XlsxTranlsator::onObjectSelected(uint64_t /*old_id*/, uint64_t new_id)
{
    // clear all data
    clearData();

    // inspect selected object
    auto mngr = RegionBizManager::instance();
    BaseAreaPtr area = mngr->getBaseArea( new_id );
    if( area
            && BaseArea::AT_FACILITY == area->getType())
        _parent_id = new_id;
    else
        _parent_id = 0;
}

#define loadCell( cell_name ) \
    QXlsx::Cell* cell = xlsx_r.cellAt( row_current, key_value[ cell_name ] )
#define cellIsValid cell->value().isValid()

void XlsxTranlsator::loadData( const QString& file_path )
{
    // clear
    clearData();

    // check parent facility
    if( !_parent_id )
        return;

    // check
    QFile file( file_path );
    if( !file.exists() )
    {
        std::cerr << "File " << file_path.toUtf8().data()
                  << " don't exists" << std::endl;
        return;
    }

    // create templateof columns
    std::map< QString, uint > key_value;
    QStringList column_nums = _col_template.split( ";" );
    for( QString str: column_nums )
    {
        QStringList key_val = str.split( ":" );
        if( key_val.size() == 2 )
        {
            uint num = key_val.at( 1 ).toUInt();
            QString key = key_val.at( 0 );
            key_value[ key ] = num;
        }
    }

    // TODO smart xlsx read by template
    // check template
    if( key_value.find( "floor" ) == key_value.end()
            || key_value.find( "room_name" ) == key_value.end()
            || key_value.find( "room_descr" ) == key_value.end()
            || key_value.find( "room_area" ) == key_value.end()
            || key_value.find( "room_arend" ) == key_value.end() )
    {
        std::cerr << "Incorrect template "
                  << _col_template.toUtf8().data() << std::endl;
        return;
    }

    // read document
    QXlsx::Document xlsx_r( file_path );
    uint row_current = _start_row;
    bool has_row = true;
    while( has_row )
    {
        has_row = false;
        RoomEntry entry;

        // read all
        // floor
        if( loadCell( "floor" ))
        {
            if( cellIsValid )
            {
                QString floor_name = cell->value().toString();
                if( !_floor_names.contains( floor_name ))
                    _floor_names.append( floor_name );
                entry.floor = floor_name;

                has_row = true;
            }
        }

        // room name
        if( loadCell( "room_name" ))
        {
            if( cellIsValid )
            {
                QString room_name = cell->value().toString();
                entry.name = room_name;

                has_row = true;
            }
        }

        // room descr
        if( loadCell( "room_descr" ))
        {
            if( cellIsValid )
            {
                QString room_descr = cell->value().toString();
                entry.descr = room_descr;

                has_row = true;
            }
        }

        // room arend
        if( loadCell( "room_arend" ))
        {
            if( cellIsValid )
            {
                QString room_arend = cell->value().toString();
                entry.arend = room_arend;

                has_row = true;
            }
        }

        // room arend
        if( loadCell( "room_area" ))
        {
            if( cellIsValid )
            {
                double room_area = cell->value().toDouble();
                entry.area = room_area;

                has_row = true;
            }
        }

        _entrys.push_back( entry );
        ++row_current;
    }
}

FloorPtrs XlsxTranlsator::loadFloors()
{
    FloorPtrs floors;

    // check parent
    if( !_parent_id )
        return floors;

    // create vector
    for( QString floor_name: _floor_names )
    {
        BaseAreaPtr floor = BaseEntity::createWithId< Floor >( BaseEntity::getMaxId() + 1 );
        if( floor )
        {
            setParentForBaseLocation( floor, _parent_id );
            floor->setName( floor_name );
            _floor_name_to_id[ floor_name ] = floor->getId();

            floors.push_back( floor->convert< Floor >() );
        }
    }

    return floors;
}

RoomPtrs XlsxTranlsator::loadRooms()
{
    RoomPtrs rooms;

    // check parent
    if( !_parent_id )
        return rooms;

    for( RoomEntry entry: _entrys )
    {
        BaseAreaPtr room = BaseEntity::createWithId< Room >( BaseEntity::getMaxId() + 1 );
        if( room )
        {
            setParentForBaseLocation( room, _floor_name_to_id[ entry.floor ] );
            room->setName( entry.name );
            room->setDesription( entry.descr );
            room->addMetadata( "double", "area", entry.area );
            room->addMetadata( "string", "arend", entry.arend );

            rooms.push_back( room->convert< Room >() );
        }
    }

    return rooms;
}

void XlsxTranlsator::clearData()
{
    _floor_name_to_id.clear();
    _floor_names.clear();
    _entrys.clear();
}
