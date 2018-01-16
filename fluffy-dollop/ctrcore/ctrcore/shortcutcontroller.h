#ifndef SHORTCUTCONTROLLER_H
#define SHORTCUTCONTROLLER_H

#include <QObject>
#include <QMap>
#include <QKeySequence>

/**
 * @brief The ShortcutController class - класс предназначен для перенаправления событий нажатия комбинаций клавиш.
 */
class ShortcutController : public QObject
{
    Q_OBJECT
public:
    static ShortcutController *instance();
    void setWidget(QWidget * widget);
    bool subscribeShortcut(const QKeySequence& keySequence, QObject *receiver, const char *member, quint64 ewWidgetId);
    void unsubscribeShortcut(const QKeySequence& keySequence, quint64 ewWidgetId);
    QStringList shortcuts();

signals:
    /**
     * @brief activated - запрещено присоединяться к сигналу ! Только для внутреннего использования !
     */
    void activated();

private slots:
    void slotShortcutActivated();

private:

    struct Receiver
    {
        QObject *object;
        QString member;
    };

    ShortcutController() {}
    void emitEvent(Receiver receiver);

    QWidget * _widget = nullptr;
    static ShortcutController * _instance;
    QMap<QString, QMap<quint64, Receiver> > receivers;
};

#endif

