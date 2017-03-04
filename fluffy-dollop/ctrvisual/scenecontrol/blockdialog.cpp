#include "blockdialog.h"

#include <QVBoxLayout>
#include <QSpacerItem>
#include <QMessageBox>
#include <QPushButton>
#include <QLabel>

using namespace visualize_system;

BlockDialog::BlockDialog(bool &cancelFlag)
    : m_cancelFlag(cancelFlag)
{
    m_cancelFlag = false;

    setStyleSheet("QWidget{background:lightGray;}");
    setWindowFlags(Qt::SplashScreen);
    setWindowModality(Qt::ApplicationModal);
    setModal(true);
    setMinimumWidth(500);

    QVBoxLayout *pLayout = new QVBoxLayout(this);

    QLabel * textLabel = new QLabel(QString::fromUtf8("Кэширование данных"), this);
    pLayout->addWidget(textLabel, 0, Qt::AlignCenter);

    QSpacerItem * spacer = new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);
    pLayout->addSpacerItem(spacer);

    spacer = new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Fixed);
    pLayout->addSpacerItem(spacer);

    QHBoxLayout *pHLayout = new QHBoxLayout(this);

    m_ProgressBar = new QProgressBar(this);
    m_ProgressBar->setMaximum(100);
    m_ProgressBar->setValue(0);
    pHLayout->addWidget(m_ProgressBar);

    spacer = new QSpacerItem(10, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);
    pHLayout->addSpacerItem(spacer);

    QPushButton * pb = new QPushButton(QString::fromUtf8("Прервать"), this);
    connect(pb, SIGNAL(clicked()), this, SLOT(slotCancel()));
    pHLayout->addWidget(pb);

    pLayout->addLayout(pHLayout);
}

void BlockDialog::setPercent(int percent)
{
    m_ProgressBar->setValue(percent);
}

void BlockDialog::slotCancel()
{
    if( QMessageBox::information(0, QString::fromUtf8("Внимание"), QString::fromUtf8("Прервать кэширование данных ?"), QMessageBox::Abort, QMessageBox::Cancel) == QMessageBox::Abort)
        m_cancelFlag = true;
}

