#include "messagerouter.h"
#include <QDebug>

#include "imessagenotifier.h"

static MessageRouter * _instance = 0;



MessageRouter::MessageRouter()
{

}


MessageRouter * MessageRouter::instance()
{
    static MessageRouter router;

    return &router;
}

void MessageRouter::registerObject(IMessageNotifier * object, int messageType, int dataMetaTypeId, int priority)
{
    ReggisteredMessage message;
    message.notifierInterface = object;
    message.messageTypeId = messageType;
    message.dataTypeId = dataMetaTypeId;
    message.priority = priority;


    QList<ReggisteredMessage> messages = m_registerredMessages.values(messageType);
    bool alreadyReggistered = false;
    foreach (ReggisteredMessage rmes, messages)
    {
        if(rmes.dataTypeId == dataMetaTypeId && rmes.messageTypeId == messageType && rmes.notifierInterface == object)
        {
            alreadyReggistered = true;
            break;
        }
    }

    if(!alreadyReggistered)
    {
        m_registerredMessages.insertMulti(messageType, message);

    }
}

void MessageRouter::unregisterObject(IMessageNotifier * object)
{
    QHash<quint64, ReggisteredMessage> ::iterator i = m_registerredMessages.begin();
    while (i != m_registerredMessages.end())
    {
        ReggisteredMessage rmes = i.value();
        if(rmes.notifierInterface == object)
        {
            i = m_registerredMessages.erase(i);
        }
        else
        {
            ++i;
        }
    }

}

void MessageRouter::sendMessage(int messageId, QVariant messageData, IMessageNotifier * notif)
{
    QList<ReggisteredMessage> messageList = m_registerredMessages.values(messageId);
    QMultiMap<int,IMessageNotifier*> priorMessages;
    foreach (ReggisteredMessage mess, messageList)
    {
        if(messageId == mess.messageTypeId)
        {
            QString str1 = mess.notifierInterface->getNamespace();
            QString str2 = notif->getNamespace();

            if( str1 == str2 &&
                    ( notif->isCanSendYouself() ||  mess.notifierInterface != notif ))
            {
                if(mess.dataTypeId == messageData.userType())
                {
                    priorMessages.insertMulti(mess.priority, mess.notifierInterface);
                }
                else
                {
                    qDebug() << "MessageRouter::sendMessage(): ERROR! Registerred message data type not equal";
                }
            }
        }
    }

    QMultiMap<int, IMessageNotifier*>::iterator i = priorMessages.begin();
    while (i != priorMessages.end())
    {
        IMessageNotifier* interface = i.value();
        interface->incommingMessage(messageId, messageData);
        ++i;
    }
}
