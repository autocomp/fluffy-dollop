#include "rb_selection_manager.h"

using namespace regionbiz;

SelectionManager::SelectionManager()
{
    qRegisterMetaType< uint64_t >("uint64_t");
}

void SelectionManager::selectNewArea( uint64_t new_id )
{
    uint64_t prev_id = _selected_area_id;
    _selected_area_id = new_id;

    Q_EMIT selectBaseArea( prev_id, new_id );
}
