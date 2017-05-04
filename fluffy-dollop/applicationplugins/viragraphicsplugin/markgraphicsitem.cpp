#include "markgraphicsitem.h"
#include "viraeditorview.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QDir>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <regionbiz/rb_manager.h>
#include <regionbiz/rb_locations.h>

using namespace regionbiz;

MarkGraphicsItem::MarkGraphicsItem(qulonglong id)
    : _id(id)
{
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setZValue(100000);
    setPixmap(QPixmap(":/img/mark_tr.png"));
    setOffset(-6, -43);
    setAcceptHoverEvents(true);

    reinit();
}

void MarkGraphicsItem::setItemselected(bool on_off)
{
    //setPixmap(on_off ? QPixmap(":/img/selected_mark.png") : QPixmap(":/img/mark.png"));
    if(on_off)
    {
        foreach(QGraphicsView * view, scene()->views())
        {
            QRectF viewportSceneRect(view->mapToScene(view->contentsRect().topLeft()), view->mapToScene(view->contentsRect().bottomRight()));
            if(viewportSceneRect.contains(scenePos()) == false)
                view->centerOn(this);
        }

        _preview = new MarkPreviewItem(_pixmap, this);
        _preview->setPos(-6, - (40 + _pixmap.height())); // -6, -43

        MarkPtr ptr = RegionBizManager::instance()->getMark(_id);
        if(ptr)
        {
            QPolygonF pol = ptr->getCoords();
            if(pol.size() > 1)
            {

                QPen pen(Qt::blue);
                pen.setWidth(2);
                pen.setColor(true);

                _area = new QGraphicsPolygonItem(pol);
                _area->setFlag(QGraphicsItem::ItemIsSelectable, false);
                _area->setPen(pen);
                _area->setBrush(QBrush(QColor(0,255,0,30)));
                _area->setZValue(99999);
                scene()->addItem(_area);
                setOpacity(0.25);
                connect(_preview, SIGNAL(hoverEnterEvent(bool)), this, SLOT(hoverEnterEventInPreview(bool)));
            }
        }
    }
    else
    {
        setOpacity(1);

        delete _area;
        _area = nullptr;;

        delete _preview;
        _preview = nullptr;
    }

    _isSelected = on_off;
}

void MarkGraphicsItem::centerOnItem()
{
    foreach(QGraphicsView * view, scene()->views())
    {
        QRectF viewportSceneRect(view->mapToScene(view->contentsRect().topLeft()), view->mapToScene(view->contentsRect().bottomRight()));
        if(viewportSceneRect.contains(scenePos()) == false)
            view->centerOn(this);
    }
}

quint64 MarkGraphicsItem::getId()
{
    return _id;
}

void MarkGraphicsItem::reinit()
{
    MarkPtr ptr = RegionBizManager::instance()->getMark(_id);
    if( ! ptr)
        return;

    QString annotation = ptr->getName();
    if( annotation.isEmpty() )
        annotation = QString::fromUtf8("дефект");

    QString toolotip = annotation;
    BaseMetadataPtr data_time = ptr->getMetadata("date");
    if(data_time)
        toolotip.append(QString(", выполнить до : ") + data_time->getValueAsString());

    BaseMetadataPtr responsible = ptr->getMetadata("worker");
    if(responsible)
    {
        QString text = responsible->getValueAsString();
        if(text.isEmpty() == false)
            toolotip.append(QString(", исполнитель : ") + text);
    }
    setToolTip(toolotip);

    QPixmap pmOrig(":/img/mark_tr.png");
    BaseMetadataPtr category = ptr->getMetadata("category");
    if(category)
    {
        QString categoryStr = category->getValueAsString();
        if(categoryStr == QString::fromUtf8("электроснабжение"))
            pmOrig = QPixmap(":/img/lamp.png");
        else if(categoryStr == QString::fromUtf8("водоснабжение"))
            pmOrig = QPixmap(":/img/bathtube.png");
        else if(categoryStr == QString::fromUtf8("водоотведение"))
            pmOrig = QPixmap(":/img/plumbing.png");
        else if(categoryStr == QString::fromUtf8("отопление"))
            pmOrig = QPixmap(":/img/heater.png");
        else if(categoryStr == QString::fromUtf8("вентиляция"))
            pmOrig = QPixmap(":/img/tiles.png");
        else if(categoryStr == QString::fromUtf8("тепловые сети"))
            pmOrig = QPixmap(":/img/balance-ruler.png");
        else if(categoryStr == QString::fromUtf8("слаботочные сети"))
            pmOrig = QPixmap(":/img/electrical-plug.png");
        else if(categoryStr == QString::fromUtf8("газоснабжение"))
            pmOrig = QPixmap(":/img/water-heater.png");
        else if(categoryStr == QString::fromUtf8("технологические решения"))
            pmOrig = QPixmap(":/img/wrench.png");
        else if(categoryStr == QString::fromUtf8("архитектурные решения"))
            pmOrig = QPixmap(":/img/stairs.png");
    }
    QColor color;
    BaseMetadataPtr status = ptr->getMetadata("status");
    if(status)
    {
        QString statusStr = status->getValueAsString();
        if(statusStr == QString::fromUtf8("новый"))
            color = QColor(233,124,27);
        else if(statusStr == QString::fromUtf8("в работе"))
            color = QColor(Qt::yellow); // 226,224,111);
        else if(statusStr == QString::fromUtf8("на проверку"))
            color = QColor(Qt::green); // 86,206,18);

        color.setAlpha(120);

        QPixmap pm(pmOrig.size());
        pm.fill(Qt::transparent);
        QPainter pr(&pm);
        pr.setPen(Qt::NoPen);
        pr.setBrush(QBrush(color));

        QPolygonF pixmapFillPolygon;
        pixmapFillPolygon << QPointF(1,5) << QPointF(46,5) << QPointF(46,35) << QPointF(14,35) << QPointF(7,41) << QPointF(1,35);
        pr.drawPolygon(pixmapFillPolygon);
        pr.drawPixmap(0, 0, pmOrig);

        setPixmap(pm);
    }
    else
        setPixmap(QPixmap(":/img/mark_tr.png"));

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
        if(_area)
        {
            delete _area;
            _area = nullptr;
        }
        setItemselected(true);
    }
}

void MarkGraphicsItem::hoverEnterEventInPreview(bool on_off)
{
    setOpacity(on_off ? 1 : 0.25);
}

void MarkGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalSelectItem(_id, false);
    //QGraphicsPixmapItem::mousePressEvent(event);
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

//----------------------------------------------------------------------------

MarkPreviewItem::MarkPreviewItem(QPixmap pixmap, QGraphicsItem *parent)
    : QGraphicsPixmapItem(pixmap, parent)
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    setFlag(QGraphicsItem::ItemStacksBehindParent, true);
    setAcceptHoverEvents(true);
}

void MarkPreviewItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsPixmapItem::hoverEnterEvent(event);
    emit hoverEnterEvent(true);
}

void MarkPreviewItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsPixmapItem::hoverLeaveEvent(event);
    emit hoverEnterEvent(false);
}
















