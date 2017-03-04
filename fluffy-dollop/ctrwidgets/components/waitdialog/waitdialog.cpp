#include "waitdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QFrame>
#include <QApplication>
#include <QDesktopWidget>

WaitDialog::WaitDialog(bool &cancelFlag, const QString& captionText, const QString &cancelButtonText)
    : m_cancelFlag(cancelFlag)
{
    m_cancelFlag = false;

    setStyleSheet("QWidget{background:lightGray;}");
    setWindowFlags(Qt::SplashScreen);
    setWindowModality(Qt::ApplicationModal);
    setModal(true);

    QVBoxLayout *pLayout = new QVBoxLayout(this);

    QLabel * label = new QLabel(captionText, this);
    pLayout->addWidget(label, 0, Qt::AlignCenter);

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

    show();
}

void WaitDialog::setPersent(uint persent)
{
    m_progressBar->setValue(persent);
}

void WaitDialog::slotCancel()
{
    m_cancelButton->hide();
    m_cancelFlag = true;
    emit signalCancel();
    close();
}

//---------------------------------------------------------------

WaitDialogUnlimited::WaitDialogUnlimited( QString descriptionText, QString userButtonText, uint maxWgtWidth, QWidget *parent, QString gifResourcePath)
    : QDialog(parent)
{
    setWindowModality(Qt::ApplicationModal);
    setWindowFlags(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);

    uint defaultMaxWidth = maxWgtWidth;         //150
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

//    setFixedSize(QSize(defaultMaxWidth,defaultMaxHeight));

    startWaiting();
}

WaitDialogUnlimited::~WaitDialogUnlimited()
{
    stopWaiting();
    delete m_movie;
    m_movie = nullptr;
}

void WaitDialogUnlimited::startWaiting()
{
    m_movie->start();
    show();
    move(qApp->desktop()->screenGeometry(qApp->desktop()->primaryScreen()).width()/2 - width()/2,
         qApp->desktop()->screenGeometry(qApp->desktop()->primaryScreen()).height()/2 - height()/2);
}

void WaitDialogUnlimited::stopWaiting()
{
    m_movie->stop();
    hide();
}



