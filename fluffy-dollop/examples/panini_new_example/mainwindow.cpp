#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    panini_window = new pvQtView(  );
    //ui->verticalLayout_in->addWidget( panini_window );
    panini_window->show();
    bool gl_ok = panini_window->OpenGLOK();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_show_clicked()
{
    pvQtPic* pic = new pvQtPic();
    pic->setImageFOV( QSizeF( 90, 90 ));
    pic->setType( pvQtPic::cyl );

    QString path = "/home/loadbot/projects/vira/panini.git/images/cyli135.jpg";
    QImage* img = new QImage( path );
    pic->setFaceImage( pvQtPic::PicFace(0), img );

    bool ok = panini_window->showPic( pic );
    panini_window->setTurn( 0, 0, 0, 0 );
    qDebug() << "Ok" << ok;
}

void MainWindow::on_pushButton_disable_clicked()
{
    panini_window->showPic( 0 );
}
