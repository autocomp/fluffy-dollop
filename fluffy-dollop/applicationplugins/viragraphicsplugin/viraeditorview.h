#ifndef VIRAEDITORVIEW_H
#define VIRAEDITORVIEW_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsLineItem>
#include <QPainter>
#include "types.h"
#include "roomgraphicstem.h"

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
    explicit ViraEditorView();
    ~ViraEditorView();
    void reinit(qulonglong facilityId);
    void centerEditorOn(qulonglong id);
    void selectionItemsChanged(uint64_t prev_id, uint64_t curr_id);
    void selectViraItem(qulonglong id, bool centerOnArea);

signals:
    
public slots:
    void setFloor(qulonglong floorId);
    void zoomIn();
    void zoomOut();
    void zoomReset();
    void slotSelectItem(qulonglong id, bool centerOnArea);
    void slotEditAreaGeometry(QVariant var);
    void slotSetMarkPosition(QVariant var);
    void slotUpdateMark(QVariant var);

protected:
    virtual void wheelEvent(QWheelEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);
    virtual void mouseReleaseEvent(QMouseEvent* e);
    virtual void mouseMoveEvent(QMouseEvent* e);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void mouseDoubleClickEvent(QMouseEvent *event);
    void clearTempItems();

private:
    enum Mode {ScrollMode, EditAreaMode, EditMarkMode};

    Mode _mode = ScrollMode;
    int _zoom = 1;
    int _zoomMin = 1;
    int _zoomMax = 1;
    double _scale = 1;
    bool _recalcZoom = true;
    uint64_t _currFloor_id = 0;
    QList<GraphicsPixmapItem *> _owerViews;
    QMap<qulonglong, QGraphicsItem *> _itemsOnFloor;
    uint64_t _editObjectGeometry = 0;
    QPolygonF _editObjectExtend;

    QList<QGraphicsLineItem *> _lines;
};

#endif // PDFEDITORVIEW_H
