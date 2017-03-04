#ifndef COMMON_MESSAGE_NOTIFIER_H
#define COMMON_MESSAGE_NOTIFIER_H

#include "imessagenotifier.h"

#define NAME_OF_NOTIFIER "CommonMessageNotifier"

class MessageCarrier;

/**
 * @brief The CommonMessageNotifier class
 * реализация шины для подписки через Qt signal/slot
 *
 * автор - Трифонов А.Е.
 */
class CommonMessageNotifier: public IMessageNotifier
{
public:
    /**
     * @brief subscribe подписка на сообщения
     * @param id идентификатор сообщения > 0
     * @param reciver указатель на QObject, принимающий сообщения
     * @param slot SLOT(...) - слот, куда придет сообщение
     * @param name_space - пространство имен (внутри которого уникальны id)
     * @param metatype тип передаваемых данных
     * @param priority приоритет 0 - 100
     * @param quaed нужно ли подписываться на сигнал через очередь
     * @return успешное подписание на сообщение
     */
    static bool subscribe( uint id, QObject *reciver, const char *slot,
                           uint metatype, QString name_space = "",
                           uint priority = 100, bool quaed = false);
    /**
     * @brief send отправка данных
     * @param id идентификатор сообщения
     * @param data данные
     * @param name_space - пространство имен (внутри которого уникальны id)
     */
    static void send( uint id, QVariant data, QString name_space = "" );

private:
    CommonMessageNotifier( QString name_space, bool can_youself );
    virtual ~CommonMessageNotifier();

    /**
     * @brief getMessageSender получеине сущности менеджера сообщений
     * @param name_space - пространство имен (внутри которого уникальны id)
     * @return указатель на менеджер
     */
    static CommonMessageNotifier* getMessageSender( QString name_space = "" );

    /**
     * @brief incommingMessage отправляем данные через доставщика
     * @param messageId id сообщения
     * @param messageData данные
     */
    void incommingMessage( int messageId,
                           QVariant messageData ) override;
    /**
     * @brief getMessageCarrierFromMatrix извлечение доставщика сообщения из карты
     * @param tag имя сообщения
     * @return указатель на доставщик сообщений
     */
    MessageCarrier* getMessageCarrierFromMatrix( uint id );

    //! карта доставщиков сообщений
    QMap< uint, MessageCarrier* > _matrix_carriers;
};

/**
 * @brief The CtrMessageCarrier class
 * Дотсавщик сообщений, используется Менеджером Сообщений для отправки сообщений
 */
class MessageCarrier : public QObject
{
    Q_OBJECT
public:
    explicit MessageCarrier(QObject *parent = 0){}

public:
    //! напрямую вызывается менеджером сообщений при отправке сообщений
    void sendMessageBySignal( QVariant data );

signals:
    //! сигнал, на который подписывается объект-получатель
    void sendMessage( QVariant data );
};


#endif // COMMONMESSAGENOTIFIER_H
