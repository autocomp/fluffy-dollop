#ifndef ABSTRACTVISUALIZER_H
#define ABSTRACTVISUALIZER_H

#include "visualizertype.h"
#include "properties.h"
#include <QObject>
#include <QPointF>
#include <QRectF>
#include <QMap>
#include <QPixmap>
#include <QToolButton>
#include <QtPlugin>
#include <QStackedWidget>
#include <ctrcore/plugin/ctrplugin.h>

#include <ctrcore/provider/rasterdataprovider.h>

namespace sw
{
class AbstractSceneWidget;
}

class QGraphicsView;

namespace visualize_system
{

class ViewInterface;
class DataInterface;
class VectorDataInterface;
class StateInterface;
class SelectionModelInterface;
class VisualizerStateObject;

class AbstractVisualizer : public CtrPlugin
{
    Q_OBJECT

public:
    virtual ~AbstractVisualizer() {}

    virtual void init(uint visualizerId) = 0;

    virtual TYPE_PLUGIN getPluginType() const { return TYPE_VISUALIZER;}


    virtual visualize_system::ViewInterface * getViewInterface() = 0;
    virtual visualize_system::DataInterface * getDataInterface() = 0;
    virtual visualize_system::VectorDataInterface * getVectorInterface() = 0;
    virtual visualize_system::StateInterface * getStateInterface() = 0;
    virtual visualize_system::SelectionModelInterface * getSelectionModelInterface() = 0;

    // возвращаюстя урлы фактически добавленных или удаленных провайдеров
    virtual QList<uint> addProviders(const QList<uint> & providers) = 0;
    virtual QList<uint> addBaseProviders(const QList<uint> & providers) = 0;
    virtual QList<uint> removeProviders(const QList<uint> & providers) = 0;
    virtual QList<uint> getProviders(ProviderType providerType = ProviderType::BaseCoverAndImages) = 0;

    // установка объекта-состояния в визуализатор.
    virtual bool setVisualizerState(QSharedPointer<visualize_system::VisualizerStateObject> state) = 0;
    virtual QString getCurrentState() = 0;

    // под нативными подразумеваются родные координаты визуализатора, например для двумерного и трехмерного - WGS84, для пиксельного и видео - координаты матрицы изображения.
    virtual QRectF getViewportRect() = 0;
    virtual void centerOn(QPointF nativeCoords) = 0;
    virtual void centerOn(QRectF nativeArea) = 0;

    virtual void setZlevel(int zLevel) = 0;
    virtual int getZlevel() = 0;
    virtual int getMinZlevel() = 0;
    virtual int getMaxZlevel() = 0;
    virtual void setRotate(int angle) = 0;
    virtual int getRotate() = 0;
    virtual void blockRotate(bool on_off) = 0;
    virtual void scrollLeft() = 0;
    virtual void scrollRight() = 0;
    virtual void scrollUp() = 0;
    virtual void scrollDown() = 0;
    virtual void setVisualizerWindowId(quint64 id) = 0;
    virtual quint64 getVisualizerWindowId() = 0;
    virtual uint32_t getVisualizerCursorId() = 0;
    virtual sw::AbstractSceneWidget* getAbstractSceneWidget() = 0;

    virtual QWidget* widget() = 0;
    virtual QGraphicsView* graphicsView()= 0;
    virtual QStackedWidget* stackedWidget() = 0;
    virtual QWidget* minimapWidget() = 0;
    virtual QPixmap snapShort() = 0;
    virtual bool addWidgetToSplitterLeftArea(QWidget * wdg) {return false;}
    virtual bool addWidgetToStatusBar(QWidget * wdg) {return false;}

    virtual uint getVisualizerId() = 0;
    virtual visualize_system::VisualizerType getVisualizerType() = 0;
    virtual void addToolButton(QToolButton* tb) = 0;
    virtual void addActionToObjectMenu(QList<QAction*> actions, QString groupName, uint groupWeight, QString subMenuName) = 0;
    virtual void addActionToRasterMenu(QList<QAction*> actions, QString groupName, uint groupWeight, QString subMenuName) = 0;

signals:
    void signalStateChanged();

    void signalZoomChanged(int zLevel);
    void signalMinMaxZoomChanged();
    void signalRotateChanged(int angle);
    void signalScaleLevelChanged(QString textTop, double valueTop, QString textBottom, double valueBottom);

    void signalMouseMoved(QPointF wgsCoords, QPoint matrixCoords);
    void signalMoveParallelCursor(uint visualizerId, QPointF wgsCoords);

};

}

#endif










