#ifndef CTRAPPPLUGIN_H
#define CTRAPPPLUGIN_H

#include "ctrplugin.h"

using namespace ctrplugin;

class CtrAppPlugin : public CtrPlugin
{
public:
    CtrAppPlugin();
    virtual  ~CtrAppPlugin();
    
    virtual TYPE_PLUGIN getPluginType() const { return TYPE_APP;}

    virtual void initPluginInterface() {}

public slots:
    /**
     * @brief showPluginInterface
     */
    virtual void showPluginInterface(bool);

    
};

#endif // CTRAPPPLUGIN_H
