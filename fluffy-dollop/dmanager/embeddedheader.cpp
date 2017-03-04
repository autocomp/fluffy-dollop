#include "embeddedheader.h"

#include "runninglabel.h"
#include <QApplication>
#include <QCursor>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPair>
#include <QPushButton>
#include <QWhatsThis>

using namespace ew;

EmbeddedHeader::EmbeddedHeader(QWidget *parent)
    : QWidget(parent)
      , m_buttonSize(20)
//    , m_isRunningHeader(false)
{
    setFocusPolicy(Qt::ClickFocus);
    loadIcons();
    m_visibleFlag = true;
    m_denyMove = false;
    setObjectName("EmbeddedWindow_EmbeddedHeader");

    m_windowMaximized = false;
    m_pAppIconLabel = new QLabel(this);
    m_pAppIconLabel->setScaledContents(true);
    m_appIcon = QApplication::windowIcon();
    m_pAppIconLabel->setPixmap(m_appIcon.pixmap(QSize(m_buttonSize, m_buttonSize)));
    m_pAppIconLabel->setFixedSize(m_buttonSize, m_buttonSize);

    m_pTytleLabel = new CurtailLabel(this);

    m_pTytleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    m_pTytleLabel->setText(QApplication::applicationName() );
    setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);



    m_pFoldedButtonsBtn = new QPushButton(m_menuIcon, "", this);
    m_pFoldedButtonsBtn->setFixedSize(QSize(m_buttonSize, m_buttonSize));
    m_pFoldedButtonsBtn->setIconSize(QSize(m_buttonSize - 4, m_buttonSize - 4));
    connect(m_pFoldedButtonsBtn, SIGNAL(clicked(bool)), this, SLOT(slotFoldedButtonsBtnClicked()));

    m_pButtonLayout = new QHBoxLayout();
    m_pButtonLayout->setMargin(0);
    m_pButtonLayout->setSpacing(3);
    m_headerLayout = new QHBoxLayout(this);



    m_pButtonLayout->setMargin(0);
    m_headerLayout->setMargin(0);
    m_headerLayout->addWidget(m_pAppIconLabel);
    m_headerLayout->addWidget(m_pTytleLabel, 1);

    m_headerLayout->addWidget(m_pFoldedButtonsBtn);
    m_headerLayout->addLayout(m_pButtonLayout);


    //Инициализация меню свернутых кнопок
    m_pFoldedButtonsMenu = new QMenu("V"); //создали меню
    //    init();
}


EmbeddedHeader::~EmbeddedHeader()
{
    foreach(WindowHeaderButtons button, m_foldableButtons.keys())
    {
        deleteHeaderButton(button);
    }
}


QString EmbeddedHeader::text()
{
    return m_pTytleLabel->text();
}


void EmbeddedHeader::slotButtonClicked(bool)
{
    bool acceptFlag = false;

    if(headerButton(WHB_MIDGET))
    {
        if(sender() == headerButton(WHB_MIDGET)->button
           || sender() == headerButton(WHB_MIDGET)->action)
        {
            emit signalWindowEvent(WHE_MIDGET, &acceptFlag);
        }
    }

    if(headerButton(WHB_HIDE_HEADER))
    {
        if(sender() == headerButton(WHB_HIDE_HEADER)->button
           || sender() == headerButton(WHB_HIDE_HEADER)->action)
        {
            emit signalWindowEvent(WHE_HIDE_HEADER, &acceptFlag);
        }
    }

    if(headerButton(WHB_SETTINGS))
    {
        if(sender() == headerButton(WHB_SETTINGS)->button
           || sender() == headerButton(WHB_SETTINGS)->action)
        {
            emit signalWindowEvent(WHE_SHOW_SETTINGS, 0);
        }
    }

    if(headerButton(WHB_STYLE))
    {
        if(sender() == headerButton(WHB_STYLE)->button
           || sender() == headerButton(WHB_STYLE)->action)
        {
            emit signalWindowEvent(WHE_SHOW_STYLE_WIDGET, 0);
        }
    }

    if(headerButton(WHB_COLLAPSE))
    {
        if(sender() == headerButton(WHB_COLLAPSE)->button
           || sender() == headerButton(WHB_COLLAPSE)->action)
        {
            emit signalWindowEvent(WHE_COLLAPSE, 0);
        }
    }

    if(headerButton(WHB_FULLNORMSCREEN))
    {
        if(sender() == headerButton(WHB_FULLNORMSCREEN)->button
           || sender() == headerButton(WHB_FULLNORMSCREEN)->action)
        {
            minMaxWidget(&acceptFlag);
        }
    }

    if(headerButton(WHB_CLOSE))
    {
        if(sender() == headerButton(WHB_CLOSE)->button
           || sender() == headerButton(WHB_CLOSE)->action)
        {
            emit signalWindowEvent(WHE_CLOSE, 0);
            return;
        }
    }

    if(headerButton(WHB_WHAT_THIS))
    {
        if(sender() == headerButton(WHB_WHAT_THIS)->button)
        {
            QPushButton *pb = headerButton(WHB_WHAT_THIS)->button;

            if(!pb->isChecked())
            {
                QWhatsThis::enterWhatsThisMode();
            }
        }
    }

    if(headerButton(WHB_OPTIMIZE_SIZES))
    {
        if(sender() == headerButton(WHB_OPTIMIZE_SIZES)->button
           || sender() == headerButton(WHB_OPTIMIZE_SIZES)->action)
        {
            emit signalWindowEvent(WHE_OPTIMIZE_SIZES, 0);
        }
    }

    if(headerButton(WHB_OPTIMIZE_SPACE))
    {
        if(sender() == headerButton(WHB_OPTIMIZE_SPACE)->button
           || sender() == headerButton(WHB_OPTIMIZE_SPACE)->action)
        {
            emit signalWindowEvent(WHE_OPTIMIZE_SPACE, 0);
        }
    }
}


void EmbeddedHeader::slotFoldedButtonsBtnClicked()
{
    QRect r = m_pFoldedButtonsMenu->geometry();
    QPoint p = QCursor::pos();

    r.setX(p.x());
    r.setY(p.y());
    m_pFoldedButtonsMenu->setGeometry(r);
    m_pFoldedButtonsMenu->exec();
}


void EmbeddedHeader::setDescription(QString str)
{
    m_descrStr = str;
}


//QString EmbeddedHeader::tytle()
//{
//if (this && m_pTytleLabel)
//    return m_pTytleLabel->text();
//return "";


//}


void EmbeddedHeader::setIcon(QPixmap pm)
{
    m_appIcon = QIcon(pm);
    m_pAppIconLabel->setPixmap(pm);
}


void EmbeddedHeader::setIcon(QIcon ic)
{
    m_appIcon = ic;
    m_pAppIconLabel->setPixmap(m_appIcon.pixmap(QSize(32, 32)));
}


void EmbeddedHeader::setToolTip(QString str)
{
    m_pTytleLabel->setToolTip(str);
}


void EmbeddedHeader::setVisisbleButton(WindowHeaderButtons button, bool vis)
{
    EmbeddedHeader::HeaderButton *fb = headerButton(button);

    if(!fb)
    {
        if(vis)
        {
            createWindowHeaderButton(button
                                     , getButtonIcon(button)
                                     , getButtonText(button)
                                     , true);
        }
    }
    else
    {
        if(!vis)
        {
            deleteHeaderButton(button);
        }
    }
}


void EmbeddedHeader::setText(QString str)
{
    m_pTytleLabel->setText(str);

    if(m_pTytleLabel->toolTip().isEmpty())
    {
        m_pTytleLabel->setToolTip(str);
    }
}


void EmbeddedHeader::mousePressEvent(QMouseEvent *e)
{
    m_windowMoveMode = true; // NOTE: Заменил тут фалс на тру, мне показалось что так правильнее
    emit signalWindowEvent(WHE_START_MOVE, &m_windowMoveMode);

    if(m_windowMoveMode)
    {
        m_pressedGlobalLastPos = e->globalPos();
    }

    checkCursor();
    e->setAccepted(true);
}


void EmbeddedHeader::checkCursor()
{
    if(m_windowMoveMode)
    {
        if(!m_cursorChanged)
        {
            m_cursorChanged = true;
            //setCursor(QCursor(Qt::ArrowCursor));
            setCursor(QCursor(Qt::SizeAllCursor));
        }
    }
    else
    {
        if(m_cursorChanged)
        {
            m_cursorChanged = false;
            setCursor(QCursor(Qt::ArrowCursor));
        }
    }
}


void EmbeddedHeader::minMaxWidget(bool *acceptFlag)
{
    HeaderButton *button = headerButton(WHB_FULLNORMSCREEN);

    if(nullptr == button)
    {
        return;
    }

    if(!m_windowMaximized)
    {
        emit signalWindowEvent(WHE_MAXIMIZE, acceptFlag);
    }
    else
    {
        emit signalWindowEvent(WHE_SHOW_NORMAL, acceptFlag);
    }

    if(acceptFlag)
    {
        if(m_windowMaximized)
        {
            button->button->setIcon(m_maxIcon);
            button->action->setIcon(m_maxIcon);
            m_windowMaximized = false;
        }
        else
        {
            button->button->setIcon(m_minIcon);
            button->action->setIcon(m_minIcon);
            m_windowMaximized = true;
        }
    }

    button->button->setToolTip(getButtonText(WHB_FULLNORMSCREEN, m_windowMaximized));
}


void EmbeddedHeader::mouseMoveEvent(QMouseEvent *e)
{
    if(m_windowMoveMode)
    {
        int dx = e->globalX() - m_pressedGlobalLastPos.x();
        int dy = e->globalY() - m_pressedGlobalLastPos.y();
        bool resFlag = false;
        emit signalMoveWindowRequest(dx, dy, &resFlag);

        if(resFlag)
        {
            m_pressedGlobalLastPos = e->globalPos();
        }
    }

    checkCursor();
}


void EmbeddedHeader::mouseReleaseEvent(QMouseEvent *e)
{
    m_pressedGlobalLastPos = e->globalPos();
    emit signalWindowEvent(WHE_END_MOVE, &m_windowMoveMode);

    if(m_windowMoveMode)
    {
        m_pressedGlobalLastPos = e->globalPos();
        m_windowMoveMode = false;
        emit signalWindowDropped();
    }

    if(m_cursorChanged)
    {
        m_cursorChanged = false;
        setCursor(QCursor(Qt::ArrowCursor));
    }

    checkCursor();
}


void EmbeddedHeader::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    foldHeaderButtons();
}


void EmbeddedHeader::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    bool acceptFlag = true;
    minMaxWidget(&acceptFlag);
}


void EmbeddedHeader::foldHeaderButtons()
{
    int delta = getButtonSpace();

    QMapIterator<WindowHeaderButtons, HeaderButton *> it(m_foldableButtons);

    while(it.hasNext())
    {
        it.next();
        HeaderButton *fBtn = it.value();
        QPushButton *b = fBtn->button;

        if(nullptr != b)
        {
            if(it.key() == WHB_CLOSE)
            {
                fBtn->button->setVisible(true);
                fBtn->action->setVisible(false);
                delta -= b->width() + m_pButtonLayout->spacing();
                continue;
            }

            if(delta > fBtn->button->width())
            {
                fBtn->button->setVisible(true);
                fBtn->action->setVisible(false);
            }
            else
            {
                fBtn->button->setVisible(false);
                fBtn->action->setVisible(true);
            }

            delta -= b->width() + m_pButtonLayout->spacing();
        }
        else
        {
            qWarning() << __FUNCTION__ << QString::fromLocal8Bit("Видимо кнопка не инициализирована.");
        }
    }

    m_pFoldedButtonsBtn->setVisible(!checkNoVisibleActions());
}


void EmbeddedHeader::setVisibleFlag(bool vis)
{
    m_visibleFlag = vis;
    setVisible(vis);
}


bool EmbeddedHeader::isVisibleFlagSetted()
{
    return m_visibleFlag;
}


//bool EmbeddedHeader::getIsRunningHeader() const
//{
//    return m_isRunningHeader;
//}

//void EmbeddedHeader::setIsRunningHeader(bool isRunningHeader)
//{
//    if(m_isRunningHeader != isRunningHeader)
//    {
//        m_isRunningHeader = isRunningHeader;

//        if(isRunningHeader)
//        {
//            delete qobject_cast<CurtailLabel*>(m_pTytleLabel);

//            m_pTytleLabel = new RunningLabel(QApplication::applicationName(), this);
//            m_pTytleLabel->setAlignment( Qt::AlignCenter );
//            setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
//        }

//        else
//        {
//            delete qobject_cast<RunningLabel*>(m_pTytleLabel);

//            m_pTytleLabel = new CurtailLabel(this);
//            m_pTytleLabel->setAlignment(Qt::AlignCenter);
//            m_pTytleLabel->setText(QApplication::applicationName() );
//            setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
//        }
//        m_headerLayout->addWidget(m_pTytleLabel,1);
//    }


//}

void EmbeddedHeader::setCloseIcon(QString icon)
{
    m_closeIcon    = QIcon(icon); redrawIcons();
}


void EmbeddedHeader::setMaxIcon(QString icon)
{
    m_maxIcon      = QIcon(icon); redrawIcons();
}


void EmbeddedHeader::setMinIcon(QString icon)
{
    m_minIcon      = QIcon(icon); redrawIcons();
}


void EmbeddedHeader::setCollapseIcon(QString icon)
{
    m_collapseIcon = QIcon(icon); redrawIcons();
}


void EmbeddedHeader::setAppIcon(QString icon)
{
    m_appIcon      = QIcon(icon); redrawIcons();
}


void EmbeddedHeader::setWhatThisIcon(QString icon)
{
    m_whatThisIcon = QIcon(icon); redrawIcons();
}


void EmbeddedHeader::setMenuIcon(QString icon)
{
    m_menuIcon     = QIcon(icon); redrawIcons();
}


void EmbeddedHeader::setStyleIcon(QString icon)
{
    m_styleIcon    = QIcon(icon); redrawIcons();
}


void EmbeddedHeader::setSettingsIcon(QString icon)
{
    m_settingsIcon = QIcon(icon); redrawIcons();
}


void EmbeddedHeader::setMidgetIcon(QString icon)
{
    m_midgetIcon   = QIcon(icon); redrawIcons();
}


void EmbeddedHeader::setHideHeaderIcon(QString icon)
{
    m_hideHeaderIcon   = QIcon(icon); redrawIcons();
}


void EmbeddedHeader::redrawIcons()
{
    if(headerButton(WHB_HIDE_HEADER               ))
    {
        headerButton(WHB_HIDE_HEADER               )->button->setIcon(m_hideHeaderIcon);
        headerButton(WHB_HIDE_HEADER               )->action->setIcon(m_hideHeaderIcon);
    }

    if(headerButton(WHB_CLOSE               ))
    {
        headerButton(WHB_CLOSE               )->button->setIcon(m_closeIcon);
        headerButton(WHB_CLOSE               )->action->setIcon(m_closeIcon);
    }

    if(headerButton(WHB_COLLAPSE            ))
    {
        headerButton(WHB_COLLAPSE            )->button->setIcon(m_collapseIcon);
        headerButton(WHB_COLLAPSE            )->action->setIcon(m_collapseIcon);
    }

    if(headerButton(WHB_FULLNORMSCREEN         ))
    {
        headerButton(WHB_FULLNORMSCREEN         )->button->setIcon(m_windowMaximized ? m_minIcon : m_maxIcon);
        headerButton(WHB_FULLNORMSCREEN         )->action->setIcon(m_windowMaximized ? m_minIcon : m_maxIcon);
    }

    if(headerButton(WHB_WHAT_THIS))
    {
        headerButton(WHB_WHAT_THIS)->button->setIcon(m_whatThisIcon);
        headerButton(WHB_WHAT_THIS)->action->setIcon(m_whatThisIcon);
    }

    if(headerButton(WHB_STYLE               ))
    {
        headerButton(WHB_STYLE               )->button->setIcon(m_styleIcon);
        headerButton(WHB_STYLE               )->action->setIcon(m_styleIcon);
    }

    if(headerButton(WHB_SETTINGS            ))
    {
        headerButton(WHB_SETTINGS            )->button->setIcon(m_settingsIcon);
        headerButton(WHB_SETTINGS            )->action->setIcon(m_settingsIcon);
    }

    if(headerButton(WHB_MIDGET              ))
    {
        headerButton(WHB_MIDGET              )->button->setIcon(m_midgetIcon);
        headerButton(WHB_MIDGET              )->action->setIcon(m_midgetIcon);
    }

    if(m_pFoldedButtonsBtn)
    {
        m_pFoldedButtonsBtn->setIcon(QIcon(m_menuIcon));
    }
}


int EmbeddedHeader::getButtonSpace()
{
    QFontMetrics fm(m_pTytleLabel->font());
    int headerW     = width();
    int tytleW      = fm.width(m_pTytleLabel->text());
    int appIconW    = m_pAppIconLabel->width();
    int foldedBtnW  = m_pFoldedButtonsBtn->width();
    int closeBtnW   = 0;

    if(headerButton(WHB_CLOSE))
    {
        headerButton(WHB_CLOSE)->button->width();
    }

    int ret =  headerW
              - tytleW
              - appIconW
              - foldedBtnW
              - closeBtnW
              - m_pButtonLayout->margin() * 2
              - m_pButtonLayout->spacing() * 2
    ;
    return ret;
}


bool EmbeddedHeader::checkNoVisibleActions()
{
    QMapIterator<WindowHeaderButtons, HeaderButton *> it(m_foldableButtons);

    while(it.hasNext())
    {
        it.next();
        HeaderButton *fBtn = it.value();

        if(fBtn->action->isVisible())
            return false;
    }

    return true;
}


void EmbeddedHeader::loadIcons()
{
    m_maxIcon      = QIcon(":/embwidgets/img/img/047_icons_32_window.png");
    m_minIcon      = QIcon(":/embwidgets/img/img/046_icons_32_window.png");
    m_closeIcon    = QIcon(":/embwidgets/img/img/black/002_icons_20_up_close.png");
    m_collapseIcon = QIcon(":/embwidgets/img/img/icon_5.png");
    m_whatThisIcon = QIcon(":/embwidgets/img/img/188_users_manual.png");
    m_menuIcon     = QIcon(":/embwidgets/img/img/105_key_menu_white.png");
    m_styleIcon    = QIcon(":/embwidgets/img/img/041_icons_32_style.png");
    m_settingsIcon = QIcon(":/embwidgets/img/img/1_widget_settings.png");
    m_midgetIcon   = QIcon(":/embwidgets/img/img/key_midget_white.png");
    m_hideHeaderIcon   = QIcon(":/embwidgets/img/img/007_icons_16_header.png");
    m_optimizePosIcon   = QIcon(":/embwidgets/img/img/optPos.png");
    m_optimizeSpaceIcon   = QIcon(":/embwidgets/img/img/optSpace.png");
}


void EmbeddedHeader::createWindowHeaderButton(EmbeddedHeader::WindowHeaderButtons btn, QIcon icon, QString descryption, bool isFoldable)
{
    HeaderButton *fb;

    if(m_foldableButtons.contains(btn))
    {
        fb = m_foldableButtons[btn];

        if(fb->action)
        {
            delete fb->action;
        }

        if(fb->button)
        {
            delete fb->button;
        }
    }
    else
    {
        fb = new HeaderButton();
    }

    fb->action = m_pFoldedButtonsMenu->addAction(icon, descryption);
    fb->action->setIcon(icon);
    fb->action->setText(descryption);

    fb->button = new QPushButton(icon, "", this);
    fb->button->setIconSize(QSize(m_buttonSize - 4, m_buttonSize - 4));
    fb->button->setFocusPolicy(Qt::NoFocus);
    fb->button->setFixedSize(m_buttonSize, m_buttonSize);
    fb->button->setToolTip(descryption);
    fb->isFoldable = isFoldable;

    connect(fb->button, SIGNAL(clicked(bool)), this, SLOT(slotButtonClicked(bool)));
    connect(fb->action, SIGNAL(triggered(bool)), this, SLOT(slotButtonClicked(bool)));

    m_pButtonLayout->addWidget(fb->button);
    m_foldableButtons.insert(btn, fb);

    QString htm = getButtonWhatThis(btn);
    fb->button->setWhatsThis(htm);
}


QIcon EmbeddedHeader::getButtonIcon(EmbeddedHeader::WindowHeaderButtons button)
{
    QIcon ret;

    switch(button)
    {
      case WHB_CLOSE:
      {
          ret = m_closeIcon;
      } break;
      case WHB_FULLNORMSCREEN:
      {
          ret = (isMinimized()) ? (m_minIcon) : (m_maxIcon);
      } break;
      case WHB_COLLAPSE:
      {
          ret = m_collapseIcon;
      } break;
          //ret = m_whatThisIcon;
          break;
      case WHB_WHAT_THIS:
      {
          ret = m_whatThisIcon;
      } break;
      case WHB_SETTINGS:
      {
          ret = m_settingsIcon;
      } break;

      case WHB_STYLE:
      {
          ret = m_styleIcon;
      } break;
      case WHB_MIDGET:
      {
          ret = m_midgetIcon;
      } break;
      case WHB_HIDE_HEADER:
      {
          ret = m_hideHeaderIcon;
      } break;
      case WHB_OPTIMIZE_SPACE:
      {
          ret = m_optimizeSpaceIcon;
      } break;
      case WHB_OPTIMIZE_SIZES:
      {
          ret = m_optimizePosIcon;
      } break;
      default:

          break;
    }

    return ret;
}


QString EmbeddedHeader::getButtonWhatThis(EmbeddedHeader::WindowHeaderButtons button)
{
    QString htmDir = ":/embwidgets/htm/htm";

    //    QDir dr(htmDir);
    QString htmName;

    switch(button)
    {
      case WHB_CLOSE:
      { htmName = "closebtn"; } break;
      case WHB_COLLAPSE:
      { htmName = "collapsebtn"; } break;
      case WHB_WHAT_THIS:
      { htmName = "whatthisbtn"; } break;
      case WHB_GLOBAL_ONPARENT_MOVE:
      { htmName = "global_onparent_movebtn"; } break;
      case WHB_LOCKUNLOCK:
      { htmName = "lockunlockbtn"; } break;
      case WHB_FULLNORMSCREEN:
      { htmName = "manmaximizebtn"; } break;
      case WHB_PINUNPIN:
      { htmName = "pinunpinbtn"; } break;
      case WHB_SETTINGS:
      { htmName = "settingsbtn"; } break;
      case WHB_STYLE:
      { htmName = "stylebtn"; } break;
      case WHB_HIDE_HEADER:
      { htmName = "hideheaderbtn"; } break;
      case WHB_OPTIMIZE_SIZES:
      { htmName = "optimizespacebtn"; } break;
      case WHB_OPTIMIZE_SPACE:
      { htmName = "optimizesizesbtn"; } break;
      default:
      { return ""; };
          break;
    }

    QFile f(htmDir + QDir::separator() + htmName + ".htm");

    if(f.exists())
    {
        bool res = f.open(QFile::ReadOnly);

        if(!res)
        {
            return "";
        }

        QString htm = QString::fromLocal8Bit(f.readAll());
        f.close();
        return htm;
    }

    return "";
}


QString EmbeddedHeader::getButtonText(EmbeddedHeader::WindowHeaderButtons button, bool state)
{
    if(button == WHB_CLOSE               )
    {
        return tr("Close"             );
    }

    if(button == WHB_COLLAPSE            )
    {
        return tr("Collapse"          );
    }

    if(button == WHB_WHAT_THIS         )
    {
        return tr("What this"       );
    }

    if(button == WHB_GLOBAL_ONPARENT_MOVE)
    {
        return tr("onParrentMove"     );
    }

    if(button == WHB_LOCKUNLOCK          )
    {
        return tr("Lock"              );
    }

    if(button == WHB_FULLNORMSCREEN      )
    {
        return state ? (tr("minimize")) : (tr("Maximize"));
    }

    if(button == WHB_PINUNPIN            )
    {
        return tr("Pin/unpin"         );
    }

    if(button == WHB_SETTINGS            )
    {
        return tr("Settings"          );
    }

    if(button == WHB_STYLE               )
    {
        return tr("Style"             );
    }

    if(button == WHB_MIDGET              )
    {
        return tr("Collapse to midget");
    }

    if(button == WHB_HIDE_HEADER         )
    {
        return tr("Hide header");
    }

    if(button == WHB_OPTIMIZE_SIZES      )
    {
        return tr("Optimize size");
    }

    if(button == WHB_OPTIMIZE_SPACE      )
    {
        return tr("Optimize space");
    }

    return "";
}


void EmbeddedHeader::deleteHeaderButton(EmbeddedHeader::WindowHeaderButtons button)
{
    HeaderButton *b = headerButton(button);

    delete b->action;
    delete b->button;
    delete b;
    m_foldableButtons.remove(button);
}


EmbeddedHeader::HeaderButton *EmbeddedHeader::headerButton(EmbeddedHeader::WindowHeaderButtons button)
{
    if(m_foldableButtons.contains(button))
    {
        return m_foldableButtons[button];
    }

    return 0;
}
