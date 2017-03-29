#include "delegate.h"
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QLineEdit>
#include <QPainter>
#include <QDebug>

SpinBoxDelegate::SpinBoxDelegate(QObject *parent)
    : ViraDelegate(parent)
{
}

void SpinBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int itemType = index.model()->data(index, TYPE).toInt();
    if(itemType == ItemTypeFloor || itemType == ItemTypeFacility)
    {
        double total = index.model()->data(index, TOTAL_AREA).toDouble();
        double rented = index.model()->data(index, RENTED_AREA).toDouble();
        // QString text = QString::number(total, 'f', 1) + QString("   ") + QString::number(rented, 'f', 1);
        painter->save();
        painter->setPen(Qt::NoPen);
        QRect leftRect(option.rect.left(), option.rect.top(), option.rect.width()/2, option.rect.height());
        painter->setBrush(QBrush(Qt::gray));
        painter->drawRect(leftRect);

        painter->setBrush(QBrush(Qt::NoBrush));
        painter->setPen(Qt::white);
        painter->drawText(leftRect, Qt::AlignCenter, QString::number(total, 'f', 1));

        QRect rightRect(option.rect.left() + option.rect.width()/2, option.rect.top(), option.rect.width()/2, option.rect.height());
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(QColor(226,224,111)));
        painter->drawRect(rightRect);

        painter->setBrush(QBrush(Qt::NoBrush));
        painter->setPen(Qt::white);
        painter->drawText(rightRect, Qt::AlignCenter, QString::number(rented, 'f', 1));

        painter->restore();
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex & index) const
{
    qulonglong id = index.model()->data(index, ID).toULongLong();
    int itemType = index.model()->data(index, TYPE).toInt();
    qDebug() << "SpinBoxDelegate::createEditor col:" << index.column() << ", row:" << index.row() << ", TYPE:" << itemType << ", ID:" << id;

    if(itemType == ItemTypeRoom || itemType == ItemTypeFloor || itemType == ItemTypeFacility)
    {
        QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
        editor->setFrame(false);
        editor->setDecimals(1);
        editor->setSingleStep(1);
        editor->setMinimum(0);
        editor->setMaximum(100000);
        return editor;
    }
    else
        return nullptr;
}

void SpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    int itemType = index.model()->data(index, TYPE).toInt();
    double value;
    if(itemType == ItemTypeRoom)
        value = index.model()->data(index, Qt::EditRole).toDouble();
    else
        value = index.model()->data(index, TOTAL_AREA).toDouble();

    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->setValue(value);
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
    spinBox->interpretText();
    double value = spinBox->value();
    int itemType = index.model()->data(index, TYPE).toInt();
    if(itemType == ItemTypeRoom)
        model->setData(index, value, Qt::EditRole);
    else
        model->setData(index, value, TOTAL_AREA);

    emit resaclArea(index);
    emit saveItemToDb(index);
}

void SpinBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

//-------------------------------------------------------------------------

ComboBoxDelegate::ComboBoxDelegate(QObject *parent)
    : ViraDelegate(parent)
{
}

void ComboBoxDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
//    if(option.state == QStyle::State_Selected)
//        return;

    painter->save();

    QString value = index.model()->data(index, Qt::EditRole).toString();
    if(value == QString::fromUtf8("Свободно"))
    {
        painter->setBrush(QBrush(QColor(86,206,18)));
    }
    else if(value == QString::fromUtf8("В аренде"))
    {
        painter->setBrush(QBrush(QColor(226,224,111)));
    }
    else if(value == QString::fromUtf8("Недоступно"))
    {
        painter->setBrush(QBrush(Qt::gray));
    }
    painter->setPen(Qt::NoPen);
    painter->drawRect(option.rect);

    painter->setPen(Qt::white);
    painter->drawText(option.rect, Qt::AlignCenter, value);

    painter->restore();
}

QSize ComboBoxDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
//    if (index.data().canConvert<StarRating>())
//    {
//        StarRating starRating = qvariant_cast<StarRating>(index.data());
//        return starRating.sizeHint();
//    } else
    {
        return QStyledItemDelegate::sizeHint(option, index);
    }
}

QWidget *ComboBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex & index ) const
{
    qulonglong id = index.model()->data(index, ID).toULongLong();
    int itemType = index.model()->data(index, TYPE).toInt();
    qDebug() << "ComboBoxDelegate::createEditor col:" << index.column() << ", row:" << index.row() << ", TYPE:" << itemType << ", ID:" << id;

    if(itemType != ItemTypeRoom)
    {
        qDebug() << "333 ---> return nullptr";
        return nullptr;
    }

    QStringList list;
    list << QString::fromUtf8("Свободно") << QString::fromUtf8("В аренде") << QString::fromUtf8("Недоступно");

    QComboBox *editor = new QComboBox(parent);
    editor->addItems(list);

    return editor;
}

void ComboBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    comboBox->setCurrentText(value);
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    QString value = comboBox->currentText();
    model->setData(index, value, Qt::EditRole);

    emit resaclArea(index);
    emit saveItemToDb(index);
}

void ComboBoxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

//-------------------------------------------------------------------------

LineEditDelegate::LineEditDelegate(QObject *parent)
    : ViraDelegate(parent)
{
}

QWidget *LineEditDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex & index ) const
{
    qulonglong id = index.model()->data(index, ID).toULongLong();
    int itemType = index.model()->data(index, TYPE).toInt();
    qDebug() << "LineEditDelegate::createEditor col:" << index.column() << ", row:" << index.row() << ", TYPE:" << itemType << ", ID:" << id;

    if(index.column() == 4)
    {
        qDebug() << "111 ---> return nullptr";
        return nullptr;
    }

    if(itemType != ItemTypeRoom && (index.column() == 3 || index.column() == 4 || index.column() == 5))
    {
        qDebug() << "222 ---> return nullptr";
        return nullptr;
    }

    QLineEdit *editor = new QLineEdit(parent);
    return editor;
}

void LineEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    lineEdit->setText(value);
}

void LineEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    QString value = lineEdit->text();
    model->setData(index, value, Qt::EditRole);

    emit saveItemToDb(index);
}

void LineEditDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}




