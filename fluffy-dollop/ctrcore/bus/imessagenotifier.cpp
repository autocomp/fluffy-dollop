#include "imessagenotifier.h"
#include "messagerouter.h"
#include <QList>

IMessageNotifier::IMessageNotifier(QString namespaceString) :
    m_nameSpaceString( namespaceString),
    _can_send_youself( false )
{}

IMessageNotifier::IMessageNotifier(QString namespaceString, bool send_youself):
    m_nameSpaceString( namespaceString),
    _can_send_youself( send_youself )
{}

QString IMessageNotifier::getNamespace()
{
    return m_nameSpaceString;
}


void IMessageNotifier::incommingMessage(int messageId, QVariant messageData)
{}


void IMessageNotifier::sendMessage(int messageId, QVariant messageData)
{
    MessageRouter::instance()->sendMessage(messageId, messageData,this);
}


void IMessageNotifier::registerMessage(int messageType, int dataMetaTypeId, int priority)
{
    MessageRouter::instance()->registerObject(this, messageType, dataMetaTypeId,priority);
}


IMessageNotifier::~IMessageNotifier()
{
    MessageRouter::instance()->unregisterObject(this);
}


using namespace globalbusmessages;

GlobalMessageNotifier::GlobalMessageNotifier(QObject* parent):
    QObject(parent),
    IMessageNotifier(globalbusmessages::globalNamespace)
{
    registerMessage(GMT_SCENARIO_OPENNED, qMetaTypeId<int>());
    registerMessage(GMT_SCENARIO_CLOSED, qMetaTypeId<int>());
    registerMessage(GMT_SCENARIO_PREPARE_TO_CLOSE, qMetaTypeId<int>());
    registerMessage(GMT_RELOAD_SCENARIO, qMetaTypeId<int>());
    registerMessage(GMT_OPEN_SCENARIO, qMetaTypeId<int>());
    registerMessage(GMT_CLOSE_SCENARIO, qMetaTypeId<int>());
    registerMessage(GMT_EDIT_LIST_CHANGED, qMetaTypeId< QList<quint64> >());
    registerMessage(GMT_USER_ACTION_ON_MAP, qMetaTypeId< UserActionOnMap >());
}

void GlobalMessageNotifier::incommingMessage(int messageId, QVariant messageData)
{
    emit signalMessageReceived((globalbusmessages::GLOBAL_MESSAGE_TYPE)messageId, messageData);
}


void GlobalMessageNotifier::sendMessageData(globalbusmessages::GLOBAL_MESSAGE_TYPE messageId, QVariant messageData)
{
    sendMessage((int)messageId, messageData);
}
