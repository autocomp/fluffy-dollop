#include "embeddedwindowheader.h"

#include <QApplication>
#include <QMouseEvent>


using namespace ew;

int EmbeddedWindowHeader::getButtonSpace()
{
    if(m_pInstalledMenu)
    {
        return ew::EmbeddedHeader::getButtonSpace()-m_pInstalledMenu->width();
    }
    return ew::EmbeddedHeader::getButtonSpace();
}



EmbeddedWindowHeader::EmbeddedWindowHeader(QWidget *parent)
    : EmbeddedWidgetHeader(parent)
{

    setObjectName("EmbeddedWindow_EmbeddedWindowHeader");
    m_pInstalledMenu = 0;
}




void EmbeddedWindowHeader::setMenuBar(QMenuBar * mainMenu)
{
    delete m_pInstalledMenu;
    m_pInstalledMenu = mainMenu;
    m_headerLayout->insertWidget(2,mainMenu);

}


