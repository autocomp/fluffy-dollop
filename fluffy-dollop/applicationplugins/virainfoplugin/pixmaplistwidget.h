#ifndef PIXMAPLISTWIDGET_H
#define PIXMAPLISTWIDGET_H

#include <QListWidget>

class PixmapListWidget : public QListWidget
{
public:
    PixmapListWidget();
    void addItem(const QPixmap& pm);

protected:
    virtual void resizeEvent(QResizeEvent * e);

private:
    QSize _iconSize;
    double _coef = 1;
};

#endif // PIXMAPLISTWIDGET_H


