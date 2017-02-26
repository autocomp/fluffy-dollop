#ifndef RASTERLAYER_H
#define RASTERLAYER_H

#include <QGraphicsPolygonItem>
#include <QGraphicsPixmapItem>
#include <QPainterPath>
#include <QPen>
#include <ctrcore/provider/rasterdataprovider.h>
#include <ctrcore/provider/t_image.h>
#include <QSharedPointer>

namespace visualize_system {

const int RASTER_LAYER_ITEM(QGraphicsItem::UserType + 10);

enum class RasterLayerItemType
{
    PointLayer,
    PolygonLayer
};

struct RasterLayerInitData
{
    RasterLayerInitData() : partOfSide(10)
    {
        defaultPen.setColor(Qt::lightGray);
        defaultPen.setWidth(2);
        defaultPen.setStyle(Qt::SolidLine);
        defaultPen.setCosmetic(true);

        selectPen.setColor(Qt::green);
        selectPen.setWidth(2);
        selectPen.setStyle(Qt::SolidLine);
        selectPen.setCosmetic(true);

        activePen.setColor(Qt::yellow);
        activePen.setWidth(2);
        activePen.setStyle(Qt::SolidLine);
        activePen.setCosmetic(true);

        QColor col(Qt::yellow);
        col.setAlpha(125);
        markBrush = QBrush(col);
    }

    QPen defaultPen, selectPen, activePen;
    QBrush markBrush;
    uint partOfSide;
};

class RasterLayer;

class RasterLayerItem
{
public:
    RasterLayerItem(RasterLayer & rasterLayer);
    virtual ~RasterLayerItem() {}
    RasterLayer * getRasterLayer();
    virtual RasterLayerItemType getRasterLayerItemType() const = 0;
protected:
    RasterLayer & _rasterLayer;
};

class RasterLayerPolygonItem : public RasterLayerItem, public QGraphicsPolygonItem
{
public:
    RasterLayerPolygonItem(RasterLayer & rasterLayer,  QPolygonF polygon);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    int type() const;
    RasterLayerItemType getRasterLayerItemType() const;

protected:

};

class DirectionPixmapItem;

class RasterLayerPixmapItem : public QObject, public RasterLayerItem, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    RasterLayerPixmapItem(RasterLayer & rasterLayer, int orientation, double direction);
    virtual ~RasterLayerPixmapItem();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    int type() const;
    RasterLayerItemType getRasterLayerItemType() const;
    void setVisiblePreviw(bool on_off);

protected slots:
    void updateImg(int taskId);
    void slotDeleteLater();

protected:
    QPixmap pm_w, pm_g, pm_y;
    int _orientation;
    double _direction;
    QBrush _markedBrush;
    QGraphicsPixmapItem * _preview;
    int _taskId;
    image_types::TImage _timg;
//    QSharedPointer<data_system::RasterDataProvider> _currProvider;
    DirectionPixmapItem * _directionPixmapItem = nullptr;
    QGraphicsLineItem * _line = nullptr;
};

class RasterLayer : public QObject
{
    Q_OBJECT
public:
    RasterLayer(uint id, QPolygonF polygon, RasterLayerInitData rasterLayerInitData);
    RasterLayer(uint id, QPointF centerMatchingPoint, double imgDirection, int orientation, RasterLayerInitData rasterLayerInitData);
    ~RasterLayer();
    uint id() const;
    void setRasterPolygon(const QPolygonF &polygon);
    void setDefaultPen(QPen pen);
    void setLayerSelected(bool on_off);
    bool isLayerSelected() {return _isSelected;}
    void setLayerMarked(bool on_off);
    bool isLayerMarked() {return _isMarked;}
    void emit_signalSetActive();
    void emit_signalClicked(bool withControl);
    bool containsPoint(QPointF pos);
    bool intersect(QPolygonF polygon);
    void subtract(QPolygonF polygon);
    QPolygonF getPolygon();
    void setOnTop();

    void setVisible(bool on_off);
    QGraphicsItem * getGraphicsItem();
    bool layerIsSelected() {return _isSelected;}
    bool layerIsMarked() {return _isMarked;}
    bool layerIsActivated() {return _isActivated;}
    const RasterLayerInitData & getRasterLayerInitData() {return _rasterLayerInitData;}
    const QPainterPath & getPainterPath() {return _painterPath;}
    const QList<QPainterPath> & getCornersPath() {return _cornersPath;}

signals:
    void signalSetActive();
    void signalClicked(bool withControl);

private:
    void recalcMarks();
    double lenght(const QPointF & p1, const QPointF & p2) const;

    const uint _id;
    const int Z_LEVEL;
    QPolygonF _polygon;
    QPainterPath _painterPath;
    QList<QPolygonF> _corners;
    QList<QPainterPath> _cornersPath;
    bool _isSelected, _isMarked, _isActivated;
    double _imgDirection;
    RasterLayerInitData _rasterLayerInitData;

    RasterLayerPolygonItem * _rasterLayerPolygonItem = nullptr;
    RasterLayerPixmapItem * _rasterLayerPixmapItem = nullptr;
};

class DirectionPixmapItem : public RasterLayerItem, public QGraphicsPixmapItem
{

public:
    DirectionPixmapItem(RasterLayer & rasterLayer, QGraphicsItem * parent);
    virtual ~DirectionPixmapItem();
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
    int type() const;
    RasterLayerItemType getRasterLayerItemType() const;

protected:
    QPixmap pm_w, pm_g, pm_y;
};

}
#endif

