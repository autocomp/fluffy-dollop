#ifndef VIRAINFOWIDGET_H
#define VIRAINFOWIDGET_H

#include <QWidget>

namespace Ui {
class ViraInfoWidget;
}

class ViraInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ViraInfoWidget(QWidget *parent = 0);
    ~ViraInfoWidget();

protected:
    void showEvent( QShowEvent* ev );

private:
    void appendPhotos();

    Ui::ViraInfoWidget *ui;
};

#endif // VIRAINFOWIDGET_H
