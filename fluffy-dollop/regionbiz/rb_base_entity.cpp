#include "rb_base_entity.h"

#include <iostream>

using namespace regionbiz;

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
