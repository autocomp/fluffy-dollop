#include "menubarcloseforeground.h"
#include <QPainter>


MenuBarCloseForeground::MenuBarCloseForeground(QWidget *parent) : QWidget(parent)
{
    setToolTip(tr("Hide instruments panel"));
    setFixedWidth(30);
    icon = QPixmap(":/img/upArrow_white.png");
    icon = icon.scaled(icon.width()/2.0, icon.height()/2.0 );
}


void MenuBarCloseForeground::paintEvent(QPaintEvent * pe)
{
    QWidget::paintEvent(pe);
    QPainter pr(this);
    pr.drawPixmap(width()/2 - icon.width()/2 , height()/2 - icon.height()/2, icon.width(), icon.height(), icon);
}

