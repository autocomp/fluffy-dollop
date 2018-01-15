#include <QApplication>

#include "mainwindow.h"

int main(int argc, char **argv )
{
    QApplication app(argc, argv);

    MainWindow *window = new MainWindow;
    // ok, run the GUI...
    window->show();

    // event loop
    return app.exec();
}
