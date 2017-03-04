#include "serializeriface.h"

EmbeddedSerializationData::EmbeddedSerializationData()
{
    data = 0;
    parent = 0;
    id = 0;
    tp = ew::EWT_WIDGET;
}

EmbeddedSerializationData::EmbeddedSerializationData(const EmbeddedSerializationData& prev)
{
    data = prev.data;/*new ew::EmbeddedWidgetBaseStruct(*prev.data);*/
    parent = prev.parent;
    id = prev.id;
    tp = prev.tp;
}

EmbeddedSerializationData& EmbeddedSerializationData::operator=(const EmbeddedSerializationData& prev)
{
    data = prev.data; /*new ew::EmbeddedWidgetBaseStruct(*prev.data);*/
    parent = prev.parent;
    id = prev.id;
    tp = prev.tp;
    return *this;
}

bool EmbeddedSerializationData::operator!= ( const EmbeddedSerializationData & prev ) const
{
    if( (*prev.data != *data) ||
        parent != prev.parent ||
            id != prev.id ||
            tp != prev.tp)
    {
        return true;
    }

    return false;
}

bool EmbeddedSerializationData::operator == ( const EmbeddedSerializationData & prev ) const
{
    if( (*prev.data == *data) &&
        (parent == prev.parent) &&
            (id == prev.id) &&
            (tp == prev.tp) )
    {
        return true;
    }

    return false;
}


SerializerConfigBase::SerializerConfigBase(QObject* parent) :
    QObject(parent)
{

}


SerializerIFace::SerializerIFace(QObject *parent) :
    QObject(parent)
{

}
