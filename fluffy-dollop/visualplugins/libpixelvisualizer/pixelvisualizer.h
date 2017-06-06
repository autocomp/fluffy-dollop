#ifndef PIXELVISUALIZER_H
#define PIXELVISUALIZER_H

#include <ctrcore/visual/visualizertype.h>
#include <ctrcore/visual/abstractvisualizer.h>
#include <ctrcore/visual/pixelscenedescriptor.h>
#include <ctrvisual/scenecontrol/conturtilemixer.h>
#include <ctrvisual/scenecontrol/scenecontroller.h>
#include <ctrvisual/scenecontrol/viewercontroller.h>
#include <ctrvisual/scenecontrol/scene2dwidget.h>
#include <ctrcore/visual/abstractscenewidget.h>

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

class PixelVisualizer : public AbstractVisualizer
{
    Q_OBJECT
public:
    PixelVisualizer();
    virtual ~PixelVisualizer();

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
    virtual sw::AbstractSceneWidget* getAbstractSceneWidget();
    virtual SceneDescriptor * getSceneDescriptor();

    virtual QWidget* widget();
    virtual QGraphicsView* graphicsView();
    virtual QWidget* minimapWidget();
    virtual QPixmap snapShort();
    virtual QStackedWidget* stackedWidget();

    virtual void addToolButton(QToolButton* tb);
    virtual void addActionToObjectMenu(QList<QAction*> actions, QString groupName, uint groupWeight, QString subMenuName);
    virtual void addActionToRasterMenu(QList<QAction*> actions, QString groupName, uint groupWeight, QString subMenuName);

    void loadPluginCategory(QString pluginType);
    void readSettings();
    void saveSettings(QPair<ctrplugin::InitPluginData, CtrAppVisualizerPlugin*> &_struct_plugin);
    void saveSetting(const QString &key, const QString &value);

    virtual uint32_t getVisualizerCursorId();

    virtual void repaintVisualizer();

signals:

private:
    uint VISUALIZER_ID;
    quint64 VISUALIZER_WINDOW_ID;

    visualize_system::ViewInterface * _viewInterface;
    visualize_system::DataInterface * _dataInterface;
    visualize_system::VectorDataInterface * _vectorDataInterface;
    visualize_system::StateInterface * _stateInterface;
    visualize_system::SelectionModelInterface * _selectionModelInterface;

    SceneDescriptor *m_sceneDescriptor;
    visualize_system::ConturTileMixer *m_tileMixer;
    SceneController *m_controller;
    ViewerController* m_viewerController;
    sw::Scene2DWidget *m_scene2dWidget;
    VisualizerControlPanel *vcp = nullptr;
    QHash<QAction*, QPair<ctrplugin::InitPluginData, CtrAppVisualizerPlugin*> > m_ActionsToPlugins;

private slots:
    void initPixelScene(QSizeF rasterSceneSize, double baseZlevel, int frameId, int empty, QString pixelVizFilePath);
    void createPlugins();
    void slotPluginActivated(bool checked);
    void slotPluginChecked(const QString &buttonName,bool checked);
    void sceneWidgetSizeChanged(int newW, int newH);
    void panelVisibleChanged(bool tf);
};
}

class AbstractPixelVisualizerInterface : public CtrPluginIface
{
    Q_OBJECT
    Q_INTERFACES(CtrPluginIface)
    Q_PLUGIN_METADATA(IID "ru.vega.contour.CtrPluginIface")
public:
    AbstractPixelVisualizerInterface() {}
    virtual ~AbstractPixelVisualizerInterface() {}

    virtual TYPE_PLUGIN getPluginType() const { return TYPE_VISUALIZER; }

    virtual CtrPlugin* createPlugin() {return new visualize_system::PixelVisualizer();}
};

#endif










