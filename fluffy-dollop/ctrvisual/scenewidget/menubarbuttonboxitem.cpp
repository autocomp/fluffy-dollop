#include "menubarbuttonboxitem.h"
#include <QDebug>
#include <QLayout>
MenuBarButtonBoxItem::MenuBarButtonBoxItem(QIcon & iconForPressedState, QIcon & iconForReleasedState, QString &text, int id, bool isTriggerType, QString toolTipText, QWidget *parent) :
    QObject(parent)
{
    m_id = id;
    m_text = text;
    m_iconForPressedState = iconForPressedState;
    m_iconForReleasedState = iconForReleasedState;
    m_iconActionWidget= new IconActionWidget(iconForReleasedState,text,parent);
    m_button = new MenuButtonItem();
    m_triggerType = isTriggerType;

    m_button->setIcon(iconForReleasedState);
//    m_button->layout()->setMargin(0);
//    m_button->layout()->setSpacing(0);
    //m_button->setContentsMargins(0,0,0,0);
    m_curIcon = iconForReleasedState;


    m_state = BS_RELEASED;

    m_toolTipText = toolTipText;
    m_button->setToolTip(m_toolTipText);

    connect(m_button, SIGNAL(clicked()), this, SLOT(slotClicked()));
    connect(m_iconActionWidget, SIGNAL(triggered()), this,SLOT(slotClicked()));


}


void MenuBarButtonBoxItem::setVisible(bool isVis)
{
//    m_button->hide();
    m_iconActionWidget->setVisible(isVis);
    m_button->setVisible(isVis);
}

void MenuBarButtonBoxItem::setButtonStateWithoutEmit(BUTTON_STATE state)
{
    if(state != m_state)
    {
        //m_state = state;

        if(m_triggerType)
        {
            if(BS_PRESSED == state)
            {
                m_state = BS_PRESSED;
                m_curIcon = m_iconForPressedState;
            }
            else
            {
                m_state = BS_RELEASED;
                m_curIcon = m_iconForReleasedState;
            }
        }
        else
        {
            m_state = BS_RELEASED;
            m_curIcon = m_iconForReleasedState;
        }

        m_button->setIcon(m_curIcon);
        m_iconActionWidget->setIcon(m_curIcon);

    }
}

void MenuBarButtonBoxItem::setButtonState(BUTTON_STATE state)
{
    if(state != m_state)
    {
        setButtonStateWithoutEmit(state);
        emit btnClicked(getId());
    }
}


void MenuBarButtonBoxItem::setIcon(QIcon & icon, BUTTON_STATE state)
{
    if(state == BS_PRESSED)
    {
        m_iconForPressedState = icon;
        m_button->setIcon(icon);

        m_iconActionWidget->setIcon(icon);
    }
    else
    {
        m_button->setIcon(icon);
        m_iconActionWidget->setIcon(icon);
        m_iconForReleasedState = icon;
    }
}

void MenuBarButtonBoxItem::setButtonType(bool triggerType)
{
    m_triggerType = triggerType;
}

MenuBarButtonBoxItem::BUTTON_STATE MenuBarButtonBoxItem::getState()
{
    return m_state;
}

QIcon MenuBarButtonBoxItem::getIcon( BUTTON_STATE state)
{
    if( BS_PRESSED == state )
    {
        return m_iconForPressedState;
    }
    else
    {
        return m_iconForReleasedState;
    }
}


QIcon MenuBarButtonBoxItem::getCurStateIcon()
{
    if( BS_PRESSED == m_state )
    {
        return m_iconForPressedState;
    }
    else
    {
        return m_iconForReleasedState;
    }
}


void MenuBarButtonBoxItem::reCreatePushButton()
{
    bool isEnabled;

    if(0 != m_button)
    {
        isEnabled = m_button->isEnabled();
    }

    delete m_button;

    m_button = new MenuButtonItem();
    m_button->setEnabled(isEnabled);
    connect(m_button, SIGNAL(clicked()), this, SLOT(slotClicked()));

    if(BS_PRESSED == m_state)
    {
        m_button->setIcon(m_iconForPressedState);
        m_button->setToolTip(m_toolTipText);
    }
    else
    {
        m_button->setIcon(m_iconForReleasedState);
        m_button->setToolTip(m_toolTipText);
    }

    m_button->setToolTip(m_toolTipText);
}



MenuBarButtonBoxItem::~MenuBarButtonBoxItem()
{
    //if(0 == m_button->parent())
        delete m_button;
    //if(0 == m_iconActionWidget->parent())
        delete m_iconActionWidget;

}

bool MenuBarButtonBoxItem::isTriggerType()
{
    return m_triggerType;
}

void MenuBarButtonBoxItem::setDisactive(bool flag)
{
    if(0 != m_button)
        m_button->setDisabled(flag);

    if(0 != m_iconActionWidget)
    {
        m_iconActionWidget->setEnabled(flag);
    }
}

void MenuBarButtonBoxItem::resetState()
{
    qDebug() << "MenuBarButtonBoxItem::resetState()";

    if(m_triggerType)
    {
        if(BS_RELEASED != m_state)
        {
            m_state = BS_RELEASED;
            m_curIcon = m_iconForReleasedState;
        }
    }
    else
    {
        m_state = BS_RELEASED;
        m_curIcon = m_iconForReleasedState;
    }

    m_button->setIcon(m_curIcon);
    m_iconActionWidget->setIcon(m_curIcon);

    emit btnClicked(getId());
}

void MenuBarButtonBoxItem::slotClicked()
{
    qDebug() << "MenuBarButtonBoxItem::slotClicked()";

    if(m_button->isEnabled())
    {
        if(m_triggerType)
        {

            if(BS_RELEASED == m_state)
            {
                m_state = BS_PRESSED;
                m_curIcon = m_iconForPressedState;
            }
            //HARDCODE
            else if((getId()==31)||(getId()==32)||(getId()==33))
            {
                if(BS_PRESSED == m_state)
                {
                    m_state = BS_PRESSED;
                    m_curIcon = m_iconForPressedState;
                }
            }
            else
            {
                m_state = BS_RELEASED;
                m_curIcon = m_iconForReleasedState;
            }
        }
        else
        {
            m_state = BS_RELEASED;
            m_curIcon = m_iconForReleasedState;
        }

        m_button->setIcon(m_curIcon);
        m_iconActionWidget->setIcon(m_curIcon);
    }

    // emit clicked();
    emit btnClicked(getId());
}


MenuButtonItem * MenuBarButtonBoxItem::getPushButton()
{
    return m_button;
}


IconActionWidget * MenuBarButtonBoxItem::getIconActionWidget()
{
    return m_iconActionWidget;
}


QString MenuBarButtonBoxItem::getText()
{
    return m_text;
}


int MenuBarButtonBoxItem::getId()
{
    return m_id;

}
