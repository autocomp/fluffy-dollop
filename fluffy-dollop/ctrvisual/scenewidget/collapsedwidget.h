#ifndef RECORDWIDGET_H
#define RECORDWIDGET_H

#include <QDialog>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <QMouseEvent>

class CollapsedWidget : public QDialog
{
    Q_OBJECT

    bool m_visible;
    QWidget * m_topWidget;
    QVBoxLayout * m_pMainPanel;
    bool m_windowMoveMode, m_windowPrepareToMove;
    QLabel * m_pTitleLbl;
    QPoint m_pressedLocalLastPos, m_pressedGlobalLastPos;
    QWidgetList m_insertedWidgets;
    bool m_isMinimized;
    QIcon m_minIcon, m_maxIcon, m_closeIcon, m_pinIcon, m_unpinIcon, m_lockIcon, m_unlockIcon;
    QIcon m_onParentIcon, m_onGlobalIcon;
    QPushButton * m_pMinimizeBtn, *m_pCloseBtn, *m_pPinButton, *m_pGlobalMoveBtn, *m_pLockButton;
    int m_mainPanelMargin, m_mainPanelSpacing;
    const int m_headerHeight;
    QVBoxLayout * m_pMainLayout;
    QSize m_prevSize;

    bool m_moveOnlyOnParent;
    QWidget * m_pLastParent;
    int m_pressedX, m_pressedY;
    quint64 m_id;
    QSize m_defaultFixedSize;

public:
    explicit CollapsedWidget(QWidget *parent = 0);
    void setWindowTitleText(QString title);
    void expand();
    void collapse();
    void removeWidget(QWidget * widget);
    void insertWidget(QWidget * widget);
    void setBorders(int margins, int spacings);
    void hide();
    void show();
    void setLocked(bool isLocked);
    void setVisisbleLockButton(bool vis);
    void setVisisbleCollapseButton(bool vis);
    void setVisisblePinButton(bool vis);
    void setVisisbleCloseButton(bool vis);
    void setVisibleHeader(bool vis);
    void setPinned(bool pinned);
    void moveOnlyOnParent(bool on);
    void setVisible(bool visible);
    bool isVisible();
    void setId(quint64 id);
    void setDefaultFixedSize(QSize sz);
    QSize defaultFixedSize();

    int getTopMargin();
    int getBottomMargin();
    int getRightMargin();
    int getLeftMargin();

    bool isPinned();

protected:

    bool eventFilter(QObject *obj, QEvent *ev);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *e);
    void resizeEvent(QResizeEvent *e);
    void hideEvent(QResizeEvent *e);
    void showEvent(QResizeEvent *e);
    void keyPressEvent(QKeyEvent *e);

private slots:
    void slotGlobalMove();
    void slotMinMaxWindow();
    void slotPinWindow();
    void slotCloseWindow();
    void slotLockWindow();
signals:
    void signalVisibleNeedChangeInternal(bool);
    void signalHideWindow();
    void signalSizeChanged(QSize size);
    void signalDragGlobPosChanged(QRect, QPoint,QMouseEvent::Type);
    void signalRemoveMe();
    void signalVisibleNeedChange(bool);
    void signalPosChanged(quint64 id, QPoint p);
    void signalLockCollapsedWidget(bool);
    void signalPinCollapsedWidget(bool);
    
public slots:
    
};

#endif // RECORDWIDGET_H
