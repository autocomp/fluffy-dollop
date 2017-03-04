#ifndef MENUOPENBUTTONWIDGET_H
#define MENUOPENBUTTONWIDGET_H

#include <QWidget>

#include <QTimer>
#include <QList>
#include <QLayout>
#include <QStateMachine>
#include <QPropertyAnimation>

class MenuOpenButtonWidget : public QWidget
{
    Q_OBJECT
    QPixmap m_backgroundPixmap;
    bool m_needUpdateBackground;

private:
    QTimer *showTimer, *opacityLessTimer;
    int m_startHeight;
    bool m_needShow, m_needHide;
    int m_minSize ;
    int widthBar, heightBar;
    int menuType;
    QPixmap  icon;
    QStateMachine machine_;
    QState* st1 = nullptr;
    QState* st2 = nullptr;
    QPropertyAnimation* an1 = nullptr;

protected:
    void mousePressEvent(QMouseEvent *);
    //void paintEvent(QPaintEvent *);

public:
    explicit MenuOpenButtonWidget(QWidget *parent = 0);
    void setBackgroundImage(QPixmap pm);
    void setSize(int width, int height);
    void setHeight(int height);
    void setViewMenuOpenButtonWidget();

    ~MenuOpenButtonWidget();

signals:
    void needHide();
    void needShow();
public slots:
    void show();
    void hide();

};

#endif // MENUOPENBUTTONWIDGET_H
