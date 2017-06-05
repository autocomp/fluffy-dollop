#ifndef RB_GROUP_H
#define RB_GROUP_H

#include <set>

#include "rb_base_entity.h"
#include "rb_locations.h"

namespace regionbiz {

class GroupEntity;
typedef std::shared_ptr< GroupEntity > GroupEntityPtr;

class GroupEntity: public BaseEntity
{
    friend class RegionBizManager;
public:
    GroupEntity( uint64_t id );
    EntityType getEntityType();
    bool commit() override;

    BaseAreaPtrs getElements();
    std::vector< uint64_t > getElementsIds();

    bool addElements( std::vector< uint64_t > ids );
    bool addElements( BaseAreaPtrs elements );
    bool addElement( uint64_t id );
    bool addElement( BaseAreaPtr element );

    bool removeElements( std::vector< uint64_t > ids );
    bool removeElements( BaseAreaPtrs elements );
    bool removeElement( uint64_t id );
    bool removeElement( BaseAreaPtr element );

    uint getCount();
    bool isEmpty();
    bool isValid();
    void disband();
    GroupEntityPtr combine( GroupEntityPtr other_group );

private:
    BaseArea::AreaType getType();
    void setChanged();

    std::set< uint64_t > _ids;
    BaseArea::AreaType _type = BaseArea::AT_ROOM;
};
typedef std::vector< GroupEntityPtr > GroupEntityPtrs;

//--------------------------------------------------

class GroupWatcher
{
    friend class BaseDataTranslator;
public:
    static void appendGroup( uint64_t id );
    static std::vector< uint64_t > getChangedGroups( bool clear = true );

private:
    static std::set< uint64_t >& getChangedSet();
    static void freeChanged();
};

}

#endif // RB_GROUP_H
