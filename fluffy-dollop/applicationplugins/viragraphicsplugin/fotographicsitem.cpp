#include "fotographicsitem.h"
#include "handledirectionitem.h"
#include <QGraphicsView>
#include <QGraphicsLineItem>
#include <QDir>
#include <QDebug>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <regionbiz/rb_manager.h>
#include <regionbiz/rb_locations.h>

using namespace regionbiz;

FotoGraphicsItem::FotoGraphicsItem(qulonglong id)
    : _id(id)
{
    setPixmap(QPixmap(":/img/foto.png"));
    setOffset(-25, -22);

    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    setZValue(1000000);

    reinit();
}

FotoGraphicsItem::~FotoGraphicsItem()
{

}

void FotoGraphicsItem::setItemselected(bool on_off)
{
    if(on_off)
    {
        setPixmap(QPixmap(":/img/foto_selected.png"));
        _preview = new QGraphicsPixmapItem(_pixmap, this);
        QPointF _pos = _directionItem->pos();
        if(_direction > 270)
        {
        }
        else if(_direction > 180)
        {
            _pos.setX( _pos.x() -  _pixmap.width());
        }
        else if(_direction > 90)
        {
            _pos.setX( _pos.x() -  _pixmap.width());
            _pos.setY( _pos.y() -  _pixmap.height());
        }
        else
        {
            _pos.setY( _pos.y() -  _pixmap.height());
        }
        _preview->setPos(_pos);
    }
    else
    {
        setPixmap(QPixmap(":/img/foto.png"));
        delete _preview;
        _preview = nullptr;
    }
}

void FotoGraphicsItem::centerOnItem()
{
    foreach(QGraphicsView * view, scene()->views())
    {
        QRectF viewportSceneRect(view->mapToScene(view->contentsRect().topLeft()), view->mapToScene(view->contentsRect().bottomRight()));
        if(viewportSceneRect.contains(scenePos()) == false)
            view->centerOn(this);
    }
}

quint64 FotoGraphicsItem::getId()
{
    return _id;
}

void FotoGraphicsItem::reinit()
{
    MarkPtr ptr = RegionBizManager::instance()->getMark(_id);
    if( ! ptr)
        return;

    setPos(ptr->getCenter());

    BaseMetadataPtr directionPtr = ptr->getMetadata("foto_direction");
    if(directionPtr)
        _direction = directionPtr->getValueAsVariant().toDouble();

    QPen pen;
    QColor colPen;
    colPen.setRgba(qRgba(0,0,0,255)); // qRgba(0x09,0x61,0xD3,150));
    pen.setColor(colPen);
    pen.setWidth(2);
    pen.setCosmetic(true);
    pen.setJoinStyle(Qt::RoundJoin);

    double r(45);
    QPointF pos(r * cos( -_direction * M_PI / 180 ), r * sin( -_direction * M_PI / 180 ) );
    QLineF line(QPointF(0,0), pos);

    if( ! _line)
    {
        _line = new QGraphicsLineItem(this);
        _line->setFlag(QGraphicsItem::ItemStacksBehindParent, true);
        _line->setPen(pen);
    }
    _line->setLine(line);

    if( ! _directionItem)
        _directionItem = new HandleDirectionItem(this, false);
    _directionItem->setPos(pos);

    QPixmap pixmap;
    QVariant regionBizInitJson_Path = CtrConfig::getValueByName("application_settings.regionBizFilesPath");
    if(regionBizInitJson_Path.isValid())
    {
         QString destPath = regionBizInitJson_Path.toString()+ QDir::separator() + QString::number(_id);
         QDir dir(destPath);
         QStringList list = dir.entryList(QDir::Files);
         if(list.isEmpty() == false)
         {
             destPath = destPath + QDir::separator() + list.first();
             pixmap = QPixmap(destPath);
         }
    }
    if(pixmap.isNull())
        return;

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

    QString annotation = ptr->getName();
//    if( annotation.isEmpty() )
//        annotation = QString::fromUtf8("фотография");

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
    if(_preview)
    {
        delete _preview;
        _preview = nullptr;
        setItemselected(true);
    }
}

void FotoGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalSelectItem(_id, false);
}

void FotoGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalSelectItem(_id, true);
}





































