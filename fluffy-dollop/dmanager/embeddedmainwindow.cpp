#include "embeddedmainwindow.h"


#include "embeddedpanel.h"
#include <QApplication>
#include <QDebug>

using namespace ew;
EmbeddedMainWindow::EmbeddedMainWindow(QWidget *parent)
    : EmbeddedWindow(parent)
{
    m_pViewPanel = 0;
    m_pViewPanel = new EmbeddedPanel(this);
    m_pMainViewWidgetLayout->addWidget(m_pViewPanel);
    m_pMainViewWidgetLayout->setMargin(0);
    m_pMainViewWidgetLayout->setSpacing(0);
}


void EmbeddedMainWindow::closeEvent(QCloseEvent *e)
{
    ewApp()->close();
    EmbeddedWindow::closeEvent(e);
}

EmbeddedMainWindow::~EmbeddedMainWindow()
{
    ewApp()->close();
    qApp->closeAllWindows();

}

EmbeddedWidgetType EmbeddedMainWindow::widgetType()
{
    return ew::EWT_MAINWINDOW;
}


void EmbeddedMainWindow::removeWidget(ew::EmbeddedWidgetBaseStructPrivate * embStrPriv)
{
    m_pViewPanel->removeEmbeddedWidget(embStrPriv);
}

QRect EmbeddedMainWindow::getParkingRect()
{
    return m_mainViewWidget->geometry();
}

void EmbeddedMainWindow::setWidget(ew::EmbeddedWidgetBaseStructPrivate * embStrPriv)
{
    m_pViewPanel->setWidget(embStrPriv);
}

void EmbeddedMainWindow::addWidget(ew::EmbeddedWidgetBaseStructPrivate * embStrPriv, QPoint pos)
{
    m_pViewPanel->insertEmbeddedWidget(embStrPriv,pos);
    m_pViewPanel->optimizeWidgetSizes();
}
