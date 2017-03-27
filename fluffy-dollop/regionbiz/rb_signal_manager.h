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
    void selectBaseArea( uint64_t old_id,
                         uint64_t new_id );
    void centerOnBaseArea( uint64_t id );

private:
    SelectionManager();
    void selectNewArea( uint64_t new_id );
    void centerOnNewArea(uint64_t id );

    uint64_t _selected_area_id = 0;
};

}

// WARNING resolve dependency include trouble
#include "rb_manager.h"

#endif // RB_SIGNAL_MANAGER_H
