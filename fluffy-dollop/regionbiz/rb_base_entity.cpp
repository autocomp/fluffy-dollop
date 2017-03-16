#include "rb_base_entity.h"

#include <iostream>
#include "rb_manager.h"

using namespace regionbiz;

class RegionBizManager;

BaseEntity::BaseEntity(uint64_t id):
    _id( id )
{}

uint64_t BaseEntity::getId()
{
    return _id;
}

uint64_t BaseEntity::getMaxId()
{
    // if map is empty
    if( !getEntitys().size() )
        return 0;

    // key of last element - max id
    uint64_t max = getEntitys().rbegin()->first;
    return max;
}

BaseEntityPtr BaseEntity::getEntity(uint64_t id)
{
    if( isEntityExist( id ))
        return getEntitys()[id];
}

std::map< uint64_t, BaseEntityPtr >& BaseEntity::getEntitys()
{
    static std::map< uint64_t, BaseEntityPtr > entitys;
    return entitys;
}

bool BaseEntity::isEntityExist(uint64_t id)
{
    bool exist = getEntitys().find( id ) != getEntitys().end();
    return exist;
}

bool BaseEntity::isMetadataPresent(QString name)
{
    auto mngr = RegionBizManager::instance();
    bool present = mngr->isMetadataPresent( _id, name );

    return present;
}

QVariant BaseEntity::getMetadataValue(QString name)
{
    auto mngr = RegionBizManager::instance();
    QVariant val = mngr->getMetadataValue( _id, name );

    return val;
}

BaseMetadataPtr BaseEntity::getMetadata(QString name)
{
    auto mngr = RegionBizManager::instance();
    BaseMetadataPtr data = mngr->getMetadata( _id, name );

    return data;
}

MetadataByName BaseEntity::getMetadataMap()
{
    auto mngr = RegionBizManager::instance();
    MetadataByName data = mngr->getMetadataMap( _id );

    return data;
}

bool BaseEntity::setMetadataValue(QString name, QVariant val)
{
    auto mngr = RegionBizManager::instance();
    bool set = mngr->setMetadataValue( _id, name, val );

    return set;
}

bool BaseEntity::addMetadata( QString type, QString name, QVariant val )
{
    auto mngr = RegionBizManager::instance();
    return mngr->addMetadata( _id, type, name, val );
}

bool BaseEntity::addMetadata(BaseMetadataPtr data)
{
    if( _id == data->getParentId() )
    {
        auto mngr = RegionBizManager::instance();
        return mngr->addMetadata( data );
    }

    return false;
}

bool BaseEntity::deleteEntity(BaseEntityPtr ent)
{
    return deleteEntity( ent->getId() );
}

bool BaseEntity::deleteEntity(uint64_t id)
{
    return getEntitys().erase( id );
}

