#ifndef MENUOPENBUTTONFOREGROUND_H
#define MENUOPENBUTTONFOREGROUND_H

#include <QWidget>

class MenuOpenButtonForeground : public QWidget
{
    Q_OBJECT
    QPixmap icon;
public:
    explicit MenuOpenButtonForeground(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent * pe);
signals:

public slots:
};

#endif // MENUOPENBUTTONFOREGROUND_H
