#include "mainconteinerwidget.h"

MainConteinerWidget::MainConteinerWidget(QWidget *parent) :
    QWidget(parent)
{
    m_mainViewWidget = 0;
    //setStyleSheet("QWidget { border: 0px; border-radius:0px; background-color: yellow } ");
    m_mainLayout = new QVBoxLayout(this);
}

void MainConteinerWidget::setMainWidget(QWidget * _mainViewWidget)
{
    if(0 != m_mainViewWidget)
    {
        m_mainLayout->removeWidget(m_mainViewWidget);
    }

    m_mainViewWidget = _mainViewWidget;
    if(0 != _mainViewWidget)
    {
        m_mainViewWidget->setParent(this);
        m_mainLayout->addWidget(_mainViewWidget);
        m_mainLayout->setMargin(0);
        m_mainLayout->setSpacing(0);
    }
}

QWidget * MainConteinerWidget::getMainViewWidget()
{
    return m_mainViewWidget;
}
