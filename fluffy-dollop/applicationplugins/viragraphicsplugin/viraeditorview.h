#ifndef VIRAEDITORVIEW_H
#define VIRAEDITORVIEW_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsLineItem>
#include <QComboBox>
#include <QPainter>
#include "types.h"
#include "roomgraphicstem.h"
#include <QToolButton>
#include <QLabel>

class LayersMenu;

class GraphicsPixmapItem : public QGraphicsPixmapItem
{
public:
    GraphicsPixmapItem(const QPixmap &pixmap)
        : QGraphicsPixmapItem(pixmap)
    {}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR)
    {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
        painter->setRenderHint(QPainter::SmoothPixmapTransform, true);

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
    void selectViraItem(qulonglong id, bool centerOnEntity);

signals:
    
public slots:
    void setFloor(qulonglong floorId);
    void zoomIn();
    void zoomOut();
    void zoomReset();
    void slotSelectItem(qulonglong id, bool centerOnEntity);
    void slotEditAreaGeometry(QVariant var);
    void slotSetMarkPosition(QVariant var);
    void slotObjectChanged(uint64_t id);
    void slotFacilityUp();
    void slotFacilityDown();
    void slotRastersVisibleChanged();
    void slotGetNeedVisibleRasters(bool & base, bool & axis, bool & sizes);

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
    //QList<GraphicsPixmapItem *> _owerViews;
    GraphicsPixmapItem * _baseRasterItem = nullptr;
    GraphicsPixmapItem * _axisRasterItem = nullptr;
    GraphicsPixmapItem * _sizesRasterItem = nullptr;

    QMap<qulonglong, QGraphicsItem *> _itemsOnFloor;
    uint64_t _editObjectGeometry = 0;
    QPolygonF _editObjectExtend;
    QToolButton* _upButton, *_downButton;
    QLabel * _currentFacility;
    LayersMenu * _layersMenu;
    QMap<uint64_t, QString> _floorsMap;

    QList<QGraphicsLineItem *> _lines;
};

class LayersMenu : public QLabel
{
    Q_OBJECT
public:
    LayersMenu();
signals:
    void getNeedVisibleRasters(bool & base, bool & axis, bool & sizes);
    void rastersVisibleChanged();
protected:
    virtual void mousePressEvent(QMouseEvent* e);
};

#endif // PDFEDITORVIEW_H
