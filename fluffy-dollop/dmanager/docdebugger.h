#ifndef DOCEBUGGER_H
#define DOCEBUGGER_H

#include <QMap>
#include <QObject>
#include <QPixmap>

#include "docviewerwidget.h"
#include "embeddednetworktypes.h"

class QTimer;
class QTextDocument;
class QWhatsThisClickedEvent;

namespace ew {
namespace network {
class DocDebugger : public QObject
{
    Q_OBJECT

    DocViewerWidget * m_pDocWidget = nullptr;
    QString m_doxDebugString;
    QString m_savedTitle;
    friend class EmbeddedNetworkManager;
    const QString m_fullDocFilename = "full.html";
    const QString m_shortDocFilename = "short.html";
    const QString m_headerDocFilename = "header.hdr";
    const QString m_userTreeFileName = "usertree.json";
    const QString m_treeNamesSeparator = "|||";
    bool m_pSelectModeActive;
    QTimer *m_pCheckCoordsTimer;
    bool m_activated;
    QString m_docRootPath;

    explicit DocDebugger(QObject *parent = 0);
    void fillNode(QWidget *wdg, QMap<QString, QWidget *> &widgetMap, QMap<QString, DocTreeNode> &nodeMap);
    void resetDocRecursive(QWidget *wdg);
    void fillFSDirNode(QString curPath, QList<QString> &dirList);

    QList<QString> getRelFSDocDirPath();

public:
    /// \todo явно запретить копирование
    static DocDebugger *instance();

    /**
     * @brief Инициализация компонента работы с документацией. Явный вызов требуется только для серверной части(WM_SERVER).
     * @param docRootPath
     */
    void               init(QString docRootPath);
    static QStringList getImageList(QTextDocument *document);


    static QString getImageDirName(){return "img"; }

    /********* client side ******************/


    void loadWidgetDoc(QString widgetFullName);
    void loadWidgetScreenshot(QString widgetFullName);
    void setWidgetDoc(QString widgetFullName, QString header, QString widgetShortDoc, QString widgetFullDoc, QList<MsgFile> lst);
    void setSelectionMode(bool isEnable);
    void selectWidget(QString widgetFullName);
    void deleteDoc(QString widgetFullName);
    void loadFullWidgetTree();
    bool getNodeByFullName(QString fullName, const QList<DocTreeNode> &lst, DocTreeNode &foundedNode);
    void loadFiles(QList<ew::network::MsgFile> fList);
    void setFiles(QList<ew::network::MsgFile> fList);
    void setUserTree(QList<ew::network::UserTreeNode> treeList);
    void loadUserTree();

    /********* server side ******************/
    bool installDoc(QWidget *wdg);

    /// server side

    void    removeDoc(QString widgetFullName);
    bool    installDoc(QString widgetFullName);
    QString getDocRootPath();
    bool    loadHeaderDoc(QString widgetFullName, QString &header);
    bool    loadShortDoc(QString widgetFullName, QString &shortDoc);
    bool    loadExtendDoc(QString widgetFullName, QString &fullDoc);

    //! \returns Возвращает список папок со справкой на ФС начиная от "parentDocPath"
    QStringList getChildRelPath(const QString &parentDocPath);

    QList<UserTreeNode> deSerailizeUserTree();
    void                serailizeUserTree(QList<UserTreeNode> lst);
    QString             getHeaderDocFilename();
    QString             getShortDocFilename();
    QString             getFullDocFilename();

    /**
     * @brief Метод устанавливает курсор в DocViewerWidget на документацию  активного виджета
     */
    void setActiveWidgetDoc();
    void setVisibleDocViewer(bool vis);
signals:
    // client side
    void signalSetFiles(QList<ew::network::MsgFile> );
    void signalGetFiles(QList<ew::network::MsgFile> );
    void signalTreeUpdated(QList<ew::network::DocTreeNode> nodeList);
    void signalWidgetScreenShot(QString widgetFullName, QPixmap pm);
    void signalDocUpdated(QString widgetFullName);
    void signalWidgetDoc(QString widgetFullName, QString header, QString widgetShortDoc
                         , QString widgetFullDoc, QList<ew::network::MsgFile> fList);

    void signalWidgetSelected(QString);
    void signalUserTreeUpdated(QList<ew::network::UserTreeNode> );

    //server side
    void signalHighlightWidget(QString);
    void signalWidgetClicked(QString);
    void signalShowHelp(QString);

protected:
    /// server side
    void loadFullDoc();
    bool isEmptyNode(QString absolutePath);

    QList<ew::network::DocTreeNode> getWidgetTree(QWidget *selectedWidget = 0);
    void                            setSelectModeEnabled(bool enabled);
    QByteArray                      getScreenShotByFullName(QString fullWidgetName);
    QString                         getNameSeparator();
    QString                         getWidgetName(QWidget *wdg);
    QString                         getWidgetFullName(QWidget *wdg);
    QWidgetList                     getWidgetByFullName(QString fullWidgetName);
    void                            resetDocs();

    /**
     * @brief Перехват событий от приложения
     * @param obj - sender
     * @param ev - event type
     * @return - always true
     */
    bool       eventFilter(QObject *obj, QEvent *ev);
    QByteArray getDocMsgAnswer(DocMessageType mt, QByteArray &arr, bool &asyncAnswer, DocMessageType &answeredMessageType);
    void       setReceivedMessage(DocMessageType mt, QByteArray arr);
    bool       saveDoc(QString widgetFullName, const QString &header, const QString &shortDoc, const QString &fullDoc);
    bool       loadDoc(QString widgetFullName, QString &header, QString &shortDoc, QString &fullDoc);
    bool       docExist(QString widgetFullName);
    bool       docExist(QWidget *wdg);

private slots:
    /**
     * @brief Отслеживание координат мыши для подсветки активного виджета
     */
    void slotCheckMouseCoords();
    void slotHighlightWidgets(QString fullWidgetName);
};
}
}

#endif // DOCEBUGGER_H
