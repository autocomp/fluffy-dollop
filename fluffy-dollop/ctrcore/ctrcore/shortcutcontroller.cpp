#include "shortcutcontroller.h"
#include <QShortcut>
#include <libembeddedwidgets/embeddedapp.h>

ShortcutController * ShortcutController::_instance = nullptr;

bool ShortcutController::subscribeShortcut(const QKeySequence &keySequence, QObject *_receiver, const char *member, quint64 ewWidgetId)
{
    if( ! _widget)
        return false;

    Receiver receiver;
    receiver.object = _receiver;
    receiver.member = QString(member);

    auto it = receivers.find(keySequence.toString());
    if(it != receivers.end())
    {
        it.value().insert(ewWidgetId, receiver);
    }
    else
    {
        QShortcut * shortcut = new QShortcut(_widget);
        shortcut->setKey(keySequence);
        shortcut->setContext(Qt::ApplicationShortcut);
        connect(shortcut, SIGNAL(activated()), this, SLOT(slotShortcutActivated()));

        QMap<quint64, Receiver> map;
        map.insert(ewWidgetId, receiver);
        receivers.insert(keySequence.toString(), map);
    }
    return true;
}

void ShortcutController::unsubscribeShortcut(const QKeySequence& keySequence, quint64 ewWidgetId)
{
    auto it1 = receivers.find(keySequence.toString());
    if(it1 != receivers.end())
    {
        auto it2 = it1.value().find(ewWidgetId);
        if(it2 != it1.value().end())
            it1.value().erase(it2);
    }
}

QStringList ShortcutController::shortcuts()
{
    QStringList list;
    for(auto it = receivers.begin(); it != receivers.end(); ++it)
        list.append(it.key());
    return list;
}

ShortcutController *ShortcutController::instance()
{
    if(! _instance)
        _instance = new ShortcutController;
    return _instance;
}

void ShortcutController::setWidget(QWidget *widget)
{
    _widget = widget;
}

void ShortcutController::slotShortcutActivated()
{
    QShortcut * shortcut = dynamic_cast<QShortcut*>(sender());
    if(shortcut)
    {
        auto it1 = receivers.find(shortcut->key().toString());
        if(it1 != receivers.end())
        {
            QMap<quint64, Receiver> & map(it1.value());
            auto it2 = map.find(ewApp()->getActiveWidgetId());
            if(it2 != map.end())
            {
                emitEvent(it2.value());
            }
            else
            {
                if(map.contains(0))
                    emitEvent(map[0]);
            }
        }
    }
}

void ShortcutController::emitEvent(Receiver receiver)
{
    connect(this, SIGNAL(activated()), receiver.object, receiver.member.toUtf8(), Qt::DirectConnection);
    emit activated();
    disconnect(this, SIGNAL(activated()), receiver.object, receiver.member.toUtf8());
}


















