#ifndef LAYERINSTRUMENTALFORM_H
#define LAYERINSTRUMENTALFORM_H

#include <QWidget>
#include <QLabel>
#include <QMouseEvent>

namespace Ui {
class LayerInstrumentalForm;
}

namespace pixmap_transform_state {
class PixmapTransformState;
}

class LayerInstrumentalForm : public QWidget
{
    Q_OBJECT
    
public:
    LayerInstrumentalForm(uint visualizerId, const QPixmap &pixmap, bool onTop);
    LayerInstrumentalForm(uint visualizerId, const QPixmap &pixmap, QPointF scenePos, double scaleX, double scaleY, double rotate, bool onTop);
    ~LayerInstrumentalForm();
    void setEmbeddedWidgetId(quint64 id);

signals:
    void signalSaved(QString filePath, QPointF scenePos, double scaleW, double scaleH, double rotate);

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
    void pixmapChanged();
    void undoAction();
    void save();
    
private:
    void init();
    void changeColorRBdependElementsEnabled(bool on_off);

    quint64 _embeddedWidgetId = 0;
    uint _visualizerId;
    Ui::LayerInstrumentalForm *ui;
    QColor _colorIn, _colorOut;
    QSharedPointer<pixmap_transform_state::PixmapTransformState> _pixmapTransformState;
    bool _changed = false;
};


#endif // LAYERINSTRUMENTALFORM_H
