#include "viraeditorform.h"
#include "ui_viraeditorform.h"
#include "xmlreader.h"
#include "types.h"
//#include <poppler/qt5/poppler-qt5.h>
#include <QImage>
#include <ctrwidgets/components/waitdialog/waitdialog.h>
#include <QApplication>
#include <QtConcurrent/QtConcurrentRun>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <QDir>
#include <QDomDocument>
#include <libembeddedwidgets/embeddedapp.h>
#include <regionbiz/rb_manager.h>

using namespace regionbiz;

ViraEditorForm::ViraEditorForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViraEditorForm)
{
    ui->setupUi(this);
    _sceneViraWidget = new SceneViraWidget;
    _sceneViraWidget->createModeButtonGroup();
    connect(_sceneViraWidget, SIGNAL(switchOnMap()), this, SIGNAL(switchOnMap()));

    ui->topLayout->addWidget(_sceneViraWidget);

    _viraPagesListWidget = new ViraPagesListWidget;
    connect(_viraPagesListWidget, SIGNAL(setFloor(qulonglong)), this, SLOT(slotSetFloor(qulonglong)));
    ui->downLayout->addWidget(_viraPagesListWidget);

    _scene.setBackgroundBrush(QBrush(Qt::black));
    _view = new ViraEditorView();
    _view->setScene(&_scene);
    _sceneViraWidget->setMainViewWidget(_view);

    auto mngr = RegionBizManager::instance();
    mngr->subscribeCenterOn(this, SLOT(slotCenterOn(uint64_t)));
    mngr->subscribeOnSelect(this, SLOT(slotSelectionItemsChanged(uint64_t,uint64_t)));
}

ViraEditorForm::~ViraEditorForm()
{
    delete _sceneViraWidget;
    delete _viraPagesListWidget;
    delete ui;
}

void ViraEditorForm::slotSelectionItemsChanged(uint64_t prev_id, uint64_t curr_id)
{
    if(curr_id > 0)
    {
        BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(curr_id);
        if( ! ptr)
            return;

        uint64_t facilityId(0);
        switch(ptr->getType())
        {
        case BaseArea::AT_REGION :
        case BaseArea::AT_LOCATION :
            return;

        case BaseArea::AT_FACILITY :
        {
            facilityId = curr_id;
        }break;

        case BaseArea::AT_FLOOR :
        case BaseArea::AT_ROOMS_GROUP :
        case BaseArea::AT_ROOM :
        {
            BaseAreaPtr parentPtr = ptr->getParent();
            while(parentPtr)
            {
                if(parentPtr->getType() == BaseArea::AT_FACILITY)
                {
                    facilityId = parentPtr->getId();
                    break;
                }
                parentPtr = parentPtr->getParent();
            }
        }break;
        }

        if(facilityId > 0)
            if(_currFacilityId != facilityId)
            {
                _currFacilityId = facilityId;
                _viraPagesListWidget->reinit(facilityId);
                _view->reinit(facilityId);
            }
    }
    _viraPagesListWidget->selectionItemsChanged(prev_id, curr_id);
    _view->selectionItemsChanged(prev_id, curr_id);
}

void ViraEditorForm::slotCenterOn(uint64_t id)
{
    BaseAreaPtr ptr = RegionBizManager::instance()->getBaseArea(id);
    if( ! ptr)
        return;

    if(ptr->getType() == BaseArea::AT_ROOM)
    {
        _view->centerEditorOn(id);
    }
}

void ViraEditorForm::setParentWindowId(qulonglong parentWindowId)
{
    _parentWindowId = parentWindowId;
}

void ViraEditorForm::centerEditorOn(qulonglong id)
{
    _view->centerEditorOn(id);
}

void ViraEditorForm::slotSetFloor(qulonglong floorId)
{
    _view->setFloor(floorId);
}







/*

QFileInfo fi(xmlFilePath);
if(fi.isFile())
{
    _futureWatcher = new QFutureWatcher<void>;
    connect(_futureWatcher, SIGNAL(finished()), this, SLOT(finishRun()));
    _runFinished = false;
    destPath = getSavePath();
    _futureWatcher->setFuture(QtConcurrent::run( std::bind(&ViraEditorForm::run, this, xmlFilePath, destPath) ) );
    WaitDialogUnlimited waitDialogUnlimited;
    while(_runFinished == false)
        qApp->processEvents();
}
else
{
    destPath = xmlFilePath;
    QDir dir(destPath);
    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Dirs);
    for(int i(0); i<fileInfoList.size(); ++i)
    {
        QString name = destPath + QDir::separator() + QString::number(i) + QDir::separator() + QString("0.tiff");
        if(QFile::exists(name))
        {
            QImage img;
            img.load(name);

//                QImage img2 = img3.scaled(img3.width()/2, img3.height()/2, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//                QString name2 = destPath + QDir::separator() + QString::number(i) + QDir::separator() + QString("2.tiff");
//                img2.save(name2, "TIFF");

//                QImage img1 = img3.scaled(img3.width()/4, img3.height()/4, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//                QString name1 = destPath + QDir::separator() + QString::number(i) + QDir::separator() + QString("1.tiff");
//                img1.save(name1, "TIFF");

//                QImage img0 = img3.scaled(img3.width()/8, img3.height()/8, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
//                QString name0 = destPath + QDir::separator() + QString::number(i) + QDir::separator() + QString("0.tiff");
//                img0.save(name0, "TIFF");

            _images.append(img);
        }
    }
}

_scene.setBackgroundBrush(QBrush(Qt::black));
_view = new PdfEditorView(destPath);
_view->setScene(&_scene);
_sceneViraWidget->setMainViewWidget(_view);

for(int i(0); i < _images.size(); ++i)
    _viraPagesListWidget->addImage(i, _images.at(i));
*/

//void ViraEditorForm::run(QString pdfFilePath, QString savePath)
//{
//    QDir dir(savePath);
//    Poppler::Document * doc = Poppler::Document::load(pdfFilePath);
//    for(uint a(0); a < 3 && a < doc->numPages(); ++a)
//    {
//        Poppler::Page * page = doc->page(a);
//        QString subDirName(QString::number(a));
//        bool res1 = dir.mkdir(subDirName);

//        int K(36);
//        for(uint b(0); b < 4; ++b)
//        {
//            QImage img = page->renderToImage(K, K);
//            K = K*2;
//            if(b == 0)
//                _images.append(img);
//            QString fileName = savePath + QDir::separator() + subDirName + QDir::separator() + QString::number(b) + QString(".tiff");
//            bool res2 = img.save(fileName, "TIFF");

//            qDebug() << "res1 :" << res1 << ", res2 :" << res2 << ", a :" << a << ", b :" << b << "--->" << fileName;
//        }
//        delete page;
//    }
//    delete doc;
//}

//void ViraEditorForm::finishRun()
//{
//    _runFinished = true;
//}

//QString ViraEditorForm::getSavePath()
//{
//    QString tempFilesDir;
//    QVariant tempFileDir_Path = CtrConfig::getValueByName(QString("application_settings.tempFileDir_Path"));
//    if(tempFileDir_Path.isValid())
//    {
//        tempFilesDir = tempFileDir_Path.toString();

//        QDir dir(tempFilesDir);
//        QString dest_dir = QDateTime::currentDateTime().toString("dd_MM_yy__hh_mm_ss_z");
//        dir.mkdir(dest_dir);

//        return tempFilesDir + QDir::separator() + dest_dir;
//    }
//    else
//        return QString();
//}




