#include "transparentscrollarea.h"

TransparentScrollArea::TransparentScrollArea(QWidget *parent) :
    QScrollArea(parent)
{
    //setStyleSheet(" background-color:rgba(130,130,130,0); ");
    setWindowFlags( Qt::FramelessWindowHint);
}


void TransparentScrollArea::resizeEvent(QResizeEvent *e)
{
    if(e->oldSize() != e->size())
    {
        QScrollArea::resizeEvent(e);
        emit signalSizeUpdated();
    }
    return;
}

void TransparentScrollArea::showEvent(QShowEvent *e)
{
    QScrollArea::showEvent(e);
    emit signalSizeUpdated();
}


void TransparentScrollArea::hideEvent(QHideEvent *e)
{
    QScrollArea::hideEvent(e);
    emit signalSizeUpdated();
}
