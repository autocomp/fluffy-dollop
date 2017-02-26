#include "buttonboxwidget.h"
#include <QPainter>
#include <QDebug>
#include <QFile>
#include <QPainter>
#include <QColor>

ButtonBoxWidget::ButtonBoxWidget(int buttonSize, int verticalItemCount, QWidget *parent) :
    QWidget(parent)
{

    isOnePressedButtons = false;
    m_toolBoxMainItem = 0;
    m_mainButtonTriangleWidth = 4;

    setFixedSize(buttonSize*verticalItemCount + 40,35);
    setMinimumWidth(buttonSize*verticalItemCount + 25);
    m_maxVerticalItemCount = verticalItemCount;
    m_toolButton = 0;
    toolButtonDisabled = false;

    m_layout = new QHBoxLayout(this);
    m_layout->setMargin(0);
    m_layout->setSpacing(5);
    m_layout->setContentsMargins(5,0,5,0);
    m_buttonSize = buttonSize/*qMin( width()/m_maxVerticalItemCount , height() )*/;

    hide();


}


void ButtonBoxWidget::reinit(int verticalItemCount)
{
    removeAllButtons();

    isOnePressedButtons = false;
    m_mainButtonTriangleWidth = 4;

    setFixedSize(m_buttonSize*verticalItemCount + 10*verticalItemCount,30);
    setMinimumWidth(m_buttonSize*verticalItemCount + 10*verticalItemCount);
    m_maxVerticalItemCount = verticalItemCount;
    toolButtonDisabled = false;

    //m_layout = new QHBoxLayout(this);
    //m_layout->setMargin(0);
    // m_layout->setSpacing(0);
    // m_layout->setContentsMargins(5,0,0,0);
}

MenuBarButtonBoxItem* ButtonBoxWidget::addItem(int id, QIcon iconForPressedState, QString text, bool isTriggerButton, QString toolTipText)
{
    QPixmap pIcon;
    //pIcon = iconForPressedState.pixmap(32);
    QPainter pr(&pIcon);
    pr.fillRect(0,0,pIcon.width(), pIcon.height(),QColor(0xC3, 0xC3, 0xC3,150));

    QIcon releaseIcon = pIcon;

    addItem(id, iconForPressedState, releaseIcon,text,isTriggerButton,toolTipText);

}

MenuBarButtonBoxItem* ButtonBoxWidget::addItem(int id, QIcon iconForPressedState, QIcon iconForReleasedState,QString text, bool isTriggerButton, QString toolTipText)
{

    show();

    // qDebug() << "ButtonBoxWidget::addItem()";
    MenuBarButtonBoxItem *buttonBoxItem = new MenuBarButtonBoxItem(iconForPressedState,iconForReleasedState,text,id,isTriggerButton,toolTipText,this);
    m_buttons.insert(id, buttonBoxItem);
    connect(buttonBoxItem, SIGNAL(btnClicked(int)), this, SLOT(slotItemClicked(int)));

    m_allWidgets.insert(id,buttonBoxItem);

    buttonBoxItem->getPushButton()->setFixedSize(m_buttonSize, m_buttonSize);
    buttonBoxItem->getPushButton()->setContentsMargins(1,1,1,1);
    buttonBoxItem->getPushButton()->setIconSize(QSize(m_buttonSize-2, m_buttonSize-2));


    if(0 == m_toolButton && m_verticalWidgetList.count() == m_maxVerticalItemCount)
    {
        foreach (MenuBarButtonBoxItem * item, m_verticalWidgetList)
        {
            m_layout->removeWidget(item->getPushButton());
            item->reCreatePushButton();
            buttonBoxItem->getPushButton()->setFixedSize(m_buttonSize, m_buttonSize);
            buttonBoxItem->getPushButton()->setContentsMargins(1,1,1,1);
            buttonBoxItem->getPushButton()->setIconSize(QSize(m_buttonSize-2, m_buttonSize-2));
        }



        m_toolBoxMainItem = m_verticalWidgetList.first();
        m_toolButton = new QToolButton(this);
        m_layout->addWidget(m_toolButton);
        m_toolButton->setFixedSize(m_buttonSize*1.3, m_buttonSize);
        m_toolButton->setContentsMargins(1,1,1,1);
        m_toolButton->setIconSize(QSize(m_buttonSize-2, m_buttonSize-2));

        m_toolButton->setIcon(m_toolBoxMainItem->getPushButton()->icon());
        connect(m_toolButton, SIGNAL(clicked()), this,SLOT( slotToolButtonClicked() ));
        m_toolButton->setPopupMode(QToolButton::MenuButtonPopup);

        m_verticalWidgetList.removeFirst();
        //m_layout->removeWidget(m_toolBoxMainItem->getPushButton());

        m_toolButtonWidgetList.prepend(buttonBoxItem);
        m_toolButton->addAction(buttonBoxItem->getIconActionWidget());

        foreach (MenuBarButtonBoxItem * item, m_verticalWidgetList)
        {
            //m_verticalWidgetList.prepend(item);
            m_layout->addWidget(item->getPushButton(),0,Qt::AlignCenter);
        }
    }
    else
    {
        /*смотрим, в какой контейнер добавить новый итем*/
        if( ((m_verticalWidgetList.count() < m_maxVerticalItemCount) && 0 == m_toolButton)
                || ( (0 != m_toolButton && (m_verticalWidgetList.count() < m_maxVerticalItemCount - 1) )) )
        {
            m_verticalWidgetList.prepend(buttonBoxItem);
            m_layout->addWidget(buttonBoxItem->getPushButton(),0,Qt::AlignCenter);
        }
        else
        {
            m_toolButtonWidgetList.prepend(buttonBoxItem);
            m_toolButton->addAction(buttonBoxItem->getIconActionWidget());
        }
    }

    if(toolButtonDisabled)
        m_toolButton->hide();

    return buttonBoxItem;

}


void ButtonBoxWidget::setOnePressed(bool onePressed)
{
    isOnePressedButtons = onePressed;
}


MenuBarButtonBoxItem::BUTTON_STATE ButtonBoxWidget::getButtonState(int id)
{
    MenuBarButtonBoxItem::BUTTON_STATE buttonState = MenuBarButtonBoxItem::BS_RELEASED;
    MenuBarButtonBoxItem * btn = m_buttons.value(id,0);

    if(0 != btn)
        buttonState = btn->getState();

    return buttonState;
}

void ButtonBoxWidget::setDisactive(int id, bool active)
{
    MenuBarButtonBoxItem * btn = m_buttons.value(id,0);
    if(0 != btn)
    {
        btn->setDisactive(active);
    }
}

void ButtonBoxWidget::removeAllButtons()
{

    foreach (MenuBarButtonBoxItem * wdg, m_buttons)
    {
        if(-1 != m_layout->indexOf(wdg->getPushButton()));
        m_layout->removeWidget(wdg->getPushButton());
        delete wdg;
    }

    m_layout->removeWidget(m_toolButton);
    delete m_toolButton;
    m_toolButton = 0;
    m_toolBoxMainItem = 0;
    m_buttons.clear();
    m_verticalWidgetList.clear();
    m_toolButtonWidgetList.clear();
    m_allWidgets.clear();

}
/*
void ButtonBoxWidget::setVisible(int id, bool isVis)
{
    MenuBarButtonBoxItem * btn = m_buttons.value(id,0);
    if(0 != btn)
    {
        btn->setVisible(isVis);
    }
}*/

void ButtonBoxWidget::setButtonState(int id, MenuBarButtonBoxItem::BUTTON_STATE state)
{
    MenuBarButtonBoxItem * btn = m_buttons.value(id,0);

    if(0 != btn)
    {
        btn->setButtonState(state);
    }
}

void ButtonBoxWidget::setButtonStateWithoutEmit(int id, MenuBarButtonBoxItem::BUTTON_STATE state)
{
    MenuBarButtonBoxItem * btn = m_buttons.value(id,0);

    if(0 != btn)
    {
        btn->setButtonStateWithoutEmit(state);
    }
}


int ButtonBoxWidget::getIdByPtr(MenuBarButtonBoxItem * it)
{
    return m_allWidgets.key(it,-1);
}

void ButtonBoxWidget::setButtonStateWithoutEmit(int id, bool state)
{
    MenuBarButtonBoxItem * btn = m_buttons.value(id,0);

    MenuBarButtonBoxItem::BUTTON_STATE statebs;
    if(!state)
        statebs = MenuBarButtonBoxItem::BS_RELEASED;
    else
        statebs = MenuBarButtonBoxItem::BS_PRESSED;

    if(0 != btn)
    {
        btn->setButtonStateWithoutEmit(statebs);
    }
}

void ButtonBoxWidget::resetTriggerButtons()
{
    foreach (MenuBarButtonBoxItem * btn, m_buttons)
    {
        if( (0 != btn ) &&
                (MenuBarButtonBoxItem::BS_PRESSED == btn->getState()) )
        {
            btn->resetState();
            //btn->setButtonState(MenuBarButtonBoxItem::BS_RELEASED);
        }
    }
}

void ButtonBoxWidget::setToolButtonDisabled( bool disabled )
{
    toolButtonDisabled = disabled;
}

bool ButtonBoxWidget::isTriggerType(int id)
{
    bool buttonType = false;
    MenuBarButtonBoxItem * btn = m_buttons.value(id,0);

    if(0 != btn)
        buttonType = btn->isTriggerType();

    return buttonType;
}


void ButtonBoxWidget::setIcon(int buttonId, QIcon & icon, MenuBarButtonBoxItem::BUTTON_STATE state)
{
    MenuBarButtonBoxItem * btn = m_buttons.value(buttonId,0);

    if(0 != btn)
        btn->setIcon(icon, state);
}





MenuBarButtonBoxItem* ButtonBoxWidget::getButton(int buttonId)
{
    MenuBarButtonBoxItem * btn = m_buttons.value(buttonId,0);
    return btn;
}


void ButtonBoxWidget::showButton(int buttonId)
{
    MenuBarButtonBoxItem * btn = m_buttons.value(buttonId,0);
    if(0 != btn)
    {
        btn->setVisible(true);
    }
}

void ButtonBoxWidget::hideButton(int buttonId)
{
    MenuBarButtonBoxItem * btn = m_buttons.value(buttonId,0);
    if(0 != btn)
    {
        btn->setVisible(false);
    }
}

void ButtonBoxWidget::setButtonVisible(int buttonId, bool isVisisble)
{
    MenuBarButtonBoxItem * btn = m_buttons.value(buttonId,0);
    if(0 != btn)
    {
        btn->setVisible(isVisisble);
    }
}

void ButtonBoxWidget::slotItemClicked(int id)
{
    int a = 5;

    MenuBarButtonBoxItem * temp = m_allWidgets.value(id);

    if( temp->isTriggerType() && (MenuBarButtonBoxItem::BS_PRESSED == temp->getState() ) )
    {
        /*сброс тригерных кнопок*/
        if(isOnePressedButtons)
        {
            foreach (MenuBarButtonBoxItem * item, m_allWidgets)
            {
                if(item->isTriggerType() && ( temp != item)
                        && (MenuBarButtonBoxItem::BS_PRESSED == item->getState()) )
                {
                    //                    item->setNonreleased(false);
                    item->resetState();
                    emit signalButtonClicked(item->getId());
                    bool isPressed = (item->getState() == MenuBarButtonBoxItem::BS_PRESSED);
                    emit signalButtonClicked(item->getId(), isPressed);
                }
            }
        }
    }

    if(m_toolButtonWidgetList.contains(temp) && temp != m_toolBoxMainItem)
    {
        /*перемещаем итем*/
        int a = m_toolButtonWidgetList.removeAll(temp);
        m_toolButton->removeAction(temp->getIconActionWidget());

        MenuBarButtonBoxItem * oldMainItem = m_toolBoxMainItem;
        m_toolButton->removeAction(oldMainItem->getIconActionWidget());

        // m_toolButton->insertAction(0,temp->getIconActionWidget());
        m_toolBoxMainItem = temp;

        m_toolButton->setIcon(temp->getCurStateIcon());

        m_verticalWidgetList.prepend(oldMainItem);

        m_layout->insertWidget(1,oldMainItem->getPushButton(),0,Qt::AlignCenter);

        MenuBarButtonBoxItem * lastVerticalWidget = m_verticalWidgetList.last();

        m_layout->removeWidget(lastVerticalWidget->getPushButton());

        lastVerticalWidget->reCreatePushButton();
        lastVerticalWidget->getPushButton()->setFixedSize(m_buttonSize, m_buttonSize);
        m_verticalWidgetList.removeAll(lastVerticalWidget);
        //astVerticalWidget->getPushButton()->hide();


        m_toolButtonWidgetList.append(lastVerticalWidget);
        m_toolButton->addAction(lastVerticalWidget->getIconActionWidget());

    }

    if(0 != m_toolButton && temp == m_toolBoxMainItem)
        m_toolButton->setIcon( m_toolBoxMainItem->getCurStateIcon() );

    emit signalButtonClicked(id);

    MenuBarButtonBoxItem * item = m_allWidgets.value(id);
    bool isPressed = (id == MenuBarButtonBoxItem::BS_PRESSED);
    emit signalButtonClicked(id, isPressed);
}


void ButtonBoxWidget::slotToolButtonClicked()
{
    //if( m_toolButton->toggleViewAction == m_mainAction->)
    qDebug() << "ButtonBoxWidget::slotToolButtonClicked() ";

    if(0 != m_toolBoxMainItem->getPushButton())
    {
        if(m_toolBoxMainItem->getPushButton()->isEnabled() )
            m_toolBoxMainItem->slotClicked();
    }
}


void ButtonBoxWidget::paintEvent ( QPaintEvent * event )
{
    QPainter pr(this);

    pr.setPen(Qt::gray);
    pr.setOpacity(0.9);

    pr.drawRoundedRect(0,0, width()-2, height() - 2 /*(height() - m_buttonSize)/2*/ , 4,4);
}
