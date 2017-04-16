#include "imageviewer.h"
#include <dmanager/embeddedstruct.h>
#include <dmanager/embeddedapp.h>
#include <QDebug>
#include <QHBoxLayout>

ImageViewer::ImageViewer(const QList<QPixmap> &pixmaps, int currIndex)
    : _pixmaps(pixmaps)
    , _currIndex(currIndex)
{
    setRenderHint(QPainter::SmoothPixmapTransform, true);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setScene(&_scene);

    setResizeAnchor(ViewportAnchor::AnchorViewCenter);
    setDragMode(DragMode::ScrollHandDrag);

    _prevImgButton = new QToolButton();
    _prevImgButton->setShortcut(QKeySequence(Qt::Key_Left));
    _prevImgButton->setIcon(QIcon(":/img/left.png"));
    _prevImgButton->setFixedSize(32,32);
    _prevImgButton->setStyleSheet("border-radius:4;border-color: rgb(255, 255, 255);");
    connect(_prevImgButton, SIGNAL(clicked()), this, SLOT(slotPrevImg()));

    _nextImgButton = new QToolButton();
    _nextImgButton->setShortcut(QKeySequence(Qt::Key_Right));
    _nextImgButton->setIcon(QIcon(":/img/right.png"));
    _nextImgButton->setFixedSize(32,32);
    _nextImgButton->setStyleSheet("border-radius:4;border-color: rgb(255, 255, 255);");
    connect(_nextImgButton, SIGNAL(clicked()), this, SLOT(slotNextImg()));

    _zoomPlusButton = new QToolButton();
    _zoomPlusButton->setShortcut(QKeySequence(Qt::Key_Up));
    _zoomPlusButton->setIcon(QIcon(":/img/icons_plus.png"));
    _zoomPlusButton->setFixedSize(32,32);
    _zoomPlusButton->setStyleSheet("border-radius:4;border-color: rgb(255, 255, 255);");
    connect(_zoomPlusButton, SIGNAL(clicked()), this, SLOT(slotZoomPlus()));

    _zoomPrefButton = new QToolButton();
    _zoomPrefButton->setShortcut(QKeySequence(Qt::Key_Space));
    _zoomPrefButton->setIcon(QIcon(":/img/image_perf_size.png"));
    _zoomPrefButton->setFixedSize(32,32);
    _zoomPrefButton->setCheckable(true);
    _zoomPrefButton->setChecked(true);
    _zoomPrefButton->setStyleSheet("border-radius:4;border-color: rgb(255, 255, 255);");
    connect(_zoomPrefButton, SIGNAL(clicked(bool)), this, SLOT(slotZoomPref(bool)));

    _zoomMinusButton = new QToolButton();
    _zoomMinusButton->setShortcut(QKeySequence(Qt::Key_Down));
    _zoomMinusButton->setIcon(QIcon(":/img/icons_minus.png"));
    _zoomMinusButton->setFixedSize(32,32);
    _zoomMinusButton->setStyleSheet("border-radius:4;border-color: rgb(255, 255, 255);");
    connect(_zoomMinusButton, SIGNAL(clicked()), this, SLOT(slotZoomMinus()));

    QVBoxLayout* vLeftLayout = new QVBoxLayout;
    vLeftLayout->addStretch();
    vLeftLayout->addWidget(_prevImgButton);
    vLeftLayout->setAlignment( _prevImgButton, Qt::AlignLeft );
    vLeftLayout->addStretch();

    QVBoxLayout* vRightLayout = new QVBoxLayout;
    vRightLayout->addStretch();
    vRightLayout->addWidget(_nextImgButton);
    vRightLayout->setAlignment( _nextImgButton, Qt::AlignRight );
    vRightLayout->addStretch();

    QHBoxLayout* hLayout = new QHBoxLayout;
    hLayout->addLayout(vLeftLayout);

    hLayout->addStretch();
    hLayout->addWidget(_zoomPlusButton);
    hLayout->setAlignment( _zoomPlusButton, Qt::AlignTop );
    hLayout->addWidget(_zoomPrefButton);
    hLayout->setAlignment( _zoomPrefButton, Qt::AlignTop );
    hLayout->addWidget(_zoomMinusButton);
    hLayout->setAlignment( _zoomMinusButton, Qt::AlignTop );
    hLayout->addStretch();

    hLayout->addLayout(vRightLayout);

    setLayout(hLayout);
}

ImageViewer::~ImageViewer()
{
    if(_ifaceInfoWidget)
    {
        ewApp()->setVisible(_ifaceInfoWidget->id(), false);
        delete _ifaceInfoWidget;
    }
}

void ImageViewer::showImageViewer()
{
    if(_currIndex < 0 || _currIndex >= _pixmaps.size())
        return;

    if(_pixmaps.size() < 2)
    {
        _nextImgButton->hide();
        _prevImgButton->hide();
    }
    else
    {
        if((_currIndex-1) < 0 || (_currIndex-1) >= _pixmaps.size())
            _prevImgButton->setDisabled(true);
        if((_currIndex+1) < 0 || (_currIndex+1) >= _pixmaps.size())
            _nextImgButton->setDisabled(true);
    }

    reloadImg();

    if( !_ifaceInfoWidget )
    {
        _ifaceInfoWidget = new EmbIFaceNotifier(this);
        QString tag = QString("ViraStatusBar_ImageViewer");
        quint64 widgetId = ewApp()->restoreWidget(tag, _ifaceInfoWidget);
        if(0 == widgetId)
        {
            ew::EmbeddedWidgetStruct struc;
            ew::EmbeddedHeaderStruct headStr;
            headStr.hasCloseButton = true;
            headStr.hasMinMaxButton = true;
            headStr.hasCollapseButton = false;
            headStr.headerPixmap = ":/img/img_icon.png";
            headStr.windowTitle = QString::fromUtf8("Просмотр изображений");
            struc.header = headStr;
            struc.iface = _ifaceInfoWidget;
            struc.widgetTag = tag;
            struc.minSize = QSize(300,300);
            struc.topOnHint = true;
            struc.isModal = true;
            ewApp()->createWidget(struc);
        }
    }
    else
    {
        ewApp()->setVisible(_ifaceInfoWidget->id(), true);
    }
}

void ImageViewer::slotPrevImg()
{
    if((_currIndex-1) < 0 || (_currIndex-1) >= _pixmaps.size())
        return;

    _nextImgButton->setDisabled(false);
    --_currIndex;
    reloadImg();

    if((_currIndex-1) < 0 || (_currIndex-1) >= _pixmaps.size())
        _prevImgButton->setDisabled(true);
}

void ImageViewer::slotNextImg()
{
    if((_currIndex+1) < 0 || (_currIndex+1) >= _pixmaps.size())
        return;

    _prevImgButton->setDisabled(false);
    ++_currIndex;
    reloadImg();

    if((_currIndex+1) < 0 || (_currIndex+1) >= _pixmaps.size())
        _nextImgButton->setDisabled(true);
}

void ImageViewer::slotZoomPlus()
{
    _zoomPrefButton->setChecked(false);
    double newScale = _currScale * 1.1;
    if(newScale < 10)
    {
        _currScale = newScale;
        resetMatrix();
        scale(_currScale,_currScale);
    }
}

void ImageViewer::slotZoomMinus()
{
    _zoomPrefButton->setChecked(false);
    double newScale = _currScale * 0.9;
    if(newScale > 0.025)
    {
        _currScale *= 0.9;
        resetMatrix();
        scale(_currScale,_currScale);
    }
}

void ImageViewer::slotZoomPref(bool on_off)
{
    if(on_off)
        setPerfScale();
}

void ImageViewer::reloadImg()
{
    if(! _pixmapItem)
    {
        _pixmapItem = new QGraphicsPixmapItem;
        _scene.addItem(_pixmapItem);
    }
    _pixmapItem->setPixmap(_pixmaps.at(_currIndex));
    _currPixmapSize = _pixmaps.at(_currIndex).size();
    setSceneRect(0, 0, _currPixmapSize.width(), _currPixmapSize.height());

    _zoomPrefButton->setChecked(true);
    setPerfScale();
}

void ImageViewer::setPerfScale()
{
    double coef_W = (double)_currViewSize.width() / _currPixmapSize.width();
    double coef_H = (double)_currViewSize.height() / _currPixmapSize.height();
    if(coef_W < coef_H)
        _currScale = coef_W;
    else
        _currScale = coef_H;
    if(_currScale > 1)
        _currScale = 1;
    resetMatrix();
    scale(_currScale,_currScale);
}

void ImageViewer::resizeEvent(QResizeEvent *e)
{
    QGraphicsView::resizeEvent(e);
    _currViewSize = e->size();
    if(_zoomPrefButton->isChecked())
        setPerfScale();
}

void ImageViewer::wheelEvent(QWheelEvent *e)
{
    setResizeAnchor(ViewportAnchor::AnchorUnderMouse);
    QGraphicsView::wheelEvent(e);
    if(e->delta() > 0)
    {
        slotZoomPlus();
    }
    else
    {
        slotZoomMinus();
    }
    setResizeAnchor(ViewportAnchor::AnchorViewCenter);
}

void ImageViewer::mousePressEvent(QMouseEvent *e)
{
    QGraphicsView::mousePressEvent(e);
    if(e->button() & Qt::MiddleButton)
        if(_zoomPrefButton->isChecked() == false)
        {
            _currScale = 1;
            resetMatrix();
            scale(_currScale,_currScale);
        }
}



















