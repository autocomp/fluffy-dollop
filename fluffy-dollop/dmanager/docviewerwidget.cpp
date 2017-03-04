#include "docviewerwidget.h"

#include "embeddedapp.h"
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QDockWidget>
#include <QHeaderView>
#include <QLineEdit>
#include <QListWidget>
#include <QMenuBar>
#include <QPushButton>
#include <QShortcut>
#include <QSplitter>
#include <QTextBrowser>
#include <QTextDocumentFragment>
#include <QToolBar>
#include <QTreeWidget>
#include <QVBoxLayout>

#include <QStack>

#ifndef QT_V5
#include <QWebElement>
#include <QWebFrame>
#include <QWebView>
#else
#include <QWebEnginePage>
#include <QWebEngineView>
#endif

#include "docdebugger.h"


void DocViewerWidget::slotLinkClicked(QUrl url)
{
    setSourceDir(url.toString());
}


void DocViewerWidget::slotLoaded()
{
    QString windowTextColor = this->palette().color(QPalette::WindowText).name();

    /// TODO: сделать аналог на qt5
#ifndef QT_V5

    QWebView *w = qobject_cast<QWebView *>(sender());

    //Подставляем цвет шрифта из стайлшита
    w->page()->mainFrame()->findFirstElement("body").setAttribute("style", QString("color:") + windowTextColor);
#endif
}


DocViewerWidget::DocViewerWidget(QWidget *parent)
    : QWidget(parent)
{
    QWidget *browserWidgetContainer = new QWidget(this);
    QVBoxLayout *browserContLt = new QVBoxLayout(browserWidgetContainer);


    /// TODO: сделать аналог на qt5
#ifndef QT_V5
    m_pBrowser = new QWebView(browserWidgetContainer);
    connect(m_pBrowser, SIGNAL(loadFinished(bool)), SLOT(slotLoaded()));
    m_pBrowser->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    connect(m_pBrowser, SIGNAL(linkClicked(QUrl)), SLOT(slotLinkClicked(QUrl)));
#else
    m_pBrowser = new QWebEngineView(browserWidgetContainer);
    connect(m_pBrowser, SIGNAL(loadFinished(bool)), SLOT(slotLoaded()));
    //m_pBrowser->page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);
    //connect(m_pBrowser, SIGNAL(linkClicked(QUrl)), SLOT(slotLinkClicked(QUrl)));
#endif


    m_pBackwardButton = new QPushButton(QIcon(":/embwidgets/img/img/left.png"), "", this);
    m_pForwardButton  = new QPushButton(QIcon(":/embwidgets/img/img/right.png"), "", this);
    QHBoxLayout *buttonLayout = new QHBoxLayout();

    buttonLayout->addWidget(m_pBackwardButton);
    buttonLayout->addWidget(m_pForwardButton);
    browserContLt->addLayout(buttonLayout);
    browserContLt->addWidget(m_pBrowser);

    QWidget *treeWidgetContainer = new QWidget(this);
    QVBoxLayout *treeContLt = new QVBoxLayout(treeWidgetContainer);
    QHBoxLayout *searchLayout = new QHBoxLayout();
    m_pSearchLine = new QLineEdit(treeWidgetContainer);
    m_pClearSearchBtn = new QPushButton(QIcon(":/embwidgets/img/img/darkStyle/close.png"), "", this);
    connect(m_pClearSearchBtn, SIGNAL(clicked(bool)), m_pSearchLine, SLOT(clear()));
    connect(m_pSearchLine, SIGNAL(textChanged(QString)), this, SLOT(slotSearchText(QString)));
    searchLayout->addWidget(new QLabel(tr("Search:")));
    searchLayout->addWidget(m_pSearchLine);
    searchLayout->addWidget(m_pClearSearchBtn);

    treeContLt->addLayout(searchLayout);
    buttonLayout->addStretch(1);
    m_pTreeWidget = new QTreeWidget(treeWidgetContainer);
    m_pTreeWidget->header()->hide();
    treeContLt->addWidget(m_pTreeWidget);
//    connect(m_pTreeWidget, SIGNAL(currentItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)),
//            this, SLOT(slotCurrentTreeItemChanged(QTreeWidgetItem *,QTreeWidgetItem *)));
    connect(m_pTreeWidget, SIGNAL(clicked(QModelIndex)), SLOT(slotCurrentTreeItemChanged(QModelIndex)));


    m_pListWidget = new QListWidget(treeWidgetContainer);
    //m_pListWidget->header()->hide();
    treeContLt->addWidget(m_pListWidget);
    connect(m_pListWidget, SIGNAL(currentItemChanged(QListWidgetItem *,QListWidgetItem *)),
            this, SLOT(slotCurrentListItemChanged(QListWidgetItem *,QListWidgetItem *)));

    QSplitter *spl = new QSplitter(Qt::Horizontal, this);
    spl->addWidget(treeWidgetContainer);
    spl->addWidget(browserWidgetContainer);

    QVBoxLayout *mainLt = new QVBoxLayout(this);
    mainLt->addWidget(spl);

    connect(m_pForwardButton, SIGNAL(clicked()), this, SLOT(slotForward()));
    connect(m_pBackwardButton, SIGNAL(clicked()), this, SLOT(slotBackward()));

    slotSearchText("");
    slotCheckButtons();
}


void DocViewerWidget::slotShowDocViewer()
{
    //quint64 id = ewApp()->getActiveWidgetId();

    // ищем виджет с документацией
    ewApp()->setVisible(widgetId(), true);
}


void DocViewerWidget::slotCurrentTreeItemChanged(QModelIndex index)
{
    QMap<int, QVariant> data = qobject_cast<QTreeWidget *>(sender())->model()->itemData(index);

    if(data.contains(Qt::UserRole))
    {
        QString path = data[Qt::UserRole].toString();
        setSourceDir(path, true);
    }

    slotCheckButtons();
}


void DocViewerWidget::slotCurrentListItemChanged(QListWidgetItem *current, QListWidgetItem *prev)
{
    Q_UNUSED(prev);

    if(current)
    {
        QString path = current->data(Qt::UserRole).toString();
        setSourceDir(path, true);
    }
    else
    {
        m_pBrowser->setHtml(QString("<H1>" + tr("Document not open") + "</h1>"));
    }

    slotCheckButtons();
}


void DocViewerWidget::slotCheckButtons()
{
    m_pBackwardButton->setEnabled((0 < m_curHistoryIndex));
    m_pForwardButton->setEnabled((m_curHistoryIndex < (int)(m_history.size() - 1)));
}


void DocViewerWidget::slotSearchText(QString str)
{
    m_pClearSearchBtn->setEnabled(!str.isEmpty());
    m_pTreeWidget->setVisible(str.isEmpty());
    m_pListWidget->setVisible(!str.isEmpty());

    if(str.isEmpty())
    {
        QMapIterator<QString, QString> iter(m_treeHeaderMap);

        while(iter.hasNext())
        {
            iter.next();
            QListWidgetItem *it = m_listItemMap.value(iter.key());

            if(it)
            {
                it->setHidden(false);
            }
        }


        return;
    }

    QMapIterator<QString, QString> iter(m_treeHeaderMap);

    while(iter.hasNext())
    {
        iter.next();
        QListWidgetItem *it = m_listItemMap.value(iter.key());

        if(it)
        {
            it->setHidden(!(iter.value().contains(str)));
        }
    }
}


void DocViewerWidget::setWidgetId(quint64 id)
{
    m_widgetId = id;
}


//bool DocViewerWidget::selectItemByUrl(QString url)
//{
//    QTreeWidgetItem *root = m_pTreeWidget->invisibleRootItem();

//    auto getallitems = [](QTreeWidgetItem *from, QString u)
//                       {
//                           int total = from->childCount();

//                           for(int i = 0; i < total; ++i)
//                           {
//                               QTreeWidgetItem *item = from->child(i);
//                               QString url1 = item->data(0, Qt::UserRole).toString();
//                               u = u.replace(QString() + QDir::separator() + "full.html", "");
//                               if(url1 == u)
//                               {
//                                   item->setSelected(url1 == u);
//                               }
//                           }
//                       };

//    getallitems(root, url);

//    return 0;
//}


DocViewerWidget::~DocViewerWidget()
{
    if(m_pBrowser)
    {
        m_pBrowser->stop();
        //m_pBrowser->setParent(0);
    }

//     m_pBrowser->deleteLater();
//    clear();
//    delete m_pBrowser;
}


quint64 DocViewerWidget::widgetId()
{
    return m_widgetId;
}


void DocViewerWidget::clear()
{
    m_treeItemMap.clear();
    m_curHistoryIndex = 0;
    m_pBrowser->setHtml("");
    m_history.clear();
    m_pTreeWidget->clear();
    slotCheckButtons();
}


void DocViewerWidget::slotForward()
{
    if(m_curHistoryIndex < (int)(m_history.size()) - 1)
    {
        m_curHistoryIndex++;
    }

    slotCheckButtons();

    QString str = m_history.at(m_curHistoryIndex);
    setSourceDir(str, false);
}


void DocViewerWidget::slotBackward()
{
    if(m_curHistoryIndex > 0)
    {
        m_curHistoryIndex--;
    }

    slotCheckButtons();

    QString str = m_history.at(m_curHistoryIndex);
    setSourceDir(str, false);
}


//void DocViewerWidget::updateTree(QStringList pathList)
//{
//    qDebug() << pathList;
//    updateTreeRecursive(0, "", pathList);
//}


void DocViewerWidget::updateTree(QList<UserTreeNode> pathList, QStringList existDocList)
{
    m_pTreeWidget->clear();
    m_treeItemMap.clear();
    QTreeWidgetItem *m_pRoot = m_pTreeWidget->invisibleRootItem();
//    m_pTreeWidget->addTopLevelItem(m_pRoot);
    m_pTreeWidget->setCurrentItem(m_pRoot);

    m_pRoot->setExpanded(true);
//    QStack<QTreeWidgetItem *> parents;
//    QTreeWidgetItem *last = m_pRoot;
//    parents.append(m_pRoot);

    const int DataTag = Qt::UserRole;
    const int DataParentTag = Qt::UserRole + 1;


    QMap<QString, QTreeWidgetItem*> itemMap;
    foreach(ew::network::UserTreeNode node, pathList)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem();

        item->setText(0, node.title);
        item->setData(0, DataParentTag, QVariant::fromValue(node.parentTag));
        item->setData(0, DataTag, QVariant::fromValue(node.tag));
        itemMap.insert(node.tag, item);
    }

    foreach(ew::network::UserTreeNode node, pathList)
    {
        auto ptr = itemMap.value(node.tag);
        auto parentPtr = itemMap.value(node.parentTag);
        if(parentPtr)
        {
            parentPtr->insertChild(parentPtr->childCount(),ptr);
        }
        else
        {
            m_pRoot->insertChild(m_pRoot->childCount(),ptr);
        }
    }

    m_treeItemMap = itemMap;


//        QTreeWidgetItem *lastParent =  parents.isEmpty() != false ? parents.last() : m_pRoot;

//        qDebug() << "title = " << node.title << "; tag = " << node.tag << "; ptag = " << node.parentTag;
//        qDebug() << "pLastTag=" << lastParent->data(0, DataTag).toString()
//                 << "Last tag=" << last->data(0, DataTag).toString();
//        QString d1 = lastParent->data(0, DataTag).toString();

//        if(lastParent->data(0, DataTag).toString() == node.parentTag) // последний родитель не менялся
//        {
//            last = createItem(lastParent, node);
//        }
//        else if( node.parentTag == last->data(0, DataTag).toString() ) // последний родитель изменился и стал последним добавленным итемом
//        {
//            parents.append(last);
//            last = createItem(lastParent, node);
//        }
//        else
//        {
//            while(lastParent->data(0, DataTag).toString() != node.parentTag)
//            {
//                if(lastParent == m_pRoot)
//                {
//                    qDebug() << tr("File user tree is broken");
//                }

//                parents.pop();
//            }

//            last = createItem(lastParent, node);
//        }
//    }
}


//void DocViewerWidget::updateTreeRecursive(QTreeWidgetItem *parentIt, const QString & curRelPath, const QStringList & pathList)

//{
//    QStringList lst = ew::network::DocDebugger::instance()->getChildRelPath(curRelPath);


//    foreach (QString str, lst)
//    {
//        if(!pathList.contains(str))
//        {
//            continue;
//        }

//        QString htmlHead;
//        bool res = ew::network::DocDebugger::instance()->loadHeaderDoc(str, htmlHead);

//        if(res || curRelPath.isEmpty())
//        {
//            QTreeWidgetItem *treeIt = m_treeItemMap.value(str);


//            if(!treeIt)
//            {
//                treeIt = new QTreeWidgetItem();
//                treeIt->setData(0, Qt::UserRole, str);
//                m_treeItemMap.insert(str, treeIt);
//                m_treeHeaderMap.insert(str, htmlHead);

//                if(parentIt)
//                {
//                    parentIt->insertChild(parentIt->childCount(), treeIt);
//                }
//                else
//                {
//                    m_pTreeWidget->insertTopLevelItem(m_pTreeWidget->topLevelItemCount(), treeIt);
//                }
//            }

//            if(res)
//            {
//                QString head = QTextDocumentFragment::fromHtml(htmlHead).toPlainText();
//                treeIt->setText(0, head);

//                QListWidgetItem *listIt = m_listItemMap.value(str);

//                if(!listIt)
//                {
//                    listIt = new QListWidgetItem(htmlHead);
//                    listIt->setData(Qt::UserRole, str);
//                    m_pListWidget->addItem(listIt);
//                    m_listItemMap.insert(str, listIt);
//                }
//                else
//                {
//                    listIt->setText(head);
//                }
//            }
//            else
//            {
//                treeIt->setText(0, tr("Not documented"));
//            }

//            updateTreeRecursive(treeIt, str, pathList);
//        }
//        else
//        {
//            updateTreeRecursive(parentIt, str, pathList);
//        }
//    }
//}


/*DocViewerWidget *DocViewerWidget::instance()
   {
    static DocViewerWidget wdg;
    static bool created = false;

    if(!created)
    {
        ew::EmbeddedFace *mainFace = new ew::EmbeddedFace(&wdg);
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

        wdg.setWidgetId(windowId);

        created = true;
        ew::network::DocDebugger::instance()->setActiveWidgetDoc();
    }

    return &wdg;
   }*/

void DocViewerWidget::setSourceDir(QString dirUrl, bool addHistory)
{
    QStringList sl = dirUrl.split('#');
    QString fPath = ew::network::DocDebugger::instance()->getDocRootPath() + QDir::separator() + sl.front();

    if(-1 == fPath.indexOf("full.html"))
    {
        fPath = fPath.append(QString() + QDir::separator() + "full.html");
    }

    if(sl.count() > 1)
    {
        fPath = fPath.append(+'#' + sl.back());
    }

    //selectItemByUrl(sl.front());

    m_pBrowser->setUrl(fPath);

    if(addHistory)
    {
        if(m_history.size() > 0)
        {
            if(m_history.last() != dirUrl)
            {
                m_history.append(dirUrl);
            }
        }
        else
        {
            m_history.append(dirUrl);
        }

        m_curHistoryIndex = m_history.size() - 1;
    }

    QTreeWidgetItem *it = m_treeItemMap.value(dirUrl);

    if(it)
    {
        m_pTreeWidget->blockSignals(true);
        m_pTreeWidget->setCurrentItem(it);
        it->setSelected(true);
        m_pTreeWidget->blockSignals(false);
    }

    slotCheckButtons();
}
