#ifndef VIRAGRAPHICSITEM_H
#define VIRAGRAPHICSITEM_H

#include <QObject>
#include <QPolygonF>

class ViraGraphicsItem : public QObject
{
    Q_OBJECT
public:
    enum ItemType {ItemType_Undefinite = 0, ItemType_Defect = 1, ItemType_Foto = 2, ItemType_Foto360 =3};

    ViraGraphicsItem() {}
    virtual ~ViraGraphicsItem() {}
    bool itemIsSelected() {return _isSelected;}
    virtual void setItemselected(bool on_off) = 0;
    virtual void setItemEnabled(bool on_off) = 0;
    virtual void setItemVisible(bool on_off) = 0;
    virtual void centerOnItem() = 0;
    virtual quint64 getId() = 0;
    virtual void reinit() = 0;
    virtual QPolygonF getPolygon() {return QPolygonF();}
    virtual ItemType getItemType() {return ItemType_Undefinite;}

signals:
    void itemIsHover(qulonglong id, bool on_off);
    void signalSelectItem(qulonglong id, bool centerOnEntity);

protected:
    bool _isSelected = false;

};


#endif // VIRAGRAPHICSITEM_H
