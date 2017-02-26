#include "visiblelistwidget.h"
#include <QDebug>
#include <QFile>

VisibleListWidget::VisibleListWidget(QWidget *parent) :
    QWidget(parent)
{

    m_comboBox = new QComboBox(this);
    setFixedWidth(180 );

    QHBoxLayout * lt = new QHBoxLayout(this);
    lt->setMargin(0);

    lt->addWidget(m_comboBox);


    m_comboBox->setEditable(false);

    connect(m_comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(slotCurrentIndexChanged(int)));
    m_model = new QStandardItemModel(m_comboBox);
    connect(m_model, SIGNAL(dataChanged ( const QModelIndex&, const QModelIndex&)), this, SLOT(slot_changed(const QModelIndex&, const QModelIndex&)));
    m_comboBox->setModel(m_model);

    QStandardItem * tmp = new QStandardItem(QString(tr("View")));
    tmp->setDropEnabled(false);
    m_model->insertRow(0,tmp);
//    setStyle(new QPlastiqueStyle);


    id = 1;
}

void VisibleListWidget::slotCurrentIndexChanged(int curIndex)
{
    m_comboBox->setCurrentIndex(0);
}

void VisibleListWidget::clear()
{
    m_model->clear();
    m_comboBox->clear();
    m_checkBoxItems.clear();
    m_standartItems.clear();

    QStandardItem * tmp = new QStandardItem(QString(tr("View")));
    tmp->setDropEnabled(false);
    m_model->insertRow(0,tmp);
    id = 1;
}

CheckBoxActionWidget * VisibleListWidget::addItem(QString text)
{

    CheckBoxActionWidget * chBox = new CheckBoxActionWidget(text, this);


    m_standartItems.push_back(chBox->getItem());
    m_checkBoxItems.push_back(chBox);

    m_model->insertRow(id, chBox->getItem());
    id++;
    m_comboBox->setCurrentIndex(-1);

    return chBox;
}




void VisibleListWidget::slot_changed(const QModelIndex& topLeft, const QModelIndex& bottomRight)
{
  //std::cout << "topLeft: " << topLeft.row() << std::endl;
  //std::cout << "bottomRight: " << bottomRight.row() << std::endl;

    int itemNum = topLeft.row() - 1;
    if(m_standartItems.capacity() <= 1 && m_checkBoxItems.capacity() <= 1)
    {
        return;
    }

  QStandardItem *item = 0;
  item = m_standartItems[topLeft.row() - 1];

  CheckBoxActionWidget * chBox = m_checkBoxItems[topLeft.row() - 1];
  if( item!=0  && item->data(Qt::CheckStateRole).isValid() &&
          item->data(Qt::CheckStateRole).toInt()/*->checkState()*/ == Qt::Unchecked )
    {

      emit chBox->sigCheckChange(false);
    //qDebug() << "Unchecked!" ;
    }
  else
    {
    //  item->setCheckState(Qt::Checked);
      emit chBox->sigCheckChange();
      emit chBox->sigCheckChange(true);
      //qDebug() << "Checked";
    }



}
