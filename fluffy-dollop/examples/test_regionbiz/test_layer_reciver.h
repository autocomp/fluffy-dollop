#ifndef TEST_LAYER_RECIVER_H
#define TEST_LAYER_RECIVER_H

#include <QObject>
#include <QDebug>

#include <regionbiz/rb_manager.h>

class TestLayerReciver: public QObject
{
    Q_OBJECT
public:
    void subscribe()
    {
        auto mngr = regionbiz::RegionBizManager::instance();
        mngr->subscribeLayerAdded( this, SLOT( onAdd( uint64_t )));
        mngr->subscribeLayersChanged( this, SLOT( onChange() ));
        mngr->subscribeLayerDeleted( this, SLOT( onDelete( uint64_t )));
        mngr->subscribeLayerChangeShowed( this,
                                          SLOT( onChangeShowed( uint64_t, bool)));
        mngr->subscribeLayersChangedOrder( this, SLOT( onChangeOrder() ));
    }

public Q_SLOTS:
    void onAdd( uint64_t id )
    {
        qDebug() << "Added:" << id;
    }

    void onChange()
    {
        qDebug() << "Changed";
    }

    void onDelete( uint64_t id )
    {
        qDebug() << "Deleted:" << id;
    }

    void onChangeOrder()
    {
        qDebug() << "Change order";
    }

    void onChangeShowed( uint64_t id, bool showed )
    {
        qDebug() << "Change showed:" << id << showed;
    }
};

#endif // TEST_LAYER_RECIVER_H
