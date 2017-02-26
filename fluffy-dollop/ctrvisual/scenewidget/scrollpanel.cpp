#include "scrollpanel.h"
#include <QPainter>
#include <QPaintEvent>
#include <QScrollBar>
#include <QDebug>
#include <QStyle>

ScrollPanel::ScrollPanel(bool isInnerPanel, QWidget *parent) :
    QFrame(parent)
{
    m_needUpdateBackground = false;

    setWindowFlags( Qt::FramelessWindowHint);
    setFrameStyle(QFrame::NoFrame);
    m_showFullFrame = false;

    m_borderPen = QPen(Qt::blue);
    m_borderPen.setWidth(4);
    //setWindowFlags(Qt::WindowFlags);
    m_isInnerPanel = isInnerPanel;
    m_painterBackgroundOpacity = 0.5;
    QPen pen(QColor(65,65,65,150));
    pen.setWidth(2);

    m_pen = pen;
    m_brush = QBrush(QColor(100,100,100,255));
    m_showFrameBorder = false;

    QVBoxLayout * topWidgetLt = new QVBoxLayout(this);
    topWidgetLt->setMargin(0);
    topWidgetLt->setSpacing(0);
    m_pScrollArea = new TransparentScrollArea(this);
    m_pScrollArea->    setWindowFlags( Qt::FramelessWindowHint);
    topWidgetLt->addWidget(m_pScrollArea, 10/*,Qt::AlignCenter*/);
    m_pInnerWidget = new QWidget(this);
    m_pInnerWidget->setWindowFlags( Qt::FramelessWindowHint);
    m_pScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_pScrollArea->setAutoFillBackground(false);
    //m_pScrollArea->sho
    m_pScrollArea->setAlignment(Qt::AlignBottom);
    m_pInnerWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    m_pMainLayout = new QVBoxLayout(m_pInnerWidget);

    m_pInnerEmptyWidget = new QWidget(this);
    m_pInnerTopWidget = new QWidget(this);

    QVBoxLayout * lt1 =new QVBoxLayout(m_pInnerTopWidget);
    QVBoxLayout * lt2 =new QVBoxLayout(m_pInnerEmptyWidget);
    lt1->setMargin(0);
    lt1->setSpacing(0);

    lt2->setMargin(0);
    lt2->setSpacing(0);

    lt2->addStretch(1);

    //  QVBoxLayout * lt =new QVBoxLayout(m_pScrollArea);
    if(isInnerPanel)
        lt1->addWidget(m_pInnerEmptyWidget,1);
    lt1->addWidget(m_pInnerWidget);

    if(!isInnerPanel)
        lt1->addWidget(m_pInnerEmptyWidget,1);

    m_pScrollArea->setContentsMargins(0,0,0,0);

    m_pScrollArea->setWidget(m_pInnerTopWidget);
    m_pScrollArea->setWidgetResizable(true);
    m_pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_pInnerTopWidget->setWindowFlags( Qt::FramelessWindowHint);
    m_pInnerWidget->setWindowFlags( Qt::FramelessWindowHint);
    m_pInnerEmptyWidget->setWindowFlags( Qt::FramelessWindowHint);

    //    setStyleSheet("ScrollPanel { background-color:rgba(130,130,130,255); }");
    //    m_pInnerTopWidget->setStyleSheet(" background-color:rgba(130,130,130,255); ");
    //    m_pInnerWidget->setStyleSheet(" background-color:rgba(130,130,130,255); ");

    //    m_pInnerEmptyWidget->setStyleSheet(" background-color:rgba(130,130,130,255); ");
    //    setStyleSheet("ScrollPanel { background-color:rgba(130,130,130,255); }");
    //    m_pInnerTopWidget->setStyleSheet(" background-color:rgba(130,130,130,255); ");
    //    m_pInnerWidget->setStyleSheet(" background-color:rgba(130,130,130,255); ");
    setFixedWidth(280);
    m_pMainLayout->setMargin(0);
    //    m_pMainLayout->setContentsMargins(QStyle::PM_LayoutLeftMargin,0,
    //                                      QStyle::PM_LayoutRightMargin,QStyle::PM_LayoutBottomMargin);
    m_pMainLayout->setSpacing(20);

    //    m_pScrollArea->setStyleSheet("QScrollArea { background-color:rgba(0xD2,0xD2,0xD2,0x255); } \
    //                                 QScrollBar {background-color:rgba(0xD2,0xD2,0xD2,255);}" );


    m_pScrollArea->setAttribute(Qt::WA_OpaquePaintEvent, true);

    connect(m_pScrollArea, SIGNAL(signalSizeUpdated()), this, SIGNAL(signalGeomChanged()) );

}


void ScrollPanel::setMinimumWidth(int minw)
{
    QFrame::setMinimumWidth(minw);
    m_pInnerWidget->setMinimumWidth(minw - m_pScrollArea->verticalScrollBar()->height());
}

void ScrollPanel::setFixedWidth(int minw)
{
    QFrame::setFixedWidth(minw);
    m_pInnerWidget->setFixedWidth(minw );
}

void ScrollPanel::setBackgroundImage(QPixmap pm)
{
    m_backgroundPixmap = pm;
    m_needUpdateBackground = true;

    update();

}


int ScrollPanel::prefferHeight()
{
    //    QScrollBar * bar = m_pScrollArea->horizontalScrollBar();
    //    int addH = 0;
    //    if(0 != bar)
    //    {
    //        addH = bar->height();
    //    }


    int h = 0;
    foreach (QWidget * wdg, m_widgets)
    {
        h += wdg->height() ;
    }


    return (m_pInnerWidget->height() /*+ addH*/ ) ;
}

void ScrollPanel::setWidgetSet(QList<CollapsedWidget*> widgetSet)
{
    m_widgetSet = widgetSet;
    foreach (CollapsedWidget * wdg, widgetSet)
    {
        addWidgetToSet(wdg);
    }
}

void ScrollPanel::addWidgetToSet(CollapsedWidget* widget)
{
    m_widgetSet.append(widget);
    connect(widget, SIGNAL(signalDragGlobPosChanged(QRect, QPoint,QMouseEvent::Type)),
            this, SLOT(slotWidgetPositionFromSetChanged(QRect, QPoint,QMouseEvent::Type )));

}

void ScrollPanel::removeWidgetFromSet(CollapsedWidget* widget)
{
    m_widgetSet.removeAll(widget);
    disconnect(widget);
}

void ScrollPanel::slotWidgetPositionFromSetChanged(QRect widgetGeom, QPoint pressedMousePos, QMouseEvent::Type eventType)
{
    CollapsedWidget * wdg = dynamic_cast<CollapsedWidget*>(sender());

    QRect rect = geometry();
    QPoint globalPos = mapToGlobal(QPoint(0,0));
    QRect globalRect( globalPos.x(), globalPos.y(),rect.width(),rect.height() );

    QPoint globalWidgPos;
    if(0 != wdg->parentWidget())
        globalWidgPos = wdg->mapToGlobal(wdg->pos());
    else
        globalWidgPos = wdg->pos();

    QRect globalWidgRect( globalWidgPos.x(), globalWidgPos.y(),widgetGeom.width(),widgetGeom.height() );



    if(eventType == QMouseEvent::Move)
    {

        //m_showFrameBorder = true;
        m_showFullFrame = true;
        update();
        if(!isVisible())
        {
            show();
        }



        if(globalRect.contains(pressedMousePos))
        {
            bool prevShow = m_showFrameBorder;
            m_showFrameBorder = true;
            if(prevShow != m_showFrameBorder)
                update();
        }
        else
        {
            bool prevShow = m_showFrameBorder;
            m_showFrameBorder = false;
            if(prevShow != m_showFrameBorder)
                update();
        }
    }

    if(eventType == QMouseEvent::MouseButtonRelease)
    {
        m_showFullFrame = false;
        update();

        if(globalRect.contains(pressedMousePos))
        {
            m_showFrameBorder = false;
            // update();

            if(0 != wdg)
                appendWidget(wdg);
        }
        else
        {
            if(0 == m_widgets.count())
            {
                hide();
            }
        }
    }

}
bool ScrollPanel::isNeedShowFull()
{
    return m_showFullFrame;
}

void ScrollPanel::hide()
{
    QWidget::hide();
    m_isVisible = false;
    emit signalGeomChanged();
}


void ScrollPanel::show()
{
    QWidget::show();
    m_isVisible = true;
    emit signalGeomChanged();
}

void ScrollPanel::setVisible(bool visible)
{
    QWidget::setVisible(visible);
    m_isVisible = visible;
    emit signalGeomChanged();
}

bool ScrollPanel::isVisible()
{
    bool vis = QWidget::isVisible();
    return m_isVisible;
}

void ScrollPanel::appendWidget(CollapsedWidget * widget, bool visible)
{
    //widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    widget->setParent(this);
    m_pMainLayout->addWidget(widget,1/*,Qt::AlignCenter*/);
    if(!m_widgets.contains(widget))
    {
        m_widgets.append(widget);
    }

    connect(widget, SIGNAL(signalSizeChanged(QSize)), this, SLOT( slotInsertedWidgetSizeChanged(QSize) ) );
    connect(widget, SIGNAL(signalRemoveMe()), this, SLOT( slotRemoveWidget( ) ));
    connect(widget, SIGNAL(signalVisibleNeedChange(bool)), this, SLOT( slotVisisbleNeedChange(bool) ));

    if(visible)
    {
        m_visisbleListWidget.append(widget);
        widget->show();
        show();
    }

    widget->setFixedWidth(width());



}

void ScrollPanel::slotVisisbleNeedChange(bool isVis)
{
    CollapsedWidget * wdg = dynamic_cast<CollapsedWidget*>(sender());

    if(m_widgets.contains(wdg) && isVis)
    {
        m_visisbleListWidget.append(wdg);
        show();
    }
    else
    {
        m_visisbleListWidget.removeAll(wdg);
        if(m_visisbleListWidget.isEmpty())
            hide();
    }
}

void ScrollPanel::slotRemoveWidget( )
{
    CollapsedWidget * widget = dynamic_cast<CollapsedWidget*>(sender());
    if(0 != widget)
    {
        removeWidget(widget);
    }
}

void ScrollPanel::appendWidgets(const QList<CollapsedWidget*> &widgets)
{
    foreach (CollapsedWidget * wdg, widgets)
    {
        appendWidget(wdg);
    }

}


void ScrollPanel::removeWidget(CollapsedWidget * widget)
{
//    QString str = m_pInnerWidget->styleSheet();
    m_pMainLayout->removeWidget(widget);
    m_widgets.removeOne(widget);
    disconnect(widget, SIGNAL(signalSizeChanged(QSize)), this, SLOT( slotInsertedWidgetSizeChanged(QSize) ) );
    disconnect(widget, SIGNAL(signalRemoveMe()), this, SLOT( slotRemoveWidget() ) );

    m_visisbleListWidget.removeAll(widget);

    if(!widget->defaultFixedSize().isNull())
    {
        widget->setFixedSize(widget->defaultFixedSize());
    }

    if(0 == m_widgets.count())
    {
        hide();
    }

//    widget->setStyleSheet(str);

}


void ScrollPanel::slotInsertedWidgetSizeChanged(QSize size)
{

    bool anyvisisble = false;
    foreach (CollapsedWidget* wdg, m_widgets) {
        if(wdg->isVisible())
        {
            anyvisisble = true;
        }
    }

    qDebug() << isVisible();
    qDebug() << anyvisisble;

    if(isVisible() && false == anyvisisble )
    {
        hide();
    }

    if(!isVisible() && anyvisisble )
    {
        show();
    }

    //    m_pInnerWidget->adjustSize();
    //    m_pScrollArea->adjustSize();
    //    m_pInnerWidget->adjustSize();
    //    adjustSize();

    emit signalGeomChanged();
    update();
    //    adjustSize();
}



QList<CollapsedWidget*> ScrollPanel::takeAllWidgets()
{
    foreach (QWidget * wdg, m_widgets)
    {
        m_pMainLayout->removeWidget(wdg);
    }
    hide();

    return m_widgets;
}

void ScrollPanel::paintEvent(QPaintEvent * ev)
{
    QPainter pr(this);

    if(true == m_needUpdateBackground)
    {
        //pr.setOpacity(1.0);
        pr.drawPixmap(0,0,m_backgroundPixmap.width(), m_backgroundPixmap.height(),m_backgroundPixmap);

    }

    if(m_showFrameBorder)
    {
        pr.setPen(m_borderPen);
        pr.drawRect(rect());
    }

    if( !m_isInnerPanel || m_showFullFrame)
    {
        //pr.setOpacity(m_painterBackgroundOpacity);
        pr.setPen(m_pen);
        pr.setBrush(m_brush);
        pr.drawRect(rect());
    }
    else
    {
        int rectH = m_pInnerWidget->height();

        int y = m_pInnerWidget->pos().y() ;
        int bordersPx = 8;
        if(y > bordersPx)
        {
            y = y - bordersPx;
            rectH = rectH + 2*bordersPx;
        }

        if(rectH > height())
        {
            rectH = height();
        }

        bool anyVisible = false;
        foreach (CollapsedWidget * wdg, m_widgets)
        {
            if(wdg->isVisible())
            {
                anyVisible = true;
                break;
            }
        }

        if(anyVisible)
        {
            /*pr.setOpacity(m_painterBackgroundOpacity);
            pr.setPen(m_pen);
            pr.setBrush(m_brush);
            pr.drawRect(rect().x() + 2, y , rect().width() - 4, rectH);*/
        }
    }
}
