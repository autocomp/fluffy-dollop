#include "rb_locations.h"

#include "rb_group.h"
#include "rb_manager.h"

using namespace regionbiz;

typedef BaseArea::AreaType LocationType;
typedef Region::RegionChildFilter RegionChildFilter;

BaseArea::BaseArea(uint64_t id):
    BaseEntity( id )
{}

BaseEntity::EntityType BaseArea::getEntityType()
{
    return EntityType::ET_AREA;
}

QPolygonF BaseArea::getCoords()
{
    return _coords;
}

void BaseArea::setCoords(QPolygonF coord)
{
    _coords = coord;
}

uint64_t BaseArea::getParentId()
{
    return _parent_id;
}

BaseAreaPtr BaseArea::getParent(AreaType parent_type)
{
    auto mngr = RegionBizManager::instance();
    BaseAreaPtr parent = mngr->getBaseArea( _parent_id, parent_type );

    return parent;
}

BaseAreaPtr BaseArea::getParent()
{
    auto mngr = RegionBizManager::instance();
    BaseAreaPtr parent = mngr->getBaseArea( _parent_id );

    return parent;
}

BaseAreaPtrs BaseArea::getBaseAreaChilds()
{
    auto mngr = RegionBizManager::instance();
    auto childs = mngr->getAreaChildsByParent( _id );

    return childs;
}

GroupEntityPtr BaseArea::getGroup()
{
    auto mngr = RegionBizManager::instance();
    auto group = mngr->getGroupOfEntity( getId() );

    return group;
}

bool BaseArea::moveToGroup(GroupEntityPtr group)
{
    return group->addElement( getId() );
}

bool BaseArea::leaveGroup()
{
    auto group = getGroup();
    if( group )
        return group->removeElement( getId() );

    return true;
}

MarksHolderPtr BaseArea::convertToMarksHolder()
{
    auto holder = convert< MarksHolder>();
    if( holder )
        return holder;

    return MarksHolderPtr( new MarksHolder( 0 ));
}

LocalTransformKeeperPtr BaseArea::convertToLocalTransformHolder()
{
    auto holder = convert< LocalTransformKeeper >();
    if( holder )
        return holder;

    return LocalTransformKeeperPtr( new LocalTransformKeeper( 0 ));
}

bool BaseArea::commit()
{
    auto mngr = RegionBizManager::instance();
    bool comm_res = mngr->commitArea( _id );

    return comm_res;
}

void BaseArea::setParent(uint64_t id)
{
    _parent_id = id;
}

BizRelationKepper::BizRelationKepper(uint64_t id):
    BaseArea( id )
{}

BaseBizRelationPtrs BizRelationKepper::getBizRelations()
{
    auto mngr = RegionBizManager::instance();
    BaseBizRelationPtrs relations = mngr->getBizRelationByArea( _id );

    return relations;
}

PropertyPtrs BizRelationKepper::getPropertys()
{
    auto mngr = RegionBizManager::instance();
    BaseBizRelationPtrs relations = mngr->getBizRelationByArea( _id,
                                                                BaseBizRelation::RT_PROPERTY );

    PropertyPtrs propertys;
    for( BaseBizRelationPtr rel: relations )
    {
        PropertyPtr prop = BaseBizRelation::convert< Property >( rel );
        if( prop )
            propertys.push_back( prop );
    }
    return propertys;
}

RentPtrs BizRelationKepper::getRents()
{
    auto mngr = RegionBizManager::instance();
    BaseBizRelationPtrs relations = mngr->getBizRelationByArea( _id,
                                                                BaseBizRelation::RT_RENT );

    RentPtrs rents;
    for( BaseBizRelationPtr rel: relations )
    {
        RentPtr rent = BaseBizRelation::convert< Rent >( rel );
        if( rent )
            rents.push_back( rent );
    }
    return rents;
}

MarksHolder::MarksHolder(uint64_t id):
    _holder_id( id )
{}

uint64_t MarksHolder::getHolderId()
{
    return _holder_id;
}

MarkPtrs MarksHolder::getMarks()
{
    MarkPtrs marks;

    if( checkHolderId() )
    {
        auto mngr = RegionBizManager::instance();
        marks = mngr->getMarksByParent( _holder_id );
    }

    return marks;
}

MarkPtrs MarksHolder::getMarks(Mark::MarkType type)
{
    MarkPtrs marks;

    if( checkHolderId() )
    {
        auto mngr = RegionBizManager::instance();
        marks = mngr->getMarksByParent( _holder_id, type );
    }

    return marks;
}

MarkPtr MarksHolder::addMark( Mark::MarkType type, QPointF center )
{
    return addMark( type, QPolygonF( { center } ));
}

MarkPtr MarksHolder::addMark( Mark::MarkType type, QPolygonF coords )
{
    if( checkHolderId() )
    {
        auto mngr = RegionBizManager::instance();
        return mngr->addMark( _holder_id, type, coords );
    }

    return MarkPtr();
}

bool MarksHolder::commitMarks()
{
    if( checkHolderId() )
    {
        auto mngr = RegionBizManager::instance();
        MarkPtrs marks = mngr->getMarksByParent( _holder_id );
        bool comm = true;
        for( MarkPtr mark: marks )
            comm = comm && mark->commit();

        return comm;
    }

    return false;
}

bool MarksHolder::deleteMarks()
{
    if( checkHolderId() )
    {
        auto mngr = RegionBizManager::instance();
        MarkPtrs marks = mngr->getMarksByParent( _holder_id );
        bool del = true;
        for( MarkPtr mark: marks )
            del = del && mngr->deleteMark( mark );

        return del;
    }

    return false;
}

bool MarksHolder::checkHolderId()
{
    auto mngr = RegionBizManager::instance();
    BaseEntityPtr ent = mngr->getBaseArea( _holder_id );
    if( ent )
    {
        MarksHolderPtr holder = BaseEntity::convert< MarksHolder >( ent );
        if( holder )
            return true;
    }

    return false;
}

//------------------------------------------

Region::Region(uint64_t id):
    BaseArea( id )
{}

LocationType Region::getType()
{
    return AT_REGION;
}

std::vector<BaseAreaPtr> Region::getChilds(RegionChildFilter filter )
{
    auto mngr = RegionBizManager::instance();
    std::vector<BaseAreaPtr> childs;

    if( RCF_ALL == filter ||
            RCF_LOCATIONS == filter )
    {
        std::vector<LocationPtr> locations = mngr->getLocationsByParent( _id );
        for( LocationPtr loc: locations )
            childs.push_back( loc );
    }

    if( RCF_ALL == filter
            || RCF_FACILITYS == filter
            || RCF_ALL_FACILITYS == filter )
    {
        std::vector<FacilityPtr> facilitys = mngr->getFacilitysByParent( _id );
        for( FacilityPtr fac: facilitys )
            childs.push_back( fac );
    }

    if( RCF_ALL_FACILITYS == filter )
    {
        std::vector<LocationPtr> locations = mngr->getLocationsByParent( _id );
        for( LocationPtr loc: locations )
        {
            std::vector<FacilityPtr> facilitys = mngr->getFacilitysByParent( loc->getId() );
            for( FacilityPtr fac: facilitys )
                childs.push_back( fac );
        }
    }

    return childs;
}

Location::Location(uint64_t id):
   BaseArea( id ),
   MarksHolder( id )
{}

BaseArea::AreaType Location::getType()
{
    return AT_LOCATION;
}

QString Location::getAddress()
{
    if( isMetadataPresent( "adress" ))
        return getMetadataValue( "adress" ).toString();

    return "";
}

void Location::setAddress(QString address)
{
    if( isMetadataPresent( "address" ))
        setMetadataValue( "address", address );
    else
        addMetadata( "string", "address", address );
}

std::vector<FacilityPtr> Location::getChilds()
{
    auto mngr = RegionBizManager::instance();
    std::vector<FacilityPtr> facilitys = mngr->getFacilitysByParent( _id );

    return facilitys;
}

Facility::Facility(uint64_t id):
    BizRelationKepper( id ),
    MarksHolder( id )
{}

BaseArea::AreaType Facility::getType()
{
    return AT_FACILITY;
}

QString Facility::getAddress()
{
    if( isMetadataPresent( "adress" ))
        return getMetadataValue( "adress" ).toString();

    return "";
}

void Facility::setAddress(QString address)
{
    if( isMetadataPresent( "address" ))
        setMetadataValue( "address", address );
    else
        addMetadata( "string", "address", address );
}

QString Facility::getCadastralNumber()
{
    if( isMetadataPresent( "cadastral_number" ))
        return getMetadataValue( "cadastral_number" ).toString();

    return "";
}

void Facility::setCadastralNumber(QString number)
{
    if( isMetadataPresent( "cadastral_number" ))
        setMetadataValue( "cadastral_number", number );
    else
        addMetadata( "string", "cadastral_number", number );
}

std::vector<FloorPtr> Facility::getChilds()
{
    auto mngr = RegionBizManager::instance();
    std::vector<FloorPtr> floors = mngr->getFloorsByParent( _id );

    return floors;
}

bool Facility::isHaveTransform()
{
    auto mngr = RegionBizManager::instance();
    return mngr->isHaveTransform( _id );
}

bool Facility::setTransform(QTransform transform)
{
    auto mngr = RegionBizManager::instance();
    return mngr->setTransform( _id, transform );
}

QTransform Facility::getTransform()
{
    auto mngr = RegionBizManager::instance();
    return mngr->getTransform( _id );
}

void Facility::resetTransform()
{
    auto mngr = RegionBizManager::instance();
    mngr->resetTransform( _id );
}

bool Facility::commitTransformMatrix()
{
    auto mngr = RegionBizManager::instance();
    return mngr->commitTransformOfFacility( _id );
}

bool Facility::isHaveEtalonPlan()
{
    return getEtalonPlan() != nullptr;
}

#define ETALON_PLAN "etalon_plan"
BaseFileKeeperPtr Facility::getEtalonPlan()
{
    bool check = isMetadataPresent( ETALON_PLAN );
    if( !check )
        return nullptr;

    QString id_file = getMetadata( ETALON_PLAN )->getValueAsString();
    auto plans = getFilesByType( BaseFileKeeper::FT_PLAN_VECTOR );
    for( BaseFileKeeperPtr file: plans )
        if( id_file == file->getId() )
            return file;
    plans = getFilesByType( BaseFileKeeper::FT_PLAN_RASTER );
    for( BaseFileKeeperPtr file: plans )
        if( id_file == file->getId() )
            return file;

    return nullptr;
}

bool Facility::setEtalonPlan( BaseFileKeeperPtr file )
{
    if( !file
            || file->getEntityId() != _id
            || ( file->getType() != BaseFileKeeper::FT_PLAN_RASTER
                 && file->getType() != BaseFileKeeper::FT_PLAN_VECTOR ))
        return false;

    bool has = isMetadataPresent( ETALON_PLAN );
    if( has )
        return setMetadataValue( ETALON_PLAN, file->getId() );
    else
        return addMetadata( "string", ETALON_PLAN, file->getId() );
}

void Facility::resetEtalonFile()
{
    bool has = isMetadataPresent( ETALON_PLAN );
    if( has )
        setMetadataValue( ETALON_PLAN, "" );
}
#undef ETALON_PLAN

Floor::Floor(uint64_t id):
    BizRelationKepper( id ),
    MarksHolder( id ),
    LocalTransformKeeper( id )
{}

BaseArea::AreaType Floor::getType()
{
    return AT_FLOOR;
}

uint16_t Floor::getNumber()
{
    if( isMetadataPresent( "number" ))
        return getMetadataValue( "number" ).toInt();

    return 0;
}

void Floor::setNumber(uint16_t number)
{
    if( isMetadataPresent( "number" ))
        setMetadataValue( "number", number );
    else
        addMetadata( "int", "number", number );
}

RoomPtrs Floor::getChilds()
{
    auto mngr = RegionBizManager::instance();
    std::vector<RoomPtr> rooms = mngr->getRoomsByParent( _id );

    return rooms;
}

Room::Room(uint64_t id):
    BizRelationKepper( id ),
    MarksHolder( id ),
    LocalTransformKeeper( id )
{}

BaseArea::AreaType Room::getType()
{
    return AT_ROOM;
}

//-----------------------------


LocalTransformKeeper::LocalTransformKeeper(uint64_t id)
{
    _holder_id = id;
}

bool LocalTransformKeeper::isHaveTransform()
{
    auto facil = getFacilityParent();
    if( !facil )
        return false;

    auto mngr = RegionBizManager::instance();
    return mngr->isHaveTransform( facil->getId() );
}

bool LocalTransformKeeper::setTransform(QTransform transform)
{
    auto facil = getFacilityParent();
    if( !facil )
        return false;

    auto mngr = RegionBizManager::instance();
    return mngr->setTransform( facil->getId(), transform );
}

QTransform LocalTransformKeeper::getTransform()
{
    auto facil = getFacilityParent();
    if( !facil )
        return QTransform();

    auto mngr = RegionBizManager::instance();
    return mngr->getTransform( facil->getId() );
}

void LocalTransformKeeper::resetTransform()
{
    auto facil = getFacilityParent();
    if( !facil )
        return;

    auto mngr = RegionBizManager::instance();
    mngr->resetTransform( facil->getId() );
}

bool LocalTransformKeeper::commitTransformMatrix()
{
    auto facil = getFacilityParent();
    if( !facil )
        return false;

    return facil->commitTransformMatrix();
}

FacilityPtr LocalTransformKeeper::getFacilityParent()
{
    auto mngr = RegionBizManager::instance();

    BaseAreaPtr this_area = mngr->getBaseArea( _holder_id );
    if( !this_area )
        return nullptr;

    while( this_area->getType() != BaseArea::AT_FACILITY
           || !this_area )
        this_area = this_area->getParent();

    if( this_area )
        return this_area->convert< Facility >();

    return nullptr;
}
