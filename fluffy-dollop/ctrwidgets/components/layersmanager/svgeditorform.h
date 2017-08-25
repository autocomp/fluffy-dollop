#ifndef SVGEDITORFORM_H
#define SVGEDITORFORM_H

#include "svgeditortypes.h"
#include <QWidget>
#include <libembeddedwidgets/embeddedapp.h>

namespace Ui {
class SvgEditorForm;
}

namespace svg_editor_state {
class SvgEditorState;
}

class SvgEditorForm : public QWidget, public ew::EmbeddedSubIFace
{
    Q_OBJECT
    
public:
    explicit SvgEditorForm(uint visualizerId, bool onTop);
    ~SvgEditorForm();

    QWidget *getWidget();
    void closed(bool *acceptFlag);

signals:
    void signalClosed();
    void svgDocSaved(QString filePath, QPointF scenePos);

private slots:
    void slotChangeMode(bool on_off);
    void slotFill(bool on_off);
    void slotDeleteLayer();
    void slotSaveSvgDoc();
    void slotSetLineColor();
    void slotSetFillColor();
    void slotLineWidhChanged(int val);
    void slotOpasityChanged(int percent);
    void slotSetEnabledDeleteButton(bool on_off);
    void slotSetEnabledSaveButton(bool on_off);
    void slotMakeAdjustForm();
    void slotLineStyleChanged(int);
    void slotSetFont();
    void slotGetDefaultStrokeParams(svg_editor_state::StrokeParams & strokeParams);
    
private:
    Ui::SvgEditorForm *ui;
    QSharedPointer<svg_editor_state::SvgEditorState> _svgEditorState;
    uint _visualizerId;
    svg_editor_state::StrokeParams _strokeDefaultParams;
};

#endif // SVGEDITORFORM_H
