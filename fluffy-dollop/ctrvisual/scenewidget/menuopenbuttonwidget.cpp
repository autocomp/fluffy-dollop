#include "menuopenbuttonwidget.h"

#include "menuopenbuttonwidget.h"
#include <QLayout>
#include <QBitmap>
#include <QMouseEvent>
#include <QPushButton>
#include <QPainter>
#include <QBitmap>
#include <QPixmap>
#include <QState>
#include <QVariant>
#include "menuopenbuttonforeground.h"

MenuOpenButtonWidget::MenuOpenButtonWidget(QWidget *parent) :
    QWidget(parent)
{
    widthBar = 30;
    heightBar = 35 + 5;
    m_minSize = 5;

    setContentsMargins(0,4,0,0);

    QWidget * wdg = new QWidget(this);
    wdg->setObjectName("MenuOpenButtonWidget_Background");

    QHBoxLayout * lt = new QHBoxLayout(this);
    lt->setMargin(0);
    lt->setSpacing(0);
    lt->addWidget(wdg);

    QHBoxLayout * lt1 = new QHBoxLayout(wdg);
    lt1->setMargin(0);
    lt1->setSpacing(0);
    lt1->addWidget(new MenuOpenButtonForeground(wdg));
///
    setMaximumSize(widthBar, heightBar);
    resize(widthBar,heightBar);
    icon = QPixmap(":/img/downArrow_white.png");
    icon = icon.scaled(icon.width()/2.0, icon.height()/2.0 );
    setMouseTracking(true);

    setFocusPolicy(Qt::WheelFocus);
    this->resize(widthBar,m_minSize);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    st1 = new QState();
    st2 = new QState();

    st1->assignProperty(this, "size", QSize(widthBar,0));
    st2->assignProperty(this, "size", QSize(widthBar,heightBar));

    st1->addTransition(this, SIGNAL(needShow()), st2);
    st2->addTransition(this, SIGNAL(needHide()), st1);

    machine_.addState(st1);
    machine_.addState(st2);
    machine_.setInitialState(st2);

    an1 = new QPropertyAnimation(this, "size");
    an1->setEasingCurve(QEasingCurve::InOutCubic);
    an1->setDuration(100);
    machine_.addDefaultAnimation(an1);

    machine_.start();
}


void MenuOpenButtonWidget::mousePressEvent(QMouseEvent *)
{
    emit needHide();
}

void MenuOpenButtonWidget::show()
{
    emit needShow();
    QWidget::show();
}

void MenuOpenButtonWidget::hide()
{
    emit needHide();
    QWidget::hide();
}

void MenuOpenButtonWidget::setBackgroundImage(QPixmap pm)
{
    m_backgroundPixmap = pm;
    m_needUpdateBackground = true;
    update();
}

void MenuOpenButtonWidget::setSize(int width, int height)
{
    widthBar = width;
    heightBar = height;

    setMaximumSize(widthBar, heightBar);
    resize(widthBar,heightBar);
}

void MenuOpenButtonWidget::setHeight(int height)
{
    heightBar = height;
    m_minSize = height;
    setMaximumSize(widthBar, heightBar);
    resize(widthBar,heightBar);
}

void MenuOpenButtonWidget::setViewMenuOpenButtonWidget()
{
    setMaximumSize(widthBar, heightBar);
    resize(widthBar,heightBar);
    icon = QPixmap(":/img/downArrow_white.png");
    icon = icon.scaled(icon.width()/2.0, icon.height()/2.0 );
    setMouseTracking(true);

    setFocusPolicy(Qt::WheelFocus);
    this->resize(widthBar,heightBar);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    st1->assignProperty(this, "size", QSize(widthBar, 0));
    st2->assignProperty(this, "size", QSize(widthBar, heightBar));
    machine_.start();
}



//void MenuOpenButtonWidget::paintEvent(QPaintEvent * pe)
//{
//    QWidget::paintEvent(pe);
//    QPainter pr(this);
//    pr.drawPixmap(width()/2 - icon.width()/2 , height()/2 - icon.height()/2, icon.width(), icon.height(), icon);
//}



MenuOpenButtonWidget::~MenuOpenButtonWidget()
{
    delete st1;
    delete st2;
    delete an1;
}
