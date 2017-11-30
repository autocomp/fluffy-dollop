#include "elementpropertyform.h"
#include <QDebug>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>

ElementPropertyForm::ElementPropertyForm(floor_graph_maker::FloorGraphElement *element)
    : _elementId(element->id())
    , _timer(this)
{
    setWindowOpacity(0);
    setWindowFlags(Qt::Widget | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Popup); // Qt::ToolTip

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setMargin(3);
    vLayout->setSpacing(3);

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    hLayout->setMargin(3);
    hLayout->setSpacing(3);
    vLayout->addLayout(hLayout);

    QLabel * tl = new QLabel(this);
    hLayout->addWidget(tl);

    hLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

    _deleteButton = new QPushButton(this);
    _deleteButton->setFixedSize(32,32);
    _deleteButton->setIcon(QIcon(":/img/icon_delete.png"));
    connect(_deleteButton, SIGNAL(pressed()), this, SLOT(slotDeleteElement()));
    hLayout->addWidget(_deleteButton);

    connect(&_timer, SIGNAL(timeout()), this, SLOT(slotCheckOpacity()));
    _timer.start(100);

    floor_graph_maker::NodeElement * nodeElement = dynamic_cast<floor_graph_maker::NodeElement *>(element);
    if(nodeElement)
    {
        tl->setText(QString::fromUtf8("Удалить вершину"));
    }

    floor_graph_maker::EdgeElement * edgeElement = dynamic_cast<floor_graph_maker::EdgeElement *>(element);
    if(edgeElement)
    {
        tl->setText(QString::fromUtf8("Удалить грань"));
        vLayout->addSpacerItem(new QSpacerItem(0, 5, QSizePolicy::Minimum, QSizePolicy::Fixed));

        QGroupBox * box = new QGroupBox(QString::fromUtf8("Тип грани"), this);
        vLayout->addWidget(box);
        QVBoxLayout *subVLayout = new QVBoxLayout(box);
        subVLayout->setMargin(3);
        subVLayout->setSpacing(3);

        _wallRB = new QRadioButton(QString::fromUtf8("стена"), this);
        subVLayout->addWidget(_wallRB);
        _doorRB = new QRadioButton(QString::fromUtf8("дверь"), this);
        subVLayout->addWidget(_doorRB);
        _windowRB = new QRadioButton(QString::fromUtf8("окно"), this);
        subVLayout->addWidget(_windowRB);

        QHBoxLayout *subHLayout = new QHBoxLayout(this);
        subHLayout->setMargin(3);
        subHLayout->setSpacing(3);
        subVLayout->addLayout(subHLayout);

        _wallWidthTL = new QLabel(QString::fromUtf8("Толщина стены (м)"), this);
        subHLayout->addWidget(_wallWidthTL);

        subHLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

        _wallWidth = new QDoubleSpinBox(this);
        _wallWidth->setMinimum(0.1);
        _wallWidth->setMaximum(3);
        _wallWidth->setSingleStep(0.1);
        _wallWidth->setDecimals(1);
        _wallWidth->setValue(edgeElement->getWallWidth());
        subHLayout->addWidget(_wallWidth);

        switch(edgeElement->getEdgeType())
        {
        case floor_graph_maker::EdgeElement::WALL :
            _wallRB->setChecked(true);
            break;
        case floor_graph_maker::EdgeElement::DOOR :
            _wallWidthTL->setDisabled(true);
            _wallWidth->setDisabled(true);
            _doorRB->setChecked(true);
            break;
        case floor_graph_maker::EdgeElement::WINDOW :
            _wallWidthTL->setDisabled(true);
            _wallWidth->setDisabled(true);
            _windowRB->setChecked(true);
            break;
        }

        connect(_wallWidth, SIGNAL(valueChanged(double)), this, SLOT(slotWallWidthChanged(double)));
        connect(_wallRB, SIGNAL(clicked(bool)), this, SLOT(slotEdgeTypeChanged(bool)));
        connect(_doorRB, SIGNAL(clicked(bool)), this, SLOT(slotEdgeTypeChanged(bool)));
        connect(_windowRB, SIGNAL(clicked(bool)), this, SLOT(slotEdgeTypeChanged(bool)));
    }
}

ElementPropertyForm::~ElementPropertyForm()
{
//    delete ui;
}

floor_graph_maker::EdgeElement::EdgeType ElementPropertyForm::getChoiceType()
{
    if(_doorRB->isChecked())
    {
        _wallWidthTL->setDisabled(true);
        _wallWidth->setDisabled(true);
        return floor_graph_maker::EdgeElement::DOOR;
    }

    if(_windowRB->isChecked())
    {
        _wallWidthTL->setDisabled(true);
        _wallWidth->setDisabled(true);
        return floor_graph_maker::EdgeElement::WINDOW;
    }

    _wallWidthTL->setDisabled(false);
    _wallWidth->setDisabled(false);
    return floor_graph_maker::EdgeElement::WALL;
}

void ElementPropertyForm::slotCheckOpacity()
{
    _opacity += 0.1;
    if(_opacity < 0.95)
        setWindowOpacity(_opacity);
    else
        _timer.stop();
}

void ElementPropertyForm::slotDeleteElement()
{
    emit signalRemoveElement(_elementId);
}

void ElementPropertyForm::slotEdgeTypeChanged(bool on_off)
{
    if(on_off)
        emit signalEdgeTypeChanged(_elementId, getChoiceType());
}

void ElementPropertyForm::slotWallWidthChanged(double val)
{
    emit signalWallWidthChanged(_elementId, val);
}





















