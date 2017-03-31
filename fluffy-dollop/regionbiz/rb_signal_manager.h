#ifndef RB_SIGNAL_MANAGER_H
#define RB_SIGNAL_MANAGER_H

#include <QObject>

namespace regionbiz {

class RegionBizManager;

class SelectionManager: public QObject
{
    Q_OBJECT

    friend class RegionBizManager;

Q_SIGNALS:
    void selectBaseEntity( uint64_t old_id,
                           uint64_t new_id );
    void centerOnBaseEntity( uint64_t id );

private:
    SelectionManager();
    void selectNewEntity( uint64_t new_id );
    void centerOnNewEntity(uint64_t id );

    uint64_t _selected_entity_id = 0;
};

class ChangeEntitysWatcher: public QObject
{
    Q_OBJECT

    friend class RegionBizManager;

Q_SIGNALS:
    void changeBaseEntity( uint64_t id );
    void deleteBaseEntity( uint64_t id );
    void addBaseEntity( uint64_t id );

private:
    ChangeEntitysWatcher();
    void changeEntity( uint64_t id );
    void deleteEntity( uint64_t id );
    void addEntity( uint64_t id );
};

}

// WARNING resolve dependency include trouble
#include "rb_manager.h"

#endif // RB_SIGNAL_MANAGER_H
