#include "rb_base_entity.h"

#include <iostream>
#include "rb_manager.h"

using namespace regionbiz;

class RegionBizManager;

uint64_t BaseEntity::_max_id = 0;
std::recursive_mutex BaseEntity::_mutex;

BaseEntity::BaseEntity(uint64_t id):
    _id( id )
{
    if( id > _max_id )
        _max_id = id;
}

uint64_t BaseEntity::getId()
{
    return _id;
}

uint64_t BaseEntity::getMaxId()
{
    return _max_id;
}

BaseEntityPtr BaseEntity::getEntity(uint64_t id)
{
    if( isEntityExist( id ))
    {
        _mutex.lock();
        BaseEntityPtr ent = getEntitys()[id];
        _mutex.unlock();

        return ent;
    }

    return nullptr;
}

std::unordered_map<uint64_t, BaseEntityPtr> &BaseEntity::getEntitys()
{
    static std::unordered_map< uint64_t, BaseEntityPtr > entitys;
    return entitys;
}

std::map<uint64_t, BaseEntityPtrSet>& BaseEntity::getChildsOfParent()
{
    static std::map< uint64_t, BaseEntityPtrSet > childs_by_parent;
    return childs_by_parent;
}

QString BaseEntity::getDescription()
{
    return _description;
}

void BaseEntity::setDesription(QString descr)
{
    _description = descr;
}

QString BaseEntity::getName()
{
    return _name;
}

void BaseEntity::setName(QString name)
{
    _name = name;
}

bool BaseEntity::isEntityExist(uint64_t id)
{
    _mutex.lock();
    bool exist = getEntitys().find( id ) != getEntitys().end();
    _mutex.unlock();

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

bool BaseEntity::deleteMetadata(QString name)
{
    auto mngr = RegionBizManager::instance();
    return mngr->deleteMetadata( _id, name );
}

Constraints BaseEntity::getConstraints()
{
    auto mngr = RegionBizManager::instance();
    return mngr->getConstraintsOfEntity( getId() );
}

Constraints BaseEntity::getConstraints( Constraint::ConstraintType type )
{
    auto mngr = RegionBizManager::instance();
    return mngr->getConstraintsOfEntity( getId(), type );
}

BaseFileKeeperPtrs BaseEntity::getFiles()
{
    auto mngr = RegionBizManager::instance();
    return mngr->getFilesByEntity( _id );
}

BaseFileKeeperPtrs BaseEntity::getFilesByType(BaseFileKeeper::FileType type)
{
    auto mngr = RegionBizManager::instance();
    return mngr->getFilesByEntity( _id, type );
}

bool BaseEntity::deleteEntity(BaseEntityPtr ent, uint64_t parent_id)
{
    if( !ent )
        return false;
    return deleteEntity( ent->getId(), parent_id );
}

bool BaseEntity::deleteEntity(uint64_t id, uint64_t parent_id)
{
    _mutex.lock();
    bool del = getEntitys().erase( id );
    if( parent_id )
    {
        if( getChildsOfParent().find( parent_id ) != getChildsOfParent().end() )
        {
            auto entity = getEntity( id );
            if( entity )
            {
                #define SET getChildsOfParent()[ parent_id ]
                SET.erase( entity );
                #undef SET
            }
        }
    }
    _mutex.unlock();

    return del;
}

BaseEntityPtr BaseEntity::getItself()
{
    _mutex.lock();
    BaseEntityPtr itself = getEntitys()[ _id ];
    _mutex.unlock();

    return itself;
}

