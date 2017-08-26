#ifndef VIARTREEPLUGIN_H
#define VIARTREEPLUGIN_H
#include <ctrcore/plugin/ctrappvisualizerplugin.h>

class PixelVisButtonsPlugin: public CtrAppVisualizerPlugin
{
    Q_OBJECT
public:
    PixelVisButtonsPlugin();
    ~PixelVisButtonsPlugin();
    virtual QList<InitPluginData> getInitPluginData();
    virtual bool isChecked(const QString & buttonName);
    virtual void checked(const QString & buttonName, bool on_off);
    virtual void init(uint visualizerId, quint64 visualizerWindowId);

private slots:

private:
    bool scaleWidgetButtonCheched = false;
    bool gridButtonCheched = false;

};


class PixelVisButtonsPluginInterface : public CtrPluginIface
{
    Q_OBJECT
    Q_INTERFACES(CtrPluginIface)
    Q_PLUGIN_METADATA(IID "ru.vega.contour.CtrPluginIface")
public:
    PixelVisButtonsPluginInterface() {}
    virtual ~PixelVisButtonsPluginInterface() {}
    virtual TYPE_PLUGIN getPluginType() const
    {
        return TYPE_APP_VISUALIZER;
    }
    CtrPlugin * createPlugin()
    {
        return new PixelVisButtonsPlugin();
    }
};

#endif
