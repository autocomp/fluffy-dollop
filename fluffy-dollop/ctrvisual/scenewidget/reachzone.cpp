#include "reachzone.h"

ReachZone::ReachZone(QWidget *parent) :
    QWidget(parent)
{
    QWidget::setWindowFlags(Qt::FramelessWindowHint);
    setFixedSize(205,140);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    setGeometry(100,100,this->width(), this->height());

    QHBoxLayout * panel = new QHBoxLayout();
    QVBoxLayout * listpanel = new QVBoxLayout(this);
    listpanel->setMargin(1);
    listpanel->setSpacing(1);

    QLabel *label1 = new QLabel(tr("longitude"));
    QLabel *label2 = new QLabel(tr("latitude"));
    QLabel *label3 = new QLabel(tr("height"));
    QLabel *label4 = new QLabel(tr("radius"));

    label1->setFixedWidth(70);
    label2->setFixedWidth(70);
    label3->setFixedWidth(70);
    label4->setFixedWidth(70);

    te1.setFixedWidth(132);
    te2.setFixedWidth(132);
    te3.setFixedWidth(132);
    te4.setFixedWidth(132);

    QPushButton *btn = new QPushButton(tr("apply"));
    btn->setFixedSize(203,25);

    QFormLayout *l1 = new QFormLayout;
    l1->addRow(label1, &te1);
    l1->addRow(label2, &te2);
    l1->addRow(label3, &te3);
    l1->addRow(label4, &te4);
    l1->setAlignment(label1, Qt::AlignHCenter);
    l1->setAlignment(label2, Qt::AlignHCenter);
    l1->setAlignment(label3, Qt::AlignHCenter);
    l1->setAlignment(label4, Qt::AlignHCenter);

    listpanel->addLayout(l1);
    listpanel->addWidget(btn);

//    QString stylelabel("border:1px solid lightgray;"
//                  "background:rgb(255, 255, 255);"
//                  "selection-background-color:rgb(174,174,174);");

//    label1->setStyleSheet(stylelabel);
//    label2->setStyleSheet(stylelabel);
//    label3->setStyleSheet(stylelabel);
//    label4->setStyleSheet(stylelabel);

//    QString buttonstyle( "background:rgb(210, 210, 210);"
//                  "selection-color:rgb(237, 237, 237);"
//                  "selection-background-color:rgb(210,210,210);");

//    btn->setStyleSheet(buttonstyle);
//    this->setStyleSheet("background:rgb(210, 210, 210);");

    panel->addLayout(listpanel);

    panel->setMargin(1);
    panel->setSpacing(0);

    installEventFilter(this);

    connect(btn, SIGNAL(pressed()), this, SLOT(slotApply()));
}

void ReachZone::setPositionWnd(int posx, int posy)
{
    setGeometry(posx,posy,this->width(), this->height());
}

void ReachZone::setData(double longitude, double latitude, double height, double radius)
{
    te1.setText(QString::number(longitude));
    te2.setText(QString::number(latitude));
    te3.setText(QString::number(height));
    te4.setText(QString::number(radius, 'g', 10));
}

void ReachZone::slotApply()
{
    emit setReachZoneParameters(te1.text().toDouble(),te2.text().toDouble(),te3.text().toDouble(),te4.text().toDouble());
    hide();
}

void ReachZone::paintEvent(QPaintEvent *e)
{
    QPainter pr(this);
    pr.save();
    QBrush br(Qt::SolidPattern);
    br.setColor(QColor(237, 237, 237));
    pr.setBrush(br);
    pr.drawRect(rect());
    pr.restore();
}

bool ReachZone::eventFilter(QObject *obj, QEvent *event)
{
    if(((event->type() == QEvent::KeyRelease) || (event->type() == QEvent::KeyPress) || (event->type() == QEvent::Wheel)))
    {
        emit signalResendUnusedEvents(event);
        return true;
    }
}
