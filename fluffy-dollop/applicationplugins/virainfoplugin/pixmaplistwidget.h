#ifndef PIXMAPLISTWIDGET_H
#define PIXMAPLISTWIDGET_H

#include <QListWidget>

class PixmapListWidget : public QListWidget
{
    Q_OBJECT
public:
    PixmapListWidget();
    void addItem(const QPixmap& pm);

protected slots:
    void slotItemDoubleClicked(QListWidgetItem* item);

protected:
    QList<QPixmap> _pixmaps;


};

#endif // PIXMAPLISTWIDGET_H


