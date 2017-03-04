#ifndef PDFPAGESLISTWIDGET_H
#define PDFPAGESLISTWIDGET_H

#include <QListWidget>

class PdfPagesListWidget : public QListWidget
{
    Q_OBJECT
public:
    PdfPagesListWidget();
    void addImage(int pageNumber, const QImage& img);

signals:
    void setPage(int pageNumber);

protected:
    virtual void resizeEvent(QResizeEvent * e);

private slots:
    void slotSelectionChanged();

private:
    QSize _iconSize;
    double _coef;

};

#endif // PDFPAGESLISTWIDGET_H
