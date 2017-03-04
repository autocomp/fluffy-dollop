#include "common_message_notifier.h"

CommonMessageNotifier::CommonMessageNotifier( QString name_space,
                                              bool can_youself ):
    IMessageNotifier( name_space,
                      can_youself )
{}

CommonMessageNotifier::~CommonMessageNotifier()
{
    qDeleteAll( _matrix_carriers );
}

bool CommonMessageNotifier::subscribe(uint id, QObject *reciver,
                                       const char *slot, uint metatype,
                                       QString name_space, uint priority, bool quaed )
{
    CommonMessageNotifier* notifyer = getMessageSender( name_space );

    if( priority > 100 )
        priority = 100;

    notifyer->registerMessage( id, metatype, priority );

    MessageCarrier* carier = notifyer->getMessageCarrierFromMatrix( id );
    bool conn = QObject::connect( carier, SIGNAL( sendMessage( QVariant )), reciver, slot,
                                  ( quaed ? Qt::QueuedConnection : Qt::DirectConnection ));
    return conn;
}

void CommonMessageNotifier::send(uint id, QVariant data , QString name_space)
{
    getMessageSender( name_space )->sendMessage( id, data );
}

CommonMessageNotifier *CommonMessageNotifier::getMessageSender( QString name_space )
{
    if( name_space.isEmpty() )
        name_space = NAME_OF_NOTIFIER;

    static QMap< QString, CommonMessageNotifier* > map_notifier;
    if( !map_notifier[name_space] )
        map_notifier[name_space] = new CommonMessageNotifier( name_space, true );
    return map_notifier[name_space];
}

void CommonMessageNotifier::incommingMessage( int messageId,
                                              QVariant messageData )
{
    MessageCarrier* carier = getMessageCarrierFromMatrix( messageId );
    carier->sendMessageBySignal( messageData );
}

MessageCarrier *CommonMessageNotifier::getMessageCarrierFromMatrix(uint id)
{
    if( _matrix_carriers.find( id ) == _matrix_carriers.end())
    {
        _matrix_carriers[ id ] = new MessageCarrier;
    }
    return _matrix_carriers[ id ];
}

void MessageCarrier::sendMessageBySignal(QVariant data)
{
    emit sendMessage( data );
}
