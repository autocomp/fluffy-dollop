#ifndef CONCRETEEMBEDDEDSUBIFACE_H
#define CONCRETEEMBEDDEDSUBIFACE_H

#include <libembeddedwidgets/embeddedsubiface.h>

class EmbIFaceNotifier : public QObject, public ew::EmbeddedSubIFace
{
    Q_OBJECT
public:
    EmbIFaceNotifier(QWidget* widget)
    {
        _widget = widget;
    }

    virtual void closed(bool* accept)
    {
        emit signalClosed();
    }

    virtual QWidget* getWidget()
    {
        return _widget;
    }

signals:
    void signalClosed();

private:
    QWidget* _widget;
};

#endif // CONCRETEEMBEDDEDSUBIFACE_H
