#ifndef TILESCENE_H
#define TILESCENE_H

#include <QGraphicsScene>

namespace visualize_system {

class TileScene : public QGraphicsScene
{
    bool isActive, mouseMoveEnabled;

public:
    TileScene(bool isGeo, QSize sceneSize, QObject* parent = 0);
    void setSceneActive(bool on_off);
    void setMouseMoveEnabled(bool on_off);
//    void render(QPixmap& pixmap, const QRectF rect);

protected:
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent * mouseEvent);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent * mouseEvent);
};

}

#endif // SCENE_H
