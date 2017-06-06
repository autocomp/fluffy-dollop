#include "rb_layers.h"

using namespace regionbiz;

uint64_t Layer::getId()
{
    return _id;
}

QString Layer::getName()
{
    return _name;
}

std::vector< QString > Layer::getMetadataNames()
{
    std::vector< QString > names;
    for( QString name: _metadata )
        names.push_back( name );

    return names;
}

Layer::Layer(uint64_t id, QString name):
    _id( id ), _name( name )
{}
