#include "test_data_getter.h"

#include <regionbiz/rb_locations.h>
#include <regionbiz/rb_manager.h>

using namespace regionbiz;

TestDataGetter &TestDataGetter::instance()
{
    static TestDataGetter test;
    return test;
}

TestData TestDataGetter::getData(uint64_t id)
{
    if( instance()._datas.contains( id ))
        return instance()._datas[ id ];

    auto mngr = RegionBizManager::instance();
    BaseAreaPtr area = mngr->getBaseArea( id );
    if( ! area)
    {
        TestData data;
        data.id = id;
        return data;
    }

    TestData data;
    data.id = id;
    if ( BaseArea::AT_ROOM == area->getType() )
    {
        // one room
        // area
        int prob = rand() % 10;
        if( prob < 5 )
            data.area_rent = 10 + rand() % 40;
        else if ( prob < 8 )
            data.area_property = 10 + rand() % 40;
        else
            data.area_free = 10 + rand() % 40;

        // task
        data.task_work = rand() % 5;
        data.task_lost = rand() % 2;
        data.task_new = rand() % 3;

        // arendators
        QStringList name;
        name << QString::fromUtf8( "Александр" )
             << QString::fromUtf8( "Георгий" )
             << QString::fromUtf8( "Иван" )
             << QString::fromUtf8( "Рустам" );
        QStringList family;
        family << QString::fromUtf8( "Иванов" )
               << QString::fromUtf8( "Ставрицин" )
               << QString::fromUtf8( "Петроченко" )
               << QString::fromUtf8( "Скворцов" );

        QString arend_name = family[ rand() % family.size() ] + " "
                + name[ rand() % name.size() ];

        if( data.area_free < 1e-5 )
        {

            int prob_arend = rand() % 10;
            if( prob_arend >= 8 )
            {
                data.arendators_bad = 1;
                data.arendators_bad_set.insert( arend_name );

                data.debt = 5000 + ( rand() % 100 ) * 100;
            }
            else
            {
                data.arendators_good = 1;
                data.arendators_good_set.insert( arend_name );
            }
        }
    }
    else
    {
        // many rooms
        auto childs = area->getChilds();
        for( BaseAreaPtr ch_area: childs )
        {
            TestData ch_data = TestDataGetter::getData( ch_area->getId() );

            // area
            data.area_free += ch_data.area_free;
            data.area_property += ch_data.area_property;
            data.area_rent += ch_data.area_rent;

            // task
            data.task_work += ch_data.task_work;
            data.task_lost += ch_data.task_lost;
            data.task_new += ch_data.task_new;

            // arend
            data.arendators_bad_set = data.arendators_bad_set.unite( ch_data.arendators_bad_set );
            data.arendators_good_set = data.arendators_good_set.unite( ch_data.arendators_good_set );
            data.arendators_bad = data.arendators_bad_set.size();
            data.arendators_good = data.arendators_good_set.size();
            data.debt += ch_data.debt;
        }
    }

    instance()._datas[ id ] = data;
    return data;
}
