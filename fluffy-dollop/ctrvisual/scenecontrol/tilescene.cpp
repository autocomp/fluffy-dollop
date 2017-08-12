#include "tilescene.h"
#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QTime>

using namespace visualize_system;

TileScene::TileScene(bool isGeo, QSize sceneSize, QObject *parent)
    : QGraphicsScene(parent)
    , isActive(false)
    , mouseMoveEnabled(false)
{
    setBackgroundBrush(QBrush(Qt::white));
    QRect r(0,0,sceneSize.width(),sceneSize.height());

    if(isGeo)
    {
        QGraphicsRectItem* pRItem = new QGraphicsRectItem(r);
        addItem(pRItem);
        pRItem->setBrush(Qt::lightGray);
        setSceneRect(r);
    }
    else
    {
        setSceneRect(-10000,-10000,20000,20000);
    }
}

void TileScene::setSceneActive(bool on_off)
{
    isActive = on_off;
}

void TileScene::setMouseMoveEnabled(bool on_off)
{
    mouseMoveEnabled = on_off;
}

void TileScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    if(isActive)
        QGraphicsScene::mouseDoubleClickEvent(mouseEvent);
}

void TileScene::mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    if(isActive || mouseMoveEnabled)
        QGraphicsScene::mouseMoveEvent(mouseEvent);
}

void TileScene::mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    if(isActive)
        QGraphicsScene::mousePressEvent(mouseEvent);
}

void TileScene::mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent)
{
    if(isActive)
        QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

//void TileScene::render(QPixmap& pixmap, const QRectF rect)
//{
//    QTime t;
//    t.start();
//    QPainter painter(&pixmap);
//    QRectF pixmapRect(0, 0, pixmap.size().width(), pixmap.size().height() );
//    QGraphicsScene::render(&painter, pixmapRect, rect);
//    qDebug() << "elapsed" << t.elapsed();
//}
