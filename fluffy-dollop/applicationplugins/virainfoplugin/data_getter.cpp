#include "data_getter.h"

#include <regionbiz/rb_locations.h>
#include <regionbiz/rb_manager.h>

using namespace regionbiz;

DataGetter &DataGetter::instance()
{
    static DataGetter test;
    return test;
}

AreaData DataGetter::getData(uint64_t id)
{
    auto mngr = RegionBizManager::instance();
    BaseAreaPtr area = mngr->getBaseArea( id );
    if( ! area)
    {
        AreaData data;
        data.id = id;
        return data;
    }

    AreaData data;
    data.id = id;
    if ( BaseArea::AT_ROOM == area->getType() )
    {
        // one room
        // area
        int prob = rand() % 10;
        RoomPtr room = area->convert< Room >();

        double area = ( room->isMetadataPresent( "area" ) ?
                            room->getMetadataValue( "area" ).toDouble() : 0 );

        bool free = room->isMetadataPresent( "status" ) && QString::fromUtf8( "Свободно" )
                == room->getMetadataValue( "status" ).toString();

        bool on_rent = room->isMetadataPresent( "status" ) && QString::fromUtf8( "В аренде" )
                == room->getMetadataValue( "status" ).toString();

        bool unavailable = room->isMetadataPresent( "status" ) && QString::fromUtf8( "Недоступно" )
                == room->getMetadataValue( "status" ).toString();

        QString rentor = ( room->isMetadataPresent( "rentor" ) ?
                               room->getMetadataValue( "rentor" ).toString() : "" );

        if( free )
            data.area_free = area;
        else if( on_rent )
            data.area_rent = area;
        else if( unavailable )
            data.area_unavailable = area;

        // task
        MarkPtrs marks = room->getMarks();
        for( MarkPtr mark: marks )
        {
            if( mark->isMetadataPresent( "status" ))
            {
                QString status = mark->getMetadataValue( "status" ).toString();
                if( QString::fromUtf8( "новый" ) == status )
                    data.task_new += 1;
                else if( QString::fromUtf8( "в работе" ) == status )
                    data.task_work += 1;
                else if( QString::fromUtf8( "на проверку" ) == status )
                    data.task_check += 1;
            }
        }

        // arendators
        if( !free )
        {
            data.arendators_good = 1;
            data.arendators_good_set.insert( rentor );
        }
    }
    else
    {
        // many rooms
        auto childs = area->getBaseAreaChilds();
        for( BaseAreaPtr ch_area: childs )
        {
            AreaData ch_data = DataGetter::getData( ch_area->getId() );

            // area
            data.area_free += ch_data.area_free;
            data.area_property += ch_data.area_property;
            data.area_rent += ch_data.area_rent;

            // task
            data.task_work += ch_data.task_work;
            data.task_check += ch_data.task_check;
            data.task_new += ch_data.task_new;

            // arend
            data.arendators_bad_set = data.arendators_bad_set.unite( ch_data.arendators_bad_set );
            data.arendators_good_set = data.arendators_good_set.unite( ch_data.arendators_good_set );
            data.arendators_bad = data.arendators_bad_set.size();
            data.arendators_good = data.arendators_good_set.size();
            data.debt += ch_data.debt;
        }
    }

    return data;
}
