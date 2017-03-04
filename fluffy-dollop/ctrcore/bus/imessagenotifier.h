#ifndef IMESSAGENOTIFIER_H
#define IMESSAGENOTIFIER_H

#include <QVariant>
#include <QObject>
#include "globalmessages.h"

class MessageRouter;

/**
 * @brief Класс интерфейса для использования сообщений в библиотеке/плагине.
 *  Разрешен обмен сообщениями только в пределах одной библиотеки, всявязи с чем в конструкторе IMessageNotifier необходимо
 *  указать текстовую метку. Сообщения доставляются подписчику только в том случае, если тип сообщения, тип данных в сообщении
 *  и текстовые метки отправителя и получателя совпадают.
 *
 *  Для использования системы сообщений необходимо в глобальном(для библиотеки) заголовочном файле обозначить define с текстовой меткой,
 *  и зарегестрировать типы данных, которые предполагается передавать, в библиотеке Qt. Регистрация типа данных происходит макросом
 *  Q_DECLARE_METATYPE(ObjectManagerTypes::ManagerObjectInfo). Далее класс объекта подписчика(или отправителя) наследуется от интерфейса IMessageNotifier,
 *  и регистрирует сообщение, на которое хочет подписаться, напр
 *  registerMessage(ObjectManagerTypes::OMC_OBJECT_SELECTED_FROM_TREE,qMetaTypeId<ObjectManagerTypes::ManagerObjectInfo>());
 *  Пришедшие сообщения от отправителя попадают в метод incommingMessage, который так же следует переопределить в классе подписчике.
 *  Для отправки сообщений используется sendMessage с аналогичным интерфейсом.

 */
class IMessageNotifier
{
    QString m_nameSpaceString;

    friend class  MessageRouter;

protected:
    IMessageNotifier(QString namespaceString);
    IMessageNotifier(QString namespaceString, bool send_youself);
    ~IMessageNotifier();

    /**
     * @brief Метод вызывается при приеме команды от отправителя. Метод не будет вызван, если не была произведена
     *  подписка(регистрация, см registerMessage) на данный тип сообщения
     * @param messageId - id сообщения - произвольное число, характеризующее тип сообщения
     * @param messageData - данные, которые прикрепляются к сообщению
     */
    virtual void incommingMessage(int messageId, QVariant messageData);

    /**
     * @brief Отправка сообщения всем подписанным на него объектам. Отправка возможна только если объект подписан на сообщение.
     *  Сообщение не возвращается в метод incommingMessage отправителю.
     * @param messageId - id сообщения - произвольное число, характеризующее тип сообщения
     * @param messageData - данные, которые прикрепляются к сообщению
     */
    void sendMessage(int messageId, QVariant messageData);

    /**
     * @brief Метод, необходимый роутеру для выяснения имени библиотеки, в которой находятся подписчики
     * @return
     */
    QString getNamespace();

    /**
     * @brief Подписка на сообщения.
     * @param messageType - id сообщения - произвольное число, характеризующее тип сообщения
     * @param dataMetaTypeId - тип данных в сообщении. Выясняется с помощью qMetaTypeId,
     *  напр qMetaTypeId<ObjectManagerTypes::ManagerObjectInfo>()
     * @param priority - от 0 до 100. 0 - Самый высокий приоритет
     */
    void registerMessage(int messageType, int dataMetaTypeId, int priority = 100);

    /**
     * @brief isCanSendYouself проверка, может ли отправить самому себе
     * @return true - может
     */
    bool isCanSendYouself(){ return _can_send_youself; }

private:
    //! Возможность отправки самому себе
    bool _can_send_youself = false;
};


/**
 * @brief Класс-оповещатель о глобальных событиях комплекса
 */
class GlobalMessageNotifier: public QObject, public IMessageNotifier
{
    Q_OBJECT

public:
    GlobalMessageNotifier(QObject *parent = 0);
    void incommingMessage(int messageId, QVariant messageData);
    void sendMessageData(globalbusmessages::GLOBAL_MESSAGE_TYPE messageId, QVariant messageData);
signals:
    void signalMessageReceived(globalbusmessages::GLOBAL_MESSAGE_TYPE messageId, QVariant messageData);

};

Q_DECLARE_METATYPE(QList<quint64>)

#endif // IMESSAGENOTIFIER_H
