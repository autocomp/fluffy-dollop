#ifndef EMBEDDEDWIDGET_H
#define EMBEDDEDWIDGET_H

#include <QDialog>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>


#include "embeddedstruct.h"
#include "embeddedwidgetheader.h"

namespace ew {
class EmbeddedPanel;

class EmbeddedWidget : public QDialog
{
    Q_OBJECT

    friend class EmbeddedWindow;
    friend class EmbeddedMainWindow;
    friend class EmbeddedApp;


    Q_PROPERTY(QString activeWinBorderColor WRITE setActiveWinBorderColor READ getActiveWinBorderColor)
    Q_PROPERTY(QString activeWinBorderSize WRITE setActiveWinBorderSize READ getActiveWinBorderSize)

protected:
    EmbeddedPanel *m_pViewPanel;
    const QSize m_minDefaultSize;
    QWidget *m_pLastParent;
    QFrame *m_pSeparatorFrame;
    quint64 m_id; // необходим для быстрого поиска структур в EmbeddedWindow
    HighlightSection m_highlightedSection;
    QColor m_highlightColor;
    int m_highlightWidth;
    QString m_installedStylesheet;
    QTimer *m_pAdjustSizeTimer = 0;


    /*состояние окна*/
    bool m_windowMoveMode, m_windowResizeMode;
    QWidget *m_mainViewWidget, *m_pMainViewWidgetBackground;
    QVBoxLayout *m_pMainViewWidgetLayout;
    bool m_denyHide;
    bool m_isGlobalMove;
    bool m_isPinned;
    bool m_isCollapsed;
    bool m_isLocked;
    bool m_isVisible;
    QSize m_minSize, m_maxSize, m_prevSize;
    EmbeddedHeader *m_pHeaderWidget;
    QVBoxLayout *m_pMainLayout;
    enum ActiveBound
    {
        AB_NO_ACTIVE_BOUND =0,
        AB_LEFT,
        AB_RIGHT,
        AB_TOP,
        AB_BOTTOM,
        AB_TOPLEFT,
        AB_TOPRIGHT,
        AB_BOTTOM_RIGHT,
        AB_BOTTOM_LEFT,
    } m_activeBound;
    bool m_isActive;
    bool m_moveAllowed;
    bool m_resizeAllowed;
    bool m_resizeEnabled;
    bool m_resizeInProcess = false;
    QColor m_activeWinBorderColor;
    int m_activeWinBorderSize;
    QTimer *m_pResizeTimer;
    QPoint m_pressedGlobalLastPos;
    int m_borderSize, m_moveBoundSize;
    QCursor *m_pSavedCursor;

    explicit EmbeddedWidget(QWidget *parent = 0);
    ~EmbeddedWidget();

public:
    QString                    getWindowTitleText();
    virtual void               setWindowTitleText(QString title);
    virtual void               setHeader(const EmbeddedHeaderStruct & headerStruct);
    ew::EmbeddedWidgetHeader  *header();
    void                       setCollapsed(bool isCollapsed);
    bool                       isCollapsed();
    void                       setVisible(bool visible);
    bool                       isVisible();
    void                       setPinned(bool pinned);
    bool                       isPinned();
    void                       setLocked(bool isLocked);
    bool                       isLocked();
    void                       setGlobalMove(bool isGlobalMove);
    bool                       isGlobalMove();
    void                       setResizeAllowed(bool allowed);
    bool                       isResizeAllowed();
    void                       setMoveAllowed(bool allowed);
    bool                       isMoveAllowed();
    void                       setHeaderVisisbility(bool headerVis);
    bool                       isHeaderVisible();
    void                       setWidgetMaximized(bool isMaximized);
    virtual QRect              getViewRect();
    void                       setMinMaxSize(QSize minSize, QSize maxSize);
    virtual void               removeWidget(QWidget *widget);
    virtual void               setWidget(QWidget *widget);
    void                       setId(quint64 id);
    quint64                    id() const;
    QSize                      getViewSize();
    void                       setVisibleHeader(bool vis);
    QSize                      defaultFixedSize();
    void                       denyHide(bool isDenyHide);
    virtual EmbeddedWidgetType widgetType();

    /**
     * @brief Фиктивная активация виджета(обрисовка его рамкой).
     * @param isActive - true - если виджет является активным
     */
    void                 setActiveWidget(bool isActive);
    virtual void         setHighlightedSection(ew::HighlightSection section);
    ew::HighlightSection highlightedSection();
    HighlightSection     checkBorders(QRect rect);

    /**
     * @brief getCardinalDirection Получение подсветки (стороны света)
     * @param localPos
     * @return
     */
    HighlightSection getCardinalDirection(QPoint localPos) const;

    /**
     * @brief Метод возвращает расстояния от  краев view области до краев виджета
     * @return
     */
    virtual QMargins getEmbeddedViewMargins();
    QWidget         *getWidget();

protected:
    void             setActiveWinBorderSize(QString szString);
    void             setActiveWinBorderColor(QColor col);
    QString          getActiveWinBorderSize();
    QString          getActiveWinBorderColor();
    void             installStyle();
    void             installStyle(QString styleSheet);
    void             installStyleFile(const QString & filename);
    bool             eventFilter(QObject *obj, QEvent *ev);
    void             mousePressEvent(QMouseEvent *e);
    void             mouseMoveEvent(QMouseEvent *e);
    void             mouseReleaseEvent(QMouseEvent *e);
    void             paintEvent(QPaintEvent *e);
    void             resizeEvent(QResizeEvent *e);
    void             hideEvent(QHideEvent *e);
    void             showEvent(QShowEvent *e);
    void             setHeaderVisisbilityPrivate(bool headerVis);
    void             keyPressEvent(QKeyEvent *e);
    virtual QMargins getMargins();

protected slots:
    virtual void checkResize();
    void         slotResizeCheckTimeout();

    /**
     * @brief Слот вызывается при уничтожении окна.
     * @param obj
     */
    virtual void slotWidgetDestroyed(QObject *obj = 0);

signals:
    void signalWidgetClosed();
    void signalVisibleNeedChangeInternal(bool);
    void signalHideWindow();
    void signalSizeChanged(QSize size);

    void signalDragGlobPosChanged(QRect, QPoint, QMouseEvent::Type);
    void signalVisibleNeedChange(bool);
    void signalLockCollapsedWidget(bool);

    void signalCollapseWidget(quint64, bool);
    void signalWidgetMovePolicyChanged(quint64, bool);
    void signalPinCollapsedWidget(bool);
    void signalTitleChanged(QString title);
    void signalWindowDragged(QRect geometry);
    void signalWindowDropped(QRect rect);
    void signalAdjustWidgetPrivate();

public slots:
    virtual void slotHeaderWindowEvents(EmbeddedHeader::WindowHeaderEvents ev, bool *acceptFlag);//NOTE: Добавил слот обработки событий хедера
    virtual void slotMoveWindowRequest(int dx, int dy, bool *acceptFlag);//NOTE: Добавил слот обработки событий перетаскивания
    void         onSignalWindowDropped();

private slots:
    void slotAdjustWidgetPrivate();
};
}

#endif // EMBEDDEDWIDGET_H
