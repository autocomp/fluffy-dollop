#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPixmap>
#include <QList>
#include <QShortcut>
#include <QGraphicsPixmapItem>
#include <QToolButton>
#include <ctrcore/plugin/embifacenotifier.h>

class ImageViewer : public QGraphicsView
{
    Q_OBJECT

public:
    ImageViewer(const QList<QPixmap>& pixmaps, int currIndex = 0);
    ~ImageViewer();
    void showImageViewer(QString widgetTitle = QString());

protected slots:
    void slotPrevImg();
    void slotNextImg();
    void slotZoomPlus();
    void slotZoomMinus();
    void slotZoomPref(bool on_off);

protected:
    void reloadImg();
    void setPerfScale();
    virtual void resizeEvent(QResizeEvent *event);
    virtual void wheelEvent(QWheelEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);

    QGraphicsScene _scene;
    EmbIFaceNotifier* _ifaceInfoWidget = nullptr;
    const QList<QPixmap>& _pixmaps;
    int _currIndex;
    QSize _currPixmapSize, _currViewSize;
    double _currScale = 1;
    QGraphicsPixmapItem * _pixmapItem = nullptr;
    QToolButton * _nextImgButton, * _prevImgButton, * _zoomPlusButton, * _zoomMinusButton, * _zoomPrefButton;
//    QShortcut * prevImg, * nextImg;
};

#endif // IMAGEVIEWER_H
