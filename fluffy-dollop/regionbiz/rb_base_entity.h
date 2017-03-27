#ifndef RB_BASE_ENTITY_H
#define RB_BASE_ENTITY_H

#include <map>
#include <memory>
#include <iostream>
#include "rb_metadata.h"

namespace regionbiz {

class BaseEntity;
typedef std::shared_ptr< BaseEntity > BaseEntityPtr;

class RegionBizManager;

class BaseEntity
{
    friend class RegionBizManager;
public:
    enum EntityType
    {
        ET_AREA,
        ET_MARK,
        ET_RELATION
    };

    // main
    BaseEntity( uint64_t id );
    uint64_t getId();
    virtual EntityType getEntityType() = 0;

    // params
    QString getDescription();
    void setDesription( QString descr );
    QString getName();
    void setName( QString name );

    // some technick functions
    static bool isEntityExist( uint64_t id );
    static uint64_t getMaxId();
    static BaseEntityPtr getEntity( uint64_t id );

    // metadata
    bool isMetadataPresent( QString name );
    QVariant getMetadataValue( QString name );
    BaseMetadataPtr getMetadata( QString name );
    MetadataByName getMetadataMap();
    bool setMetadataValue( QString name , QVariant val );
    bool addMetadata( QString type, QString name, QVariant val = QVariant() );
    bool addMetadata( BaseMetadataPtr data );

    // create new entity
    template< typename Type >
    static std::shared_ptr< Type > createWithId( uint64_t id );

    // convert
    template< typename Type >
    static std::shared_ptr< Type > convert( BaseEntityPtr base )
    {
        return std::dynamic_pointer_cast< Type >( base );
    }

    // WARNING slow converter
    template< typename Type >
    std::shared_ptr< Type > convert()
    {
        BaseEntityPtr ptr = getItself();
        return std::dynamic_pointer_cast< Type >( ptr );
    }

protected:
    static bool deleteEntity( BaseEntityPtr ent );
    static bool deleteEntity( uint64_t id );

    // get smart pointer on this
    BaseEntityPtr getItself();

    uint64_t _id = 0;
    QString _description = "";
    QString _name = "";

private:
    static std::map< uint64_t, BaseEntityPtr >& getEntitys();
    static uint64_t _max_id;
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
