#include "markform.h"
#include "ui_markform.h"
#include <QDateTime>
#include <QTextEdit>
#include <QResizeEvent>
#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <regionbiz/rb_manager.h>
#include <regionbiz/rb_locations.h>
#include <ctrcore/ctrcore/ctrconfig.h>

using namespace regionbiz;

MarkForm::MarkForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MarkForm)
{
    ui->setupUi(this);
    _listWidget = new MarkFormListWidget;
    ui->galeryLayout->addWidget(_listWidget);

    ui->cancel->setIcon(QIcon(":/img/close_button.png"));
    ui->apply->setIcon(QIcon(":/img/ok_button.png"));
    ui->loadImage->setIcon(QIcon(":/img/add_image.png"));

    connect(ui->cancel, SIGNAL(clicked()), this, SIGNAL(signalCloseWindow()));
    connect(ui->loadImage, SIGNAL(clicked()), this, SLOT(slotLoadImage()));
    connect(ui->apply, SIGNAL(clicked()), this, SLOT(slotApply()));
}

MarkForm::~MarkForm()
{
    delete _listWidget;
    delete ui;
}

void MarkForm::showWidget(quint64 id)
{
    _id = id;
    ui->defect->clear();
    ui->responsible->clear();
    ui->description->clear();
    //ui->dateEdit->setMinimumDate(QDate::currentDate());
    ui->dateEdit->setDate(QDate::currentDate());
    _pixmaps.clear();
    _listWidget->clear();

    MarkPtr ptr = RegionBizManager::instance()->getMark(_id);
    if(ptr)
    {
        ui->defect->setText(ptr->getName());
        ui->description->setPlainText(ptr->getDescription());

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
    QStringList list = QFileDialog::getOpenFileNames(this); //, QString::fromUtf8(""));
    foreach (QString name, list)
    {
        QPixmap pm(name);
        if(pm.isNull() == false)
        {
            _pixmaps.append(pm);
            _listWidget->addItem(pm);
        }
    }
}

void MarkForm::slotApply()
{
    MarkPtr ptr = RegionBizManager::instance()->getMark(_id);
    if(ptr)
    {
        ptr->setName( ui->defect->text());
        ptr->addMetadata("string", "worker", ui->responsible->text());
        ptr->setDesription( ui->description->toPlainText());
        QString dataStr = ui->dateEdit->date().toString("dd.MM.yy");
        bool res = ptr->addMetadata("string", "date", dataStr);
        ptr->addMetadata("string", "priority", ui->importance->currentText());
        ptr->addMetadata("string", "status", ui->status->currentText());
        ptr->addMetadata("string", "category", ui->category->currentText());
        bool commitRes = ptr->commit();
        qDebug() << _id << ", dataStr:" << dataStr << res << ", commitRes:" << commitRes;

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
        emit signalUpdateMark(_id);
    }
    emit signalCloseWindow();
}

//------------------------------------------------

MarkFormListWidget::MarkFormListWidget()
{
    setViewMode(QListView::IconMode);
    setFlow(QListView::LeftToRight);
    setWrapping(false);
    setIconSize(QSize(200,100));

    setSelectionMode(QAbstractItemView::SingleSelection);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

void MarkFormListWidget::addItem(const QPixmap &pm)
{
    _coef = (double)pm.height() / (double)pm.width();
    QListWidgetItem * item = new QListWidgetItem(QIcon(pm), "", this);
}

void MarkFormListWidget::resizeEvent(QResizeEvent *e)
{
    int H = e->size().height();
    _iconSize = QSize(H/_coef - 25, H - 25);
    setIconSize(_iconSize);
    QListWidget::resizeEvent(e);
}






