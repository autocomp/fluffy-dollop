#include "pdfpageslistwidget.h"
#include<QResizeEvent>

PdfPagesListWidget::PdfPagesListWidget()
    : _coef(1)
{
    setViewMode(QListView::IconMode);
    setFlow(QListView::TopToBottom);
    setWrapping(false);
    //setIconSize(QSize(200,100));

    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragDropMode(QAbstractItemView::DragOnly);
    setDragEnabled(true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(slotSelectionChanged()));
}

void PdfPagesListWidget::addImage(int pageNumber, const QImage &img)
{
    _coef = (double)img.height() / (double)img.width();
    QListWidgetItem * item = new QListWidgetItem(QIcon(QPixmap::fromImage(img)), QString(""), this);
    item->setData(Qt::UserRole, pageNumber);
}

void PdfPagesListWidget::resizeEvent(QResizeEvent * e)
{
    int W = e->size().width();
    _iconSize = QSize(W-2, (W * _coef)-2);
    setIconSize(_iconSize);
    QListWidget::resizeEvent(e);
}

void PdfPagesListWidget::slotSelectionChanged()
{
    if(selectedItems().isEmpty())
    {
        emit setPage(-1);
    }
    else
    {
        QListWidgetItem * item = selectedItems().first();
        emit setPage(item->data(Qt::UserRole).toInt());
    }
}
