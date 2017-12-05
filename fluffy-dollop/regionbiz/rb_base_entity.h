#ifndef RB_BASE_ENTITY_H
#define RB_BASE_ENTITY_H

#include <unordered_map>
#include <memory>
#include <mutex>
#include <iostream>
#include <set>

#include "rb_metadata.h"
#include "rb_files.h"

namespace regionbiz {

class BaseEntity;
typedef std::shared_ptr< BaseEntity > BaseEntityPtr;
typedef std::set< BaseEntityPtr > BaseEntityPtrSet;

class RegionBizManager;

class BaseEntity
{
    friend class RegionBizManager;
public:
    enum EntityType
    {
        ET_AREA,
        ET_MARK,
        ET_GROUP,
        ET_RELATION,
        ET_GRAPH,
        ET_GRAPH_NODE,
        ET_GRAPH_EDGE
    };

    // main
    BaseEntity( uint64_t id );
    uint64_t getId();
    virtual EntityType getEntityType() = 0;
    virtual bool commit() = 0;

    // params
    QString getDescription();
    void setDesription( QString descr );
    QString getName();
    void setName( QString name );

    // some technick functions
    static bool isEntityExist( uint64_t id );
    static uint64_t getMaxId();

    // metadata
    bool isMetadataPresent( QString name );
    QVariant getMetadataValue( QString name );
    BaseMetadataPtr getMetadata( QString name );
    MetadataByName getMetadataMap();
    bool setMetadataValue( QString name , QVariant val );
    bool addMetadata( QString type, QString name, QVariant val = QVariant() );
    bool addMetadata( BaseMetadataPtr data );
    bool deleteMetadata( QString name );

    // constraints
    Constraints getConstraints();
    Constraints getConstraints( Constraint::ConstraintType type );

    // files
    BaseFileKeeperPtrs getFiles();
    BaseFileKeeperPtrs getFilesByType( BaseFileKeeper::FileType type );

    // TODO make create entity private
    // create new entity
    template< typename Type >
    static std::shared_ptr< Type > createWithId( uint64_t id, uint64_t parent_id = 0 );

    // convert
    template< typename Type >
    static std::shared_ptr< Type > convert( BaseEntityPtr base )
    {
        return std::dynamic_pointer_cast< Type >( base );
    }

    template< typename Type >
    std::shared_ptr< Type > convert()
    {
        BaseEntityPtr ptr = getItself();
        return std::dynamic_pointer_cast< Type >( ptr );
    }

protected:
    // private technick functions
    static BaseEntityPtr getEntity( uint64_t id );
    static bool deleteEntity( BaseEntityPtr ent, uint64_t parent_id = 0 );
    static bool deleteEntity( uint64_t id, uint64_t parent_id = 0 );

    // get smart pointer on this
    BaseEntityPtr getItself();

    uint64_t _id = 0;
    QString _description = "";
    QString _name = "";
    // TODO flag of commited

private:
    static std::unordered_map< uint64_t, BaseEntityPtr >& getEntitys();
    static std::map<uint64_t, BaseEntityPtrSet>& getChildsOfParent();

    template< typename Type >
    static std::vector< std::shared_ptr< Type >>
    getEntitysByType( BaseEntity::EntityType type );

    template< typename Type >
    static std::vector< std::shared_ptr< Type >>
    getEntitysByTypeAndParent( BaseEntity::EntityType type, uint64_t parent_id );

    template< typename Type >
    static std::vector< std::shared_ptr< Type >>
    getEntitysByParent( uint64_t parent_id );

    static uint64_t _max_id;
    static std::recursive_mutex _mutex;
};

//------------------------------------------------------------

template< typename Type >
std::shared_ptr< Type > BaseEntity::createWithId( uint64_t id, uint64_t parent_id )
{
    if( !isEntityExist( id ))
    {
        // create and add to map of all obj
        auto entity_ptr = std::shared_ptr< Type >( new Type( id ));

        if( entity_ptr )
        {
            BaseEntity::_mutex.lock();
            getEntitys()[id] = entity_ptr;
            if( parent_id )
            {
                #define SET getChildsOfParent()[ parent_id ]
                SET.insert( entity_ptr );
                #undef SET
            }
            BaseEntity::_mutex.unlock();
        }

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

template<typename Type>
std::vector<std::shared_ptr< Type >>
BaseEntity::getEntitysByType( BaseEntity::EntityType type )
{
    std::vector< std::shared_ptr< Type >> entitys;

    _mutex.lock();
    for( auto pair: getEntitys() )
    {
        BaseEntityPtr entity = pair.second;
        if( type == entity->getEntityType() )
        {
            std::shared_ptr< Type > entity_typed =
                    BaseEntity::convert< Type >( entity );
            if( entity_typed )
                entitys.push_back( entity_typed );
        }
    }
    _mutex.unlock();

    return std::move( entitys );
}

template<typename Type>
std::vector< std::shared_ptr< Type >>
BaseEntity::getEntitysByTypeAndParent( BaseEntity::EntityType type, uint64_t parent_id )
{
    std::vector< std::shared_ptr< Type >> entitys;

    _mutex.lock();
    if( getChildsOfParent().find( parent_id ) != getChildsOfParent().end() )
    {
        for( BaseEntityPtr entity: getChildsOfParent()[ parent_id ] )
        {
            if( type == entity->getEntityType() )
            {
                std::shared_ptr< Type > entity_typed =
                        BaseEntity::convert< Type >( entity );
                if( entity_typed )
                    entitys.push_back( entity_typed );
            }
        }
    }
    _mutex.unlock();

    return std::move( entitys );

}

template<typename Type>
std::vector< std::shared_ptr< Type >>
BaseEntity::getEntitysByParent( uint64_t parent_id )
{
    std::vector< std::shared_ptr< Type >> entitys;

    _mutex.lock();
    if( getChildsOfParent().find( parent_id ) != getChildsOfParent().end() )
    {
        for( BaseEntityPtr entity: getChildsOfParent()[ parent_id ] )
        {
            std::shared_ptr< Type > entity_typed =
                    BaseEntity::convert< Type >( entity );
            if( entity_typed )
                entitys.push_back( entity_typed );
        }
    }
    _mutex.unlock();

    return std::move( entitys );
}

}

#endif // RB_BASE_ENTITY_H
