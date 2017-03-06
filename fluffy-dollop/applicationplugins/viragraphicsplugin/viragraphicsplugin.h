#ifndef VIARGRAPHICSPLUGIN_H
#define VIARGRAPHICSPLUGIN_H
#include <ctrcore/plugin/ctrappvisualizerplugin.h>
#include <ctrcore/plugin/embifacenotifier.h>
#include "viraeditorform.h"
#include "workstate.h"

class QStackedWidget;

class ViraGraphicsPlugin: public CtrAppVisualizerPlugin
{
    Q_OBJECT
public:
    ViraGraphicsPlugin();
    ~ViraGraphicsPlugin();
    virtual QList<InitPluginData> getInitPluginData();
    virtual bool isChecked(const QString & buttonName);
    virtual void checked(const QString & buttonName, bool on_off);
    virtual void init(uint visualizerId, quint64 visualizerWindowId);

private slots:
    void slotEditorClosed();
    void launchWorkState();
    void showFacility(qulonglong id);
    void switchOnMap();

private:
    void startEditor();
    bool _showFirstBaseCover = true;

    ViraEditorForm * _pdfEditorForm;
    QSharedPointer<WorkState> _workState;
    QStackedWidget * _stackedWidget = nullptr;
};


class ViraGraphicsPluginInterface : public CtrPluginIface
{
    Q_OBJECT
    Q_INTERFACES(CtrPluginIface)
    Q_PLUGIN_METADATA(IID "ru.vega.contour.CtrPluginIface")
public:
    ViraGraphicsPluginInterface() {}
    virtual ~ViraGraphicsPluginInterface() {}
    virtual TYPE_PLUGIN getPluginType() const
    {
        return TYPE_APP_VISUALIZER;
    }
    CtrPlugin * createPlugin()
    {
        return new ViraGraphicsPlugin();
    }
};

#endif
