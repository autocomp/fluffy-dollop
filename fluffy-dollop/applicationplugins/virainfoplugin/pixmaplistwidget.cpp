#include "pixmaplistwidget.h"
#include "imageviewer.h"
#include <QPainter>
#include <QDebug>

PixmapListWidget::PixmapListWidget()
{
    setViewMode(QListView::IconMode);
    setFlow(QListView::LeftToRight);
    setWrapping(false);
    setIconSize(QSize(200,200));
    setFixedHeight(220);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    connect(this, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(slotItemDoubleClicked(QListWidgetItem*)));
}

void PixmapListWidget::addItem(const QPixmap &pm)
{
    _pixmaps.append(pm);
    QPixmap _pm(200,200);
    _pm.fill(QColor(50,50,50));
    QPainter pr(&_pm);
    QPixmap scaledPm = pm.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    int x = (200 - scaledPm.width())/2;
    int y = (200 - scaledPm.height())/2;
    pr.drawPixmap(x, y, scaledPm);

    QListWidgetItem * item = new QListWidgetItem(QIcon(_pm), "", this);
    item->setData(Qt::UserRole, _pixmaps.size()-1);
}

void PixmapListWidget::slotItemDoubleClicked(QListWidgetItem *item)
{
    if(!item)
        return;

    int index = item->data(Qt::UserRole).toInt();
    ImageViewer imageViewer(_pixmaps, index);
    imageViewer.showImageViewer();
}

