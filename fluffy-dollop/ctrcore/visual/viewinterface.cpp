#include "viewinterface.h"

using namespace visualize_system;

ViewInterface::ViewInterface(AbstractVisualizer& abstractVisualizer)
    : VisualizerInterface(abstractVisualizer)
{
    connect(&abstractVisualizer, SIGNAL(signalZoomChanged(int)), this, SIGNAL(signalZoomChanged(int)));
    connect(&abstractVisualizer, SIGNAL(signalMinMaxZoomChanged()), this, SIGNAL(signalMinMaxZoomChanged()));
    connect(&abstractVisualizer, SIGNAL(signalRotateChanged(int)), this, SIGNAL(signalRotateChanged(int)));
    connect(&abstractVisualizer, SIGNAL(signalScaleLevelChanged(QString,double,QString,double)), this, SIGNAL(signalScaleLevelChanged(QString,double,QString,double)));
    connect(&abstractVisualizer, SIGNAL(signalMouseMoved(QPointF,QPoint)), this, SIGNAL(signalMouseMoved(QPointF,QPoint)));
}

QRectF ViewInterface::getViewportRect()
{
    return _abstractVisualizer.getViewportRect();
}

void ViewInterface::centerOn(QPointF nativeCoords)
{
    return _abstractVisualizer.centerOn(nativeCoords);
}

void ViewInterface::centerOn(QRectF nativeArea)
{
    return _abstractVisualizer.centerOn(nativeArea);
}

void ViewInterface::setZlevel(int zLevel)
{
    return _abstractVisualizer.setZlevel(zLevel);
}

int ViewInterface::getZlevel()
{
    return _abstractVisualizer.getZlevel();
}
int ViewInterface::getMinZlevel()
{
    return _abstractVisualizer.getMinZlevel();
}

int ViewInterface::getMaxZlevel()
{
    return _abstractVisualizer.getMaxZlevel();
}

void ViewInterface::setRotate(int angle)
{
    _abstractVisualizer.setRotate(angle);
}

int ViewInterface::getRotate()
{
    return _abstractVisualizer.getRotate();
}

void ViewInterface::blockRotate(bool on_off)
{
    _abstractVisualizer.blockRotate(on_off);
}

void ViewInterface::scrollLeft()
{
    return _abstractVisualizer.scrollLeft();
}

void ViewInterface::scrollRight()
{
    return _abstractVisualizer.scrollRight();
}

void ViewInterface::scrollUp()
{
    return _abstractVisualizer.scrollUp();
}

void ViewInterface::scrollDown()
{
    return _abstractVisualizer.scrollDown();
}

void ViewInterface::setVisualizerWindowId(quint64 id)
{
    _abstractVisualizer.setVisualizerWindowId(id);
}

quint64 ViewInterface::getVisualizerWindowId()
{
    return _abstractVisualizer.getVisualizerWindowId();
}

QWidget* ViewInterface::widget()
{
    return _abstractVisualizer.widget();
}

QStackedWidget *ViewInterface::stackedWidget()
{
    return _abstractVisualizer.stackedWidget();
}

bool ViewInterface::addWidgetToSplitterLeftArea(QWidget * wdg)
{
    return _abstractVisualizer.addWidgetToSplitterLeftArea(wdg);
}

bool ViewInterface::addWidgetToStatusBar(QWidget * wdg)
{
    return _abstractVisualizer.addWidgetToStatusBar(wdg);
}

QWidget* ViewInterface::minimapWidget()
{
    return _abstractVisualizer.minimapWidget();
}

QPixmap ViewInterface::snapShort()
{
    return _abstractVisualizer.snapShort();
}

sw::AbstractSceneWidget *ViewInterface::getAbstractSceneWidget()
{
    return _abstractVisualizer.getAbstractSceneWidget();
}

void ViewInterface::addToolButton(QToolButton* tb)
{
    _abstractVisualizer.addToolButton(tb);
}

void ViewInterface::addActionToObjectMenu(QList<QAction *> actions, QString groupName, uint groupWeight, QString subMenuName)
{
    _abstractVisualizer.addActionToObjectMenu(actions, groupName, groupWeight, subMenuName);
}

void ViewInterface::addActionToRasterMenu(QList<QAction *> actions, QString groupName, uint groupWeight, QString subMenuName)
{
    _abstractVisualizer.addActionToRasterMenu(actions, groupName, groupWeight, subMenuName);
}
