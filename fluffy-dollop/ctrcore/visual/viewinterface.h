#ifndef VIEWINTERFACE_H
#define VIEWINTERFACE_H

#include "visualizerinterface.h"

namespace sw
{
class AbstractSceneWidget;
}

namespace visualize_system
{
class ViewInterface : public VisualizerInterface
{
    Q_OBJECT
public:
    explicit ViewInterface(AbstractVisualizer& abstractVisualizer);
    
    // под нативными подразумеваются родные координаты визуализатора, например для двумерного и трехмерного - WGS84, для пиксельного и видео - координаты матрицы изображения.
    QRectF getViewportRect();
    void centerOn(QPointF nativeCoords);
    void centerOn(QRectF nativeArea);
    void setZlevel(int zLevel);
    int getZlevel();
    int getMinZlevel();
    int getMaxZlevel();
    void setRotate(int angle);
    int getRotate();
    void blockRotate(bool on_off);
    void scrollLeft();
    void scrollRight();
    void scrollUp();
    void scrollDown();
    // установка\получение идентификатора виджета визуализатора в оконной системе приложения(EmbeddedWindow).
    void setVisualizerWindowId(quint64 id);
    quint64 getVisualizerWindowId();

    QWidget* widget();
    QStackedWidget* stackedWidget();
    void addWidgetToSplitter(QWidget*);
    QWidget* minimapWidget();
    QPixmap snapShort();
    sw::AbstractSceneWidget* getAbstractSceneWidget();

    void addToolButton(QToolButton* tb);
    void addActionToObjectMenu(QList<QAction*> actions, QString groupName, uint groupWeight = 0, QString subMenuName = QString());
    void addActionToRasterMenu(QList<QAction*> actions, QString groupName, uint groupWeight = 0, QString subMenuName = QString());

signals:
    void signalZoomChanged(int zLevel);
    void signalMinMaxZoomChanged();
    void signalRotateChanged(int angle);
    void signalScaleLevelChanged(QString textTop, double valueTop, QString textBottom, double valueBottom);
    void signalMouseMoved(QPointF wgsCoords, QPoint matrixCoords);
    void signalSetWidgetsEnabled(bool on_off);
    void signalInitObjectMenu(QMenu & objectMenu, quint64 objectId);

};
}
#endif // VIEWINTERFACE_H
