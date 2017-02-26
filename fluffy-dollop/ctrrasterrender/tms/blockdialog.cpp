#include "blockdialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QToolButton>

BlockDialog::BlockDialog(uint timeoutMsec, uint showTimeoutMsec)
    : m_Stop(false)
{
    setStyleSheet("QWidget{background:lightGray;}");
    setWindowFlags(Qt::SplashScreen);
    setWindowModality(Qt::ApplicationModal);
    setModal(true);

    QVBoxLayout *pLayout = new QVBoxLayout(this);

    QSpacerItem * spacer = new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);
    pLayout->addSpacerItem(spacer);

    QLabel * label = new QLabel(this);
    pLayout->addWidget(label, 0, Qt::AlignCenter);

    spacer = new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);
    pLayout->addSpacerItem(spacer);

    QHBoxLayout *pHLayout = new QHBoxLayout(this);

    QToolButton * tb = new QToolButton(this);
    connect(tb, SIGNAL(clicked()), this, SLOT(slotTimeout()));
    tb->setToolTip(QString::fromUtf8("Отменить"));
    tb->setIcon(QIcon(":/41_stop.png"));
    tb->setFixedSize(32,32);
    tb->setIconSize(QSize(32,32));
    tb->setAutoRaise(true);
    pHLayout->addWidget(tb);

    QLabel * textLabel = new QLabel(QString::fromUtf8("получение высоты"), this);
    pHLayout->addWidget(textLabel, 0, Qt::AlignCenter);

    pLayout->addLayout(pHLayout);

    m_Movie = new QMovie(":/loading.gif", QByteArray(), this);
    m_Movie->setScaledSize(QSize(128,128));

    label->setMinimumSize(QSize(128,128));
    label->resize(128,128);
    label->setMovie(m_Movie);
    label->show();
    m_Movie->start();

    QTimer::singleShot(timeoutMsec, this, SLOT(slotTimeout()));
    QTimer::singleShot(showTimeoutMsec, this, SLOT(slotShowTimeout()));
}

void BlockDialog::slotShowTimeout()
{
    show();
}

void BlockDialog::slotTimeout()
{
    m_Stop = true;
}

bool BlockDialog::stop()
{
    return m_Stop;
}





