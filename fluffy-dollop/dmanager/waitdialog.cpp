#include "embeddedapp.h"
#include "waitdialog.h"
#include <QApplication>
#include <QDebug>
#include <QGradient>
#include <QHBoxLayout>
#include <QMutex>
#include <QPainter>
#include <QPixmapCache>
#include <QProcess>
#include <QThread>
#include <QTime>

using namespace ew;

WaitDialog::WaitDialog(QWidget *parent) :
    QDialog(parent),
    ew::EmbeddedSubIFace(),
    m_startAngle(0),
    m_style(StyleArc)
{
    m_currentStateId = 0;
    QSizePolicy policy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    policy.setHeightForWidth(true);
    setSizePolicy(policy);

    m_fillColor = palette().color(QPalette::WindowText);

    m_timer = new QTimer(this);
    m_timer->setInterval(50);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(rotate()));
    m_timer->start();
}


void WaitDialog::rotate()
{
    m_startAngle += 30;
    m_startAngle %= 360;
    update();
}


void WaitDialog::setIndicatorStyle(IndicatorStyle style)
{
    m_style = style;
    update();
}


void WaitDialog::setColor(QColor color)
{
    m_fillColor = color;
}


WaitDialog::IndicatorStyle WaitDialog::indicatorStyle() const
{
    return m_style;
}


QPixmap WaitDialog::generatePixmap(int side)
{
    QPixmap pixmap(QSize(side, side));

    pixmap.fill(QColor(255, 255, 255, 0));

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(side / 2, side / 2);
    painter.scale(side / 200.0, side / 200.0);

    switch(m_style)
    {
      case StyleRect:
          drawRectStyle(&painter);
          break;
      case StyleEllipse:
          drawEllipseStyle(&painter);
          break;
      case StyleArc:
          drawArcStyle(&painter);
          break;
    }

    return pixmap;
}


void WaitDialog::drawRectStyle(QPainter *painter)
{
    //    QColor color = palette().color(QPalette::WindowText);
    QColor color = m_fillColor;
    QBrush brush(color);

    painter->setPen(Qt::NoPen);

    painter->rotate(m_startAngle);

    float angle = 0;

    while(angle < 360)
    {
        painter->setBrush(brush);
        painter->drawRect(-8, -100, 16, 35);

        painter->rotate(30);
        angle += 30;

        color.setAlphaF(angle / 360);
        brush.setColor(color);
    }
}


void WaitDialog::drawEllipseStyle(QPainter *painter)
{
    //    QColor color = palette().color(QPalette::WindowText);
    QColor color = m_fillColor;
    QBrush brush(color);

    painter->setPen(Qt::NoPen);

    painter->rotate(m_startAngle);

    float angle = 0;

    while(angle < 360)
    {
        painter->setBrush(brush);
        painter->drawEllipse(-10, -100, 30, 30);

        painter->rotate(30);
        angle += 30;

        color.setAlphaF(angle / 360);
        brush.setColor(color);
    }
}


void WaitDialog::drawArcStyle(QPainter *painter)
{
    //    QColor color = palette().color(QPalette::WindowText);
    QColor color = m_fillColor;
    QConicalGradient gradient(0, 0, -m_startAngle);

    gradient.setColorAt(0, color);
    color.setAlpha(0);
    gradient.setColorAt(0.8, color);
    color.setAlpha(255);
    gradient.setColorAt(1, color);

    QPen pen;
    pen.setWidth(30);
    pen.setBrush(QBrush(gradient));
    painter->setPen(pen);

    painter->drawArc(-85, -85, 170, 170, 0 * 16, 360 * 16);
}


void WaitDialog::setText(QString str)
{
    m_text = str;
    update();
}


void WaitDialog::paintEvent(QPaintEvent *)
{
    QString key = QString("%1:%2:%3:%4:%5")
                  .arg(metaObject()->className())
                  .arg(width())
                  .arg(height())
                  .arg(m_startAngle)
                  .arg(m_style);
    QPixmap pixmap;
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    int side = qMin(width(), height());

    if(!QPixmapCache::find(key, &pixmap))
    {
        pixmap = generatePixmap(side);
        QPixmapCache::insert(key, pixmap);
    }

    painter.save();
    painter.translate(width() / 2 - side / 2, height() / 2 - side / 2);

    painter.drawPixmap(0, 0, side, side, pixmap);
    painter.restore();

    if(!m_text.isEmpty())
    {
        QFontMetrics fontMetrics = painter.fontMetrics();
        int w = fontMetrics.width(m_text);
//        int h = fontMetrics.height();

        painter.drawText(width() / 2 - w / 2, 0, m_text);
    }
}


void WaitDialog::slotSetCurrentState(quint64 stateId, QString txt)
{
    setText(txt);
    m_currentStateId = stateId;
    ewApp()->setVisible(id(), true);
}


void WaitDialog::slotRemoveCurrentState(quint64 stateId)
{
    if(stateId != m_currentStateId)
    {
        return;
    }

    QTime tm;
    tm.start();

    while(tm.elapsed() < 200)
    {
        QApplication::processEvents();
    }

    m_currentStateId = WaitController::instance()->getCurrentState();
    QString str = WaitController::instance()->getStateText(m_currentStateId);

    if(!str.isEmpty())
    {
        setText(str);
    }
    else
    {
        ewApp()->setVisible(id(), false);
        setText("");
    }
}


QSize WaitDialog::minimumSizeHint() const
{
    return QSize(20, 20);
}


QSize WaitDialog::sizeHint() const
{
    return QSize(100, 100);
}


QWidget *WaitDialog::getWidget()
{
    return this;
}


TaskPanel::TaskPanel(QWidget *parent)
    : QWidget(parent)
{
    m_currentId = -1;
    QHBoxLayout *mainLt = new QHBoxLayout(this);
    mainLt->setMargin(0);
    m_pLabel = new QLabel("", this);
    m_pWaitWidget = new WaitDialog(this);
    m_pWaitWidget->setColor(QColor(Qt::green));

    mainLt->addWidget(m_pWaitWidget);
    mainLt->addWidget(m_pLabel, 1);

    m_pWaitWidget->hide();
    m_pLabel->clear();

    //setFixedHeight(20);
}


void TaskPanel::slotSetCurrentState(quint64 id, QString txt)
{
    m_pLabel->setText(txt);
    m_currentId = id;
    m_pWaitWidget->show();
}


void TaskPanel::slotRemoveCurrentState(quint64 id)
{
    if(id != m_currentId)
    {
        return;
    }

    QTime tm;
    tm.start();

    while(tm.elapsed() < 200)
    {
        QApplication::processEvents();
    }

    m_currentId = WaitController::instance()->getCurrentState();
    QString str = WaitController::instance()->getStateText(m_currentId);

    if(!str.isEmpty())
    {
        m_pWaitWidget->show();
        m_pLabel->setText(str);
    }
    else
    {
        m_pWaitWidget->hide();
        m_pLabel->clear();
    }
}


WaitController::WaitController()
{
    m_pMutex = new QMutex(QMutex::Recursive);
    //m_pPanel = new TaskPanel();
}


void WaitController::slotPanelDestroyed()
{
    m_pWaitDialog = 0;
}


WaitController::~WaitController()
{
}


void WaitController::init()
{
    static bool inited = false;

    if(inited)
    {
        return;
    }

    if(0 == ewApp()->getMainWindowId())
    {
        return;
    }

    m_pWaitDialog = new WaitDialog();

    if(QThread::currentThread() != QApplication::instance()->thread())
    {
        m_pWaitDialog->moveToThread(QApplication::instance()->thread());
    }

    connect(this, SIGNAL(signalSetCurrentState(quint64,QString)), m_pWaitDialog
            , SLOT(slotSetCurrentState(quint64,QString)), Qt::QueuedConnection);
    connect(this, SIGNAL(signalRemoveCurrentState(quint64)), m_pWaitDialog
            , SLOT(slotRemoveCurrentState(quint64)), Qt::QueuedConnection);
    connect(m_pWaitDialog, SIGNAL(destroyed(QObject *)), this, SLOT(slotPanelDestroyed()));

    ew::EmbeddedWidgetStruct str;
    str.iface = m_pWaitDialog;
    str.addHided = true;
    str.widgetTag = "";
    str.isModal = true;
    str.addHided = true;
    str.header.hasHideHeaderButton = false;
    str.headerVisible = false;
    str.allowMoving = true;
    str.alignType =  ew::EWA_IN_PERCENTAGE_ALIGN;
    str.alignPoint = QPointF(50,50);
    str.szPolicy = ESP_FIXED;
    str.topOnHint = true;
    ew::EmbeddedHeaderStruct headStr;
    headStr.hasCloseButton = false;
    headStr.hasMinMaxButton = false;
    str.header = headStr;
    ewApp()->createWidget(str, ewApp()->getMainWindowId());
    inited = true;
}


WaitController *WaitController::instance()
{
    static WaitController self;
    return &self;
}


quint64 WaitController::setState(QString txt)
{
    m_pMutex->lock();
    init();
    quint64 id = getEmptyStateId();
    m_states.append(id);
    m_stateString.insert(id, txt);
    m_pMutex->unlock();

    emit signalSetCurrentState(id, txt);
    return id;
}


void WaitController::removeState(quint64 stateId)
{
    m_pMutex->lock();
    init();
    for(int i = 0; i < m_states.size(); i++)
    {
        if(m_states[i] == stateId)
        {
            m_states.remove(i);
        }
    }

    m_stateString.remove(stateId);
    m_pMutex->unlock();

    emit signalRemoveCurrentState(stateId);
}


QString WaitController::getStateText(quint64 id)
{
    m_pMutex->lock();
    init();
    QString txt = m_stateString.value(id);
    m_pMutex->unlock();

    return txt;
}


quint64 WaitController::getCurrentState()
{
    m_pMutex->lock();

    init();
    if(m_states.isEmpty())
    {
        return -1;
    }

    quint64 id = m_states.last();
    m_pMutex->unlock();

    return id;
}


quint64 WaitController::getEmptyStateId()
{
    m_pMutex->lock();
    init();
    quint64 emptyId = 0;
    QList<quint64> ids = m_stateString.keys();
    qSort(ids);

    for(quint64 i = 0; i < 10000000; i++)
    {
        if(!ids.contains(i))
        {
            emptyId = i;
            break;
        }
    }

    m_pMutex->unlock();

    return emptyId;
}

