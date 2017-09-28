#include "transformingitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QImage>
#include <QSvgRenderer>
#include <QMessageBox>
#include <QDebug>

using namespace transform_state;

TransformingItem::TransformingItem(TransformingState &controller)
    : _controller(controller)
{
}

TransformingItem::~TransformingItem()
{
}

//-------------------------------------------------------


PolygonTransformingItem::PolygonTransformingItem(TransformingState &controller)
    : TransformingItem(controller)
{
    QColor color(Qt::green);
    QPen pen(color);
    pen.setCosmetic(true);
    pen.setWidth(2);
    setPen(pen);

    color.setAlpha(50);
    setBrush(QBrush(color));
}

TransformingItem::TransformingItemType PolygonTransformingItem::getTransformingItemType() const
{
    return PolygonItem;
}

QGraphicsItem *PolygonTransformingItem::castToGraphicsItem()
{
    return dynamic_cast<QGraphicsItem*>(this);
}

void PolygonTransformingItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    switch(_controller.mode())
    {
    case StateMode::TransformImage : {
        if(event->modifiers() & Qt::ControlModifier)
        {
            QGraphicsPolygonItem::setFlags(0);
        }
        else
        {
            QGraphicsPolygonItem::setFlags(QGraphicsItem::ItemIsMovable);
        }
    }break;
    default :
        QGraphicsPolygonItem::setFlags(0);
    }

    QGraphicsPolygonItem::mousePressEvent(event);
}

void PolygonTransformingItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    _controller.pixmapMoved();
    QGraphicsPolygonItem::mouseReleaseEvent(event);
}

QPointF PolygonTransformingItem::mapToScene(const QPointF &pos) const
{
    return QGraphicsPolygonItem::mapToScene(pos);
}

QPointF PolygonTransformingItem::mapFromScene(const QPointF &pos) const
{
    return QGraphicsPolygonItem::mapFromScene(pos);
}

void PolygonTransformingItem::setTransform(const QTransform &matrix)
{
    QGraphicsPolygonItem::setTransform(matrix);
}

void PolygonTransformingItem::setPos(const QPointF &pos)
{
    QGraphicsPolygonItem::setPos(pos);
}

void PolygonTransformingItem::setCursor(const QCursor &cursor)
{
    QGraphicsPolygonItem::setCursor(cursor);
}

void PolygonTransformingItem::setFlags(QGraphicsItem::GraphicsItemFlags flags)
{
    QGraphicsPolygonItem::setFlags(flags);
}

void PolygonTransformingItem::setZValue(qreal z)
{
    QGraphicsPolygonItem::setZValue(z);
}

bool PolygonTransformingItem::contains(const QPointF &pos) const
{
    return QGraphicsPolygonItem::contains(pos);
}

void PolygonTransformingItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
//    painter->save();
//    painter->setPen(Qt::red);
//    painter->setBrush(Qt::NoBrush);
//    painter->drawRect(boundingRect());
//    painter->restore();

    QGraphicsPolygonItem::paint(painter, option, widget);
}

//-------------------------------------------------------

SvgTransformingItem::SvgTransformingItem(const QString &svgFilePath, TransformingState &controller)
    : TransformingItem(controller)
    , QGraphicsSvgItem(svgFilePath)
{
}

SvgTransformingItem::~SvgTransformingItem()
{
    if(_renderer)
        _renderer->deleteLater();
}

TransformingItem::TransformingItemType SvgTransformingItem::getTransformingItemType() const
{
    return SvgItem;
}

QGraphicsItem *SvgTransformingItem::castToGraphicsItem()
{
    return dynamic_cast<QGraphicsItem*>(this);
}

void SvgTransformingItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    switch(_controller.mode())
    {
    case StateMode::TransformImage : {
        if(event->modifiers() & Qt::ShiftModifier)
        {
            QGraphicsSvgItem::setFlags(0);
            _controller.createUserHandler( mapToScene(event->pos()) );
        }
        else if(event->modifiers() & Qt::ControlModifier)
        {
            QGraphicsSvgItem::setFlags(0);
        }
        else
        {
            QGraphicsSvgItem::setFlags(QGraphicsItem::ItemIsMovable);
        }
        /*
        if(event->modifiers() & Qt::ControlModifier)
        {
            QGraphicsSvgItem::setFlags(0);
        }
        else
        {
            QGraphicsSvgItem::setFlags(QGraphicsItem::ItemIsMovable);
        }
        */
    }break;
    default :
        QGraphicsSvgItem::setFlags(0);
    }

    QGraphicsSvgItem::mousePressEvent(event);
}

void SvgTransformingItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    _controller.pixmapMoved();
    QGraphicsSvgItem::mouseReleaseEvent(event);
}

QPointF SvgTransformingItem::mapToScene(const QPointF &pos) const
{
    return QGraphicsSvgItem::mapToScene(pos);
}

QPointF SvgTransformingItem::mapFromScene(const QPointF &pos) const
{
    return QGraphicsSvgItem::mapFromScene(pos);
}

void SvgTransformingItem::setTransform(const QTransform &matrix)
{
    QGraphicsSvgItem::setTransform(matrix);
}

void SvgTransformingItem::setPos(const QPointF &pos)
{
    QGraphicsSvgItem::setPos(pos);
}

void SvgTransformingItem::setCursor(const QCursor &cursor)
{
    QGraphicsSvgItem::setCursor(cursor);
}

void SvgTransformingItem::setFlags(QGraphicsItem::GraphicsItemFlags flags)
{
    QGraphicsSvgItem::setFlags(flags);
}

void SvgTransformingItem::setZValue(qreal z)
{
    QGraphicsSvgItem::setZValue(z);
}

bool SvgTransformingItem::contains(const QPointF &pos) const
{
    return QGraphicsSvgItem::contains(pos);
}

void SvgTransformingItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setClipping(true);
    QRegion region(boundingRect().toRect());
    painter->setClipRegion(region);

    QGraphicsSvgItem::paint(painter, option, widget);
}

void SvgTransformingItem::setRenderer(const QByteArray &contents)
{
    QSvgRenderer * renderer = new QSvgRenderer(contents);
    setSharedRenderer(renderer);
    update();

    if(_renderer)
        _renderer->deleteLater();
}

//-------------------------------------------------------

PixmapTransformingItem::PixmapTransformingItem(TransformingState &controller)
    : TransformingItem(controller)
    , _transparentBrush(QPixmap("://img/transparent_background.png"))
{
    setTransformationMode(Qt::SmoothTransformation);
}

TransformingItem::TransformingItemType PixmapTransformingItem::getTransformingItemType() const
{
    return PixmapItem;
}

QGraphicsItem *PixmapTransformingItem::castToGraphicsItem()
{
    return dynamic_cast<QGraphicsItem*>(this);
}

void PixmapTransformingItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    switch(_controller.mode())
    {
    case StateMode::GetColorOnImage : {
        QImage image(pixmap().toImage().convertToFormat(QImage::Format_ARGB32));
        QColor color = QColor::fromRgb(image.pixel(event->pos().toPoint()));
        if(color.alpha() <= 1)
            QMessageBox::information(0, QString::fromUtf8("Внимание"), QString::fromUtf8("Нельзя выбрать цвет полностью прозрачного пикселя !"));
        else
            _controller.resendColor(color);
        setFlags(0);
    }break;
    case StateMode::TransformImage : {
        if(event->modifiers() & Qt::ShiftModifier)
        {
            setFlags(0);
            _controller.createUserHandler( mapToScene(event->pos()) );
        }
        else if(event->modifiers() & Qt::ControlModifier)
        {
            setFlags(0);
        }
        else
        {
            setFlags(QGraphicsItem::ItemIsMovable);
        }
    }break;
    default :
        setFlags(0);
    }

    QGraphicsPixmapItem::mousePressEvent(event);
}

void PixmapTransformingItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    _controller.pixmapMoved();
    QGraphicsPixmapItem::mouseReleaseEvent(event);
}

void PixmapTransformingItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(_showTransparentBackground)
    {
        painter->save();
        painter->setPen(Qt::NoPen);
        painter->setBrush(_transparentBrush);
        painter->drawRect(boundingRect());
        painter->restore();
    }

    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    QGraphicsPixmapItem::paint(painter, option, widget);
}

QPointF PixmapTransformingItem::mapToScene(const QPointF &pos) const
{
    return QGraphicsPixmapItem::mapToScene(pos);
}

QPointF PixmapTransformingItem::mapFromScene(const QPointF &pos) const
{
    return QGraphicsPixmapItem::mapFromScene(pos);
}

void PixmapTransformingItem::setTransform(const QTransform &matrix)
{
    QGraphicsPixmapItem::setTransform(matrix);
}

void PixmapTransformingItem::setPos(const QPointF &pos)
{
    QGraphicsPixmapItem::setPos(pos);
}

void PixmapTransformingItem::setCursor(const QCursor &cursor)
{
    QGraphicsPixmapItem::setCursor(cursor);
}

void PixmapTransformingItem::setFlags(QGraphicsItem::GraphicsItemFlags flags)
{
    QGraphicsPixmapItem::setFlags(flags);
}

void PixmapTransformingItem::setZValue(qreal z)
{
    QGraphicsPixmapItem::setZValue(z);
}

bool PixmapTransformingItem::contains(const QPointF &pos) const
{
    return QGraphicsPixmapItem::contains(pos);
}

void PixmapTransformingItem::setTransparentBackground(bool on_off)
{
    _showTransparentBackground = on_off;
    update();
}























