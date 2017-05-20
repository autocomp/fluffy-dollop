#include "markform.h"
#include "ui_markform.h"
#include <QDateTime>
#include <QTextEdit>
#include <QResizeEvent>
#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QMessageBox>
#include <QImageReader>
#include <regionbiz/rb_manager.h>
#include <regionbiz/rb_locations.h>
#include <ctrcore/ctrcore/ctrconfig.h>

using namespace regionbiz;

MarkForm::MarkForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MarkForm)
{
    ui->setupUi(this);
    _listWidget = new PixmapListWidget;
    ui->galeryLayout->addWidget(_listWidget);

    ui->cancel->setIcon(QIcon(":/img/close_button.png"));
    ui->apply->setIcon(QIcon(":/img/ok_button.png"));
    ui->loadImage->setIcon(QIcon(":/img/add_image.png"));
    ui->toArchieve->setIcon(QIcon(":/img/garbg.png"));

    connect(ui->cancel, SIGNAL(clicked()), this, SIGNAL(signalCloseWindow()));
    connect(ui->loadImage, SIGNAL(clicked()), this, SLOT(slotLoadImage()));
    connect(ui->apply, SIGNAL(clicked()), this, SLOT(slotApply()));
    connect(ui->toArchieve, SIGNAL(clicked()), this, SLOT(slotToArchieve()));
}

MarkForm::~MarkForm()
{
    delete _listWidget;
    delete ui;
}

void MarkForm::showWidgetAndCreateMark(MarkType markType, quint64 parentId, QPolygonF markArea, double direction)
{
    _markType = markType;
    _id = 0;
    _parentId = parentId;
    _markArea = markArea;
    _direction = direction;
    ui->markName->clear();
    ui->responsible->clear();
    ui->description->clear();
    //ui->dateEdit->setMinimumDate(QDate::currentDate());
    ui->dateEdit->setDate(QDate::currentDate());
    _pixmaps.clear();
    _listWidget->clear();
    ui->status->setCurrentText(QString::fromUtf8("новый"));
    _foto360FilePath.clear();
    actulize();

    ui->apply->setEnabled(_markType == Defect);
}

void MarkForm::actulize()
{
    switch(_markType)
    {
    case Defect :
        ui->responsibleLabel->show();
        ui->responsible->show();
        ui->dateEditLabel->show();
        ui->dateEdit->show();
        ui->importanceLabel->show();
        ui->importance->show();
        ui->statusLabel->show();
        ui->status->show();
        ui->categoryLabel->show();
        ui->category->show();
        ui->toArchieve->show();
        ui->loadImageLabel->setText(QString::fromUtf8("Загрузить изображение"));
        _listWidget->show();
        break;

    case Foto :
        ui->responsibleLabel->hide();
        ui->responsible->hide();
        ui->dateEditLabel->hide();
        ui->dateEdit->hide();
        ui->importanceLabel->hide();
        ui->importance->hide();
        ui->statusLabel->hide();
        ui->status->hide();
        ui->categoryLabel->hide();
        ui->category->hide();
        ui->toArchieve->hide();
        ui->loadImageLabel->setText(QString::fromUtf8("Загрузить изображение"));
        _listWidget->show();
        break;

    case Foto360 :
        ui->responsibleLabel->hide();
        ui->responsible->hide();
        ui->dateEditLabel->hide();
        ui->dateEdit->hide();
        ui->importanceLabel->hide();
        ui->importance->hide();
        ui->statusLabel->hide();
        ui->status->hide();
        ui->categoryLabel->hide();
        ui->category->hide();
        ui->toArchieve->hide();
        _listWidget->hide();
        ui->loadImageLabel->setText(QString::fromUtf8("Загрузить панорамное изображение"));
        break;
    }
}

void MarkForm::showWidget(quint64 id)
{
    _markType = Defect;
    _id = id;
    _parentId = 0;
    _markArea.clear();
    _direction = 0;
    ui->markName->clear();
    ui->responsible->clear();
    ui->description->clear();
    //ui->dateEdit->setMinimumDate(QDate::currentDate());
    ui->dateEdit->setDate(QDate::currentDate());
    _pixmaps.clear();
    _listWidget->clear();
    _foto360FilePath.clear();
    ui->status->setCurrentText(QString::fromUtf8("новый"));
    ui->apply->setEnabled(true);

    MarkPtr ptr = RegionBizManager::instance()->getMark(_id);
    if(ptr)
    {
        BaseMetadataPtr mark_type = ptr->getMetadata("mark_type");
        if(mark_type)
        {
            QString mark_type_str = mark_type->getValueAsVariant().toString();
            if(mark_type_str == QString::fromUtf8("фотография"))
                _markType = Foto;
            else if(mark_type_str == QString::fromUtf8("панорамная фотография"))
                _markType = Foto360;
        }
        actulize();

        ui->markName->setText(ptr->getName());
        ui->description->setPlainText(ptr->getDescription());

        if(_markType == Defect)
        {
            BaseMetadataPtr worker = ptr->getMetadata("worker");
            if(worker)
                ui->responsible->setText(worker->getValueAsVariant().toString());

            BaseMetadataPtr date = ptr->getMetadata("date");
            if(date)
                ui->dateEdit->setDate( QDate::fromString(date->getValueAsVariant().toString(), "dd.MM.yy") );

            BaseMetadataPtr priority = ptr->getMetadata("priority");
            if(priority)
                ui->importance->setCurrentText(priority->getValueAsVariant().toString());

            BaseMetadataPtr status = ptr->getMetadata("status");
            if(status)
                ui->status->setCurrentText(status->getValueAsVariant().toString());

            BaseMetadataPtr category = ptr->getMetadata("category");
            if(category)
                ui->category->setCurrentText(category->getValueAsVariant().toString());
        }

        QVariant regionBizInitJson_Path = CtrConfig::getValueByName("application_settings.regionBizFilesPath");
        if(regionBizInitJson_Path.isValid())
        {
             QString destPath = regionBizInitJson_Path.toString() + QDir::separator() + QString::number(_id);
             QDir dir(destPath);
             QStringList list = dir.entryList(QDir::Files);
             foreach(QString fileName,list)
             {
                 fileName.prepend(destPath + QDir::separator());
                 QPixmap pm(fileName);
                 if(pm.isNull() == false)
                     _listWidget->addItem(pm);
             }
        }
    }
}

void MarkForm::slotLoadImage()
{
    if(_markType == Foto360)
    {
        QString filePath = QFileDialog::getOpenFileName(this);
        if(filePath.isEmpty() == false)
        {
            _foto360FilePath = filePath;
            QFileInfo fi(_foto360FilePath);
            ui->loadImageLabel->setText(QString::fromUtf8("Панорамное изображение загружено : ") + fi.fileName());
            ui->apply->setEnabled(true);
        }
    }
    else
    {
        QStringList list;
        if(_markType == Foto)
            list.append(QFileDialog::getOpenFileName(this));
        else
            list = QFileDialog::getOpenFileNames(this);

        foreach (QString name, list)
        {
            QPixmap pm(name);
            if(pm.isNull())
                continue;

            if(_markType == Foto)
                ui->apply->setEnabled(true);

            int rotate(0);
            QImageReader imageReader(name);
            switch(imageReader.transformation())
            {
            case QImageIOHandler::TransformationRotate90 :{
                rotate = 90;
            }break;
            case QImageIOHandler::TransformationRotate180 :{
                rotate = 180;
            }break;
            case QImageIOHandler::TransformationRotate270 :{
                rotate = 270;
            }break;
            }

            if(rotate != 0)
                pm = pm.transformed(QTransform().rotate(rotate));

            _pixmaps.append(pm);
            _listWidget->addItem(pm);
        }
    }
}

void MarkForm::slotApply()
{
    closeAndCommit(false);
}

void MarkForm::slotToArchieve()
{
    if(QMessageBox::Yes == QMessageBox::question(this, QString::fromUtf8("Внимание"), QString::fromUtf8("Переместить задачу в архив ?"), QMessageBox::Yes, QMessageBox::No))
        closeAndCommit(true);
}

void MarkForm::closeAndCommit(bool moveToArchive)
{
    MarkPtr markPtr;
    if(_id == 0)
    {
        auto areaPtr = RegionBizManager::instance()->getBaseArea(_parentId);
        switch(areaPtr->getType())
        {
        case BaseArea::AT_LOCATION :
        {
            auto location = BaseArea::convert<Location>(areaPtr);
            markPtr = location->addMark();
        }break;
        case BaseArea::AT_FACILITY :
        {
            auto facility = BaseArea::convert<Facility>(areaPtr);
            markPtr = facility->addMark();
        }break;
        case BaseArea::AT_FLOOR :
        {
            auto floor = BaseArea::convert<Floor>(areaPtr);
            markPtr = floor->addMark();
        }break;
        case BaseArea::AT_ROOM :
        {
            auto room = BaseArea::convert<Room>(areaPtr);
            markPtr = room->addMark();
        }break;
        }

        if(markPtr)
        {
            _id = markPtr->getId();
            if(_markArea.size() == 1)
                markPtr->setCenter(_markArea.first());
            else
                markPtr->setCoords(_markArea);
            bool res = markPtr->commit();
            qDebug() << "commit mark :" << res;
        }
    }

    if( ! markPtr)
        markPtr = RegionBizManager::instance()->getMark(_id);

    if(markPtr)
    {
        if(_markType != Foto360 && _pixmaps.isEmpty() == false)
        {
            QVariant regionBizInitJson_Path = CtrConfig::getValueByName("application_settings.regionBizFilesPath");
            if(regionBizInitJson_Path.isValid())
            {
                QString destPath = regionBizInitJson_Path.toString();
                QDir dir(destPath);
                dir.mkdir(QString::number(_id));
                destPath = destPath + QDir::separator() + QString::number(_id) + QDir::separator();
                int N(0);
                foreach(QPixmap pm, _pixmaps)
                    pm.save(destPath + QString::number(QDateTime::currentMSecsSinceEpoch()) + QString("_") + QString::number(++N) + ".tiff");
            }
        }

        markPtr->setName( ui->markName->text());
        markPtr->setDesription( ui->description->toPlainText());

        if(_markType == Defect)
        {
            markPtr->addMetadata("string", "mark_type", QString::fromUtf8("дефект"));
            markPtr->addMetadata("string", "worker", ui->responsible->text());
            QString dataStr = ui->dateEdit->date().toString("dd.MM.yy");
            markPtr->addMetadata("string", "date", dataStr);
            markPtr->addMetadata("string", "priority", ui->importance->currentText());
            markPtr->addMetadata("string", "category", ui->category->currentText());
            if(moveToArchive)
                markPtr->addMetadata("string", "status", QString::fromUtf8("в архиве"));
            else
                markPtr->addMetadata("string", "status", ui->status->currentText());
        }
        else if(_markType == Foto)
        {
            markPtr->addMetadata("string", "mark_type", QString::fromUtf8("фотография"));
            markPtr->addMetadata("double", "foto_direction", _direction);
        }
        else if(_markType == Foto360)
        {
            QVariant regionBizInitJson_Path = CtrConfig::getValueByName("application_settings.regionBizFilesPath");
            if(regionBizInitJson_Path.isValid())
            {
                QString destPath = regionBizInitJson_Path.toString();
                QDir dir(destPath);
                dir.mkdir(QString::number(_id));
                QFileInfo fi(_foto360FilePath);
                destPath = destPath + QDir::separator() + QString::number(_id) + QDir::separator() + fi.fileName();
                bool res = QFile::copy(_foto360FilePath, destPath);
                qDebug() << "copy foto360 from :" << _foto360FilePath << " to :" << destPath << ", res :" << res;
            }

            markPtr->addMetadata("string", "mark_type", QString::fromUtf8("панорамная фотография"));
        }

        bool commitRes = markPtr->commit();
        qDebug() << _id << ", commitRes:" << commitRes;
    }
    emit signalCloseWindow();
}








