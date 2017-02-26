#ifndef CTRAPPVISUALIZERPLUGIN_H
#define CTRAPPVISUALIZERPLUGIN_H

#include "ctrappplugin.h"

class CtrAppVisualizerPlugin : public CtrAppPlugin
{
    Q_OBJECT
public:
    virtual ~CtrAppVisualizerPlugin();

    virtual void init(uint visualizerId, quint64 visualizerWindowId);
    virtual QList<InitPluginData> getInitPluginData() = 0;
    virtual bool isChecked(const QString & buttonName) = 0;
    virtual void checked(const QString & buttonName, bool on_off) = 0;

    TYPE_PLUGIN getPluginType() const { return TYPE_APP_VISUALIZER; }

signals:
    void setChecked(const QString & buttonName, bool on_off);
    void setEnabled(const QString & buttonName, bool on_off);

protected:
    CtrAppVisualizerPlugin();

    uint getVisualizerId() const;
    quint64 getVisualizerWindowId() const;

private:
    quint64 _visualizerWindowId;
    uint _visualizerId;

};

#endif // CTRAPPVISUALIZERPLUGIN_H
