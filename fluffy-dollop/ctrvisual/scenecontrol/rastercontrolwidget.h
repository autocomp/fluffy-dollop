#ifndef RASTERCONTROLWIDGET_H
#define RASTERCONTROLWIDGET_H

#include <ctrcore/plugin/embifacenotifier.h>
#include <QWidget>
#include <QStringList>
#include <QSlider>
#include <QToolButton>
#include <QLabel>

namespace visualize_system {

class ParallelCursor;

class RasterControlWidget : public QWidget
{
    Q_OBJECT

public:
    RasterControlWidget(uint visualizerId, QList<uint> rasters, ParallelCursor* parallelCursor);
    ~RasterControlWidget();

protected:
    void check();

protected slots:
//    void slotValueChanged(int);
//    void slotSetTransparency(bool);
    void slotDown();
    void slotUp();
    void slotDelete();
    void slotSetMark(bool on_off);
    void widgetClose();

private:
    const uint _visualizerId;
    QList<uint> _rasters;
    uint _currentRaster;
    ParallelCursor* _parallelCursor;
    EmbIFaceNotifier* _iface;
    QToolButton * _tbDown, * _tbUp, * _tbMark, * _tbDelete;
    QLabel * _tlInfo;

signals:
    void signalSetVisibleForRaster(uint rasterShow, uint rasterHide);
    void signalRasterControlWidgetClosed();
};

}

#endif









