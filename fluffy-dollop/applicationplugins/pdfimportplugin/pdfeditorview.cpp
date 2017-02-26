#include "pdfeditorview.h"
#include <QWheelEvent>
#include <QMouseEvent>
#include <QDir>

PdfEditorView::PdfEditorView(QString savePath)
    : _savePath(savePath)
{
    setDragMode(QGraphicsView::ScrollHandDrag);
}

PdfEditorView::~PdfEditorView()
{
    foreach(QGraphicsPixmapItem * item, _listItems)
        delete item;
    _listItems.clear();
}

void PdfEditorView::setPage(int pageNumber)
{
    zoomReset();

    foreach(QGraphicsPixmapItem * item, _listItems)
        delete item;
    _listItems.clear();

    QDir dir(_savePath + QDir::separator() + QString::number(pageNumber));
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Files);
    //_zoomMax = fileInfoList.size();
    foreach(QFileInfo fi, fileInfoList)
    {
        int Z = fi.baseName().toInt();
        QPixmap pm(fi.absoluteFilePath());
        QGraphicsPixmapItem * item = new QGraphicsPixmapItem(pm, 0, scene());
        item->setZValue(Z);
        switch(Z)
        {
        case 0 :
            item->setTransform(QTransform().scale(8,8));
            break;
        case 1 :
            item->setTransform(QTransform().scale(4,4));
            break;
        case 2 :
            item->setTransform(QTransform().scale(2,2));
            break;
        case 3 :
            // item->setTransform(QTransform().scale(1,1));
            break;
        }
        _listItems.append(item);
    }
}

void PdfEditorView::syncItems()
{
    if(_listItems.size() < 4)
        return;

    foreach(QGraphicsPixmapItem * item, _listItems)
        item->setVisible(false);

    switch(_zoom)
    {
    case 3 :
    case 2 :
    case 1 :
        _listItems.at(3)->setVisible(true);
        break;
    case 0 :
        _listItems.at(2)->setVisible(true);
        break;
    case -1 :
        _listItems.at(1)->setVisible(true);
        break;
    case -2 :
    default:
        _listItems.at(0)->setVisible(true);
    }
}

void PdfEditorView::wheelEvent(QWheelEvent *e)
{
    if(e->delta() > 0)
        zoomIn();
    else
        zoomOut();
    //emit signalZoomChanged();
}

void PdfEditorView::mousePressEvent(QMouseEvent *e)
{
    QGraphicsView::mousePressEvent(e);
    if(e->button() & Qt::MiddleButton)
        zoomReset();
}

void PdfEditorView::zoomReset()
{
    resetTransform();
    _scale = 1;
    _zoom = 1;
    syncItems();
}

void PdfEditorView::zoomIn()
{
    if(_zoom < 3)
    {
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        _scale *= 2;
        scale(2,2);
        ++_zoom;
        syncItems();
    }
}

void PdfEditorView::zoomOut()
{
    if(_zoom > -2)
    {
        setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
        _scale /= 2;
        scale(0.5,0.5);
        --_zoom;
        syncItems();
    }
}
