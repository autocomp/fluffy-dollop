#include "pixmapitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QImage>
#include <QMessageBox>
#include <QDebug>

using namespace pixmap_transform_state;

PixmapItem::PixmapItem(PixmapTransformState &controller)
    : _controller(controller)
    , _transparentBrush(QPixmap("://img/transparent_background.png"))
{
}

void PixmapItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
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
            _controller.pixmapMoved();
        }
    }break;
    default :
        setFlags(0);
    }

    QGraphicsPixmapItem::mousePressEvent(event);
}

void PixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
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

void PixmapItem::setTransparentBackground(bool on_off)
{
    _showTransparentBackground = on_off;
    update();
}























