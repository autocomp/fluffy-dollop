#ifndef MESSAGENOTIFIER_H
#define MESSAGENOTIFIER_H

#include <QObject>
#include <QMap>
#include <QHash>
#include <QVariant>

class IMessageNotifier;


/**
 * @brief Класс роутера сообщений. Предназначен для доставки сообщений от отправителя(наследника IMessageNotifier) к получателю(также
 *является наследником IMessageNotifier).
 */
class MessageRouter : QObject
{
    Q_OBJECT

    friend class  IMessageNotifier;

public :
    struct ReggisteredMessage
    {
        ReggisteredMessage()
        {
            priority = 100;
            notifierInterface = 0;
            messageTypeId = 0;
            dataTypeId = 0 ;
        }

        quint64 messageTypeId;
        IMessageNotifier * notifierInterface;
        quint64 dataTypeId;
        int priority;
    };


private:
    MessageRouter();

public:
    static MessageRouter * instance();


protected:
    /**
     * @brief       Регистрация сообщения, класса подписчика и типа данных сообщения в роутере
     * @param object - указатель на интерфейс
     * @param messageType - id типа сообщения
     * @param dataMetaTypeId - id мета-типа(QVariant, userType() )
     * @param priority - приоритет сообщения. 100 - самый низкий приоритет. 0 - самый высокий.
     */
    void registerObject(IMessageNotifier * object, int messageType, int dataMetaTypeId, int priority = 100);
    void sendMessage(int messageId, QVariant message, IMessageNotifier *notif);
    void unregisterObject(IMessageNotifier * object);

private:
    static MessageRouter * _instance;
    QHash<quint64, ReggisteredMessage> m_registerredMessages;


};

#endif // MESSAGENOTIFIER_H
