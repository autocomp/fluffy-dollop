#ifndef PDFIMPORTPLUGIN_H
#define PDFIMPORTPLUGIN_H
#include <ctrcore/plugin/ctrappvisualizerplugin.h>
#include <ctrcore/plugin/embifacenotifier.h>
#include "pdfeditorform.h"

class PdfImportPlugin: public CtrAppVisualizerPlugin
{
    Q_OBJECT
public:
    PdfImportPlugin();
    ~PdfImportPlugin();
    virtual QList<InitPluginData> getInitPluginData();
    virtual bool isChecked(const QString & buttonName);
    virtual void checked(const QString & buttonName, bool on_off);
    virtual void init(uint visualizerId, quint64 visualizerWindowId);

private slots:
    void slotEditorClosed();

private:
    void loadPDF();

    bool _isChecked;
    EmbIFaceNotifier* _iface;
    PdfEditorForm * _pdfEditorForm;
};


class PdfImportPluginInterface : public CtrPluginIface
{
    Q_OBJECT
    Q_INTERFACES(CtrPluginIface)

public:
    PdfImportPluginInterface() {}
    virtual ~PdfImportPluginInterface() {}
    virtual TYPE_PLUGIN getPluginType() const
    {
        return TYPE_APP_VISUALIZER;
    }
    CtrPlugin * createPlugin()
    {
        return new PdfImportPlugin();
    }
};

#endif
