#ifndef EMBEDDEDHEADER_H
#define EMBEDDEDHEADER_H
#include <QIcon>
#include <QMap>
#include <QMenu>
#include <QWidget>


class QLabel;
class QPushButton;
class QHBoxLayout;
#include "curtaillabel.h"
#include "runninglabel.h"

namespace ew {
class EmbeddedHeader : public QWidget
{
    Q_OBJECT
    friend class EmbeddedWidget;
    friend class EmbeddedWindow;
    friend class EmbeddedMainWindow;

    Q_PROPERTY(QString closeIcon WRITE setCloseIcon READ getCloseIcon   )
    Q_PROPERTY(QString maxIcon WRITE setMaxIcon READ getMaxIcon     )
    Q_PROPERTY(QString minIcon WRITE setMinIcon READ getMinIcon     )
    Q_PROPERTY(QString collapseIcon WRITE setCollapseIcon READ getCollapseIcon)
    Q_PROPERTY(QString appIcon WRITE setAppIcon READ getAppIcon     )
    Q_PROPERTY(QString whatThisIcon WRITE setWhatThisIcon READ getWhatThisIcon)
    Q_PROPERTY(QString menuIcon WRITE setMenuIcon READ getMenuIcon    )
    Q_PROPERTY(QString styleIcon WRITE setStyleIcon READ getStyleIcon   )
    Q_PROPERTY(QString settingsIcon WRITE setSettingsIcon READ getSettingsIcon)
    Q_PROPERTY(QString hideHeaderIcon WRITE setHideHeaderIcon READ getHideHeaderIcon)

protected:
    bool m_windowMoveMode; // если тру, то окно можно перемещать
    bool m_windowMaximized;
    bool m_denyMove;
    bool m_visibleFlag;
    int m_buttonSize;
    QPoint m_pressedLocalLastPos;
    QPoint m_pressedGlobalLastPos;
    QString m_descrStr;
    bool m_cursorChanged;
    QIcon m_maxIcon,
          m_minIcon,
          m_closeIcon,
          m_collapseIcon,
          m_appIcon,
          m_whatThisIcon,
          m_menuIcon,
          m_styleIcon,
          m_settingsIcon,
          m_midgetIcon,
          m_hideHeaderIcon,
          m_optimizePosIcon,
          m_optimizeSpaceIcon;

protected:
    QString      getCloseIcon(){return m_closeIcon.name(); }


    QString      getMaxIcon(){return m_maxIcon.name(); }


    QString      getMinIcon(){return m_minIcon.name(); }


    QString      getCollapseIcon(){return m_collapseIcon.name(); }


    QString      getAppIcon(){return m_appIcon.name(); }


    QString      getWhatThisIcon(){return m_whatThisIcon.name(); }


    QString      getMenuIcon(){return m_menuIcon.name(); }


    QString      getStyleIcon(){return m_styleIcon.name(); }


    QString      getSettingsIcon(){return m_settingsIcon.name(); }


    QString      getHideHeaderIcon(){return m_hideHeaderIcon.name(); }

    void         setCloseIcon(QString icon);
    void         setMaxIcon(QString icon);
    void         setMinIcon(QString icon);
    void         setCollapseIcon(QString icon);
    void         setAppIcon(QString icon);
    void         setWhatThisIcon(QString icon);
    void         setMenuIcon(QString icon);
    void         setStyleIcon(QString icon);
    void         setSettingsIcon(QString icon);
    void         setMidgetIcon(QString icon);
    void         setHideHeaderIcon(QString icon);
    virtual void redrawIcons();

    CurtailLabel *m_pTytleLabel;
    QLabel *m_pAppIconLabel;
    QHBoxLayout *m_headerLayout;
    QHBoxLayout *m_pButtonLayout;
    QPushButton *m_pFoldedButtonsBtn;   // Кнопка свернутых кнопок.
    QMenu *m_pFoldedButtonsMenu;  // Меню свернутых кнопок. Сюда попадают кнопки, не поместившиеся на заголовке.
    virtual int getButtonSpace();       // возвращает ту ширину, которая может быть занята кнопками
    bool        checkNoVisibleActions();// Если меню свернутых кнопок пусто


    virtual void init() {};
    void loadIcons();

public:
    enum MovingState
    {
        MS_START = 0,
        MS_MOVE,
        MS_END
    };

    ///
    /// \brief The WindowHeaderButtons enum
    /// Кнопки записываютсфя в порядке уменьшения значимости. Чем раньше стоит кнопка, тем дольше она не будет скрыта
    ///
    enum WindowHeaderButtons
    {
        WHB_CLOSE = 0,
        WHB_FULLNORMSCREEN,
        WHB_COLLAPSE,
        WHB_WHAT_THIS,
        WHB_GLOBAL_ONPARENT_MOVE,
        WHB_HIDE_HEADER,
        WHB_LOCKUNLOCK,
        WHB_MIDGET,
        WHB_PINUNPIN,
        WHB_SETTINGS,
        WHB_STYLE,
        WHB_OPTIMIZE_SPACE, ///<оптимизация свободного пространства
        WHB_OPTIMIZE_SIZES ///<оптимизация положения
    };
    void createWindowHeaderButton
        (WindowHeaderButtons button //Тип кнопки. (На заголовок не более одной кнопки каждого типа)
        , QIcon icon
        , QString descryption
        , bool isFoldable
        );
    virtual QIcon   getButtonIcon(WindowHeaderButtons button);
    virtual QString getButtonWhatThis(WindowHeaderButtons button);
    virtual QString getButtonText(WindowHeaderButtons button, bool state = false);
    void            deleteHeaderButton(WindowHeaderButtons button);



    enum WindowHeaderEvents
    {
        WHE_CLOSE = 0,
        WHE_SHOW_NORMAL,
        WHE_MAXIMIZE,
        WHE_COLLAPSE,
        WHE_START_MOVE,
        WHE_END_MOVE,
        WHE_PIN,
        WHE_UNPIN,
        WHE_LOCK,
        WHE_UNLOCK,
        WHE_GLOBAL_MOVE,
        WHE_MOVE_ON_PARENT,
        WHE_SHOW_STYLE_WIDGET,
        WHE_SHOW_SETTINGS,
        WHE_MIDGET,
        WHE_HIDE_HEADER,
        WHE_OPTIMIZE_SPACE,
        WHE_OPTIMIZE_SIZES
    };

    explicit EmbeddedHeader(QWidget *parent = 0);
    ~EmbeddedHeader();
    QString text();
    void    setText(QString str);
    void    setToolTip(QString str);
    void    setIcon(QIcon ic);
    void    setIcon(QPixmap pm);
    void    setDescription(QString str);
    void    setVisisbleButton(WindowHeaderButtons button, bool vis);
    void    setVisibleFlag(bool vis);
    bool    isVisibleFlagSetted();
    bool    getIsRunningHeader() const;
    void    setIsRunningHeader(bool isRunningHeader);

//    QString tytle();

protected:
    void         mousePressEvent(QMouseEvent *e);
    void         mouseMoveEvent(QMouseEvent *e);
    void         mouseReleaseEvent(QMouseEvent *e);
    void         resizeEvent(QResizeEvent *event);
    void         mouseDoubleClickEvent(QMouseEvent *event);
    virtual void checkCursor();
    virtual void minMaxWidget(bool *acceptFlag);

signals:
    void signalWindowEvent(EmbeddedHeader::WindowHeaderEvents ev, bool *acceptFlag);
    void signalMoveWindowRequest(int dx, int dy, bool *acceptFlag);
    void signalWindowDropped();

protected slots:
    virtual void slotButtonClicked(bool);
    void         slotFoldedButtonsBtnClicked();
    virtual void foldHeaderButtons();

protected:
    struct HeaderButton
    {
        QPushButton *button;
        QAction *action;     //
        bool isFoldable;         // Может сворачиваться.
        HeaderButton()
        {
            button   = 0;
            action   = 0;
            isFoldable = true;
        }
    };
    HeaderButton *headerButton(WindowHeaderButtons button);

private:
    // Руками не трогать.
    QMap<WindowHeaderButtons, HeaderButton *> m_foldableButtons;
};
}

#endif // EMBEDDEDHEADER_H
