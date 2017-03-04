#include "menubar.h"
#include <QLayout>
#include <QBitmap>
#include <QMouseEvent>
#include <QPushButton>
#include <QPainter>
#include <QBitmap>
#include <QPixmap>
#include <QLabel>
#include <QDebug>
#include "menubarcloseforeground.h"
#include "menuopenbuttonforeground.h"

MenuBar::MenuBar(QWidget *parent) :
    QWidget(parent)
{
    widthBar = 600;
    m_margins = 0;
    heightBar = 24;
    m_minSize = 3;
    m_leftMargin = 35;
    newWidth = 0;
    m_visibleLines.append(0);

    setContentsMargins(0,0,0,0);

    m_startHeight = heightBar + m_margins;

    setObjectName("MenuBar");
    QWidget * backwdg= new QWidget(this);
    QHBoxLayout *lt1 = new QHBoxLayout(backwdg);
    backwdg->setObjectName("MenuBar_Background");
    lt1->setMargin(0);
    lt1->setSpacing(0);


    m_pMainLt = new QVBoxLayout();
    QWidget * wdg= new QWidget(this);
    QHBoxLayout *lt = new QHBoxLayout(wdg);
    lt->setMargin(0);
    lt->setSpacing(0);
    //m_pMainLt->addLayout(lt);
    m_layouts.insert(0,lt);
    m_visibleContainerWidgets.insert(0,wdg);
    m_list.insert(0,QWidgetList());
    m_pMainLt->setMargin(0);
    m_pMainLt->setSpacing(0);
    m_pMainLt->addWidget(wdg,1);
    lt1->addLayout(m_pMainLt);
    lt1->addWidget(new MenuBarCloseForeground(backwdg),0);


    QVBoxLayout * mainLt = new QVBoxLayout(this);
    mainLt->setMargin(0);
    mainLt->setSpacing(0);
    mainLt->addWidget(backwdg,1);

    m_closeIcon = QPixmap(":/img/upArrow_white.png");
    m_closeIcon = m_closeIcon.scaled(m_closeIcon.width()/2.0, m_closeIcon.height()/2.0 );


    m_needHide = true;

    st1 = new QState();
    st2 = new QState();
}

void MenuBar::resizeEvent(QResizeEvent * e)
{
    QSize s = size();
    if(height() <= 0)
        return;

    m_barInited = true;
    QWidget::resizeEvent(e);
}


void MenuBar::setVisibleLine(MenuBar::MenuBarLines lineNum, bool vis)
{
    if(m_list.contains((int)lineNum))
    {
        QWidgetList wdgList = m_list.value((int)lineNum);

        if(vis)
        {
            if( !m_visibleLines.contains((int)lineNum) )
                m_visibleLines.append((int)lineNum);
            {
                QWidget * wdg = m_visibleContainerWidgets.value(lineNum,0);
                if(0 != wdg)
                {
                    wdg->show();
                }
            }
        }
        else
        {
            if( m_visibleLines.contains((int)lineNum) )
                m_visibleLines.removeAll((int)lineNum);
            {
                QWidget * wdg = m_visibleContainerWidgets.value(lineNum,0);
                if(0 != wdg)
                {
                    wdg->hide();
                }
            }
        }

        int prefferHeigth = m_visibleLines.count() * heightBar + m_margins;
        resize(width(), prefferHeigth);
        setViewMenuBar();
    }

}


void MenuBar::setViewMenuBar()
{
    machine_.removeState(st1);
    machine_.removeState(st2);
    m_pMainLt->setSpacing(m_minSize);
    m_pMainLt->setMargin(0);

    setFixedWidth(widthBar);
    int prefferHeigth = m_visibleLines.count() * heightBar + m_margins;
    resize(widthBar,prefferHeigth);
    st1->assignProperty(this, "size", QSize(widthBar,0));
    st2->assignProperty(this, "size", QSize(widthBar,prefferHeigth));

    st1->addTransition(this, SIGNAL(needShow()), st2);
    st2->addTransition(this, SIGNAL(needHide()), st1);

    machine_.addState(st1);
    machine_.addState(st2);
    machine_.setInitialState(st1);

    if(an1)
        delete an1;

    an1 = new QPropertyAnimation(this, "size");
    an1->setEasingCurve(QEasingCurve::InOutCubic);
    an1->setDuration(100);
    machine_.addDefaultAnimation(an1);

    /* starting machine */
    machine_.start();

    setFocusPolicy(Qt::WheelFocus);
    //resize(widthBar,m_minSize);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    return;

}

void MenuBar::show()
{

    QWidget::show();
    foreach (QWidgetList wlist, m_list)
    {
        int lineNum = m_list.key(wlist);
        if(!m_visibleLines.contains(lineNum))
        {
            foreach (QWidget * wdg, wlist)
            {
                wdg->hide();
            }
        }
    }

    setViewMenuBar();
    emit needShow();
}


void MenuBar::hide()
{
    emit needHide();
    QWidget::hide();
}

void MenuBar::mousePressEvent(QMouseEvent * e)
{
    if(e->pos().x() >  width() - m_leftMargin)
    {
        /*need hide*/
        emit needHide();
    }

    QWidget::mousePressEvent(e);
    repaint();
}


void MenuBar::setBackgroundImage(QPixmap & pm)
{
    m_backgroundPixmap = pm;
    m_needUpdateBackground = true;

    update();

}

void MenuBar::setFixedWidth(int w)
{
    widthBar = w;

    if(height() <= 0)
        return;

    QWidget::setFixedWidth(w);
    repaint();
}

void MenuBar::setHeight(int h)
{
    heightBar = h;
    st1->assignProperty(this, "size", QSize(widthBar,0));
    st2->assignProperty(this, "size", QSize(widthBar,m_visibleLines.count() * heightBar + m_margins));
}

void MenuBar::setWidth(int w)
{
    widthBar = w;
    QWidget::setFixedWidth(w);
    repaint();
}


//void MenuBar::paintEvent(QPaintEvent * pe)
//{
//    QWidget::paintEvent(pe);
//    QPainter pr(this);
//    pr.setRenderHint(QPainter::Antialiasing);

//    pr.drawPixmap(width() - m_leftMargin/2 - m_closeIcon.width()/2 , height()/2 - m_closeIcon.height()/2, m_closeIcon.width(), m_closeIcon.height(), m_closeIcon);
//}


int MenuBar::prefferWidth()
{
    int sp = layout()->spacing();
    int minWidth = 0;
    QWidgetList list = m_list.value(0);
    foreach (QWidget * widg, list)
    {
        minWidth += widg->minimumWidth() + sp*2 + widg->contentsMargins().left() + widg->contentsMargins().right();
    }

    minWidth +=  layout()->margin()*2 + sp*2 + contentsMargins().left() + contentsMargins().right();

    return minWidth;

}

void MenuBar::addItem(QWidget * menuItem, int stretch, Qt::Alignment al, int line )
{
    if( (!m_list.contains(line)) || (!m_layouts.contains(line)))
    {
        QWidget * wdg = new QWidget(this);
        m_list.insert(line,QWidgetList());
        QHBoxLayout * lt = new QHBoxLayout(wdg);
        lt->setMargin(0);
        lt->setSpacing(0);
        m_layouts.insert(line, lt);
        m_visibleContainerWidgets.insert(line,wdg);
        m_pMainLt->addWidget(wdg,1);
    }

    QWidgetList wlist = m_list.value(line);
    QHBoxLayout * lt = m_layouts.value(line,0);

    wlist.append(menuItem);
    lt->addWidget(menuItem, stretch,al);
    m_list.remove(line);
    m_list.insert(line, wlist);

    if(0 == line)
    {
        newWidth += menuItem->width() + lt->spacing();
    }

    return;
}

void MenuBar::insertItem(QWidget * menuItem, int pos, int stretch, Qt::Alignment al, int line )
{
    if( (!m_list.contains(line)) || (!m_layouts.contains(line)))
    {
        QWidget * wdg = new QWidget(this);
        m_list.insert(line,QWidgetList());
        QHBoxLayout * lt = new QHBoxLayout(wdg);
        lt->setMargin(0);
        lt->setSpacing(0);
        m_layouts.insert(line, lt);
        m_visibleContainerWidgets.insert(line,wdg);
        m_pMainLt->addWidget(wdg,1);
    }

    QWidgetList wlist = m_list.value(line);
    QHBoxLayout * lt = m_layouts.value(line,0);

    wlist.append(menuItem);
    lt->insertWidget(pos, menuItem, stretch,al);
    m_list.remove(line);
    m_list.insert(line, wlist);

    if(0 == line)
    {
        newWidth += menuItem->width() + lt->spacing();
    }

    return;
}


void MenuBar::addVerticalLine(int lineNum)
{
    QHBoxLayout * lt = m_layouts.value(lineNum,0);
    if(0 != lt)
    {
        QLabel * lbl = new QLabel(this);
        lbl->setFixedSize(3,18);
        lbl->setPixmap(QPixmap(":/devider.png"));
        lbl->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
        addItem(lbl, 0,Qt::AlignCenter,lineNum);

        if(0 == lineNum)
        {
            newWidth += lbl->width() + m_minSize;
        }
    }

    int a = 5;
    return;
}

void MenuBar::setBarWidth()
{
    widthBar = newWidth;
    this->setFixedWidth(widthBar);
    this->resize(widthBar,m_minSize);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    return;
}

void MenuBar::addStretch(int stretch, int line)
{
    QHBoxLayout * lt = m_layouts.value(line,0);

    if(0 != lt)
    {
        lt->addStretch(stretch);
    }
    return;
}



MenuBar::~MenuBar()
{


}
