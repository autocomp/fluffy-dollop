#ifndef MAINCONTEINERWIDGET_H
#define MAINCONTEINERWIDGET_H

#include <QWidget>
#include <QVBoxLayout>

class MainConteinerWidget : public QWidget
{
    Q_OBJECT
    QWidget * m_mainViewWidget;
    QVBoxLayout * m_mainLayout;
public:
    explicit MainConteinerWidget(QWidget *parent = 0);
    QWidget * getMainViewWidget();
    void setMainWidget(QWidget * _mainViewWidget);
    
signals:
    
public slots:
    
};

#endif // MAINCONTEINERWIDGET_H
