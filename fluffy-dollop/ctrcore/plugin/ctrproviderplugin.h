#ifndef CTRPROVIDERPLUGIN_H
#define CTRPROVIDERPLUGIN_H

#include "ctrplugin.h"

using namespace ctrplugin;

class CtrProviderPlugin : public CtrPlugin
{
public:
    CtrProviderPlugin();
    virtual ~CtrProviderPlugin();

    virtual void instanceToFactory() = 0;


    virtual TYPE_PLUGIN getPluginType() const { return TYPE_PROVIDER;}

};

#endif // CTRPROVIDERPLUGIN_H
