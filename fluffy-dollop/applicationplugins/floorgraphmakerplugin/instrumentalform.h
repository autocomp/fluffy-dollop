#ifndef INSTRUMENTALFORM_H
#define INSTRUMENTALFORM_H

#include <QWidget>
#include <libembeddedwidgets/embeddedapp.h>

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
    explicit InstrumentalForm(uint visualizerId);
    ~InstrumentalForm();

    QWidget *getWidget();
    void closed(bool *acceptFlag);

signals:
    void signalClosed();

private slots:
    void slotSetNode(bool on_off);
    void slotSetEdge(bool on_off);
    void slotSetDoor(bool on_off);
    void slotSetWindow(bool on_off);
    void slotMakeAdjustForm();

private:
    Ui::InstrumentalForm *ui;
    QSharedPointer<floor_graph_maker::FloorGraphMakerState> _floorGraphMakerState;
    uint _visualizerId;

    void uncheckedButtons(QObject * button);

};

}

#endif // INSTRUMENTALFORM_H