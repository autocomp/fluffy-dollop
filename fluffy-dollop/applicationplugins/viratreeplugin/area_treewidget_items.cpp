#include "area_treewidget_items.h"

#include <regionbiz/rb_manager.h>

#include "delegate.h"

bool FloorTreeWidgetItem::operator < ( const QTreeWidgetItem &other ) const
{
    // if we check first column
    int sortCol = treeWidget()->sortColumn();
    if( 0 == sortCol )
    {
        using namespace regionbiz;

        // get ids
        uint64_t this_id = data( sortCol, ID ).toLongLong();
        uint64_t other_id = other.data( sortCol, ID ).toLongLong();

        // find floors
        auto mngr = RegionBizManager::instance();
        FloorPtr this_floor = mngr->getBaseArea( this_id )->convert< Floor >();
        FloorPtr other_floor = mngr->getBaseArea( other_id )->convert< Floor >();
        if( this_floor && other_floor )
            // check by numbers
            return this_floor->getNumber() < other_floor->getNumber();
    }

    // check by strings
    return text( sortCol ) < other.text( sortCol );
}
