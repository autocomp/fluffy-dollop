#include "navigation.h"
#include "QPainter"
#include <QStyle>
#include <QFile>
#include <QAbstractItemView>
//#include <QPlastiqueStyle>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QButtonGroup>
#include <QPushButton>
#include <QEvent>

Navigation::Navigation(QWidget *parent) :
    QWidget(parent),
    m_visible(false)
{
    setFixedSize(100,30);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMask(QRegion(0,0,width(), height()));
    m_needUpdateBackground = true;

    QPalette palette;
    palette.setColor(QPalette::Base, Qt::transparent);

    pb1 = new QPushButton(QIcon(QString(":/109_nav_mode_freelook_released.png")),"",0);
    pb2 = new QPushButton(QIcon(QString(":/113_nav_mode_orbit_released.png")),"",0);
    pb3 = new QPushButton(QIcon(QString(":/208_nav_mode_birds_flight_released.png")),"",0);
    pb4 = new QPushButton(QIcon(QString(":/111_nav_mode_ground_released.png")),"",0);

    pb1->setFlat(true);
    pb2->setFlat(true);
    pb3->setFlat(true);
    pb4->setFlat(true);

    pb1->setFixedSize(24,24);
    pb2->setFixedSize(24,24);
    pb3->setFixedSize(24,24);
    pb4->setFixedSize(24,24);

    pb1->setFocusPolicy(Qt::NoFocus);
    pb2->setFocusPolicy(Qt::NoFocus);
    pb3->setFocusPolicy(Qt::NoFocus);
    pb4->setFocusPolicy(Qt::NoFocus);

    pb1->setIconSize(QSize(24,24));
    pb2->setIconSize(QSize(24,24));
    pb3->setIconSize(QSize(24,24));
    pb4->setIconSize(QSize(24,24));

    bg = new QButtonGroup();
    bg->setExclusive(true);
    bg->addButton(pb1,1);
    bg->addButton(pb2,2);
    bg->addButton(pb3,3);

    pb1->setDown(true);
    slotCurrentButtonPress(1);
    pb4->setDisabled(true);

    bgroup = new QHBoxLayout(this);
    bgroup->setMargin(2);
    bgroup->setSpacing(0);

    bgroup->addWidget(pb1);
    bgroup->addWidget(pb2);
    bgroup->addWidget(pb3);
    bgroup->addWidget(pb4);

    pb1->setAttribute(Qt::WA_TranslucentBackground, true);
    pb1->setPalette(palette);

    pb2->setAttribute(Qt::WA_TranslucentBackground, true);
    pb2->setPalette(palette);

    pb3->setAttribute(Qt::WA_TranslucentBackground, true);
    pb3->setPalette(palette);

    pb4->setAttribute(Qt::WA_TranslucentBackground, true);
    pb4->setPalette(palette);

    connect(bg, SIGNAL(buttonPressed (int)), this, SLOT(slotCurrentButtonPress(int)));
    connect(pb4, SIGNAL(clicked()), this, SLOT(slotButtonPress()));

    installEventFilter(this);
}


void Navigation::setBackgroundImage(QPixmap & pm)
{
    m_backgroundPixmap = pm;
    m_needUpdateBackground = true;

    update();
}


void Navigation::paintEvent(QPaintEvent *)
{
    QPainter pr(this);
    if(true == m_needUpdateBackground)
    {
        QPainter pr(this);
        pr.setOpacity(1.0);
        pr.drawPixmap(0,0,m_backgroundPixmap.width(), m_backgroundPixmap.height(),m_backgroundPixmap);
    }
    pr.save();
    QBrush br(/*Qt::LinearGradientPattern,*/Qt::SolidPattern);
//    br.setColor(QColor(0xFF, 0xFF, 0xFF /*100*/));
    br.setColor(QColor(125, 125, 125 /*100*/));
    pr.setBrush(br);
//    pr.setOpacity(0.7);
//    pr.drawRoundedRect(rect(),5,5);
    pr.drawRect(rect());
    pr.restore();
}

bool Navigation::eventFilter(QObject *obj, QEvent *event)
{
    if(((event->type() == QEvent::KeyRelease) || (event->type() == QEvent::KeyPress)))
    {
        emit signalResendUnusedEvents(event);
        return true;
    }
}


void Navigation::slotButtonPress()
{
    emit buttonNavigation();
}

void Navigation::setCurrentNavigation( int id )
{
    slotCurrentButtonPress(id);
}

void Navigation::slotCurrentButtonPress(int id)
{
    emit currentNavigation(id);
    currentIdNavigation = id;
    QPushButton * push = new QPushButton();
    switch( id )
    {
        case 1:
        {
            push = (QPushButton*)bg->button(1);
            push->setIcon(QIcon(QString(":/110_nav_mode_freelook_pressed.png")));
            bg->button(2)->setIcon(QIcon(QString(":/113_nav_mode_orbit_released.png")));
            bg->button(3)->setIcon(QIcon(QString(":/208_nav_mode_birds_flight_released.png")));
            pb4->setDisabled(true);
            break;
        }
        case 2:
        {
            bg->button(1)->setDown(false);
            push = (QPushButton*)bg->button(2);
            push->setIcon(QIcon(QString(":/114_nav_mode_orbit_pressed.png")));
            bg->button(1)->setIcon(QIcon(QString(":/109_nav_mode_freelook_released.png")));
            bg->button(3)->setIcon(QIcon(QString(":/208_nav_mode_birds_flight_released.png")));
            pb4->setDisabled(true);
            break;
        }
        case 3:
        {
            bg->button(1)->setDown(false);
            push = (QPushButton*)bg->button(3);
            push->setIcon(QIcon(QString(":/209_nav_mode_birds_flight_pressed.png")));
            bg->button(2)->setIcon(QIcon(QString(":/113_nav_mode_orbit_released.png")));
            bg->button(1)->setIcon(QIcon(QString(":/109_nav_mode_freelook_released.png")));
            pb4->setDisabled(false);
            break;
        }
    }
}

Navigation::~Navigation()
{
    delete pb1;
    delete pb2;
    delete pb3;
    delete pb4;
    delete bg;
    delete bgroup;
}
