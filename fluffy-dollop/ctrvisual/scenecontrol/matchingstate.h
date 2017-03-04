#ifndef MATCINGSTATE_H
#define MATCINGSTATE_H

#include "rematchingstate.h"
#include <QGraphicsLineItem>

namespace visualize_system {

class MatchingState : public RematchingState
{
    Q_OBJECT

public:
    struct MatchingItem
    {
        MatchingItem() : itemOnRaster(0), itemOnMap(0), lineBetween(0), isSelected(false) {}

        QGraphicsPixmapItem* itemOnRaster;
        QGraphicsPixmapItem* itemOnMap;
        QGraphicsLineItem* lineBetween;
        bool isSelected;

        void clear()
        {
            itemOnRaster = 0;
            itemOnMap = 0;
            lineBetween = 0;
            isSelected = false;
        }
    };

    struct MatchingPoint
    {
        QPointF onRaster;
        QPointF onMap;

        void clear()
        {
            onRaster = QPointF();
            onMap = QPointF();
        }
    };

    MatchingState(uint rasterId, QPolygonF sceneVertex);
    MatchingState(uint rasterId, QPolygonF sceneVertex, QList<MatchingPoint> initPoints);
    ~MatchingState();

    virtual QString stateName();
    virtual void statePushedToStack();
    virtual void statePoppedFromStack();
    virtual void init(QGraphicsScene * scene, QGraphicsView * view, const int * zoom, const double * scale, double frameCoef, uint visualizerId);

    virtual bool mouseMoveEvent(QMouseEvent* e, QPointF scenePos);
    virtual bool mousePressEvent(QMouseEvent* e, QPointF scenePos);
    virtual bool mouseReleaseEvent(QMouseEvent *e, QPointF scenePos);
    virtual bool mouseDoubleClickEvent(QMouseEvent *e, QPointF scenePos);
    virtual bool keyPressEvent(QKeyEvent * e);

    void rasterMatched();
    void deleteSelectedPoint();
    QList<MatchingState::MatchingPoint> getMatchingPoints();

signals:
    void signalSetTransparancy(int);
    void signalMatchingFinished();
    void signalEnabledApplyButton(bool);
    void signalEnabledDeleteButton(bool);

private:
    QList<MatchingItem> _matchingItems;
    MatchingItem _currentMatchingItem;
    QList<MatchingPoint> _initPoints;
    QGraphicsLineItem * _currentLine;
    MatchingPoint _currentMatchingPoint;
    QList<QGraphicsLineItem *> _lines;
    QCursor _checkCursor, _emptyCursor, _setPointCursorOnRaster, _setPointCursorOnMap;

    QGraphicsPixmapItem* createPoint(QPointF pos, bool onRaser);
    void checkApply();
};

}

#endif // TRANSFORMINGSTATE_H

