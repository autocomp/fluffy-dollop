#include "waitdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QFrame>
#include <QApplication>
#include <QDesktopWidget>
#include <libembeddedwidgets/embeddedstruct.h>
#include <libembeddedwidgets/embeddedapp.h>

ProgressDialog::ProgressDialog(const QString& captionText)
{
    QVBoxLayout *pLayout = new QVBoxLayout(this);

    m_label = new QLabel(captionText, this);
    pLayout->addWidget(m_label, 0, Qt::AlignCenter);

    QSpacerItem * spacer = new QSpacerItem(0, 5, QSizePolicy::Minimum, QSizePolicy::Preferred);
    pLayout->addSpacerItem(spacer);

    QFrame *fr = new QFrame(this);
    fr->setFrameShape(QFrame::HLine);
    fr->setFrameShadow(QFrame::Sunken );
    pLayout->addWidget(fr);

    spacer = new QSpacerItem(0, 5, QSizePolicy::Minimum, QSizePolicy::Preferred);
    pLayout->addSpacerItem(spacer);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);
    pLayout->addWidget(m_progressBar);

    _iface = new EmbIFaceNotifier(this);
    QString tag = QString("ProgressDialog.withProgressBar");
    ew::EmbeddedWidgetStruct struc;
    ew::EmbeddedHeaderStruct headStr;
    struc.headerVisible = false;
    struc.topOnHint = true;
    struc.header = headStr;
    struc.isModal = true;
    struc.isModalBlock = false;
    struc.iface = _iface;
    struc.widgetTag = tag;
    struc.topOnHint = true;
    struc.minSize = QSize(300,80);
    struc.maxSize = QSize(300,80);
    ewApp()->createWidget(struc, ewApp()->getMainWindowId());
    ewApp()->setVisible(_iface->id(), true);
    connect(_iface, SIGNAL(signalClosed()), this, SLOT(slotEmbWidgetClose()));
}

ProgressDialog::~ProgressDialog()
{
    ewApp()->setVisible(_iface->id(), false);
    _iface->deleteLater();
}

void ProgressDialog::setPersent(uint persent)
{
    m_progressBar->setValue(persent);
}

void ProgressDialog::slotEmbWidgetClose()
{
    ewApp()->setVisible(_iface->id(), true);
}

//---------------------------------------------------------------

WaitDialog::WaitDialog(bool &cancelFlag, const QString& captionText, const QString &cancelButtonText)
    : m_cancelFlag(cancelFlag)
{
    m_cancelFlag = false;

    QVBoxLayout *pLayout = new QVBoxLayout(this);

    m_label = new QLabel(captionText, this);
    pLayout->addWidget(m_label, 0, Qt::AlignCenter);

    QSpacerItem * spacer = new QSpacerItem(0, 5, QSizePolicy::Minimum, QSizePolicy::Preferred);
    pLayout->addSpacerItem(spacer);

    QFrame *fr = new QFrame(this);
    fr->setFrameShape(QFrame::HLine);
    fr->setFrameShadow(QFrame::Sunken );
    pLayout->addWidget(fr);

    spacer = new QSpacerItem(0, 5, QSizePolicy::Minimum, QSizePolicy::Preferred);
    pLayout->addSpacerItem(spacer);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setMinimum(0);
    m_progressBar->setMaximum(100);
    m_progressBar->setValue(0);
    pLayout->addWidget(m_progressBar);

    QHBoxLayout *pHLayout = new QHBoxLayout(this);
    spacer = new QSpacerItem(200, 0, QSizePolicy::Preferred, QSizePolicy::Minimum);
    pHLayout->addSpacerItem(spacer);

    m_cancelButton = new QPushButton(cancelButtonText, this);
    connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(slotCancel()));
    pHLayout->addWidget(m_cancelButton);

    pLayout->addLayout(pHLayout);

    _iface = new EmbIFaceNotifier(this);
    QString tag = QString("WaitDialog.withProgressBar");
    ew::EmbeddedWidgetStruct struc;
    ew::EmbeddedHeaderStruct headStr;
    struc.headerVisible = false;
    struc.topOnHint = true;
    struc.header = headStr;
    struc.isModal = true;
    struc.isModalBlock = false;
    struc.iface = _iface;
    struc.widgetTag = tag;
    struc.topOnHint = true;
    struc.minSize = QSize(300,120);
    struc.maxSize = QSize(300,120);
    ewApp()->createWidget(struc, ewApp()->getMainWindowId());
    ewApp()->setVisible(_iface->id(), true);
    connect(_iface, SIGNAL(signalClosed()), this, SLOT(slotEmbWidgetClose()));
}

WaitDialog::~WaitDialog()
{
    ewApp()->setVisible(_iface->id(), false);
    _iface->deleteLater();
}


void WaitDialog::setCancelButtonVisibility(bool on_off)
{
    m_cancelButton->setVisible(on_off);
}

void WaitDialog::setPersent(uint persent)
{
    m_progressBar->setValue(persent);
}

void WaitDialog::slotCancel()
{
    m_label->setText(QString::fromUtf8("Останавливается ..."));
    m_cancelButton->setDisabled(true);
    m_cancelFlag = true;
    emit signalCancel();
}

void WaitDialog::slotEmbWidgetClose()
{
    ewApp()->setVisible(_iface->id(), true);
}

//-----------------------------------------------------------------------

WaitDialogUnlimited::WaitDialogUnlimited(QString descriptionText, QString userButtonText, uint maxWgtWidth, QWidget *parent, QString gifResourcePath)
{
    uint defaultMaxWidth = maxWgtWidth;
    uint defaultMaxHeight = defaultMaxWidth;

    QVBoxLayout *pMainVBLayout = new QVBoxLayout(this);
    pMainVBLayout->setContentsMargins(0,0,0,0);
    pMainVBLayout->setSpacing(3);

    if(!descriptionText.isEmpty())
    {
        QLabel *descrLabel = new QLabel(descriptionText,this);
        descrLabel->setWordWrap(true);
        descrLabel->setFixedWidth(defaultMaxWidth);
        descrLabel->setAlignment(Qt::AlignHCenter);
        pMainVBLayout->addWidget(descrLabel);
    }

    QLabel *tmpLabel = new QLabel(this);
    m_movie = new QMovie(this);
    m_movie->setCacheMode(QMovie::CacheAll);
    m_movie->setScaledSize(QSize(defaultMaxWidth,defaultMaxWidth));
    m_movie->setFileName(gifResourcePath);      //":/img/loading.gif"
    m_movie->setSpeed(300);
    tmpLabel->setMovie(m_movie);
    pMainVBLayout->addWidget(tmpLabel);

    if(!descriptionText.isEmpty())
    {
        defaultMaxHeight = defaultMaxWidth + 25;
    }

    if(!userButtonText.isEmpty())
    {
        QPushButton *m_pbUserButton = new QPushButton(userButtonText,this);
        connect(m_pbUserButton,SIGNAL(clicked()),SIGNAL(sigUserButtonClicked()));
        pMainVBLayout->addWidget(m_pbUserButton);
        defaultMaxHeight = defaultMaxWidth + m_pbUserButton->height();
    }

    _iface = new EmbIFaceNotifier(this);
    QString tag = QString("WaitDialogUnlimited.withProgressBar");
    ew::EmbeddedWidgetStruct struc;
    ew::EmbeddedHeaderStruct headStr;
    struc.headerVisible = false;
    struc.topOnHint = true;
    struc.header = headStr;
    struc.isModal = true;
    struc.isModalBlock = false;
    struc.iface = _iface;
    struc.widgetTag = tag;
    struc.topOnHint = true;
    struc.minSize = QSize(120,120);
    struc.maxSize = QSize(120,120);
    ewApp()->createWidget(struc, ewApp()->getMainWindowId());
    ewApp()->setVisible(_iface->id(), true);
    connect(_iface, SIGNAL(signalClosed()), this, SLOT(slotEmbWidgetClose()));

    startWaiting();
}

WaitDialogUnlimited::~WaitDialogUnlimited()
{
    ewApp()->setVisible(_iface->id(), false);
    _iface->deleteLater();
    stopWaiting();
    delete m_movie;
    m_movie = nullptr;
}

void WaitDialogUnlimited::startWaiting()
{
    m_movie->start();
}

void WaitDialogUnlimited::stopWaiting()
{
    m_movie->stop();
}

void WaitDialogUnlimited::slotEmbWidgetClose()
{
    ewApp()->setVisible(_iface->id(), true);
}



