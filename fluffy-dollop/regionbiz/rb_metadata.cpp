#include "rb_metadata.h"

using namespace regionbiz;

typedef BaseMetadata::MetadataParentType MetadataParentType;

BaseMetadata::BaseMetadata(uint64_t parent_id):
    _parent_id( parent_id )
{}

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

DoubleMetadata::DoubleMetadata(uint64_t parent_id):
    BaseMetadata( parent_id )
{}

QString DoubleMetadata::getType()
{
    return "double";
}

MetadataParentType DoubleMetadata::parentType()
{
    return MPT_AREA;
}

QString DoubleMetadata::getValueAsString()
{
    return QString::number(  _value );
}

void DoubleMetadata::setValueByString( QString val )
{
    _value = val.toDouble();
}

double DoubleMetadata::getValue()
{
    return _value;
}

void DoubleMetadata::setValue(double val)
{
    _value = val;
}

BaseMetadataPtr MetadataFabric::createMetadata( QString type,
                                                uint64_t parent_id )
{
    if( "double" == type )
        return DoubleMetadataPtr( new DoubleMetadata( parent_id ));

    return nullptr;
}
