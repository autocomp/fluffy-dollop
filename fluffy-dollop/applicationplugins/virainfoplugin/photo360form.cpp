#include "photo360form.h"
#include "ui_photo360form.h"
#include <regionbiz/rb_manager.h>
#include <regionbiz/rb_locations.h>
#include <ctrcore/ctrcore/ctrconfig.h>
#include <QDir>
#include <QDebug>

using namespace regionbiz;

Photo360Form::Photo360Form()
    : ui(new Ui::Photo360Form)
{
    ui->setupUi(this);
    ui->cancel->setIcon(QIcon(":/img/close_button.png"));
    connect(ui->cancel, SIGNAL(clicked()), this, SLOT(slotCloseWindow()));

    // создаём просмотрщик панорамных фото
    // _photo360Viewer = new Photo360Viewer;

    // устанавливаем просмотрщик в лэйаут
    // ui->viewerLayout->addWidget(_photo360Viewer);
}

Photo360Form::~Photo360Form()
{
    // убиваем просмотрщик
    // delete _photo360Viewer;

    delete ui;
}

void Photo360Form::showWidget(quint64 id)
{
    MarkPtr ptr = RegionBizManager::instance()->getMark(id);
    if( ! ptr )
        return;

    BaseMetadataPtr mark_type = ptr->getMetadata("mark_type");
    if( ! mark_type )
        return;
    else
    {
        QString mark_type_str = mark_type->getValueAsVariant().toString();
        if(mark_type_str != QString::fromUtf8("панорамная фотография"))
            return;
    }

    ui->markNameLabel->setText(ptr->getName());

    QVariant regionBizInitJson_Path = CtrConfig::getValueByName("application_settings.regionBizFilesPath");
    if(regionBizInitJson_Path.isValid())
    {
         QString destPath = regionBizInitJson_Path.toString() + QDir::separator() + QString::number(id);
         QDir dir(destPath);
         QStringList list = dir.entryList(QDir::Files);
         if(list.isEmpty() == false)
         {
             QFileInfo fi = destPath + QDir::separator() + list.first();
             QString filePath = fi.absoluteFilePath();

             // устанавливаем в просмотрщик путь к файлу с панорамной фотографией
             // _photo360Viewer->setPhoto(filePath);
         }
    }
}

void Photo360Form::slotCloseWindow()
{
    // при необходимости останавливаем просмотрщик перед закрытием окна
    // _photo360Viewer->stop();

    emit signalCloseWindow();
}















