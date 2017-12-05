#include "rb_group.h"

#include "rb_manager.h"

using namespace regionbiz;

GroupEntity::GroupEntity(uint64_t id):
    BaseEntity( id )
{}

BaseEntity::EntityType GroupEntity::getEntityType()
{
    return ET_GROUP;
}

BaseAreaPtrs GroupEntity::getElements()
{
    BaseAreaPtrs areas;

    auto mngr = RegionBizManager::instance();
    for( uint64_t id : _ids)
    {
        // TODO check type of element
        auto area = mngr->getBaseArea( id );
        if( area )
            areas.push_back( area );
        else
            std::cerr << "Can't find area element " << id
                      << " of group " << getId() << std::endl;
    }

    return areas;
}

std::vector<uint64_t> GroupEntity::getElementsIds()
{
    std::vector<uint64_t> ids;

    auto mngr = RegionBizManager::instance();
    for( uint64_t id : _ids)
    {
        // TODO check type of element
        auto area = mngr->getBaseArea( id );
        if( area )
            ids.push_back( id );
        else
            std::cerr << "Element of group: " << id  << " isn't part of group"
                      << getId() << std::endl;
    }

    return ids;
}

bool GroupEntity::addElements(std::vector<uint64_t> ids)
{
    if( !isValid() )
        return false;

    auto mngr = RegionBizManager::instance();

    for( uint64_t id: ids )
    {
        auto element = mngr->getBaseArea( id );
        if( element )
        {
            GroupType group_type = getGroupType();
            switch ( group_type ) {
            case GT_AREAS:
            {
                if( !addElementToAreaGroup( element ))
                    return false;
                break;
            }

            case GT_ELEVATOR:
            case GT_STAIRS:
            {
                if( !addElementToStairsOrElevator( element ))
                    return false;
                break;
            }

            }
        }
        else
            std::cerr << "Can't add element " << id
                      << " to group " << getId() << std::endl;
    }

    setChanged();
    return true;
}

bool GroupEntity::addElements( BaseAreaPtrs elements )
{
    std::vector< uint64_t > ids;
    for( auto element: elements )
        ids.push_back( element->getId() );

    return addElements( ids );
}

bool GroupEntity::addElement( uint64_t id )
{
    std::vector< uint64_t > ids = { id };
    return addElements( ids );
}

bool GroupEntity::addElement(BaseAreaPtr element)
{
    return addElement( element->getId() );
}

bool GroupEntity::removeElements(std::vector<uint64_t> ids)
{
    if( !isValid() )
        return false;

    for( uint64_t id: ids )
    {
        if( _ids.count( id ))
            _ids.erase( _ids.find( id ));
    }

    setChanged();
    return true;
}

bool GroupEntity::removeElements(BaseAreaPtrs elements)
{
    std::vector< uint64_t > ids;
    for( auto element: elements )
        ids.push_back( element->getId() );
    return removeElements( ids );
}

bool GroupEntity::removeElement(uint64_t id)
{
    std::vector< uint64_t > ids = { id };
    return removeElements( ids );
}

bool GroupEntity::removeElement( BaseAreaPtr element )
{
    return removeElement( element->getId() );
}

uint GroupEntity::getCount()
{
    return _ids.size();
}

bool GroupEntity::isEmpty()
{
    return _ids.empty();
}

BaseArea::AreaType GroupEntity::getType()
{
    return _type;
}

void GroupEntity::disband()
{
    _ids.clear();
    setChanged();
}

GroupEntityPtr GroupEntity::combine( GroupEntityPtr other_group )
{
    using namespace std;

    // if other type of group - return this group
    if( other_group->getGroupType() != getGroupType() )
    {
        cerr << "Try combine groups of different type" << endl;
        return RegionBizManager::instance()->getGroup( getId() );
    }

    // if other group deleted - just return this group
    if( !other_group->isValid() )
        return RegionBizManager::instance()->getGroup( getId() );

    other_group->addElements( getElements() );
    disband();

    setChanged();
    other_group->setChanged();

    return other_group;
}

bool GroupEntity::commit()
{
    auto mngr = RegionBizManager::instance();
    return mngr->commitGroup( getId() );
}

GroupEntity::GroupType GroupEntity::getGroupType()
{
    // by default - this is group of rooms
    if( !isMetadataPresent( GROUP_TYPE_METADATA ))
        return GT_AREAS;

    QString type = getMetadata( GROUP_TYPE_METADATA )->getValueAsString();
    if( "areas" == type )
        return GT_AREAS;
    if( "stairs" == type )
        return GT_STAIRS;
    if( "elevator" == type )
        return GT_ELEVATOR;

    // default
    return GT_AREAS;
}

void GroupEntity::setChanged()
{
    GroupWatcher::appendGroup( getId() );
}

bool GroupEntity::setGroupType(GroupEntity::GroupType type)
{
    QString type_str;
    switch( type )
    {
    case GT_AREAS:
        type_str = "areas";
        break;
    case GT_STAIRS:
        type_str = "stairs";
        break;
    case GT_ELEVATOR:
        type_str = "elevator";
        break;
    }

    if( type_str.isEmpty() )
        return false;
    return addMetadata( "string", GROUP_TYPE_METADATA, type_str );
}

bool GroupEntity::addElementToAreaGroup(BaseAreaPtr element)
{
    if( element->getType() == BaseArea::AT_ROOM
            && element->convert< Room >()->getRoomType() != Room::RT_COMMON )
    {
        std::cerr << "New element " << element->getId() << " of group " <<
                     getId() << " don't a common room" << std::endl;
        return false;
    }

    // check type (if empty - set type)
    if( isEmpty() )
    {
        _type = element->getType();
        // check previous group
        if( element->getGroup() )
            element->leaveGroup();
        _ids.insert( element->getId() );
    }
    else
    {
        if( _type == element->getType() )
        {            
            // check previous group
            if( element->getGroup() )
                element->leaveGroup();
            _ids.insert( element->getId() );
        }
        else
        {
            std::cerr << "New element " << element->getId() << " of group " <<
                         getId() << "has wrong type" << std::endl;
            return false;
        }
    }
    return true;
}

bool GroupEntity::addElementToStairsOrElevator(BaseAreaPtr element)
{
    using namespace std;

    if( BaseArea::AT_ROOM != element->getType() )
    {
        cerr << "Wrong type of Area element for Vertiacal group: "
             << element->getId() << endl;
        return false;
    }

    GroupType type = getGroupType();
    Room::RoomType room_type = element->convert< Room >()->getRoomType();
    if(( type == GT_STAIRS && room_type == Room::RT_STAIRS )
            || ( type == GT_ELEVATOR && room_type == Room::RT_ELEVATOR ))
    {
        // check rooms on same floor
        uint64_t parent_element = element->getParentId();
        for( BaseAreaPtr elem: getElements() )
            if( elem->getParentId() == parent_element
                    && elem->getId() != element->getId() )
            {
                cerr << "Other element of group present on same floor for: "
                     << element->getId() << endl;
                return false;
            }

        // check previous group
        if( element->getGroup() )
            element->leaveGroup();
        _ids.insert( element->getId() );
    }
    else
    {
        cerr << "Wrong type of Room element for Vertiacal group: "
             << element->getId() << endl;
        return false;
    }

    return true;
}

bool GroupEntity::isValid()
{
    auto mngr = RegionBizManager::instance();
    auto group = mngr->getGroup( getId() );
    bool valid = ( group != nullptr );
    return valid;
}

//-----------------------------------------------------------------

void GroupWatcher::appendGroup( uint64_t id )
{
    getChangedSet().insert( id );
}

std::vector<uint64_t> GroupWatcher::getChangedGroups( bool clear )
{
    std::vector<uint64_t> groups;
    for( uint64_t id: getChangedSet() )
        groups.push_back( id );

    if( clear )
        freeChanged();

    return groups;
}

std::set<uint64_t> &GroupWatcher::getChangedSet()
{
    static std::set<uint64_t> set_of_change;
    return set_of_change;
}

void GroupWatcher::freeChanged()
{
    getChangedSet().clear();
}
