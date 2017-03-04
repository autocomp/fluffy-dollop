#ifndef DOCWIEVERWIDGET_H
#define DOCWIEVERWIDGET_H


#include <QMap>
#include <QUrl>
#include <QWidget>


class QListWidget;
class QTextBrowser;
class QTreeWidget;
class QPushButton;
class QComboBox;
class QLineEdit;
class QTreeWidgetItem;
class QListWidgetItem;

#ifdef QT_V5
class QWebEngineView;
#else
class QWebView;
#endif


#include <QModelIndex>

#include "embeddednetworktypes.h"
using namespace ew::network;

/// \defgroup Documrntation Документация

///
/// \ingroup Documrntation \ingroup GUI
/// \brief Виджет для поиска и просмотра документации.
///

class DocViewerWidget : public QWidget
{
    Q_OBJECT

//    QTextBrowser * m_pBrowser;

#ifdef QT_V5
    QWebEngineView * m_pBrowser = 0;
#else
    QWebView * m_pBrowser = 0;
#endif


    QString m_rootDir;
    QTreeWidget *m_pTreeWidget = 0;
    QListWidget *m_pListWidget = 0;
    QLineEdit *m_pSearchLine = 0;
    QStringList m_history;
    quint64 m_widgetId = 0;
    int m_curHistoryIndex = 0;
    QPushButton *m_pBackwardButton, *m_pForwardButton, *m_pClearSearchBtn;
    QMap<QString, QTreeWidgetItem *> m_treeItemMap; ///< <Путь к файлу документации, элемент>
    QMap<QString, QListWidgetItem *> m_listItemMap; ///<
    QMap<QString, QString>          m_treeHeaderMap;

private:
public:
    explicit DocViewerWidget(QWidget *parent = 0);
    ~DocViewerWidget();
    //static DocViewerWidget *instance();
    quint64 widgetId();

    /// \brief Отображает переданный в качестве параметра HTML текст.
    /// \param HTML текст.
    void setHtml(QString html);

    /// \brief setSourceUrl Отображает справку из переданной директории.
    /// \param url URL к  директории.
    /// \example setSource("somedir#RenderFlag-enum")
    void setSourceDir(QString dirUrl, bool addHistory = true);

    /**
     * @brief Построение/обновление дерева документации. pathList и existDocList могут не совпадать!
     *  Если такое происходит, то existDocList выстраивается отдельно от корня.
     * @param pathList - юзер дерево - построено в docEditor
     * @param existDocList - загруженная с диска документация
     */
    void updateTree(QList<UserTreeNode> pathList, QStringList existDocList);
    //void updateTree(QStringList pathList);
    //void updateTreeRecursive(QTreeWidgetItem *parentIt, const QString &curRelPath, const QStringList &pathList);
    void setWidgetId(quint64 id);

protected:
    //bool selectItemByUrl(QString url);
    void clear();

protected slots:
    void slotBackward();
    void slotForward();
    void slotSearchText(QString str);
    void slotCheckButtons();
    void slotCurrentTreeItemChanged(QModelIndex index);
    void slotCurrentListItemChanged(QListWidgetItem *current, QListWidgetItem *prev);

private slots:
    void slotShowDocViewer();

    void slotLinkClicked(QUrl);
    void slotLoaded();
};

#endif // DOCWIEVERWIDGET_H
