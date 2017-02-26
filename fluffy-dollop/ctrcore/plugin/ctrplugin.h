#ifndef CTRPLUGIN_H
#define CTRPLUGIN_H

#include <QtPlugin>
#include "ctrplugindefines.h"

using namespace ctrplugin;

class CtrPlugin : public QObject
{
public:

    CtrPlugin();
    virtual ~CtrPlugin() {}

    virtual TYPE_PLUGIN getPluginType() const = 0;

};

class CtrPluginIface : public QObject
{
    Q_OBJECT

public:
    virtual ~CtrPluginIface() {}
    virtual TYPE_PLUGIN getPluginType() const = 0;
    virtual CtrPlugin* createPlugin() = 0;
};

Q_DECLARE_INTERFACE(CtrPluginIface, "ru.vega.contour.CtrPluginIface")


#endif // CTRPLUGIN_H
