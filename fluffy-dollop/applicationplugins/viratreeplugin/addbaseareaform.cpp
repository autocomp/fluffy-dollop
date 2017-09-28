#include "addbaseareaform.h"
#include <ctrcore/ctrcore/tempdircontroller.h>
#include "ui_addbaseareaform.h"
#include <ctrcore/bus/common_message_notifier.h>
#include <ctrcore/bus/bustags.h>
#include <poppler/qt5/poppler-qt5.h>
#include <QFileDialog>
#include <QDebug>

using namespace regionbiz;

AddBaseAreaForm::AddBaseAreaForm(regionbiz::BaseArea::AreaType type, uint64_t id)
    : ui(new Ui::AddBaseAreaForm)
    , ew::EmbeddedSubIFace()
    , _type(type)
    , _id(id)
{
    ui->setupUi(this);

    int index(0);
    switch(_type)
    {
    case BaseArea::AT_REGION : {
        _name = QString::fromUtf8("регион");
        ui->addressLabel->hide();
        ui->address->hide();
        ui->cadastralNumberLabel->hide();
        ui->cadastralNumber->hide();
    }break;
    case BaseArea::AT_LOCATION : {
        _name = QString::fromUtf8("локация");
        ui->cadastralNumberLabel->hide();
        ui->cadastralNumber->hide();
    }break;
    case BaseArea::AT_FACILITY : {
        _name = QString::fromUtf8("здание");
    }break;
    case BaseArea::AT_FLOOR : {
        index = 1;
        _name = QString::fromUtf8("этаж");
    }break;
    case BaseArea::AT_ROOM : {
        index = 1;
        _name = QString::fromUtf8("комната");
        ui->floorOrRoomNumberWith->setMinimum(1);
        ui->floorOrRoomNumberWith->setMaximum(10000);
        ui->loadPdfLabel->hide();
        ui->loadPdf->hide();
    }break;
    }

    ui->stackedWidget->setCurrentIndex(index);
    ui->name->setText(_name);
    ui->floorOrRoomName->setText(_name);
    ui->cancel->setIcon(QIcon(":/img/close_button.png"));
    ui->apply->setIcon(QIcon(":/img/ok_button.png"));
    ui->loadPdf->setIcon(QIcon(":/img/icon_pdf.png"));

    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(slotClose()));
    connect(ui->apply, SIGNAL(clicked()), this, SLOT(slotApply()));
    connect(ui->loadPdf, SIGNAL(clicked()), this, SLOT(slotLoadPdf()));
    bool b = connect(ui->floorOrRoomCount, SIGNAL(valueChanged(int)), this, SLOT(slotFloorOrRoomCountChanged(int)));
    qDebug() << b;
}

AddBaseAreaForm::~AddBaseAreaForm()
{
    delete ui;
}

QWidget *AddBaseAreaForm::getWidget()
{
    return this;
}

void AddBaseAreaForm::slotClose()
{
    ewApp()->setVisible(id(), false);
}

void AddBaseAreaForm::slotLoadPdf()
{
    QString fileName = QFileDialog::getOpenFileName(this, QString::fromUtf8("Выберите файл с поэтажным планом здания"), QString(), QString("*.pdf"));
    if(fileName.isEmpty() == false)
    {
        _pdfFileName = fileName;

        Poppler::Document * doc = Poppler::Document::load(_pdfFileName);
        ui->loadPdfLabel->setText(QFileInfo(fileName).fileName());
        ui->floorOrRoomCount->setValue(doc->numPages());
        ui->floorOrRoomCount->setDisabled(true);
        delete doc;
    }
}

void AddBaseAreaForm::slotFloorOrRoomCountChanged(int count)
{
    QString text = ui->floorOrRoomName->text();
    if(count == 1)
    {
        ui->floorOrRoomName->setText(_name);
        ui->floorOrRoomNumberWith->setDisabled(true);
        ui->floorOrRoomNumberWith->setValue(1);
    }
    else
    {
        ui->floorOrRoomNumberWith->setDisabled(false);
        if(text.contains(QString("%N%")) == false)
        {
            if(_type == BaseArea::AT_FLOOR)
                text = QString("%N% ") + _name;
            else
                text = _name + QString(" %N%");
            ui->floorOrRoomName->setText(text);
        }
    }
}

void AddBaseAreaForm::slotApply()
{
    bool createSingleObject(true);
    if(_type == BaseArea::AT_FLOOR || _type == BaseArea::AT_ROOM)
    {
        if(ui->floorOrRoomCount->value() > 1)
            createSingleObject = false;
    }

    if(createSingleObject)
    {
        BaseAreaPtr createdPtr = RegionBizManager::instance()->addArea(_type, _id);
        if(createdPtr)
        {
            QString description = ui->description->toPlainText();
            if( ! description.isEmpty())
                createdPtr->setDesription(ui->description->toPlainText());
            switch(_type)
            {
            case BaseArea::AT_REGION : {
                createdPtr->setName(ui->name->text());
            }break;
            case BaseArea::AT_LOCATION : {
                createdPtr->setName(ui->name->text());
                auto location = BaseArea::convert<Location>(createdPtr);
                if(location)
                    location->setAddress(ui->address->text());
            }break;
            case BaseArea::AT_FACILITY : {
                createdPtr->setName(ui->name->text());
                auto facility = BaseArea::convert<Facility>(createdPtr);
                if(facility)
                {
                    facility->setAddress(ui->address->text());
                    facility->setCadastralNumber(ui->cadastralNumber->text());
                }
            }break;
            case BaseArea::AT_FLOOR : {
                createdPtr->setName(ui->floorOrRoomName->text());
                uint16_t maxNumber(0);
                auto facilityPtr = BaseArea::convert<Facility>(createdPtr->getParent());
                if(facilityPtr)
                {
                    FloorPtrs floors = facilityPtr->getChilds();
                    for( FloorPtr floorPtr: floors )
                        if(maxNumber < floorPtr->getNumber())
                            maxNumber = floorPtr->getNumber();
                }
                auto floor = BaseArea::convert<Floor>(createdPtr);
                if(floor)
                    floor->setNumber(++maxNumber);

                CommonMessageNotifier::send( (uint)visualize_system::BusTags::SetRoomVisibleOnFloor, QVariant(), QString("visualize_system"));
            }break;
            case BaseArea::AT_ROOM : {
                createdPtr->setName(ui->floorOrRoomName->text());
            }break;
            }
            createdPtr->commit();
        }
    }
    else
    {
        Poppler::Document * doc = nullptr;
        if(_pdfFileName.isEmpty() == false)
            doc = Poppler::Document::load(_pdfFileName);

        uint16_t maxNumber(0);
        if(_type == BaseArea::AT_FLOOR)
        {
            auto facilityPtr = BaseArea::convert<Facility>(RegionBizManager::instance()->getBaseArea(_id));
            if(facilityPtr)
            {
                FloorPtrs floors = facilityPtr->getChilds();
                for( FloorPtr floorPtr: floors )
                    if(maxNumber < floorPtr->getNumber())
                        maxNumber = floorPtr->getNumber();
            }
        }

        for(int N(0); N< 2 /*ui->floorOrRoomCount->value()*/ ; ++N)
        {
            BaseAreaPtr createdPtr = RegionBizManager::instance()->addArea(_type, _id);
            if(createdPtr)
            {
                QString name = ui->floorOrRoomName->text();
                QString NUMBER = QString::number(ui->floorOrRoomNumberWith->value() + N);
                name.replace(QString("%N%"), NUMBER);
                createdPtr->setName(name);

                QString description = ui->description->toPlainText();
                if( ! description.isEmpty())
                    createdPtr->setDesription(ui->description->toPlainText());

                if(doc)
                {
                    Poppler::Page * page = doc->page(N);
                    if(page)
                    {
                        double resolution = 72 * 2;
                        QImage image = page->renderToImage(resolution, resolution);
                        QString filePath = TempDirController::createTempDirForCurrentUser() + QDir::separator() + "temp.tif";
                        bool res = image.save(filePath, "TIF");
                        if(res)
                        {
                            BaseFileKeeperPtr basePlan = RegionBizManager::instance()->addFile(filePath, BaseFileKeeper::FT_PLAN, createdPtr->getId());
                            PlanFileKeeperPtr plan = BaseFileKeeper::convert<PlanFileKeeper>(basePlan);
                            if(plan)
                            {
                                PlanFileKeeper::PlanParams planParams;
                                planParams.scale_w = 1;
                                planParams.scale_h = 1;
                                planParams.rotate = 0;
                                planParams.x = 0;
                                planParams.y = 0;
                                plan->setPlanParams(planParams);
                                plan->commit();
                            }
                        }
                        delete page;
                    }
                }

                if(_type == BaseArea::AT_FLOOR)
                {
                    auto floor = BaseArea::convert<Floor>(createdPtr);
                    if(floor)
                        floor->setNumber(++maxNumber);
                }

                createdPtr->commit();
            }
        }
        delete doc;
        RegionBizManager::instance()->clearCurrent();
        CommonMessageNotifier::send( (uint)visualize_system::BusTags::SetRoomVisibleOnFloor, QVariant(), QString("visualize_system"));
    }

    slotClose();
}















