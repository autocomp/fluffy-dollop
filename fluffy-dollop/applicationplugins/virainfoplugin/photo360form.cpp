#include "photo360form.h"
#include "ui_photo360form.h"
#include <regionbiz/rb_locations.h>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <libpanini/pvQtPic.h>
#include <QDir>
#include <QDebug>

using namespace regionbiz;

Photo360Form::Photo360Form()
    : ui(new Ui::Photo360Form)
{
    ui->setupUi(this);

    ui->cancel->setIcon(QIcon(":/img/close_button.png"));
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(slotCloseWindow()));

    auto mngr = RegionBizManager::instance();
    mngr->subscribeFileSynced( this, SLOT(slotFileLoaded(regionbiz::BaseFileKeeperPtr)));

    _photo360Viewer = new pvQtView( this );
    ui->viewerLayout->addWidget(_photo360Viewer, 100);
    _photo360Viewer->OpenGLOK();

    ui->projCB->setEnabled(false);
    connect(ui->projCB, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotProjChanged(QString)));
}

Photo360Form::~Photo360Form()
{
    delete _photo360Viewer;
    delete ui;
}

void Photo360Form::showWidget(quint64 id)
{
    MarkPtr ptr = RegionBizManager::instance()->getMark(id);
    if( ! ptr )
        return;

    if(ptr->getMarkType() != Mark::MT_PHOTO_3D)
        return;

    BaseFileKeeperPtrs imagesPtrs = ptr->getFilesByType(BaseFileKeeper::FT_IMAGE);
    if(imagesPtrs.empty())
        return;

    BaseFileKeeperPtr imagePtr = imagesPtrs.at(0);
    _fileKey = imagePtr->getPath();
    _markName = ptr->getName();
    BaseFileKeeper::FileState fileState = imagePtr->getFileState();
    switch(fileState)
    {
    case BaseFileKeeper::FS_UNSYNC : {
        qDebug() << "Photo360Form::reload, FS_UNSYNC";
        imagePtr->syncFile();
        ui->markNameLabel->setText(QString::fromUtf8("загрузка"));
        // запускаем крутилку
    }break;
    case BaseFileKeeper::FS_UPLOAD :
    case BaseFileKeeper::FS_SYNC : {
        qDebug() << "Photo360Form::reload, FS_SYNC";
        slotFileLoaded(imagePtr);
    }break;
    case BaseFileKeeper::FS_INVALID : {
        qDebug() << "Photo360Form::reload, FS_INVALID";
        ui->markNameLabel->setText(QString::fromUtf8("ошибка загрузки"));
    }break;
    }
}

void Photo360Form::slotCloseWindow()
{
    emit signalCloseWindow();
}

void Photo360Form::slotFileLoaded(BaseFileKeeperPtr imagePtr)
{
    // останавливаем крутилку
    ui->markNameLabel->setText(_markName);

    qDebug() << "Photo360Form::slotFileLoaded";
    if(_fileKey == imagePtr->getPath())
    {
        QFilePtr file_ptr = imagePtr->getLocalFile();
        QFileInfo info( *(file_ptr.get()) );
        _filePath = info.absoluteFilePath();

        QTimer::singleShot(300, this, SLOT(slotReload()));
    }
}

void Photo360Form::slotReload()
{
    ui->projCB->setEnabled(true);
    slotProjChanged(ui->projCB->currentText());
}

void Photo360Form::slotProjChanged(QString proj)
{
    pvQtPic::PicType picType;

    if(proj == QString("cubic"))
        picType = pvQtPic::cub;
    else if(proj == QString("rectilinear"))
        picType = pvQtPic::rec;
    else if(proj == QString("equal solid angle"))
        picType = pvQtPic::eqs;
    else if(proj == QString("equal angle"))
        picType = pvQtPic::eqa;
    else if(proj == QString("cylindrical"))
        picType = pvQtPic::cyl;
    else if(proj == QString("equirectangular"))
        picType = pvQtPic::eqr;
    else if(proj == QString("stereographic"))
        picType = pvQtPic::stg;
    else if(proj == QString("mercator"))
        picType = pvQtPic::mrc;

    pvQtPic* pic = new pvQtPic();
    pic->setImageFOV( QSizeF( 90, 90 ));
    pic->setType(picType);

    QImage* img = new QImage( _filePath );
    pic->setFaceImage( pvQtPic::PicFace(0), img );

    bool ok = _photo360Viewer->showPic( pic );
    _photo360Viewer->setTurn( 0, 0, 0, 0 );

    qDebug() << "Photo360Form::slotReload ---> OK, filePath:" << _filePath << ", showPic:" << ok;
}
















