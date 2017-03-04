#ifndef VISIBLELISTWIDGET_H
#define VISIBLELISTWIDGET_H

#include <QWidget>
#include <QComboBox>
#include <QHBoxLayout>
#include <QStandardItemModel>

#include "checkboxactionwidget.h"
#include "iconactionwidget.h"

class VisibleListWidget : public QWidget
{
    Q_OBJECT
    QComboBox * m_comboBox;
    QStandardItemModel  * m_model;
    std::vector<QStandardItem*> m_standartItems;
    std::vector<CheckBoxActionWidget*> m_checkBoxItems;
    int id;

public:
    explicit VisibleListWidget(QWidget *parent = 0);
    void clear();
    CheckBoxActionWidget * addItem(QString text);
    
signals:
    void signalVisisbleChanged();
private slots:
    void slotCurrentIndexChanged(int curIndex);
    void slot_changed(const QModelIndex& topLeft, const QModelIndex& bottomRight);
    
};

#endif // VISIBLELISTWIDGET_H
