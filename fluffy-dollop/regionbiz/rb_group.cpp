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
            // check type (if empty - set type)
            if( isEmpty() )
            {
                _type = element->getType();
                // check previous group
                if( element->getGroup() )
                    element->leaveGroup();
                _ids.insert( id );
            }
            else
            {
                if( _type == element->getType() )
                {
                    // check previous group
                    if( element->getGroup() )
                        element->leaveGroup();
                    _ids.insert( id );
                }
                else
                {
                    std::cerr << "New element " << id << " of group " <<
                                 getId() << "has wrong type" << std::endl;
                    return false;
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

void GroupEntity::setChanged()
{
    GroupWatcher::appendGroup( getId() );
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
