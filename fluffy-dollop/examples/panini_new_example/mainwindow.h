#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <libpanini/pvQtView.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_show_clicked();

    void on_pushButton_disable_clicked();

private:
    Ui::MainWindow *ui;

    pvQtView* panini_window;
};

#endif // MAINWINDOW_H
