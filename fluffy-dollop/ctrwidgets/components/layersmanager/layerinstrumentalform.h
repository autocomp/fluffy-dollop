#ifndef LAYERINSTRUMENTALFORM_H
#define LAYERINSTRUMENTALFORM_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>
#include <QPolygonF>

namespace Ui {
class LayerInstrumentalForm;
}

namespace transform_state {
class TransformingState;
}

struct RasterSaveDatad
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
    LayerInstrumentalForm(uint visualizerId, const QPixmap &pixmap, int zValue);
    LayerInstrumentalForm(uint visualizerId, const QPixmap &pixmap, QPointF scenePos, double scaleX, double scaleY, double rotate, int zValue);

    LayerInstrumentalForm(uint visualizerId, const QPolygonF &polygon, int zValue);

    ~LayerInstrumentalForm();
    void setEmbeddedWidgetId(quint64 id);
    void setModeMoveAndRotateOnly();

signals:
    void signalRasterSaved(RasterSaveDatad data);
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

    quint64 _embeddedWidgetId = 0;
    uint _visualizerId;
    Ui::LayerInstrumentalForm *ui;
    QColor _colorIn, _colorOut;
    QSharedPointer<transform_state::TransformingState> _transformingState;
    bool _changed = false;
};


#endif // LAYERINSTRUMENTALFORM_H
