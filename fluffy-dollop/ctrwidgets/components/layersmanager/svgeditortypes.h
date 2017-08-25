#ifndef SVGEDITORTYPES_H
#define SVGEDITORTYPES_H

#include <QPen>
#include <QBrush>
#include <QColor>

namespace svg_editor_state
{

struct StrokeParams
{
    Qt::PenStyle lineStyle = Qt::SolidLine;
    int lineWidth = 1;
    QColor lineColor = Qt::black;

    bool fillArea = false;
    QColor fillColor = Qt::black;
    double opacity = 0.5;

    QPen getLinePen()const
    {
        QPen pen(lineColor);
        pen.setCosmetic(true);
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::RoundJoin);
        pen.setWidth(lineWidth);
        pen.setStyle(lineStyle);
        return pen;
    }

    QBrush getBrush()const
    {
        if(fillArea)
        {
            QColor _fillColor(fillColor);
            _fillColor.setAlpha(255 * opacity);
            return QBrush(_fillColor);
        }
        else
            return QBrush(Qt::NoBrush);
    }
};


}

#endif // SVGEDITORTYPES_H
