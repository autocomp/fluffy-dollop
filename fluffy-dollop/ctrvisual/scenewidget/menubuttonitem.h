#ifndef MENUBUTTONITEM_H
#define MENUBUTTONITEM_H

#include <QPushButton>

/**
 * @brief Класс кнопки панели задач. Отличается только внешним видом от обычного pushButton
 */
class MenuButtonItem : public QPushButton
{
    Q_OBJECT
    QIcon m_pressedIcon, m_unpressedIcon;
    bool m_checkStates;
public:
    explicit MenuButtonItem( QWidget *parent = 0);
    void setIcons(const QIcon &pressedIcon, const QIcon &unpressedIcon);
    
signals:
    
public slots:
    void slotCheckIcon(bool res);
};

#endif // MENUBUTTONITEM_H
