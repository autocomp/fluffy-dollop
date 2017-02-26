#ifndef PDFEDITORVIEW_H
#define PDFEDITORVIEW_H

#include <QGraphicsView>
#include <QGraphicsPixmapItem>

class PdfEditorView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit PdfEditorView(QString savePath);
    ~PdfEditorView();

signals:
    
public slots:
    void setPage(int pageNumber);
    void zoomIn();
    void zoomOut();
    void zoomReset();

protected:
    virtual void wheelEvent(QWheelEvent* e);
    virtual void mousePressEvent(QMouseEvent* e);

private:
    void syncItems();

    QString _savePath;
    QList<QGraphicsPixmapItem *> _listItems;
    int _zoom = 1;
    //int _zoomMin = 1;
    double _scale = 1;
    
};

#endif // PDFEDITORVIEW_H
