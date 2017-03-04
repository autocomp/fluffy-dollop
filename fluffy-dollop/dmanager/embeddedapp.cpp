#include "embeddedapp.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QDir>
#include <QSizePolicy>
#include <QTextEdit>
#include <QTranslator>
#include <QVariantMap>

#include "docdebugger.h"
#include "embeddedgroupwidget.h"
#include "embeddedlogger.h"
#include "embeddedlogger.h"
#include "embeddedmainwindow.h"
#include "embeddedmidget.h"
#include "embeddednetworkmanager.h"
#include "embeddedpanel.h"
#include "embeddedsessionmanager.h"
#include "embeddedsettingswidget.h"
#include "embeddedutils.h"
#include "embeddedwidget.h"
#include "embeddedwindow.h"
#include "styledebugger.h"
#include "stylesheetdialog.h"
#include "waitdialog.h"

#ifndef QT_V5
#include <QCleanlooksStyle>
#endif


using namespace ew;

EmbeddedApp::EmbeddedApp(QObject *parent) : QObject(parent)
{
    m_closeAppDenied = false;
    m_mainWindowId = -1;
    m_activeWidgetId = 0;
    m_pMainWindow = nullptr;
    connect(qApp, SIGNAL(focusChanged(QWidget *,QWidget *)),
            this, SLOT(slotAppFocusChanged(QWidget *,QWidget *)));

#ifndef QT_V5
    QApplication::setStyle(new QCleanlooksStyle());
#endif

    qApp->installEventFilter(this);
}


bool EmbeddedApp::eventFilter(QObject *obj, QEvent *ev)
{
    switch(ev->type())
    {
      case QEvent::Close:
      {
          auto ptr = getWidget(getMainWindowId());

          if(obj == qApp || getWidget(getMainWindowId()) == dynamic_cast<ew::EmbeddedWidget *>(obj))
          {
              if(ptr)
              {
                  close();

                  if(isCloseDenied())
                  {
                      ev->ignore();
                      return true;
                  }
              }
          }
      } break;
      default:
          break;
    }

    return false;
}


void EmbeddedApp::slotAppFocusChanged(QWidget *oldWdg, QWidget *newWdg)
{
    Q_UNUSED(oldWdg);

    if(nullptr == newWdg)
    {
        auto oldWidgetPtr = getWidget(m_activeWidgetId);

        if(oldWidgetPtr)
        {
            oldWidgetPtr->setActiveWidget(false);
            m_activeWidgetId = 0;
        }

        ew::EmbeddedActionLogger::instance()->slotFocusChanged(oldWidgetPtr, nullptr);
        return;
    }

    QWidget *focusWdg = newWdg /*QApplication::focusWidget()*/;
    QWidget *focusWdgParent = focusWdg;

    while(0 != focusWdgParent)
    {
        ew::EmbeddedWidget *embWidg = dynamic_cast<ew::EmbeddedWidget *>(focusWdgParent);

        if(nullptr != embWidg)
        {
            if( (nullptr != dynamic_cast<ew::EmbeddedWindow *>(focusWdgParent))
                && embWidg->isHeaderVisible())
            {
                auto embPtr = ewApp()->getPrivateStruct(embWidg->id());

                if(embPtr)
                {
                    if(0 != embPtr->parentId)
                    {
                        focusWdgParent = ewApp()->getWidget(embPtr->parentId);
                        continue;
                    }
                }
                else
                {
                    focusWdgParent = focusWdgParent->parentWidget();
                    continue;
                }
            }

            auto oldWidgetPtr = getWidget(m_activeWidgetId);

            if(oldWidgetPtr)
            {
                if(oldWidgetPtr->id() != embWidg->id())
                {
                    oldWidgetPtr->setActiveWidget(false);
                }
                else
                {
                    // фокус окна верхнего уровня не изменился
                    return;
                }
            }

            //qDebug() << "EmbeddedApp::slotAppFocusChanged()";
            embWidg->raise();
            embWidg->setActiveWidget(true);
            m_activeWidgetId = embWidg->id();
            ew::EmbeddedActionLogger::instance()->slotFocusChanged(oldWidgetPtr, embWidg);
            return;
        }
        else
        {
            if(nullptr != focusWdgParent)
            {
                focusWdgParent = focusWdgParent->parentWidget();
                continue;
            }
            else
            {
                return;
            }
        }
    }

    ew::EmbeddedActionLogger::instance()->slotFocusChanged(nullptr, nullptr);
}


quint64 EmbeddedApp::getActiveWidgetId()
{
    return m_activeWidgetId;
}


quint64 EmbeddedApp::setWaitState(QString txt)
{
    return WaitController::instance()->setState(txt);
}


void EmbeddedApp::removeWaitState(quint64 stateId)
{
    WaitController::instance()->removeState(stateId);
}


QString EmbeddedApp::getWaitText(quint64 stateId)
{
    return WaitController::instance()->getStateText(stateId);
}


quint64 EmbeddedApp::getWaitCurrentState()
{
    return WaitController::instance()->getCurrentState();
}


ew::EmbeddedApp *EmbeddedApp::instance()
{
    static EmbeddedApp embApp;

    return &embApp;
}


EmbeddedApp::~EmbeddedApp()
{
}


void EmbeddedApp::about(quint64 parentId, const QString &title, const QString &text)
{
    QMessageBox *messBox = new QMessageBox(QMessageBox::Information, title, text, QMessageBox::Ok, 0);

    auto iface = new EmbeddedFace(messBox);

    ew::EmbeddedWidgetStruct str;
    str.header.windowTitle = title;
    str.header.hasCloseButton = true;
    str.header.hasCollapseButton = false;
    str.header.hasGlobalMoveButton = false;
    str.header.hasPinButton = false;
    str.header.hasStyleButton = false;

    str.header.hasWhatButton = false;
    str.header.hasLockButton = false;
    str.header.hasMinMaxButton = false;
    str.widgetTag = "";
    str.autoAdjustSize = true;
    str.addHided = false;
    str.topOnHint = true;
    str.size = QSize();
    str.isModal = true;
    str.iface = iface;
    quint64 id = ewApp()->createWidget(str, parentId);

    ewApp()->removeWidget(id);

    delete iface;
    delete messBox;

    return;
}


QMessageBox::StandardButton
EmbeddedApp::critical(quint64 parentId, const QString &title
                      , const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton, const QString & detailedText)
{
    QMessageBox *messBox = new QMessageBox(QMessageBox::Critical, title, text, buttons, 0);

    messBox->setDefaultButton(defaultButton);
    messBox->setDetailedText(detailedText);

    auto iface = new EmbeddedFace(messBox);
    ew::EmbeddedWidgetStruct str;
    str.header.windowTitle = title;
    str.header.hasCloseButton = true;
    str.header.hasCollapseButton = false;
    str.header.hasGlobalMoveButton = false;
    str.header.hasPinButton = false;
    str.header.hasStyleButton = false;
    str.autoAdjustSize = true;
    str.header.hasWhatButton = false;
    str.header.hasLockButton = false;
    str.header.hasMinMaxButton = false;
    str.widgetTag = "";
    str.addHided = false;
    str.topOnHint = true;
    str.size = QSize();
    str.isModal = true;
    str.iface = iface;
    quint64 id = ewApp()->createWidget(str, parentId);

    QMessageBox::StandardButton btn = (QMessageBox::StandardButton)messBox->result();
    ewApp()->removeWidget(id);


    if(0 == btn)
    {
        btn = defaultButton;
    }

    delete iface;
    delete messBox;

    return btn;
}


QMessageBox::StandardButton
EmbeddedApp::information(quint64 parentId, const QString &title
                         , const QString &text, QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton, QString detailText)
{
    QMessageBox *messBox = new QMessageBox(QMessageBox::Information, title, text, buttons, 0);

    messBox->setDefaultButton(defaultButton);
    messBox->setDetailedText(detailText);


    auto iface = new EmbeddedFace(messBox);
    ew::EmbeddedWidgetStruct str;
    str.header.windowTitle = title;
    str.header.hasCloseButton = true;
    str.header.hasCollapseButton = false;
    str.header.hasGlobalMoveButton = false;
    str.header.hasPinButton = false;
    str.header.hasStyleButton = false;
    str.header.hasWhatButton = false;
    str.header.hasLockButton = false;
    str.header.hasMinMaxButton = false;
    str.autoAdjustSize = true;
    str.widgetTag = "";
    str.addHided = false;
    str.topOnHint = true;
    str.isModal = true;
    str.iface = iface;

    //QSize sz = EmbeddedUtils::getWidgetSizeHint(iface->getWidget());
    quint64 id = ewApp()->createWidget(str, parentId);

    QMessageBox::StandardButton btn = (QMessageBox::StandardButton)messBox->result();
    ewApp()->removeWidget(id);


    if(0 == btn)
    {
        btn = defaultButton;
    }

    delete iface;
    delete messBox;

    return btn;
}


QMessageBox::StandardButton
EmbeddedApp::question(quint64 parentId, const QString &title, const QString &text
                      , QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton, const QString & detailedText)
{
    QMessageBox *messBox = new QMessageBox(QMessageBox::Question, title, text, buttons, 0);

    if(!detailedText.isEmpty())
    {
        messBox->setDetailedText(detailedText);
    }

    messBox->setDefaultButton(defaultButton);

    auto iface = new EmbeddedFace(messBox);
    ew::EmbeddedWidgetStruct str;
    str.header.windowTitle = title;
    str.header.hasCloseButton = true;
    str.header.hasCollapseButton = false;
    str.header.hasGlobalMoveButton = false;
    str.header.hasPinButton = false;
    str.header.hasStyleButton = false;

    str.header.hasWhatButton = false;
    str.header.hasLockButton = false;
    str.header.hasMinMaxButton = false;
    str.widgetTag = "";
    str.addHided = true;
    str.topOnHint = true;
    str.size = QSize();
    str.isModal = true;
    str.iface = iface;
    str.autoAdjustSize = true;

    quint64 id = ewApp()->createWidget(str, parentId);
    ewApp()->setVisible(id, true);
    QMessageBox::StandardButton btn = (QMessageBox::StandardButton)messBox->result();
    ewApp()->removeWidget(id);

    if(0 == btn)
    {
        btn = defaultButton;
    }

    delete iface;
    delete messBox;

    return btn;
}


QMessageBox::StandardButton
EmbeddedApp::warning(quint64 parentId, const QString &title, const QString &text
                     , QMessageBox::StandardButtons buttons, QMessageBox::StandardButton defaultButton)
{
    QMessageBox *messBox = new QMessageBox(QMessageBox::Information, title, text, buttons, 0);

    messBox->setDefaultButton(defaultButton);
    auto iface = new EmbeddedFace(messBox);
    ew::EmbeddedWidgetStruct str;
    str.header.windowTitle = title;
    str.header.hasCloseButton = true;
    str.header.hasCollapseButton = false;
    str.header.hasGlobalMoveButton = false;
    str.header.hasPinButton = false;
    str.header.hasStyleButton = false;

    str.header.hasWhatButton = false;
    str.header.hasLockButton = false;
    str.header.hasMinMaxButton = false;
    str.widgetTag = "";
    str.addHided = false;
    str.topOnHint = true;
    str.isModal = true;
    str.iface = iface;
    str.autoAdjustSize = true;


    quint64 id = ewApp()->createWidget(str, parentId);

    QMessageBox::StandardButton btn = (QMessageBox::StandardButton)messBox->result();
    ewApp()->removeWidget(id);


    if(0 == btn)
    {
        btn = defaultButton;
    }

    delete iface;
    delete messBox;

    return btn;
}


bool EmbeddedApp::init(ew::SerializerType st, SerializerConfigBase *config, ew::network::EmbeddedNetworkManager::WorkMode wm, quint16 debugPort, QString docPath)
{
    qWarning() << __FUNCTION__ << tr(" is deprectaed");

    ew::EmbeddedApp::EmbeddedInitStruct iStruct;
    iStruct.serializerType = st;
    iStruct.serializerConfigBase = config;
    iStruct.workMode             = wm;
    iStruct.debugPort            = debugPort;
    iStruct.docPath              = docPath;

    return init(iStruct);
}


bool EmbeddedApp::init(ew::EmbeddedApp::EmbeddedInitStruct initStruct)
{
    QTranslator *trans = new QTranslator(this);

    if(trans->load(":/embwidgets/ts/ts/src_ru.qm"))
    {
        QCoreApplication::installTranslator(trans);
    }



    bool isFirstRun = false;
    ew::network::EmbeddedNetworkManager::instance()->init(initStruct.workMode, initStruct.debugPort);
    bool res = EmbeddedSessionManager::instance()->init(initStruct.serializerType, initStruct.serializerConfigBase, &isFirstRun);
    connect(EmbeddedSessionManager::instance()
            , SIGNAL(signalSessionLoaded(QString,QMap<quint64,EmbeddedSerializationData>,QMap<quint64,QMap<QString,QVariant> > )),
            this, SLOT(slotSessionLoaded(QString,QMap<quint64,EmbeddedSerializationData>,QMap<quint64,QMap<QString,QVariant> > )));

    if(isFirstRun)
    {
        ewApp()->setStylesheet(initStruct.defaultStylesheet);
    }

    if(ew::network::EmbeddedNetworkManager::WM_SERVER == initStruct.workMode)
    {
        QString path = initStruct.docPath;

        if(path.startsWith("./") || path.startsWith(QString(".") + QDir::separator()))
        {
            path = path.remove("./");
            path = path.remove(QString(".") + QDir::separator());
            path = path.prepend(QCoreApplication::applicationDirPath() + QDir::separator());
        }

        ew::network::DocDebugger::instance()->init(path);
    }

    if(initStruct.loggerFuncDebug)
    {
        ewInitDebug(initStruct.loggerFuncDebug);
    }

    if(initStruct.loggerFuncWarning)
    {
        ewInitDebug(initStruct.loggerFuncWarning);
    }

    if(initStruct.loggerFuncFatal)
    {
        ewInitDebug(initStruct.loggerFuncFatal);
    }

    if(initStruct.loggerFuncUserActions)
    {
        ewInitActionLogger(initStruct.loggerFuncUserActions);
    }

    return res;
}


ew::EmbeddedWidget *EmbeddedApp::getWidget(quint64 id)
{
    if(0 != m_pMainWindow)
    {
        if(id == m_pMainWindow->id())
        {
            return m_pMainWindow;
        }
    }

    if(m_widgetMap.contains(id))
    {
        return m_widgetMap.value(id)->embWidgetPtr;
    }

    return 0;
}


QList<EmbeddedMidget *> EmbeddedApp::getMidgetsByParent(quint64 parentId)
{
    QList<EmbeddedMidget *> lst;
    QMapIterator<quint64, EmbeddedWidgetBaseStructPrivate *> iter(m_widgetMap);

    while(iter.hasNext())
    {
        iter.next();
        EmbeddedWidgetBaseStructPrivate *basePriv = iter.value();
        EmbeddedWidgetStructPrivate *priv = dynamic_cast<EmbeddedWidgetStructPrivate *>(basePriv);

        if((priv->parentId == parentId) && (nullptr != priv->embMidgetPtr))
        {
            lst.append(priv->embMidgetPtr);
        }
    }

    return lst;
}


EmbeddedMidget *EmbeddedApp::getMidget(quint64 id)
{
    auto ptr = dynamic_cast<EmbeddedGroupStructPrivate *>(m_widgetMap.value(id));

    if(0 != ptr)
    {
        return ptr->embMidgetPtr;
    }

    return 0;
}


QRect EmbeddedApp::getParentViewRect(quint64 id)
{
    QRect parentRect;

    auto wdgPtr = ewApp()->getPrivateStruct(id);

    ew::EmbeddedWidgetBaseStructPrivate *parentPtr = 0;

    if(nullptr != wdgPtr)
    {
        if(!wdgPtr->embWidgetStruct->isModal)
        {
            parentPtr = ewApp()->getPrivateStruct(wdgPtr->parentId);

            if(parentPtr)
            {
                parentRect = parentPtr->embWidgetPtr->getViewRect();
            }
        }
    }

    return parentRect;
}


void EmbeddedApp::showSettingsWidget(quint64 widgetId)
{
    ew::EmbeddedWidgetBaseStructPrivate *eWdg = getPrivateStruct(widgetId);

    if(nullptr == eWdg)
    {
        return;
    }

    switch(eWdg->ewType)
    {
      case EWT_WIDGET:
      {
          EmbeddedSettingsWidget dlg(*(eWdg->embWidgetStruct));
          dlg.installStyleFile(getMainStylesheetFile());
          dlg.exec();
      } break;
      case EWT_GROUP:
      {
      } break;
      case EWT_MAINWINDOW:
      {
      } break;
      case EWT_WINDOW:
      {
      } break;
      default:
      {
      } break;
    }
}


void EmbeddedApp::showStyleWidget(quint64 widgetId)
{
    if(0 == m_pStyleDialog)
    {
        m_pStyleDialog =  new StylesheetDialog();
        QString tag = "styleDialog";
        quint64 id = ewApp()->restoreWidget(tag, m_pStyleDialog);

        if(0 == id)
        {
            ew::EmbeddedWidgetStruct str;
            str.header.windowTitle = tr("Select style");
            str.header.hasCloseButton = true;
            str.header.hasCollapseButton = false;
            str.header.hasGlobalMoveButton = false;
            str.header.hasPinButton = false;
            str.header.hasStyleButton = false;

            str.header.hasWhatButton = false;
            str.header.hasLockButton = false;
            str.header.hasMinMaxButton = false;
            str.widgetTag = "styleDialog";
            str.minSize = QSize(200, 300);
            str.size = QSize(264, 385);
            str.addHided = true;
            str.topOnHint = true;
            str.isModal = true;
            str.iface = m_pStyleDialog;
            ewApp()->createWidget(str, 0);
        }

        ewApp()->setVisible(m_pStyleDialog->id(), true);
    }
    else
    {
        ewApp()->setVisible(m_pStyleDialog->id(), true);
    }

    int retCode = m_pStyleDialog->result();
    QString str = m_pStyleDialog->selectedStyleFile();

    if(QDialog::Accepted != retCode)
    {
        return;
    }

    ew::EmbeddedWidgetBaseStructPrivate *priv = ewApp()->getPrivateStruct(widgetId);
    ew::EmbeddedWidget *eWdg = getWidget(widgetId);

    if(0 != eWdg)
    {
        QFile styleSheetFile(str);

        if(styleSheetFile.open(QFile::ReadOnly))
        {
            QString stStr = styleSheetFile.readAll();

            if(eWdg == m_pMainWindow)
            {
                QMapIterator<quint64, ew::EmbeddedWidgetBaseStructPrivate *> iter(m_widgetMap);

                while(iter.hasNext())
                {
                    iter.next();
                    iter.value()->embWidgetPtr->installStyle(stStr);
                    ew::EmbeddedWidgetBaseStructPrivate *priv = iter.value();
                    priv->embWidgetStruct->stylesheetFile = str;
                    ewApp()->embWidgetChanged(priv->id);
                }

                QMapIterator<QString, EmbeddedSerializationData> loadedIter(m_loadedMap);

                while(loadedIter.hasNext())
                {
                    loadedIter.next();
                    loadedIter.value().data->stylesheetFile = str;
                    ewApp()->embWidgetChanged(loadedIter.value().id);
                }
            }
            else
            {
                eWdg->setStyleSheet(stStr);
                priv->embWidgetStruct->stylesheetFile = str;
                ewApp()->embWidgetChanged(priv->id);
            }

            styleSheetFile.close();
        }
    }
}


quint64 EmbeddedApp::createMainWindowPrivate(const ew::EmbeddedMainWindowStruct & str, quint64 id, bool restore)
{
    m_denySave = true;

    if(nullptr == m_pMainWindow)
    {
        EmbeddedWindowStructPrivate *privWindowStruct = new EmbeddedWindowStructPrivate();
        privWindowStruct->id = (id);
        privWindowStruct->parentId = 0;
        privWindowStruct->ewType = EWT_MAINWINDOW;
        auto embWindowPtr = new EmbeddedMainWindow();
        m_pMainWindow = embWindowPtr;
        connect(embWindowPtr, SIGNAL(signalWidgetClosed()), this, SLOT(slotWidgetClosed()));
        privWindowStruct->embWidgetPtr = embWindowPtr;
        privWindowStruct->embWidgetPtr->setHeader(str.header);
        privWindowStruct->embWidgetPtr->setWindowTitle((str.header).windowTitle);
        privWindowStruct->embWidgetPtr->setId(privWindowStruct->id);

        auto mainWindStruct = new ew::EmbeddedMainWindowStruct(str);
        mainWindStruct->menuBar = str.menuBar;
        mainWindStruct->bottomWidget = str.bottomWidget;
        privWindowStruct->embWidgetStruct = mainWindStruct;
        embWindowPtr->move(privWindowStruct->embWidgetStruct->alignPoint.toPoint());
        privWindowStruct->embWidgetPtr->setMoveAllowed(str.headerVisible);
        privWindowStruct->embWidgetPtr->setResizeAllowed(str.szPolicy != ew::ESP_FIXED);

        embWindowPtr->installStyleFile(mainWindStruct->stylesheetFile);

        if(str.topOnHint)
        {
            privWindowStruct->embWidgetPtr->setWindowFlags(privWindowStruct->embWidgetPtr->windowFlags() | Qt::WindowStaysOnTopHint);
        }

        m_pMainWindow = embWindowPtr;
        m_pMainWindow->setHeader(str.header);

        if(nullptr != str.menuBar)
        {
            embWindowPtr->setMenuBar(str.menuBar);
        }

        if(nullptr != str.bottomWidget)
        {
            embWindowPtr->setBottomBar(str.bottomWidget);
        }

        /* проверка размеров виджетов */
        if(privWindowStruct->embWidgetStruct->size.isEmpty() || !privWindowStruct->embWidgetStruct->size.isValid())
        {
            privWindowStruct->embWidgetStruct->size = ew::EmbeddedUtils::getCorrectSize(EmbeddedUtils::getWidgetSizeHint(embWindowPtr)
                                                                                        , privWindowStruct->embWidgetStruct->minSize
                                                                                        , privWindowStruct->embWidgetStruct->maxSize);
        }

        /* ------- проверка размеров виджетов */

        m_widgetMap.insert(id, privWindowStruct);
        m_pMainWindow->setVisible(true);

        privWindowStruct->embWidgetPtr->setWidgetMaximized(privWindowStruct->embWidgetStruct->maximized);

        if(restore)
        {
            changeWidgetAlignGeometry(id, privWindowStruct->embWidgetStruct->alignPoint, privWindowStruct->embWidgetStruct->size);

            privWindowStruct->embWidgetPtr->setMinMaxSize(privWindowStruct->embWidgetStruct->minSize
                                                          , privWindowStruct->embWidgetStruct->maxSize);
        }
        else
        {
            QMargins marg = privWindowStruct->embWidgetPtr->getEmbeddedViewMargins();
            privWindowStruct->embWidgetStruct->minSize = EmbeddedUtils::getEmbeddedSize(privWindowStruct->embWidgetStruct->minSize, marg);
            privWindowStruct->embWidgetStruct->maxSize = EmbeddedUtils::getEmbeddedSize(privWindowStruct->embWidgetStruct->maxSize, marg);
            privWindowStruct->embWidgetStruct->size = EmbeddedUtils::getEmbeddedSize(privWindowStruct->embWidgetStruct->size, marg);

            privWindowStruct->embWidgetPtr->setMinMaxSize(privWindowStruct->embWidgetStruct->minSize
                                                          , privWindowStruct->embWidgetStruct->maxSize);

            changeWidgetAlignGeometry(id, privWindowStruct->embWidgetStruct->alignPoint, privWindowStruct->embWidgetStruct->size);
        }

        //privWindowStruct->embWidgetPtr->setVisible(!privWindowStruct->embWidgetStruct->addHided);

        ew::network::DocDebugger::instance()->installDoc(embWindowPtr);
    }
    else
    {
        return 0;
    }

    m_denySave = false;

    ewApp()->embWidgetChanged(id);


    return (id);
}


EmbeddedWidgetBaseStruct *EmbeddedApp::getWidgetStructById(quint64 id) const
{
    ew::EmbeddedWidgetBaseStructPrivate *privStruct =  m_widgetMap.value(id, 0);

    if(0 != privStruct)
    {
        return privStruct->embWidgetStruct;
    }

    return 0;
}


EmbeddedSubIFace *EmbeddedApp::getSubIFaceById(quint64 id)
{
    ew::EmbeddedWidgetBaseStructPrivate *priv =  m_widgetMap.value(id);

    if(0 != priv)
    {
        EmbeddedWidgetStruct *widgStrPtr = dynamic_cast<EmbeddedWidgetStruct *>(priv->embWidgetStruct);

        if(0 != widgStrPtr)
        {
            return widgStrPtr->iface;
        }

        EmbeddedWindowStruct *windStrPtr = dynamic_cast<EmbeddedWindowStruct *>(priv->embWidgetStruct);

        if(0 != windStrPtr)
        {
            return windStrPtr->iface;
        }
    }

    return 0;
}


void EmbeddedApp::changeWidgetAlignGeometry(quint64 id, QPointF alignPos, QSize widgetSize)
{
    ew::EmbeddedWidgetBaseStructPrivate *privBase =  getPrivateStruct(id);

    if(nullptr == privBase)
    {
        qWarning() << "EmbeddedApp::changeWidgetAlignGeometry(): Widget struct not found.";
        return;
    }

    if(0 == privBase->parentId )
    {
        QPointF calcedPos = EmbeddedUtils::convertFromAlignPoint(alignPos, privBase->embWidgetStruct->alignType
                                                                 , widgetSize, QApplication::desktop()->size()
                                                                 , QPointF(0, 0));
        changeWidgetGeometry(id, calcedPos, widgetSize);
    }
    else
    {
        ew::EmbeddedWidgetBaseStructPrivate *privParentBase =  getPrivateStruct(privBase->parentId);

        if(nullptr == privParentBase)
        {
            qWarning() << "EmbeddedApp::changeWidgetGeometry(): Parent widget struct not found.";
            return;
        }

        QPointF calcedPos;

        if(!privBase->embWidgetStruct->isModal)
        {
            calcedPos = EmbeddedUtils::convertFromAlignPoint(alignPos, privBase->embWidgetStruct->alignType
                                                             , widgetSize, privParentBase->embWidgetStruct->size
                                                             , QPointF(0, 0));
        }
        else
        {
            calcedPos = EmbeddedUtils::convertFromAlignPoint(alignPos, privBase->embWidgetStruct->alignType
                                                             , widgetSize, privParentBase->embWidgetStruct->size
                                                             , QPointF(0, 0));

            QPointF parentGlobalPos = privParentBase->embWidgetPtr->mapToGlobal(QPoint(0, 0));
            calcedPos = calcedPos + parentGlobalPos;
        }

        changeWidgetGeometry(id, calcedPos, widgetSize);
    }
}


void EmbeddedApp::changeWidgetGeometry(quint64 id, QPointF widgetPos, QSize widgetSize)
{
    ew::EmbeddedWidgetBaseStructPrivate *privBase =  getPrivateStruct(id);

    if(nullptr == privBase)
    {
        qWarning() << "EmbeddedApp::changeWidgetGeometry(): Widget struct not found.";
        return;
    }

    if(privBase->embWidgetStruct->maximized)
    {
        return;
    }

    QSize correctSize = ew::EmbeddedUtils::getCorrectSize(widgetSize
                                                          , privBase->embWidgetStruct->minSize
                                                          , privBase->embWidgetStruct->maxSize);

    if(0 == privBase->parentId)
    {
        privBase->embWidgetPtr->setUpdatesEnabled(false);

        privBase->embWidgetStruct->alignPoint
            = EmbeddedUtils::convertToAlignPoint(widgetPos, privBase->embWidgetStruct->alignType
                                                 , widgetSize, QApplication::desktop()->size()
                                                 , QPoint(0, 0));
        privBase->embWidgetStruct->size = correctSize;

        // qDebug() << "EmbeddedApp::alignPoint " << privBase->embWidgetStruct->alignPoint;
        privBase->embWidgetPtr->setGeometry(widgetPos.toPoint().x(), widgetPos.toPoint().y(), correctSize.width(), correctSize.height());
        privBase->embWidgetPtr->setUpdatesEnabled(true);
        ewApp()->embWidgetChanged(id);

        // qDebug() << "EmbeddedApp::changeWidgetGeometry() " << widgetPos.toPoint() << " name " << privBase->id;
    }
    else
    {
        ew::EmbeddedWidgetBaseStructPrivate *privParentBase =  getPrivateStruct(privBase->parentId);

        if(nullptr == privBase)
        {
            qWarning() << "EmbeddedApp::changeWidgetGeometry(): widget struct not found.";
            return;
        }

        if(nullptr == privParentBase)
        {
            qWarning() << "EmbeddedApp::changeWidgetGeometry(): Parent widget struct not found.";
            return;
        }


        QSize parentSize;

        if(privParentBase->embWidgetStruct->maximized)
        {
            parentSize = privParentBase->embWidgetPtr->size();
        }
        else
        {
            parentSize = privParentBase->embWidgetStruct->size;
        }

        correctSize = ew::EmbeddedUtils::getCorrectSize(correctSize
                                                        , privBase->embWidgetStruct->minSize
                                                        , parentSize);

        if(!privBase->embWidgetStruct->isModal)
        {
            bool correctGeomRes = true;
            QRect widgetRect = QRect(widgetPos.toPoint(), correctSize);

            if(ew::EWT_GROUP != privParentBase->type())
            {
                /// для группы возможна вставка за границы
                bool correctGeomRes = EmbeddedUtils::correctWidgetGeometry(
                    privParentBase->embWidgetPtr->getViewRect(), widgetRect
                    , privBase->embWidgetStruct->minSize, privBase->embWidgetStruct->maxSize);
            }

            if(!correctGeomRes)
            {
                return;
            }

            correctSize = widgetRect.size();
            widgetPos = widgetRect.topLeft();
        }
        else
        {
            correctSize = widgetSize;
        }

        if(ew::EWT_GROUP == privParentBase->type())
        {
            privBase->embWidgetPtr->setUpdatesEnabled(false);

            privBase->embWidgetStruct->alignPoint
                = EmbeddedUtils::convertToAlignPoint(widgetPos, privBase->embWidgetStruct->alignType
                                                     , widgetSize, privParentBase->embWidgetPtr->getViewSize()
                                                     , privParentBase->embWidgetPtr->pos());
            privBase->embWidgetStruct->size = correctSize;
            privBase->embWidgetPtr->setGeometry(widgetPos.toPoint().x(), widgetPos.toPoint().y(), correctSize.width(), correctSize.height());

            //            privBase->embWidgetPtr->move(widgetPos.toPoint());
            //            privBase->embWidgetPtr->resize(correctSize);

            privBase->embWidgetPtr->setUpdatesEnabled(true);
            ewApp()->embWidgetChanged(id);
        }
        else if(ew::EWT_WINDOW == privParentBase->type())
        {
            privBase->embWidgetPtr->setUpdatesEnabled(false);

            privBase->embWidgetStruct->alignPoint
                = EmbeddedUtils::convertToAlignPoint(widgetPos, privBase->embWidgetStruct->alignType
                                                     , widgetSize, privParentBase->embWidgetPtr->getViewSize()
                                                     , privParentBase->embWidgetPtr->pos());
            privBase->embWidgetStruct->size = correctSize;
            //            privBase->embWidgetPtr->resize(correctSize);
            //            privBase->embWidgetPtr->move(widgetPos.toPoint());
            privBase->embWidgetPtr->setGeometry(widgetPos.toPoint().x(), widgetPos.toPoint().y(), correctSize.width(), correctSize.height());

            privBase->embWidgetPtr->setUpdatesEnabled(true);
            ewApp()->embWidgetChanged(id);
        }
        else if(ew::EWT_MAINWINDOW == privParentBase->type())
        {
            privBase->embWidgetPtr->setUpdatesEnabled(false);

            if(!privBase->embWidgetStruct->isModal)
            {
                privBase->embWidgetStruct->alignPoint
                    = EmbeddedUtils::convertToAlignPoint(widgetPos, privBase->embWidgetStruct->alignType
                                                         , widgetSize, privParentBase->embWidgetPtr->getViewSize()
                                                         , privParentBase->embWidgetPtr->pos());
            }
            else
            {
                QPointF parentPos = privParentBase->embWidgetPtr->pos();
                privBase->embWidgetStruct->alignPoint
                    = EmbeddedUtils::convertToAlignPoint((widgetPos - parentPos), privBase->embWidgetStruct->alignType
                                                         , widgetSize, privParentBase->embWidgetPtr->size()
                                                         , privParentBase->embWidgetPtr->pos());
            }

            privBase->embWidgetStruct->size = correctSize;
            privBase->embWidgetPtr->setGeometry(widgetPos.toPoint().x(), widgetPos.toPoint().y(), correctSize.width(), correctSize.height());

            privBase->embWidgetPtr->setUpdatesEnabled(true);
            ewApp()->embWidgetChanged(id);
        }
    }
}


void EmbeddedApp::changeWidgetSizeInGroup(quint64 id, const QSize & sz)
{
    ew::EmbeddedWidgetBaseStructPrivate *privBase =  getPrivateStruct(id);

    if(nullptr == privBase)
    {
        qWarning() << "EmbeddedApp::changeWidgetSizeInGroup(): Widget struct not found.";
        return;
    }

    QSize desiredSize(sz);

    switch(privBase->embWidgetStruct->szPolicy)
    {
      case ESP_EXPAND_FREE:
      {
          desiredSize = ew::EmbeddedUtils::getCorrectSize(sz,
                                                          privBase->embWidgetStruct->minSize,
                                                          privBase->embWidgetStruct->maxSize);
          break;
      }

      case ESP_EXPAND_PROPORTIONAL:
      {
          double scaleW = static_cast<double>(sz.width()) /
                          static_cast<double>(privBase->embWidgetStruct->size.width());
          double scaleH = static_cast<double>(sz.height()) /
                          static_cast<double>(privBase->embWidgetStruct->size.height());
          double scale = qMin(scaleW, scaleH);
          int w = static_cast<int>(static_cast<double>(privBase->embWidgetStruct->size.width()) * scale);
          int h = static_cast<int>(static_cast<double>(privBase->embWidgetStruct->size.height()) * scale);
          desiredSize = ew::EmbeddedUtils::getCorrectSize(QSize(w, h),
                                                          privBase->embWidgetStruct->minSize,
                                                          privBase->embWidgetStruct->maxSize);
          break;
      }

      case ESP_FIXED:
      {
          desiredSize = privBase->embWidgetStruct->size;
          break;
      }
    }

    if(privBase->embWidgetStruct->size != desiredSize)
    {
        privBase->embWidgetStruct->size = desiredSize;
        privBase->embWidgetPtr->resize(desiredSize);
        ewApp()->embWidgetChanged(id);
    }
}


quint64 EmbeddedApp::getWidgetIdBySubIFacePtr(const EmbeddedSubIFace *iFacePtr)
{
    QMapIterator<quint64, ew::EmbeddedWidgetBaseStructPrivate *> iter(m_widgetMap);

    while(iter.hasNext())
    {
        iter.next();

        EmbeddedWidgetStruct *widgStrPtr = dynamic_cast<EmbeddedWidgetStruct *>(iter.value()->embWidgetStruct);

        if(0 != widgStrPtr)
        {
            if(iFacePtr == widgStrPtr->iface)
            {
                return iter.value()->id;
            }
        }

        EmbeddedWindowStruct *windStrPtr = dynamic_cast<EmbeddedWindowStruct *>(iter.value()->embWidgetStruct);

        if(0 != windStrPtr)
        {
            if(iFacePtr == windStrPtr->iface)
            {
                return iter.value()->id;
            }
        }
    }

    return 0;
}


quint64 EmbeddedApp::getWidgetIdByWidgetPtr(QWidget *wdgPtr)
{
    QMapIterator<quint64, ew::EmbeddedWidgetBaseStructPrivate *> iter(m_widgetMap);

    while(iter.hasNext())
    {
        iter.next();
        ew::EmbeddedWidgetStruct *wdgStruct =  dynamic_cast<ew::EmbeddedWidgetStruct *>(iter.value()->embWidgetStruct);

        if(wdgPtr == wdgStruct->iface->getWidget())
        {
            return iter.value()->id;
        }
    }

    return 0;
}


EmbeddedWidgetBaseStruct *EmbeddedApp::getWidgetStructByWidgetPtr(QWidget *wdgPtr)
{
    QMapIterator<quint64, ew::EmbeddedWidgetBaseStructPrivate *> iter(m_widgetMap);

    while(iter.hasNext())
    {
        iter.next();
        ew::EmbeddedWidgetStruct *wdgStruct =  dynamic_cast<ew::EmbeddedWidgetStruct *>(iter.value()->embWidgetStruct);

        if(nullptr != wdgStruct)
        {
            if(wdgPtr == wdgStruct->iface->getWidget())
            {
                return wdgStruct;
            }
        }
    }

    return 0;
}


void EmbeddedApp::showMidget(quint64 widgetId, bool state)
{
    auto privStr = dynamic_cast<EmbeddedWidgetStructPrivate *>(getPrivateStruct(widgetId));

    if(0 == privStr)
    {
        qWarning() << __FUNCTION__ << " midget not found";
        return;
    }

    ew::EmbeddedMidget *eMdg = privStr->embMidgetPtr;
    ew::EmbeddedWidget *eWdg = privStr->embWidgetPtr;

    if(eMdg)
    {
        eWdg->setVisible(!state);
        eMdg->setVisible(state);
    }
    else
    {
        qWarning() << __FUNCTION__ << " midget not found";
        return;
    }
}


quint64 EmbeddedApp::createServiceWidgetPrivate(const EmbeddedWidgetStruct &str, quint64 id, bool restore)
{
    Q_UNUSED(restore);

    m_denySave = true;

    quint64 parentId = 0;
    QWidget *parent = 0;
    bool parentFound = false;

    if(0 != parentId)
    {
        if(0 != m_pMainWindow)
        {
            if(parentId == m_pMainWindow->id())
            {
                parentFound = true;
                parent = m_pMainWindow;
            }
        }

        if(!parentFound)
        {
            if(m_widgetMap.contains(parentId))
            {
                EmbeddedWidgetBaseStructPrivate *privWidgetStruct = m_widgetMap.value(parentId);
                parentFound = true;
                parent = privWidgetStruct->embWidgetPtr;
            }
        }
    }

    EmbeddedWidgetStructPrivate *privWidgetStruct = new EmbeddedWidgetStructPrivate();
    connect(m_pMainWindow, SIGNAL(signalWidgetClosed()), this, SLOT(slotWidgetClosed()));
    privWidgetStruct->id = (id);
    privWidgetStruct->parentId = parentId;
    privWidgetStruct->embWidgetPtr = new EmbeddedWidget(parent);
    connect(privWidgetStruct->embWidgetPtr, SIGNAL(signalWidgetClosed()), this, SLOT(slotWidgetClosed()));
    privWidgetStruct->embWidgetPtr->setId(privWidgetStruct->id);
    privWidgetStruct->embWidgetPtr->setHeader(str.header);
    privWidgetStruct->embWidgetPtr->setWindowTitle((str.header).windowTitle);
    privWidgetStruct->embWidgetPtr->setWidget(str.iface->getWidget());
    privWidgetStruct->embWidgetStruct = new ew::EmbeddedWidgetStruct(str);

    privWidgetStruct->embWidgetPtr->installStyleFile(str.stylesheetFile);

    if(!str.widgetTag.isEmpty())
    {
        privWidgetStruct->embWidgetPtr->setObjectName(str.widgetTag);
    }

    /* проверка размеров виджетов */
    if(privWidgetStruct->embWidgetStruct->size.isEmpty() || !privWidgetStruct->embWidgetStruct->size.isValid())
    {
        privWidgetStruct->embWidgetStruct->size = ew::EmbeddedUtils::getCorrectSize(EmbeddedUtils::getWidgetSizeHint(privWidgetStruct->embWidgetPtr)
                                                                                    , privWidgetStruct->embWidgetStruct->minSize
                                                                                    , privWidgetStruct->embWidgetStruct->maxSize);
    }

    /* ------- проверка размеров виджетов */

    if(0 != parentId)
    {
        ew::EmbeddedWidget *_parentWidget = ewApp()->getWidget(parentId);

        if(0 != _parentWidget)
        {
            ew::EmbeddedMainWindow *mWindow = qobject_cast<ew::EmbeddedMainWindow *>(_parentWidget);

            if(0 != mWindow)
            {
                mWindow->addWidget(privWidgetStruct);
            }

            ew::EmbeddedGroupWidget *mGroup = qobject_cast<ew::EmbeddedGroupWidget *>(_parentWidget);

            if(0 != mGroup)
            {
                mGroup->addWidget(privWidgetStruct, privWidgetStruct->embWidgetStruct->alignPoint.toPoint());
            }

            ew::EmbeddedWindow *window = qobject_cast<ew::EmbeddedWindow *>(_parentWidget);

            if(0 != window)
            {
                window->setWidget(privWidgetStruct);
            }
        }

        ew::network::DocDebugger::instance()->installDoc(privWidgetStruct->embWidgetPtr);
    }
    else
    {
        privWidgetStruct->embWidgetPtr->setVisible(true);
    }

    m_widgetMap.insert(id, privWidgetStruct);

    m_denySave = false;
    ewApp()->embWidgetChanged(id);
    return (id);
}


//ew::EmbeddedWidgetBaseStructPrivate* EmbeddedApp::getPrivateStruct(quint64 id, ew::EmbeddedWidgetType tp )
//{
//    auto str = getPrivateStruct(id);
//    if(str->type() == tp)
//    {
//        return str;
//    }

//    return 0;


//}


QString EmbeddedApp::getMainStylesheetFile()
{
    if(0 != m_pMainWindow)
    {
        auto ptr = m_widgetMap.value(m_pMainWindow->id());

        if(ptr)
        {
            return ptr->embWidgetStruct->stylesheetFile;
        }
        else
        {
            return m_mainStylesheetFile;
        }
    }
    else
    {
        return m_mainStylesheetFile;
    }

    return "";
}


QString EmbeddedApp::getMainStylesheet()
{
    if(0 != m_pMainWindow)
    {
        QString str = m_widgetMap.value(m_pMainWindow->id())->embWidgetStruct->stylesheetFile;
        QFile styleSheetFile(str);

        if(styleSheetFile.open(QFile::ReadOnly))
        {
            QString stStr = styleSheetFile.readAll();
            styleSheetFile.close();
            return stStr;
        }
    }
    else
    {
        if(!m_mainStylesheetFile.isEmpty())
        {
            QFile styleSheetFile(m_mainStylesheetFile);

            if(styleSheetFile.open(QFile::ReadOnly))
            {
                QString stStr = styleSheetFile.readAll();
                styleSheetFile.close();
                return stStr;
            }
        }
    }

    return "";
}


QStringList EmbeddedApp::getAvailableStylesheets()
{
    QDir stDir(":/embwidgets/stylesheets/stylesheets");
    QStringList lst;

    lst.append("*.qss");
    lst = stDir.entryList(lst, QDir::Files);
    return lst;
}


void EmbeddedApp::setStylesheet(QString str)
{
    QDir stDir(":/embwidgets/stylesheets/stylesheets");
    QString fullStr = stDir.path() + QDir::separator() + str;
    QStringList lst;

    lst.append("*.qss");
    lst = stDir.entryList(lst, QDir::Files);

    if(lst.contains(str))
    {
        m_mainStylesheetFile = fullStr;
        QMapIterator<quint64, ew::EmbeddedWidgetBaseStructPrivate *> iter(m_widgetMap);

        while(iter.hasNext())
        {
            iter.next();
            iter.value()->embWidgetPtr->installStyleFile(str);
            ew::EmbeddedWidgetBaseStructPrivate *priv = iter.value();
            priv->embWidgetStruct->stylesheetFile = fullStr;
            ewApp()->embWidgetChanged(priv->id);
        }

        QMapIterator<QString, EmbeddedSerializationData> loadedIter(m_loadedMap);

        /// не созданным виджетам, но сохраненным в базе так же меняем стиль
        while(loadedIter.hasNext())
        {
            loadedIter.next();
            loadedIter.value().data->stylesheetFile = fullStr;
            //quint64 loadedId = loadedIter.value().id;
            ewApp()->embWidgetChanged(loadedIter.value().id);
        }
    }
}


bool EmbeddedApp::getLoadedStruct(quint64 id, EmbeddedSerializationData & data)
{
    QMapIterator<QString, EmbeddedSerializationData> loadedIter(m_loadedMap);

    while(loadedIter.hasNext())
    {
        loadedIter.next();

        if(id == loadedIter.value().id)
        {
            data = loadedIter.value();
            return true;
        }
    }

    return false;
}


ew::EmbeddedWidgetBaseStructPrivate *EmbeddedApp::getPrivateStruct(quint64 id)
{
    auto str = m_widgetMap.value(id);

    if(0 != str)
    {
        return str;
    }

    return 0;
}


EmbeddedWidgetBaseStruct *EmbeddedApp::getEmbeddedStruct(quint64 id)
{
    auto strPriv = getPrivateStruct(id);

    if(0 != strPriv)
    {
        return strPriv->embWidgetStruct;
    }

    return 0;
}


EmbeddedWidgetBaseStruct *EmbeddedApp::getEmbeddedStruct(quint64 id, ew::EmbeddedWidgetType tp)
{
    auto str = getPrivateStruct(id);

    if(0 != str)
    {
        if(str->type() == tp)
        {
            return str->embWidgetStruct;
        }
    }

    return 0;
}


quint64 EmbeddedApp::createGroupPrivate(const EmbeddedGroupStruct &str, quint64 id, QWidget *parentWidget, bool restore, quint64 parentId)
{
    m_denySave = true;
    QWidget *parent = 0;
    bool parentFound = false;

    if(!parentWidget)
    {
        if(0 != parentId)
        {
            if(0 != m_pMainWindow)
            {
                if(parentId == m_pMainWindow->id())
                {
                    parentFound = true;
                    parent = m_pMainWindow;
                }
            }

            if(!parentFound)
            {
                if(m_widgetMap.contains(parentId))
                {
                    EmbeddedWidgetBaseStructPrivate *privWindowStruct = m_widgetMap.value(parentId);
                    parentFound = true;
                    parent = privWindowStruct->embWidgetPtr;
                }
            }
            else
            {
                return 0;
            }
        }
    }
    else
    {
        parent = parentWidget;
    }

    EmbeddedGroupStructPrivate *privGroupStruct = new EmbeddedGroupStructPrivate();
    privGroupStruct->id = id;
    privGroupStruct->parentId = parentId;
    privGroupStruct->ewType = EWT_GROUP;

    if(!str.isModal)
    {
        auto groupPtr = new ew::EmbeddedGroupWidget(parent);
        privGroupStruct->embWidgetPtr = groupPtr;

        if(parentWidget)
        {
            if(parent->layout())
            {
                auto defLt = parent->layout();

                auto vLt = dynamic_cast<QVBoxLayout *>(defLt);
                auto hLt = dynamic_cast<QHBoxLayout *>(defLt);
                auto gridLt = dynamic_cast<QGridLayout *>(defLt);


                if(vLt)
                {
                    vLt->addWidget(privGroupStruct->embWidgetPtr, 1);
                }
                else if(hLt)
                {
                    hLt->addWidget(privGroupStruct->embWidgetPtr, 1);
                }
                else if(gridLt)
                {
                    gridLt->addWidget(privGroupStruct->embWidgetPtr);
                }
                else
                {
                    parent->layout()->addWidget(groupPtr);
                }
            }

            groupPtr->setExternalParent(parentWidget);

            switch(str.szPolicy)
            {
              case ESP_FIXED:
              {
                  groupPtr->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
              } break;
              case ESP_EXPAND_FREE:
              {
                  groupPtr->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
              } break;
              case ESP_EXPAND_PROPORTIONAL:
              {
              } break;
              default:
                  break;
            }

            auto sizePolicy = groupPtr->sizePolicy();
            sizePolicy.setHorizontalStretch(1);
            sizePolicy.setVerticalStretch(1);
            groupPtr->setSizePolicy(sizePolicy);
        }

        privGroupStruct->embWidgetPtr->setModal(false);
        privGroupStruct->embWidgetPtr->setWindowModality(Qt::NonModal);
    }
    else
    {
        privGroupStruct->embWidgetPtr = new ew::EmbeddedGroupWidget(0);
        privGroupStruct->embWidgetPtr->setModal(true);
    }

    privGroupStruct->embWidgetPtr->setId(privGroupStruct->id);
    privGroupStruct->embWidgetPtr->setHeader(str.header);
    privGroupStruct->embWidgetPtr->setWindowTitle((str.header).windowTitle);

    auto embGroupStruct = new EmbeddedGroupStruct(str);
    privGroupStruct->embWidgetStruct = embGroupStruct;
    privGroupStruct->embWidgetPtr->setHeaderVisisbility(str.headerVisible);
    privGroupStruct->embWidgetPtr->setMoveAllowed(str.headerVisible);
    privGroupStruct->embWidgetPtr->setResizeAllowed(str.szPolicy != ew::ESP_FIXED);

    privGroupStruct->embWidgetPtr->installStyleFile(privGroupStruct->embWidgetStruct->stylesheetFile);

    if(!str.widgetTag.isEmpty())
    {
        privGroupStruct->embWidgetPtr->setObjectName(str.widgetTag);
    }

    if(privGroupStruct->embWidgetStruct->header.hasMidgetButton && !str.isModal)
    {
        privGroupStruct->embMidgetPtr = new EmbeddedMidget(parent);
        connect(privGroupStruct->embWidgetPtr, SIGNAL(signalMidgetPlaced(QRect)), parent, SLOT(slotPlacedMidget(QRect)));
        connect(privGroupStruct->embWidgetPtr, SIGNAL(signalMidgetMoveRequest(QRect)), parent, SLOT(slotMidgetMoveRequest(QRect)));
        privGroupStruct->embMidgetPtr->setId(privGroupStruct->id);
        privGroupStruct->embMidgetPtr->setText((str.header).windowTitle);
    }

    /* проверка размеров виджетов */
    if(privGroupStruct->embWidgetStruct->size.isEmpty() || !privGroupStruct->embWidgetStruct->size.isValid())
    {
        privGroupStruct->embWidgetStruct->size = ew::EmbeddedUtils::getCorrectSize(   EmbeddedUtils::getWidgetSizeHint(privGroupStruct->embWidgetPtr)
                                                                                      , privGroupStruct->embWidgetStruct->minSize
                                                                                      , privGroupStruct->embWidgetStruct->maxSize);
    }

    /* ------- проверка размеров виджетов */


    if(str.topOnHint)
    {
        privGroupStruct->embWidgetPtr->setWindowFlags(privGroupStruct->embWidgetPtr->windowFlags() | Qt::WindowStaysOnTopHint);
    }

    if(0 != parentId && !str.isModal)
    {
        ew::EmbeddedWidget *_parentWidget = ewApp()->getWidget(parentId);

        if(0 != _parentWidget)
        {
            ew::EmbeddedMainWindow *mWindow = qobject_cast<ew::EmbeddedMainWindow *>(_parentWidget);

            if(0 != mWindow)
            {
                mWindow->addWidget(privGroupStruct);
            }

            ew::EmbeddedGroupWidget *mGroup = qobject_cast<ew::EmbeddedGroupWidget *>(_parentWidget);

            if(0 != mGroup)
            {
                mGroup->addWidget(privGroupStruct, privGroupStruct->embWidgetStruct->alignPoint.toPoint());
            }

            ew::EmbeddedWindow *window = qobject_cast<ew::EmbeddedWindow *>(_parentWidget);

            if(0 != window)
            {
                window->addEmbeddedWidget(privGroupStruct->id, privGroupStruct->embWidgetPtr, embGroupStruct);
            }
        }
    }

    m_widgetMap.insert(id, privGroupStruct);

    //changeWidgetPos(id, privGroupStruct->embWidgetStruct->alignPoint);
    privGroupStruct->embWidgetPtr->setWidgetMaximized(privGroupStruct->embWidgetStruct->maximized);

    if(restore)
    {
        changeWidgetAlignGeometry(id, privGroupStruct->embWidgetStruct->alignPoint, privGroupStruct->embWidgetStruct->size);

        privGroupStruct->embWidgetPtr->setMinMaxSize(privGroupStruct->embWidgetStruct->minSize
                                                     , privGroupStruct->embWidgetStruct->maxSize);
    }
    else
    {
        QMargins marg = privGroupStruct->embWidgetPtr->getEmbeddedViewMargins();
        privGroupStruct->embWidgetStruct->minSize = EmbeddedUtils::getEmbeddedSize(privGroupStruct->embWidgetStruct->minSize, marg);
        privGroupStruct->embWidgetStruct->maxSize = EmbeddedUtils::getEmbeddedSize(privGroupStruct->embWidgetStruct->maxSize, marg);
        privGroupStruct->embWidgetStruct->size = EmbeddedUtils::getEmbeddedSize(privGroupStruct->embWidgetStruct->size, marg);

        privGroupStruct->embWidgetPtr->setMinMaxSize(privGroupStruct->embWidgetStruct->minSize
                                                     , privGroupStruct->embWidgetStruct->maxSize);

        if(!privGroupStruct->embWidgetStruct->isModal )
        {
            changeWidgetAlignGeometry(id, privGroupStruct->embWidgetStruct->alignPoint, privGroupStruct->embWidgetStruct->size);
        }
        else
        {
            ewApp()->embWidgetChanged(id);

            if(QPointF(-1, -1) == privGroupStruct->embWidgetStruct->alignPoint)
            {
                QPoint widgetPos(0, 0);
                auto ptr = getPrivateStruct(parentId);

                if(!m_pMainWindow)
                {
                    widgetPos =  QApplication::desktop()->availableGeometry().center();
                }
                else
                {
                    if(ptr)
                    {
                        widgetPos = ptr->embWidgetPtr->window()->geometry().center();
                    }
                    else
                    {
                        widgetPos = m_pMainWindow->geometry().center();
                    }
                }

                QSize wSz = EmbeddedUtils::getWidgetSizeHint(privGroupStruct->embWidgetPtr);
                widgetPos = QPoint(widgetPos.x() -  wSz.width() / 2
                                   , widgetPos.y() - wSz.height() / 2);

                privGroupStruct->parentId = 0;

                if(widgetPos.x() < 0)
                {
                    widgetPos.setX(0);
                }

                if(widgetPos.y() < 0)
                {
                    widgetPos.setY(0);
                }

                privGroupStruct->embWidgetStruct->alignPoint = widgetPos;
            }

            changeWidgetAlignGeometry(id, privGroupStruct->embWidgetStruct->alignPoint, privGroupStruct->embWidgetStruct->size);
        }
    }

    if(privGroupStruct->embWidgetStruct->isModal )
    {
        m_denySave = false;
        ewApp()->embWidgetChanged(id);


        ewApp()->embWidgetChanged(id);

        if(!privGroupStruct->embWidgetStruct->addHided)
        {
            ewApp()->embWidgetChanged(id);
            privGroupStruct->embWidgetPtr->raise();
            ewApp()->setVisible(id, true);
        }

        m_denySave = false;

        return id;
    }
    else
    {
        privGroupStruct->embWidgetPtr->setVisible(!privGroupStruct->embWidgetStruct->addHided);
    }

    m_denySave = false;


    ewApp()->embWidgetChanged(id);

    return (id);
}


quint64 EmbeddedApp::createWindowPrivate(const ew::EmbeddedWindowStruct & str, quint64 id, bool restore, quint64 parentId)
{
    m_denySave = true;

    QWidget *parent = 0;
    bool parentFound = false;

    if(0 != parentId)
    {
        if(0 != m_pMainWindow)
        {
            if(parentId == m_pMainWindow->id())
            {
                parentFound = true;
                parent = m_pMainWindow;
            }
        }

        if(!parentFound)
        {
            if(m_widgetMap.contains(parentId))
            {
                EmbeddedWidgetBaseStructPrivate *privWindowStruct = m_widgetMap.value(parentId);
                parentFound = true;
                parent = privWindowStruct->embWidgetPtr;
            }
        }
    }


    EmbeddedWindowStructPrivate *privWindowStruct = new EmbeddedWindowStructPrivate();
    privWindowStruct->id = id;
    privWindowStruct->ewType = EWT_WINDOW;
    privWindowStruct->parentId = parentId;
    EmbeddedWindow *embWindowPtr = nullptr;

    if(!str.isModal)
    {
        embWindowPtr = new ew::EmbeddedWindow(parent);
        embWindowPtr->setModal(false);
        embWindowPtr->setWindowModality(Qt::NonModal);
    }
    else
    {
        embWindowPtr = new ew::EmbeddedWindow(0);
        embWindowPtr->setModal(true);
    }

    connect(embWindowPtr, SIGNAL(signalWidgetClosed()), this, SLOT(slotWidgetClosed()));
    privWindowStruct->embWidgetPtr = embWindowPtr;
    privWindowStruct->embWidgetPtr->setHeader(str.header);
    privWindowStruct->embWidgetPtr->setWindowTitle((str.header).windowTitle);
    privWindowStruct->embWidgetPtr->setId(privWindowStruct->id);
    privWindowStruct->embWidgetStruct = new ew::EmbeddedWindowStruct(str);
    privWindowStruct->embWidgetPtr->setMoveAllowed(str.headerVisible);
    privWindowStruct->embWidgetPtr->setResizeAllowed(str.szPolicy != ew::ESP_FIXED);


    privWindowStruct->embWidgetPtr->installStyleFile(privWindowStruct->embWidgetStruct->stylesheetFile);

    if(!str.widgetTag.isEmpty())
    {
        embWindowPtr->setObjectName(str.widgetTag);
    }

    privWindowStruct->embWidgetPtr->setWidget(str.iface->getWidget());
    m_widgetMap.insert((id), privWindowStruct);

    /* проверка размеров виджетов */
    if(privWindowStruct->embWidgetStruct->size.isEmpty() || !privWindowStruct->embWidgetStruct->size.isValid())
    {
        privWindowStruct->embWidgetStruct->size = ew::EmbeddedUtils::getCorrectSize(EmbeddedUtils::getWidgetSizeHint(embWindowPtr)
                                                                                    , privWindowStruct->embWidgetStruct->minSize
                                                                                    , privWindowStruct->embWidgetStruct->maxSize);
    }

    /* ------- проверка размеров виджетов */

    if(str.topOnHint)
    {
        privWindowStruct->embWidgetPtr->setWindowFlags(privWindowStruct->embWidgetPtr->windowFlags() | Qt::WindowStaysOnTopHint);
    }

    if(0 != parentId)
    {
        if(str.header.hasMidgetButton)
        {
            privWindowStruct->embMidgetPtr = new EmbeddedMidget(parent);
            connect(privWindowStruct->embMidgetPtr, SIGNAL(signalMidgetPlaced(QRect)), parent, SLOT(slotPlacedMidget(QRect)));
            connect(privWindowStruct->embMidgetPtr, SIGNAL(signalMidgetMoveRequest(QRect)), parent, SLOT(slotMidgetMoveRequest(QRect)));
            privWindowStruct->embMidgetPtr->setId(privWindowStruct->id);
            privWindowStruct->embMidgetPtr->setText((str.header).windowTitle);
        }

        ew::EmbeddedWidget *_parentWidget = ewApp()->getWidget(parentId);

        if(0 != _parentWidget)
        {
            ew::EmbeddedMainWindow *mWindow = qobject_cast<ew::EmbeddedMainWindow *>(_parentWidget);

            if(0 != mWindow)
            {
                mWindow->setWidget(privWindowStruct);

                if(!restore)
                {
                    QMargins marg = mWindow->getEmbeddedViewMargins();
                    QSize sz = privWindowStruct->embWidgetStruct->size;
                    sz = EmbeddedUtils::getEmbeddedSize(sz, marg);
                    ewApp()->changeWidgetAlignGeometry(parentId, privWindowStruct->embWidgetStruct->alignPoint, sz);
                }
            }

            ew::EmbeddedGroupWidget *mGroup = qobject_cast<ew::EmbeddedGroupWidget *>(_parentWidget);

            if(0 != mGroup)
            {
                mGroup->addWidget(privWindowStruct, privWindowStruct->embWidgetStruct->alignPoint.toPoint());
            }
        }
    }
    else
    {
        //privWindowStruct->embWidgetPtr->show();
        //embWindowPtr->move(privWindowStruct->embWidgetStruct->alignPoint.toPoint());
        ewApp()->changeWidgetAlignGeometry(embWindowPtr->id(), privWindowStruct->embWidgetStruct->alignPoint
                                           , privWindowStruct->embWidgetStruct->size);
    }

    embWindowPtr->slotUpdateChildsGeometry();

    privWindowStruct->embWidgetPtr->setVisible(!privWindowStruct->embWidgetStruct->addHided);

    checkChildWidgets(parentId);
    privWindowStruct->embWidgetPtr->setWidgetMaximized(privWindowStruct->embWidgetStruct->maximized);

    ewApp()->changeWidgetAlignGeometry(embWindowPtr->id(), privWindowStruct->embWidgetStruct->alignPoint
                                       , privWindowStruct->embWidgetStruct->size);

    if(restore)
    {
        changeWidgetAlignGeometry(id, privWindowStruct->embWidgetStruct->alignPoint, privWindowStruct->embWidgetStruct->size);

        privWindowStruct->embWidgetPtr->setMinMaxSize(privWindowStruct->embWidgetStruct->minSize
                                                      , privWindowStruct->embWidgetStruct->maxSize);
    }
    else
    {
        QMargins marg = privWindowStruct->embWidgetPtr->getEmbeddedViewMargins();
        privWindowStruct->embWidgetStruct->minSize = EmbeddedUtils::getEmbeddedSize(privWindowStruct->embWidgetStruct->minSize, marg);
        privWindowStruct->embWidgetStruct->maxSize = EmbeddedUtils::getEmbeddedSize(privWindowStruct->embWidgetStruct->maxSize, marg);
        privWindowStruct->embWidgetStruct->size = EmbeddedUtils::getEmbeddedSize(privWindowStruct->embWidgetStruct->size, marg);


        privWindowStruct->embWidgetPtr->setMinMaxSize(privWindowStruct->embWidgetStruct->minSize
                                                      , privWindowStruct->embWidgetStruct->maxSize);


        changeWidgetAlignGeometry(id, privWindowStruct->embWidgetStruct->alignPoint, privWindowStruct->embWidgetStruct->size);
    }

    m_denySave = false;


    if(privWindowStruct->embWidgetStruct->isModal && !privWindowStruct->embWidgetStruct->addHided)
    {
        ewApp()->embWidgetChanged(id);
        ew::network::DocDebugger::instance()->installDoc(embWindowPtr);

        if(restore)
        {
            str.iface->restore(m_propertyMap.value(id));
        }

        setVisible(privWindowStruct->embWidgetPtr->id(), true);
    }
    else
    {
        privWindowStruct->embWidgetPtr->setVisible(!privWindowStruct->embWidgetStruct->addHided);
    }

    ewApp()->embWidgetChanged(id);
    ew::network::DocDebugger::instance()->installDoc(embWindowPtr);

    if(restore)
    {
        str.iface->restore(m_propertyMap.value(id));
    }

    return (id);
}


quint64 EmbeddedApp::createWindow(const ew::EmbeddedWindowStruct & str, quint64 parentId)
{
    quint64 id = getUniquieId();

    createWindowPrivate(str, id, false, parentId);
    return id;
}


quint64 EmbeddedApp::createMainWindow(const ew::EmbeddedMainWindowStruct & str)
{
    quint64 id = getUniquieId();

    m_mainWindowId = id;
    createMainWindowPrivate(str, id, false);
    return m_mainWindowId;
}


quint64 EmbeddedApp::getMainWindowId()
{
    return m_mainWindowId;
}


quint64 EmbeddedApp::createWidget(const ew::EmbeddedWidgetStruct & str, quint64 parentId)
{
    quint64 id = getUniquieId();

    createWidgetPrivate(str, id, false, parentId);
    return id;
}


quint64 EmbeddedApp::createServiceWidget(const ew::EmbeddedWidgetStruct & str)
{
    quint64 id = getUniquieId();

    createServiceWidgetPrivate(str, id, false);
    return id;
}


quint64 EmbeddedApp::createGroup(const ew::EmbeddedGroupStruct & str, QWidget *parentWidget)
{
    quint64 id = getUniquieId();

    createGroupPrivate(str, id, parentWidget, false, 0);
    return id;
}


quint64 EmbeddedApp::createGroup(const ew::EmbeddedGroupStruct & str, quint64 parentId)
{
    quint64 id = getUniquieId();

    createGroupPrivate(str, id, nullptr, false, parentId);
    return id;
}


void EmbeddedApp::slotWidgetClosed()
{
    EmbeddedWidget *wdg = dynamic_cast<EmbeddedWidget *>(sender());

    if(nullptr == wdg)
    {
        qWarning() << __FUNCTION__ << "EmbeddedWidget not found";
        return;
    }

    //    if(0 != acceptFlag)
    //    {
    //        *acceptFlag = true;
    //    }


    quint64 wId = wdg->id();
    auto privStr = getPrivateStruct(wId);

    if(0 == privStr)
    {
        qWarning() << __FUNCTION__ << "EmbeddedWidgetStruct not found";
        return;
    }

    if(m_pMainWindow == wdg)
    {
        close();
    }
}


quint64 EmbeddedApp::getUniquieId()
{
    for(quint64 i = 1; i < UINT64_MAX; i++)
    {
        if(!m_widgetMap.contains(i) && !m_loadedMapIds.contains(i)
           && (i != m_mainWindowId))
        {
            return i;
        }
    }

    qCritical("EmbeddedApp::getUniquieId: unique id not found! FATAL");
    return 0;
}


quint64 EmbeddedApp::restoreMainWidget(QString tag, QMenuBar *menuBar, QWidget *bottomPanel )
{
    bool restored = false;
    quint64 restoredId = 0;

    if(m_loadedMap.contains(tag))
    {
        EmbeddedSerializationData data = m_loadedMap.value(tag);
        restoredId = data.id;
        m_mainWindowId = restoredId;

        switch(data.tp)
        {
          case EWT_MAINWINDOW:
          {
              auto _data = dynamic_cast<ew::EmbeddedMainWindowStruct *>(data.data);

              if(!_data)
              {
                  return 0;
              }

              _data->menuBar = menuBar;
              _data->bottomWidget = bottomPanel;


              restoredId = createMainWindowPrivate(*_data, data.id, true);
          } break;
          default:
          {
              return 0;
          } break;
        }
    }

    if(restored)
    {
        m_loadedMap.remove(tag);
        m_loadedMapIds.removeAll(restoredId);
    }

    return restoredId;
}


quint64 EmbeddedApp::restoreWindow(QString tag, ew::EmbeddedSubIFace *iFase)
{
    return restoreWidget(tag, iFase);
}


quint64 EmbeddedApp::restoreWidget(QString tag, ew::EmbeddedSubIFace *iFase)
{
    bool restored = false;
    quint64 restoredId = 0;

    if(m_loadedMap.contains(tag))
    {
        EmbeddedSerializationData data = m_loadedMap.value(tag);
        restoredId = data.id;

        switch(data.tp)
        {
          case EWT_WINDOW:
          {
              auto _data = dynamic_cast<ew::EmbeddedWindowStruct *>(data.data);
              _data->iface = iFase;
              restoredId = createWindowPrivate(*_data, data.id, true, data.parent);
          } break;
          case EWT_WIDGET:
          {
              auto _data = dynamic_cast<ew::EmbeddedWidgetStruct *>(data.data);

              if(!_data)
              {
                  return 0;
              }

              _data->iface = iFase;
              restoredId = createWidgetPrivate(*_data, data.id, true, data.parent);
          } break;
          case EWT_SERVICE_WIDGET:
          {
              auto _data = dynamic_cast<ew::EmbeddedWidgetStruct *>(data.data);
              _data->iface = iFase;
              restoredId = createServiceWidgetPrivate(*_data, data.id, true);
          } break;
          default:
          {
              return 0;
          } break;
        }
    }

    if(restored)
    {
        m_loadedMap.remove(tag);
        m_loadedMapIds.removeAll(restoredId);
    }

    return restoredId;
}


quint64 EmbeddedApp::restoreGroup(QString tag, QWidget *parentWidget)
{
    bool restored = false;
    quint64 restoredId = 0;

    if(m_loadedMap.contains(tag))
    {
        EmbeddedSerializationData data = m_loadedMap.value(tag);
        restoredId = data.id;

        switch(data.tp)
        {
          case EWT_GROUP:
          {
              auto _data = dynamic_cast<ew::EmbeddedGroupStruct *>(data.data);
              //_data->iface = iFase;
              restoredId = createGroupPrivate(*_data, data.id, parentWidget, true, data.parent);
          } break;
          default:
          {
              return 0;
          } break;
        }
    }

    if(restored)
    {
        m_loadedMap.remove(tag);
        m_loadedMapIds.removeAll(restoredId);
    }

    return restoredId;
}


void EmbeddedApp::slotSessionLoaded(QString session, QMap<quint64, EmbeddedSerializationData> serialMap, QMap<quint64, QMap<QString, QVariant> > propMap)
{
    Q_UNUSED(session);
    /// TODO сделать переинициализацию сессии

    /// FIXME - отключено
    //return;
    QMapIterator<quint64, EmbeddedSerializationData> iter(serialMap);
    foreach (EmbeddedSerializationData data, serialMap)
    {
        m_loadedMap.insert(data.data->widgetTag, data);
        m_loadedMapIds.append(data.id);
    }

    m_propertyMap = propMap;
}


void EmbeddedApp::slotWidgetPosChanged(quint64 id, EmbeddedHeader::MovingState ms)
{
    bool used = false;

    if(!used)
    {
        used = checkGroupAction(id, ms);
    }

    if(!used)
    {
        used = checkGroupCreation(id, ms);
    }

    if(ew::EmbeddedHeader::MS_END == ms)
    {
        auto ptr = getPrivateStruct(id);

        if(nullptr != ptr)
        {
            if(0 == ptr->parentId)
            {
                //ptr->embWidgetStruct->alignPoint = ptr->embWidgetPtr->pos();
            }
        }
    }

    embWidgetChanged(id);
}


quint64 EmbeddedApp::createWidgetPrivate(const ew::EmbeddedWidgetStruct & str, quint64 id, bool restore, quint64 parentId)
{
    m_denySave = true;
    QWidget *parent = 0;
    bool parentFound = false;

    if(0 != parentId && !str.isModal)
    {
        if(0 != m_pMainWindow)
        {
            if(parentId == m_pMainWindow->id())
            {
                parentFound = true;
                parent = m_pMainWindow;
            }
        }

        if (!parentFound)
        {
            if(m_widgetMap.contains(parentId))
            {
                auto * privParentStruct = m_widgetMap.value(parentId);
                parentFound = true;
                parent = privParentStruct->embWidgetPtr;
            }
        }

        if(!parentFound)
        {
            //TODO: Что тогда?
            qWarning() << __FILE__ << ":" << __LINE__ << ":" << "Parent not found";
        }
    }

    auto embWidgetStruct = new ew::EmbeddedWidgetStruct(str);
    EmbeddedWidgetStructPrivate *privWidgetStruct = new EmbeddedWidgetStructPrivate();
    privWidgetStruct->id = (id);
    privWidgetStruct->parentId = parentId;

    ew::EmbeddedWidget *embWidgetPtr = 0;

    if(!str.isModal)
    {
        embWidgetPtr = new ew::EmbeddedWidget(parent);
        embWidgetPtr->setModal(false);
        embWidgetPtr->setWindowModality(Qt::NonModal);
    }
    else
    {
        embWidgetPtr = new ew::EmbeddedWidget(0);
        embWidgetPtr->setModal(true);
    }

    privWidgetStruct->embWidgetPtr = embWidgetPtr;
    privWidgetStruct->embWidgetPtr->setId(privWidgetStruct->id);
    privWidgetStruct->embWidgetPtr->setHeader(str.header);
    privWidgetStruct->embWidgetPtr->setWindowTitle((str.header).windowTitle);
    QWidget *_wdg = str.iface->getWidget();
    privWidgetStruct->embWidgetPtr->setWidget(_wdg);
    privWidgetStruct->embWidgetStruct = embWidgetStruct;
    privWidgetStruct->embWidgetPtr->setHeaderVisisbility(str.headerVisible);
    privWidgetStruct->embWidgetPtr->setMoveAllowed(str.headerVisible);


    privWidgetStruct->embWidgetPtr->installStyleFile(privWidgetStruct->embWidgetStruct->stylesheetFile);

    if(!str.widgetTag.isEmpty())
    {
        embWidgetPtr->setObjectName(str.widgetTag);
    }

    if(privWidgetStruct->embWidgetStruct->header.hasMidgetButton && !str.isModal)
    {
        privWidgetStruct->embMidgetPtr = new EmbeddedMidget(parent);
        connect(privWidgetStruct->embWidgetPtr, SIGNAL(signalMidgetPlaced(QRect)), parent, SLOT(slotPlacedMidget(QRect)));
        connect(privWidgetStruct->embWidgetPtr, SIGNAL(signalMidgetMoveRequest(QRect)), parent, SLOT(slotMidgetMoveRequest(QRect)));
        privWidgetStruct->embMidgetPtr->setId(privWidgetStruct->id);
        privWidgetStruct->embMidgetPtr->setText((str.header).windowTitle);
    }

    /* проверка размеров виджетов */
    if(privWidgetStruct->embWidgetStruct->size.isEmpty() || !privWidgetStruct->embWidgetStruct->size.isValid())
    {
        privWidgetStruct->embWidgetStruct->size = ew::EmbeddedUtils::getCorrectSize(   EmbeddedUtils::getWidgetSizeHint(_wdg)
                                                                                       , privWidgetStruct->embWidgetStruct->minSize
                                                                                       , privWidgetStruct->embWidgetStruct->maxSize);
    }

    /* ------- проверка размеров виджетов */


    if(str.topOnHint)
    {
        privWidgetStruct->embWidgetPtr->setWindowFlags(privWidgetStruct->embWidgetPtr->windowFlags() | Qt::WindowStaysOnTopHint);
    }

    if(0 != parentId && !str.isModal)
    {
        ew::EmbeddedWidget *_parentWidget = ewApp()->getWidget(parentId);

        if(0 != _parentWidget)
        {
            ew::EmbeddedMainWindow *mWindow = qobject_cast<ew::EmbeddedMainWindow *>(_parentWidget);

            if(0 != mWindow)
            {
                mWindow->addWidget(privWidgetStruct);
            }

            ew::EmbeddedGroupWidget *mGroup = qobject_cast<ew::EmbeddedGroupWidget *>(_parentWidget);

            if(0 != mGroup)
            {
                mGroup->addWidget(privWidgetStruct, privWidgetStruct->embWidgetStruct->alignPoint.toPoint());
            }

            ew::EmbeddedWindow *window = qobject_cast<ew::EmbeddedWindow *>(_parentWidget);

            if(0 != window)
            {
                window->addEmbeddedWidget(privWidgetStruct->id, embWidgetPtr, embWidgetStruct);
            }
        }
    }

    m_widgetMap.insert(id, privWidgetStruct);

    //changeWidgetPos(id, privWidgetStruct->embWidgetStruct->alignPoint);
    privWidgetStruct->embWidgetPtr->setWidgetMaximized(privWidgetStruct->embWidgetStruct->maximized);

    if(restore)
    {
        changeWidgetAlignGeometry(id, privWidgetStruct->embWidgetStruct->alignPoint, privWidgetStruct->embWidgetStruct->size);

        privWidgetStruct->embWidgetPtr->setMinMaxSize(privWidgetStruct->embWidgetStruct->minSize
                                                      , privWidgetStruct->embWidgetStruct->maxSize);
    }
    else
    {
        QMargins marg = privWidgetStruct->embWidgetPtr->getEmbeddedViewMargins();
        privWidgetStruct->embWidgetStruct->minSize = EmbeddedUtils::getEmbeddedSize(privWidgetStruct->embWidgetStruct->minSize, marg);
        privWidgetStruct->embWidgetStruct->maxSize = EmbeddedUtils::getEmbeddedSize(privWidgetStruct->embWidgetStruct->maxSize, marg);
        privWidgetStruct->embWidgetStruct->size = EmbeddedUtils::getEmbeddedSize(privWidgetStruct->embWidgetStruct->size, marg);

        privWidgetStruct->embWidgetPtr->setMinMaxSize(privWidgetStruct->embWidgetStruct->minSize
                                                      , privWidgetStruct->embWidgetStruct->maxSize);

        if(!privWidgetStruct->embWidgetStruct->isModal )
        {
            changeWidgetAlignGeometry(id, privWidgetStruct->embWidgetStruct->alignPoint, privWidgetStruct->embWidgetStruct->size);
        }
        else
        {
            ewApp()->embWidgetChanged(id);

            if(QPointF(-1, -1) == privWidgetStruct->embWidgetStruct->alignPoint)
            {
                QPoint widgetPos(0, 0);
                auto ptr = getPrivateStruct(parentId);

                if(!m_pMainWindow)
                {
                    widgetPos =  QApplication::desktop()->availableGeometry().center();
                }
                else
                {
                    if(ptr)
                    {
                        widgetPos = ptr->embWidgetPtr->window()->geometry().center();
                    }
                    else
                    {
                        widgetPos = m_pMainWindow->geometry().center();
                    }
                }

                QSize wSz = EmbeddedUtils::getWidgetSizeHint(str.iface->getWidget());
                widgetPos = QPoint(widgetPos.x() -  wSz.width() / 2
                                   , widgetPos.y() - wSz.height() / 2);

                privWidgetStruct->parentId = 0;

                if(widgetPos.x() < 0)
                {
                    widgetPos.setX(0);
                }

                if(widgetPos.y() < 0)
                {
                    widgetPos.setY(0);
                }

                privWidgetStruct->embWidgetStruct->alignPoint = widgetPos;
            }

            changeWidgetAlignGeometry(id, privWidgetStruct->embWidgetStruct->alignPoint, privWidgetStruct->embWidgetStruct->size);
        }
    }

    privWidgetStruct->embWidgetPtr->setResizeAllowed(str.szPolicy != ew::ESP_FIXED);

    if(privWidgetStruct->embWidgetStruct->isModal )
    {
        m_denySave = false;
        ewApp()->embWidgetChanged(id);


        QDialog *dlg = dynamic_cast<QDialog *>(str.iface->getWidget());
        connect(dlg, SIGNAL(accepted()), privWidgetStruct->embWidgetPtr, SLOT(accept()));
        connect(dlg, SIGNAL(rejected()), privWidgetStruct->embWidgetPtr, SLOT(reject()));
        connect(dlg, SIGNAL(finished(int)), privWidgetStruct->embWidgetPtr, SLOT(close()));

        ewApp()->embWidgetChanged(id);

        if(!privWidgetStruct->embWidgetStruct->addHided)
        {
            ewApp()->embWidgetChanged(id);
            ew::network::DocDebugger::instance()->installDoc(embWidgetPtr);

            if(restore)
            {
                str.iface->restore(m_propertyMap.value(id));
            }

            privWidgetStruct->embWidgetPtr->raise();
            ewApp()->setVisible(id, true);
        }

        m_denySave = false;

        return id;
    }
    else
    {
        privWidgetStruct->embWidgetPtr->setVisible(!privWidgetStruct->embWidgetStruct->addHided);
    }

    m_denySave = false;


    ewApp()->embWidgetChanged(id);
    ew::network::DocDebugger::instance()->installDoc(embWidgetPtr);

    if(restore)
    {
        str.iface->restore(m_propertyMap.value(id));
    }

    return (id);
}


bool EmbeddedApp::removeWidget(quint64 widgetId)
{
    if(!m_widgetMap.contains(widgetId))
    {
        qWarning() << __FUNCTION__ << " embeddedWidget not with id " << widgetId << " not found";
        return false;
    }

    ew::EmbeddedWidgetBaseStructPrivate *priv = m_widgetMap.value(widgetId);


    // добавляем виджет в кеш
    EmbeddedSerializationData data;
    data.data = priv->embWidgetStruct;
    data.id = priv->id;
    data.parent = priv->parentId;
    data.tp = priv->ewType;
    m_loadedMap.insert(data.data->widgetTag, data);
    m_loadedMapIds.append(data.id);

    switch(priv->type())
    {
      case EWT_SERVICE_WIDGET:
      case EWT_WIDGET:
      {
          auto privWidget = dynamic_cast<EmbeddedWidgetStruct *>(priv->embWidgetStruct);
          setVisible(widgetId, false);
          priv->embWidgetPtr->removeWidget(privWidget->iface->getWidget());
          priv->embWidgetPtr->deleteLater();
          priv->embWidgetStruct = 0;
      }
      break;

      case EWT_GROUP:
      {
          auto childs = getIdsByParent(priv->id);
          //QList<EmbeddedWidget*> widgets = privWindow->m_widgets;
          foreach (quint64 chIds, childs)
          {
              removeWidget(chIds);
          }

          priv->embWidgetPtr->deleteLater();
          priv->embWidgetStruct = 0;
      } break;

      case EWT_WINDOW:
      {
          auto privWindow = dynamic_cast<EmbeddedWindowStruct *>(priv->embWidgetStruct);
          auto childs = getIdsByParent(priv->id);
          //QList<EmbeddedWidget*> widgets = privWindow->m_widgets;
          foreach (quint64 chIds, childs)
          {
              removeWidget(chIds);
          }

          priv->embWidgetPtr->removeWidget(privWindow->iface->getWidget() /*windowWidget*/);
          priv->embWidgetPtr->close();
          priv->embWidgetPtr->deleteLater();
          priv->embWidgetStruct = 0;
      } break;
      case EWT_MAINWINDOW:
      {
          auto childs = getIdsByParent(priv->id);
          //QList<EmbeddedWidget*> widgets = privWindow->m_widgets;
          foreach (quint64 chIds, childs)
          {
              removeWidget(chIds);
          }

          priv->embWidgetPtr->deleteLater();
          priv->embWidgetStruct = 0;
          close();
      } break;
      default:
      {
          qWarning() << __FUNCTION__ << " Unsupported widget type " << priv->type();
      }
      break;
    }

    m_widgetMap.remove(widgetId);

    // qWarning() << __FUNCTION__ << " embeddedWidget not with id " << widgetId << " not found";
    return false;
}


void EmbeddedApp::loadLastSession()
{
    EmbeddedSessionManager::instance()->loadSession();
}


QString EmbeddedApp::getWidgetTitle(quint64 id)
{
    EmbeddedWidgetBaseStructPrivate *str = m_widgetMap.value(id);

    if(str)
    {
        return str->embWidgetPtr->getWindowTitleText();
    }
    else
    {
        qWarning() << __FUNCTION__ << " Can't get tooltip, widget not found";
    }

    return "";
}


void EmbeddedApp::setWidgetIcon(quint64 id, QString iconPath)
{
    EmbeddedWidgetBaseStructPrivate *str = m_widgetMap.value(id);

    if(0 != str)
    {
        if(str->embWidgetPtr->header())
        {
            str->embWidgetPtr->header()->setIcon(QPixmap(iconPath));
            str->embWidgetStruct->header.headerPixmap = iconPath;

            emit embWidgetChanged(id);
        }
    }
    else
    {
        qWarning() << __FUNCTION__ << " Can't change tooltip, widget not found";
    }
}


void EmbeddedApp::setWidgetTitle(quint64 id, QString titleStr)
{
    EmbeddedWidgetBaseStructPrivate *str = m_widgetMap.value(id);

    if(0 != str)
    {
        str->embWidgetPtr->setWindowTitleText(titleStr);
    }
    else
    {
        qWarning() << __FUNCTION__ << " Can't change tooltip, widget not found";
    }
}


void EmbeddedApp::close()
{
    emit signalAppCloseRequest();

    if(isCloseDenied())
    {
        return;
    }

//    exit(0);
    qApp->exit();
}


bool EmbeddedApp::isCloseDenied()
{
    return m_closeAppDenied;
}


void EmbeddedApp::setCloseAppDenied(bool denyCloseFlag)
{
    m_closeAppDenied = denyCloseFlag;
}


quint64 EmbeddedApp::getParentId(quint64 id)
{
    EmbeddedWidgetBaseStructPrivate *str = m_widgetMap.value(id);

    if(str)
    {
        return str->parentId;
    }

    return 0;
}


void EmbeddedApp::setVisible(quint64 id, bool visibility)
{
    EmbeddedWidgetBaseStructPrivate *str = m_widgetMap.value(id);

    if(0 != str)
    {
        auto ptr = dynamic_cast<EmbeddedWidgetStruct *>(str->embWidgetStruct);

        if(ptr)
        {
            bool allowChange = true;
            ptr->iface->visibleChanged(visibility, &allowChange);

            if(!allowChange)
            {
                qDebug() << "EmbeddedApp::setVisible(): Deny change widget visisbility";
            }
        }

        if(str->embWidgetStruct->isModal)
        {
            if(visibility)
            {
                if(str->embWidgetStruct->isModalBlock)
                {
                    str->embWidgetPtr->setWindowFlags(str->embWidgetPtr->windowFlags() | Qt::WindowStaysOnTopHint);
                    str->embWidgetPtr->raise();
                    str->embWidgetPtr->exec();
                }
                else
                {
                    str->embWidgetPtr->setWindowFlags(str->embWidgetPtr->windowFlags() | Qt::WindowStaysOnTopHint);
                    str->embWidgetPtr->raise();
                    str->embWidgetPtr->show();
                }
            }
            else
            {
                str->embWidgetPtr->setVisible(visibility);
            }
        }
        else
        {
            bool needRaise = (visibility != str->embWidgetPtr->isVisible());

            if(str->embWidgetPtr->isCollapsed() && visibility)
            {
                str->embWidgetPtr->setCollapsed(false);
            }
            else
            {
                str->embWidgetPtr->setVisible(visibility);
            }

            if(needRaise)
            {
                str->embWidgetPtr->raise();
            }

            //str->embWidgetStruct->addHided = !visibility;
        }
    }
    else
    {
        qWarning() << __FUNCTION__ << " Can't set visisbility, widget not found";
    }

    ewApp()->embWidgetChanged(id);
}


bool EmbeddedApp::isVisible(quint64 id)
{
    auto ptr = getPrivateStruct(id);

    if(!ptr)
    {
        return false;
    }

    if(!ptr->embWidgetPtr)
    {
        return false;
    }

    bool vis = ptr->embWidgetPtr->isVisible();
    return vis;
}


QList<EmbeddedWidgetBaseStructPrivate *> EmbeddedApp::getPrivateStructByParent(quint64 parentId, ew::EmbeddedWidgetType tp)
{
    QList<EmbeddedWidgetBaseStructPrivate *> resList;
    QMapIterator<quint64, EmbeddedWidgetBaseStructPrivate *> servIter(m_widgetMap);

    while(servIter.hasNext())
    {
        servIter.next();

        if(parentId == servIter.value()->parentId && tp == servIter.value()->ewType)
        {
            resList.append(servIter.value());
        }
    }

    return resList;
}


QList<EmbeddedWidgetBaseStructPrivate *> EmbeddedApp::getPrivateStructByParent(quint64 parentId)
{
    QList<EmbeddedWidgetBaseStructPrivate *> resList;
    QMapIterator<quint64, EmbeddedWidgetBaseStructPrivate *> servIter(m_widgetMap);

    while(servIter.hasNext())
    {
        servIter.next();

        if(parentId == servIter.value()->parentId)
        {
            resList.append(servIter.value());
        }
    }

    return resList;
}


QList<EmbeddedWidget *> EmbeddedApp::getWidgetsByParent(quint64 parentId)
{
    QList<EmbeddedWidget *> resList;
    QMapIterator<quint64, EmbeddedWidgetBaseStructPrivate *> servIter(m_widgetMap);

    while(servIter.hasNext())
    {
        servIter.next();

        if(parentId == servIter.value()->parentId)
        {
            resList.append(servIter.value()->embWidgetPtr);
        }
    }

    return resList;
}


QList<EmbeddedWidgetBaseStructPrivate *> EmbeddedApp::getPrivateStructByType(EmbeddedWidgetType tp)
{
    QList<EmbeddedWidgetBaseStructPrivate *> resList;
    QMapIterator<quint64, EmbeddedWidgetBaseStructPrivate *> servIter(m_widgetMap);

    while(servIter.hasNext())
    {
        servIter.next();

        if(tp == servIter.value()->ewType)
        {
            resList.append(servIter.value());
        }
    }

    return resList;
}


QList<quint64> EmbeddedApp::getIdsByParent(quint64 parentId)
{
    QList<quint64> resList;
    QMapIterator<quint64, EmbeddedWidgetBaseStructPrivate *> servIter(m_widgetMap);

    while(servIter.hasNext())
    {
        servIter.next();

        if(parentId == servIter.value()->parentId)
        {
            resList.append(servIter.key());
        }
    }

    return resList;
}


void EmbeddedApp::checkChildWidgets(quint64 parentId)
{
    // return;
    if(0 == parentId)
    {
        return;
    }

    QList<EmbeddedWidgetBaseStructPrivate *> chList = getPrivateStructByParent(parentId);


    /*скрытие заголовков дочерних виджетов*/
    if(1 == chList.count())
    {
        auto privStruct = chList.first();

        if(privStruct->embWidgetStruct->headerVisible)
        {
            privStruct->embWidgetPtr->setHeaderVisisbilityPrivate(false);
            privStruct->embWidgetPtr->setResizeAllowed(false);
        }
    }
    else
    {
        foreach (EmbeddedWidgetBaseStructPrivate * privStruct, chList)
        {
            if(!privStruct->embWidgetStruct->headerVisible)
            {
                privStruct->embWidgetPtr->setHeaderVisisbility(false);
            }
        }
    }
}


/// FIXME: надо вставлять не по левому углу таскаемого виджета, а по координатам мышки
bool EmbeddedApp::checkGroupAction(quint64 id, EmbeddedHeader::MovingState ms)
{
    /// FIXME пока что считаем, что группы и виджеты все внешние(parent == 0)
    EmbeddedWidgetBaseStructPrivate *movedWidget = getPrivateStruct(id);

    if(!movedWidget)
    {
        return false;
    }

    if(EWT_WIDGET == movedWidget->type() || EWT_WINDOW == movedWidget->type())
    {
        // Начало движения. Если виджет в панели, то вытаскиваем его из нее
        if(ms == EmbeddedHeader::MS_MOVE && (0 != movedWidget->parentId) )
        {
            EmbeddedPanel *panel = dynamic_cast<EmbeddedPanel *>(movedWidget->embWidgetPtr->parent());

            if(panel)
            {
                /// если виджет может открепляться, то меняем родителя,
                /// если нет - открепляем виджет, но не меняем родителя:
                /// виджет по окончанию перемещения должен либо встроится
                /// на назначенное пользователем место, либо вернется на своё предыдущее
                /// место в группе
                if(movedWidget->embWidgetStruct->allowChangeParent)
                {
                    movedWidget->parentId = 0;
                }

                panel->removeEmbeddedWidget(movedWidget);
            }

            //if()
            //return true;
        }

        if(((0 != movedWidget->parentId) && movedWidget->embWidgetStruct->allowChangeParent))
        {
            return false;
        }

        EmbeddedWidgetBaseStructPrivate *group = nullptr;
        QList<EmbeddedWidgetBaseStructPrivate *> groups = getPrivateStructByType(EWT_GROUP);

        foreach (EmbeddedWidgetBaseStructPrivate * item, groups)
        {
            auto groupPtr = item->embWidgetPtr;
            groupPtr->m_pViewPanel->highlightWidgetTurnOff();
            QPoint pos = movedWidget->embWidgetPtr->mapToGlobal(QPoint(0, 0));
            QSize grSize = groupPtr->size();
            QRect r = QRect(groupPtr->mapToGlobal(QPoint(0, 0)),
                            groupPtr->mapToGlobal(QPoint(grSize.width(), grSize.height())));

            if(movedWidget->embWidgetStruct->allowChangeParent
               || groupPtr->id() == movedWidget->parentId)
            {
                if(r.contains(pos) )
                {
                    group = item;
                    break;
                }
            }
        }

        if(nullptr == group)
        {
            if(ms == EmbeddedHeader::MS_END && !movedWidget->embWidgetStruct->allowChangeParent)
            {
                auto prevParentStruct = dynamic_cast<EmbeddedGroupStructPrivate *>(getPrivateStruct(movedWidget->parentId));

                if(prevParentStruct)
                {
                    prevParentStruct->embWidgetPtr->m_pViewPanel->insertEmbeddedWidget(movedWidget, QPoint(0, 0));
                    EmbeddedGroupWidget *groupWdg = dynamic_cast<EmbeddedGroupWidget *>(prevParentStruct->embWidgetPtr);

                    if(groupWdg)
                    {
                        resizeGroup(prevParentStruct);
                    }
                }

                return true;
            }

            return false;
        }

        if(!group->embWidgetPtr->m_pViewPanel->canAdd(movedWidget->type()))
        {
            return false;
        }

        // Вычисление координат вставки


        QPoint groupPos = group->embWidgetPtr->m_pViewPanel->mapToGlobal(QPoint(0, 0));
        QPoint point = QPoint(movedWidget->embWidgetPtr->mapToGlobal(QPoint(0, 0)));
        int groupW = group->embWidgetPtr->m_pViewPanel->width();
        int groupH = group->embWidgetPtr->m_pViewPanel->height();


        if(!((groupPos.x() < point.x()) && ((groupPos.x() + groupW) > point.x())
             && (groupPos.y() < point.y()) && ((groupPos.y() + groupH) > point.y())))
        {
            return false;
        }

        point = group->embWidgetPtr->m_pViewPanel->mapFromGlobal(point);

        if(ms == EmbeddedHeader::MS_MOVE)
        {
            group->embWidgetPtr->m_pViewPanel->highlightWidget(point, movedWidget->embWidgetPtr->size());
            return true;
        }

        // Конец передвижения. Если левый верхний угол находится над панелью, то происходит встраивание
        if(ms == EmbeddedHeader::MS_END)
        {
            group->embWidgetPtr->m_pViewPanel->insertEmbeddedWidget(movedWidget, point);
            movedWidget->parentId = group->id;
            resizeGroup(group);
            return true;
        }
    }

    return false;
}


bool EmbeddedApp::checkGroupCreation(quint64 id, EmbeddedHeader::MovingState ms)
{
    EmbeddedWidgetBaseStructPrivate *movedWidget = getPrivateStruct(id);

    if(!movedWidget)
    {
        return false;
    }

    if(!movedWidget->embWidgetStruct->allowGrouping)
    {
        return false;
    }

    if((EWT_WIDGET == movedWidget->type() || EWT_WINDOW == movedWidget->type()) && 0 == movedWidget->parentId)
    {
        EmbeddedWidgetBaseStructPrivate *wdg = nullptr;
        QList<EmbeddedWidgetBaseStructPrivate *> widgets = getPrivateStructByType(movedWidget->type());

        foreach (EmbeddedWidgetBaseStructPrivate * item, widgets)
        {
            item->embWidgetPtr->setHighlightedSection(HS_NO_HIGHLIGHT);
            item->embWidgetPtr->update();
            QPoint pos = movedWidget->embWidgetPtr->pos();

            if(item != movedWidget && !item->embWidgetStruct->addHided && 0 == item->parentId &&
               item->embWidgetPtr->geometry().contains(pos))
            {
                wdg = item;
            }
        }

        if(nullptr == wdg)
        {
            return false;
        }

        if(ms == EmbeddedHeader::MS_MOVE)
        {
            QPoint shift(movedWidget->embWidgetPtr->x() - wdg->embWidgetPtr->x(),
                         movedWidget->embWidgetPtr->y() - wdg->embWidgetPtr->y());
            HighlightSection hsWdg = wdg->embWidgetPtr->getCardinalDirection(shift);
            wdg->embWidgetPtr->setHighlightedSection(hsWdg);
            wdg->embWidgetPtr->update();

            switch(hsWdg)
            {
              case HS_TOP:
                  hsWdg = HS_BOTTOM;
                  break;
              case HS_BOTTOM:
                  hsWdg = HS_TOP;
                  break;

              case HS_LEFT:
                  hsWdg = HS_RIGHT;
                  break;

              case HS_RIGHT:
                  hsWdg = HS_LEFT;
                  break;
              default:
                  hsWdg = HS_NO_HIGHLIGHT;
                  break;
            }

            movedWidget->embWidgetPtr->setHighlightedSection(hsWdg);
            movedWidget->embWidgetPtr->update();
        }

        if(ms == EmbeddedHeader::MS_END)
        {
            EmbeddedWidgetBaseStructPrivate *group = nullptr;
            {
                ew::EmbeddedGroupStruct groupStr;

                // FIXME Какое имя задавать?
                groupStr.header.tooltip = "Group";
                groupStr.header.windowTitle = "Group";
                groupStr.header.hasCloseButton = true;
                groupStr.header.hasCollapseButton = true;
                groupStr.header.hasLockButton = true;
                groupStr.size = QSize(300, 300);
                groupStr.header.hasMinMaxButton = true;
                groupStr.header.hasLockButton = true;
                groupStr.header.hasWhatButton = true;
                groupStr.header.hasHideHeaderButton = true;
                groupStr.header.hasPinButton = true;
                quint64 groupId = ewApp()->createGroup(groupStr, (quint64)0);
                group = getPrivateStruct(groupId);
            }

            if(nullptr == group)
            {
                return false;
            }

            group->embWidgetPtr->move(wdg->embWidgetPtr->pos());
            group->embWidgetStruct->alignPoint = wdg->embWidgetPtr->pos();

            QPoint shift(movedWidget->embWidgetPtr->x() - wdg->embWidgetPtr->x(),
                         movedWidget->embWidgetPtr->y() - wdg->embWidgetPtr->y());

            group->embWidgetPtr->m_pViewPanel->insertEmbeddedWidget(wdg, QPoint(0, 0));
            wdg->parentId = group->id;
            resizeGroup(group);

            group->embWidgetPtr->m_pViewPanel->insertEmbeddedWidget(movedWidget, shift);
            movedWidget->parentId = group->id;
            resizeGroup(group);
        }
    }

    return false;
}


void EmbeddedApp::resizeGroup(EmbeddedWidgetBaseStructPrivate *group)
{
    // TODO krotov Нужно как то обдумать даный функционал. Убрали для инновации.
//    int h = group->embWidgetPtr->height() - group->embWidgetPtr->m_pViewPanel->height();

//    group->embWidgetPtr->m_pViewPanel->autoResize();
//    h += group->embWidgetPtr->m_pViewPanel->height();
//    /// FIXME какие +12? Нужно взять ширину рамки. Не знаю где.
//    group->embWidgetPtr->resize(group->embWidgetPtr->m_pViewPanel->width() + 12, h);
//    group->embWidgetStruct->size = group->embWidgetPtr->size();
//    group->embWidgetPtr->m_pViewPanel->autoResize();
//    QSize sz = group->embWidgetPtr->m_pViewPanel->size();

//    /// TODO: проверить.
//    QMargins marg = group->embWidgetPtr->getEmbeddedViewMargins();
//    ewApp()->changeWidgetAlignGeometry(group->embWidgetPtr->id(), group->embWidgetStruct->alignPoint
//                                       , QSize(sz.width() + marg.left() + marg.right()
//                                               , sz.height() + marg.top() + marg.bottom()));
}


void EmbeddedApp::embWidgetChanged(quint64 wId, bool forceSave)
{
    if(!m_denySave)
    {
        // добавляем виджет в кеш
//        auto priv = getPrivateStruct(wId);

//        if(priv)
//        {
//            if(!m_loadedMap.contains(priv->embWidgetStruct->widgetTag))
//            {
//                if(!priv->embWidgetStruct->widgetTag.isEmpty())
//                {
//                    EmbeddedSerializationData data;
//                    data.data = priv->embWidgetStruct;
//                    data.id = priv->id;
//                    data.parent = priv->parentId;
//                    data.tp = priv->ewType;
//                    m_loadedMap.insert(data.data->widgetTag, data);
//                    m_loadedMapIds.append(data.id);
//                }
//            }
//        }

        //auto ptr = getPrivateStruct(wId)->embWidgetStruct;
        EmbeddedSessionManager::instance()->saveWidget(wId, forceSave);
    }
}


QVariant EmbeddedApp::getWidgetProperty(quint64 wId, QString propName)
{
    return m_propertyMap.value(wId).value(propName);
}


bool EmbeddedApp::setWidgetProperty(quint64 wId, QString propName, QVariant propValue)
{
    if(!m_denySave)
    {
        // добавляем виджет в кеш
        auto priv = getPrivateStruct(wId);

        if(0 == wId || priv)
        {
            auto map = m_propertyMap.value(wId);
            map.insert(propName, propValue);
            m_propertyMap.insert(wId, map);

            EmbeddedSessionManager::instance()->saveProperty(wId, propName, propValue);
            return true;
        }
    }

    return false;
}


void EmbeddedApp::adjustWidgetSize(quint64 widgetId)
{
    auto ptr = getPrivateStruct(widgetId);

    if(ptr)
    {
        auto widgetPtr = ptr->embWidgetPtr;

        if(widgetPtr)
        {
            switch(widgetPtr->widgetType())
            {
              case ew::EWT_WIDGET:
              {
                  QWidget *wdg = widgetPtr->getWidget();
                  QSize minSz = wdg->minimumSizeHint();
                  QSize sz = EmbeddedUtils::getWidgetSizeHint(wdg);

                  sz.setWidth((sz.width() > minSz.width()) ? sz.width() : minSz.width());
                  sz.setHeight((sz.height() > minSz.height()) ? sz.height() : minSz.height());

                  QMargins marg = widgetPtr->getEmbeddedViewMargins();
                  QSize recalced = EmbeddedUtils::getEmbeddedSize(sz, marg);
                  changeWidgetGeometry(widgetId, ptr->embWidgetStruct->alignPoint, recalced);
              } break;
              default:
                  break;
            }
        }
    }
}


QString
EmbeddedApp::getExistingDirectory(  quint64 parentId, const QString & caption, const QString & dir, QFileDialog::Options options)
{
    QFileDialog *pFileDlg = new QFileDialog(0, caption, dir);

    pFileDlg->setOptions(options | QFileDialog::DontUseNativeDialog);
    pFileDlg->setFileMode(QFileDialog::DirectoryOnly);

    auto iface = new EmbeddedFace(pFileDlg);
    ew::EmbeddedWidgetStruct str;
    str.header.windowTitle = caption;
    str.header.hasCloseButton = true;
    str.widgetTag = "";
    str.addHided = false;
    str.topOnHint = true;
    str.isModal = true;
    str.iface = iface;
    str.autoAdjustSize = true;

    quint64 id = ewApp()->createWidget(str, parentId);
    QStringList lst = pFileDlg->selectedFiles();
    ewApp()->removeWidget(id);


    if(!lst.isEmpty() && QDialog::Rejected != pFileDlg->result())
    {
        delete iface;
        delete pFileDlg;

        return lst.first();
    }

    delete iface;
    delete pFileDlg;

    return QString();
}


QString
EmbeddedApp::getOpenFileName(  quint64 parentId, const QString & caption, const QString & dir, const QString & filter, QString *selectedFilter, QFileDialog::Options options)
{
    QFileDialog *pFileDlg = new QFileDialog(0, caption, dir, filter);

    pFileDlg->setOptions(options | QFileDialog::DontUseNativeDialog);

    if(selectedFilter)
    {
        pFileDlg->selectNameFilter(*selectedFilter);
    }

    auto iface = new EmbeddedFace(pFileDlg);
    ew::EmbeddedWidgetStruct str;
    str.header.windowTitle = caption;
    str.header.hasCloseButton = true;
    str.widgetTag = "";
    str.addHided = false;
    str.topOnHint = true;
    str.isModal = true;
    str.iface = iface;
    str.autoAdjustSize = true;

    quint64 id = ewApp()->createWidget(str, parentId);
    QStringList lst = pFileDlg->selectedFiles();
    ewApp()->removeWidget(id);


    if(!lst.isEmpty() && QDialog::Rejected != pFileDlg->result())
    {
        delete iface;
        delete pFileDlg;
        return lst.first();
    }

    delete iface;
    delete pFileDlg;

    return QString();
}


QStringList
EmbeddedApp::getOpenFileNames(  quint64 parentId, const QString & caption, const QString & dir, const QString & filter, QString *selectedFilter, QFileDialog::Options options)
{
    QFileDialog *pFileDlg = new QFileDialog(0, caption, dir, filter);

    pFileDlg->setOptions(options | QFileDialog::DontUseNativeDialog);

    if(selectedFilter)
    {
        pFileDlg->selectNameFilter(*selectedFilter);
    }

    auto iface = new EmbeddedFace(pFileDlg);
    ew::EmbeddedWidgetStruct str;
    str.header.windowTitle = caption;
    str.header.hasCloseButton = true;
    str.widgetTag = "";
    str.addHided = false;
    str.topOnHint = true;
    str.isModal = true;
    str.iface = iface;
    str.autoAdjustSize = true;

    quint64 id = ewApp()->createWidget(str, parentId);
    QStringList lst = pFileDlg->selectedFiles();
    ewApp()->removeWidget(id);


    if(!lst.isEmpty() && QDialog::Rejected != pFileDlg->result())
    {
        delete iface;
        delete pFileDlg;

        return lst;
    }

    delete iface;
    delete pFileDlg;

    return lst;
}


QString
EmbeddedApp::getSaveFileName(  quint64 parentId, const QString & caption, const QString & dir, const QString & filter, QString *selectedFilter, QFileDialog::Options options)
{
    QFileDialog *pFileDlg = new QFileDialog(0, caption, dir, filter);

    pFileDlg->setOptions(options | QFileDialog::DontUseNativeDialog);
    pFileDlg->setAcceptMode(QFileDialog::AcceptSave);

    if(selectedFilter)
    {
        pFileDlg->selectNameFilter(*selectedFilter);
    }

    auto iface = new EmbeddedFace(pFileDlg);
    ew::EmbeddedWidgetStruct str;
    str.header.windowTitle = caption;
    str.header.hasCloseButton = true;
    str.widgetTag = "";
    str.addHided = false;
    str.topOnHint = true;
    str.isModal = true;
    str.iface = iface;
    str.autoAdjustSize = true;

    quint64 id = ewApp()->createWidget(str, parentId);
    QStringList lst = pFileDlg->selectedFiles();
    ewApp()->removeWidget(id);


    if(!lst.isEmpty() && QDialog::Rejected != pFileDlg->result())
    {
        delete iface;
        delete pFileDlg;

        return lst.first();
    }

    delete iface;
    delete pFileDlg;

    return QString();
}


quint64 EmbeddedApp::getTopWidgetId(QWidget *wdg)
{
    QWidget *parentWdg = wdg;

    auto ptr = dynamic_cast<EmbeddedWidget *>(parentWdg);
    quint64 id = 0;

    if(ptr)
    {
        id = ptr->id();
    }

    while(nullptr != parentWdg->parentWidget())
    {
        parentWdg = parentWdg->parentWidget();
        ptr = dynamic_cast<EmbeddedWidget *>(parentWdg);

        if(ptr)
        {
            id = ptr->id();
        }
    }

    return id;
}


void EmbeddedApp::setVisibleDocViewer(bool vis)
{
    DocDebugger::instance()->setVisibleDocViewer(vis);
}


EmbeddedApp::EmbeddedInitStruct::EmbeddedInitStruct(const EmbeddedApp::EmbeddedInitStruct &other)
{
    serializerType       = other.serializerType;
    serializerConfigBase = other.serializerConfigBase;
    workMode             = other.workMode;
    debugPort            = other.debugPort;
    docPath              = other.docPath;
    defaultStylesheet    = other.defaultStylesheet;
    //            logUserActions       = other.logUserActions      ;
    loggerFuncDebug      = other.loggerFuncDebug;
    loggerFuncWarning    = other.loggerFuncWarning;
    loggerFuncFatal      = other.loggerFuncFatal;
    loggerFuncUserActions = other.loggerFuncUserActions;
}


EmbeddedApp::EmbeddedInitStruct::EmbeddedInitStruct()
    : serializerType       ( ew::ST_SQLITE )
      , serializerConfigBase ( nullptr )
      , workMode             ( ew::network::EmbeddedNetworkManager::WM_SERVER )
      , debugPort            ( 20342 )
      , docPath              ( "./doc" )
      //            , logUserActions       ( true )
      , loggerFuncDebug      ( nullptr )
      , loggerFuncWarning    ( nullptr )
      , loggerFuncFatal      ( nullptr )
      , loggerFuncUserActions( nullptr )
{
}


quint64 EmbeddedApp::getEmbeddedWidgetByChildWidget(QWidget *wdg)
{
    QWidget *focusWdg = wdg /*QApplication::focusWidget()*/;
    QWidget *focusWdgParent = focusWdg;

    while(0 != focusWdgParent)
    {
        ew::EmbeddedWidget *embWidg = dynamic_cast<ew::EmbeddedWidget *>(focusWdgParent);

        if(nullptr != embWidg)
        {
            quint64 wId = embWidg->id();
            return wId;
        }
        else
        {
            if(nullptr != focusWdgParent)
            {
                focusWdgParent = focusWdgParent->parentWidget();
                continue;
            }
            else
            {
                return 0;
            }
        }
    }

    return 0;
}


bool EmbeddedApp::insertWidgetInGroup(quint64 groupId, quint64 moduleId, QPoint localPos)
{
    auto _parentPrivPtr = getPrivateStruct(groupId);
    auto _privPtr = getPrivateStruct(moduleId);

    if(!_privPtr)
    {
        qWarning() << "EmbeddedApp::insertWidgetInGroup(): Error! item is not found!";
        return false;
    }

    if(!_parentPrivPtr )
    {
        qWarning() << "EmbeddedApp::insertWidgetInGroup(): Error! Parent item is not found!";
        return false;
    }

    EmbeddedGroupWidget *groupPtr = dynamic_cast<EmbeddedGroupWidget *>(_parentPrivPtr->embWidgetPtr);

    if(EWT_GROUP != _parentPrivPtr->type() || nullptr == groupPtr)
    {
        qWarning() << "EmbeddedApp::insertWidgetInGroup(): Error! Founded item is NOT GROUP!";
        return false;
    }

    groupPtr->addWidget(_privPtr, localPos);

    return true;
}


QRectF EmbeddedApp::getWidgetRect(quint64 id)
{
    auto ptr = getPrivateStruct(id);

    if(!ptr)
    {
        return QRectF();
    }

    auto wdgPtr = ptr->embWidgetPtr;

    QRectF r = wdgPtr->rect();
    return r;
}
