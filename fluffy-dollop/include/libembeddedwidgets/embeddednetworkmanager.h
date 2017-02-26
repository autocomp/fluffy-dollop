#ifndef EMBEDDEDNETWORKMANAGER_H
#define EMBEDDEDNETWORKMANAGER_H

#include "embeddednetworktypes.h"
#include <QObject>

class QTcpServer;
class QTcpSocket;

namespace ew {
class EmbeddedApp;
namespace network {
/**
 * @brief Класс менеджера сети для обеспечения связи с внешними приложениями.
 */

class EmbeddedNetworkManager : public QObject
{
    Q_OBJECT

    friend class ew::EmbeddedApp;
    friend class StyleDebugger;
    friend class DocDebugger;

    quint16 m_port = 20342;
    QTcpServer *m_pServer;
    QTcpSocket *m_pClient;
    MessageClassType m_lastMessageClassType;
    int m_lastMessageType;
    QTcpSocket *m_pConnectedSocket;

public:
    enum WorkMode
    {
        WM_SERVER = 0, ///< Целевое приложение
        WM_CLIENT ///< Программа для настройки
    };

private:
    int m_recvTimeoutSec = 10;
    const int m_sendMulticastDelaySec = 10;
    WorkMode m_mode;
    explicit EmbeddedNetworkManager(QObject *parent = 0);

protected:
    static EmbeddedNetworkManager *instance();
    void                           init(WorkMode wm, quint16 port);

    /// client side
    void sendMessage(ew::network::MessageClassType mt, quint32 messageType, QByteArray data);

signals:
    //client side
    void signalMessageReceived(ew::network::MessageClassType mt, QByteArray data);

public slots:
protected slots:
    //server side
    void slotClientConnect();

    // client side
    void slotConnectedWithServer();

    //server side
    void slotWidgetSelected(QString widgetFullName);
    void slotDataAvailableFromServer();

    void slotClientReceivedData();
};
}
}

#endif // EMBEDDEDNETWORKMANAGER_H
