#ifndef PUSHBUTTONICONWIDGET_H
#define PUSHBUTTONICONWIDGET_H

#include <QPushButton>
#include "iconactionwidget.h"
#include "menubuttonitem.h"
//#include "buttonboxwidget.h"


/**
 * @brief Класс элемента виджета ButtonBoxWidget
 * Для внешнего использования имеет сигнал clicked()
 */
class MenuBarButtonBoxItem : public QObject
{
    Q_OBJECT

public:
    enum BUTTON_STATE
    {
        BS_PRESSED,
        BS_RELEASED
    };

private:
    friend class ButtonBoxWidget;

    IconActionWidget * m_iconActionWidget;
    MenuButtonItem * m_button;
    QIcon  m_iconForPressedState;
    QIcon  m_iconForReleasedState;
    QIcon m_curIcon;
    QString  m_text;
    int m_id;
    QString m_toolTipText;
    bool m_triggerType;

    BUTTON_STATE m_state;
protected:
    IconActionWidget * getIconActionWidget();
    MenuButtonItem *getPushButton();
    QString getText();
    int getId();
    void reCreatePushButton();
    bool isTriggerType();
    void setButtonState(BUTTON_STATE state);
    void setButtonStateWithoutEmit(BUTTON_STATE state);
public:
    explicit MenuBarButtonBoxItem(QIcon & iconForPressedState, QIcon & iconForReleasedState,
                                  QString &text, int id, bool isTriggerButton = false,QString toolTipText = QString(""), QWidget *parent = 0) ;
    ~MenuBarButtonBoxItem();
    void setIcon(QIcon & icon, BUTTON_STATE state);
    BUTTON_STATE getState();
    QIcon getIcon( BUTTON_STATE state);
    QIcon getCurStateIcon();
    void setVisible(bool isVisisble);
    void setButtonType(bool triggerType);
    void resetState();
    void setDisactive(bool flag);

signals:
    /**
     * @brief Сигнал для внутреннего использования
     * @param id - внутренний индитификатор кнопки
     */
    void btnClicked(int id);

    /**
     * @brief Сигнал для внешнего использования. При нажатии на кнопку ButtonBoxWidget(вне зависимости от положения кнопка),
     * формируется сигнал от MenuBarButtonBoxItem элемента. В методе init() sw::SceneWidget необходимо создать connect на соответсующий слот - обработчик
     */
    void clicked();
private slots:

    void slotClicked();
    
};

#endif // PUSHBUTTONICONWIDGET_H
