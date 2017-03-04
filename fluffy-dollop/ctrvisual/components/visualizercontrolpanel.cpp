#include "visualizercontrolpanel.h"
#include <QFrame>
#include <libembeddedwidgets/embeddedapp.h>
#include <QStyleOption>
#include <QPainter>

using namespace visualize_system;

VisualizerControlPanel::VisualizerControlPanel(QWidget *parent) :
    QWidget(parent)
{
    mainLayout = new QHBoxLayout(this);
    mainLayout->setSpacing(3);
    mainLayout->setContentsMargins(3,1,3,1);

    setItemSize(32,32);
    ctrBtnHide = new QToolButton(this);
    ctrBtnHide->setIcon(QIcon(":/img/082_icons_32_uplist.png"));
    ctrBtnHide->setFixedSize(32,32);

    mainLayout->addWidget(ctrBtnHide);
    connect(ctrBtnHide, SIGNAL(clicked()), this, SLOT(hidePanel()));

    ctrBtnShow = new QToolButton(parent);
    ctrBtnShow->setIcon(QIcon(":/img/082_icons_32_downlist.png"));
    ctrBtnShow->setFixedSize(32,32);
    ctrBtnShow->hide();

    connect(ctrBtnShow, SIGNAL(clicked()), this, SLOT(showPanel()));

    st1 = new QState();
    st2 = new QState();

    an1 = new QPropertyAnimation(this, "size");
    an1->setEasingCurve(QEasingCurve::InOutCubic);
    an1->setDuration(100);
    machine_.addDefaultAnimation(an1);

    //connect(_iface, SIGNAL(signalClosed()), this, SLOT(widgetClose()));
}

VisualizerControlPanel::~VisualizerControlPanel()
{
    delete mainLayout;
}

QAction *VisualizerControlPanel::addItem(const QString &itemName, const QString &itemTitle, const QIcon &itemIcon, const QString &itemToolTip, bool chekable)
{
    QToolButton *pNewBtn = new QToolButton(this);
    pNewBtn->setProperty("buttonName",itemName);
    QAction *pDefAct = new QAction(pNewBtn);
    pDefAct->setCheckable(chekable);

    if(itemTitle != "")
    {
         if(!isTitleVisible)
         pDefAct->setText(itemTitle);
    }

    if(!itemIcon.isNull())
    {
         pDefAct->setIcon(itemIcon);

         if(itemSize.width() > 0 && itemSize.height() > 0)
         {
             pNewBtn->setFixedSize(itemSize);
             pNewBtn->setIconSize(QSize(itemSize.width() - 8, itemSize.height() - 8));
         }
    }
    else
    {
         pNewBtn->setFixedSize(itemSize);
    }

    if(itemToolTip != "")
    {
         pDefAct->setToolTip(itemToolTip);
    }
    pNewBtn->setDefaultAction(pDefAct);

    mainLayout->addWidget(pNewBtn);
    mainLayout->removeWidget(ctrBtnHide);
    mainLayout->addWidget(ctrBtnHide);
    hAllButtons.insert(pDefAct,pNewBtn);

    setFixedHeight(36);
    setFixedWidth(36+hAllButtons.count() * 36);
   // initAnimationStates();

    return pDefAct;
}

QAction *VisualizerControlPanel::addMenuItem(QList<QAction *> menuItems, const QString &menuTitle, const QIcon &menuIcon, const QString &menuToolTip)
{
    QMenu *pCtxMenu = nullptr;
    QToolButton *pMenuBtn = nullptr;

    if(!mAllMenus.isEmpty())
    {
        if(mAllMenus.contains(menuTitle))
        {
            pCtxMenu = mAllMenus.value(menuTitle);
            pCtxMenu->addActions(menuItems);

            return nullptr;
        }
        else
        {
            pCtxMenu = new QMenu(this);
        }
    }
    else
    {
        pCtxMenu = new QMenu(this);
    }

    pCtxMenu->addActions(menuItems);

    QAction *ac = addItem("",menuTitle, menuIcon, menuToolTip);
    pMenuBtn = hAllButtons.value(ac);
    pMenuBtn->setMenu(pCtxMenu);
    pMenuBtn->setPopupMode(QToolButton::InstantPopup);
    mAllMenus.insert(menuTitle,pCtxMenu);
    return ac;
}

void VisualizerControlPanel::paintEvent(QPaintEvent *pe)
{
    QPainter painter(this);
    QStyleOption styleOption;
    styleOption.initFrom( this );
    style()->drawPrimitive(QStyle::PE_Widget, &styleOption, &painter, this);

    QWidget::paintEvent(pe);
}

const QSize &VisualizerControlPanel::getItemSize()
{
    return itemSize;
}

int VisualizerControlPanel::getPanelWidth()
{
    return panelWidth;
}

int VisualizerControlPanel::count()
{
    return hAllButtons.count();
}

QIcon VisualizerControlPanel::getMenuIcon()
{
    return QIcon(":/vcpMenuIcon");
}

void VisualizerControlPanel::init(int visId)
{
  /**  _iface = new EmbIFaceNotifier(this);

    QString tag("ControlPanel");
    quint64 widgetId = ewApp()->restoreWidget(tag, _iface);
    if(0 == widgetId)
    {
        ew::EmbeddedWidgetStruct struc;
        ew::EmbeddedHeaderStruct headStr;

        headStr.hasCloseButton = true;
        headStr.windowTitle = tr("MainPanel");
        headStr.headerPixmap = ":/layerselectorplg_img/019_icons_32_layers.png";

        struc.alignPoint = QPoint(10,10);
        struc.alignType = ew::EWA_IN_RIGHT_TOP_CORNER_OFFSET;
        struc.widgetTag = tag;
//        struc.minSize = QSize(0,0);
//        struc.size = QSize(0,0);
        struc.headerVisible = false;
        struc.iface = _iface;
        struc.topOnHint = true;
        struc.szPolicy = ew::ESP_FIXED;
        struc.autoAdjustSize = true;
        struc.allowMoving = false;
        struc.headerVisible = false;




        ewApp()->createWidget(struc, visId);
    }

    ewApp()->setVisible(_iface->id(), false);*/
}

void VisualizerControlPanel::setTitleVisible(bool tf)
{
    Qt::ToolButtonStyle ts;
    isTitleVisible = tf;

    if(tf)
    {
        ts = Qt::ToolButtonTextUnderIcon;
    }
    else
    {
        ts = Qt::ToolButtonIconOnly;
    }

    for(QToolButton *pBtn: hAllButtons)
    {
        pBtn->setToolButtonStyle(ts);
    }

}

void VisualizerControlPanel::setItemSize(uint w, uint h)
{
    itemSize = QSize(w,h);
    for(QToolButton *pBtn: hAllButtons)
    {
        pBtn->setFixedSize(itemSize);
        pBtn->setIconSize(QSize(itemSize.width() - 8, itemSize.height() - 8));
    }
}

void VisualizerControlPanel::addSeparator()
{
    QFrame *line = new QFrame(this);
    line->setObjectName(QString::fromUtf8("VCP_group_separator"));
    line->setFrameShape(QFrame::VLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background: #555");

    mainLayout->addWidget(line);
}

void VisualizerControlPanel::setPluginButtonEnabled(const QString &buttonName, bool on_off)
{
    for(QToolButton *pBtn: hAllButtons)
    {
        if(buttonName == pBtn->property("buttonName").toString())
        {
            pBtn->setEnabled(on_off);
            break;
        }
    }
}

void VisualizerControlPanel::initAnimationStates()
{
    machine_.removeState(st1);
    machine_.removeState(st2);

    st1->assignProperty(this, "size", QSize(width(),0));
    st2->assignProperty(this, "size", QSize(width(),36));

    st1->addTransition(this, SIGNAL(needShow()), st2);
    st2->addTransition(this, SIGNAL(needHide()), st1);

    machine_.addState(st1);
    machine_.addState(st2);
    machine_.setInitialState(st1);

   // machine_.removeDefaultAnimation(an1);

    if(an1)
        delete an1;

    an1 = new QPropertyAnimation(this, "size");
    an1->setEasingCurve(QEasingCurve::InOutCubic);
    an1->setDuration(100);
    machine_.addDefaultAnimation(an1);

    machine_.start();
}

void VisualizerControlPanel::hidePanel()
{
    hide();
    ctrBtnShow->show();
    if(sender() == ctrBtnHide)
    {
        emit visibleChanged(false);
    }
    ///ewApp()->setVisible(_iface->id(), false);
}

void VisualizerControlPanel::showPanel()
{
    show();
    ctrBtnShow->hide();
    if(sender() == ctrBtnShow)
    {
        emit visibleChanged(true);
    }
    ///ewApp()->setVisible(_iface->id(), true);
}

void VisualizerControlPanel::movePanel(int x, int y)
{
    ctrBtnShow->move(x - ctrBtnShow->width() -4, 4);
    move(x - width() - 4, 4);
}
