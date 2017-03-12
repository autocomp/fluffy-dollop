#ifndef VIARINFOPLUGIN_H
#define VIARINFOPLUGIN_H
#include <ctrcore/plugin/ctrappvisualizerplugin.h>
#include <ctrcore/plugin/embifacenotifier.h>

class ViraStatusBar;

class ViraInfoPlugin: public CtrAppVisualizerPlugin
{
    Q_OBJECT
public:
    ViraInfoPlugin();
    ~ViraInfoPlugin();
    virtual QList<InitPluginData> getInitPluginData();
    virtual bool isChecked(const QString & buttonName);
    virtual void checked(const QString & buttonName, bool on_off);
    virtual void init(uint visualizerId, quint64 visualizerWindowId);

private slots:

private:
    ViraStatusBar * _viraStatusBar;

};


class ViraInfoPluginInterface : public CtrPluginIface
{
    Q_OBJECT
    Q_INTERFACES(CtrPluginIface)
    Q_PLUGIN_METADATA(IID "ru.vega.contour.CtrPluginIface")
public:
    ViraInfoPluginInterface() {}
    virtual ~ViraInfoPluginInterface() {}
    virtual TYPE_PLUGIN getPluginType() const
    {
        return TYPE_APP_VISUALIZER;
    }
    CtrPlugin * createPlugin()
    {
        return new ViraInfoPlugin();
    }
};

#endif
