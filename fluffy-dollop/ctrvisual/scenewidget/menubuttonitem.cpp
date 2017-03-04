#include "menubuttonitem.h"
#include <QFile>

MenuButtonItem::MenuButtonItem(QWidget *parent) :
    QPushButton(parent)
{
    /*описание стилей кнопок*/
//    QFile file(":/pagefold.qss");
//    bool res = file.open(QFile::ReadOnly);
//    QString styleSheet = QLatin1String(file.readAll());
//    this->setStyleSheet(styleSheet);
//    setStyleSheet("MenuButtonItem {border-width: 0;} ");
//    file.close();
    m_checkStates = false;
    connect(this, SIGNAL(toggled(bool)), this, SLOT(slotCheckIcon(bool)));
/*
    setFixedSize(30, 30);
    setContentsMargins(1,1,1,1);
    setIconSize(QSize(28, 28));*/
}


void MenuButtonItem::setIcons(const QIcon & pressedIcon, const QIcon & unpressedIcon)
{
    m_checkStates = true;
    m_unpressedIcon = unpressedIcon;
    m_pressedIcon = pressedIcon;
    update();
    slotCheckIcon(isChecked());
}


void MenuButtonItem::slotCheckIcon(bool res)
{
    if(m_checkStates)
    {
        if(!m_pressedIcon.isNull() && !m_unpressedIcon.isNull())
        {
            if(res)
            {
                setIcon(m_pressedIcon);
            }
            else
            {
                setIcon(m_unpressedIcon);
            }
        }
    }
}
