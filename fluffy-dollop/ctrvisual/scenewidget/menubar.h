#ifndef MENUBAR_H
#define MENUBAR_H

#include <QWidget>

#include <QTimer>
#include <QList>
#include <QLayout>
#include <QStateMachine>
#include <QPropertyAnimation>


/**
 * @brief Класс верхнего выезжающего меню
 *
 */
class MenuBar : public QWidget
{
    Q_OBJECT

private:

    QTimer *showTimer, *opacityLessTimer;
    int m_startHeight;
    bool m_needShow, m_needHide;

    int m_minSize ;
    int widthBar, heightBar, m_margins;
    QVBoxLayout * m_pMainLt ;
    int menuType;
    bool m_barInited;

    int m_leftMargin;

    QMap<int, QHBoxLayout*> m_layouts;
    QMap<int, QWidgetList> m_list;
    QMap<int, QWidget*> m_visibleContainerWidgets;

    QList<int> m_visibleLines;

    QPixmap m_closeIcon;

    QStateMachine machine_;

    QState* st1, *st2;
    QPixmap m_backgroundPixmap;
    bool m_needUpdateBackground;

    int newWidth;
    QPropertyAnimation* an1 = nullptr;

protected:
    void mousePressEvent(QMouseEvent *e);
    //void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent * e);

public:

    enum MenuBarLines
    {
        MBL_MAIN = 0
        //MBL_SEARCH

    };

    explicit MenuBar(QWidget *parent = 0);
    void setFixedWidth(int w);
    void setHeight(int h);
    void setWidth(int w);
    void setBackgroundImage(QPixmap & pm);
    ~MenuBar();



    /**
     * @brief Функция скрывает или отображаает строку
     * @param lineNum - номер строки меню, видимостью которой управляем
     * @param vis - видимость
     */
    void setVisibleLine(MenuBar::MenuBarLines lineNum, bool vis);


    /**
     * @brief Добавляет виджет на панель меню.
     * @param menuItem - указатель на добавляемый виджет
     * @param stretch - степень растяжения
     * @param Qt::Alignment - выравнивание
     */
    void addItem(QWidget * menuItem, int stretch = 0, Qt::Alignment al = Qt::AlignCenter, int line = 0);

    void insertItem(QWidget * menuItem, int pos = 0, int stretch = 0, Qt::Alignment al = Qt::AlignLeft, int line = 0 );
    /**
     * @brief Добавляет stretch в панель задач
     * @param stretch
     */
    void addStretch(int stretch = 0, int line = 0);

    /**
     * @brief Добавляет вертикальную черту на панель задач
     */
    void addVerticalLine(int lineNum = 0);

    void show();
    void hide();

    int getBarWidth(){return widthBar;}

    void setBarWidth();

    int prefferWidth();
    void setViewMenuBar();

signals:
    /**
     * @brief Для внутреннего использования
     */
    void needShow();
    /**
     * @brief Для внутреннего использования
     */
    void needHide();

    /*оповещение родителя о необходимости изменения размеров*/
    void signalResizeParent();
protected slots:
    
};

#endif // MENUBAR_H
