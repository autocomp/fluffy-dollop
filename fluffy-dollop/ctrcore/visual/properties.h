#ifndef PROVIDERPROPERTIES_H
#define PROVIDERPROPERTIES_H

#include <QMap>
#include <QVariant>
#include "visualizertype.h"

namespace visualize_system
{
class Properties
{
public:
    Properties() {}
    void setProperty(QString propertyName, const QVariant& property);
    bool getProperty(QString propertyName, QVariant& property);
    QMap<QString,QVariant> getProperties();
private:
    QMap<QString,QVariant> _viewProperties;
};
}
#endif
