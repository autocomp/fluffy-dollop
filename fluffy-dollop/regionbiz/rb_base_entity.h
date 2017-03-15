#ifndef RB_BASE_ENTITY_H
#define RB_BASE_ENTITY_H

#include <map>
#include <memory>
#include <iostream>

namespace regionbiz {

class BaseEntity;
typedef std::shared_ptr< BaseEntity > BaseEntityPtr;

class BaseEntity
{
public:
    // main
    BaseEntity( uint64_t id );
    uint64_t getId();

    // some technick functions
    static bool isEntityExist( uint64_t id );
    static uint64_t getMaxId();
    static BaseEntityPtr getEntity( uint64_t id );

    // create new entity
    template< typename Type >
    static std::shared_ptr< Type > createWithId( uint64_t id );

    // convert
    template< typename Type >
    static std::shared_ptr< Type > convert( BaseEntityPtr base )
    {
        return std::dynamic_pointer_cast< Type >( base );
    }

protected:
    uint64_t _id = 0;

private:
    static std::map< uint64_t, BaseEntityPtr >& getEntitys();
};

//------------------------------------------------------------

template< typename Type >
std::shared_ptr< Type > BaseEntity::createWithId( uint64_t id )
{
    if( !isEntityExist( id ))
    {
        // create and add to map of all obj
        auto entity_ptr = std::shared_ptr< Type >( new Type( id ));
        getEntitys()[id] = entity_ptr;
        return entity_ptr;
    }
    else
    {
        // already exist, can't duplicate
        std::cerr << "WARNING: somthing with id = " << id
                  << " already exists" << std::endl;
        return nullptr;
    }
}

}

#endif // RB_BASE_ENTITY_H
