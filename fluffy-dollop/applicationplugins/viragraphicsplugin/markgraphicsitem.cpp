#include "markgraphicsitem.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QLabel>

MarkGraphicsItem::MarkGraphicsItem(qulonglong id, const QPixmap &pixmap, const QString &annotation)
    : _id(id)
{
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setZValue(10000);
    setPixmap(QPixmap(":/img/mark.png"));
    setOffset(-6, -43);
    setAcceptHoverEvents(true);

    double aspectRatio = ((double)pixmap.width() / (double)pixmap.height());
    QSize dstSize(200 * aspectRatio,200);
    if(dstSize.width() > 300)
    {
        dstSize.setWidth(300);
        dstSize.setHeight(300 * aspectRatio);
    }
    if(dstSize.height() > 200)
    {
        dstSize.setWidth(200 * aspectRatio);
        dstSize.setHeight(200);
    }
    QPixmap img = pixmap.scaled(dstSize, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    if(annotation.isEmpty())
    {
        _pixmap = QPixmap(img.width() + 4, img.height() + 4);
        QPainter pr(&_pixmap);
        _pixmap.fill(Qt::lightGray);

        pr.drawPixmap(2, 2, img);
        //pr.setPen(_rasterLayer.getRasterLayerInitData().activePen);
        //pr.drawRect(2, 2, img.width(), img.height());

        pr.setPen(Qt::black);
        pr.drawRect(QRectF(1, 1, _pixmap.width() - 2, _pixmap.height() - 2));
    }
    else
    {
        QPainter _pr(&img);
        QFontMetrics fm(_pr.font());

        if(fm.width(annotation) <= img.width())
        {
            _pixmap = QPixmap(img.width()+4, 2 + img.height() + 2 + fm.height() + 2);
            QPainter pr(&_pixmap);
            _pixmap.fill(Qt::lightGray);
            pr.drawPixmap(2,2, img);
            pr.setPen(Qt::black);
            pr.drawRect(QRectF(1, 1, _pixmap.width() - 2, _pixmap.height() - 2));
            pr.drawText(QRectF(2, img.height() + 4, img.width(), fm.height()), Qt::AlignHCenter | Qt::AlignBottom ,annotation);
        }
        else
        {
            QStringList lines;
            QStringList list = annotation.split(" ");
            QString line;
            foreach(QString str, list)
            {
                QString _line = line;
                if(_line.isEmpty())
                    _line.append(str);
                else
                    _line.append(" " + str);

                if(fm.width(_line) <= img.width())
                {
                    line = _line;
                }
                else
                {
                    lines.append(line);
                    line = str;
                }
            }
            if(line.isEmpty() == false)
                lines.append(line);

            _pixmap = QPixmap(img.width()+4, 2 + img.height() + 2 + fm.height() * lines.size() + 4);
            QPainter pr(&_pixmap);
            _pixmap.fill(Qt::lightGray);
            pr.drawPixmap(2,2, img);
            pr.setPen(Qt::black);
            pr.drawRect(QRectF(1, 1, _pixmap.width() - 2, _pixmap.height() - 2));
            int N(0);
            foreach(QString line, lines)
                pr.drawText(2, 2+ img.height() + 2 + fm.height() * (++N), line);
        }
    }
}

void MarkGraphicsItem::setItemselected(bool on_off)
{
    setPixmap(on_off ? QPixmap(":/img/selected_mark.png") : QPixmap(":/img/mark.png"));
    if(on_off)
    {
        foreach(QGraphicsView * view, scene()->views())
        {
            QRectF viewportSceneRect(view->mapToScene(view->contentsRect().topLeft()), view->mapToScene(view->contentsRect().bottomRight()));
            if(viewportSceneRect.contains(scenePos()) == false)
                view->centerOn(this);
        }

        _preview = new QGraphicsPixmapItem(_pixmap, this);
        _preview->setFlag(QGraphicsItem::ItemIsSelectable, false);
        _preview->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
        _preview->setFlag(QGraphicsItem::ItemStacksBehindParent, true);
        _preview->setAcceptHoverEvents(true);
        _preview->setPos(-6, - (40 + _pixmap.height())); // -6, -43
    }
    else
    {
        delete _preview;
        _preview = nullptr;
    }

    _isSelected = on_off;
}

void MarkGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalSelectItem(_id, false);
}

void MarkGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalSelectItem(_id, true);
}

void MarkGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
//    if( ! _isSelected)
//    {
//        emit itemIsHover(_areaInitData.id, true);
//        setPen(_areaInitData.penHoverl);
//        setBrush(_areaInitData.brushHoverl);
//    }
//    QGraphicsPolygonItem::hoverEnterEvent(event);
}

void MarkGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
//    if( ! _isSelected)
//    {
//        emit itemIsHover(_areaInitData.id, false);
//        setPen(_areaInitData.penNormal);
//        setBrush(_areaInitData.brushNormal);
//    }
//    QGraphicsPolygonItem::hoverLeaveEvent(event);
}
