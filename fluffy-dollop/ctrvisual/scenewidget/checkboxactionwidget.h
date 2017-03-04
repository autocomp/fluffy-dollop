#ifndef CHECKBOXACTIONWIDGET_H
#define CHECKBOXACTIONWIDGET_H
#include <QCheckBox>
#include <QWidgetAction>
#include <QStandardItem>

class CheckBoxActionWidget : public QObject
{
    Q_OBJECT

    friend class VisibleListWidget;
    QStandardItem* m_item;
    QString m_text;

public:
    explicit CheckBoxActionWidget(QString text, QObject *parent = 0);
    bool getSelectState();
    void setSelected(bool selected);
    void setActive(bool active);

private:
    QStandardItem * getItem();
    
signals:
    void sigCheckChange(bool);
    void sigCheckChange();
    
public slots:
    
};

#endif // CHECKBOXACTIONWIDGET_H
