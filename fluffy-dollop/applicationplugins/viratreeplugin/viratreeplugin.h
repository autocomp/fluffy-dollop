#ifndef VIARTREEPLUGIN_H
#define VIARTREEPLUGIN_H
#include <ctrcore/plugin/ctrappvisualizerplugin.h>
#include <ctrcore/plugin/embifacenotifier.h>

class QStackedWidget;

class ViraTreePlugin: public CtrAppVisualizerPlugin
{
    Q_OBJECT
public:
    ViraTreePlugin();
    ~ViraTreePlugin();
    virtual QList<InitPluginData> getInitPluginData();
    virtual bool isChecked(const QString & buttonName);
    virtual void checked(const QString & buttonName, bool on_off);
    virtual void init(uint visualizerId, quint64 visualizerWindowId);

private slots:
    void slotAddEntity();
    void slotDeleteEntity();

private:
    QWidget * _wdg = nullptr;
};


class ViraTreePluginInterface : public CtrPluginIface
{
    Q_OBJECT
    Q_INTERFACES(CtrPluginIface)
    Q_PLUGIN_METADATA(IID "ru.vega.contour.CtrPluginIface")
public:
    ViraTreePluginInterface() {}
    virtual ~ViraTreePluginInterface() {}
    virtual TYPE_PLUGIN getPluginType() const
    {
        return TYPE_APP_VISUALIZER;
    }
    CtrPlugin * createPlugin()
    {
        return new ViraTreePlugin();
    }
};

#endif
