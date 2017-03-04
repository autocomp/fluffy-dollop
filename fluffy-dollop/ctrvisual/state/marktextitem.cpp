#include "marktextitem.h"
#include <QPainter>
#include <QFontMetrics>
#include <QColor>
#include <QDebug>

MarkTextItem::MarkTextItem(QGraphicsItem * _parent, const QBrush &brush, const QPen &pen)
    : QGraphicsSimpleTextItem(_parent)
    , _brush(brush)
    , _pen(pen)
    , _dx(0)
    , _dy(0)
    , _draw(true)
    , _transparency(70)
{
}

bool MarkTextItem::showHoverEnterOnly(bool on_off, uint transparency)
{
    setAcceptHoverEvents(on_off);
    _draw = ! on_off;
    if(on_off)
    {
        _transparency = transparency;
        if(transparency < 0 || transparency > 255)
            _transparency = 70;
    }
    update();
}

void MarkTextItem::translate(qreal dx, qreal dy)
{
//    QGraphicsSimpleTextItem::translate(-_dx, -_dy);
//    QGraphicsSimpleTextItem::translate(dx, dy);
//    _dx = dx;
//    _dy = dy;
}

void MarkTextItem::setText(const QString &text)
{
    QFont font;
    QFontMetrics fontMetrics(font);
    _size = QSize(fontMetrics.width(text)-1, fontMetrics.height()-1);

    QGraphicsSimpleTextItem::setText(text);
}

void MarkTextItem::setText(const QStringList &text)
{
    _size = QSize(0,0);
    QString resultText;
    for(int i(0); i < text.length(); ++i)
    {
        QString str = text[i];
        QFont font;
        QFontMetrics fontMetrics(font);
        int strW = fontMetrics.width(str)-1;
        int strH = fontMetrics.height()-1;
        _size = QSize( ( strW > _size.width() ? strW : _size.width() ), _size.height() + strH);
        resultText.append(str);
        if(i < text.length()-1)
            resultText.append('\n');
    }
    resultText.remove(resultText.length()-1, 1);

    QGraphicsSimpleTextItem::setText(resultText);
}

QSize MarkTextItem::getTextSize()
{
    return _size;
}

void MarkTextItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    int w = _size.width();
    int h = _size.height() + 1;
    painter->save();
    painter->setPen(_pen);

    if(_draw)
    {
        painter->setBrush(_brush);
    }
    else
    {
        QBrush br(_brush);
        QColor colBr(_brush.color());
        colBr.setRgba(qRgba(colBr.red(), colBr.green(), colBr.blue(), _transparency));
        br.setColor(colBr);
        painter->setBrush(br);
    }

    painter->drawRect(0, 0, w, h);

    //        int r = h / 3 * 2; // радиус закругления
    //        // QBrush
    //        painter->setPen(QPen(QColor(Qt::transparent)));
    //        painter->drawChord(-r / 2 + 1, 0, r, h, 90 * 16, 180 * 16);
    //        painter->drawChord(w - r / 2 - 1, 0, r, h, 90 * 16, -180 * 16);
    //        // QPen
    //        painter->setPen(_pen);
    //        painter->setBrush(QBrush(QColor(Qt::transparent)));
    //        painter->drawArc(-r / 2 + 1, 0, r, h, 90 * 16, 180 * 16);
    //        painter->drawArc(w - r / 2 - 1, 0, r, h, 90 * 16, -180 * 16);

    painter->restore();

    QGraphicsSimpleTextItem::paint(painter, option, widget);
}

void MarkTextItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    _draw = true;
    if(parentItem())
    {
        double Z = parentItem()->zValue();
        parentItem()->setZValue(Z + 1000);
    }
    else
    {
        double Z = zValue();
        setZValue(Z + 1000);
    }
    update();
}

void MarkTextItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    _draw = false;
    if(parentItem())
    {
        double Z = parentItem()->zValue();
        parentItem()->setZValue(Z - 1000);
    }
    else
    {
        double Z = zValue();
        setZValue(Z - 1000);
    }
    update();
}






