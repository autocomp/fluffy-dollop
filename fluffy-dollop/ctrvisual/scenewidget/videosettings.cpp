#include "videosettings.h"
#include <QFileDialog>
#include <QSettings>

using namespace sw;

VideoSettings::VideoSettings(VideoPlayerTypes::SettingsType sType, QWidget *parent) :
    QDialog(parent)
{

    setModal(true);

    QVBoxLayout * mainLt =new QVBoxLayout(this);

    QGroupBox * gBoxParams = new QGroupBox(tr("Record params"));


    QGridLayout * gridParams = new QGridLayout(gBoxParams);


    setWindowTitle(tr("Video record params"));
    setWindowIcon(QIcon(":/89_manage.png"));
    m_pFormatLbl = new QLabel(tr("File format:"),gBoxParams);
    m_pDestLbl = new QLabel(tr("File destination:"),gBoxParams);
    m_pPrefixLbl = new QLabel(tr("File prefix:"),gBoxParams);

    m_pDenyMinimizeCb = new QCheckBox(tr("Deny minimize where record started"),gBoxParams);
    connect(m_pDenyMinimizeCb, SIGNAL(clicked(bool)), this, SLOT(slotDenyMinimizeClicked(bool)));
    m_pDenyCloseCb = new QCheckBox(tr("Deny close where record started"),gBoxParams);
    connect(m_pDenyCloseCb, SIGNAL(clicked(bool)), this, SLOT(slotDenyCloseClicked(bool)));

    m_pFilepathLineEdit = new QLineEdit(gBoxParams);
    connect(m_pFilepathLineEdit, SIGNAL(textChanged(QString)), this, SLOT( slotFilepathChanged(QString)) );
    m_pPrefixLineEdit = new QLineEdit(gBoxParams);
    connect(m_pPrefixLineEdit, SIGNAL(textChanged(QString)), this, SLOT( slotPrefixChanged(QString)) );

    m_pFilepathPb = new QPushButton("...",gBoxParams);
    connect(m_pFilepathPb, SIGNAL(clicked()), this, SLOT(slotFilepathButtonClicked()));
    m_pFilepathPb->setFixedSize(36,32);


    m_pFormatComboBox = new QComboBox(gBoxParams);
    m_pFormatComboBox->addItem(tr("Auto"));
    connect(m_pFormatComboBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotFormatChanged(QString)));



    gridParams->addWidget(m_pFormatLbl,0,0,1,1);
    gridParams->addWidget(m_pFormatComboBox,0,1,1,1);


    gridParams->addWidget(m_pDestLbl,1,0,1,1);

    gridParams->addWidget(m_pFilepathLineEdit,2,0,1,1);
    gridParams->addWidget(m_pFilepathPb,2,1,1,1);

    gridParams->addWidget(m_pPrefixLbl,3,0,1,1);

    gridParams->addWidget(m_pPrefixLineEdit,4,0,1,1);

    gridParams->addWidget(m_pDenyMinimizeCb,5,0,1,1);
    gridParams->addWidget(m_pDenyCloseCb,6,0,1,1);

    mainLt->addWidget(gBoxParams,1);


    /*Effects*/

    QGroupBox * gBoxEffects = new QGroupBox(tr("Effects"));
    QGridLayout * gridEffects = new QGridLayout(gBoxEffects);

    m_pVisibleControlsCheckBox = new QCheckBox(tr("Visisble controls"),gBoxParams);

    gridEffects->addWidget(m_pVisibleControlsCheckBox,0,0,1,1);
        connect(m_pVisibleControlsCheckBox, SIGNAL(clicked(bool)), this, SLOT(slotRecordControlsClicked(bool)));


    mainLt->addWidget(gBoxEffects,1);


    /*ok cancel buttons*/

    QHBoxLayout * buttonLt = new QHBoxLayout(this);
    mainLt->addLayout(buttonLt);

    m_pOkPb = new QPushButton(tr("OK"), this);
    connect(m_pOkPb, SIGNAL(clicked()), this, SLOT(slotOkClicked()));

    m_pCancelPb = new QPushButton(tr("Cancel"), this);
    connect(m_pCancelPb, SIGNAL(clicked()), this, SLOT(slotCancelClicked()));

    buttonLt->addStretch(1);
    buttonLt->addWidget(m_pOkPb);
    buttonLt->addWidget(m_pCancelPb);

    m_settings.sType = sType;
}



void VideoSettings::slotOkClicked()
{
    hide();

    emit signalSavedSettings(m_settings);
}


void VideoSettings::slotPrefixChanged(QString str)
{
    m_settings.filePrefix = str;
}


void VideoSettings::setSettings(VideoPlayerTypes::VideoSaveSettings sett)
{
    m_settings = sett;
    m_pFilepathLineEdit->setText(m_settings.fileDestination);
    m_pPrefixLineEdit->setText(m_settings.filePrefix);

    for(int i = 0; i<m_pFormatComboBox->count(); i++)
    {
        if(m_pFormatComboBox->itemText(i) == m_settings.fileFormat)
            m_pFormatComboBox->setCurrentIndex(i);
    }


    m_pVisibleControlsCheckBox->setChecked( m_settings.recordControls);
    m_pDenyMinimizeCb->setChecked( m_settings.denyMinimizeWindow);
    m_pDenyCloseCb->setChecked( m_settings.denyCloseWindow);

    emit signalSavedSettings(m_settings);
}

void VideoSettings::slotFilepathChanged(QString str)
{
    m_pFilepathLineEdit->setText(str);
    m_settings.fileDestination = str;

}


void VideoSettings::slotFormatChanged(QString str)
{
    m_settings.fileFormat = str;
}


void VideoSettings::slotDenyMinimizeClicked(bool val)
{
    m_settings.denyMinimizeWindow = val;
}


void VideoSettings::slotDenyCloseClicked(bool val)
{
    m_settings.denyCloseWindow = val;
}


void VideoSettings::slotRecordControlsClicked(bool val)
{
    m_settings.recordControls = val;
}

void VideoSettings::slotCancelClicked()
{
    hide();
}

void VideoSettings::slotFilepathButtonClicked()
{
    QFileDialog dialog(this);
    dialog.setWindowTitle(tr("Select saved directory"));

    QString openDir ;
    if(m_settings.fileDestination.isEmpty())
        openDir = QDir::homePath();
    else
        openDir = m_settings.fileDestination;

    QString filepath = dialog.getExistingDirectory(this,tr("Select saved directory"),openDir,QFileDialog::ShowDirsOnly);
    m_settings.fileDestination = filepath;
    m_pFilepathLineEdit->setText(filepath);
}
