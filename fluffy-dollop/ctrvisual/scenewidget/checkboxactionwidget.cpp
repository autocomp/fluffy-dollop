#include "checkboxactionwidget.h"
#include <QHBoxLayout>


CheckBoxActionWidget::CheckBoxActionWidget(QString text, QObject *parent) :
    QObject(parent)
{
    m_text = text;
    m_item = new QStandardItem;
    m_item->setText(text);
    m_item->setTristate(false);
    m_item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    m_item->setData(Qt::Unchecked, Qt::CheckStateRole);

}

QStandardItem * CheckBoxActionWidget::getItem()
{
    return m_item;
}

bool CheckBoxActionWidget::getSelectState()
{
    if(m_item->checkState() == Qt::Unchecked)
    {
        return false;
    }
    else if(m_item->checkState() == Qt::Checked)
    {
        return true;
    }
}

void CheckBoxActionWidget::setSelected(bool selected)
{
    if(selected)
    {
        m_item->setCheckState(Qt::Checked);
    }
    else
    {
        m_item->setCheckState(Qt::Unchecked);
    }

}

void CheckBoxActionWidget::setActive(bool active)
{
    m_item->setEnabled(active);
}


