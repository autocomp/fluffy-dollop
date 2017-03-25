#ifndef VIRAGRAPHICSITEM_H
#define VIRAGRAPHICSITEM_H

#include <QObject>

class ViraGraphicsItem : public QObject
{
    Q_OBJECT
public:
    ViraGraphicsItem() {}
    virtual ~ViraGraphicsItem() {}
    bool itemIsSelected() {return _isSelected;}
    virtual void setItemselected(bool on_off) = 0;
    virtual void centerOnItem() = 0;

signals:
    void itemIsHover(qulonglong id, bool on_off);
    void signalSelectItem(qulonglong id, bool centerOnArea);

protected:
    bool _isSelected = false;
};


#endif // VIRAGRAPHICSITEM_H
