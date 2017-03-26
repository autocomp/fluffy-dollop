#include "markform.h"
#include "ui_markform.h"
#include <QDateTime>
#include <QTextEdit>
#include <QResizeEvent>
#include <QFileDialog>
#include <QFile>
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
    ui->close->setIcon(QIcon(":/img/close_button.png"));
    ui->apply->setIcon(QIcon(":/img/ok_button.png"));
    ui->loadImage->setIcon(QIcon(":/img/add_image.png"));

    connect(ui->cancel, SIGNAL(clicked()), this, SIGNAL(signalCloseWindow()));
    connect(ui->close, SIGNAL(clicked()), this, SIGNAL(signalCloseWindow()));
    connect(ui->loadImage, SIGNAL(clicked()), this, SLOT(slotLoadImage()));
    connect(ui->apply, SIGNAL(clicked()), this, SLOT(slotApply()));
}

MarkForm::~MarkForm()
{
    delete _listWidget;
    delete ui;
}

void MarkForm::showEditWidget(quint64 id)
{
    _id = id;

    ui->defect->clear();
    ui->responsible->clear();
    ui->description->clear();

    ui->loadImage->show();
    ui->loadImageLabel->show();
    ui->apply->show();
    ui->cancel->show();
    ui->close->hide();

    //---------------------------------------------

    quint64 curr_dt = QDateTime::currentDateTime().toMSecsSinceEpoch();
    ui->dateTime->setMinimumDateTime(QDateTime::fromMSecsSinceEpoch(curr_dt));
    ui->dateTime->setDateTime(QDateTime::fromMSecsSinceEpoch(curr_dt + 24*60*60*1000));

    _listWidget->clear();
}

void MarkForm::showInfoWidget(quint64 id)
{
    _id = id;

    ui->dateTime->setDateTime(QDateTime::currentDateTime());
    ui->dateTime->setMinimumDateTime(QDateTime::currentDateTime());

    ui->defect->clear();
    ui->responsible->clear();
    ui->description->clear();

    _pixmaps.clear();

    ui->loadImage->hide();
    ui->loadImageLabel->hide();
    ui->apply->hide();
    ui->cancel->hide();
    ui->close->show();


}

void MarkForm::slotLoadImage()
{
    QStringList list = QFileDialog::getOpenFileNames(this, QString::fromUtf8(""));
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
        if(ui->defect->text().isEmpty() == false)
            ptr->setMetadataValue("defect", ui->defect->text());

        if(ui->responsible->text().isEmpty() == false)
            ptr->setMetadataValue("responsible", ui->responsible->text());

        if(ui->description->toPlainText().isEmpty() == false)
            ptr->setMetadataValue("description", ui->description->toPlainText());

        ptr->setMetadataValue("data_time", ui->dateTime->dateTime().toString());
        ptr->setMetadataValue("importance", ui->importance->currentText());
        ptr->setMetadataValue("status", ui->status->currentText());
        ptr->setMetadataValue("category", ui->category->currentText());

        ptr->commit();

        QVariant regionBizInitJson_Path = CtrConfig::getValueByName("application_settings.regionBizFilesPath");
        if(regionBizInitJson_Path.isValid())
        {
             QString destPath = regionBizInitJson_Path.toString();
             QDir dir(destPath);
             bool res = dir.mkdir(QString::number(_id));
             if(res)
             {
                 destPath = destPath + QDir::separator() + QString::number(_id) + QDir::separator();
                 int N(0);
                 foreach(QPixmap pm, _pixmaps)
                 {
                     pm.save(destPath + QString::number(++N) + ".tif");
                 }
             }
        }
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






