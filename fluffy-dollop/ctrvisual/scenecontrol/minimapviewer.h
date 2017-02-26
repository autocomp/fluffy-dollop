#ifndef MINIMAPVIEWER_H
#define MINIMAPVIEWER_H

#include <QGraphicsView>

namespace visualize_system {

class MiniMapViewer : public QGraphicsView
{
    Q_OBJECT

public:
    MiniMapViewer(QGraphicsScene* scene, QWidget* parent = 0);
    void viewCenterOn(QPointF p);
    void setPolygon(const QPolygonF& polygon);

signals:
    void clickOnMinimap(QPointF);
    void signalWheelEvent(QWheelEvent* e);

protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void mouseMoveEvent(QMouseEvent *event);
    virtual void resizeEvent(QResizeEvent* e);
    virtual void wheelEvent(QWheelEvent* e);

private:
    QGraphicsPolygonItem * _polygon;
    bool isScroll;
};

}

#endif // MINIMAPVIEWER_H
