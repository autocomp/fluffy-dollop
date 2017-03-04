#include "menuopenbuttonforeground.h"
#include <QPainter>
MenuOpenButtonForeground::MenuOpenButtonForeground(QWidget *parent) : QWidget(parent)
{
    setToolTip(tr("Show instruments panel"));
    icon = QPixmap(":/img/downArrow_white.png");
    icon = icon.scaled(icon.width()/2.0, icon.height()/2.0 );
}


void MenuOpenButtonForeground::paintEvent(QPaintEvent * pe)
{
    QWidget::paintEvent(pe);
    QPainter pr(this);
    pr.drawPixmap(width()/2 - icon.width()/2 , height()/2 - icon.height()/2, icon.width(), icon.height(), icon);
}
