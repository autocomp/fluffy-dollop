#include "collapsedwidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QDebug>

#include <QMouseEvent>

CollapsedWidget::CollapsedWidget(QWidget *parent) :
    QDialog(parent),
    m_headerHeight(30)
{
    m_mainPanelMargin = 2;
    m_mainPanelSpacing = 0;

    m_visible = false;
    m_moveOnlyOnParent = false;

    m_minIcon = QIcon(":/img/211_collapse_window_white.png");
    m_maxIcon = QIcon(":/img/212_expand_window_white.png");
    m_closeIcon = QIcon(":/img/210_close_window_white.png");
    m_pinIcon = QIcon(":/img/303_pin_unpin_window(pinned)_white.png");
    m_unpinIcon = QIcon(":/img/213_pin-unpin_window_white.png");

    m_lockIcon = QIcon(":/img/447_key_close_white.png");
    m_unlockIcon = QIcon(":/img/447_key_open_white.png");

    m_onParentIcon = QIcon(":/img/262_step_back_white.png");
    m_onGlobalIcon = QIcon(":/img/263_step_forth_white.png");

    m_windowPrepareToMove = false;
    m_windowPrepareToMove = false;
    m_isMinimized = false;
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    setWindowFlags(Qt::FramelessWindowHint);


    m_topWidget = new QWidget(this);

    m_pMainLayout = new QVBoxLayout(this);
    m_pMainLayout->setMargin(0);
    m_pMainLayout->setSpacing(0);

    m_pTitleLbl = new QLabel(QString(tr("Video Recorder")),m_topWidget);

//    m_pTitleLbl->setStyleSheet("background-color:rgba(255,255,255,0); \
//                               color:rgb(255,255,255);"
//            );

    m_pTitleLbl->setMinimumWidth(30);


    m_pPinButton = new QPushButton(m_pinIcon,QString(""),m_topWidget);
    m_pPinButton->setFlat(true);
    m_pPinButton->setCheckable(true);
    connect(m_pPinButton, SIGNAL(clicked()), this, SLOT(slotPinWindow()));
    m_pPinButton->setChecked(true);
//    m_pPinButton->setStyleSheet("QPushButton:checked { \
//                                background-color:rgba(240,240,240,0); \       }" );

m_pMinimizeBtn = new QPushButton(m_minIcon,"",m_topWidget);
m_pMinimizeBtn->setFlat(true);
connect(m_pMinimizeBtn, SIGNAL(clicked()), this, SLOT(slotMinMaxWindow()));
m_pMinimizeBtn->hide();


m_pCloseBtn = new QPushButton(m_closeIcon,"",m_topWidget);
m_pCloseBtn->setFlat(true);
connect(m_pCloseBtn, SIGNAL(clicked()), this, SLOT(slotCloseWindow()));

m_pLockButton = new QPushButton(m_lockIcon,"",m_topWidget);
m_pLockButton->setFlat(true);
m_pLockButton->setCheckable(true);
m_pLockButton->setChecked(false);
connect(m_pLockButton, SIGNAL(clicked()), this, SLOT(slotLockWindow()));

m_pGlobalMoveBtn = new QPushButton(m_onGlobalIcon,"",m_topWidget);
m_pGlobalMoveBtn->setFlat(true);
connect(m_pGlobalMoveBtn, SIGNAL(clicked()), this, SLOT(slotGlobalMove()));
m_pGlobalMoveBtn->hide();

QHBoxLayout * top = new QHBoxLayout();
top->addWidget(m_topWidget);
m_topWidget->setFixedHeight(m_headerHeight);
QHBoxLayout * topWidgetLayout = new QHBoxLayout(m_topWidget);

topWidgetLayout->setMargin(4);
topWidgetLayout->setSpacing(4);


top->setMargin(0);
top->setSpacing(0);
topWidgetLayout->addWidget(m_pPinButton,0,Qt::AlignLeft);
topWidgetLayout->addStretch(1);
topWidgetLayout->addWidget(m_pTitleLbl,0,Qt::AlignLeft);
topWidgetLayout->addStretch(1);
topWidgetLayout->addWidget(m_pGlobalMoveBtn,0,Qt::AlignRight);
topWidgetLayout->addWidget(m_pMinimizeBtn,0,Qt::AlignRight);
topWidgetLayout->addWidget(m_pCloseBtn,0,Qt::AlignRight);
topWidgetLayout->addWidget(m_pLockButton, 0, Qt::AlignRight);


m_pMainPanel = new QVBoxLayout();
m_pMainPanel->setMargin(m_mainPanelMargin);
m_pMainPanel->setSpacing(m_mainPanelSpacing);


m_pMainLayout->addLayout(top);
m_pMainLayout->addLayout(m_pMainPanel);

setMinimumHeight(m_headerHeight);

}


int CollapsedWidget::getTopMargin()
{
   return m_mainPanelMargin + m_mainPanelSpacing + m_headerHeight;
}

int CollapsedWidget::getBottomMargin()
{
    return m_mainPanelMargin + m_mainPanelSpacing;
}

int CollapsedWidget::getRightMargin()
{
    return m_mainPanelMargin + m_mainPanelSpacing;
}

int CollapsedWidget::getLeftMargin()
{
    return m_mainPanelMargin + m_mainPanelSpacing;
}


void CollapsedWidget::setLocked(bool isLocked)
{

    m_pLockButton->blockSignals(true);
    if(isLocked)
    {
        m_pLockButton->setIcon(m_lockIcon);
        m_pLockButton->setChecked(true);
    }
    else
    {
        m_pLockButton->setChecked(false);
        m_pLockButton->setIcon(m_unlockIcon);
    }

    m_pLockButton->blockSignals(false);
}

void CollapsedWidget::slotGlobalMove()
{
    if(m_moveOnlyOnParent)
    {
        m_pGlobalMoveBtn->setIcon(m_onParentIcon);
        moveOnlyOnParent(!m_moveOnlyOnParent);
    }
    else
    {
        m_pGlobalMoveBtn->setIcon(m_onGlobalIcon);
        moveOnlyOnParent(!m_moveOnlyOnParent);
    }
}

void CollapsedWidget::setDefaultFixedSize(QSize sz)
{
    m_defaultFixedSize = sz;
}

QSize CollapsedWidget::defaultFixedSize()
{
    return m_defaultFixedSize;
}



bool CollapsedWidget::eventFilter(QObject * obj, QEvent * ev)
{
    //qDebug() << "CollapsedWidget::eventFilter(): Eventtype "<< ev->type();
    foreach(QWidget * wdg, m_insertedWidgets)
    {
        if(wdg == obj)
        {
            if(QEvent::Hide == ev->type())
            {
                //wdg->setVisible(true);
                emit signalVisibleNeedChangeInternal(false);
                ev->ignore();
                return false;
            }

            if(QEvent::Show == ev->type())
            {
                emit signalVisibleNeedChangeInternal(true);
                ev->ignore();
                return false;
            }

            if(QEvent::Close == ev->type())
            {
                //wdg->setVisible(true);
                emit signalVisibleNeedChangeInternal(false);
                ev->ignore();
                return false;
            }
        }
    }

    return false;
}


void CollapsedWidget::setVisibleHeader(bool vis)
{
    m_topWidget->setVisible(vis);
}


void CollapsedWidget::setBorders(int margins, int spacings)
{
    m_pMainPanel->setMargin(margins);
    m_pMainPanel->setSpacing(spacings);
}

void CollapsedWidget::setWindowTitleText(QString title)
{
    m_pTitleLbl->setText(title);

}

void CollapsedWidget::insertWidget(QWidget * widget)
{
    m_pMainPanel->addWidget(widget,10);
    widget->setParent(this);
    m_insertedWidgets.append(widget);

    if(m_isMinimized)
    {
        m_pMainPanel->setMargin(0);
        m_pMainPanel->setSpacing(0);
    }
    else
    {
        m_pMainPanel->setMargin(m_mainPanelMargin);
        m_pMainPanel->setSpacing(m_mainPanelSpacing);
    }

}


void CollapsedWidget::moveOnlyOnParent(bool on)
{
    m_moveOnlyOnParent = on;

    if(m_moveOnlyOnParent)
    {
        m_pGlobalMoveBtn->setIcon(m_onParentIcon);
    }
    else
    {
        m_pGlobalMoveBtn->setIcon(m_onGlobalIcon);
    }
}
void CollapsedWidget::paintEvent(QPaintEvent *e)
{
    QDialog::paintEvent(e);
//    QPainter pr(this);


//    QPen pn1(QBrush(QColor(130, 130, 130)),0, Qt::SolidLine, Qt::RoundCap ,Qt::BevelJoin  );
//    pn1.setColor(QColor(150, 150, 150, 255));
//    pn1.setWidth(2);

//    QBrush br(Qt::LinearGradientPattern,Qt::SolidPattern);
//    br.setColor(QColor(120, 120, 120, 255));


//    pr.setBrush(br);
//    pr.setPen(pn1);
//    pr.fillRect(rect(), Qt::SolidPattern);

//    pr.save();
//    br.setColor(QColor(230, 230, 230, 255));
//    pr.fillRect(m_topWidget->rect(), Qt::SolidPattern);
//    pr.restore();

//    pr.drawRect(rect());
}

void CollapsedWidget::removeWidget(QWidget * widget)
{
    widget->setParent(0);
    m_pMainPanel->removeWidget(widget);
    m_insertedWidgets.removeOne(widget);
}

void CollapsedWidget::collapse()
{

    foreach (QWidget *wdg, m_insertedWidgets)
    {
        wdg->hide();
    }

    m_pMinimizeBtn->setIcon(m_maxIcon);
    m_pMainPanel->setMargin(0);
    m_pMainPanel->setSpacing(0);
    m_isMinimized = true;
    setMinimumHeight(m_headerHeight);
    adjustSize();
    if(m_prevSize != size())
    {
        m_prevSize = size();
        emit signalSizeChanged(size());
    }
}


void CollapsedWidget::hideEvent(QResizeEvent *e)
{
    m_visible = false;
    emit signalVisibleNeedChange(false);
    if(m_prevSize != e->size())
    {
        m_prevSize = e->size();
        emit signalSizeChanged(e->size());
    }

}

void CollapsedWidget::hide()
{
    m_visible = false;
    emit signalVisibleNeedChange(false);

    QWidget::hide();
    adjustSize();
    if(m_prevSize != size())
    {
        m_prevSize = size();
        emit signalSizeChanged(size());
    }
}

bool CollapsedWidget::isVisible()
{
    return m_visible;
}
void CollapsedWidget::show()
{
    m_visible = true;
    emit signalVisibleNeedChange(true);
    QWidget::show();
    adjustSize();
    if(m_prevSize != size())
    {
        m_prevSize = size();
        emit signalSizeChanged(size());
    }
}

void CollapsedWidget::showEvent(QResizeEvent *e)
{
    m_visible = true;
    emit signalVisibleNeedChange(true);
    if(m_prevSize != e->size())
    {
        m_prevSize = e->size();
        emit signalSizeChanged(e->size());
    }

}

void CollapsedWidget::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    if(m_prevSize != e->size())
    {
        m_prevSize = e->size();
        emit signalSizeChanged(e->size());
    }
}

void CollapsedWidget::expand()
{
    foreach (QWidget *wdg, m_insertedWidgets)
    {
        wdg->show();
    }

    m_pMainPanel->setMargin(m_mainPanelMargin);
    m_pMainPanel->setSpacing(m_mainPanelSpacing);
    m_pMinimizeBtn->setIcon(m_minIcon);
    m_isMinimized = false;
    adjustSize();

    if(m_prevSize != size())
    {
        m_prevSize = size();
        emit signalSizeChanged(size());
    }

    /*QSize sz = sizeHint();
            resize(sz);*/

}


void CollapsedWidget::slotMinMaxWindow()
{
    if(m_isMinimized)
    {
        /*must be expanded*/
        expand();
    }
    else
    {
        collapse();
    }
}

void CollapsedWidget::setVisible(bool visible)
{
    m_visible = visible;
    emit signalVisibleNeedChange(visible);
    QDialog::setVisible(visible);
    emit signalVisibleNeedChange(visible);

    foreach (QWidget * wdg, m_insertedWidgets) {
        wdg->setVisible(true);
    }

}
void CollapsedWidget::setPinned(bool pinned)
{
    if(m_pPinButton->isChecked() != pinned)
    {
        m_pPinButton->setChecked(pinned);
    };

    slotPinWindow();
}

void CollapsedWidget::setVisisblePinButton(bool vis)
{
    m_pPinButton->setVisible(vis);
}

void CollapsedWidget::setVisisbleCloseButton(bool vis)
{
    m_pCloseBtn->setVisible(vis);
}

void CollapsedWidget::setVisisbleCollapseButton(bool vis)
{
    m_pMinimizeBtn->setVisible(vis);
}

void CollapsedWidget::setVisisbleLockButton(bool vis)
{
    m_pLockButton->setVisible(vis);
}


bool CollapsedWidget::isPinned()
{
    return m_pPinButton->isChecked();
}

void CollapsedWidget::slotPinWindow()
{
    if(m_pPinButton->isChecked())
    {
        m_pPinButton->setIcon(m_pinIcon);
    }
    else
    {
        m_pPinButton->setIcon(m_unpinIcon);
    }

    emit signalPinCollapsedWidget(m_pPinButton->isChecked());

}


void CollapsedWidget::slotLockWindow()
{
    if(m_pLockButton->isChecked())
    {
        m_pLockButton->setIcon(m_lockIcon);
    }
    else
    {
        m_pLockButton->setIcon(m_unlockIcon);
    }


    emit signalLockCollapsedWidget(m_pLockButton->isChecked());
}

void CollapsedWidget::slotCloseWindow()
{
    emit signalHideWindow();
    emit signalSizeChanged(QSize(0,0));
}

void CollapsedWidget::keyPressEvent(QKeyEvent *e)
{
    if(Qt::Key_Escape == e->key())
    {
        int a = 5;
    }

    QWidget::keyPressEvent(e);

}
void CollapsedWidget::mousePressEvent(QMouseEvent *e)
{
    QRect r = m_topWidget->geometry();
    if( !r.contains(e->pos()) )
        return ;

    /*if(0 != parent())
                return;*/

    if(! m_pPinButton->isChecked())
        m_windowPrepareToMove = true;

    m_pressedLocalLastPos = e->pos();
    m_pressedGlobalLastPos = e->globalPos();
}




void CollapsedWidget::mouseMoveEvent(QMouseEvent *e)
{
    if(!m_windowPrepareToMove)
        return;

    m_windowMoveMode = true;
    QPoint tmpPoint = m_pressedGlobalLastPos;
    if(0 != parent())
    {

        if(!m_moveOnlyOnParent)
        {
            m_pLastParent = parentWidget();
            m_pLastParent->layout()->removeWidget(this);
            emit signalRemoveMe();
            setParent(0);
            show();
            m_pressedGlobalLastPos = e->globalPos();
            m_pressedLocalLastPos = e->pos();
            setGeometry(m_pressedGlobalLastPos.x() + m_pressedLocalLastPos.x(),
                        m_pressedGlobalLastPos.y() + m_pressedLocalLastPos.y(),
                        width(),
                        height());

            //            emit signalPosChanged(m_id, QPoint(m_pressedGlobalLastPos.x() + m_pressedLocalLastPos.x(),
            //                                         m_pressedGlobalLastPos.y() + m_pressedLocalLastPos.y()));
        }
        else
        {
            QPoint p = pos();
            /*setGeometry(p.x() + ,
                                p.y() + (e->pos().y() - m_pressedLocalLastPos.y() ),
                                width(),
                                height());*/
            move(p.x() + (e->pos().x() - m_pressedLocalLastPos.x())
                 , p.y() + (e->pos().y() - m_pressedLocalLastPos.y()));
            //m_pressedLocalLastPos = e->pos();
            emit  signalDragGlobPosChanged(geometry(),pos(), QMouseEvent::Move);
            // emit signalPosChanged(m_id, pos());
        }

    }
    else
    {
        int x = -m_pressedLocalLastPos.x() + ( e->globalPos().x());
        int y = -m_pressedLocalLastPos.y() + ( e->globalPos().y());
        m_pressedGlobalLastPos = e->globalPos();
        // m_pressedLocalLastPos = e->pos();
        move(x,y);
        emit  signalDragGlobPosChanged(geometry(),m_pressedGlobalLastPos, QMouseEvent::Move);
        //        emit signalPosChanged(m_id, QPoint(m_pressedGlobalLastPos.x() + m_pressedLocalLastPos.x(),
        //                                     m_pressedGlobalLastPos.y() + m_pressedLocalLastPos.y()));
    }

}


void CollapsedWidget::setId(quint64 id)
{
    m_id = id;
}

void CollapsedWidget::mouseReleaseEvent(QMouseEvent *e)
{
    int x = -m_pressedLocalLastPos.x() + ( e->globalPos().x());
    int y = -m_pressedLocalLastPos.y() + ( e->globalPos().y());
    m_pressedGlobalLastPos = e->globalPos();

    if((m_windowPrepareToMove && m_windowMoveMode) )
        emit  signalDragGlobPosChanged(geometry(),m_pressedGlobalLastPos, QMouseEvent::MouseButtonRelease);

    m_windowPrepareToMove = false;
    m_windowMoveMode = false;
    emit signalPosChanged(m_id, pos());
}
