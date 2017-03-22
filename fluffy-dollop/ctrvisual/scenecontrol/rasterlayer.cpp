#include "rasterlayer.h"
#include <QPen>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <ctrcore/provider/dataproviderfactory.h>
#include <math.h>

using namespace visualize_system;

RasterLayer::RasterLayer(uint id, QPolygonF polygon, RasterLayerInitData rasterLayerInitData)
    : _id(id)
    , _rasterLayerInitData(rasterLayerInitData)
    , Z_LEVEL(50)
    , _polygon(polygon)
    , _isSelected(false)
    , _isMarked(false)
    , _isActivated(false)
{
    _polygon.append(_polygon.first());
    _painterPath.addPolygon(_polygon);
    recalcMarks();

    _rasterLayerPolygonItem = new RasterLayerPolygonItem(*this, polygon);
    _rasterLayerPolygonItem->setZValue(Z_LEVEL);
}

RasterLayer::RasterLayer(uint id, QPointF centerMatchingPoint, double imgDirection, int orientation, RasterLayerInitData rasterLayerInitData)
    : _id(id)
    , _imgDirection(imgDirection)
    , _rasterLayerInitData(rasterLayerInitData)
    , Z_LEVEL(50)
    , _isSelected(false)
    , _isMarked(false)
    , _isActivated(false)
{
    _rasterLayerPixmapItem = new RasterLayerPixmapItem(*this, orientation, imgDirection);
    _rasterLayerPixmapItem->setPos(centerMatchingPoint);
    _rasterLayerPixmapItem->setZValue(Z_LEVEL);
}

RasterLayer::~RasterLayer()
{
    if(_rasterLayerPolygonItem)
        delete _rasterLayerPolygonItem;

    if(_rasterLayerPixmapItem)
        delete _rasterLayerPixmapItem;
}

void RasterLayer::recalcMarks()
{
    _corners.clear();
    _cornersPath.clear();
    if(_polygon.size() != 5)
        return;

    double minSide(0);
    for(int i(1); i<4; ++i)
    {
        double _lenght = lenght(_polygon.at(i-1), _polygon.at(i));
        if(_lenght < minSide || i == 1)
            minSide = _lenght;
    }

    double cornerSide = minSide / _rasterLayerInitData.partOfSide;
    for(int i(0); i<4; ++i)
    {
        QPointF _p = ( i == 0 ? _polygon.at(3) : _polygon.at(i-1));
        QPointF _p_ = _polygon.at(i);
        QPointF p_ = _polygon.at(i+1);

        QPointF p = _p - _p_;
        double fi = atan2(p.y(), p.x()) * 180 / M_PI;
        if (fi < 0 )
            fi += 360;
        QPointF p1( _p_.x() + cornerSide * cos( fi * M_PI / 180 ), _p_.y() + cornerSide * sin( fi * M_PI / 180 ) );

        p = p_ - _p_;
        fi = atan2(p.y(), p.x()) * 180 / M_PI;
        if (fi < 0 )
            fi += 360;
        QPointF p2( _p_.x() + cornerSide * cos( fi * M_PI / 180 ), _p_.y() + cornerSide * sin( fi * M_PI / 180 ) );

        QPolygonF pol;
        pol << _p_ << p1 << p2;
        _corners.append(pol);

        QPainterPath painterPath;
        painterPath.addPolygon(pol);
        _cornersPath.append(painterPath);
    }
}

double RasterLayer::lenght(const QPointF & p1, const QPointF & p2) const
{
    return sqrt( (p1.x()-p2.x())*(p1.x()-p2.x()) + (p1.y()-p2.y())*(p1.y()-p2.y()) );
}

uint RasterLayer::id() const
{
    return _id;
}

void RasterLayer::setRasterPolygon(const QPolygonF &polygon)
{
    if(_rasterLayerPolygonItem)
    {
        _rasterLayerPolygonItem->setPolygon(polygon);
        _polygon = polygon;
        _polygon.append(_polygon.first());
        _painterPath = 	QPainterPath();
        _painterPath.addPolygon(_polygon);
        recalcMarks();

        _rasterLayerPolygonItem->update();
    }
}

void RasterLayer::setDefaultPen(QPen pen)
{
    _rasterLayerInitData.defaultPen = pen;
    if(_rasterLayerPolygonItem)
        _rasterLayerPolygonItem->update();
    if(_rasterLayerPixmapItem)
        _rasterLayerPixmapItem->update();
}

void RasterLayer::setLayerSelected(bool on_off)
{
    _isSelected = on_off;
    if(_rasterLayerPolygonItem)
        _rasterLayerPolygonItem->update();
    if(_rasterLayerPixmapItem)
        _rasterLayerPixmapItem->update();
}

void RasterLayer::setLayerMarked(bool on_off)
{
    _isMarked = on_off;
    if(_rasterLayerPolygonItem)
        _rasterLayerPolygonItem->update();
    if(_rasterLayerPixmapItem)
        _rasterLayerPixmapItem->update();
}

bool RasterLayer::containsPoint(QPointF p)
{
    if(_rasterLayerPolygonItem)
    {
        if(_rasterLayerPolygonItem->isVisible())
            return _polygon.containsPoint(p, Qt::OddEvenFill);
    }
    if(_rasterLayerPixmapItem)
    {
        if(_rasterLayerPixmapItem->isVisible())
            return true;
    }

    return false;
}

bool RasterLayer::intersect(QPolygonF polygon)
{
    if(_rasterLayerPolygonItem)
    {
        if(_rasterLayerPolygonItem->isVisible())
            return _polygon.intersected(polygon).isEmpty() == false;
    }
    if(_rasterLayerPixmapItem)
    {
        if(_rasterLayerPixmapItem->isVisible())
            return polygon.containsPoint(_rasterLayerPixmapItem->scenePos(), Qt::OddEvenFill);
    }

    return false;
}

void RasterLayer::emit_signalSetActive()
{
    emit signalSetActive();
}

void RasterLayer::emit_signalClicked(bool withControl)
{
    emit signalClicked(withControl);
}

void RasterLayer::setOnTop()
{
    if(_rasterLayerPolygonItem)
        _rasterLayerPolygonItem->setZValue(Z_LEVEL + 10);
    if(_rasterLayerPixmapItem)
    {
        _rasterLayerPixmapItem->setZValue(Z_LEVEL + 10);
        _rasterLayerPixmapItem->setVisiblePreviw(true);
    }

    _isActivated = true;

    _painterPath = QPainterPath();
    _painterPath.addPolygon(_polygon);

    _cornersPath.clear();
    foreach(QPolygonF pol, _corners)
    {
        QPainterPath painterPath;
        painterPath.addPolygon(pol);
        _cornersPath.append(painterPath);
    }

    if(_rasterLayerPolygonItem)
        _rasterLayerPolygonItem->update();
    if(_rasterLayerPixmapItem)
        _rasterLayerPixmapItem->update();
}

void RasterLayer::setVisible(bool on_off)
{
    if(_rasterLayerPolygonItem)
        _rasterLayerPolygonItem->setVisible(on_off);

    if(_rasterLayerPixmapItem)
        _rasterLayerPixmapItem->setVisible(on_off);
}

QGraphicsItem *RasterLayer::getGraphicsItem()
{
    if(_rasterLayerPolygonItem)
        return _rasterLayerPolygonItem;

    if(_rasterLayerPixmapItem)
        return _rasterLayerPixmapItem;

    return 0;
}

void RasterLayer::subtract(QPolygonF polygon)
{
    if(_rasterLayerPixmapItem)
        _rasterLayerPixmapItem->setZValue(_isSelected ? Z_LEVEL + 1 : Z_LEVEL - 1);

    if(_rasterLayerPolygonItem)
        _rasterLayerPolygonItem->setZValue(_isSelected ? Z_LEVEL + 1 : Z_LEVEL - 1);

    _isActivated = false;

    _painterPath = QPainterPath();
    _painterPath.addPolygon(_polygon);

    _cornersPath.clear();
    foreach(QPolygonF pol, _corners)
    {
        QPainterPath painterPath;
        painterPath.addPolygon(pol);
        _cornersPath.append(painterPath);
    }

    if(polygon.isEmpty() == false)
        if(_polygon.boundingRect().intersects(polygon.boundingRect()))
        {
            QPainterPath sabtractPath;
            polygon.append(polygon.first());
            sabtractPath.addPolygon(polygon);
            _painterPath = _painterPath.subtracted(sabtractPath);

            for(auto it(_cornersPath.begin()); it != _cornersPath.end(); ++it)
                (*it) = (*it).subtracted(sabtractPath);
        }

    if(_rasterLayerPolygonItem)
        _rasterLayerPolygonItem->update();
}

QPolygonF RasterLayer::getPolygon()
{
    return _polygon;
}


///-----------------------------------------------------------

RasterLayerItem::RasterLayerItem(RasterLayer &rasterLayer)
    : _rasterLayer(rasterLayer)
{
}

RasterLayer *RasterLayerItem::getRasterLayer()
{
    return &_rasterLayer;
}

///-----------------------------------------------------------

RasterLayerPolygonItem::RasterLayerPolygonItem(RasterLayer &rasterLayer, QPolygonF polygon)
    : RasterLayerItem(rasterLayer)
    , QGraphicsPolygonItem(polygon)
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
}

void RasterLayerPolygonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
    if(isVisible())
    {
        painter->save();
        painter->setBrush(QBrush());

        if(_rasterLayer.layerIsActivated())
        {
            painter->setPen(_rasterLayer.getRasterLayerInitData().activePen);
            painter->drawPath(_rasterLayer.getPainterPath());
        }
        else if(_rasterLayer.layerIsSelected())
        {
            painter->setPen(_rasterLayer.getRasterLayerInitData().selectPen);
            painter->drawPath(_rasterLayer.getPainterPath());
        }
        else
        {
            painter->setPen(_rasterLayer.getRasterLayerInitData().defaultPen);
            painter->drawPath(_rasterLayer.getPainterPath());
        }

        if(_rasterLayer.layerIsMarked())
        {
            painter->setPen(QPen(Qt::NoPen));
            painter->setBrush(_rasterLayer.getRasterLayerInitData().markBrush);
            foreach(QPainterPath path, _rasterLayer.getCornersPath())
                painter->drawPath(path);
        }

        painter->restore();
    }
}

int RasterLayerPolygonItem::type() const
{
    return RASTER_LAYER_ITEM;
}

RasterLayerItemType RasterLayerPolygonItem::getRasterLayerItemType() const
{
    return RasterLayerItemType::PolygonLayer;
}

///-----------------------------------------------------------

RasterLayerPixmapItem::RasterLayerPixmapItem(RasterLayer &rasterLayer, int orientation, double direction)
    : RasterLayerItem(rasterLayer)
    , _orientation(orientation)
    , _direction(direction)
    , _preview(0)
    , _taskId(-1)
{
    QColor markColor = _rasterLayer.getRasterLayerInitData().markBrush.color();
    QColor color(markColor.red(), markColor.green(), markColor.blue(), 130);
    _markedBrush.setColor(color);

    pm_w = QPixmap(":/img/flag.png");
    pm_g = QPixmap(":/img/flag_green.png");
    pm_y = QPixmap(":/img/flag_yel.png");

    setPixmap(pm_w);
    setOffset(-4, -38);

    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);

    QPen pen;
    QColor colPen;
    colPen.setRgba(qRgba(0x09,0x61,0xD3,150));
    pen.setColor(colPen);
    pen.setWidth(2);
    pen.setCosmetic(true);
    pen.setJoinStyle(Qt::RoundJoin);


    direction -= 90; //! <----------- !!!
    double r(75);
    QPointF pos(r * cos( direction * M_PI / 180 ), r * sin( direction * M_PI / 180 ) );

    _line = new QGraphicsLineItem(this);
    _line->setFlag(QGraphicsItem::ItemStacksBehindParent, true);
    _line->setLine(0, 0, pos.x(), pos.y());
    _line->setPen(pen);

    _directionPixmapItem = new DirectionPixmapItem(rasterLayer, this);
    _directionPixmapItem->setPos(pos);

}

RasterLayerPixmapItem::~RasterLayerPixmapItem()
{
}

void RasterLayerPixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    if(_rasterLayer.layerIsActivated() == false && _preview)
        setVisiblePreviw(false);

    if(isVisible())
    {
        painter->save();

        if(_rasterLayer.layerIsActivated())
        {
            painter->drawPixmap(-4, -38, pm_y);
        }
        else if(_rasterLayer.layerIsSelected())
        {
            painter->drawPixmap(-4, -38, pm_g);
        }
        else
        {
            painter->drawPixmap(-4, -38, pm_w);
        }

        if(_rasterLayer.layerIsMarked())
        {
            painter->setPen(QPen(Qt::NoPen));
            QColor markColor = _rasterLayer.getRasterLayerInitData().markBrush.color();
            QColor color(markColor.red(), markColor.green(), markColor.blue(), 70);
            QBrush markedBrush(color);
            painter->setBrush(markedBrush);
            painter->drawRect(boundingRect());
        }

        painter->restore();
    }
}

int RasterLayerPixmapItem::type() const
{
    return RASTER_LAYER_ITEM;
}

RasterLayerItemType RasterLayerPixmapItem::getRasterLayerItemType() const
{
    return RasterLayerItemType::PointLayer;
}

void RasterLayerPixmapItem::setVisiblePreviw(bool on_off)
{
    if(on_off)
    {
        if(_preview)
        {
            delete _preview;
            _preview = 0;
        }

//        if(_currProvider)
//        {
//            if(_taskId > 0)
//                _currProvider->abort(_taskId);
//            disconnect(_currProvider.data(), SIGNAL(dataReady(int)), this, SLOT(updateImg(int)));
//        }
//        _currProvider.clear();
        _taskId = -1;

//        QSharedPointer<data_system::RasterDataProvider> rdp = data_system::DataProviderFactory::instance()->getProvider(getRasterLayer()->id()).dynamicCast<data_system::RasterDataProvider>();
//        if(rdp)
//        {
//            if(rdp->open(rdp->getProviderUrl()))
//            {
//                _currProvider = rdp;
//                connect(_currProvider.data(), SIGNAL(dataReady(int)), this, SLOT(updateImg(int)));

//                QRect srcRect = _currProvider->boundingRect();
//                srcRect.setX(0);
//                srcRect.setY(0);
//                double aspectRatio = ((double)srcRect.width() / (double)srcRect.height());
//                QSize dstSize(200 * aspectRatio,200);
//                if(dstSize.width() > 300)
//                {
//                    dstSize.setWidth(300);
//                    dstSize.setHeight(300 * aspectRatio);
//                }
//                if(dstSize.height() > 200)
//                {
//                    dstSize.setWidth(200 * aspectRatio);
//                    dstSize.setHeight(200);
//                }

//                _taskId = _currProvider->readData(_timg, QRect(), dstSize);
//            }
//        }
    }
    else
    {
        delete _preview;
        _preview = 0;
    }
}

void RasterLayerPixmapItem::updateImg(int taskId)
{
    if(taskId == _taskId)
    {
        if(_preview)
        {
            delete _preview;
            _preview = 0;
        }

        if(_rasterLayer.layerIsActivated())
        {
            QImage img((unsigned char*)_timg.prt, _timg.w, _timg.h, QImage::Format_RGB32);

            if(_orientation != 1)
            {
                QTransform tr;
                switch(_orientation)
                {
                case 3: tr.rotate(180); break; // по часовой на 180
                case 6: tr.rotate(90); break; // по часовой на 90
                case 8: tr.rotate(270); break; // по часовой на 270
                }
                img = img.transformed(tr);
            }

            QString annotation;
//            if(_currProvider)
//            {
//                objrepr::SpatialObjectPtr ptr= objrepr::RepresentationServer::instance()->objectManager()->getObject(_currProvider->getObjreprRasterId());
//                if(ptr)
//                {
//                    objrepr::AttributePtr atr = ptr->attrMap()->getAttr("Описание фотографии");
//                    if(atr)
//                        annotation = QString::fromUtf8(atr->valueAsString().c_str());
//                }
//            }

            /*
            QPixmap pixmap(img.width(),img.height());
            pixmap.fill(Qt::transparent);
            QPainter p(&pixmap);
            p.save();
            p.drawImage(0, 0, img);
            p.setPen(_rasterLayer.getRasterLayerInitData().activePen);
            p.drawRect(0, 0, img.width(), img.height());
            if(annotation.isEmpty() == false)
            {
                p.setPen(QPen(Qt::white));
                p.drawText(QRectF(2, 2, img.width()-4, img.height()-4), Qt::AlignHCenter | Qt::AlignBottom ,annotation);
            }
            p.restore();
            */

            QPixmap pm;
            if(annotation.isEmpty())
            {
                pm = QPixmap(img.width() + 4, img.height() + 4);
                QPainter pr(&pm);
                pm.fill(Qt::lightGray);

                pr.drawImage(2, 2, img);
                //pr.setPen(_rasterLayer.getRasterLayerInitData().activePen);
                //pr.drawRect(2, 2, img.width(), img.height());

                pr.setPen(Qt::black);
                //pr.drawRect(QRectF(1, 1, pm.width() - 2, pm.height() - 2));
            }
            else
            {
                QPainter _pr(&img);
                QFontMetrics fm(_pr.font());

                if(fm.width(annotation) <= img.width())
                {
                    pm = QPixmap(img.width()+4, 2 + img.height() + 2 + fm.height() + 2);
                    QPainter pr(&pm);
                    pm.fill(Qt::lightGray);
                    pr.drawImage(2,2, img);
                    pr.setPen(Qt::black);
                    //pr.drawRect(QRectF(1, 1, pm.width() - 2, pm.height() - 2));
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

                    pm = QPixmap(img.width()+4, 2 + img.height() + 2 + fm.height() * lines.size() + 4);
                    QPainter pr(&pm);
                    pm.fill(Qt::lightGray);
                    pr.drawImage(2,2, img);
                    pr.setPen(Qt::black);
                    //pr.drawRect(QRectF(1, 1, pm.width() - 2, pm.height() - 2));
                    int N(0);
                    foreach(QString line, lines)
                        pr.drawText(2, 2+ img.height() + 2 + fm.height() * (++N), line);
                }
            }

            _preview = new QGraphicsPixmapItem(pm, this);
            _preview->setFlag(QGraphicsItem::ItemIsSelectable, false);
            _preview->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
            _preview->setFlag(QGraphicsItem::ItemStacksBehindParent, true);

            QPointF pos = _directionPixmapItem->pos();
            if(_direction > 270)
            {
                pos.setX( pos.x() -  pm.width());
                pos.setY( pos.y() -  pm.height());
            }
            else if(_direction > 180)
            {
                pos.setX( pos.x() -  pm.width());
            }
            else if(_direction > 90)
            {

            }
            else
            {
                pos.setY( pos.y() -  pm.height());
            }

            _preview->setPos(pos); // _preview->setPos(-3, -(pm.height() + 38));
        }

//        if(_currProvider)
//            disconnect(_currProvider.data(), SIGNAL(dataReady(int)), this, SLOT(updateImg(int)));

//        _currProvider.clear();
        _taskId = -1;
    }
}

void RasterLayerPixmapItem::slotDeleteLater()
{

}

///-----------------------------------------------------------


DirectionPixmapItem::DirectionPixmapItem(RasterLayer & rasterLayer, QGraphicsItem * parent)
    : RasterLayerItem(rasterLayer)
    , QGraphicsPixmapItem(parent)
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIsMovable, false);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
    //setFlag(QGraphicsItem::ItemStacksBehindParent, true);
    // setCursor(QCursor(Qt::CrossCursor));

    QPen pen;
    QColor colPen;
    colPen.setRgba(qRgba(0x09,0x61,0xD3,150));
    pen.setColor(colPen);
    pen.setWidth(2);
    pen.setCosmetic(true);
    pen.setJoinStyle(Qt::RoundJoin);

    {
        pm_w = QPixmap(11,11);
        pm_w.fill(Qt::transparent);

        QBrush brush;
        brush.setColor(Qt::white);
        brush.setStyle(Qt::SolidPattern);

        QPainter pr(&pm_w);
        pr.save();
        pr.setPen(pen);
        pr.setBrush(brush);
        pr.drawEllipse(0, 0, pm_w.width()-1, pm_w.height()-1);
        pr.restore();
    }
    {
        pm_g = QPixmap(11,11);
        pm_g.fill(Qt::transparent);

        QBrush brush;
        brush.setColor(Qt::green);
        brush.setStyle(Qt::SolidPattern);

        QPainter pr(&pm_g);
        pr.save();
        pr.setPen(pen);
        pr.setBrush(brush);
        pr.drawEllipse(0, 0, pm_g.width()-1, pm_g.height()-1);
        pr.restore();
    }
    {
        pm_y = QPixmap(11,11);
        pm_y.fill(Qt::transparent);

        QBrush brush;
        brush.setColor(Qt::yellow);
        brush.setStyle(Qt::SolidPattern);

        QPainter pr(&pm_y);
        pr.save();
        pr.setPen(pen);
        pr.setBrush(brush);
        pr.drawEllipse(0, 0, pm_y.width()-1, pm_y.height()-1);
        pr.restore();
    }

    setPixmap(pm_y);
    setOffset(-5, -5);
}

DirectionPixmapItem::~DirectionPixmapItem()
{
}

void DirectionPixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(isVisible())
    {
        painter->save();

        if(_rasterLayer.layerIsActivated())
        {
            painter->drawPixmap(-5, -5, pm_y);
        }
        else if(_rasterLayer.layerIsSelected())
        {
            painter->drawPixmap(-5, -5, pm_g);
        }
        else
        {
            painter->drawPixmap(-5, -5, pm_w);
        }

        painter->restore();
    }
}

RasterLayerItemType DirectionPixmapItem::getRasterLayerItemType() const
{
    return RasterLayerItemType::PointLayer;
}

int DirectionPixmapItem::type() const
{
    return RASTER_LAYER_ITEM;
}










/*
                static int i = 0;
                if(++i > 33)
                    i = 1;
                QPainter::CompositionMode compositionMode;
                switch(i)
                {
                case 1 :
                    annotation = "CompositionMode_SourceOver";
                    compositionMode = QPainter::CompositionMode_SourceOver;
                    break;

                case 2 :
                    annotation = "CompositionMode_DestinationOver";
                    compositionMode = QPainter::CompositionMode_DestinationOver;
                    break;

                case 3 :
                    annotation = "CompositionMode_Clear";
                    compositionMode = QPainter::CompositionMode_Clear;
                    break;

                case 4 :
                    annotation = "CompositionMode_Source";
                    compositionMode = QPainter::CompositionMode_Source;
                    break;

                case 5 :
                    annotation = "CompositionMode_Destination";
                    compositionMode = QPainter::CompositionMode_Destination;
                    break;

                case 6 :
                    annotation = "CompositionMode_SourceIn";
                    compositionMode = QPainter::CompositionMode_SourceIn;
                    break;

                case 7 :
                    annotation = "CompositionMode_DestinationIn";
                    compositionMode = QPainter::CompositionMode_DestinationIn;
                    break;

                case 8 :
                    annotation = "CompositionMode_SourceOut";
                    compositionMode = QPainter::CompositionMode_SourceOut;
                    break;

                case 9 :
                    annotation = "CompositionMode_DestinationOut";
                    compositionMode = QPainter::CompositionMode_DestinationOut;
                    break;

                case 10 :
                    annotation = "CompositionMode_SourceAtop";
                    compositionMode = QPainter::CompositionMode_SourceAtop;
                    break;

                case 11 :
                    annotation = "CompositionMode_DestinationAtop";
                    compositionMode = QPainter::CompositionMode_DestinationAtop;
                    break;

                case 12 :
                    annotation = "CompositionMode_Xor";
                    compositionMode = QPainter::CompositionMode_Xor;
                    break;

                case 13 :
                    annotation = "CompositionMode_Plus";
                    compositionMode = QPainter::CompositionMode_Plus;
                    break;

                case 14 :
                    annotation = "CompositionMode_Multiply";
                    compositionMode = QPainter::CompositionMode_Multiply;
                    break;

                case 15 :
                    annotation = "CompositionMode_Screen";
                    compositionMode = QPainter::CompositionMode_Screen;
                    break;

                case 16 :
                    annotation = "CompositionMode_Overlay";
                    compositionMode = QPainter::CompositionMode_Overlay;
                    break;

                case 17 :
                    annotation = "CompositionMode_Darken";
                    compositionMode = QPainter::CompositionMode_Darken;
                    break;

                case 18 :
                    annotation = "CompositionMode_Lighten";
                    compositionMode = QPainter::CompositionMode_Lighten;
                    break;

                case 19 :
                    annotation = "CompositionMode_ColorDodge";
                    compositionMode = QPainter::CompositionMode_ColorDodge;
                    break;

                case 20 :
                    annotation = "CompositionMode_ColorBurn";
                    compositionMode = QPainter::CompositionMode_ColorBurn;
                    break;

                case 21 :
                    annotation = "CompositionMode_HardLight";
                    compositionMode = QPainter::CompositionMode_HardLight;
                    break;

                case 22:
                    annotation = "CompositionMode_SoftLight";
                    compositionMode = QPainter::CompositionMode_SoftLight;
                    break;

                case 23 :
                    annotation = "CompositionMode_Difference";
                    compositionMode = QPainter::CompositionMode_Difference;
                    break;

                case 24 :
                    annotation = "CompositionMode_Exclusion";
                    compositionMode = QPainter::CompositionMode_Exclusion;
                    break;

                case 25 :
                    annotation = "RasterOp_SourceOrDestination";
                    compositionMode = QPainter::RasterOp_SourceOrDestination;
                    break;

                case 26 :
                    annotation = "RasterOp_SourceAndDestination";
                    compositionMode = QPainter::RasterOp_SourceAndDestination;
                    break;

                case 27 :
                    annotation = "RasterOp_SourceXorDestination";
                    compositionMode = QPainter::RasterOp_SourceXorDestination;
                    break;

                case 28 :
                    annotation = "RasterOp_NotSourceAndNotDestination";
                    compositionMode = QPainter::RasterOp_NotSourceAndNotDestination;
                    break;

                case 29 :
                    annotation = "RasterOp_NotSourceOrNotDestination";
                    compositionMode = QPainter::RasterOp_NotSourceOrNotDestination;
                    break;

                case 30 :
                    annotation = "RasterOp_NotSourceXorDestination";
                    compositionMode = QPainter::RasterOp_NotSourceXorDestination;
                    break;

                case 31 :
                    annotation = "RasterOp_NotSource";
                    compositionMode = QPainter::RasterOp_NotSource;
                    break;

                case 32 :
                    annotation = "RasterOp_NotSourceAndDestination";
                    compositionMode = QPainter::RasterOp_NotSourceAndDestination;
                    break;

                case 33 :
                    annotation = "RasterOp_SourceAndNotDestination";
                    compositionMode = QPainter::RasterOp_SourceAndNotDestination;
                    break;
                }
                annotation.append(QString("  i - ") + QString::number(i));
*/
