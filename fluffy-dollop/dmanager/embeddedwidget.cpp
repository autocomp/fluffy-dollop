#include "embeddedwidget.h"
#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QEvent>
#include <QLabel>
#include <QMargins>
#include <QMargins>
#include <QMetaProperty>
#include <QMouseEvent>
#include <QPainter>
#include <QPalette>
#include <QStyleOption>
#include <QTimer>
#include <QVBoxLayout>

#include "embeddedapp.h"
#include "embeddedheader.h"
#include "embeddedpanel.h"
#include "embeddedprivate.h"
#include "embeddedsubiface.h"
#include "embeddedwidgetheader.h"

#ifdef QT_V5
#include <QInputMethodQueryEvent>
#include <QWindow>
#endif

using namespace ew;

EmbeddedWidget::EmbeddedWidget(QWidget *parent)
    : QDialog(parent)
      , m_minDefaultSize(50, 50)
      , m_resizeInProcess(false)
      , m_pAdjustSizeTimer(0)
{
    setWindowModality(Qt::NonModal);
    setFocusPolicy(Qt::ClickFocus);
    m_activeWinBorderColor = QColor(Qt::transparent);
    m_isActive = false;
    this->setMouseTracking(true);

    m_borderSize = 4;
    m_moveBoundSize = 10;
    m_highlightWidth = 2;
    m_highlightColor = QColor(Qt::blue);

    m_pHeaderWidget = 0;
    m_windowMoveMode = false;
    m_minSize = QSize(0, 0);
    m_maxSize = QSize(0, 0);
    m_prevSize = QSize(0, 0);
    m_resizeAllowed = true;
    m_moveAllowed = true;

    m_mainViewWidget = 0;
    m_windowMoveMode = false;
    m_windowResizeMode = false;
    m_highlightedSection = HS_NO_HIGHLIGHT;
    m_isCollapsed = false;
    m_activeBound = AB_NO_ACTIVE_BOUND;
    //m_userResizable = true;

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Maximum);
    setWindowFlags(Qt::FramelessWindowHint);
    setWindowFlags(Qt::CustomizeWindowHint);

    setObjectName("EmbeddedWidget_EmbeddedWidget");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    QWidget *mainBackgroundWidget = new QWidget(this);
    mainBackgroundWidget->setObjectName("EmbeddedWidget_mainBackgroundWidget");
    mainLayout->addWidget(mainBackgroundWidget, 10);

    mainLayout->setMargin(m_borderSize);
    m_pMainLayout  = new QVBoxLayout(mainBackgroundWidget);
    m_pMainLayout->setSpacing(0);
    m_pMainLayout->setMargin(0);


    /*Заполняем меню окна*/

    m_pMainViewWidgetBackground = new QWidget(mainBackgroundWidget);
    m_pMainViewWidgetBackground->setObjectName("EmbeddedWidget_MainViewBackground");
    m_pMainViewWidgetLayout = new QVBoxLayout(m_pMainViewWidgetBackground);

    m_pMainViewWidgetLayout->setMargin(0);
    //m_pMainViewWidgetLayout->setSpacing(0);


    m_pSeparatorFrame = new QFrame(mainBackgroundWidget);
    m_pSeparatorFrame->setObjectName("EmbeddedWindow_FrameSeparator");
    m_pSeparatorFrame->setFrameStyle(QFrame::HLine);

    m_pMainLayout->addWidget(m_pSeparatorFrame);
    m_pMainLayout->addWidget(m_pMainViewWidgetBackground, 10);

    m_pResizeTimer = new QTimer(this);
    m_pResizeTimer->setSingleShot(true);
    m_pResizeTimer->setInterval(50);
    connect(m_pResizeTimer, SIGNAL(timeout()), this, SLOT(checkResize()));

    setMouseTracking(true);
    //installEventFilter(this);
    m_pMainViewWidgetBackground->setMouseTracking(true);
    m_pMainViewWidgetBackground->installEventFilter(this);
    repaint();
    setMinimumSize(0, 0);
    m_pAdjustSizeTimer =  new QTimer(this);
    m_pAdjustSizeTimer->setSingleShot(true);
    m_pAdjustSizeTimer->setInterval(10);
    connect(m_pAdjustSizeTimer, SIGNAL(timeout()), this, SLOT(slotAdjustWidgetPrivate()), Qt::QueuedConnection);
}


void EmbeddedWidget::setHighlightedSection(ew::HighlightSection section)
{
    if(EWT_GROUP == widgetType())
    {
        qWarning() << __FUNCTION__ << "Attempt to set highlight in group. Ignored.";
        return;
    }

    m_highlightedSection = section;
}


ew::HighlightSection EmbeddedWidget::highlightedSection()
{
    return m_highlightedSection;
}


void EmbeddedWidget::setMoveAllowed(bool allowed)
{
    m_moveAllowed = allowed;
}


bool EmbeddedWidget::isMoveAllowed()
{
    return m_moveAllowed;
}


void EmbeddedWidget::setResizeAllowed(bool allowed)
{
    m_resizeAllowed = allowed;
    //installStyle();
}


bool EmbeddedWidget::isResizeAllowed()
{
    return m_resizeAllowed;
}


void EmbeddedWidget::setHeaderVisisbility(bool headerVis)
{
    if(nullptr != header())
    {
        header()->setVisibleFlag(headerVis);
        m_pSeparatorFrame->setVisible(headerVis);
    }
}


void EmbeddedWidget::setHeaderVisisbilityPrivate(bool headerVis)
{
    if(nullptr != header())
    {
        header()->setVisible(headerVis);
        m_pSeparatorFrame->setVisible(headerVis);
    }
}


bool EmbeddedWidget::isHeaderVisible()
{
    return header()->isVisibleFlagSetted();
}


void EmbeddedWidget::setWidget(QWidget *widget)
{
    widget->setParent(this);
    m_mainViewWidget = widget;

    if(widget->isModal())
    {
        setModal(true);
    }

    m_pMainViewWidgetLayout->addWidget(widget, 10);
    connect(widget, SIGNAL(destroyed(QObject *)), this, SLOT(slotWidgetDestroyed(QObject *)));

    QDialog *dlg = dynamic_cast<QDialog *>(widget);

    if(nullptr != dlg)
    {
        connect(dlg, SIGNAL(finished(int)), this, SLOT(slotWidgetDestroyed()));
    }

    widget->setMouseTracking(true);
    widget->installEventFilter(this);
    setMinimumSize(0, 0);
}


QWidget *EmbeddedWidget::getWidget()
{
    return m_mainViewWidget;
}


void EmbeddedWidget::slotWidgetDestroyed(QObject *obj)
{
    if(nullptr != obj)
    {
        if(m_mainViewWidget == dynamic_cast<QWidget *>(obj))
        {
            emit signalWidgetClosed();
        }
    }
    else
    {
        if(m_mainViewWidget == dynamic_cast<QWidget *>(sender()))
        {
            emit signalWidgetClosed();
        }
    }
}


EmbeddedWidgetType EmbeddedWidget::widgetType()
{
    return ew::EWT_WIDGET;
}


void EmbeddedWidget::setActiveWidget(bool isActive)
{
    m_isActive = isActive;
    update();
}


QMargins EmbeddedWidget::getMargins()
{
    QMargins margins;
    QWidget *tmpParent;

    if(0 != m_mainViewWidget)
    {
        tmpParent = m_mainViewWidget;
    }
    else
    {
        tmpParent = m_pMainViewWidgetBackground;
    }


    QPoint topLeft;
    QSize viewWidgSize;
    QPoint bottomRight;

    while(this != tmpParent || 0 != tmpParent)
    {
        topLeft = tmpParent->mapToParent(QPoint(0, 0));
        viewWidgSize = tmpParent->geometry().size();
        bottomRight = tmpParent->mapToParent(QPoint(viewWidgSize.width(), viewWidgSize.height()));
        tmpParent = tmpParent->parentWidget();;
    }

    margins = QMargins(topLeft.x(), topLeft.y(), bottomRight.x(), bottomRight.y());
    return margins;
}


//void EmbeddedWidget::setVisisbleButton(EmbeddedHeader::WindowHeaderButtons button, bool vis)
//{


//}

void EmbeddedWidget::setHeader(const EmbeddedHeaderStruct &headerStruct)
{
    bool needEnableUpdates = true;

    if(updatesEnabled())
    {
        needEnableUpdates = true;
        setUpdatesEnabled(false);
    }

    delete m_pHeaderWidget;
    m_pHeaderWidget = 0;

    m_pHeaderWidget = new ew::EmbeddedWidgetHeader(this);

    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_STYLE, headerStruct.hasStyleButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_SETTINGS, headerStruct.hasSettingsButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_PINUNPIN, headerStruct.hasPinButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_MIDGET, headerStruct.hasMidgetButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_LOCKUNLOCK, headerStruct.hasLockButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_HIDE_HEADER, headerStruct.hasHideHeaderButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_GLOBAL_ONPARENT_MOVE, headerStruct.hasGlobalMoveButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_WHAT_THIS, headerStruct.hasWhatButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_COLLAPSE, headerStruct.hasCollapseButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_FULLNORMSCREEN, headerStruct.hasMinMaxButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_CLOSE, headerStruct.hasCloseButton);

    m_pHeaderWidget->setToolTip(headerStruct.tooltip);
    m_pHeaderWidget->setText(headerStruct.windowTitle);
    m_pHeaderWidget->setDescription(headerStruct.descr);
    m_pHeaderWidget->setIcon(headerStruct.headerPixmap);

    setWindowTitleText(headerStruct.windowTitle);

    m_pMainLayout->insertWidget(0, m_pHeaderWidget);

    connect(m_pHeaderWidget, SIGNAL(signalWindowEvent(EmbeddedHeader::WindowHeaderEvents,bool *))
            , this, SLOT(slotHeaderWindowEvents(EmbeddedHeader::WindowHeaderEvents,bool *)));


    connect(m_pHeaderWidget, SIGNAL(signalMoveWindowRequest(int,int,bool *))
            , this, SLOT(slotMoveWindowRequest(int,int,bool *)));

    connect(m_pHeaderWidget, SIGNAL(signalWindowDropped()),
            SLOT(onSignalWindowDropped()));

    if(needEnableUpdates)
    {
        setUpdatesEnabled(true);
    }

    m_pHeaderWidget->installEventFilter(this);
}


ew::EmbeddedWidgetHeader *EmbeddedWidget::header()
{
    ew::EmbeddedWidgetHeader *head = dynamic_cast<ew::EmbeddedWidgetHeader *>(m_pHeaderWidget);
    return head;
}


void EmbeddedWidget::setMinMaxSize(QSize minSize, QSize maxSize)
{
    setMinimumSize(0, 0);
    m_minSize = minSize;
    m_maxSize = maxSize;
}


void EmbeddedWidget::slotResizeCheckTimeout()
{
    checkResize();
}


void EmbeddedWidget::slotHeaderWindowEvents(EmbeddedHeader::WindowHeaderEvents ev, bool *acceptFlag)
{
    ew::EmbeddedWidgetBaseStructPrivate *privStruct = ewApp()->getPrivateStruct(id());
    EmbeddedSubIFace *subIFace = ewApp()->getSubIFaceById(id());

    switch(ev)
    {
      case EmbeddedWidgetHeader::WHE_SHOW_SETTINGS:
      {
          ewApp()->showSettingsWidget(id());
      } break;
      case EmbeddedWidgetHeader::WHE_SHOW_STYLE_WIDGET:
      {
          ewApp()->showStyleWidget(id());
      } break;
      case EmbeddedWidgetHeader::WHE_START_MOVE:
      {
          raise();

          if(0 != acceptFlag)
          {
              *acceptFlag = m_moveAllowed;
              ewApp()->slotWidgetPosChanged(id(), EmbeddedHeader::MS_START);
          }
      } break;
      case EmbeddedWidgetHeader::WHE_END_MOVE:
      {
          if(0 != acceptFlag)
          {
              *acceptFlag = true;
              ewApp()->slotWidgetPosChanged(id(), EmbeddedHeader::MS_END);
          }
      } break;
      case EmbeddedWidgetHeader::WHE_CLOSE:
      {
          bool flag = true;

          if(subIFace)
          {
              subIFace->visibleChanged(false, &flag);
              subIFace->closed(&flag);
          }

          if(0 != acceptFlag)
          {
              *acceptFlag = flag;
          }

          if(flag)
          {
              if((ewApp()->getMainWindowId() != id()) ||
                 ((ewApp()->getMainWindowId() != id()) && !ewApp()->isCloseDenied()))
              {
                  close();
                  emit signalWidgetClosed();
              }
              else
              {
                  ewApp()->close();
              }

              return;
          }
      } break;
      case EmbeddedWidgetHeader::WHE_COLLAPSE:
      {
          bool flag = true;

          if(subIFace)
          {
              subIFace->minimized(&flag);
          }

          if(0 != acceptFlag)
          {
              *acceptFlag = flag;
          }

          if(flag)
          {
              showMinimized();
              m_isCollapsed = true;
              privStruct->embWidgetStruct->collapsed = true;
          }
      } break;
      case EmbeddedWidgetHeader::WHE_MAXIMIZE:
      {
          bool flag = true;

          if(subIFace)
          {
              subIFace->maximized(&flag);
          }

          if(0 != acceptFlag)
          {
              *acceptFlag = flag;
          }

          if(flag)
          {
              if(0 != privStruct)
              {
                  privStruct->embWidgetStruct->maximized = true;
                  ewApp()->embWidgetChanged(id());
              }

              setGeometry(QApplication::desktop()->availableGeometry(this));
              setResizeAllowed(false);
              setMoveAllowed(false);
              privStruct->embWidgetStruct->collapsed = false;
              //showMaximized();
          }
      } break;
      case EmbeddedWidgetHeader::WHE_SHOW_NORMAL:
      {
          bool flag = true;

          if(subIFace)
          {
              subIFace->resored(&flag);
          }

          if(0 != acceptFlag)
          {
              *acceptFlag = flag;
          }

          if(flag)
          {
              privStruct->embWidgetStruct->maximized = false;
              privStruct->embWidgetStruct->collapsed = false;
              m_isCollapsed = false;
              setResizeAllowed((privStruct->embWidgetStruct->szPolicy != ESP_FIXED));
              setMoveAllowed((privStruct->embWidgetStruct->headerVisible));
              showNormal();
              ewApp()->changeWidgetGeometry(id(), privStruct->embWidgetStruct->alignPoint, privStruct->embWidgetStruct->size);
              ewApp()->embWidgetChanged(id());
          }
      } break;
      case EmbeddedWidgetHeader::WHE_HIDE_HEADER:
      {
          if(0 != acceptFlag)
          {
              *acceptFlag = true;
          }

          setHeaderVisisbility(false);
      }
      case EmbeddedWidgetHeader::WHE_MIDGET:
      {
          if(0 != acceptFlag)
          {
              *acceptFlag = true;
          }

          ewApp()->showMidget(id());
      }
      default:
          break;
    }

    ewApp()->embWidgetChanged(id());
}


/**/
void EmbeddedWidget::slotMoveWindowRequest(int dx, int dy, bool *acceptFlag)
{
    emit signalWindowDragged(this->geometry());

    if(0 != acceptFlag)
    {
        *acceptFlag = true;
    }

    int newPosX = pos().x() + dx;
    int newPosY = pos().y() + dy;
    QRect desktopRect;
    QWidget *parentWdg = parentWidget();

    if(nullptr == parentWdg)
    {
        desktopRect = QApplication::desktop()->rect();
    }
    else
    {
        desktopRect = QRect(0, 0, parentWdg->size().width(), parentWdg->size().height());
    }


    QRect currentRect = geometry(); // Пр
    QRect desiredRect(currentRect);
    desiredRect.moveTo(newPosX, newPosY);

    if(!desktopRect.contains(currentRect))
    {
        QRect currentAreaRect = desktopRect.intersected(currentRect);
        QRect desiredAreaRect = desktopRect.intersected(desiredRect);
        int currentArea = currentAreaRect.width() * currentAreaRect.height();
        int desiredArea = desiredAreaRect.width() * desiredAreaRect.height();

        if(currentArea < desiredArea)
        {
            ewApp()->changeWidgetGeometry(id(), QPointF(newPosX, newPosY), size());
            ewApp()->slotWidgetPosChanged(id(), EmbeddedHeader::MS_MOVE);

            return;
        }
    }

    if(!desktopRect.contains(desiredRect))
    {
        if(newPosX < 0)
        {
            newPosX = 0;
        }

        if(newPosY < 0)
        {
            newPosY = 0;
        }

        if(desiredRect.right() > desktopRect.right())
        {
            newPosX = desktopRect.width() - width();
        }

        if(desiredRect.bottom() > desktopRect.bottom())
        {
            newPosY = desktopRect.height() - height();
        }
    }

    ewApp()->changeWidgetGeometry(id(), QPointF(newPosX, newPosY), size());
    ewApp()->slotWidgetPosChanged(id(), EmbeddedHeader::MS_MOVE);
}


void EmbeddedWidget::onSignalWindowDropped()
{
    emit signalWindowDropped(this->geometry());
}


void EmbeddedWidget::setWidgetMaximized(bool isMaximized)
{
    ew::EmbeddedWidgetHeader *head = dynamic_cast<ew::EmbeddedWidgetHeader *>(m_pHeaderWidget);

    if(nullptr != head)
    {
        head->setWidgetMaximized(isMaximized);
    }

    //    m_isLocked = isLocked;
}


void EmbeddedWidget::setLocked(bool isLocked)
{
    ew::EmbeddedWidgetHeader *head = dynamic_cast<ew::EmbeddedWidgetHeader *>(m_pHeaderWidget);

    if(nullptr != head)
    {
        head->setLocked(isLocked);
    }

    m_isLocked = isLocked;
}


bool EmbeddedWidget::isLocked()
{
    return m_isLocked;
}


void EmbeddedWidget::setGlobalMove(bool isGlobalMove)
{
    ew::EmbeddedWidgetHeader *head = dynamic_cast<ew::EmbeddedWidgetHeader *>(m_pHeaderWidget);

    if(nullptr != head)
    {
        head->setGlobalMove(isGlobalMove);
    }

    m_isGlobalMove = isGlobalMove;
}


bool EmbeddedWidget::isGlobalMove()
{
    return m_isGlobalMove;
}


void EmbeddedWidget::setPinned(bool isLocked)
{
    ew::EmbeddedWidgetHeader *head = dynamic_cast<ew::EmbeddedWidgetHeader *>(m_pHeaderWidget);

    if(nullptr != head)
    {
        head->setPinned(isLocked);
    }

    m_isPinned = isLocked;
}


bool EmbeddedWidget::isPinned()
{
    return m_isPinned;
}


QMargins EmbeddedWidget::getEmbeddedViewMargins()
{
    QWidget *parent = m_pMainViewWidgetBackground;
    QMargins sumMarg = QMargins(m_pMainViewWidgetLayout->margin() + m_pMainViewWidgetLayout->spacing()
                                , m_pMainViewWidgetLayout->margin() + m_pMainViewWidgetLayout->spacing()
                                , m_pMainViewWidgetLayout->margin() + m_pMainViewWidgetLayout->spacing()
                                , m_pMainViewWidgetLayout->margin() + m_pMainViewWidgetLayout->spacing());

    do
    {
        parent = parent->parentWidget();
        QMargins m = parent->contentsMargins();

        if(0 != parent->layout())
        {
            sumMarg = QMargins(m.left() + sumMarg.left() /*+ parent->layout()->margin() + parent->layout()->spacing()*/
                               , m.top() + sumMarg.top() /*+ parent->layout()->margin() + parent->layout()->spacing()*/
                               , m.right() + sumMarg.right() /*+ parent->layout()->margin() + parent->layout()->spacing()*/
                               , m.bottom() + sumMarg.bottom() /*+ parent->layout()->margin() + parent->layout()->spacing()*/);
        }
        else
        {
            sumMarg = QMargins(m.left() + sumMarg.left()
                               , m.top() + sumMarg.top()
                               , m.right() + sumMarg.right()
                               , m.bottom() + sumMarg.bottom() );
        }
    } while(this != parent);

    if(0 != header())
    {
        if(header()->isVisibleFlagSetted())
        {
            sumMarg.setTop(sumMarg.top() + header()->height()
                           + m_pSeparatorFrame->sizeHint().height() + m_pMainLayout->spacing());
        }
    }

    return sumMarg;
}


void EmbeddedWidget::setActiveWinBorderColor(QColor col)
{
    m_activeWinBorderColor = col;
}


QString EmbeddedWidget::getActiveWinBorderSize()
{
    return QString::number(m_activeWinBorderSize);
}


QString EmbeddedWidget::getActiveWinBorderColor()
{
    return m_activeWinBorderColor.name();
}


void EmbeddedWidget::setActiveWinBorderSize(QString szString)
{
    m_activeWinBorderSize = szString.remove("px").toInt();
}


QSize EmbeddedWidget::defaultFixedSize()
{
    return m_minDefaultSize;
}


bool EmbeddedWidget::eventFilter(QObject *obj, QEvent *ev)
{
    if(obj == m_mainViewWidget)
    {
        switch(ev->type())
        {
//          case (QEvent::InputMethodQuery):
//          {
//              auto ptr = ewApp()->getSubIFaceById(id());
//              QWindow *wnd = ptr->getWindow();

//              if(0 != wnd)
//              {
////                  wnd->requestActivate();
////                  QInputMethodQueryEvent *inputEv = dynamic_cast<QInputMethodQueryEvent *>(ev);

////                  if(inputEv)
////                  {
////                      Qt::InputMethodQueries qq = inputEv->queries();

////                      for(int i = 0; i < 32; i++)
////                      {
////                          if(0 != (qq & (1 << i)))
////                              qDebug() << "attr type" << (Qt::InputMethodQueries)(1 << i) << " val" << inputEv->value((Qt::InputMethodQueries)(1 << i));
////                      }
////                  }
//              }
//          } break;
          case (QEvent::KeyPress):
          {
              QKeyEvent *kPressEv = dynamic_cast<QKeyEvent *>(ev);

              if(kPressEv)
              {
                  auto ptr = ewApp()->getSubIFaceById(id());

//                  if(ptr)
//                  {
//                      QWindow *wnd = ptr->getWindow();

//                      if(wnd)
//                      {
//                          wnd->requestActivate();
//                          QKeyEvent *event = new QKeyEvent(QEvent::KeyPress, kPressEv->key()
//                                                           , kPressEv->modifiers(), kPressEv->text() );
//                          QApplication::sendEvent(wnd, event);
//                      }
//                  }

                  if(kPressEv->key() == Qt::Key_Escape)
                  {
                      kPressEv->accept();
                      return true;
                  }
              }
          } break;
          case (QEvent::Hide):
          {
              if(!m_denyHide)
              {
                  //wdg->setVisible(true);
                  //emit signalVisibleNeedChangeInternal(false);
              }

              /// TODO - перехватываем события скрытия окон
              ev->ignore();
              emit signalVisibleNeedChangeInternal(false);
              return false;
          } break;

          case (QEvent::Show):
          {
              emit signalVisibleNeedChangeInternal(true);
              ev->ignore();
              return false;
          } break;
          case (QEvent::Close ):
          {
              emit signalVisibleNeedChangeInternal(false);
              ev->ignore();
              return false;
          }
          case (QEvent::LayoutRequest):
          {
              auto ptr = ewApp()->getWidgetStructById(id());

              if(ptr)
              {
                  if(ptr->autoAdjustSize)
                  {
                      if(m_pAdjustSizeTimer)
                      {
                          m_pAdjustSizeTimer->start();
                      }
                  }
              }

              //emit signalAdjustWidgetPrivate();
          } break;
          default:
          {
              return false;
          } break;
        }
    }

    if(obj != this && (ev->type() == QEvent::Leave ||
                       ev->type() == QEvent::Enter ||
                       ev->type() == QEvent::FocusIn ||
                       ev->type() == QEvent::FocusOut ||
                       ev->type() == QEvent::MouseMove ||
                       ev->type() == QEvent::HoverEnter ||
                       ev->type() == QEvent::HoverLeave ||
                       ev->type() == QEvent::HoverMove) )
    {
        checkResize();
    }

//    if(ev->type() == QEvent::WindowActivate)
//    {
//        auto ptr = ewApp()->getSubIFaceById(id());

//        if(ptr)
//        {
//            QWindow *wnd = ptr->getWindow();

//            if(0 != wnd)
//            {
//                //wnd->blockSignals(true);
//                //wnd->requestActivate();
//                //wnd->setFlags(wnd->flags() & (0xFFFFFFFF & Qt::WindowDoesNotAcceptFocus));
//                m_mainViewWidget->setFocusPolicy(Qt::StrongFocus);
//                m_mainViewWidget->setFocus();
//                m_mainViewWidget->grabKeyboard();
//                //wnd->setKeyboardGrabEnabled(true);
//                //wnd->blockSignals(false);
//            }
//        }
//    }

//    qDebug() << "EmbeddedWidget::eventFilter: widget event type: " << ev->type();
    QWidget::eventFilter(obj, ev);
    return false;
}


void EmbeddedWidget::slotAdjustWidgetPrivate()
{
    ewApp()->adjustWidgetSize(id());
}


void EmbeddedWidget::setVisibleHeader(bool vis)
{
    if(0 != m_pHeaderWidget)
    {
        m_pHeaderWidget->setVisible(vis);
        m_pHeaderWidget->setVisibleFlag(vis);
    }
    else
    {
        qWarning() << "EmbeddedWidget::setVisibleHeader(): WARN! Header not installed!";
    }

    ewApp()->embWidgetChanged(id());
}


QString EmbeddedWidget::getWindowTitleText()
{
    if(0 != m_pHeaderWidget)
    {
        return m_pHeaderWidget->text();
    }

    return "";
}


void EmbeddedWidget::setWindowTitleText(QString title)
{
    if(0 != m_pHeaderWidget)
    {
        m_pHeaderWidget->setText(title);
    }

    emit signalTitleChanged(title);
}


//void EmbeddedWidget::moveOnlyOnParent(bool on)
//{
//    m_moveOnlyOnParent = on;

//    if(m_moveOnlyOnParent)
//    {
//        m_pGlobalMoveBtn->setToolTip(tr("Global move"));
//        m_pGlobalMoveBtn->setIcon(m_onGlobalIcon);
//    }
//    else
//    {
//        m_pGlobalMoveBtn->setToolTip(tr("Local move"));
//        m_pGlobalMoveBtn->setIcon(m_onParentIcon);
//    }

//    emit signalWidgetMovePolicyChanged(m_id, on);


//}


void EmbeddedWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);

    QStyleOption opt;
    opt.init(this);
    QPainter pr(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &pr, this);

    if(m_isActive && m_resizeAllowed)
    {
        if(m_activeWinBorderColor != Qt::transparent)
        {
            //QBrush br =  QBrush(m_activeWinBorderColor);
            QPen pn;
            pn.setColor(m_activeWinBorderColor);
            pn.setWidth(m_activeWinBorderSize);
            pn.setJoinStyle(Qt::MiterJoin);
            pr.setPen(pn);
            pr.drawRect(m_activeWinBorderSize / 2, m_activeWinBorderSize / 2
                        , width() - m_activeWinBorderSize, height() - m_activeWinBorderSize);
        }
    }

    if(HS_NO_HIGHLIGHT != m_highlightedSection)
    {
        int hMargin = 2;
        int vMargin = 2;
        int cr = 15; //circleRadius
        pr.setRenderHint(QPainter::HighQualityAntialiasing, true);
        QPen pn(m_highlightColor);
        pn.setWidth(m_highlightWidth);
        pn.setWidth(10);
        pn.setCapStyle(Qt::RoundCap);
        pn.setColor(QColor(0, 50, 200, 128));
        pr.setPen(pn);

        switch(m_highlightedSection)
        {
          case ew::HS_LEFT:
              pr.drawLine(QPoint(0 + hMargin, 0 + vMargin),
                          QPoint(0 + hMargin, height() - vMargin));
              break;
          case ew::HS_TOP_LEFT:
              pr.drawPie(QRect(-cr + hMargin, -cr + vMargin, 2 * cr, 2 * cr), 270 * 16, 90 * 16);
              break;
          case ew::HS_TOP:
              pr.drawLine( QPoint(width() - hMargin, 0 + vMargin),
                           QPoint(0 + hMargin, 0 + vMargin));
              break;
          case ew::HS_BOTTOM:
              pr.drawLine(
                  QPoint(0 + hMargin, height() - vMargin),
                  QPoint(width() - hMargin, height() - vMargin)
                  );
              break;

          case ew::HS_BOTTOM_LEFT:
              pr.drawPie(QRect(-15 + hMargin, height() - 15 - vMargin, 30, 30)
                         , 0 * 16, 90 * 16);
              break;
          case ew::HS_CENTER:
              pr.drawEllipse(width() / 2 - 15, height() / 2 - 15, 30, 30);
              break;
          case ew::HS_TOP_RIGHT:
              pr.drawPie(QRect(width() - 15 - hMargin, -15 + vMargin, 30, 30), 180 * 16, 90 * 16);
              break;
          case ew::HS_RIGHT:
              pr.drawLine(
                  QPoint(width() - hMargin, 0 + vMargin),
                  QPoint(width() - hMargin, height() - vMargin)
                  );
              break;
          case ew::HS_BOTTOM_RIGHT:
              pr.drawPie(QRect(width() - 15 - hMargin, height() - 15 - vMargin,
                               30, 30), 90 * 16, 90 * 16);
              break;
          default:
              break;
        }
    }
}


void EmbeddedWidget::removeWidget(QWidget *widget)
{
    m_pMainViewWidgetLayout->removeWidget(widget);
    widget->setParent(0);
    m_mainViewWidget = 0;
}


bool EmbeddedWidget::isCollapsed()
{
    return m_isCollapsed;
}


void EmbeddedWidget::setCollapsed(bool isCollapsed)
{
    bool res = false;

    if(isCollapsed)
    {
        slotHeaderWindowEvents(EmbeddedHeader::WHE_COLLAPSE, &res);
    }
    else
    {
        slotHeaderWindowEvents(EmbeddedHeader::WHE_SHOW_NORMAL, &res);
    }

    if(res)
    {
        m_isCollapsed = isCollapsed;
    }
}


void EmbeddedWidget::hideEvent(QHideEvent *e)
{
    Q_UNUSED(e);
    m_isVisible = false;
    emit signalVisibleNeedChange(m_isVisible);
    //    if(m_prevSize != e->size())
    //    {
    //        m_prevSize = e->size();
    //        emit signalSizeChanged(e->size());
    //    }
}


bool EmbeddedWidget::isVisible()
{
    return m_isVisible;
}


void EmbeddedWidget::showEvent(QShowEvent *e)
{
    //m_resizeLabel->move( width() - m_resizeLabel->width(), height() - m_resizeLabel->height());

    Q_UNUSED(e);
    m_isVisible = true;
    emit signalVisibleNeedChange(m_isVisible);
    //    if(m_prevSize != e->size())
    //    {
    //        m_prevSize = e->size();
    //        emit signalSizeChanged(e->size());
    //    }
}


void EmbeddedWidget::resizeEvent(QResizeEvent *e)
{
    //qDebug() << __FUNCTION__ << " " << e->size() << " " << id();
    //    if(5 == id())
    //    {
    //        int a = 5;
    //    }
    QWidget::resizeEvent(e);
}


void EmbeddedWidget::setVisible(bool visible)
{
    m_isVisible = visible;
    emit signalVisibleNeedChange(visible);
    QDialog::setVisible(visible);
    emit signalVisibleNeedChange(visible);

    if(m_mainViewWidget)
    {
        if(isModal())
        {
            setUpdatesEnabled(false);
            blockSignals(false);

            m_mainViewWidget->setVisible(isModal());

            blockSignals(true);
            setUpdatesEnabled(true);
        }
    }

    /// TODO надо ли скрывать?
    //m_pViewPanel->setVisible(true);
}


void EmbeddedWidget::keyPressEvent(QKeyEvent *e)
{
    if(Qt::Key_H == e->key() && (e->modifiers() & Qt::CTRL) && (e->modifiers() & Qt::ShiftModifier))
    {
        setHeaderVisisbility(!isHeaderVisible());
        return;
    }

    QWidget::keyPressEvent(e);
}


void EmbeddedWidget::mousePressEvent(QMouseEvent *e)
{
    setFocus();

    if(m_activeBound != AB_NO_ACTIVE_BOUND && Qt::LeftButton == e->button())
    {
        m_resizeInProcess = true;
        m_pressedGlobalLastPos = e->globalPos();
    }
}


void EmbeddedWidget::mouseMoveEvent(QMouseEvent *e)
{
    int dx = e->globalX() - m_pressedGlobalLastPos.x();
    int dy = e->globalY() - m_pressedGlobalLastPos.y();



    if(m_resizeInProcess && m_resizeAllowed)
    {
        QRect parentRect = ewApp()->getParentViewRect(id());/*
                                                               auto wdgPtr = ewApp()->getPrivateStruct(id());
                                                               ew::EmbeddedWidgetBaseStructPrivate* parentPtr = 0;
                                                               if(nullptr != wdgPtr)
                                                               {
                                                               parentPtr = ewApp()->getPrivateStruct(wdgPtr->parentId);
                                                               if(parentPtr)
                                                               {
                                                               parentRect = parentPtr->embWidgetPtr->getViewRect();
                                                               }
                                                               }*/
        QRect g = geometry();

        if(m_activeBound == AB_TOPRIGHT ||
           m_activeBound == AB_RIGHT ||
           m_activeBound == AB_BOTTOM_RIGHT)
        {
            int newRight = g.right() + dx;

            if(!parentRect.isEmpty() && (parentRect.right() < newRight))
            {
                g.setRight(parentRect.right());
            }
            else
            {
                g.setRight(newRight);
            }
        }

        if(m_activeBound == AB_TOPLEFT ||
           m_activeBound == AB_LEFT ||
           m_activeBound == AB_BOTTOM_LEFT)
        {
            int newLeft = (g.left() + dx);

            if(!parentRect.isEmpty() && (parentRect.left() > newLeft))
            {
                if((g.right() - parentRect.left()) > qMax(minimumSize().width(), m_minSize.width()))
                {
                    g.setLeft(parentRect.left());
                }
            }
            else
            {
                if((g.width() - dx > qMax(minimumSize().width(), m_minSize.width()))
                   && (g.width() - dx < qMin(maximumSize().width(), m_maxSize.width())) )
                {
                    g.setLeft(newLeft);
                }
            }
        }

        if(m_activeBound == AB_TOP ||
           m_activeBound == AB_TOPRIGHT ||
           m_activeBound == AB_TOPLEFT)
        {
            {
                int newTop = (g.top() + dy);

                if(!parentRect.isEmpty() && (parentRect.top() > newTop))
                {
                    if((g.bottom() - parentRect.top()) > qMax(minimumSize().height(), m_minSize.height()))
                    {
                        g.setTop(parentRect.top());
                    }
                }
                else
                {
                    if((g.height() - dy > qMax(minimumSize().height(), m_minSize.height()))
                       && (g.height() - dy < qMin(maximumSize().height(), m_maxSize.height())) )
                    {
                        g.setTop(newTop);
                    }
                }
            }
        }

        if(m_activeBound == AB_BOTTOM ||
           m_activeBound == AB_BOTTOM_LEFT ||
           m_activeBound == AB_BOTTOM_RIGHT)
        {
            int newBottom = g.bottom() + dy;

            if(!parentRect.isEmpty() && (parentRect.bottom() < newBottom))
            {
                g.setBottom(parentRect.bottom());
            }
            else
            {
                int desktopBottom = qApp->desktop()->availableGeometry().bottom();

                if(desktopBottom <= newBottom)
                {
                    g.setBottom(desktopBottom);
                }
                else
                {
                    g.setBottom(newBottom);
                }
            }
        }

        ewApp()->changeWidgetGeometry(id(), g.topLeft(), g.size());
    }

    if(m_windowMoveMode)
    {
        emit signalDragGlobPosChanged(rect(), QPoint(dx, dy), e->type());
    }

    m_pressedGlobalLastPos = e->globalPos();
    m_pResizeTimer->start();
}


QSize EmbeddedWidget::getViewSize()
{
    return m_pMainViewWidgetBackground->size();
}


QRect EmbeddedWidget::getViewRect()
{
    QRect r = m_pMainViewWidgetBackground->contentsRect();

    //    if(0 != m_pMainViewWidgetBackground->layout())
    //    {
    //        QLayout* lt = m_pMainViewWidgetBackground->layout();
    //        r.setLeft(r.left() - lt->margin() - lt->spacing());
    //        r.setBottom(r.bottom() - lt->margin() - lt->spacing());
    //    }
    return r;
}


void EmbeddedWidget::installStyle()
{
    if(m_installedStylesheet.isEmpty())
    {
        QFile styleSheetFile(ewApp()->getMainStylesheetFile());

        if(styleSheetFile.open(QFile::ReadOnly))
        {
            m_installedStylesheet = styleSheetFile.readAll();
            styleSheetFile.close();
        }
    }

    if(m_resizeAllowed)
    {
        m_pMainLayout->setMargin(m_borderSize);
        setStyleSheet(m_installedStylesheet);
    }
    else
    {
        QString styleStr = m_installedStylesheet;
        styleStr.append("ew--EmbeddedWindow {border:0px}");
        setStyleSheet(styleStr);
        m_pMainLayout->setMargin(0);
    }
}


void EmbeddedWidget::installStyleFile(const QString &filename)
{
    QString str = filename;

    if(str.isEmpty())
    {
        str = ewApp()->getMainStylesheetFile();

        if(str.isEmpty())
        {
            str = ewApp()->getMainStylesheetFile();
        }
    }

    QFile styleSheetFile(str);

    if(styleSheetFile.open(QFile::ReadOnly))
    {
        m_installedStylesheet = styleSheetFile.readAll();
        styleSheetFile.close();
        installStyle();
    }
}


void EmbeddedWidget::installStyle(QString _styleSheet)
{
    m_installedStylesheet = _styleSheet;
    installStyle();
}


EmbeddedWidget::~EmbeddedWidget()
{
}


void EmbeddedWidget::checkResize()
{
    if(!m_resizeInProcess)
    {
        ActiveBound oldBound = m_activeBound;
        bool left = (
            mapToParent(mapFromGlobal(cursor().pos())).x() >= (geometry()).left() &&
            mapToParent(mapFromGlobal(cursor().pos())).x() <= (geometry()).left() + m_moveBoundSize      /*m_borderSize*/
            );
        bool right = (
            mapToParent(mapFromGlobal(cursor().pos())).x() <= (geometry()).right() &&
            mapToParent(mapFromGlobal(cursor().pos())).x() >= (geometry()).right() - m_moveBoundSize     /*m_borderSize*/
            );
        bool top = (
            mapToParent(mapFromGlobal(cursor().pos())).y() >= (geometry()).top() &&
            mapToParent(mapFromGlobal(cursor().pos())).y() <= (geometry()).top() + m_moveBoundSize       /*m_borderSize*/
            );
        bool bottom = (
            mapToParent(mapFromGlobal(cursor().pos())).y() <= (geometry()).bottom() &&
            mapToParent(mapFromGlobal(cursor().pos())).y() >= (geometry()).bottom() - m_moveBoundSize      /*m_borderSize*/
            );


        if(right)
        {
            if(top)
            {
                m_activeBound = AB_TOPRIGHT;
            }
            else if(bottom)
            {
                m_activeBound = AB_BOTTOM_RIGHT;
            }
            else
            {
                m_activeBound = AB_RIGHT;
            }
        }
        else if(left)
        {
            if(top)
            {
                m_activeBound = AB_TOPLEFT;
            }
            else if(bottom)
            {
                m_activeBound = AB_BOTTOM_LEFT;
            }
            else
            {
                m_activeBound = AB_LEFT;
            }
        }
        else if(top)
        {
            m_activeBound = AB_TOP;
        }
        else if(bottom)
        {
            m_activeBound = AB_BOTTOM;
        }
        else
        {
            m_activeBound = AB_NO_ACTIVE_BOUND;
        }

        if(oldBound == m_activeBound)
        {
            return;
        }

        /// TODO: установка курсора должна быть через APP класс!
        if(oldBound != AB_NO_ACTIVE_BOUND)
        {
            setCursor(QCursor(Qt::ArrowCursor));
        }

        if(m_resizeAllowed)
        {
            if(m_activeBound == AB_LEFT ||
               m_activeBound == AB_RIGHT )
            {
                setCursor(QCursor((Qt::SizeHorCursor)));
            }
            else if(m_activeBound == AB_TOP ||
                    m_activeBound == AB_BOTTOM )
            {
                setCursor(QCursor((Qt::SizeVerCursor)));
            }
            else if(m_activeBound == AB_TOPLEFT ||
                    m_activeBound == AB_BOTTOM_RIGHT )
            {
                setCursor(QCursor((Qt::SizeFDiagCursor)));
            }
            else if(m_activeBound == AB_TOPRIGHT ||
                    m_activeBound == AB_BOTTOM_LEFT )
            {
                setCursor(QCursor((Qt::SizeBDiagCursor)));
            }
        }
    }
}


HighlightSection EmbeddedWidget::checkBorders(QRect rect)
{
    QRect r1 = this->geometry();
    int STICK_RANGE = 5;

    if(std::abs(rect.right() - r1.left()) < STICK_RANGE)
    {
        m_highlightedSection = HS_LEFT;
    }
    else if(std::abs(rect.top() - r1.bottom()) < STICK_RANGE)
    {
        m_highlightedSection = HS_BOTTOM;
    }
    else if(std::abs(rect.bottom() - r1.top()) < STICK_RANGE)
    {
        m_highlightedSection = HS_TOP;
    }
    else if(std::abs(rect.left() - r1.right()) < STICK_RANGE)
    {
        m_highlightedSection = HS_RIGHT;
    }
    else
    {
        m_highlightedSection = HS_NO_HIGHLIGHT;
    }

    update();
    return m_highlightedSection;
}


HighlightSection EmbeddedWidget::getCardinalDirection(QPoint localPos) const
{
    int mapX = localPos.x();
    int mapY = localPos.y();
    double mapW = width();
    double mapH = height();
    bool first = mapY > mapH - (mapH / mapW) * mapX;
    bool second = mapY > (mapH / mapW) * mapX;

    if(first && second)  // Bottom
    {
        //qDebug() << "BOTTOM";
        return HS_BOTTOM;
    }
    else if(first && !second)  // Right
    {
        //qDebug() << "RIGHT";
        return HS_RIGHT;
    }
    else if(!first && !second)  // Top
    {
        //qDebug() << "TOP";
        return HS_TOP;
    }

    // Left
    qDebug() << "LEFT";
    return HS_LEFT;
}


void EmbeddedWidget::mouseReleaseEvent(QMouseEvent *e)
{
    m_pressedGlobalLastPos = e->globalPos();
    m_resizeInProcess = false;
    m_highlightedSection = HS_NO_HIGHLIGHT;

    checkResize();
}


quint64 EmbeddedWidget::id() const
{
    return m_id;
}


void EmbeddedWidget::setId(quint64 id)
{
    m_id = id;
}


void EmbeddedWidget::denyHide(bool denyHide)
{
    m_denyHide = denyHide;
}
