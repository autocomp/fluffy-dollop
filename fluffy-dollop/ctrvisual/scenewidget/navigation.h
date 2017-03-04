#ifndef NAVIGATION_H
#define NAVIGATION_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QButtonGroup>
#include <QPushButton>
#include <QHBoxLayout>

/**
 * @brief Класс виджета, управляющего режимами навигации
 *
 */
class Navigation : public QWidget
{
    Q_OBJECT

public:
    explicit Navigation(QWidget *parent = 0);
    ~Navigation();

    void            setBackgroundImage(QPixmap & pm);
    int             getCurrentIdNavigation(){return currentIdNavigation;}
    void            setCurrentNavigation( int id );

private:
    void            paintEvent(QPaintEvent *);
    bool            eventFilter(QObject *obj, QEvent *event);

    QPixmap         m_backgroundPixmap;
    bool            m_needUpdateBackground;

    bool            m_visible;
    QButtonGroup    *bg;
    QPushButton     *pb1, *pb2, *pb3, *pb4;
    QHBoxLayout     *bgroup;
    int             currentIdNavigation;

signals:
    void            currentNavigation(int);
    void            buttonNavigation();
    void            signalResendUnusedEvents(QEvent * e);

private slots:
    void            slotCurrentButtonPress(int id);
    void            slotButtonPress();

};

#endif
