#ifndef REACHZONE_H
#define REACHZONE_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMouseEvent>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QDebug>
#include <QToolBar>
#include <QPainter>
#include <QStyle>

class ReachZone : public QWidget
{
    Q_OBJECT
public:
    explicit ReachZone(QWidget *parent = 0);
    ~ReachZone(){}

    void setData(double longitude, double latitude, double height, double radius);
    void setPositionWnd(int posx, int posy);

    
private:
    QLineEdit te1, te2, te3, te4;
    void paintEvent(QPaintEvent *e);
    bool eventFilter(QObject *obj, QEvent *event);


signals:
    void setReachZoneParameters(double lon, double lat, double h, double rad);
    void signalResendUnusedEvents(QEvent * e);


public slots:
    void slotApply();
    
};

#endif // REACHZONE_H
