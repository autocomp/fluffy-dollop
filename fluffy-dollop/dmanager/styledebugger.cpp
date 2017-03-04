#include "embeddednetworkmanager.h"
#include "styledebugger.h"
#include <QAction>
#include <QApplication>
#include <QBuffer>
#include <QEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QTimer>
#include "embeddedapp.h"

using namespace ew::network;

StyleDebugger::StyleDebugger(QObject *parent) : QObject(parent)
{
    m_styleDebugString = " | " + tr("Style debug mode - select widget");
    m_activated = false;
    m_pSelectModeActive = false;
    m_pCheckCoordsTimer = new QTimer();
    m_pCheckCoordsTimer->setInterval(20);
    connect(m_pCheckCoordsTimer, SIGNAL(timeout()), this, SLOT(slotCheckMouseCoords()));
}


StyleDebugger *StyleDebugger::instance()
{
    static StyleDebugger stDebug;

    return &stDebug;
}


void StyleDebugger::loadWidgetStylesheet(QString widgetFullName)
{
    StyleMsgWidgetStylesheet msg;

    msg.widgetPath = widgetFullName;
    msg.widgetStylesheet = "";


    EmbeddedNetworkManager::instance()->sendMessage(ew::network::MT_STYLE_MESSAGE
                                                    , ew::network::SMT_GET_STYLESHEET
                                                    , StyleMsgWidgetStylesheet::serialize(msg));
}


void StyleDebugger::loadWidgetScreenshot(QString widgetFullName)
{
    BaseMsgGetWidgetScreenShot msg;

    msg.widgetPath = widgetFullName;


    EmbeddedNetworkManager::instance()->sendMessage(ew::network::MT_STYLE_MESSAGE
                                                    , ew::network::SMT_GET_WIDGET_SCREENSHOT
                                                    , BaseMsgGetWidgetScreenShot::serialize(msg));
}


void StyleDebugger::setWidgetStylesheet(QString widgetFullName, QString widgetStylesheet)
{
    StyleMsgWidgetStylesheet msg;

    msg.widgetPath = widgetFullName;
    msg.widgetStylesheet = widgetStylesheet;


    EmbeddedNetworkManager::instance()->sendMessage(ew::network::MT_STYLE_MESSAGE
                                                    , ew::network::SMT_SET_STYLESHEET
                                                    , StyleMsgWidgetStylesheet::serialize(msg));
}


void StyleDebugger::setSelectionMode(bool isEnable)
{
    Q_UNUSED(isEnable);
    EmbeddedNetworkManager::instance()->sendMessage(ew::network::MT_STYLE_MESSAGE
                                                    , ew::network::SMT_GET_CLICKED_WIDGET
                                                    , QByteArray());
}


void StyleDebugger::loadFullWidgetTree()
{
    EmbeddedNetworkManager::instance()->sendMessage(ew::network::MT_STYLE_MESSAGE
                                                    , ew::network::SMT_GET_FULL_WIDGET_TREE
                                                    , QByteArray());
}


QString StyleDebugger::getNodeFullName(const StyleTreeNode &parentNode, const QList<StyleTreeNode> &lst)
{
    QString fullName = parentNode.className + "_" + parentNode.objectName;
    StyleTreeNode curNode = parentNode;

    while(0 != curNode.parentId)
    {
        foreach (auto tmpNode, lst)
        {
            if( curNode.parentId == tmpNode.id)
            {
                curNode = tmpNode;
                fullName.prepend(curNode.className + "_" + curNode.objectName + getNameSeparator());
                break;
            }
        }
    }

    return fullName;
}


bool StyleDebugger::getNodeByFullName(QString fullName, const QList<StyleTreeNode> &lst, StyleTreeNode & foundedNode)
{
    QStringList nameList = fullName.split(getNameSeparator());

    QListIterator<QString> listIter(nameList);

    quint64 parentId = 0;

    QString name;

    while(listIter.hasNext())
    {
        QString str = listIter.next();

        foreach (auto tmpNode, lst)
        {
            QString tmpName = tmpNode.className + "_" + tmpNode.objectName;

            if(tmpName == str && parentId == tmpNode.parentId)
            {
                parentId = tmpNode.id;
                if(name.isEmpty())
                {
                    name = tmpName;
                }
                else
                {
                    name.append(getNameSeparator() + tmpName );
                }

                foundedNode = tmpNode;

                break;
            }
        }

        if(name.isEmpty())
        {
            return false;
        }
    }

    return true;
}


void StyleDebugger::resetAllStyles()
{
    EmbeddedNetworkManager::instance()->sendMessage(ew::network::MT_STYLE_MESSAGE
                                                    , ew::network::SMT_RESET_STYLES
                                                    , QByteArray());
}


void StyleDebugger::resetStylesRecursive(QWidget *wdg)
{
    if(!wdg->styleSheet().isEmpty())
    {
        wdg->setStyleSheet("");
    }

    auto objList = wdg->children();
    QWidgetList wList;
    foreach (auto obj, objList)
    {
        auto wdgTmp = dynamic_cast<QWidget *>(obj);

        if(wdgTmp)
        {
            wList.append(wdgTmp);
        }
    }

    foreach (auto wdg, wList)
    {
        resetStylesRecursive(wdg);
    }
}


void StyleDebugger::resetStyles()
{
    auto wList = qApp->topLevelWidgets();
    foreach (auto wdg, wList)
    {
        resetStylesRecursive(wdg);
    }
}


void StyleDebugger::setSelectModeEnabled(bool enabled)
{

    if(m_pSelectModeActive == enabled)
    {
        return;
    }

    quint64 id = ewApp()->getMainWindowId();
    auto ptr = ewApp()->getWidgetStructById(id);

    if(enabled)
    {
        if(ptr)
        {
            m_savedTitle = ptr->header.windowTitle;
            ewApp()->setWidgetTitle(id, m_savedTitle + m_styleDebugString);
        }
        qApp->installEventFilter(this);
        m_pCheckCoordsTimer->start();
    }
    else
    {
        if(ptr)
        {
            QString tmp = ewApp()->getWidgetTitle(id);

            if(tmp.contains(m_styleDebugString))
            {
                ewApp()->setWidgetTitle(id, m_savedTitle);
            }
        }
        qApp->removeEventFilter(this);
        m_pCheckCoordsTimer->stop();
    }

    m_pSelectModeActive = enabled;
}


QWidget *StyleDebugger::getWidgetByFullName(QString fullWidgetName)
{
    QStringList lst = fullWidgetName.split(getNameSeparator());
    QWidgetList wList = qApp->topLevelWidgets();

    QListIterator<QWidget *> listIter(wList);

    while(listIter.hasNext())
    {
        QWidget *wdg = listIter.next();

        if(getWidgetName(wdg) == lst.first())
        {
            if(1 == lst.size())
            {
                return wdg;
            }
            else
            {
                lst.removeFirst();
                auto objList = wdg->children();
                wList.clear();
                foreach (auto obj, objList)
                {
                    auto wdgTmp = dynamic_cast<QWidget *>(obj);

                    if(wdgTmp)
                    {
                        wList.append(wdgTmp);
                    }
                }

                listIter = QListIterator<QWidget *>(wList);
            }
        }
    }

    return 0;
}


QString StyleDebugger::getStyleSheetByFullName(QString fullWidgetName)
{
    QWidget *wdg = getWidgetByFullName(fullWidgetName);

    if(wdg)
    {
        return wdg->styleSheet();
    }

    return QString("");
}


void StyleDebugger::setStyleSheetByFullName(QString fullWidgetName, QString stylesheet)
{
    QWidget *wdg = getWidgetByFullName(fullWidgetName);

    if(wdg)
    {
        return wdg->setStyleSheet(stylesheet);
    }
}


QByteArray StyleDebugger::getScreenShotByFullName(QString fullWidgetName)
{
    QWidget *wdg = getWidgetByFullName(fullWidgetName);

    if(nullptr == wdg)
    {
        return QByteArray();
    }

    QPixmap pm(wdg->size());

    wdg->render(&pm);

    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);
    /*bool res = */pm.save(&buffer, "PNG"); // writes pixmap into bytes in PNG format
    bytes = buffer.data();

    return bytes;
}


QString StyleDebugger::getNameSeparator()
{
    return QString("|");
}


QString StyleDebugger::getWidgetName(QWidget *wdg)
{
    return QString(wdg->metaObject()->className()) + "_" + wdg->objectName();
}


QString StyleDebugger::getWidgetFullName(QWidget *wdg)
{
    QString clickedWidget = getWidgetName(wdg) + getNameSeparator();
    QWidget *parentWdg = wdg->parentWidget();

    while(parentWdg)
    {
        clickedWidget.prepend( QString(parentWdg->metaObject()->className()) + "_" + parentWdg->objectName() + getNameSeparator() );
        parentWdg = parentWdg->parentWidget();
    }

    return clickedWidget;
}



void StyleDebugger::fillNode(QWidget *wdg, quint64 &id
                             , QMap<quint64, QWidget *> & widgetMap, QMap<quint64, ew::network::StyleTreeNode> & nodeMap)
{
    ew::network::StyleTreeNode node;
    node.objectName = wdg->objectName();
    node.className = wdg->metaObject()->className();
    node.id = id;
    node.hasStylesheet = !wdg->styleSheet().isEmpty();
    QWidget *parentWdg = wdg->parentWidget();

    if(parentWdg)
    {
        node.parentId = widgetMap.key(parentWdg);
    }
    else
    {
        node.parentId = 0;
    }

    if(0 != dynamic_cast<QPushButton *>(wdg) )
    {
        QPushButton *btn = dynamic_cast<QPushButton *>(wdg);


        node.title = btn->text();

        if(!btn->icon().isNull())
        {
            QByteArray bytes;
            QBuffer buffer(&bytes);
            buffer.open(QIODevice::ReadWrite);
            /*bool res = */btn->icon().pixmap(32, 32).save(&buffer, "PNG"); // writes pixmap into bytes in PNG format
            node.icon = buffer.data();
        }
    }
    else if(0 != dynamic_cast<QLabel *>(wdg) )
    {
        QLabel *lbl = dynamic_cast<QLabel *>(wdg);
        node.title = lbl->text();
    }
    else
    {
        node.title = wdg->windowTitle();
    }

    widgetMap.insert(id, wdg);
    nodeMap.insert(id, node);
    id++;

    QObjectList lst = wdg->children();
    foreach (QObject * obj, lst)
    {
        QWidget *chWidget = dynamic_cast<QWidget *>(obj);

        if(chWidget)
        {
            fillNode(chWidget, id, widgetMap, nodeMap);
        }
    }
}


void StyleDebugger::setReceivedMessage(StyleMessageType mt, QByteArray arr)
{
    switch(mt)
    {
      case SMT_GET_FULL_WIDGET_TREE:
    {
        StyleMsgGetWidgetTree msg = StyleMsgGetWidgetTree::deserialize(arr);
        emit signalTreeUpdated(msg.rootNodes);
    } break; //StyleMsgGetWidgetTree
//      case SMT_GET_CLICKED_WIDGET_TREE:
//    {
//        BaseMsgGetClickedWidget msg = BaseMsgGetClickedWidget::deserialize(arr);
//        emit signalWidgetSelected(msg.widgetPath);
//    }break;
      case SMT_GET_WIDGET_SCREENSHOT:
      {
          BaseMsgGetWidgetScreenShot msg = BaseMsgGetWidgetScreenShot::deserialize(arr);
          QPixmap pm;
          pm.loadFromData(msg.screenData, "PNG");
          emit signalWidgetScreenShot(msg.widgetPath, pm);
      } break;//BaseMsgGetWidgetScreenShot
      case SMT_GET_STYLESHEET:
      {
          StyleMsgWidgetStylesheet msg = StyleMsgWidgetStylesheet::deserialize(arr);
          emit signalWidgetStylesheet(msg.widgetPath, msg.widgetStylesheet);
      } break;//StyleMsgWidgetStylesheet

      case SMT_GET_CLICKED_WIDGET:
    {
        BaseMsgGetClickedWidget msg = BaseMsgGetClickedWidget::deserialize(arr);
        emit signalWidgetSelected(msg.widgetPath);
    }break;
      case SMT_SET_STYLESHEET:
      case SMT_RESET_STYLES:
      //case SMT_ENABLE_STYLE_MODE:
      case SMT_NULL_MESSAGE:
      default:
          break;
    }
}


QList<ew::network::StyleTreeNode> StyleDebugger::getWidgetTree(QWidget *selectedWidget)
{
    QMap<quint64, QWidget *> widgetMap;
    QMap<quint64, ew::network::StyleTreeNode> nodeMap;

    quint64 i = 1;
    QWidgetList wdgList;

    if(selectedWidget)
    {
        wdgList.append(selectedWidget);
    }
    else
    {
        wdgList = qApp->topLevelWidgets();
    }

    foreach (QWidget * wdg, wdgList)
    {
        if(0 == widgetMap.key(wdg, 0))
        {
            ew::network::StyleTreeNode node;
            fillNode(wdg, i, widgetMap, nodeMap);
        }
    }

    return nodeMap.values();
}


//QList<ew::network::StyleTreeNode> StyleDebugger::getClickedWidgetTree()
//{
//    QMap<quint64, QWidget *> widgetMap;
//    QMap<quint64, ew::network::StyleTreeNode> nodeMap;
//    quint64 i = 1;
//    QWidgetList wdgList = qApp->topLevelWidgets();

//    foreach (QWidget * wdg, wdgList)
//    {
//        if(0 == widgetMap.key(wdg, 0))
//        {
//            ew::network::StyleTreeNode node;
//            fillNode(wdg, i, widgetMap, nodeMap);
//        }
//    }

//    return nodeMap.values();


//}


void StyleDebugger::slotCheckMouseCoords()
{
    QWidget *wdg = QApplication::widgetAt(QCursor::pos());

    if(0 != wdg)
    {
        QApplication::setActiveWindow(wdg);
        wdg->activateWindow();
    }
}


bool StyleDebugger::eventFilter(QObject *obj, QEvent *ev)
{
    QWidget *sendWidget = dynamic_cast<QWidget *>(obj);

    if(0 == sendWidget)
    {
        return false;
    }

    switch(ev->type())
    {
      case QEvent::MouseButtonPress:
      {
          if(m_pSelectModeActive)
          {
              //Qt::KeyboardModifiers mod = QApplication::keyboardModifiers();

              if(0 != (QApplication::keyboardModifiers() & Qt::ControlModifier))
              {
                  QWidget *par = sendWidget;

                  while(0 != par)
                  {
                      sendWidget = par;
                      par = sendWidget->parentWidget();
                  }

                  setSelectModeEnabled(false);
              }
              else if(0 != (QApplication::keyboardModifiers() & Qt::ShiftModifier))
              {
                  setSelectModeEnabled(false);
              }
              else if(0 == (QApplication::keyboardModifiers() ))
              {
                  setSelectModeEnabled(false);
                  QString str = getWidgetFullName(sendWidget);

                  emit signalWidgetClicked(str);
              }
          }
      } break;
      case QEvent::MouseButtonRelease:
      {
      } break;

      default:
      {
      } break;
    }


    ev->ignore();
    return false;
}


QByteArray StyleDebugger::getStyleMsgAnswer(StyleMessageType mt
                                            , QByteArray & arr, bool & asyncAnswer)
{
    StyleMessageType stmt = mt;

    switch(stmt)
    {/*
      case SMT_ENABLE_STYLE_MODE:
      {
          StyleDebugger::instance()->setEnabled(true);
          asyncAnswer = false;
      } break;*/
      case SMT_GET_FULL_WIDGET_TREE: //StyleMsgGetWidgetTree
      {
          auto list = StyleDebugger::instance()->getWidgetTree();

          StyleMsgGetWidgetTree msg;
          msg.rootNodes = list;
          QByteArray resArr = StyleMsgGetWidgetTree::serialize(msg);
          asyncAnswer = false;
          return resArr;
      } break;
//      case SMT_GET_CLICKED_WIDGET_TREE: //StyleMsgGetWidgetTree
//      {
//          StyleDebugger::instance()->setSelectModeEnabled(true);
//          asyncAnswer = true;
//          return QByteArray();

//          /*ждем срабатывания slotNodeTreeSelected*/
//      } break;
      case SMT_GET_CLICKED_WIDGET://StyleMsgGetClickedWidget
      {
          StyleDebugger::instance()->setSelectModeEnabled(true);
          asyncAnswer = true;
          return QByteArray();
      } break;
      case SMT_GET_WIDGET_SCREENSHOT://StyleMsgGetWidgetScreenShot
      {
          BaseMsgGetWidgetScreenShot msg = BaseMsgGetWidgetScreenShot::deserialize(arr);
          msg.screenData = getScreenShotByFullName(msg.widgetPath);


          QByteArray resArr = BaseMsgGetWidgetScreenShot::serialize(msg);

          asyncAnswer = false;
          return resArr;
      } break;
      case SMT_GET_STYLESHEET://StyleMsgWidgetStylesheet
      {
          StyleMsgWidgetStylesheet msg = StyleMsgWidgetStylesheet::deserialize(arr);
          QWidget *wdg = getWidgetByFullName(msg.widgetPath);

          if(wdg)
          {
              msg.widgetStylesheet = wdg->styleSheet();
          }

          QByteArray resArr = StyleMsgWidgetStylesheet::serialize(msg);

          asyncAnswer = false;
          return resArr;
      } break;
      case SMT_SET_STYLESHEET://StyleMsgWidgetStylesheet
      {
          StyleMsgWidgetStylesheet msg = StyleMsgWidgetStylesheet::deserialize(arr);
          asyncAnswer = false;
          QWidget *wdg = getWidgetByFullName(msg.widgetPath);
          if(wdg)
          {
              wdg->setStyleSheet(msg.widgetStylesheet);
          }
          return QByteArray();
      } break;
      case SMT_RESET_STYLES://null data
      {
          StyleDebugger::instance()->resetStyles();
          asyncAnswer = false;
      } break;
      case SMT_NULL_MESSAGE:
      default:
      {
      } break;
    }

    return QByteArray();
}
