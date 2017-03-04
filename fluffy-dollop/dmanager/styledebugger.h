#ifndef STYLEDEBUGGER_H
#define STYLEDEBUGGER_H

#include <QObject>
#include <QMap>
#include <QPixmap>
#include "embeddednetworktypes.h"

class QTimer;

namespace ew {
namespace network {
class StyleDebugger : public QObject
{
    Q_OBJECT


    QString m_styleDebugString;
    QString m_savedTitle;
    friend class EmbeddedNetworkManager;

    bool m_pSelectModeActive;
    QTimer *m_pCheckCoordsTimer;
    bool m_activated;

    explicit StyleDebugger(QObject *parent = 0);
    void fillNode(QWidget *wdg, quint64 & id, QMap<quint64, QWidget *> & widgetMap, QMap<quint64, ew::network::StyleTreeNode> & nodeMap);
    void resetStylesRecursive(QWidget *wdg);

public:
    static StyleDebugger *instance();

    /// client side
    void loadWidgetStylesheet(QString widgetFullName);

    /// client side
    void loadWidgetScreenshot(QString widgetFullName);

    /// client side
    void setWidgetStylesheet(QString widgetFullName, QString widgetStylesheet);

    /// client side
    void setSelectionMode(bool isEnable);

    /// client side
    void resetAllStyles();

    void loadFullWidgetTree();

    QString getNodeFullName(const ew::network::StyleTreeNode & parentNode
                            , const QList<ew::network::StyleTreeNode> & lst);

    bool getNodeByFullName(QString fullName, const QList<StyleTreeNode> &lst, StyleTreeNode &foundedNode);
signals:
    void signalWidgetStylesheet(QString widgetFullName, QString stylesheet);
    void signalWidgetScreenShot(QString widgetFullName, QPixmap pm);
    void signalTreeUpdated(QList<ew::network::StyleTreeNode> nodeList);
    void signalWidgetSelected(QString);

    //server side
    void signalWidgetClicked(QString);
protected:

    QList<ew::network::StyleTreeNode> getWidgetTree(QWidget *selectedWidget = 0);
    void                              setSelectModeEnabled(bool enabled);
    QString                           getStyleSheetByFullName(QString fullWidgetName);
    void                              setStyleSheetByFullName(QString fullWidgetName, QString stylesheet);
    QByteArray                        getScreenShotByFullName(QString fullWidgetName);
    QString                           getNameSeparator();
    QString                           getWidgetName(QWidget *wdg);
    QString                           getWidgetFullName(QWidget *wdg);
    QWidget                          *getWidgetByFullName(QString fullWidgetName);
    void                              resetStyles();

    /**
     * @brief Перехват событий от приложения
     * @param obj - sender
     * @param ev - event type
     * @return - always true
     */
    bool eventFilter(QObject *obj, QEvent *ev);

    //QList<ew::network::StyleTreeNode> getClickedWidgetTree();

    QByteArray getStyleMsgAnswer(StyleMessageType mt, QByteArray &arr, bool &asyncAnswer);

    void setReceivedMessage(StyleMessageType mt, QByteArray arr);
private slots:
    /**
     * @brief Отслеживание координат мыши для подсветки активного виджета
     */
    void slotCheckMouseCoords();


};
}
}

#endif // STYLEDEBUGGER_H
