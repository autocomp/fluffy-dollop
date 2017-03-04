#include "embeddedmidget.h"
#include <QApplication>
#include <QBoxLayout>
#include <QBrush>
#include <QDebug>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QStyleOption>

#include "embeddedwindow.h"



ew::EmbeddedMidget::EmbeddedMidget(QWidget *parent) : QWidget(parent)
{
    m_anchorPoint = Qt::AnchorLeft;
    //setAutoFillBackground(true);
    m_id = 0;

    QVBoxLayout *mainLt = new QVBoxLayout(this);
    mainLt->setMargin(0);
    mainLt->setSpacing(0);
    QWidget *wdg = new QWidget(this);
    wdg->setObjectName("EmbeddedMidget_Background");
    QVBoxLayout *lt = new  QVBoxLayout(wdg);
    lt->setSpacing(3);
    lt->setMargin(3);
    mainLt->addWidget(wdg);

    lt->setSpacing(0);
    m_pInnerWidget = new QLabel("", this);
    m_pTextLabel = new QLabel("", this);
    lt->addWidget(m_pInnerWidget, 10);
    m_pInnerWidget->setScaledContents(false);
    lt->addWidget(m_pTextLabel);
    m_pTextLabel->setScaledContents(true);

    m_font = font();
    QFontMetrics fm(m_font);
    setFixedSize(40, 40);
    oldGeometry = geometry();

    showIconSize = 16;
    m_rightArrow = QPixmap(":/embwidgets/img/img/right_arrow_white.png").scaled(showIconSize, showIconSize);
    m_leftArrow = QPixmap(":/embwidgets/img/img/left_arrow_white.png").scaled(showIconSize, showIconSize);
    m_topArrow = QPixmap(":/embwidgets/img/img/top_arrow_white.png").scaled(showIconSize, showIconSize);
    m_bottomArrow = QPixmap(":/embwidgets/img/img/bottom_arrow_white.png").scaled(showIconSize, showIconSize);

    m_pIconLabel = new QLabel("", this);
    m_pIconLabel->setFixedSize(showIconSize, showIconSize);
    setAnchorPoint(Qt::AnchorLeft);
    setObjectName("EmbeddedMidget");
    m_pIconLabel->setPixmap(m_rightArrow);

    m_pInnerWidget->setPixmap(QPixmap(":/embwidgets/img/img/188_users_manual.png").scaled(32, 32));
}


ew::EmbeddedMidget::EmbeddedMidget(QString name, QWidget *parent)
    : EmbeddedMidget(parent)
{
    m_anchorPoint = Qt::AnchorLeft;
    dragModeOn = false;
    m_text = name;
}


ew::EmbeddedMidget::~EmbeddedMidget()
{
    //    delete label;
}


QString ew::EmbeddedMidget::text()
{
    return m_text;
}


void ew::EmbeddedMidget::setText(QString text)
{
    m_text = text;
    m_pTextLabel->setText(m_text);
    setToolTip(text);
    m_pInnerWidget->setToolTip(text);
    m_pTextLabel->setToolTip(text);
}


void ew::EmbeddedMidget::setIcon(const QPixmap & pm)
{
    m_pInnerWidget->setPixmap(pm);
}


void ew::EmbeddedMidget::setAnchorPoint(Qt::AnchorPoint p)
{
    if(m_anchorPoint != p)
    {
        m_anchorPoint = p;
        recalcIconPos(width(), height());

        switch(m_anchorPoint)
        {
          case Qt::AnchorLeft:
          {
              m_pIconLabel->setPixmap(m_rightArrow);
          } break;
          case Qt::AnchorRight:
          {
              m_pIconLabel->setPixmap(m_leftArrow);
          } break;
          case Qt::AnchorBottom:
          {
              m_pIconLabel->setPixmap(m_topArrow);
          } break;
          case Qt::AnchorTop:
          default:
          {
              m_pIconLabel->setPixmap(m_bottomArrow);
          } break;
        }

        update();
    }
}


void ew::EmbeddedMidget::commitGeometry()
{
    qWarning() << __FUNCTION__ << oldGeometry << geometry();
    oldGeometry = geometry();
}


void ew::EmbeddedMidget::revertGeometry()
{
    qWarning() << __FUNCTION__ << oldGeometry << geometry();
    setGeometry(oldGeometry);
}


void ew::EmbeddedMidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    {
        QStyleOption opt;
        opt.init(this);
        QPainter p(this);
        style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
    }

    int pmSize = 16;
    QPen pen = QPen(Qt::white);
    QPainter p(this);
    p.setPen(pen);
    //    QRect g = QRect(QPoint(0,0),QSize(width()-1,height()-1));
    //    p.drawRect(g);

    switch(m_anchorPoint)
    {
      case Qt::AnchorLeft:
      {
          p.drawPixmap(width() - layout()->margin() - pmSize, layout()->margin() /*QRect(width() - layout()->margin() - pmSize, layout()->margin(), pmSize,pmSize)*/, m_rightArrow /*, m_rightArrow.rect()*/);
      } break;
      case Qt::AnchorRight:
      {
          p.drawPixmap(QRect(layout()->margin(), layout()->margin(), pmSize, pmSize),  m_leftArrow, m_leftArrow.rect());
      } break;
      case Qt::AnchorBottom:
      {
          p.drawPixmap(QRect(width() - layout()->margin() - pmSize, layout()->margin(), pmSize, pmSize),  m_topArrow, m_topArrow.rect());
      } break;
      case Qt::AnchorTop:
      default:
      {
          p.drawPixmap(QRect(width() - layout()->margin() - pmSize, height() - layout()->margin() - pmSize, pmSize, pmSize),  m_bottomArrow, m_bottomArrow.rect());
      } break;
    }
}


void ew::EmbeddedMidget::recalcIconPos(int w, int h)
{
    Q_UNUSED(w);
    Q_UNUSED(h);

    switch(m_anchorPoint)
    {
      case Qt::AnchorLeft:
      {
          m_iconLabelPos = QPoint(width() - layout()->margin() - showIconSize, layout()->margin());
      } break;
      case Qt::AnchorRight:
      {
          m_iconLabelPos = QPoint(layout()->margin(), layout()->margin());
      } break;
      case Qt::AnchorBottom:
      {
          m_iconLabelPos = QPoint(width() - layout()->margin() - showIconSize, layout()->margin());
      } break;
      case Qt::AnchorTop:
      default:
      {
          m_iconLabelPos = QPoint(width() - layout()->margin() - showIconSize, height() - layout()->margin() - showIconSize);
      } break;
    }

    m_pIconLabel->move(m_iconLabelPos);
}


void ew::EmbeddedMidget::resizeEvent(QResizeEvent *event)
{
    recalcIconPos(event->size().width(), event->size().height());
}


void ew::EmbeddedMidget::slotCheckStartMove()
{
    QRect r(mapToGlobal(QPoint(0, 0)), mapToGlobal(rect().bottomRight()));

    if(r.contains(QCursor::pos()))
    {
        if(!dragModeOn)
        {
            dragModeOn = true;
            delta = mapFromGlobal(QCursor::pos());
            setCursor(QCursor((Qt::SizeAllCursor)));
        }
    }
}


void ew::EmbeddedMidget::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    if(!dragModeOn)
    {
        QTimer::singleShot(500, this, SLOT(slotCheckStartMove()));
    }
}


void ew::EmbeddedMidget::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);

    if(dragModeOn)
    {
        dragModeOn = false;
        setCursor(QCursor(Qt::ArrowCursor));
        emit signalMidgetPlaced(geometry());
    }
}


void ew::EmbeddedMidget::mouseMoveEvent(QMouseEvent *event)
{
    if(dragModeOn)
    {
        //        QPoint delta = event->globalPos() - delta;
        //        dragStartPoint = event->globalPos();
        //        QRect g = geometry();
        //        QRect geom = (QRect(g.topLeft()+delta,QSize(g.width(),g.height())));
        QRect geom = geometry();
        QPoint point = event->globalPos() - delta;
        emit signalMidgetMoveRequest(QRect(point, geom.size()));
        //setGeometry(QRect(g.topLeft()+delta,QSize(g.width(),g.height())));
    }
}
