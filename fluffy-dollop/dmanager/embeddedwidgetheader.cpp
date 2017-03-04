#include "embeddedheader.h"
#include "embeddedwidgetheader.h"
#include <QApplication>
#include <QDebug>
#include <QMouseEvent>


using namespace ew;


QString EmbeddedWidgetHeader::getPinIcon()
{
    return m_pinIcon.name();
}


QString EmbeddedWidgetHeader::getUnpinIcon()
{
    return m_unpinIcon.name();
}


QString EmbeddedWidgetHeader::getLockIcon()
{
    return m_lockIcon.name();
}


QString EmbeddedWidgetHeader::getUnlockIcon()
{
    return m_unlockIcon.name();
}


QString EmbeddedWidgetHeader::getMoveOnParentIcon()
{
    return m_moveOnParentIcon.name();
}


QString EmbeddedWidgetHeader::getMoveGlobalIcon()
{
    return m_moveGlobalIcon.name();
}


void EmbeddedWidgetHeader::setPinIcon(QString url)
{
    m_pinIcon          = QIcon(url); redrawIcons();
}


void EmbeddedWidgetHeader::setUnpinIcon(QString url)
{
    m_unpinIcon        = QIcon(url); redrawIcons();
}


void EmbeddedWidgetHeader::setLockIcon(QString url)
{
    m_lockIcon         = QIcon(url); redrawIcons();
}


void EmbeddedWidgetHeader::setUnlockIcon(QString url)
{
    m_unlockIcon       = QIcon(url); redrawIcons();
}


void EmbeddedWidgetHeader::setMoveOnParentIcon(QString url)
{
    m_moveOnParentIcon = QIcon(url); redrawIcons();
}


void EmbeddedWidgetHeader::setMoveGlobalIcon(QString url)
{
    m_moveGlobalIcon   = QIcon(url); redrawIcons();
}


void EmbeddedWidgetHeader::redrawIcons()
{
    EmbeddedHeader::redrawIcons();

    if(headerButton(WHB_PINUNPIN            ))
        headerButton(WHB_PINUNPIN            )->button->setIcon(m_pinIcon);

    if(headerButton(WHB_COLLAPSE            ))
        headerButton(WHB_COLLAPSE            )->action->setIcon(m_pinIcon);

    if(headerButton(WHB_LOCKUNLOCK          ))
        headerButton(WHB_LOCKUNLOCK          )->button->setIcon(m_lockIcon);

    if(headerButton(WHB_LOCKUNLOCK          ))
        headerButton(WHB_LOCKUNLOCK          )->action->setIcon(m_lockIcon);

    if(headerButton(WHB_GLOBAL_ONPARENT_MOVE))
        headerButton(WHB_GLOBAL_ONPARENT_MOVE)->button->setIcon(m_moveGlobalIcon);

    if(headerButton(WHB_GLOBAL_ONPARENT_MOVE))
        headerButton(WHB_GLOBAL_ONPARENT_MOVE)->action->setIcon(m_moveGlobalIcon);
}


QIcon EmbeddedWidgetHeader::getButtonIcon(EmbeddedHeader::WindowHeaderButtons button)
{
    if(button == WHB_GLOBAL_ONPARENT_MOVE)
    {
        return m_moveOnParentIcon;
    }

    if(button == WHB_PINUNPIN)
    {
        return m_pinIcon;
    }

    if(button == WHB_LOCKUNLOCK)
    {
        return m_lockIcon;
    }

    return EmbeddedHeader::getButtonIcon(button); // Если не знаешь сам -- спроси у родителя
}


EmbeddedWidgetHeader::EmbeddedWidgetHeader(QWidget *parent)
    : EmbeddedHeader(parent)
{
    m_visibleFlag = true;
    m_windowPinned = false;
    m_windowLocked = false;

    setObjectName("EmbeddedWindow_EmbeddedWidgetHeader");

    m_moveOnParentIcon = QIcon(":/embwidgets/img/img/262_step_back_white.png");
    m_moveGlobalIcon   = QIcon(":/embwidgets/img/img/263_step_forth_white.png");
    m_pinIcon          = QIcon(":/embwidgets/img/img/213_pin-unpin_window_white.png");
    m_unpinIcon        = QIcon(":/embwidgets/img/img/303_pin_unpin_window(pinned)_white.png");
    m_lockIcon         = QIcon(":/embwidgets/img/img/447_key_open_white.png");
    m_unlockIcon       = QIcon(":/embwidgets/img/img/447_key_close_white.png");
    m_collapseIcon = QIcon(":/embwidgets/img/img/icon_5.png");
}


void EmbeddedWidgetHeader::setLocked(bool locked)
{
    m_windowLocked = !locked;
    emit headerButton(WHB_LOCKUNLOCK)->button->click();
}


void EmbeddedWidgetHeader::setPinned(bool pinned)
{
    m_windowPinned = !pinned;
    emit headerButton(WHB_PINUNPIN)->button->click();
    //    emit headerButton(WHB_PINUNPIN)->button->click();
}


void EmbeddedWidgetHeader::setWidgetMaximized(bool maximized)
{
    if(m_windowMaximized != maximized)
    {
        headerButton(WHB_FULLNORMSCREEN)->button->click();
        //m_windowMaximized = maximized;
    }
}


void EmbeddedWidgetHeader::setGlobalMove(bool globalMove)
{
    m_windowPinned = !globalMove;
//    emit m_pGlobalMoveButton->click();
    emit headerButton(WHB_GLOBAL_ONPARENT_MOVE)->button->click();
}


void EmbeddedWidgetHeader::slotButtonClicked(bool state)
{
    EmbeddedHeader::slotButtonClicked(state);

    bool acceptFlag = false;


//    if(headerButton(WHB_MIDGET))
//    {
//        if((sender() == headerButton(WHB_MIDGET)->button)
//          )
//        {
//            emit(signalWindowEvent(WHE_MIDGET,&acceptFlag));
//            qDebug()<<QString::fromLocal8Bit("Свернуть в миджет");
//        }
//    }

    if(headerButton(WHB_PINUNPIN))
    {
        if(sender() == headerButton(WHB_PINUNPIN)->button) //TODO: Добавить сравнение с экшеном
        {
            if(m_windowPinned)
            {
                emit signalWindowEvent(WHE_UNPIN, &acceptFlag);
            }
            else
            {
                emit signalWindowEvent(WHE_PIN, &acceptFlag);
            }

//            if(acceptFlag) //FIXME: Не приходит подтверждение
            {
                if(m_windowPinned)
                {
                    headerButton(WHB_PINUNPIN)->button->setIcon(m_pinIcon);
                    m_windowPinned = false;
                }
                else
                {
                    headerButton(WHB_PINUNPIN)->button->setIcon(m_unpinIcon);
                    m_windowPinned = true;
                }
            }
        }
    }

    if(headerButton(WHB_LOCKUNLOCK))
    {
        if(sender() == headerButton(WHB_LOCKUNLOCK)->button)//TODO: Добавить сравнение с экшеном
        {
            if(m_windowLocked)
            {
                emit signalWindowEvent(WHE_UNLOCK, &acceptFlag);
            }
            else
            {
                emit signalWindowEvent(WHE_LOCK, &acceptFlag);
            }

//            if(acceptFlag) //FIXME: Не приходит подтверждение
            {
                if(m_windowLocked)
                {
                    headerButton(WHB_LOCKUNLOCK)->button->setIcon(m_lockIcon);
                    m_windowLocked = false;
                }
                else
                {
                    headerButton(WHB_LOCKUNLOCK)->button->setIcon(m_unlockIcon);
                    m_windowLocked = true;
                }
            }
        }
    }

    if(headerButton(WHB_GLOBAL_ONPARENT_MOVE))
    {
        if(sender() == headerButton(WHB_GLOBAL_ONPARENT_MOVE)->button)//TODO: Добавить сравнение с экшеном
        {
            if(m_globalMoved)
            {
                emit signalWindowEvent(WHE_MOVE_ON_PARENT, &acceptFlag);
            }
            else
            {
                emit signalWindowEvent(WHE_GLOBAL_MOVE, &acceptFlag);
            }

            if(acceptFlag)
            {
                if(m_globalMoved)
                {
                    headerButton(WHB_LOCKUNLOCK)->button->setIcon(m_moveGlobalIcon);
                    m_globalMoved = false;
                }
                else
                {
                    headerButton(WHB_LOCKUNLOCK)->button->setIcon(m_moveOnParentIcon);
                    m_globalMoved = true;
                }
            }
        }
    }
}


void EmbeddedWidgetHeader::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    foldHeaderButtons();
}
