#ifndef Visualizer2d_H
#define Visualizer2d_H

#include <ctrcore/visual/visualizertype.h>
#include <ctrcore/visual/abstractvisualizer.h>
#include <ctrcore/visual/geoscenedescriptor.h>
#include <ctrvisual/scenecontrol/conturtilemixer.h>
#include <ctrvisual/scenecontrol/scenecontroller.h>
#include <ctrvisual/scenecontrol/viewercontroller.h>
#include <ctrvisual/scenecontrol/scene2dwidget.h>
#include <ctrcore/visual/abstractscenewidget.h>
//#include <ctrvectorrender/render/vectorrender.h>
#include <ctrcore/bus/imessagenotifier.h>

#include <QDebug>
#include <QtPlugin>

#include <ctrcore/provider/rasterdataprovider.h>


namespace visualize_system
{

class ViewInterface;
class DataInterface;
class VectorDataInterface;
class StateInterface;
class SelectionModelInterface;
class VisualizerControlPanel;

class Visualizer2d : public AbstractVisualizer
{
    Q_OBJECT  
public:
    Visualizer2d();
    virtual ~Visualizer2d();

    virtual void init(uint visualizerId);
    virtual uint getVisualizerId();
    virtual visualize_system::VisualizerType getVisualizerType();

    virtual visualize_system::ViewInterface * getViewInterface();
    virtual visualize_system::DataInterface * getDataInterface();
    virtual visualize_system::VectorDataInterface * getVectorInterface();
    virtual visualize_system::StateInterface * getStateInterface();
    virtual visualize_system::SelectionModelInterface * getSelectionModelInterface();

    virtual QList<uint> addProviders(const QList<uint> & providers);
    virtual QList<uint> addBaseProviders(const QList<uint> & providers);
    virtual QList<uint> removeProviders(const QList<uint> & providers);
    virtual QList<uint> getProviders(ProviderType providerType = ProviderType::BaseCoverAndImages);

    virtual bool setVisualizerState(QSharedPointer<visualize_system::VisualizerStateObject> state);
    virtual QString getCurrentState();
    virtual QRectF getViewportRect();
    virtual void centerOn(QPointF nativeCoords);
    virtual void centerOn(QRectF nativeArea);
    virtual void setZlevel(int zLevel);
    virtual int getZlevel();
    virtual int getMinZlevel();
    virtual int getMaxZlevel();
    virtual void setRotate(int angle);
    virtual int getRotate();
    virtual void blockRotate(bool on_off);
    virtual void scrollLeft();
    virtual void scrollRight();
    virtual void scrollUp();
    virtual void scrollDown();
    virtual void setVisualizerWindowId(quint64 id);
    virtual quint64 getVisualizerWindowId();
    virtual uint32_t getVisualizerCursorId();
    virtual sw::AbstractSceneWidget* getAbstractSceneWidget();

    virtual QWidget* widget();
    virtual QWidget* minimapWidget();
    virtual QPixmap snapShort();

    virtual void addToolButton(QToolButton* tb);
    virtual void addActionToObjectMenu(QList<QAction*> actions, QString groupName, uint groupWeight, QString subMenuName);
    virtual void addActionToRasterMenu(QList<QAction*> actions, QString groupName, uint groupWeight, QString subMenuName);

    void loadPluginCategory(QString pluginType);
    void readSettings();
    void saveSettings(QPair<ctrplugin::InitPluginData, CtrAppVisualizerPlugin*> &_struct_plugin);

signals:

private:
    struct TmsXmlData
    {
        QString name, srsEpsg, srsProj;
    };

    GlobalMessageNotifier *m_pGlobalNotif;
//    QTimer *m_waitInitContextTimer;
    uint VISUALIZER_ID;
    quint64 VISUALIZER_WINDOW_ID;
    uint32_t VISUALIZER_CURSOR_ID;
    ViewInterface * _viewInterface;
    DataInterface * _dataInterface;
    VectorDataInterface * _vectorDataInterface;
    StateInterface * _stateInterface;
    SelectionModelInterface * _selectionModelInterface;
    SceneDescriptor *m_sceneDescriptor;
    ConturTileMixer *m_tileMixer;
    SceneController *m_controller;
    ViewerController* m_viewerController;
    sw::Scene2DWidget *m_scene2dWidget;
//    VectorRender *m_pVectorRender;
    QPointF m_centerScenePos;
    VisualizerControlPanel *vcp = nullptr;

    QString getDefaultXml();
    void getTmsXmlData(const QByteArray& byteArray, QList<TmsXmlData>& tmsDataList);
    QHash<QAction*, QPair<ctrplugin::InitPluginData, CtrAppVisualizerPlugin*> > m_ActionsToPlugins;

private slots:
    void createPlugins();
    void slotUserViewControlAction(UserAction);
    void slotCenterViewOn();
    void slotPluginActivated(bool checked);
    void slotPluginChecked(const QString &buttonName,bool checked);
    void slot2DCursorCreated(uint32_t id);
    void sceneWidgetSizeChanged(int newW, int newH);
    void panelVisibleChanged(bool tf);
};

}

class AbstractVisualizer2dInterface : public CtrPluginIface
{
    Q_OBJECT
    Q_INTERFACES(CtrPluginIface)
    Q_PLUGIN_METADATA(IID "ru.vega.contour.CtrPluginIface")
public:
    AbstractVisualizer2dInterface() {}
    virtual ~AbstractVisualizer2dInterface() {}

    virtual TYPE_PLUGIN getPluginType() const { return TYPE_VISUALIZER; }

    virtual CtrPlugin* createPlugin() {return new visualize_system::Visualizer2d;}
};


#endif










