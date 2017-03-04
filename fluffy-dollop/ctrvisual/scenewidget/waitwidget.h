#ifndef WAITWIDGET_H
#define WAITWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "busyindicator.h"

class WaitWidget : public QWidget
{
    Q_OBJECT

    BusyIndicator * m_pBusyIndicator;
    QLabel * m_pInfoLabel;
    QPushButton * m_pPushButton;
public:
    explicit WaitWidget(QWidget *parent = 0);
    void setInfoText(QString str, bool isCloseButtonVisisble);
    void setProgress(QString progressString);
signals:
    void signalOkClicked();
public slots:
    
};

#endif // WAITWIDGET_H
