#include "defectgraphicsitem.h"
#include "viraeditorview.h"
#include <QGraphicsPixmapItem>
#include <QGraphicsView>
#include <QDir>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <regionbiz/rb_manager.h>
#include <regionbiz/rb_locations.h>

using namespace regionbiz;

DefectGraphicsItem::DefectGraphicsItem(qulonglong id)
    : _id(id)
{
    setFlag(QGraphicsItem::ItemIgnoresTransformations);
    setZValue(100000);
    setPixmap(QPixmap(":/img/point_defect.png"));
    setOffset(-6, -43);
    setAcceptHoverEvents(true);

    reinit();
}

DefectGraphicsItem::~DefectGraphicsItem()
{
    if(_area)
        delete _area;
}

void DefectGraphicsItem::setItemselected(bool on_off)
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

        _preview = new DefectPreviewItem(_pixmap, this);
        //-18, -2


        MarkPtr ptr = RegionBizManager::instance()->getMark(_id);
        if(ptr)
        {
            QPolygonF pol = ptr->getCoords();
            if(pol.size() > 1)
            {
                QPen pen(Qt::blue);
                pen.setWidth(2);
                pen.setCosmetic(true);

                QColor brushColor(233,124,27);
                BaseMetadataPtr status = ptr->getMetadata("status");
                if(status)
                {
                    QString statusStr = status->getValueAsString();
                    if(statusStr == QString::fromUtf8("новый"))
                        brushColor = QColor(233,124,27);
                    else if(statusStr == QString::fromUtf8("в работе"))
                        brushColor = QColor(Qt::yellow);
                    else if(statusStr == QString::fromUtf8("на проверку"))
                        brushColor = QColor(Qt::green);
                }
                brushColor.setAlpha(30);

                _area = new DefectAreaItem(pol);
                _area->setFlag(QGraphicsItem::ItemIsSelectable, false);
                _area->setPen(pen);
                _area->setBrush(QBrush(brushColor));
                _area->setZValue(99999);
                scene()->addItem(_area);
                setOpacity(0.25);
                connect(_preview, SIGNAL(hoverEnterEvent(bool)), this, SLOT(hoverEnterEventInPreview(bool)));

                _preview->setPos(- (pixmap().width()/2. - 18) , - (2 + pixmap().height()/2. + _pixmap.height()));
            }
            else
            {
                _preview->setPos(-6, - (40 + _pixmap.height()));
            }
        }
    }
    else
    {
        setOpacity(1);

        delete _area;
        _area = nullptr;

        delete _preview;
        _preview = nullptr;
    }

    _isSelected = on_off;
}

void DefectGraphicsItem::setItemEnabled(bool on_off)
{
    setEnabled(on_off);
}

void DefectGraphicsItem::setItemVisible(bool on_off)
{
    setVisible(on_off);
}

void DefectGraphicsItem::centerOnItem()
{
    foreach(QGraphicsView * view, scene()->views())
    {
        QRectF viewportSceneRect(view->mapToScene(view->contentsRect().topLeft()), view->mapToScene(view->contentsRect().bottomRight()));
        if(viewportSceneRect.contains(scenePos()) == false)
            view->centerOn(this);
    }
}

quint64 DefectGraphicsItem::getId()
{
    return _id;
}

ViraGraphicsItem::ItemType DefectGraphicsItem::getItemType()
{
    return ItemType_Defect;
}

void DefectGraphicsItem::reinit()
{
    MarkPtr ptr = RegionBizManager::instance()->getMark(_id);
    if( ! ptr)
        return;

    setPos(ptr->getCenter());

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

    QPixmap pmOrig(":/img/point_defect.png");
    QPoint pixmapPos(0,0);
    QPolygonF pixmapFillPolygon;
    pixmapFillPolygon << QPointF(1,5) << QPointF(46,5) << QPointF(46,35) << QPointF(14,35) << QPointF(7,41) << QPointF(1,35);

    setOffset(-offset().x(), -offset().y());
    QPolygonF pol = ptr->getCoords();
    if(pol.size() > 1)
    {
        pmOrig = QPixmap(":/img/polygon_defect.png");
        setOffset(-pmOrig.width()/2., -pmOrig.height()/2.);
        pixmapPos = QPoint(11, 3);
        pixmapFillPolygon.clear();
        pixmapFillPolygon << QPointF(19,0) << QPointF(50,0) << QPointF(67,26) << QPointF(50,52) << QPointF(19,52) << QPointF(1,26) << QPointF(19,0);
    }
    else
    {
        setOffset(-6, -43);
    }
    BaseMetadataPtr category = ptr->getMetadata("category");
    if(category)
    {
        QPainter pr(&pmOrig);
        QString categoryStr = category->getValueAsString();
        if(categoryStr == QString::fromUtf8("электроснабжение"))
            pr.drawPixmap(pixmapPos, QPixmap(":/img/lamp.png"));
        else if(categoryStr == QString::fromUtf8("водоснабжение"))
            pr.drawPixmap(pixmapPos, QPixmap(":/img/bathtube.png"));
        else if(categoryStr == QString::fromUtf8("водоотведение"))
            pr.drawPixmap(pixmapPos, QPixmap(":/img/plumbing.png"));
        else if(categoryStr == QString::fromUtf8("отопление"))
            pr.drawPixmap(pixmapPos, QPixmap(":/img/heater.png"));
        else if(categoryStr == QString::fromUtf8("вентиляция"))
            pr.drawPixmap(pixmapPos, QPixmap(":/img/tiles.png"));
        else if(categoryStr == QString::fromUtf8("тепловые сети"))
            pr.drawPixmap(pixmapPos, QPixmap(":/img/balance-ruler.png"));
        else if(categoryStr == QString::fromUtf8("слаботочные сети"))
            pr.drawPixmap(pixmapPos, QPixmap(":/img/electrical-plug.png"));
        else if(categoryStr == QString::fromUtf8("газоснабжение"))
            pr.drawPixmap(pixmapPos, QPixmap(":/img/water-heater.png"));
        else if(categoryStr == QString::fromUtf8("технологические решения"))
            pr.drawPixmap(pixmapPos, QPixmap(":/img/wrench.png"));
        else if(categoryStr == QString::fromUtf8("архитектурные решения"))
            pr.drawPixmap(pixmapPos, QPixmap(":/img/stairs.png"));
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
        pr.drawPolygon(pixmapFillPolygon);
        pr.drawPixmap(0, 0, pmOrig);

        setPixmap(pm);
    }
    else
        setPixmap(QPixmap(":/img/point_defect.png"));

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

void DefectGraphicsItem::hoverEnterEventInPreview(bool on_off)
{
    setOpacity(on_off ? 1 : 0.25);
}

void DefectGraphicsItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalSelectItem(_id, false);
    //QGraphicsPixmapItem::mousePressEvent(event);
}

void DefectGraphicsItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    emit signalSelectItem(_id, true);
}

void DefectGraphicsItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
//    if( ! _isSelected)
//    {
//        emit itemIsHover(_areaInitData.id, true);
//        setPen(_areaInitData.penHoverl);
//        setBrush(_areaInitData.brushHoverl);
//    }
//    QGraphicsPolygonItem::hoverEnterEvent(event);
}

void DefectGraphicsItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
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

DefectAreaItem::DefectAreaItem(const QPolygonF &polygon)
    : QGraphicsPolygonItem(polygon)
{
}

void DefectAreaItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setRenderHint(QPainter::Antialiasing);
    QGraphicsPolygonItem::paint(painter, option, widget);
}

DefectPreviewItem::DefectPreviewItem(QPixmap pixmap, QGraphicsItem *parent)
    : QGraphicsPixmapItem(pixmap, parent)
{
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
    setFlag(QGraphicsItem::ItemStacksBehindParent, true);
    setAcceptHoverEvents(true);
}

void DefectPreviewItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsPixmapItem::hoverEnterEvent(event);
    emit hoverEnterEvent(true);
}

void DefectPreviewItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QGraphicsPixmapItem::hoverLeaveEvent(event);
    emit hoverEnterEvent(false);
}


