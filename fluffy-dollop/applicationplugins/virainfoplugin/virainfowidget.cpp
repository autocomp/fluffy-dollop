#include "virainfowidget.h"
#include "ui_virainfowidget.h"

#include <QDir>

#include <ctrcore/ctrcore/ctrconfig.h>

ViraInfoWidget::ViraInfoWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ViraInfoWidget)
{
    ui->setupUi(this);
}

ViraInfoWidget::~ViraInfoWidget()
{
    delete ui;
}

void ViraInfoWidget::showEvent(QShowEvent *ev)
{
    appendPhotos();

    ui->tableWidget_defects->resizeColumnsToContents();
    ui->tableWidget_pays->resizeColumnsToContents();

    QWidget::showEvent( ev );
}

void ViraInfoWidget::appendPhotos()
{
    // free
    while( ui->horizontalLayout_photos->count() )
    {
        auto item = ui->horizontalLayout_photos->itemAt( 0 );
        ui->horizontalLayout_photos->removeItem( item );
        if( item->widget() )
            delete item->widget();
        delete item;
    }

    // append
    QString path = CtrConfig::getValueByName("application_settings.infoPhotosPath",
                                             QVariant(), true).toString();
    if( !path.isEmpty() )
    {
        QDir dir( path );
        QFileInfoList list = dir.entryInfoList( QDir::Files );
        for( QFileInfo info: list )
        {
            QPixmap px;
            bool load = px.load( info.filePath() );
            if( load )
            {
                QLabel* label = new QLabel;
                label->setPixmap( px.scaled( 300, 200, Qt::KeepAspectRatio ));
                ui->horizontalLayout_photos->addWidget( label );
            }
        }
    }
    ui->horizontalLayout_photos->addStretch();
}
