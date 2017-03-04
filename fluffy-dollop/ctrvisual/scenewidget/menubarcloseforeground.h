#ifndef MENUBARCLOSEFOREGROUND_H
#define MENUBARCLOSEFOREGROUND_H

#include <QWidget>

class MenuBarCloseForeground : public QWidget
{
    Q_OBJECT
    QPixmap icon;
public:
    explicit MenuBarCloseForeground(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent * pe);
signals:

public slots:
};

#endif // MENUBARCLOSEFOREGROUND_H
