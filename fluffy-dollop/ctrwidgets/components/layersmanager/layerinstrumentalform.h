#ifndef LAYERINSTRUMENTALFORM_H
#define LAYERINSTRUMENTALFORM_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QPolygonF>
#include "commontypes.h"

namespace Ui {
class LayerInstrumentalForm;
}

namespace transform_state {
class TransformingState;
}

struct TransformingItemSaveDatad
{
   QString filePath;
   QPointF scenePos;
   double scaleW = 1;
   double scaleH = 1;
   double rotate = 0;
   QPolygonF vertex;
   double pixmapWidth = 0;
   double pixmapHeight = 0;
};

class LayerInstrumentalForm : public QWidget
{
    Q_OBJECT
    
public:
    LayerInstrumentalForm(uint visualizerId, const QPixmap &pixmap, const QString &rasterFileBaseName, int zValue);
    LayerInstrumentalForm(uint visualizerId, const QPixmap &pixmap, const QString &rasterFileBaseName, QPointF scenePos, double scaleX, double scaleY, double rotate, int zValue);

    LayerInstrumentalForm(uint visualizerId, const QPolygonF &polygon, const QRectF &bRectOnMapScene, const QTransform &transformer, int zValue);

    LayerInstrumentalForm(uint visualizerId, const QString &svgFilePath, int zValue);
    LayerInstrumentalForm(uint visualizerId, const QString &svgFilePath, QPointF scenePos, double scaleX, double scaleY, double rotate, int zValue);

    ~LayerInstrumentalForm();
    void setEmbeddedWidgetId(quint64 id);
    void setGlobalMode(transform_state::GlobalMode mode);

signals:
    void signalTransformingItemSaved(TransformingItemSaveDatad data);
    void signalPolygonSaved(QPolygonF);

private slots:
    void setMode(bool);
    void setColorMode(bool);
    void pressSetColor();
    void makeAdjustForm();
    void setOpacityValue(int val);
    void apply();
    void GetColorOnImage(bool on_off);
    void setColorOnImage(QColor color);
    void setArea(bool on_off);
    void areaSetted();
    void clearArea();
    void applyForImageOrArea(bool on_off);
    void itemChanged();
    void undoAction();
    void save();
    
private:
    void init();
    void changeColorRBdependElementsEnabled(bool on_off);
    void inscribeSizeToGeoViewport(const QSize &srcSize, QPointF &pos, double &originalScale);

    quint64 _embeddedWidgetId = 0;
    uint _visualizerId;
    Ui::LayerInstrumentalForm *ui;
    QColor _colorIn, _colorOut;
    QSharedPointer<transform_state::TransformingState> _transformingState;
    bool _changed = false;
};


#endif // LAYERINSTRUMENTALFORM_H
