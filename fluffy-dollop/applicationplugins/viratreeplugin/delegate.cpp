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
    if(index.column() == (int)ColumnTitle::CALC_RENT)
    {
        if(itemType == (int)ItemType::ItemTypeFloor || itemType == (int)ItemType::ItemTypeFacility)
        {
            double real_rent = index.model()->data(index, REAL_RENT).toDouble();
            double possible_rent = index.model()->data(index, POSSIBLE_RENT).toDouble();

            painter->save();
            painter->setPen(Qt::NoPen);
            QRect leftRect(option.rect.left(), option.rect.top(), option.rect.width()/2, option.rect.height());
            painter->setBrush(QBrush(Qt::gray));
            painter->drawRect(leftRect);

            painter->setBrush(QBrush(Qt::NoBrush));
            painter->setPen(Qt::white);
            painter->drawText(leftRect, Qt::AlignCenter, QString::number(possible_rent, 'f', 2));

            QRect rightRect(option.rect.left() + option.rect.width()/2, option.rect.top(), option.rect.width()/2, option.rect.height());
            painter->setPen(Qt::NoPen);
            painter->setBrush(QBrush(QColor(226,224,111)));
            painter->drawRect(rightRect);

            painter->setBrush(QBrush(Qt::NoBrush));
            painter->setPen(Qt::white);
            painter->drawText(rightRect, Qt::AlignCenter, QString::number(real_rent, 'f', 2));

            painter->restore();

            return;
        }
    }
    else if(index.column() == (int)ColumnTitle::SQUARE)
    {
        if(itemType == (int)ItemType::ItemTypeFloor || itemType == (int)ItemType::ItemTypeFacility)
        {
            double total = index.model()->data(index, TOTAL_AREA).toDouble();
            double rented = index.model()->data(index, RENTED_AREA).toDouble();
            // QString text = QString::number(total, 'f', 1) + QString("   ") + QString::number(rented, 'f', 1);
            painter->save();
            painter->setPen(Qt::NoPen);
            QRect leftRect(option.rect); //.left(), option.rect.top(), option.rect.width()/2, option.rect.height());
            painter->setBrush(QBrush(Qt::gray));
            painter->drawRect(leftRect);

            painter->setBrush(QBrush(Qt::NoBrush));
            painter->setPen(Qt::white);
            painter->drawText(leftRect, Qt::AlignCenter, QString::number(total, 'f', 1));

//            QRect rightRect(option.rect.left() + option.rect.width()/2, option.rect.top(), option.rect.width()/2, option.rect.height());
//            painter->setPen(Qt::NoPen);
//            painter->setBrush(QBrush(QColor(226,224,111)));
//            painter->drawRect(rightRect);

//            painter->setBrush(QBrush(Qt::NoBrush));
//            painter->setPen(Qt::white);
//            painter->drawText(rightRect, Qt::AlignCenter, QString::number(rented, 'f', 1));

            painter->restore();

            return;
        }
    }

    QStyledItemDelegate::paint(painter, option, index);
}

QWidget *SpinBoxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &/* option */, const QModelIndex & index) const
{
    int itemType = index.model()->data(index, TYPE).toInt();
    if(index.column() == (int)ColumnTitle::BASE_RENT)
    {
        if(itemType == (int)ItemType::ItemTypeRoom)
        {
            QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
            editor->setFrame(false);
            editor->setDecimals(2);
            editor->setSingleStep(1);
            editor->setMinimum(0);
            editor->setMaximum(1000000);
            return editor;
        }
    }
    else if(index.column() == (int)ColumnTitle::SQUARE)
    {
        if(itemType == (int)ItemType::ItemTypeRoom || itemType == (int)ItemType::ItemTypeFloor || itemType == (int)ItemType::ItemTypeFacility)
        {
            QDoubleSpinBox *editor = new QDoubleSpinBox(parent);
            editor->setFrame(false);
            editor->setDecimals(1);
            editor->setSingleStep(1);
            editor->setMinimum(0);
            editor->setMaximum(100000);
            return editor;
        }
    }

    return nullptr;
}

void SpinBoxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    int itemType = index.model()->data(index, TYPE).toInt();
    if(index.column() == (int)ColumnTitle::BASE_RENT)
    {
        double value;
        if(itemType == (int)ItemType::ItemTypeRoom)
            value = index.model()->data(index, Qt::EditRole).toDouble();

        QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
        spinBox->setProperty("original_value", QString::number(value, 'f', 2));
        spinBox->setValue(value);
    }
    else if(index.column() == (int)ColumnTitle::SQUARE)
    {
        double value;
        if(itemType == (int)ItemType::ItemTypeRoom)
            value = index.model()->data(index, Qt::EditRole).toDouble();
        else
            value = index.model()->data(index, TOTAL_AREA).toDouble();

        QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
        spinBox->setProperty("original_value", QString::number(value, 'f', 1));
        spinBox->setValue(value);
    }
}

void SpinBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    int itemType = index.model()->data(index, TYPE).toInt();
    if(index.column() == (int)ColumnTitle::BASE_RENT)
    {
        QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
        spinBox->interpretText();
        double value = spinBox->value();
        QString original_value = spinBox->property("original_value").toString();
        if(original_value != QString::number(value, 'f', 2))
        {
            if(itemType == (int)ItemType::ItemTypeRoom)
                model->setData(index, value, Qt::EditRole);

            emit resaclArea(index);
            emit saveItemToDb(index);
        }
    }
    else if(index.column() == (int)ColumnTitle::SQUARE)
    {
        QDoubleSpinBox *spinBox = static_cast<QDoubleSpinBox*>(editor);
        spinBox->interpretText();
        double value = spinBox->value();
        QString original_value = spinBox->property("original_value").toString();
        if(original_value != QString::number(value, 'f', 1))
        {
            if(itemType == (int)ItemType::ItemTypeRoom)
                model->setData(index, value, Qt::EditRole);
            else
                model->setData(index, value, TOTAL_AREA);

            emit resaclArea(index);
            emit saveItemToDb(index);
        }
    }
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

    if(itemType != (int)ItemType::ItemTypeRoom)
        return nullptr;

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
    comboBox->setProperty("original_value", value);
}

void ComboBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *comboBox = static_cast<QComboBox*>(editor);
    QString value = comboBox->currentText();
    QString original_value = comboBox->property("original_value").toString();
    if(original_value != value)
    {
        model->setData(index, value, Qt::EditRole);
        emit resaclArea(index);
        emit saveItemToDb(index);
    }
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
    int itemType = index.model()->data(index, TYPE).toInt();

    if(index.column() == (int)ColumnTitle::CALC_RENT)
        return nullptr;

    if(index.column() == (int)ColumnTitle::NAME)
    {
        // OK
    }
    else
    {
        if( itemType != (int)ItemType::ItemTypeRoom &&
                (index.column() == (int)ColumnTitle::RENTER || index.column() == (int)ColumnTitle::COMMENT) )
            return nullptr; // huyOK
    }

    QLineEdit *editor = new QLineEdit(parent);
    return editor;
}

void LineEditDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index, Qt::EditRole).toString();
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    lineEdit->setProperty("original_value", value);
    lineEdit->setText(value);
}

void LineEditDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *lineEdit = static_cast<QLineEdit*>(editor);
    QString value = lineEdit->text();
    QString original_value = lineEdit->property("original_value").toString();
    if(original_value != value)
    {
        model->setData(index, value, Qt::EditRole);
        emit saveItemToDb(index);
    }
}

void LineEditDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
    editor->setGeometry(option.rect);
}

//-------------------------------------------------------------------------

StatusDelegate::StatusDelegate(QObject *parent)
    : ViraDelegate(parent)
{
}

void StatusDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int itemType = index.model()->data(index, TYPE).toInt();
    if(itemType != (int)ItemType::ItemTypeRegion)
    {
        int tasks_new = index.model()->data(index, TASKS_NEW).toInt();
        int tasks_in_work = index.model()->data(index, TASKS_IN_WORK).toInt();
        int tasks_for_check = index.model()->data(index, TASKS_FOR_CHECK).toInt();
        if(tasks_new == 0 && tasks_in_work == 0 && tasks_for_check == 0)
        {
            QStyledItemDelegate::paint(painter, option, index);
        }
        else
        {
            int part = option.rect.width()/3;

            painter->save();
            QRect leftRect(option.rect.left(), option.rect.top(), part, option.rect.height());
            painter->setPen(Qt::NoPen);
            painter->setBrush(QBrush(QColor(233,124,27)));
            painter->drawRect(leftRect);

            painter->setBrush(QBrush(Qt::NoBrush));
            painter->setPen(Qt::white);
            painter->drawText(leftRect, Qt::AlignCenter, QString::number(tasks_new));

//-----------

            QRect centerRect(option.rect.left() + part, option.rect.top(), part, option.rect.height());
            painter->setPen(Qt::NoPen);
            painter->setBrush(QBrush(QColor(226,224,111)));
            painter->drawRect(centerRect);

            painter->setBrush(QBrush(Qt::NoBrush));
            painter->setPen(Qt::white);
            painter->drawText(centerRect, Qt::AlignCenter, QString::number(tasks_in_work));

//-----------

            QRect rightRect(option.rect.left() + part*2, option.rect.top(), part, option.rect.height());
            painter->setPen(Qt::NoPen);
            painter->setBrush(QBrush(QColor(86,206,18)));
            painter->drawRect(rightRect);

            painter->setBrush(QBrush(Qt::NoBrush));
            painter->setPen(Qt::white);
            painter->drawText(rightRect, Qt::AlignCenter, QString::number(tasks_for_check));

            painter->restore();
        }
    }
    else
    {
        QStyledItemDelegate::paint(painter, option, index);
    }
}

QWidget *StatusDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return nullptr;
}






