#include "rb_metadata.h"

#include <iostream>

#include "rb_manager.h"

using namespace regionbiz;

BaseMetadata::BaseMetadata(uint64_t parent_id):
    _parent_id( parent_id )
{}

bool BaseMetadata::isEmpty()
{
    return false;
}

uint64_t BaseMetadata::getParentId()
{
    return _parent_id;
}

QString BaseMetadata::getName()
{
    return _name;
}

void BaseMetadata::setName(QString name)
{
    _name = name;
}

LayerPtr BaseMetadata::getLayer()
{
    auto mngr = RegionBizManager::instance();
    return mngr->getLayerOfMetadataName( getName() );
}

void BaseMetadata::moveToLayer(LayerPtr layer)
{
    layer->addMetadataName( getName() );
}

void BaseMetadata::leaveLayer()
{
    auto layer = getLayer();
    if( layer )
        layer->removeMetadataName( getName() );
}

void BaseMetadata::removeForEntity( uint64_t id )
{
    getMutex().lock();
    getMetadatas().erase( id );
    getMutex().unlock();
}

void BaseMetadata::addForEntityByName( BaseMetadataPtr data )
{
    getMutex().lock();
    getMetadatas()[ data->getParentId() ][ data->getName() ] = data;
    getMutex().unlock();
}

MetadataById& BaseMetadata::getMetadatas()
{
    static MetadataById meta;
    return meta;
}

std::recursive_mutex& BaseMetadata::getMutex()
{
    static std::recursive_mutex mutex;
    return mutex;
}

std::vector< Constraint > BaseMetadata::getConstraints()
{
    auto empty = std::vector< Constraint >();

    auto mngr = RegionBizManager::instance();
    auto entity = mngr->getBaseEntity( _parent_id );
    if( ! entity )
        return empty;

    switch( entity->getEntityType() )
    {
    case BaseEntity::ET_AREA:
    {
        auto area = entity->convert< BaseArea >();
        return ConstraintsManager::getConstraints( area->getType() );
    }

    case BaseEntity::ET_MARK:
    {
        auto mark = entity->convert< Mark >();
        return ConstraintsManager::getConstraints( mark->getMarkType() );
    }

    case BaseEntity::ET_GROUP:
        return ConstraintsManager::getConstraints( BaseEntity::ET_GROUP );

    case BaseEntity::ET_RELATION:
    {
        // TODO Constrait get for type of relation
        static_assert( true, "Not realised constrait for Relation" );
    }

    }

    return empty;
}

void BaseMetadata::printIncorrectConstraint(Constraint cons)
{
    std::cerr << "Incorrect constrait of " << getName().toUtf8().data()
              << " metadata of type " << getType().toUtf8().data()
              << ": " << cons.getConstraint().toUtf8().data() << std::endl;
}

void BaseMetadata::printWrongCheckConstraints()
{
    std::cerr << "Incorrect check constrait of " << getName().toUtf8().data()
              << " metadata of type " << getType().toUtf8().data() << std::endl;
}

BaseMetadataPtr BaseMetadata::getItself()
{
    BaseMetadataPtr itself;

    getMutex().lock();
    if( getMetadatas().find( _parent_id ) != getMetadatas().end() )
    {
        auto metadatas = getMetadatas()[ _parent_id ];
        if( metadatas.find( _name ) != metadatas.end() )
            itself = metadatas[ _name ];
    }
    getMutex().unlock();

    return itself;
}

//-------------------------------------------

DoubleMetadata::DoubleMetadata(uint64_t parent_id):
    BaseMetadata( parent_id )
{}

QString DoubleMetadata::getType()
{
    return "double";
}

QString DoubleMetadata::getValueAsString()
{
    return QString::number(  _value );
}

bool DoubleMetadata::setValueByString( QString val )
{
    return setValue( val.toDouble() );
}

QVariant DoubleMetadata::getValueAsVariant()
{
    return _value;
}

bool DoubleMetadata::setValueByVariant( QVariant val )
{
    return setValue( val.toDouble() );
}

double DoubleMetadata::getValue()
{
    return _value;
}

bool DoubleMetadata::setValue(double val)
{
    if( !checkConstraitsByVariant( val ))
    {
        printWrongCheckConstraints();
        return false;
    }

    _value = val;

    return true;
}

bool DoubleMetadata::checkConstraits()
{
    return checkConstraitsByVariant( _value );
}

bool DoubleMetadata::checkConstraitsByVariant(QVariant new_value)
{
    if( new_value.isNull() )
        return false;

    auto constraits = getConstraints();
    for( Constraint cons: constraits )
    {
        auto min_max = cons.getConstraintAsList();
        if( min_max.size() != 2 )
        {
            printIncorrectConstraint( cons );
            continue;
        }

        if( min_max[ 0 ].isValid() )
        {
            double min = min_max[ 0 ].toDouble();
            if( new_value.toDouble() < min )
                return false;
        }
        if( min_max[ 1 ].isValid() )
        {
            double max = min_max[ 1 ].toDouble();
            if( new_value.toDouble() > max )
                return false;
        }
    }

    return true;
}

//----------------------------------------------

StringMetadata::StringMetadata( uint64_t parent_id ):
    BaseMetadata( parent_id )
{}

QString StringMetadata::getType()
{
    return "string";
}

bool StringMetadata::checkConstraits()
{
    return checkConstraitsByVariant( _value );
}

bool StringMetadata::checkConstraitsByVariant(QVariant new_value)
{
    auto constraits = getConstraints();
    for( Constraint cons: constraits )
    {
        QString reg_exp = cons.getConstraint();
        QRegExp re( reg_exp );
        if( !re.exactMatch( new_value.toString() ))
            return false;
    }

    return true;
}

QString StringMetadata::getValueAsString()
{
    return _value;
}

bool StringMetadata::setValueByString(QString val)
{
    return setValue( val );
}

QVariant StringMetadata::getValueAsVariant()
{
    return _value;
}

bool StringMetadata::setValueByVariant(QVariant val)
{
    return setValue( val.toString() );
}

QString StringMetadata::getValue()
{
    return _value;
}

bool StringMetadata::setValue(QString val)
{
    if( !checkConstraitsByVariant( val ))
    {
        printWrongCheckConstraints();
        return false;
    }

    _value = val;

    return true;
}

//----------------------------------------------

IntegerMetadata::IntegerMetadata(uint64_t parent_id):
    BaseMetadata( parent_id )
{}

QString IntegerMetadata::getType()
{
    return "int";
}

bool IntegerMetadata::checkConstraits()
{
    return checkConstraitsByVariant( _value );
}

bool IntegerMetadata::checkConstraitsByVariant(QVariant new_value)
{
    if( new_value.isNull() )
        return false;

    auto constraits = getConstraints();
    for( Constraint cons: constraits )
    {
        auto min_max = cons.getConstraintAsList();
        if( min_max.size() != 2 )
        {
            printIncorrectConstraint( cons );
            continue;
        }

        if( min_max[ 0 ].isValid() )
        {
            int min = min_max[ 0 ].toInt();
            if( new_value.toInt() < min )
                return false;
        }
        if( min_max[ 1 ].isValid() )
        {
            int max = min_max[ 1 ].toInt();
            if( new_value.toInt() > max )
                return false;
        }
    }

    return true;
}

QString IntegerMetadata::getValueAsString()
{
    return QString::number( _value );
}

bool IntegerMetadata::setValueByString(QString val)
{
    return setValue( val.toInt() );
}

QVariant IntegerMetadata::getValueAsVariant()
{
    return _value;
}

bool IntegerMetadata::setValueByVariant(QVariant val)
{
    setValue( val.toInt() );
}

int IntegerMetadata::getValue()
{
    return _value;
}

bool IntegerMetadata::setValue(int val)
{
    if( !checkConstraitsByVariant( val ))
    {
        printWrongCheckConstraints();
        return false;
    }

    _value = val;

    return true;
}

//----------------------------------------------------

BaseMetadataPtr MetadataFabric::createMetadata( QString type,
                                                uint64_t parent_id )
{
    if( "double" == type )
        return DoubleMetadataPtr( new DoubleMetadata( parent_id ));
    if( "int" == type )
        return IntegerMetadataPtr( new IntegerMetadata( parent_id ));
    if( "string" == type )
        return StringMetadataPtr( new StringMetadata( parent_id ));

    return nullptr;
}

EmptyMetadata::EmptyMetadata():
    BaseMetadata( 0 )
{}

EmptyMetadataPtr EmptyMetadata::instance()
{
    static EmptyMetadataPtr empty = EmptyMetadataPtr( new EmptyMetadata() );
    return empty;
}

QString EmptyMetadata::getType()
{
    return "";
}

bool EmptyMetadata::checkConstraits()
{
    static_assert( true, "Why are you check constraints "
                         "of empty metadata?" );
    return false;
}

bool EmptyMetadata::checkConstraitsByVariant( QVariant /*new_value*/ )
{
    static_assert( true, "Why are you check constraints "
                         "of empty metadata?" );
    return false;
}

QString EmptyMetadata::getValueAsString()
{
    return "";
}

bool EmptyMetadata::setValueByString(QString val)
{
    std::cerr << "Set value for empty metadata: "
              << val.toUtf8().data() << std::endl;
    static_assert( true, "Set value for empty metadata" );

    return false;
}

QVariant EmptyMetadata::getValueAsVariant()
{
    return QVariant();
}

bool EmptyMetadata::setValueByVariant(QVariant val)
{
    std::cerr << "Set value for empty metadata: "
              << val.toString().toUtf8().data() << std::endl;
    static_assert( true, "Set value for empty metadata" );

    return false;
}

bool EmptyMetadata::isEmpty()
{
    return true;
}
