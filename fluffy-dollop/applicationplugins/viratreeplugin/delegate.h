#ifndef DELEGATE_H
#define DELEGATE_H

#include <QStyledItemDelegate>

enum ItemType{ItemTypeRoom = 1, ItemTypeFloor = 2, ItemTypeFacility = 3, ItemTypeOther = 4};
const int ID = Qt::UserRole + 1;
const int TYPE = Qt::UserRole + 2;
const int RENTED_AREA = Qt::UserRole + 3;
const int TOTAL_AREA = Qt::UserRole + 4;
const int CALC_TOTAL_AREA_AUTOMATIC = Qt::UserRole + 5;

class ViraDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ViraDelegate(QObject *parent) : QStyledItemDelegate(parent) {}
signals:
    void resaclArea(const QModelIndex &index) const;
    void saveItemToDb(const QModelIndex &index) const;
};

class SpinBoxDelegate : public ViraDelegate
{
    Q_OBJECT
public:
    SpinBoxDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class ComboBoxDelegate : public ViraDelegate
{
    Q_OBJECT
public:
    ComboBoxDelegate(QObject *parent = 0);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

class LineEditDelegate : public ViraDelegate
{
    Q_OBJECT
public:
    LineEditDelegate(QObject *parent = 0);
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override;
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};


#endif
