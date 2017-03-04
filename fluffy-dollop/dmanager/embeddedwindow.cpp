#include "embeddedwindow.h"

#include <QApplication>
#include <QCursor>
#include <QDebug>
#include <QDesktopWidget>
#include <QErrorMessage>
#include <QMenuBar>
#include <QPainter>
#include <QRect>
#include <QSettings>
#include <QStyle>
#include <QStyleOption>
#include <QVBoxLayout>

#include <QMetaObject>
#include <QMetaProperty>

#include "embeddedapp.h"
#include "embeddedmidget.h"
#include "embeddedutils.h"
#include "embeddedwidget.h"
#include "embeddedwindowheader.h"


// для Midget
#include "embeddedpanel.h"

using namespace ew;


void ew::EmbeddedWindow::resizeEvent(QResizeEvent *e)
{
    EmbeddedWidget::resizeEvent(e);

    if(m_denyGeometryChange)
    {
        e->ignore();
    }
    else
    {
        e->accept();
    }

    slotUpdateChildsGeometry();
}


void ew::EmbeddedWindow::setVisible(bool visible)
{
    if( (false == visible) && (m_denyMinimize || m_denyClose) )
    {
        return;
    }
    else
    {
        QList<EmbeddedWidget *> childs = ewApp()->getWidgetsByParent(id());
        foreach (EmbeddedWidget * w, childs)
        {
            w->denyHide(true);
        }

        QWidget::setVisible(visible);


        foreach (EmbeddedWidget * w, childs)
        {
            w->denyHide(false);
        }
    }
}


void EmbeddedWindow::slotPlacedMidget(QRect midgetRect)
{
    ew::EmbeddedMidget *mPtr = dynamic_cast<ew::EmbeddedMidget *>(sender());

    if(!mPtr)
    {
        qWarning() << __FUNCTION__ << "EmbeddedMidget not found";
        return;
    }

    moveMidgets(midgetRect, mPtr->id());
    checkMidgetPositions(midgetRect, mPtr->id());


    QList<EmbeddedMidget *> lst =  ewApp()->getMidgetsByParent(id());
    foreach (EmbeddedMidget * wdg, lst)
    {
        wdg->setSavedGeometry(wdg->geometry());
    }
}


void EmbeddedWindow::slotMidgetMoveRequest(QRect midgetRect)
{
    ew::EmbeddedMidget *mPtr = dynamic_cast<ew::EmbeddedMidget *>(sender());

    if(!mPtr)
    {
        qWarning() << __FUNCTION__ << "EmbeddedMidget not found";
        return;
    }

    QList<EmbeddedMidget *> lst =  ewApp()->getMidgetsByParent(id());
    foreach (EmbeddedMidget * wdg, lst)
    {
        wdg->setSavedGeometry(wdg->geometry());
    }
    QPoint p = mapToGlobal(QPoint(0, 0));
    QPoint localPos( midgetRect.left() - p.x(), midgetRect.top() - p.y());
    QRect localRect = QRect(localPos, midgetRect.size());
    moveMidgets(localRect, mPtr->id());
    //checkMidgetPositions(localRect,mPtr->id());
}


void ew::EmbeddedWindow::hideEvent(QHideEvent *e)
{
    if(m_denyMinimize)
    {
        e->accept();
        //show();
        setVisible(true);
    }
}


void EmbeddedWindow::mousePressEvent(QMouseEvent *e)
{
    if(m_activeBound != AB_NO_ACTIVE_BOUND)
    {
        m_resizeInProcess = true;
        m_pressedGlobalLastPos = e->globalPos();
    }
}


void EmbeddedWindow::mouseReleaseEvent(QMouseEvent *e)
{
    m_pressedGlobalLastPos = e->globalPos();
    m_resizeInProcess = false;
}


void ew::EmbeddedWindow::slotUpdateChildsGeometry()
{
    QList<EmbeddedWidgetBaseStructPrivate *> childs = ewApp()->getPrivateStructByParent(id(), ew::EWT_WIDGET);

    foreach (EmbeddedWidgetBaseStructPrivate * privStr, childs)
    {
        auto w = privStr->embWidgetPtr;
        ewApp()->changeWidgetAlignGeometry(w->id(), privStr->embWidgetStruct->alignPoint, privStr->embWidgetPtr->size());
    }
}


//void EmbeddedWindow::paintEvent(QPaintEvent *e)
//{
//    QStyleOption opt;

//    opt.init(this);
//    QPainter p(this);
//    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
//    EmbeddedWidget::paintEvent(e);
//}


int EmbeddedWindow::convertGeomToWeight(QRect r)
{
    if(r.y() == 0 )
    {
        return r.width();
    }

    if( r.x() == width() - r.width())
    {
        return (width() + r.y());
    }

    if(r.y() == height() - r.height())
    {
        return (width() + height() + (width() - r.x()));
    }

    if(r.x() == 0)
    {
        return (width() * 2 + height() * 2 - r.y());
    }

    qWarning() << __FUNCTION__ << "unsupported geom, can't convert";
    return 0;
}


QRect EmbeddedWindow::convertWeightToGeom(int weight, QSize sz)
{
    QRect r = QRect(0, 0, sz.width(), sz.height());
    int fullLen = 2 * width() + 2 * height();

    while((weight > fullLen))
    {
        weight -= fullLen;
    }

    //    QList<int> lenLst;
    //    int topLen    = ewPtr->geometry().top();
    //    int bottomLen = bRect.height() - ewPtr->geometry().bottom();
    //    int leftLen   = ewPtr->geometry().left();
    //    int rightLen  = bRect.width() - ewPtr->geometry().right();

    if(weight < width())
    {
        r.moveTo(weight, 0);
        return r;
    }

    if(weight < height() + width())
    {
        weight = weight - width();
        r.moveTo(width() - sz.width(), weight );

        return r;
    }

    if(weight < 2 * width() + height())
    {
        weight = weight - width() - height();
        r.moveTo(width() - weight - sz.width(), height() - sz.height());
        return r;
    }

    if(weight < 2 * width() + 2 * height())
    {
        weight = weight - 2 * width() - height();
        r.moveTo(0, height() - weight);
        return r;
    }

    return r;
}


void EmbeddedWindow::checkMidgetPositions(QRect wantedRect, quint64 midgetId )
{
//    EmbeddedMidget* mPtr = ewApp()->getMidget(midgetId);
//    QPoint mPos = wantedRect.topLeft();
//    QRect windGeom = QRect(0, 0, width(), height());

    Q_UNUSED(wantedRect);
    Q_UNUSED(midgetId);
}


bool EmbeddedWindow::moveMidgetsRecursive(QRect wantedRect, quint64 midgetId, const QList<int> & sortedWeightList)
{
    Q_UNUSED(wantedRect);
    Q_UNUSED(midgetId);
    Q_UNUSED(sortedWeightList);
    return true;
}


void EmbeddedWindow::moveMidgets(QRect wantedRect, quint64 midgetId )
{
    EmbeddedMidget *mPtr = ewApp()->getMidget(midgetId);
    QPoint mPos = wantedRect.topLeft();

    QList<int> lenLst;
    int topLen = qAbs(m_pViewPanel->y() - mPos.y());
    int bottomLen = qAbs(m_pViewPanel->y() + m_pViewPanel->height() - mPos.y());
    int leftLen = qAbs(m_pViewPanel->x() - mPos.x());
    int rightLen = qAbs(m_pViewPanel->x() + m_pViewPanel->width() - mPos.x());
    lenLst << topLen << bottomLen << leftLen << rightLen;
    qSort(lenLst);

    if(lenLst.first() == topLen)
    {
        if(mPos.x() < m_pViewPanel->x())
        {
            mPos = QPoint(m_pViewPanel->x(), m_pViewPanel->y());
        }
        else if(mPos.x() > m_pViewPanel->x() + m_pViewPanel->width() - wantedRect.width())
        {
            mPos = QPoint(m_pViewPanel->x() + m_pViewPanel->width() - wantedRect.width()
                          , m_pViewPanel->y());
        }

        mPtr->move(mPos.x(), m_pViewPanel->y());
        mPtr->setAnchorPoint(Qt::AnchorTop);
    }
    else if(lenLst.first() == bottomLen)
    {
        if(mPos.x() < m_pViewPanel->x())
        {
            mPos.setX(m_pViewPanel->x());
        }
        else if(mPos.x() > m_pViewPanel->x() + m_pViewPanel->width() - wantedRect.width())
        {
            mPos.setX(m_pViewPanel->x() + m_pViewPanel->width() - wantedRect.width());
        }

        mPtr->move(mPos.x(), m_pViewPanel->y() + m_pViewPanel->height() - wantedRect.height());
        mPtr->setAnchorPoint(Qt::AnchorBottom);
    }
    else if(lenLst.first() == leftLen)
    {
        if(mPos.y() < m_pViewPanel->y())
        {
            mPos.setY(m_pViewPanel->y());
        }
        else if(mPos.y() > m_pViewPanel->y() + m_pViewPanel->height() - wantedRect.height())
        {
            mPos.setY(m_pViewPanel->y() + m_pViewPanel->height() - wantedRect.height());
        }

        mPtr->move(m_pViewPanel->x(), mPos.y());
        mPtr->setAnchorPoint(Qt::AnchorLeft);
    }
    else if(lenLst.first() == rightLen)
    {
        if(mPos.y() < m_pViewPanel->y())
        {
            mPos.setY(0);
        }
        else if(mPos.y() > m_pViewPanel->y() + m_pViewPanel->height() - wantedRect.height())
        {
            mPos.setY(m_pViewPanel->y() + m_pViewPanel->height() - wantedRect.height());
        }

        mPtr->move(m_pViewPanel->x() + m_pViewPanel->width(), mPos.y());
        mPtr->setAnchorPoint(Qt::AnchorRight);
    }

    return;

//    if(convertGeomToWeight(prevRect) < convertGeomToWeight(mPtr->geometry()))
//    {
//        mPtr->move(mPos.x(),m_pViewPanel->height() - mPtr->height());
//    }

//    QMap<int, EmbeddedMidget*> keySortedWeightMap;
//    QList<EmbeddedMidget*> lst = ewApp()->getMidgetsByParent(id());
//    int currWeight = 0;
//    QMap<int, EmbeddedMidget*>::iterator  currIter;
//    foreach (EmbeddedMidget* _mPtr, lst)
//    {    int weight = convertGeomToWeight(_mPtr->geometry());

//        if(_mPtr->id() == midgetId)
//        {
//            currWeight = weight;
//            currIter = keySortedWeightMap.insert(weight, _mPtr);
//        }
//        else
//        {
//            keySortedWeightMap.insert(weight, _mPtr);
//        }

//        mPtr->move(0, mPos.y());
//    }

//    bool haveIntersectionWithNext = false;
//    bool haveIntersectionWithPrev = false;

//    if(currIter != keySortedWeightMap.begin())
//    {
//        QMap<int, EmbeddedMidget*>::iterator  prevIter = currIter - 1 ;
//        if(prevIter !=  keySortedWeightMap.begin())
//        {
//            haveIntersectionWithPrev = currIter.value()->geometry().intersects(prevIter.value()->geometry());

//        }
//        mPtr->move(width() - mPtr->width(), mPos.y());
//    }

//    if(currIter != keySortedWeightMap.end() )
//    {
//        if((currIter + 1) != keySortedWeightMap.end())
//        {
//            QMap<int, EmbeddedMidget*>::iterator  nextIter = currIter + 1;
//            if(nextIter !=  keySortedWeightMap.end())
//            {
//                QRect r1 = currIter.value()->geometry();
//                QRect r2 = nextIter.value()->geometry();
//                haveIntersectionWithNext = r1.intersects(r2);
//            }
//        }
//    }


//    if(haveIntersectionWithNext)
//    {
//        qDebug() << __FUNCTION__ << "intersect next";
//    }

//    if(haveIntersectionWithPrev)
//    {
//        qDebug() << __FUNCTION__ << "intersect prev";
//    }
//    //проверяем, а есть ли вообще пересечения
//    //moveMidgetsRecursive();

//    return;
}


EmbeddedWindow::EmbeddedWindow(QWidget *parent)
    : EmbeddedWidget(parent)
{
    m_denyGeometryChange = false;
    m_denyMinimize = false;
    m_denyClose = false;

    m_mainViewWidget = 0;

    setObjectName("EmbeddedWindow_EmbeddedWindow");

    /*Заполняем меню окна*/
    setWindowFlags(Qt::CustomizeWindowHint);
    setMinimumSize(0, 0);
}


void EmbeddedWindow::setHeader(const EmbeddedHeaderStruct &headerStruct)
{
    bool needEnableUpdates = true;

    if(updatesEnabled())
    {
        needEnableUpdates = true;
        setUpdatesEnabled(false);
    }

    delete m_pHeaderWidget;
    m_pHeaderWidget = 0;

    m_pHeaderWidget = new ew::EmbeddedWindowHeader(this);

    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_GLOBAL_ONPARENT_MOVE, headerStruct.hasGlobalMoveButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_HIDE_HEADER, headerStruct.hasHideHeaderButton           );
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_LOCKUNLOCK, headerStruct.hasLockButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_MIDGET, headerStruct.hasMidgetButton           );
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_PINUNPIN, headerStruct.hasPinButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_SETTINGS, headerStruct.hasSettingsButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_STYLE, headerStruct.hasStyleButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_WHAT_THIS, headerStruct.hasWhatButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_COLLAPSE, headerStruct.hasCloseButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_FULLNORMSCREEN, headerStruct.hasMinMaxButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_CLOSE, headerStruct.hasCloseButton);

    m_pHeaderWidget->setToolTip(headerStruct.tooltip);
    m_pHeaderWidget->setText(headerStruct.windowTitle);
    m_pHeaderWidget->setDescription(headerStruct.descr);
    m_pHeaderWidget->setIcon(headerStruct.headerPixmap);

    m_pHeaderWidget->setIcon(headerStruct.headerPixmap);
    setWindowIcon(QIcon(headerStruct.headerPixmap));

    //    m_pHeaderWidget->init();

    setWindowTitleText(headerStruct.windowTitle);

    m_pHeaderWidget->setParent(this);
    m_pMainLayout->insertWidget(0, m_pHeaderWidget);

    connect(m_pHeaderWidget, SIGNAL(signalWindowEvent(EmbeddedHeader::WindowHeaderEvents,bool *))
            , this, SLOT(slotHeaderWindowEvents(EmbeddedHeader::WindowHeaderEvents,bool *)));


    connect(m_pHeaderWidget, SIGNAL(signalMoveWindowRequest(int,int,bool *))
            , this, SLOT(slotMoveWindowRequest(int,int,bool *)));

    if(needEnableUpdates)
    {
        setUpdatesEnabled(true);
    }
}


void EmbeddedWindow::setMenuBar(QMenuBar *menuBar)
{
    EmbeddedWindowHeader *head = dynamic_cast<EmbeddedWindowHeader *>(m_pHeaderWidget);

    if(nullptr != head)
    {
        head->setMenuBar(menuBar);
    }
    else
    {
        qWarning() << "EmbeddedWindow::setMenuBar(): WARN! Can't install QMenuBar, window header is null";
    }
}


void EmbeddedWindow::setBottomBar(QWidget *bottomBar)
{
    m_pMainLayout->addWidget(bottomBar, 0);
}


void EmbeddedWindow::slotReloadButtonClicked()
{
    int mainWidgW = width();
    int mainWidgX = 0;

    QList<EmbeddedWidgetBaseStructPrivate *> childs = ewApp()->getPrivateStructByParent(id());
    foreach (EmbeddedWidgetBaseStructPrivate * privStr, childs)
    {
        auto w = privStr->embWidgetPtr;
        ew::EmbeddedWidgetStruct *eStruct = dynamic_cast<ew::EmbeddedWidgetStruct *>(privStr->embWidgetStruct);

        switch(eStruct->alignType)
        {
          case EWA_IN_LEFT_TOP_CORNER_OFFSET:   /*привязка виджета относительно левого верхнего угла с фиксированным смещением*/
          {
              //if(w->isPinned() || !eStruct->allowMoving)
              {
                  w->move(mainWidgX + eStruct->alignPoint.x(), eStruct->alignPoint.y());
              }
          } break;
          case EWA_IN_LEFT_BOTTOM_CORNER_OFFSET: /*привязка виджета относительно левого нижнего угла с фиксированным смещением*/
          {
              //if(w->isPinned() || !eStruct->allowMoving)
              {
                  QPoint p(mainWidgX + eStruct->alignPoint.x()
                           , height() - w->frameGeometry().height() - eStruct->alignPoint.y() );
                  w->move(p);
              }
          } break;
          case EWA_IN_RIGHT_TOP_CORNER_OFFSET:  /*привязка виджета относительно правого верхнего угла с фиксированным смещением*/
          {
              //if(w->isPinned() || !eStruct->allowMoving)
              {
                  QPoint p(mainWidgX + mainWidgW - w->frameGeometry().width() - eStruct->alignPoint.x()
                           , eStruct->alignPoint.y() );

                  w->move(p);
              }
          } break;
          case EWA_IN_RIGHT_BOTTOM_CORNER_OFFSET: /*привязка виджета относительно правого нижнего угла с фиксированным смещением*/
          {
              //if(w->isPinned() || !eStruct->allowMoving)
              {
                  QPoint p(mainWidgX + mainWidgW - w->frameGeometry().width() - eStruct->alignPoint.x()
                           , height() - w->frameGeometry().height() - eStruct->alignPoint.y() );
                  w->move(p);
              }
          } break;
          case EWA_OUT_FREE_WIDGET:
          {
              //if(w->isPinned() || !eStruct->allowMoving)
              {
                  QPoint p((eStruct->alignPoint.x()) - w->frameGeometry().width() / 2
                           , (eStruct->alignPoint.y()) - w->frameGeometry().height() / 2 );

                  w->move(p);
                  QRect rr = QApplication::desktop()->availableGeometry();
                  QRect r = w->frameGeometry();

                  if(!rect().contains(r))
                  {
                      w->blockSignals(true);
                      w->setUpdatesEnabled(false);

                      bool moved = false;
                      QLineF l(rr.center(), p);
                      QPoint validCenter = w->pos();/*(rr.center().x() - w->frameGeometry().width()/2
                                                       , rr.center().y() - w->frameGeometry().height()/2);*/

                      for(int i = 0; i < l.length(); i++)
                      {
                          p = l.pointAt(0.01 * i).toPoint();
                          QRect tmpRect = r;
                          tmpRect.moveTo(p);

                          if(rr.contains(tmpRect))
                          {
                              validCenter = p;
                          }
                          else
                          {
                              moved = true;
                              QPoint alignPoint  = validCenter;/*QPoint(validCenter.x() - w->frameGeometry().width()/2,
                                                                 validCenter.y() - w->frameGeometry().height()/2 );*/

                              eStruct->alignPoint = alignPoint;
                              w->move(alignPoint);
                              break;
                          }
                      }

                      w->blockSignals(false);
                      w->setUpdatesEnabled(true);

                      if(moved)
                      {
                          emit signalEmbWidgetSettingsChanged(eStruct);
                      }
                  }
              }
          }
          case EWA_IN_PERCENTAGE_ALIGN:          /*привязка виджета с динамическим смещением(процент от ширины/высоты виджета)*/
          {
              //if(w->isPinned() || !eStruct->allowMoving)
              {
                  QPoint p((eStruct->alignPoint.x() * width()) / 100 - w->frameGeometry().width() / 2
                           , (eStruct->alignPoint.y() * height()) / 100 - w->frameGeometry().height() / 2 );

                  w->move(p);
//                QRect rr = rect();
                  QRect r = w->frameGeometry();
//                QRect r1 = w->geometry();
//                QRect r2 = w->rect();

                  if(!rect().contains(r))
                  {
                      w->blockSignals(true);
                      w->setUpdatesEnabled(false);

                      bool moved = false;
                      QLineF l(rect().center(), p);
                      QPoint validCenter(width() / 2 - w->frameGeometry().width() / 2
                                         , height() / 2 - w->frameGeometry().height() / 2);

                      for(int i = 0; i < l.length(); i++)
                      {
                          p = l.pointAt(0.01 * i).toPoint();
                          QRect tmpRect = r;
                          tmpRect.moveCenter(p);

                          if(rect().contains(tmpRect))
                          {
                              validCenter = p;
                          }
                          else
                          {
                              moved = true;
                              QPoint alignPoint  = QPoint(validCenter.x() - w->frameGeometry().width() / 2,
                                                          validCenter.y() - w->frameGeometry().height() / 2 );
                              QPoint percPoint(100 * ((alignPoint.x() * 1.0 + w->frameGeometry().width() / 2.0) / width())
                                               , 100 * ((alignPoint.y() * 1.0 + w->frameGeometry().height() / 2.0) / height()) );

                              eStruct->alignPoint = percPoint;
                              w->move(alignPoint);
                              break;
                          }
                      }

                      w->blockSignals(false);
                      w->setUpdatesEnabled(true);

                      if(moved)
                      {
                          emit signalEmbWidgetSettingsChanged(eStruct);
                      }
                  }
              }
          } break;

          default:
              break;
        }
    }
}


//QMargins ew::EmbeddedWindow::getEmbeddedViewMargins()
//{
//    QMargins marg = EmbeddedWidget::getEmbeddedViewMargins();
//    return marg;


//}



void ew::EmbeddedWindow::closeEvent(QCloseEvent *e)
{
    if(m_denyClose)
    {
        e->accept();
    }
}


void ew::EmbeddedWindow::slotLockCollapsedWidget(bool isLocked)
{
    ew::EmbeddedWidgetBaseStruct *str = (ewApp()->getWidgetStructByWidgetPtr(dynamic_cast<QWidget *>(sender())));

    if(0 != str)
    {
        str->locked = isLocked;
        emit signalEmbWidgetSettingsChanged(str);
    }
    else
    {
        qWarning() << __FUNCTION__ << " EmbeddedWidgetStructPrivate not found";
    }
}


void ew::EmbeddedWindow::slotPinCollapsedWidget(bool isPinned)
{
    ew::EmbeddedWidgetBaseStruct *str = (ewApp()->getWidgetStructByWidgetPtr(dynamic_cast<QWidget *>(sender())));

    if(0 != str)
    {
        str->allowMoving = isPinned;
        emit signalEmbWidgetSettingsChanged(str);
    }
    else
    {
        qWarning() << __FUNCTION__ << " EmbeddedWidgetStructPrivate not found";
    }
}


void ew::EmbeddedWindow::addEmbeddedGroup(quint64 embId, EmbeddedGroupWidget *wdg, EmbeddedGroupStruct *eStruct)
{
    Q_UNUSED(embId);
    Q_UNUSED(wdg);
    Q_UNUSED(eStruct);
    return;
}


void ew::EmbeddedWindow::addEmbeddedWidget(quint64 embId, ew::EmbeddedWidget *wdg, ew::EmbeddedWidgetBaseStruct *eStruct)
{
    setUpdatesEnabled(false);

    wdg->setParent(m_pMainViewWidgetBackground);

    connect(wdg, SIGNAL(signalVisibleNeedChange(bool))
            , this, SLOT(slotEmbendedVisibleChanged(bool)));

    connect(wdg, SIGNAL(signalVisibleNeedChangeInternal(bool))
            , this, SLOT(slotEmbendedVisibleChangedInternal(bool)));

    connect(wdg, SIGNAL(signalWidgetMovePolicyChanged(quint64,bool)),
            this, SLOT(slotWidgetMovePolicyChanged(quint64,bool)));


    QPointF p = ew::EmbeddedUtils::convertFromAlignPoint(eStruct->alignPoint, eStruct->alignType,
                                                         eStruct->size, size(), pos());


    ewApp()->changeWidgetGeometry(embId, p, eStruct->size);
    setUpdatesEnabled(true);

    setMinimumSize(0, 0);
    return;
}


void ew::EmbeddedWindow::slotWidgetMovePolicyChanged(quint64 id, bool isMoveOnParent)
{
    ew::EmbeddedWidgetStruct *embStr = dynamic_cast<EmbeddedWidgetStruct *>(ewApp()->getWidgetStructById(id));
    auto embWdg = (ewApp()->getWidget(id));
    QWidget *wdg = embWdg->parentWidget();

    if(this == wdg )
    {
        if(isMoveOnParent)
        {
            //ничего не делаем, политика не поменялась
        }
        else
        {
            //                m_pInnerRightScrollArea->addWidgetToSet(embStr->embWidgetPtr);

            //                m_pOuterLeftScrollArea->addWidgetToSet(embStr->embWidgetPtr);

            //                m_pOuterRightScrollArea->addWidgetToSet(embStr->embWidgetPtr);

            embStr->alignType = EWA_OUT_FREE_WIDGET;
            QPoint globColPoint = mapToGlobal(embWdg->pos());
            embWdg->setParent(0);
            embStr->alignPoint = globColPoint;
            embWdg->move(globColPoint);
            embWdg->setWindowFlags(embWdg->windowFlags() | Qt::WindowStaysOnTopHint);

            embWdg->setVisible(true);
        }
    }
    else
    {
        if(0 == wdg)
        {
            //свободный виджет
            if(isMoveOnParent)
            {
                //                    m_pInnerRightScrollArea->removeWidget(embStr->embWidgetPtr);
                //                    m_pInnerRightScrollArea->removeWidgetFromSet(embStr->embWidgetPtr);

                //                    m_pOuterLeftScrollArea->removeWidget(embStr->embWidgetPtr);
                //                    m_pOuterLeftScrollArea->removeWidgetFromSet(embStr->embWidgetPtr);

                //                    m_pOuterRightScrollArea->removeWidget(embStr->embWidgetPtr);
                //                    m_pOuterRightScrollArea->removeWidgetFromSet(embStr->embWidgetPtr);

                //пересчитываем координаты
                QPoint globalPos = embWdg->pos();
                QRect swRect = rect();

                if(swRect.contains(globalPos))
                {
                    QPointF mappedColPos = mapFromGlobal(globalPos);
                    QPointF alignPoint(((mappedColPos.x() + embWdg->frameGeometry().width() / 2.0) / width()) * 100.0
                                       , ((mappedColPos.y() + embWdg->frameGeometry().height() / 2.0) / height()) * 100.0 );

                    embStr->alignPoint = alignPoint;
                    embWdg->move(mappedColPos.toPoint());
                }
                else
                {
                    embStr->alignPoint = QPointF(50, 50);
                }

                embStr->alignType = EWA_IN_PERCENTAGE_ALIGN;

                embWdg->setParent(this);
                embWdg->setVisible(true);

                slotUpdateChildsGeometry();
            }
            else
            {
                embStr->alignType = EWA_OUT_FREE_WIDGET;
            }
        }
        else
        {
            //виджет в прокрутке
            if(isMoveOnParent)
            {
                embWdg->setParent(this);
                //                    m_pInnerRightScrollArea->removeWidget(embStr->embWidgetPtr);
                //                    m_pInnerRightScrollArea->removeWidgetFromSet(embStr->embWidgetPtr);

                //                    m_pOuterLeftScrollArea->removeWidget(embStr->embWidgetPtr);
                //                    m_pOuterLeftScrollArea->removeWidgetFromSet(embStr->embWidgetPtr);

                //                    m_pOuterRightScrollArea->removeWidget(embStr->embWidgetPtr);
                //                    m_pOuterRightScrollArea->removeWidgetFromSet(embStr->embWidgetPtr);

                embStr->alignType = EWA_IN_PERCENTAGE_ALIGN;
                embStr->alignPoint = QPointF(50, 50);

                embWdg->setVisible(true);
            }
            else
            {
                embWdg->setParent(0);
                //                    m_pInnerRightScrollArea->removeWidget(embStr->embWidgetPtr);
                //                    //m_pInnerRightScrollArea->removeWidgetFromSet(embStr->embWidgetPtr);

                //                    m_pOuterLeftScrollArea->removeWidget(embStr->embWidgetPtr);
                //                    //m_pOuterLeftScrollArea->removeWidgetFromSet(embStr->embWidgetPtr);

                //                    m_pOuterRightScrollArea->removeWidget(embStr->embWidgetPtr);
                //                    //m_pOuterRightScrollArea->removeWidgetFromSet(embStr->embWidgetPtr);

                embStr->alignType = EWA_OUT_FREE_WIDGET;
                embWdg->setWindowFlags(embWdg->windowFlags() | Qt::WindowStaysOnTopHint);

                embWdg->setVisible(true);
            }
        }
    }

    slotUpdateChildsGeometry();
    emit signalEmbWidgetSettingsChanged(embStr);
}


void ew::EmbeddedWindow::slotEmbendedVisibleChangedInternal(bool isToogled)
{
    return;
    ew::EmbeddedWidgetStruct *str = dynamic_cast<EmbeddedWidgetStruct *>((ewApp()->getWidgetStructByWidgetPtr(dynamic_cast<QWidget *>(sender()))));

    if(0 != str)
    {
        if(str->iface->getWidget())
        {
            str->iface->getWidget()->setVisible(isToogled);
        }

        //str->addHided = !isToogled;

        emit signalEmbWidgetSettingsChanged(str);
    }
    else
    {
        qWarning() << __FUNCTION__ << " EmbeddedWidgetStructPrivate not found";
    }

    //saveSettings();
}


void ew::EmbeddedWindow::slotEmbendedTitleChanged(QString titleText)
{
    ew::EmbeddedWidgetBaseStruct *str = (ewApp()->getWidgetStructByWidgetPtr(dynamic_cast<QWidget *>(sender())));

    if(0 != str)
    {
        str->header.windowTitle = titleText;
        emit signalEmbWidgetSettingsChanged(str);
    }
    else
    {
        qWarning() << __FUNCTION__ << " EmbeddedWidgetStructPrivate not found";
    }
}


void ew::EmbeddedWindow::slotEmbendedWidgetClosed()
{
    ew::EmbeddedWidgetBaseStruct *str = (ewApp()->getWidgetStructByWidgetPtr(dynamic_cast<QWidget *>(sender())));
    quint64 embId = (ewApp()->getWidgetIdByWidgetPtr(dynamic_cast<QWidget *>(sender())));

    if(0 != embId)
    {
        ewApp()->removeWidget(embId);

        if(0 != str && 0 != embId)
        {
            //str->addHided = true;
            emit signalEmbWidgetSettingsChanged(str);
        }
        else
        {
            qWarning() << __FUNCTION__ << " EmbeddedWidgetStructPrivate not found";
        }
    }
}


void ew::EmbeddedWindow::slotEmbendedVisibleChanged(bool isToogled)
{
    Q_UNUSED(isToogled);
}


void ew::EmbeddedWindow::slotEmbendedCollapseChanged(quint64 wId, bool res)
{
    Q_UNUSED(wId);
    ew::EmbeddedWidgetBaseStruct *str = (ewApp()->getWidgetStructByWidgetPtr(dynamic_cast<QWidget *>(sender())));

    if(0 != str)
    {
        str->collapsed = res;
        emit signalEmbWidgetSettingsChanged(str);
    }
    else
    {
        qWarning() << __FUNCTION__ << " EmbeddedWidgetStructPrivate not found";
    }
}


void ew::EmbeddedWindow::slotColWidgetPosChanged(quint64 wId, QPoint p, QSize sz)
{
//    auto embWdg = ewApp()->getWidget(wId);
//    QPointF alignPoint = p;
//    ew::EmbeddedWidgetBaseStruct * eStruct = /*dynamic_cast<EmbeddedWidgetStruct*>*/(ewApp()->getEmbeddedStruct(wId));

//    switch (eStruct->alignType)
//    {

//    case EWA_IN_LEFT_TOP_CORNER_OFFSET:
//    {
//        if(eStruct->allowMoving && !embWdg->isPinned())
//        {
//            alignPoint = p;
//        }
//    }break;

//    case EWA_IN_LEFT_BOTTOM_CORNER_OFFSET:
//    {
//        if(eStruct->allowMoving && !embWdg->isPinned())
//        {
//            alignPoint = QPoint( p.x(), (height() - (p.y() + embWdg->frameGeometry().height())  ) );
//        }
//    }break;

//    case EWA_IN_RIGHT_TOP_CORNER_OFFSET:
//    {
//        if(eStruct->allowMoving && !embWdg->isPinned())
//        {
//            alignPoint = QPoint( (width() - (p.x() + embWdg->frameGeometry().width())), p.y()  );
//        }
//    }break;

//    case EWA_IN_RIGHT_BOTTOM_CORNER_OFFSET:
//    {
//        if(eStruct->allowMoving && !embWdg->isPinned())
//        {
//            alignPoint = QPoint( (width() - (p.x() + embWdg->frameGeometry().width())), (height() - (p.y() + embWdg->frameGeometry().height())) );
//        }
//    }break;

//    case EWA_IN_PERCENTAGE_ALIGN:
//    {
//        if(eStruct->allowMoving && !embWdg->isPinned())
//        {
//            QRect r = embWdg->frameGeometry();
//            QPointF percPoint(100.0*((p.x()*1.0 + embWdg->frameGeometry().width()/2.0)/(1.0*width()))
//                              , 100.0*((p.y()*1.0 + embWdg->frameGeometry().height()/2.0)/(1.0*height())) );
//            alignPoint = percPoint;
//        }
//    }break;
//    case EWA_OUT_FREE_WIDGET:
//    {
//        if(eStruct->allowMoving && !embWdg->isPinned())
//        {
//            alignPoint = embWdg->pos();
//            //            QPointF percPoint((p.x()*1.0 + embWdg->frameGeometry().width()/2.0)
//            //                              , (p.y()*1.0 + embWdg->frameGeometry().height()/2.0) );
//            //alignPoint = p;
//        }
//    }break;

//    default:
//        break;
//    }

    ewApp()->changeWidgetGeometry(wId, p, sz);

//    ewApp()->changeWidgetPos(wId, p, alignPoint);
//    ewApp()->changeWidgetSize(wId, sz);

//    slotUpdateChildsGeometry();
//    ewApp()->embWidgetChanged(embWdg->id());
}


//quint64 ew::EmbeddedWindow::getEmbeddedWidgetIdByTag(QString tag)
//{
//    quint64 id = (quint64)(-1);
//    QMap<quint64 , ew::EmbeddedWidgetStructPrivate*>::iterator i;
//    QList<quint64> ids;
//    for (i = m_embeddedWidgets.begin(); i != m_embeddedWidgets.end(); ++i)
//    {
//        ew::EmbeddedWidgetStructPrivate * eStruct = i.value();
//        if(tag == eStruct->embWidgetStruct->widgetTag)
//        {
//            return eStruct->id;
//        }
//    }

//    return id;
//}

//void ew::EmbeddedWindow::removeEmbendedWidget(QString widgetTag)
//{
//    quint64 id = ewApp()->getEmbeddedWidgetIdByTag(widgetTag);
//    if((-1) == id)
//        return;

//    ew::EmbeddedWidgetStructPrivate * eStructPriv = m_embeddedWidgets.value(id,0);
//    EmbeddedWidgetStruct* eStruct = dynamic_cast<EmbeddedWidgetStruct*>(eStructPriv->embWidgetStruct);

//    eStruct->embWidgetPtr->removeWidget(eStruct->embWidgetStruct->pluginWidget);
//    delete eStruct->embWidgetPtr;
//    eStruct->embWidgetPtr = 0;
//    m_embeddedWidgets.remove(id);
//}

//void EmbeddedWindow::addChild(EmbeddedWidget *cld)
//{
//    connect(cld,SIGNAL(destroyed(QObject*)),SLOT(deleteChild(QObject*)));
//    connect(cld,SIGNAL(signalWindowDragged(QRect)),SLOT(checkChildBorders(QRect)));
//    connect(cld,SIGNAL(signalWindowDropped(QRect)),SLOT(checkChildGroups(QRect)));
//    childs.append(cld);
//}

//void EmbeddedWindow::deleteChild(QObject * obj)
//{
//    childs.removeAll(qobject_cast<EmbeddedWidget *>(obj));
//}


/// DEPRECATED
void ew::EmbeddedWindow::slotEmbeddedWidgetDeleted()
{
    //    QWidget * wdg = dynamic_cast<QWidget*>(sender());
    //    QMap<quint64 , ew::EmbeddedWidgetStructPrivate*>::iterator i;
    //    QList<quint64> ids;
    //    for (i = m_embeddedWidgets.begin(); i != m_embeddedWidgets.end(); ++i)
    //    {
    //        ew::EmbeddedWidgetStructPrivate * eStruct = i.value();
    //        if(wdg == eStruct->embWidgetPtr)
    //        {
    //            ids.append(i.key());
    //            eStruct->embWidgetPtr->removeWidget(wdg);
    //            delete eStruct->embWidgetPtr;
    //            eStruct->embWidgetPtr = 0;
    //        }

    //    }
}


EmbeddedWindow::~EmbeddedWindow()
{
}


EmbeddedWidgetType EmbeddedWindow::widgetType()
{
    return ew::EWT_WINDOW;
}


QRect EmbeddedWindow::getParkingRect()
{
    if(m_mainViewWidget)
    {
        return m_mainViewWidget->geometry();
    }

    return rect();
}


void EmbeddedWindow::setWidget(ew::EmbeddedWidgetBaseStructPrivate *embStrPriv)
{
    QWidget *mainWidget = embStrPriv->embWidgetPtr;

    if(m_mainViewWidget)
    {
        m_pMainViewWidgetLayout->addWidget(mainWidget);
    }
    else
    {
        m_pMainViewWidgetLayout->removeWidget(m_mainViewWidget);
        delete m_mainViewWidget;
        m_pMainViewWidgetLayout->addWidget(mainWidget, 10);
        m_mainViewWidget = mainWidget;
    }
}
