#include "docdebugger.h"

#include "dataserializer.h"
#include "docviewerwidget.h"
#include "embeddedapp.h"
#include "embeddednetworkmanager.h"
#include "embeddedutils.h"
#include "embeddedwidget.h"
#include <QAction>
#include <QApplication>
#include <QApplication>
#include <QBuffer>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QShortcut>
#include <QTextBlock>
#include <QTextCursor>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextDocumentFragment>
#include <QTime>
#include <QTimer>
#include <QToolTip>
#include <QUrl>
#include <QWhatsThisClickedEvent>

#ifndef QT_V5
#include <QWebElement>
#include <QWebFrame>
#include <QWebPage>
#else
#include <QtWebEngineWidgets/QWebEnginePage>
#include <QtWebEngineWidgets/QWebEngineView>
#endif
using namespace ew::network;

DocDebugger::DocDebugger(QObject *parent) : QObject(parent)
{
    qApp->installEventFilter(this);
    m_docRootPath = "";
    m_doxDebugString = " | " + tr("Doc debug mode - select widget");
    m_activated = false;
    m_pSelectModeActive = false;
    m_pCheckCoordsTimer = new QTimer();
    m_pCheckCoordsTimer->setInterval(20);
    connect(m_pCheckCoordsTimer, SIGNAL(timeout()), this, SLOT(slotCheckMouseCoords()));
    connect(this, SIGNAL(signalHighlightWidget(QString)), this, SLOT(slotHighlightWidgets(QString)), Qt::QueuedConnection);
}


DocDebugger *DocDebugger::instance()
{
    static DocDebugger stDebug;


    return &stDebug;
}


void DocDebugger::init(QString docRootPath)
{
    if(!QDir(docRootPath).exists())
    {
        bool res = QDir("").mkpath(docRootPath);

        if(!res)
        {
            qWarning() << "DocDebugger::init: can't create doc root path. Doc dir: " << docRootPath;
        }
    }

    m_docRootPath = QDir(docRootPath).absolutePath();

    loadFullDoc();
}


QString DocDebugger::getDocRootPath()
{
    return m_docRootPath;
}


void DocDebugger::loadFullDoc()
{
    QDir rootDir(m_docRootPath);


    if(!rootDir.exists())
    {
        qWarning() << "DocDebugger::init: can't load dox, root doc dir not exist. Doc dir: " << m_docRootPath;
    }
    else
    {
        isEmptyNode(rootDir.path());
    }

    QStringList lst = getRelFSDocDirPath();

    foreach (QString relPath, lst)
    {
        installDoc(relPath);
    }
}


bool DocDebugger::isEmptyNode(QString absolutePath)
{
    QFileInfoList fil = QDir(absolutePath).entryInfoList(/*QDir::AllEntries |*/ QDir::NoDotAndDotDot | QDir::NoSymLinks); //TODO: проверить
    bool isHelp, isIncludedFolder, isPic;

    isPic = isHelp = isIncludedFolder = false;



    auto getFileExtension = [](QString fileName) -> QString {
                                return fileName.right(fileName.length() - fileName.lastIndexOf('.') - 1);
                            };

    foreach(QFileInfo info, fil)
    {
        if(info.isFile())

        {
            QString ext = getFileExtension(info.fileName());

            if(ext == QString("hdr"))
            {
                QFile headerFile(info.absoluteFilePath());

                if(headerFile.open(QIODevice::ReadOnly))
                {
                    if(!QString::fromLatin1(headerFile.readAll()).replace(QRegExp("[\n\t ]"), "").isEmpty())
                    {
                        isHelp = true;  ///
                    }
                }
                else
                {
                    qWarning() << info.absoluteFilePath() << tr("Header file open error");
                }
            }
            else if("html")
            {
                QFile htmlFile(info.absoluteFilePath());

                if(htmlFile.open(QIODevice::ReadOnly))
                {
                    /// TODO: fix for qt v5
#ifndef QT_V5
                    QString html = QString::fromLatin1(htmlFile.readAll());
                    QWebPage page;
                    QWebFrame *frame = page.mainFrame();
                    frame->setHtml(html);
                    QWebElement body = frame->findFirstElement("body");

                    if(!body.toPlainText().isEmpty())
                    {
                        isHelp = true;
                    }

                    htmlFile.close();
#endif
                }
            }
        }
        else
        {
            if(info.fileName() == QString("img"))
            {
                isPic = true;
            }
            else
            {
                QString s = info.absoluteFilePath();
                bool ee = isEmptyNode(s);

                if(ee)
                {
                    if(!QDir(absolutePath).rmdir(info.fileName()))
                    {
                        qWarning() << s << QString::fromUtf8(+" : Folder remove error (Ошибка удаления папки)");
                    }
                }
                else
                {
                    isIncludedFolder = true;
                }
            }
        }
    }

    if(isHelp || isIncludedFolder)
    {
        return false;
    }

    if(isPic)
    {
        QDir imgDir = QDir(absolutePath + QDir::separator() + "img");
        QStringList list = imgDir.entryList(QDir::NoDot | QDir::NoDotDot);
        foreach(QString p, list)
        {
            imgDir.remove(p);
        }

        QDir(absolutePath).rmdir("img");
    }

    return true;
}


bool DocDebugger::saveDoc(QString widgetFullName, const QString & header, const QString & shortDoc, const QString & fullDoc)
{
    QString fillPath = m_docRootPath + QDir::separator() + widgetFullName;
    QDir dr(fillPath);

    if(!dr.exists())
    {
        bool res = dr.mkpath(fillPath);

        if(!res)
        {
            qWarning() << "DocDebugger::saveDoc: can't create doc folder. Doc dir: " << fillPath;
            return false;
        }
    }

    int res = 0;
    QFile fHead(fillPath + QDir::separator() + getHeaderDocFilename());

    if(fHead.open(QFile::WriteOnly))
    {
        fHead.write(header.toUtf8());
        fHead.close();
        res++;
    }


    QFile fShort(fillPath + QDir::separator() + getShortDocFilename());

    if(fShort.open(QFile::WriteOnly))
    {
        fShort.write(shortDoc.toUtf8());
        fShort.close();
        res++;
    }


    QFile fFull(fillPath + QDir::separator() + m_fullDocFilename);

    if(fFull.open(QFile::WriteOnly))
    {
        fFull.write(fullDoc.toUtf8());
        fFull.close();
        res++;
    }

    if(3 == res)
    {
        installDoc(widgetFullName);
    }

    return (res == 3);
}


bool DocDebugger::loadHeaderDoc(QString widgetFullName, QString & header)
{
    QString fillPath = m_docRootPath + QDir::separator() + widgetFullName;
    QDir dr(fillPath);

    if(!dr.exists())
    {
        return false;
    }

    int res = 0;
    QFile fHead(fillPath + QDir::separator() + getHeaderDocFilename());

    if(fHead.open(QFile::ReadOnly))
    {
        header = QString::fromUtf8(fHead.readAll());
        fHead.close();
        res++;
        return true;
    }

    return false;
}


bool DocDebugger::loadShortDoc(QString widgetFullName, QString & shortDoc)
{
    QString fillPath = m_docRootPath + QDir::separator() + widgetFullName;
    QDir dr(fillPath);

    if(!dr.exists())
    {
        return false;
    }

    QFile fShort(fillPath + QDir::separator() + getShortDocFilename());

    if(fShort.open(QFile::ReadOnly))
    {
        shortDoc = QString::fromUtf8(fShort.readAll());
        fShort.close();
        return true;
    }

    return false;
}


bool DocDebugger::loadExtendDoc(QString widgetFullName, QString & fullDoc)
{
    QString fillPath = m_docRootPath + QDir::separator() + widgetFullName;
    QDir dr(fillPath);

    if(!dr.exists())
    {
        return false;
    }

    QFile fFull(fillPath + QDir::separator() + m_fullDocFilename);

    if(fFull.open(QFile::ReadOnly))
    {
        fullDoc = QString::fromUtf8(fFull.readAll());
        fFull.close();
        return true;
    }

    return false;
}


bool DocDebugger::loadDoc(QString widgetFullName, QString & header, QString & shortDoc, QString & fullDoc)
{
    QString fillPath = m_docRootPath + QDir::separator() + widgetFullName;
    QDir dr(fillPath);

    if(!dr.exists())
    {
        return false;
    }

    int res = 0;
    QFile fHead(fillPath + QDir::separator() + getHeaderDocFilename());

    if(fHead.open(QFile::ReadOnly))
    {
        header = QString::fromUtf8(fHead.readAll());
        fHead.close();
        res++;
    }


    QFile fShort(fillPath + QDir::separator() + getShortDocFilename());

    if(fShort.open(QFile::ReadOnly))
    {
        shortDoc = QString::fromUtf8(fShort.readAll());
        fShort.close();
        res++;
    }


    QFile fFull(fillPath + QDir::separator() + m_fullDocFilename);

    if(fFull.open(QFile::ReadOnly))
    {
        fullDoc = QString::fromUtf8(fFull.readAll());
        fFull.close();
        res++;
    }

    return (res == 3);
}


QString DocDebugger::getHeaderDocFilename()
{
    return m_headerDocFilename;
}


QString DocDebugger::getShortDocFilename()
{
    return m_shortDocFilename;
}


QString DocDebugger::getFullDocFilename()
{
    return m_fullDocFilename;
}


bool DocDebugger::installDoc(QWidget *wdg)
{
    QString str = getWidgetFullName(wdg);
    bool res = installDoc(str);

    return res;
}


void DocDebugger::setActiveWidgetDoc()
{
    QWidget *wdg = ewApp()->getWidget(ewApp()->getActiveWidgetId());

    if(wdg)
    {
        QString fullName = ew::network::DocDebugger::instance()->getWidgetFullName(wdg);

        if(ew::network::DocDebugger::instance()->docExist(fullName) || (0 == wdg->parent()))
        {
            QList<UserTreeNode> nodeList = DocDebugger::deSerailizeUserTree();
            QStringList relDocList = ew::network::DocDebugger::instance()->getRelFSDocDirPath();
            m_pDocWidget->updateTree(nodeList, relDocList);
            m_pDocWidget->setSourceDir(fullName);
        }
    }
}


void DocDebugger::setVisibleDocViewer(bool vis)
{
    static bool docViewInited = false;


    if(!docViewInited)
    {
        docViewInited = true;
        m_pDocWidget = new DocViewerWidget();
        ew::EmbeddedFace *mainFace = new ew::EmbeddedFace(m_pDocWidget);
        quint64 windowId = ewApp()->restoreWidget("HelpWidget", mainFace);

        if(0 == windowId)
        {
            ew::EmbeddedWidgetStruct str;
            str.iface = mainFace;
            str.addHided = true;
            str.widgetTag = "HelpWidget";
            str.allowMoving = true;
            str.topOnHint = true;
            ew::EmbeddedHeaderStruct headStr;
            headStr.hasCloseButton = true;
            headStr.hasMinMaxButton = true;
            headStr.headerPixmap = ":/embwidgets/img/img/188_users_manual.png";
            headStr.windowTitle = tr("Help");
            str.header = headStr;
            windowId = ewApp()->createWidget(str, 0);
        }

        loadFullDoc();
        QStringList lst = DocDebugger::instance()->getRelFSDocDirPath();
        auto nodeList = deSerailizeUserTree();
        m_pDocWidget->updateTree(nodeList, lst);

        m_pDocWidget->setWidgetId(windowId);
        setActiveWidgetDoc();
    }
    else
    {
        loadFullDoc();
        QStringList lst = DocDebugger::instance()->getRelFSDocDirPath();
        auto nodeList = deSerailizeUserTree();
        m_pDocWidget->updateTree(nodeList, lst);
        setActiveWidgetDoc();
    }

    ewApp()->setVisible(m_pDocWidget->widgetId(), vis);
}


void DocDebugger::removeDoc(QString widgetFullName)
{
    QString widgetDirPath = getDocRootPath() + QDir::separator() + widgetFullName;

    QFile(widgetDirPath + QDir::separator() + getHeaderDocFilename()).remove();
    QFile(widgetDirPath + QDir::separator() + getShortDocFilename()).remove();
    QFile(widgetDirPath + QDir::separator() + m_fullDocFilename).remove();

    ew::EmbeddedUtils::removeDir(widgetDirPath + QDir::separator() + getImageDirName());

    emit signalDocUpdated(widgetDirPath);
}


bool DocDebugger::installDoc(QString widgetFullName)
{
    QWidgetList wdgList = getWidgetByFullName(widgetFullName);

    if(wdgList.isEmpty())
    {
        return false;
    }

    QString header, shortDoc, fullDoc;
    bool res = loadDoc(widgetFullName, header, shortDoc, fullDoc);

    if(res)
    {
        QTextDocumentFragment frag1 = QTextDocumentFragment::fromHtml(header);
        QTextDocumentFragment frag2 = QTextDocumentFragment::fromHtml(shortDoc);
        QString linkLabel = tr("Full dox");
        QString url =  widgetFullName;
        QString htmlLink = QString("<a href=%1>%2</a>").arg(url).arg(tr("Go to full doc"));
        QTextDocument doc;
        QTextCursor curs(&doc);
        curs.insertFragment(frag1);
        curs.insertText("\n\n");
        curs.insertFragment(frag2);
        curs.insertText("\n\n");
        curs.insertHtml(htmlLink);
        QString htmlText = curs.document()->toHtml();
        QString absPath = "<img src=\"" + m_docRootPath + QDir::separator() + widgetFullName + QDir::separator() + getImageDirName();
        QString relPathTag = "<img src=\"" + getImageDirName();
        QString absShortHtml = htmlText.replace(relPathTag, absPath);

        foreach (QWidget * wdg, wdgList)
        {
            /*меняем относительные ссылки на абсолютные*/
            wdg->setWhatsThis(htmlText);
        }
    }

    return true;
}


void DocDebugger::selectWidget(QString widgetFullName)
{
    BaseMsgGetClickedWidget msg;

    msg.widgetPath = widgetFullName;

    EmbeddedNetworkManager::instance()->sendMessage(ew::network::MT_DOX_MESSAGE
                                                    , ew::network::DMT_SELECT_WIDGET
                                                    , BaseMsgGetClickedWidget::serialize(msg));
}


void DocDebugger::setFiles(QList<ew::network::MsgFile> fList)
{
    BaseMsgFiles msg;

    msg.files = fList;
    EmbeddedNetworkManager::instance()->sendMessage(ew::network::MT_DOX_MESSAGE
                                                    , ew::network::DMT_SET_FILES
                                                    , BaseMsgFiles::serialize(msg));
}


void DocDebugger::setUserTree(QList<UserTreeNode> treeList)
{
    DocMsgGetUserTree msg;

    msg.rootNodes = treeList;
    EmbeddedNetworkManager::instance()->sendMessage(ew::network::MT_DOX_MESSAGE
                                                    , ew::network::DMT_SET_USER_TREE
                                                    , DocMsgGetUserTree::serialize(msg));
}


void DocDebugger::loadUserTree()
{
    EmbeddedNetworkManager::instance()->sendMessage(ew::network::MT_DOX_MESSAGE
                                                    , ew::network::DMT_GET_USER_TREE
                                                    , QByteArray());
}


void DocDebugger::loadFiles(QList<ew::network::MsgFile> fList)
{
    BaseMsgFiles msg;

    msg.files = fList;
    EmbeddedNetworkManager::instance()->sendMessage(ew::network::MT_DOX_MESSAGE
                                                    , ew::network::DMT_GET_FILES
                                                    , BaseMsgFiles::serialize(msg));
}


void DocDebugger::loadWidgetDoc(QString widgetFullName)
{
    DocMsgWidgetDoc msg;

    msg.widgetPath = widgetFullName;
    msg.widgetShortDoc = "";
    msg.widgetFullDoc = "";

    EmbeddedNetworkManager::instance()->sendMessage(ew::network::MT_DOX_MESSAGE
                                                    , ew::network::DMT_GET_DOC
                                                    , DocMsgWidgetDoc::serialize(msg));
}


void DocDebugger::loadWidgetScreenshot(QString widgetFullName)
{
    BaseMsgGetWidgetScreenShot msg;

    msg.widgetPath = widgetFullName;


    EmbeddedNetworkManager::instance()->sendMessage(ew::network::MT_DOX_MESSAGE
                                                    , ew::network::DMT_GET_WIDGET_SCREENSHOT
                                                    , BaseMsgGetWidgetScreenShot::serialize(msg));
}


void DocDebugger::setWidgetDoc(QString widgetFullName, QString header
                               , QString widgetShortDoc, QString widgetFullDoc, QList<ew::network::MsgFile> lst)
{
    DocMsgWidgetDoc msg;

    msg.widgetPath = widgetFullName;
    msg.widgetShortDoc = widgetShortDoc;
    msg.widgetFullDoc = widgetFullDoc;
    msg.header = header;

    BaseMsgFiles fMsg;
    fMsg.files = lst;
    msg.files = fMsg;


    EmbeddedNetworkManager::instance()->sendMessage(ew::network::MT_DOX_MESSAGE
                                                    , ew::network::DMT_SET_DOC
                                                    , DocMsgWidgetDoc::serialize(msg));
}


void DocDebugger::setSelectionMode(bool isEnable)
{
    Q_UNUSED(isEnable);
    EmbeddedNetworkManager::instance()->sendMessage(ew::network::MT_DOX_MESSAGE
                                                    , ew::network::DMT_GET_CLICKED_WIDGET
                                                    , QByteArray());
}


void DocDebugger::loadFullWidgetTree()
{
    EmbeddedNetworkManager::instance()->sendMessage(ew::network::MT_DOX_MESSAGE
                                                    , ew::network::DMT_GET_FULL_WIDGET_TREE
                                                    , QByteArray());
}


bool DocDebugger::getNodeByFullName(QString fullName, const QList<DocTreeNode> &lst, DocTreeNode & foundedNode)
{
    QStringList nameList = fullName.split(getNameSeparator());

    QListIterator<QString> listIter(nameList);

    QString name;

    while(listIter.hasNext())
    {
        QString str = listIter.next();

        foreach (auto tmpNode, lst)
        {
            QString tmpName = tmpNode.className + "_" + tmpNode.objectName;

            if(tmpNode.tag == fullName )
            {
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


void DocDebugger::deleteDoc(QString widgetFullName)
{
    DocMsgWidgetDoc docMsg;

    docMsg.widgetPath = widgetFullName;
    EmbeddedNetworkManager::instance()->sendMessage(ew::network::MT_DOX_MESSAGE
                                                    , ew::network::DMT_DELETE_DOC
                                                    , DocMsgWidgetDoc::serialize(docMsg));
}


void DocDebugger::resetDocRecursive(QWidget *wdg)
{
    if(!wdg->whatsThis().isEmpty())
    {
        wdg->setWhatsThis("");
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
        resetDocRecursive(wdg);
    }
}


void DocDebugger::resetDocs()
{
    auto wList = qApp->topLevelWidgets();

    foreach (auto wdg, wList)
    {
        resetDocRecursive(wdg);
    }
}


#include <QWhatsThis>


void DocDebugger::setSelectModeEnabled(bool enabled)
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
            ewApp()->setWidgetTitle(id, m_savedTitle + m_doxDebugString);
        }

        //qApp->installEventFilter(this);
        //QWhatsThis::enterWhatsThisMode();
        m_pCheckCoordsTimer->start();
    }
    else
    {
        if(ptr)
        {
            QString tmp = ewApp()->getWidgetTitle(id);

            if(tmp.contains(m_doxDebugString))
            {
                ewApp()->setWidgetTitle(id, m_savedTitle);
            }
        }

        //qApp->removeEventFilter(this);
        //QWhatsThis::leaveWhatsThisMode();
        m_pCheckCoordsTimer->stop();
    }

    m_pSelectModeActive = enabled;
}


QWidgetList DocDebugger::getWidgetByFullName(QString fullWidgetName)
{
    QWidgetList resList;
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
                resList.append(wdg);
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

    return resList;
}


bool DocDebugger::docExist(QWidget *wdg)
{
    QString widgetFullName = getWidgetFullName(wdg);
    bool res = docExist(widgetFullName);

    return res;
}


bool DocDebugger::docExist(QString widgetFullName)
{
    QString fillPath = m_docRootPath + QDir::separator() + widgetFullName;
    QDir dr(fillPath);

    if(!dr.exists())
    {
        return false;
    }

    if( QFile::exists(fillPath + QDir::separator() + getHeaderDocFilename())
        && QFile::exists(fillPath + QDir::separator() + getShortDocFilename())
        && QFile::exists(fillPath + QDir::separator() + m_fullDocFilename))
    {
        return true;
    }

    return false;
}


QByteArray DocDebugger::getScreenShotByFullName(QString fullWidgetName)
{
    QWidgetList lst = getWidgetByFullName(fullWidgetName);
    QSize sz;

    foreach (QWidget * wdg, lst)
    {
        sz = QSize((wdg->width() + sz.width() + 10), qMax(wdg->height(), sz.height()));
    }

    int w = 0;
    QPixmap pm(sz);
    pm.fill(Qt::transparent);
    //pm.fill();

    foreach (QWidget * wdg, lst)
    {
        wdg->render(&pm, QPoint(w, 0));
        w += wdg->width() + 10;
    }


    QByteArray bytes;
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::WriteOnly);

    /*bool res = */ pm.save(&buffer, "PNG"); // writes pixmap into bytes in PNG format
    bytes = buffer.data();

    return bytes;
}


QString DocDebugger::getNameSeparator()
{
    return QDir::separator();
}


QString DocDebugger::getWidgetName(QWidget *wdg)
{
    return QString(wdg->metaObject()->className()) + "_" + wdg->objectName();
}


QString DocDebugger::getWidgetFullName(QWidget *wdg)
{
    QString clickedWidget = getWidgetName(wdg);
    QWidget *parentWdg = wdg->parentWidget();

    while(parentWdg)
    {
        clickedWidget.prepend( QString(parentWdg->metaObject()->className()) + "_" + parentWdg->objectName() + getNameSeparator() );
        parentWdg = parentWdg->parentWidget();
    }

    return clickedWidget;
}


void DocDebugger::fillNode(QWidget *wdg, /*quint64 &id
                                            ,*/
                           QMap<QString, QWidget *> & widgetMap, QMap<QString, ew::network::DocTreeNode> & nodeMap)
{
    ew::network::DocTreeNode node;
    node.objectName = wdg->objectName();
    node.className = wdg->metaObject()->className();

    node.tag = getWidgetFullName(wdg);
    node.unique = !nodeMap.contains(node.tag);
    node.hasDoc = docExist(wdg);
    QString htmlHead;
    bool loadRes = loadHeaderDoc(node.tag, htmlHead);

    if(loadRes)
    {
        node.hasHeader = !(htmlHead.isEmpty());
    }
    else
    {
        node.hasHeader = false;
    }


    QWidget *parentWdg = wdg->parentWidget();

    if(parentWdg)
    {
        node.parentTag = getWidgetFullName(parentWdg);
    }
    else
    {
        node.parentTag = "";
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
            btn->icon().pixmap(32, 32).save(&buffer, "PNG"); // writes pixmap into bytes in PNG format
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

    widgetMap.insert(node.tag, wdg);
    nodeMap.insert(node.tag, node);

    QObjectList lst = wdg->children();
    foreach (QObject * obj, lst)
    {
        QWidget *chWidget = dynamic_cast<QWidget *>(obj);

        if(chWidget)
        {
            fillNode(chWidget, widgetMap, nodeMap);
        }
    }
}


void DocDebugger::setReceivedMessage(DocMessageType mt, QByteArray arr)
{
    switch(mt)
    {
      case DMT_GET_USER_TREE:
      {
          DocMsgGetUserTree msg = DocMsgGetUserTree::deserialize(arr);
          emit signalUserTreeUpdated(msg.rootNodes);
      } break;

      case DMT_GET_FULL_WIDGET_TREE:
      {
          DocMsgGetWidgetTree msg = DocMsgGetWidgetTree::deserialize(arr);
          emit signalTreeUpdated(msg.rootNodes);
      } break; //DocMsgGetWidgetTree
      case DMT_GET_WIDGET_SCREENSHOT:
      {
          BaseMsgGetWidgetScreenShot msg = BaseMsgGetWidgetScreenShot::deserialize(arr);
          QPixmap pm;
          pm.loadFromData(msg.screenData, "PNG");
          emit signalWidgetScreenShot(msg.widgetPath, pm);
      } break;//BaseMsgGetWidgetScreenShot
      case DMT_GET_DOC:
      {
          DocMsgWidgetDoc msg = DocMsgWidgetDoc::deserialize(arr);
          emit signalWidgetDoc(msg.widgetPath, msg.header, msg.widgetShortDoc, msg.widgetFullDoc, msg.files.files);
      } break;//DocMsgWidgetDoc

      case DMT_GET_CLICKED_WIDGET:
      {
          BaseMsgGetClickedWidget msg = BaseMsgGetClickedWidget::deserialize(arr);
          emit signalWidgetSelected(msg.widgetPath);
      } break;
      case DMT_SET_DOC:
      case DMT_DELETE_DOC:
      case DMT_NULL_MESSAGE:
      case DMT_SET_FILES:
      {
          BaseMsgFiles msg = BaseMsgFiles::deserialize(arr);
          emit signalSetFiles(msg.files);
      } break;
      case DMT_GET_FILES:
      {
          BaseMsgFiles msg = BaseMsgFiles::deserialize(arr);
          emit signalGetFiles(msg.files);
      } break;
      default:
          break;
    }
}


QList<ew::network::DocTreeNode> DocDebugger::getWidgetTree(QWidget *selectedWidget)
{
    QMap<QString, QWidget *> widgetMap;
    QMap<QString, ew::network::DocTreeNode> nodeMap;

    //quint64 i = 1;
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
            ew::network::DocTreeNode node;
            fillNode(wdg, widgetMap, nodeMap);
        }
    }

    return nodeMap.values();
}


QStringList DocDebugger::getChildRelPath(const QString & parentDocPath)
{
    QDir dr(m_docRootPath + QDir::separator() + parentDocPath);

    if(dr.exists())
    {
        QStringList drList = dr.entryList(QStringList(), QDir::Dirs | QDir::NoDotAndDotDot);
        QStringList resList;

        foreach (QString str, drList)
        {
            QString fullPath;

            if(parentDocPath.isEmpty())
            {
                fullPath = (parentDocPath + str);
            }
            else
            {
                fullPath = (parentDocPath + QDir::separator() + str);
            }

            resList.append(fullPath.remove(m_docRootPath + QDir::separator()));
        }
        return resList;
    }

    return QStringList();
}


QList<UserTreeNode> DocDebugger::deSerailizeUserTree()
{
    QList<UserTreeNode> nodeList;
    QString str = m_docRootPath + QDir::separator() + m_userTreeFileName;
    QFile f(str);

    if(f.exists())
    {
        f.open(QFile::ReadOnly);

        QString line = QString::fromUtf8(f.readLine().data());
        line = line.remove("\n");

        while(!line.isEmpty())
        {
            QStringList lst = line.split(m_treeNamesSeparator);

            if(2 != lst.size())
            {
                qWarning() << "DocDebugger::deSerailizeUserTree(): Fatal error! Unknown line";
            }
            else
            {
                UserTreeNode node;
                node.tag = lst.at(0);
                node.parentTag = lst.at(1);
                QString header;
                loadHeaderDoc(node.tag, header);
                node.title = header;
                nodeList.append(node);
            }

            line = QString::fromUtf8(f.readLine().data());
            line = line.remove("\n");
        }

        f.close();
    }

    return nodeList;
}


void DocDebugger::serailizeUserTree(QList<UserTreeNode> lst)
{
    QString str = m_docRootPath + QDir::separator() + m_userTreeFileName;
    QFile f(str);

    f.remove();

    if(!f.open(QFile::WriteOnly))
    {
        qWarning() << "DocDebugger::deSerailizeUserTree(): can't write to tree file";
        return;
    }

    QString fileStr;
    foreach (UserTreeNode node, lst)
    {
        fileStr.append(node.tag + m_treeNamesSeparator + node.parentTag + "\n");
    }

    f.write(fileStr.toUtf8().data());

    f.close();
}


void DocDebugger::fillFSDirNode(QString curPath, QList<QString> & dirList)
{
    QString relDirPath = curPath;

    relDirPath = relDirPath.remove(m_docRootPath);

    if(relDirPath.startsWith("/"))
    {
        relDirPath.remove(0, 1);
    }

    if(!relDirPath.isEmpty())
    {
        dirList.append(relDirPath);
    }

    QDir dr(curPath);
    QStringList lst = dr.entryList(QStringList(), QDir::AllDirs | QDir::NoDotAndDotDot | QDir::NoSymLinks);

    foreach (QString str, lst)
    {
        fillFSDirNode( curPath + QDir::separator() + str, dirList);
    }
}


QList<QString> DocDebugger::getRelFSDocDirPath()
{
    QStringList lst;

    if(!QDir(m_docRootPath).exists())
    {
        return lst;
    }

    fillFSDirNode(m_docRootPath, lst);

    return lst;
}


void DocDebugger::slotCheckMouseCoords()
{
    QWidget *wdg = QApplication::widgetAt(QCursor::pos());

    if(0 != wdg)
    {
        QApplication::setActiveWindow(wdg);
        wdg->activateWindow();
    }
}


bool DocDebugger::eventFilter(QObject *obj, QEvent *ev)
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
              QMouseEvent *mev = dynamic_cast<QMouseEvent *>(ev);

              if(0 != (QApplication::keyboardModifiers() & Qt::ControlModifier))
              {
                  QWidget *par = qApp->widgetAt((mev->globalPos()));

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
      case QEvent::KeyPress:
      {
          QKeyEvent *kEv = dynamic_cast<QKeyEvent *>(ev);

          if(kEv)
          {
              if(Qt::Key_F1 == kEv->key())
              {
                  QWidget *wdg = dynamic_cast<QWidget *>(obj);
                  QString fullName = getWidgetFullName(wdg);

                  if(wdg)
                  {
                      //! \note Данная проверка добавлена из-за жалобы на то, что после второго нажатия на F1
                      //! исчезала справка (на самом деле открывалась справка на активное окно, т.е. на справку,
                      //! которая не была написана.
                      if(fullName != QString("ew::EmbeddedWidget_HelpWidget"))
                      {
                          if(docExist(fullName) || (0 == wdg->parent()))
                          {
                              setVisibleDocViewer(true);
//                              quint64 id = m_pDocWidget->widgetId();

//                              if(DocDebugger::deSerailizeUserTree().isEmpty())
//                              {
//                                  QStringList lst = DocDebugger::instance()->getRelFSDocDirPath();
//                                  m_pDocWidget->updateTree(lst);
//                              }
//                              else
//                              {
//                                  m_pDocWidget->updateTree(DocDebugger::deSerailizeUserTree());
//                              }

//                              m_pDocWidget->setSourceDir(fullName);

//                              ewApp()->setVisible(id, true);
                              return true;
                          }
                      }
                  }
              }
          }
      } break;
      case QEvent::WhatsThisClicked:
      {
          setVisibleDocViewer(false);
          QToolTip::hideText();

          QList<UserTreeNode> nodeList = DocDebugger::deSerailizeUserTree();
          QStringList relDocList = ew::network::DocDebugger::instance()->getRelFSDocDirPath();
          m_pDocWidget->updateTree(nodeList, relDocList);

          QString href = (dynamic_cast<QWhatsThisClickedEvent *>(ev))->href();

          if(!href.isEmpty())
          {
              if(docExist(href))
              {
                  quint64 id = m_pDocWidget->widgetId();
                  m_pDocWidget->setSourceDir(href);
                  ewApp()->setVisible(id, true);
              }
              else
              {
                  QDesktopServices::openUrl(href);
              }
          }

          return true;
      } break;
//      case QEvent::QueryWhatsThis:
//      {
//        dynamic_cast<QEvent::QueryWhatsThis>(ev);

//      } break;

      case QEvent::EnterWhatsThisMode:
      {
          loadFullDoc();
      } break;
      case QEvent::WhatsThis:
      {
          if(m_pSelectModeActive)
          {
              //QWhatsThisClickedEvent *mev = dynamic_cast<QWhatsThisClickedEvent *>(ev);

              // if(0 != (QApplication::keyboardModifiers() & Qt::ControlModifier))
              {
                  QWidget *par = dynamic_cast<QWidget *>(obj);

                  while(0 == par)
                  {
                      sendWidget = par;
                      par = sendWidget->parentWidget();
                  }

                  setSelectModeEnabled(false);
                  QString str = getWidgetFullName(sendWidget);
                  emit signalWidgetClicked(str);
                  return true;
              }
          }
      } break;

      default:
      {
      }
      break;
    }


    return QObject::eventFilter(obj, ev);
}


QByteArray DocDebugger::getDocMsgAnswer(DocMessageType mt
                                        , QByteArray & arr, bool & asyncAnswer, DocMessageType & answeredMessageType)
{
    DocMessageType stmt = mt;

    answeredMessageType = mt;

    switch(stmt)
    {/*
        case DMT_ENABLE_STYLE_MODE:
        {
          DocDebugger::instance()->setEnabled(true);
          asyncAnswer = false;
        } break;*/
      case DMT_GET_FULL_WIDGET_TREE: //DocMsgGetWidgetTree
      {
          auto list = DocDebugger::instance()->getWidgetTree();
          QStringList alreadyExisted;

          for(int i = 0; i < list.count(); i++)
          {
              if(list[i].hasDoc)
              {
                  QString headerPath = getDocRootPath() + QDir::separator() + list[i].tag + QDir::separator() + m_headerDocFilename;
                  QFile headerFile(headerPath);

                  if(headerFile.exists())
                  {
                      headerFile.open(QIODevice::ReadOnly);
                      list[i].title = QString::fromUtf8(headerFile.readAll());
                  }
              }

              QString pt = list[i].parentTag;

              alreadyExisted.append(pt + ((pt == "") ? "" : QString(QDir::separator())) + list[i].className + '_' + list[i].objectName);
          }

          auto customList = DocDebugger::instance()->getRelFSDocDirPath();
          foreach (QString str, customList)
          {
              QString dirname = getDocRootPath() + QDir::separator() + str;
              QDir dir(dirname);

              if( dir.exists() && !alreadyExisted.contains(str) )
              {
                  QStringList lst = dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot);
                  bool isHeader = lst.contains("header.hdr");

                  if(isHeader)
                  {
                      QString headerPath = dirname + QDir::separator() + "header.hdr";
                      QFile headerFile(headerPath);
                      ew::network::DocTreeNode c_node;

                      if(headerFile.exists())
                      {
                          headerFile.open(QIODevice::ReadOnly);
                          c_node.title = QString::fromUtf8(headerFile.readAll());
                      }

                      if(c_node.title.isEmpty())
                      {
                          qDebug() << "no title";
                      }

                      c_node.tag = str;
                      c_node.hasHeader = !c_node.title.isEmpty();

                      c_node.hasDoc = true;
                      QStringList aa = str.split(QDir::separator());
                      aa.pop_back();
                      c_node.parentTag = aa.join(QDir::separator());
                      list.append(c_node);
                  }
              }
          }

          DocMsgGetWidgetTree msg;
          msg.rootNodes = list;
          QByteArray resArr = DocMsgGetWidgetTree::serialize(msg);
          asyncAnswer = false;
          return resArr;
      } break;
      case DMT_GET_CLICKED_WIDGET://DocMsgGetClickedWidget
      {
          DocDebugger::instance()->setSelectModeEnabled(true);
          asyncAnswer = true;
          return QByteArray();
      } break;
      case DMT_SET_USER_TREE: //DocMsgGetUserTree
      {
          DocMsgGetUserTree msgTree = DocMsgGetUserTree::deserialize(arr);
          serailizeUserTree(msgTree.rootNodes);
          asyncAnswer = false;
          return QByteArray();
      } break;
      case DMT_GET_USER_TREE: //DocMsgGetUserTree
      {
          QList<UserTreeNode> userNodeList = deSerailizeUserTree();
          DocMsgGetUserTree usrTreeMsg;
          usrTreeMsg.rootNodes = userNodeList;

          QByteArray arr = DocMsgGetUserTree::serialize(usrTreeMsg);
          asyncAnswer = false;
          return arr;
      } break;
      case DMT_SELECT_WIDGET://DocMsgGetClickedWidget
      {
          BaseMsgGetClickedWidget msg = BaseMsgGetClickedWidget::deserialize(arr);
          emit signalHighlightWidget(msg.widgetPath);
          asyncAnswer = false;
          return QByteArray();
      } break;
      case DMT_GET_WIDGET_SCREENSHOT://DocMsgGetWidgetScreenShot
      {
          BaseMsgGetWidgetScreenShot msg = BaseMsgGetWidgetScreenShot::deserialize(arr);
          msg.screenData = getScreenShotByFullName(msg.widgetPath);
          QByteArray resArr = BaseMsgGetWidgetScreenShot::serialize(msg);
          asyncAnswer = false;
          return resArr;
      } break;
      case DMT_GET_FILES:
      {
          BaseMsgFiles msgFiles = BaseMsgFiles::deserialize(arr);
          QList<MsgFile> filesForSend;
          foreach (MsgFile msgFile, msgFiles.files)
          {
              QString widgetFolderPath = getDocRootPath() + QDir::separator() + msgFile.fullWidgetName;
              QString fullPath = widgetFolderPath + QDir::separator() + msgFile.relFilePath;
              QFile f(fullPath);

              if(f.exists() /*&& msgFile.sz == f.size()*/)
              {
                  if(f.open(QFile::ReadOnly))
                  {
                      msgFile.file = f.readAll();
                      f.close();
                  }

                  filesForSend.append(msgFile);
              }/*
                  else
                  {
                  if(f.open(QFile::ReadWrite))
                  {
                      f.write(msgFile.file);
                      f.close();
                  }
                  }*/
          }

          asyncAnswer = false;

          if(!filesForSend.isEmpty())
          {
              msgFiles.files = filesForSend;
              answeredMessageType = DMT_SET_FILES;
              QByteArray resArr = BaseMsgFiles::serialize(msgFiles);
              return resArr;
          }

          return QByteArray();
      } break;
      case DMT_SET_FILES:
      {
          BaseMsgFiles msgFiles = BaseMsgFiles::deserialize(arr);
          foreach (MsgFile msgFile, msgFiles.files)
          {
              QString widgetFolderPath = getDocRootPath() + QDir::separator() + msgFile.fullWidgetName;
              QDir dr(widgetFolderPath );

              if(dr.exists())
              {
                  dr.mkdir(getImageDirName());
              }

              QString fullPath = getDocRootPath()
                                 + QDir::separator() + msgFile.fullWidgetName
                                 + QDir::separator() + msgFile.relFilePath;
              QFile f(fullPath);

              if(f.exists() )
              {
                  if((f.size() != msgFile.sz))
                  {
                      f.remove();

                      if(f.open(QFile::WriteOnly))
                      {
                          f.write(msgFile.file);
                          f.close();
                      }
                  }
              }
              else
              {
                  if(f.open(QFile::WriteOnly))
                  {
                      f.write(msgFile.file);
                      f.close();
                  }
              }
          }

          asyncAnswer = false;
          return QByteArray();
      } break;
      case DMT_GET_DOC://DocMsgWidgetDoc
      {
          DocMsgWidgetDoc msg = DocMsgWidgetDoc::deserialize(arr);
          loadDoc(msg.widgetPath, msg.header, msg.widgetShortDoc, msg.widgetFullDoc);

          QString str = msg.widgetFullDoc + msg.widgetShortDoc;
          QTextDocument doc;
          doc.setHtml(str);
          QStringList lst = getImageList(&doc);
          lst.removeDuplicates();


          foreach (QString fPath, lst)
          {
              MsgFile msgFile;
              QString fullPath = getDocRootPath() + QDir::separator() + msg.widgetPath + QDir::separator() + fPath;
              QFile f(fullPath);

              if(f.exists())
              {
                  msgFile.fullWidgetName = msg.widgetPath;
                  msgFile.relFilePath = fPath;
                  msgFile.sz = f.size();
              }
              else
              {
                  continue;
              }

              msg.files.files.append(msgFile);
          }

          QByteArray resArr = DocMsgWidgetDoc::serialize(msg);
          //answeredMessageType = DMT_GET_FILES;
          asyncAnswer = false;
          return resArr;
      } break;
      case DMT_SET_DOC://DocMsgWidgetDoc
      {
          DocMsgWidgetDoc msg = DocMsgWidgetDoc::deserialize(arr);
          asyncAnswer = false;
          saveDoc(msg.widgetPath, msg.header, msg.widgetShortDoc, msg.widgetFullDoc);

          BaseMsgFiles fileListForAnswer;
          foreach (MsgFile msgFile, msg.files.files)
          {
              QString fullPath = getDocRootPath() + QDir::separator()
                                 + msgFile.fullWidgetName + QDir::separator() + msgFile.relFilePath;
              QFile f(fullPath);

              if(f.exists() && msgFile.sz == f.size())
              {
                  continue;
              }
              else
              {
                  fileListForAnswer.files.append(msgFile);
              }
          }

          if(!fileListForAnswer.files.isEmpty())
          {
              arr = BaseMsgFiles::serialize(fileListForAnswer);
              answeredMessageType = DMT_GET_FILES;
          }
          else
          {
              arr = QByteArray();
          }

          asyncAnswer = false;
          return arr;
      } break;
      case DMT_DELETE_DOC://null data
      {
          DocMsgWidgetDoc doc = DocMsgWidgetDoc::deserialize(arr);
          DocDebugger::instance()->removeDoc(doc.widgetPath);
          asyncAnswer = false;
      } break;
      case DMT_NULL_MESSAGE:
      default:
      {
      } break;
    }

    return QByteArray();
}


QStringList DocDebugger::getImageList(QTextDocument *document)
{
    QStringList list;

    if(document)
    {
        QTextBlock block = document->begin();

        while(block.isValid())
        {
            for( auto frag = block.begin(); frag != block.end(); ++frag )
            {
                auto curfrag = frag.fragment();

                if(curfrag.isValid())
                {
                    if(curfrag.charFormat().isImageFormat())
                    {
                        list.append(curfrag.charFormat().toImageFormat().name());
                    }
                }
            }

            block = block.next();
        }
    }

    return list;
}


void DocDebugger::slotHighlightWidgets(QString fullWidgetName)
{
    QWidgetList lst = getWidgetByFullName(fullWidgetName);

    QMap<QWidget *, QString> widgetsStylesheet;
    QMap<QWidget *, bool> widgetsWisibility;

    foreach (QWidget * wdg, lst)
    {
        widgetsWisibility.insert(wdg, wdg->isVisible());
        widgetsStylesheet.insert(wdg, wdg->styleSheet());
        wdg->setVisible(true);
    }
    QTime tm;

    tm.start();

    int tmStep = 0;

    while(tm.elapsed() < 1200)
    {
        if(tmStep != (tm.elapsed() / 200))
        {
            tmStep = (tm.elapsed() / 200);

            if(0 == (tmStep % 2))
            {
                foreach (QWidget * wdg, lst)
                {
                    wdg->setStyleSheet(" QWidget             \
                      {                                        \
                          color: #000000;                      \
                          background-color: #4A8FDA;           \
                          selection-background-color: #4A8FDA; \
                          selection-color: #B9B9B9;            \
                          background-clip: border;             \
                          border-image: none;                  \
                          border: 0px transparent black;       \
                          outline: 0;                          \
                      } ");
                }
            }
            else
            {
                foreach (QWidget * wdg, lst)
                {
                    wdg->setStyleSheet(widgetsStylesheet.value(wdg));
                }
            }
        }

        QApplication::instance()->processEvents();
    }

    foreach (QWidget * wdg, lst)
    {
        wdg->setStyleSheet(widgetsStylesheet.value(wdg));
        wdg->setVisible(widgetsWisibility.value(wdg));
    }
}
