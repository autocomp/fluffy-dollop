#ifndef VIRAEDITORVIEW_H
#define VIRAEDITORVIEW_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsLineItem>
#include <QPainter>
#include "types.h"
#include "roomgraphicstem.h"

class AreaGraphicsItem;

class GraphicsPixmapItem : public QGraphicsPixmapItem
{
public:
    GraphicsPixmapItem(const QPixmap &pixmap)
        : QGraphicsPixmapItem(pixmap)
    {}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR)
    {
        //painter->setRenderHint(QPainter::SmoothPixmapTransform);
        QGraphicsPixmapItem::paint(painter, option, widget);
    }
};

class ViraEditorView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ViraEditorView(); // const pdf_editor::Facility& facility, const QString& path);
    ~ViraEditorView();
    void reinit(qulonglong facilityId); //const QString &path = QString());
    void centerEditorOn(qulonglong id);
    void selectionItemsChanged(uint64_t prev_id, uint64_t curr_id);

signals:
    
public slots:
    void setFloor(qulonglong floorId);
    void zoomIn();
    void zoomOut();
    void zoomReset();

protected:
    virtual void wheelEvent(QWheelEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void keyPressEvent(QKeyEvent *event);

private:
//    void syncItems();

    int _zoom = 1;
    int _zoomMin = 1;
    int _zoomMax = 1;
    double _scale = 1;
    bool _recalcZoom = true;
    QList<GraphicsPixmapItem *> _owerViews;
    QMap<qulonglong, AreaGraphicsItem *> _rooms;

//    QList<QGraphicsLineItem *> _lines;
//    QGraphicsLineItem * _currentLine = nullptr;
};

#endif // PDFEDITORVIEW_H
