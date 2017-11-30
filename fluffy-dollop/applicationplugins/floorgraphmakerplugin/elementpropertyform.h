#ifndef ELEMENTPROPERTYFORM_H
#define ELEMENTPROPERTYFORM_H

#include <QWidget>
#include <QTimer>
#include <QPushButton>
#include <QRadioButton>
#include <QDoubleSpinBox>
#include <QLabel>
#include "floorgraphelement.h"
#include "floorgraphtypes.h"

class ElementPropertyForm : public QWidget
{
    Q_OBJECT

public:
    explicit ElementPropertyForm(floor_graph_maker::FloorGraphElement * element);
    ~ElementPropertyForm();
    floor_graph_maker::EdgeProperty getChoiceType();

signals:
    void signalRemoveElement(uint id);
    //void signalSetEdgeProperty(uint,EdgeProperty);

private slots:
//    void slotCheckOpacity();
    void slotDeleteElement();
    void slotEdgeTypeChanged(bool on_off);
    void slotWallWidthChanged(double val);

private:
    const uint _elementId;
//    QTimer _timer;
    double _opacity = 0;
    QPushButton * _deleteButton = nullptr;
    QRadioButton * _wallRB = nullptr, * _doorRB = nullptr, * _windowRB = nullptr;
    QLabel * _wallWidthTL = nullptr, * _wallHeightTL = nullptr, * _windowHeightUnderFloorTL = nullptr, * _windowHeightTL = nullptr;
    QDoubleSpinBox * _wallWidth = nullptr, * _wallHeight = nullptr, * _windowHeightUnderFloor = nullptr, * _windowHeight = nullptr;
};

#endif // ELEMENTPROPERTYFORM_H
