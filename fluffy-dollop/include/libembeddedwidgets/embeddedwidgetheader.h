#ifndef EMBEDDEDWIDGETHEADER_H
#define EMBEDDEDWIDGETHEADER_H

#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include "embeddedheader.h"

namespace ew {
class EmbeddedWidgetHeader : public EmbeddedHeader
{
    Q_OBJECT
    friend class ScrollPanel;

    bool m_windowPinned;
    bool m_windowLocked;
    bool m_globalMoved;
    QIcon m_pinIcon;
    QIcon m_unpinIcon;
    QIcon m_lockIcon;
    QIcon m_unlockIcon;
    QIcon m_moveOnParentIcon;
    QIcon m_moveGlobalIcon;


    Q_PROPERTY(QString pinIcon WRITE setPinIcon READ getPinIcon         )
    Q_PROPERTY(QString unpinIcon WRITE setUnpinIcon READ getUnpinIcon       )
    Q_PROPERTY(QString lockIcon WRITE setLockIcon READ getLockIcon        )
    Q_PROPERTY(QString unlockIcon WRITE setUnlockIcon READ getUnlockIcon      )
    Q_PROPERTY(QString moveOnParentIcon WRITE setMoveOnParentIcon READ getMoveOnParentIcon)
    Q_PROPERTY(QString moveGlobalIcon WRITE setMoveGlobalIcon READ getMoveGlobalIcon  )
    QString getPinIcon();
    QString getUnpinIcon();
    QString getLockIcon();
    QString getUnlockIcon();
    QString getMoveOnParentIcon();
    QString getMoveGlobalIcon();
    void    setPinIcon(QString url);
    void    setUnpinIcon(QString url);
    void    setLockIcon(QString url);
    void    setUnlockIcon(QString url);
    void    setMoveOnParentIcon(QString url);
    void    setMoveGlobalIcon(QString url);
    void    redrawIcons();
    QIcon   getButtonIcon(WindowHeaderButtons button);

public:
    explicit EmbeddedWidgetHeader(QWidget *parent = 0);
    void setButtonVisible();
    void setLocked(bool locked);
    void setPinned(bool pinned);
    void setWidgetMaximized(bool maximized);
    void setGlobalMove(bool globalMove);

protected slots:
    virtual void slotButtonClicked(bool state);
    virtual void resizeEvent(QResizeEvent *event);
};
}

#endif // EMBEDDEDWIDGETHEADER_H
