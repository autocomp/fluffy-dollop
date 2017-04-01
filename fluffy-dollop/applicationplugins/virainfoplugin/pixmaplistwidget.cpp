#include "pixmaplistwidget.h"
#include <QResizeEvent>

PixmapListWidget::PixmapListWidget()
{
    setViewMode(QListView::IconMode);
    setFlow(QListView::LeftToRight);
    setWrapping(false);
    setIconSize(QSize(200,100));

    setSelectionMode(QAbstractItemView::SingleSelection);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

void PixmapListWidget::addItem(const QPixmap &pm)
{
    _coef = (double)pm.height() / (double)pm.width();
    QListWidgetItem * item = new QListWidgetItem(QIcon(pm), "", this);
}

void PixmapListWidget::resizeEvent(QResizeEvent *e)
{
    int H = e->size().height();
    _iconSize = QSize(H/_coef - 25, H - 25);
    setIconSize(_iconSize);
    QListWidget::resizeEvent(e);
}
