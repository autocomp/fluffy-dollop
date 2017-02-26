#include "pdfeditorform.h"
#include "ui_pdfeditorform.h"
#include <poppler/qt4/poppler-qt4.h>
#include <QImage>
#include <ctrwidgets/components/waitdialog/waitdialog.h>
#include <QApplication>
#include <qtconcurrentrun.h>
#include <ctrcore/ctrcore/ctrconfig.h>

PdfEditorForm::PdfEditorForm(QString pdfFilePath, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PdfEditorForm)
{
    ui->setupUi(this);
    _scenePdfImportWidget = new ScenePdfImportWidget;
    ui->rightlLayout->addWidget(_scenePdfImportWidget);

    _pdfPagesListWidget = new PdfPagesListWidget;
    connect(_pdfPagesListWidget, SIGNAL(setPage(int)), this, SLOT(slotSetPage(int)));
    ui->leftTopLayout->addWidget(_pdfPagesListWidget);

    _futureWatcher = new QFutureWatcher<void>;
    connect(_futureWatcher, SIGNAL(finished()), this, SLOT(finishRun()));
    _runFinished = false;
    QString savePath = getSavePath();
    _futureWatcher->setFuture(QtConcurrent::run( std::bind(&PdfEditorForm::run, this, pdfFilePath, savePath) ) );
    WaitDialogUnlimited waitDialogUnlimited;
    while(_runFinished == false)
        qApp->processEvents();

    _scene.setBackgroundBrush(QBrush(Qt::black));
    _view = new PdfEditorView(savePath);
    _view->setScene(&_scene);
    _scenePdfImportWidget->setMainViewWidget(_view);

    for(int i(0); i < _images.size(); ++i)
        _pdfPagesListWidget->addImage(i, _images.at(i));
}

PdfEditorForm::~PdfEditorForm()
{
    delete _scenePdfImportWidget;
    delete _pdfPagesListWidget;
    delete ui;
}

void PdfEditorForm::run(QString pdfFilePath, QString savePath)
{
    QDir dir(savePath);
    Poppler::Document * doc = Poppler::Document::load(pdfFilePath);
    for(uint a(0); a < 3 && a < doc->numPages(); ++a)
    {
        Poppler::Page * page = doc->page(a);
        QString subDirName(QString::number(a));
        bool res1 = dir.mkdir(subDirName);

        int K(36);
        for(uint b(0); b < 4; ++b)
        {
            QImage img = page->renderToImage(K, K);
            K = K*2;
            if(b == 0)
                _images.append(img);
            QString fileName = savePath + QDir::separator() + subDirName + QDir::separator() + QString::number(b) + QString(".tiff");
            bool res2 = img.save(fileName, "TIFF");

            qDebug() << "res1 :" << res1 << ", res2 :" << res2 << ", a :" << a << ", b :" << b << "--->" << fileName;
        }
        delete page;
    }
    delete doc;
}

void PdfEditorForm::finishRun()
{
    _runFinished = true;
}

void PdfEditorForm::slotSetPage(int pageNumber)
{
    if(pageNumber >= 0 && pageNumber < _images.size())
    {
        _view->setPage(pageNumber);
    }
}

QString PdfEditorForm::getSavePath()
{
    QString tempFilesDir;
    QVariant tempFileDir_Path = CtrConfig::getValueByName(QString("application_settings.tempFileDir_Path"));
    if(tempFileDir_Path.isValid())
    {
        tempFilesDir = tempFileDir_Path.toString();

        QDir dir(tempFilesDir);
        QString dest_dir = QDateTime::currentDateTime().toString("dd_MM_yy__hh_mm_ss_z");
        dir.mkdir(dest_dir);

        return tempFilesDir + QDir::separator() + dest_dir;
    }
    else
        return QString();
}




