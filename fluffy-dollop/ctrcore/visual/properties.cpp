#include "properties.h"
using namespace visualize_system;

void Properties::setProperty(QString propertyName, const QVariant& property)
{
    _viewProperties.insert(propertyName, property);
}

bool Properties::getProperty(QString propertyName, QVariant& property)
{
    auto it = _viewProperties.find(propertyName);
    if(it != _viewProperties.end())
    {
        property = it.value();
        return true;
    }
    else
    {
        property = QVariant();
        return false;
    }
}

QMap<QString,QVariant> Properties::getProperties()
{
    return _viewProperties;
}
