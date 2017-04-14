#include "rb_signal_manager.h"

using namespace regionbiz;

SelectionManager::SelectionManager()
{
    qRegisterMetaType< uint64_t >("uint64_t");
}

void SelectionManager::selectNewEntity( uint64_t new_id )
{
    uint64_t prev_id = _selected_entity_id;
    _selected_entity_id = new_id;
    _selected_set.clear();
    _selected_set.insert( new_id );

    Q_EMIT selectBaseEntity( prev_id, new_id );
}

void SelectionManager::centerOnNewEntity( uint64_t id )
{
    Q_EMIT centerOnNewEntity( id );
}

void SelectionManager::clearSelect()
{
    _selected_set.clear();

    if( _selected_entity_id )
    {
        uint64_t prev_id = _selected_entity_id;
        _selected_entity_id = 0;
        Q_EMIT selectBaseEntity( prev_id, 0 );
    }

    Q_EMIT selectClear();
}

void SelectionManager::appendToSeletedSet(uint64_t id)
{
    _selected_entity_id = 0;

    if( !_selected_set.count( id ))
    {
        _selected_set.insert( id );
        Q_EMIT selectedSet( _selected_set );
    }
}

ChangeEntitysWatcher::ChangeEntitysWatcher()
{
    qRegisterMetaType< uint64_t >("uint64_t");
}

void ChangeEntitysWatcher::changeEntity(uint64_t id)
{
    Q_EMIT changeBaseEntity( id );
}

void ChangeEntitysWatcher::deleteEntity(uint64_t id)
{
    Q_EMIT deleteBaseEntity( id );
}

void ChangeEntitysWatcher::addEntity(uint64_t id)
{
    Q_EMIT addBaseEntity( id );
}
