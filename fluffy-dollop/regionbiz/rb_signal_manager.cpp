#include "rb_signal_manager.h"

using namespace regionbiz;

SelectionManager::SelectionManager()
{
    qRegisterMetaType< uint64_t >("uint64_t");
}

void SelectionManager::setNewCurrentEntity( uint64_t new_id )
{
    uint64_t prev_id = _current_entity_id;
    _current_entity_id = new_id;

    Q_EMIT currentEntityChange( prev_id, new_id );
}

void SelectionManager::centerOnNewEntity( uint64_t id )
{
    Q_EMIT centerOnNewEntity( id );
}

void SelectionManager::clearSelect()
{
    std::vector< uint64_t > deselect;
    for( uint64_t id : _selected_set )
        deselect.push_back( id );
    _selected_set.clear();

    Q_EMIT selectedSetChange( std::vector< uint64_t >(),
                              deselect );
    Q_EMIT selectedSetChange();
}

void SelectionManager::appendToSeletedSet( std::vector<uint64_t> ids, bool force )
{
    std::vector< uint64_t > added;

    for( uint64_t add_id : ids )
    {
        if( !_selected_set.count( add_id ))
        {
            _selected_set.insert( add_id );
            added.push_back( add_id );
        }
    }

    if( force )
        Q_EMIT selectedSetChange( ids, std::vector< uint64_t >() );
    else
        Q_EMIT selectedSetChange( added, std::vector< uint64_t >() );
    Q_EMIT selectedSetChange();
}

void SelectionManager::removeFromSeletedSet(std::vector<uint64_t> ids, bool force)
{
    std::vector< uint64_t > removed;

    for( uint64_t rem_id : ids )
    {
        if( _selected_set.count( rem_id ))
        {
            _selected_set.erase( _selected_set.find( rem_id ));
            removed.push_back( rem_id );
        }
    }

    if( force )
        Q_EMIT selectedSetChange( std::vector< uint64_t >(), ids );
    else
        Q_EMIT selectedSetChange( std::vector< uint64_t >(), removed );
    Q_EMIT selectedSetChange();
}

//-----------------------------------------

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
