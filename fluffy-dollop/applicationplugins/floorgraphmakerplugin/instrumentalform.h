#ifndef INSTRUMENTALFORM_H
#define INSTRUMENTALFORM_H

#include <QWidget>
#include <libembeddedwidgets/embeddedapp.h>
#include "floorgraphtypes.h"

namespace Ui {
class InstrumentalForm;
}

namespace floor_graph_maker
{
class FloorGraphMakerState;

class InstrumentalForm : public QWidget, public ew::EmbeddedSubIFace
{
    Q_OBJECT

public:
    InstrumentalForm(uint visualizerId, uint64_t floorId);
    InstrumentalForm(uint elementId, ElementType elementType);
    ~InstrumentalForm();
    QWidget *getWidget();
    void closed(bool *acceptFlag);
    void setCurrentElementType(ElementType elementType);
    void setCurrentProperty(EdgeProperty property);

signals:
    void signalClosed();
    void signalEdgeStateChanged(uint elementId, EdgeProperty property);
    void signalRemoveElement(uint elementId);

private slots:
    void slotSetNodeOrEdge(bool on_off);
    void slotSave();
    void slotRemoveElement();
    void slotMakeAdjustForm();
    void slotEdgeTypeChanged(bool on_off);
    void doubleValueChanged(double);
    void intValueChanged(int);

private:
    void connectInit();
    void sendCurrentProperty();

    Ui::InstrumentalForm *ui;
    QSharedPointer<floor_graph_maker::FloorGraphMakerState> _floorGraphMakerState;
    uint _elementId = 0;


};

}

#endif // INSTRUMENTALFORM_H
