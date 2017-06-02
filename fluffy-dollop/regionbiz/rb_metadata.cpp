#include "rb_metadata.h"

#include <iostream>

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

void DoubleMetadata::setValueByString( QString val )
{
    _value = val.toDouble();
}

QVariant DoubleMetadata::getValueAsVariant()
{
    return _value;
}

void DoubleMetadata::setValueByVariant( QVariant val )
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

//----------------------------------------------

StringMetadata::StringMetadata( uint64_t parent_id ):
    BaseMetadata( parent_id )
{}

QString StringMetadata::getType()
{
    return "string";
}

QString StringMetadata::getValueAsString()
{
    return _value;
}

void StringMetadata::setValueByString(QString val)
{
    _value = val;
}

QVariant StringMetadata::getValueAsVariant()
{
    return _value;
}

void StringMetadata::setValueByVariant(QVariant val)
{
    _value = val.toString();
}

QString StringMetadata::getValue()
{
    return _value;
}

void StringMetadata::setValue(QString val)
{
    _value = val;
}

//----------------------------------------------

IntegerMetadata::IntegerMetadata(uint64_t parent_id):
    BaseMetadata( parent_id )
{}

QString IntegerMetadata::getType()
{
    return "int";
}

QString IntegerMetadata::getValueAsString()
{
    return QString::number( _value );
}

void IntegerMetadata::setValueByString(QString val)
{
    _value = val.toInt();
}

QVariant IntegerMetadata::getValueAsVariant()
{
    return _value;
}

void IntegerMetadata::setValueByVariant(QVariant val)
{
    _value = val.toInt();
}

int IntegerMetadata::getValue()
{
    return _value;
}

void IntegerMetadata::setValue(int val)
{
    _value = val;
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

QString EmptyMetadata::getValueAsString()
{
    return "";
}

void EmptyMetadata::setValueByString(QString val)
{
    std::cerr << "Set value for empty metadata: "
              << val.toUtf8().data() << std::endl;
    static_assert( true, "Set value for empty metadata" );
}

QVariant EmptyMetadata::getValueAsVariant()
{
    return QVariant();
}

void EmptyMetadata::setValueByVariant(QVariant val)
{
    std::cerr << "Set value for empty metadata: "
              << val.toString().toUtf8().data() << std::endl;
    static_assert( true, "Set value for empty metadata" );
}

bool EmptyMetadata::isEmpty()
{
    return true;
}
