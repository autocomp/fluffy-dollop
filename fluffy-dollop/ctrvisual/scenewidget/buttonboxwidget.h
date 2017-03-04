#ifndef BUTTONBOXWIDGET_H
#define BUTTONBOXWIDGET_H

#include <QWidget>
#include <QWidgetAction>
#include <QToolButton>
#include "iconactionwidget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include "menubarbuttonboxitem.h"
#include <QIcon>

/**
 * @brief Виджет для верхнего тулбара - представляет контейнер для
 *  кнопок, двигающихся по
 */
class ButtonBoxWidget : public QWidget
{
    Q_OBJECT
    int m_maxVerticalItemCount;
    QHBoxLayout * m_layout ;
    // int m_minUseForVertical;
    int m_buttonSize;
    int m_mainButtonTriangleWidth;
    QMap<int, MenuBarButtonBoxItem*> m_buttons;
    bool isOnePressedButtons;
    bool toolButtonDisabled;

public:


    explicit ButtonBoxWidget(int buttonSize, int verticalItemCount = 4, QWidget *parent = 0);
    /**
     * @brief Установить количество вертикальных элементов, исключая кнопку списка
     * @param count
     */

    //  void setVisible(int id, bool isVisible);
    void reinit(int verticalItemCount);
    void setVerticalItemCount(int count);
    /**
     * @brief Добавить элемент в виджет
     * @param icon иконка
     * @param text текст
     * @param toolTipText всплывающая подсказка
     * @return id кнопки
     */
    MenuBarButtonBoxItem* addItem(int id, QIcon iconForPressedState, QIcon iconForReleasedState, QString  text, bool  isTriggerButton = false, QString toolTipText = QString(""));
    MenuBarButtonBoxItem* addItem(int id, QIcon iconForPressedState, QString text, bool isTriggerButton, QString toolTipText);
    void setIcon(int buttonId, QIcon & icon, MenuBarButtonBoxItem::BUTTON_STATE state);



    /**
     * @brief Функция устанавливает режим работы кнопок, при котором доускается нажатие только одной кнопки
     */
    void setOnePressed(bool onePressed);

    /**
     * @brief getButtonState - функция возвращает состояние кнопки - нажата/отжата
     * @param id - индитификатор кнопки
     * @return
     */
    MenuBarButtonBoxItem::BUTTON_STATE getButtonState(int id);


    /**
     * @brief функция возвращает true, если кнопка является триггерной(с удержанием нажатия)
     * @param id - индитификатор кнопки
     * @return
     */
    bool isTriggerType(int id);


    /**
     * @brief Установка состояния кнопке: если состояние поменялось, то будет сформирован сигнал clicked.
     * @param id - индитификатор кнопки
     * @param state - требуемое состояние
     */
    void setButtonState(int id, MenuBarButtonBoxItem::BUTTON_STATE state);
    void setButtonStateWithoutEmit(int id, MenuBarButtonBoxItem::BUTTON_STATE state);
    void setButtonStateWithoutEmit(int id, bool state);
    void setDisactive(int id, bool active);
    int getIdByPtr(MenuBarButtonBoxItem * it);

    /**
     * @brief Сброс всех триггерных кнопок. Если состояние какой то из них поменялось, то будет сформирован сигнал clicked
     */
    void resetTriggerButtons();

    void setToolButtonDisabled( bool disabled );

    /**
     * @brief Установить видимость кнопки
     * @param buttonId - id кнопки
     * @param isVisisble - флаг видимости
     */
    void setButtonVisible(int buttonId, bool isVisisble);

    /**
     * @brief Аналог setButtonVisible(buttonId, false)
     * @param buttonId - id кнопки
     */
    void hideButton(int buttonId);

    /**
     * @brief Аналог setButtonVisible(buttonId, false)
     * @param buttonId - id кнопки
     */
    void showButton(int buttonId);



protected:
    MenuBarButtonBoxItem* getButton(int buttonId);
    void moveToVerticalList(MenuBarButtonBoxItem * item);
    void moveToToolButton(MenuBarButtonBoxItem * item);
    void setSelectedItem(MenuBarButtonBoxItem * item);
    void paintEvent ( QPaintEvent * event );
    void removeAllButtons();
    
private:
    QToolButton * m_toolButton;

    QMultiMap<int, MenuBarButtonBoxItem*> m_allWidgets;

    QList<MenuBarButtonBoxItem * > m_toolButtonWidgetList;
    QList<MenuBarButtonBoxItem * > m_verticalWidgetList;

    MenuBarButtonBoxItem * m_toolBoxMainItem;

signals:

    void signalButtonClicked(int);
    void signalButtonClicked(int, bool);
    
protected slots:
    void slotItemClicked(int);
    void slotToolButtonClicked();
};

#endif // BUTTONBOXWIDGET_H
