#include "minimapviewer.h"
#include <QGraphicsPolygonItem>
#include <QMouseEvent>
#include <QDebug>

using namespace visualize_system;

MiniMapViewer::MiniMapViewer(QGraphicsScene* scene, QWidget* parent)
    : QGraphicsView(parent)
    , isScroll(false)
{
    setScene(scene);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //setDragMode(QGraphicsView::ScrollHandDrag);
    //setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    _polygon = new QGraphicsPolygonItem;
    scene->addItem(_polygon);
    _polygon->setZValue(100);
    QPen pen(QColor(246,169,246));
    pen.setCosmetic(true);
    pen.setWidth(2);
    _polygon->setPen(pen);

    setFixedSize(256,256);
    hide();

    setMouseTracking(true);
    viewport()->setMouseTracking(true);
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void MiniMapViewer::viewCenterOn(QPointF p)
{
    if(isScroll == false)
        centerOn(p);
}

void MiniMapViewer::setPolygon(const QPolygonF& polygon)
{
    _polygon->setPolygon(polygon);
}

void MiniMapViewer::mousePressEvent(QMouseEvent *e)
{
    isScroll = true;
    QGraphicsView::mousePressEvent(e);
}

void MiniMapViewer::mouseReleaseEvent(QMouseEvent *e)
{
    isScroll = false;
    QGraphicsView::mouseReleaseEvent(e);
}

void MiniMapViewer::mouseMoveEvent(QMouseEvent *e)
{
    QGraphicsView::mouseMoveEvent(e);
    if(isScroll)
    {
        QRect R(0,0,contentsRect().width(), contentsRect().height());
        emit clickOnMinimap(mapToScene(R.center()));
    }
}

void MiniMapViewer::resizeEvent(QResizeEvent* e)
{
}

void MiniMapViewer::wheelEvent(QWheelEvent* e)
{
    emit signalWheelEvent(e);
}
