#ifndef IMAGE_PROVIDER_H
#define IMAGE_PROVIDER_H

#include <ctrcore/plugin/ctrproviderplugin.h>
#include <QObject>
#include <QtPlugin>

class ImgProvider : public CtrProviderPlugin
{
public:
  ImgProvider();
  ~ImgProvider();
  void instanceToFactory();
};


class ImgProviderInterface : public CtrPluginIface
{
    Q_OBJECT
    Q_INTERFACES(CtrPluginIface)
    Q_PLUGIN_METADATA(IID "ru.vega.contour.CtrPluginIface")
public:
    ~ImgProviderInterface() {}
    virtual CtrPlugin* createPlugin()
    {
        return new ImgProvider();
    }

    virtual TYPE_PLUGIN getPluginType() const {return TYPE_PROVIDER;}

};


#endif
