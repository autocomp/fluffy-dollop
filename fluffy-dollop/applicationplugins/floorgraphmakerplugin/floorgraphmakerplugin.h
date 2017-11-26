#ifndef FLOORGRAPHMAKERPLUGIN_H
#define FLOORGRAPHMAKERPLUGIN_H
#include <ctrcore/plugin/ctrappvisualizerplugin.h>

namespace floor_graph_maker {
class InstrumentalForm;
}

class FloorGraphMakerPlugin: public CtrAppVisualizerPlugin
{
    Q_OBJECT
public:
    FloorGraphMakerPlugin();
    ~FloorGraphMakerPlugin();
    virtual QList<InitPluginData> getInitPluginData();
    virtual bool isChecked(const QString & buttonName);
    virtual void checked(const QString & buttonName, bool on_off);
    virtual void init(uint visualizerId, quint64 visualizerWindowId);

private slots:
    void slotCheckVisibleState();
    void slotInstrumentalFormClose();

private:
    bool _cheched = false;
    floor_graph_maker::InstrumentalForm * _form = nullptr;
};


class FloorGraphMakerPluginInterface : public CtrPluginIface
{
    Q_OBJECT
    Q_INTERFACES(CtrPluginIface)
    Q_PLUGIN_METADATA(IID "ru.vega.contour.CtrPluginIface")
public:
    FloorGraphMakerPluginInterface() {}
    virtual ~FloorGraphMakerPluginInterface() {}
    virtual TYPE_PLUGIN getPluginType() const
    {
        return TYPE_APP_VISUALIZER;
    }
    CtrPlugin * createPlugin()
    {
        return new FloorGraphMakerPlugin();
    }
};

#endif
