#ifndef TMS_PROVIDER_H
#define TMS_PROVIDER_H

#include <ctrcore/plugin/ctrproviderplugin.h>
#include <QObject>
#include <QtPlugin>

class TmsProvider : public CtrProviderPlugin
{
public:
  TmsProvider();
  ~TmsProvider();
  void instanceToFactory();
};


class TmsProviderInterface : public CtrPluginIface
{
    Q_OBJECT
    Q_INTERFACES(CtrPluginIface)
    Q_PLUGIN_METADATA(IID "ru.vega.contour.CtrPluginIface")
public:
    ~TmsProviderInterface() {}
    virtual CtrPlugin* createPlugin()
    {
        return new TmsProvider();
    }

    virtual TYPE_PLUGIN getPluginType() const {return TYPE_PROVIDER;}

};


#endif
