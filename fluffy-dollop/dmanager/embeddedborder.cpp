#include "embeddedborder.h"

#include <QPainter>

ew::EmbeddedBorder::EmbeddedBorder(QWidget *parent) : QWidget(parent)
{

}

ew::EmbeddedBorder::EmbeddedBorder(ew::EmbeddedBorder::EmbeddedBorderType ebt, QWidget *parent)
    : QWidget(parent)
    , _borderType(ebt)
    , _borderWidth(5)
{

    setCursors();
    checkPosition(parent->size());

}

void ew::EmbeddedBorder::checkPosition(QSize size)
{

    QWidget *p = static_cast<QWidget*>(parent());
    int w = size.width();
    int h = size.height();

    switch (_borderType) {
        case EBT_LEFT:
            setGeometry(0,
                        _borderWidth,
                        _borderWidth,
                        h-2*_borderWidth);
        break;
        case EBT_LEFT_TOP:
        setGeometry(0,
                    0,
                    _borderWidth,
                    _borderWidth
                    );
        break;
        case EBT_TOP:
        setGeometry(_borderWidth,
                    0,
                    w-2*_borderWidth,
                    _borderWidth
                    );
        break;
        case EBT_RIGHT_TOP:
        setGeometry(
                    w-_borderWidth,
                    0,
                    _borderWidth,
                    _borderWidth);
        break;
        case EBT_RIGHT:
        setGeometry(w-_borderWidth,
                    _borderWidth,
                    _borderWidth,
                    h-_borderWidth*2);
        break;
        case EBT_RIGHT_BOTTOM:
        setGeometry(w-_borderWidth,
                    h-_borderWidth,
                    _borderWidth,
                    _borderWidth);
        break;
        case EBT_BOTTOM:
        setGeometry(_borderWidth,
                    h-_borderWidth,
                    w-2*_borderWidth,
                    _borderWidth);
        break;
        case EBT_LEFT_BOTTOM:

        setGeometry(0,
                    h-_borderWidth,
                    _borderWidth,
                    _borderWidth);
        break;
    default:
        break;
    }
    setCursor(_shape);

}

void ew::EmbeddedBorder::setCursors()
{
    switch (_borderType) {
    case EBT_LEFT:
    case EBT_RIGHT:
        _shape = Qt::SizeHorCursor;
        break;

    case EBT_LEFT_TOP:
    case EBT_RIGHT_BOTTOM:
        _shape = Qt::SizeFDiagCursor;
        break;

    case EBT_TOP:
    case EBT_BOTTOM:
        _shape = Qt::SizeVerCursor;
        break;

    case EBT_RIGHT_TOP:
    case EBT_LEFT_BOTTOM:
        _shape = Qt::SizeBDiagCursor;
        break;

    default:
        break;
    }
}

void ew::EmbeddedBorder::mouseMoveEvent(QMouseEvent *event)
{
    emit signalMouseMove(event);
}

void ew::EmbeddedBorder::mousePressEvent(QMouseEvent *event)
{
    emit signalMousePress(event);
}

void ew::EmbeddedBorder::mouseReleaseEvent(QMouseEvent *event)
{
    emit signalMouseRelease(event);
}

#ifdef DEBUG_MODE_SAW
void ew::EmbeddedBorder::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    p.setBrush(QBrush(Qt::darkGray));
    p.setPen(QPen(Qt::white));
    p.drawRect(QRect(QPoint(0,0),QSize(geometry().width()-1,geometry().height()-1)));
}
#endif
