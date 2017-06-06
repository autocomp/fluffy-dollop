#ifndef VIARGRAPHICSPLUGIN_H
#define VIARGRAPHICSPLUGIN_H
#include <ctrcore/plugin/ctrappvisualizerplugin.h>
#include <ctrcore/plugin/embifacenotifier.h>
#include "workstate.h"
#include "pixelworkstate.h"
#include "setimagestate.h"
#include <ctrvisual/state/choiceareastate.h>

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
    void launchWorkState();
    void showFacility(qulonglong id);
    void switchOnMap();
    void switchOnEditor();
    void setMarkOnMap(qulonglong markType);
    void setMarkOnPlan(qulonglong markType, QPolygonF area);
    void defectStateChoiced(QPolygonF);
    void fotoCreared(QPointF pos, double direction);
    void markCreatingAbort();
    void foto360Creared(QPolygonF);
    void editAreaGeometry(bool on_off);
    void areaGeometryEdited(QPolygonF);
    void areaGeometryEditingAbort();
    void currentItemsChanged(uint64_t,uint64_t);

private:
    bool _showFirstBaseCover = true;
    uint _pixelVisualizerId = 0;

    QSharedPointer<WorkState> _workState;
    QSharedPointer<PixelWorkState> _pixelWorkState;
    QSharedPointer<SetImageState> _setImageState;
    QSharedPointer<ChoiceAreaState> _choiceAreaState;
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
