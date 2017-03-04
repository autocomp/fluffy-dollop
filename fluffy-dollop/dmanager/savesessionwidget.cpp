#include "savesessionwidget.h"

SaveSessionWidget::SaveSessionWidget(QStringList sessions, QStringList freeSession) :
    QDialog(0)
{
    m_allSessions = sessions;
    m_freeSessions = freeSession;
    setModal(true);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnBottomHint);
    m_pCbSessions = new QComboBox(this);
    m_pCbSessions->setEditable(false);

    foreach (QString str, sessions) {

        if(!freeSession.contains(str))
        {
            m_pCbSessions->insertItem(m_pCbSessions->count(),QIcon(":/447_key_close.png"),str);
        }
        else
        {
            m_pCbSessions->insertItem(m_pCbSessions->count(),str);
        }
    }
    connect(m_pCbSessions, SIGNAL(currentIndexChanged(QString)), this, SLOT(slotIndexChanged(QString)));

    m_pAddSession = new QPushButton(QIcon(":/25_create.png"),"",this);
    connect(m_pAddSession, SIGNAL(clicked(bool)), this, SLOT(slotAddNewSession()));

    m_pBtnOk = new QPushButton(QString::fromLocal8Bit("Сохранить"),this);
    connect(m_pBtnOk, SIGNAL(clicked(bool)), this, SLOT(accept()));
    m_pBtnCancel = new QPushButton(QString::fromLocal8Bit("Отмена"),this);
    connect(m_pBtnCancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

    m_pLineEdit = new QLineEdit(this);
    connect(m_pLineEdit, SIGNAL(textChanged(QString)), this, SLOT(slotCheckEnteredText(QString)));
    m_pAddSession->setEnabled(false);

    QVBoxLayout * lt = new QVBoxLayout(this);
    QHBoxLayout * addLt = new QHBoxLayout();
    QHBoxLayout * cbLt = new QHBoxLayout();
    QHBoxLayout * btnLt = new QHBoxLayout();

    m_pAddSession->setFixedSize(28,28);

    addLt->addWidget(m_pLineEdit);
    addLt->addWidget(m_pAddSession);

    cbLt->addWidget(m_pCbSessions);
    btnLt->addWidget(m_pBtnOk);
    btnLt->addWidget(m_pBtnCancel);

    lt->addLayout(addLt);
    lt->addLayout(cbLt);
    lt->addLayout(btnLt);
    lt->addStretch(1);
}


void SaveSessionWidget::slotIndexChanged(QString str)
{
    if(m_allSessions.contains(str) && !m_freeSessions.contains(str))
    {
        m_pBtnOk->setEnabled(false);
        return;
    }

    m_pBtnOk->setEnabled(true);
}

void SaveSessionWidget::slotCheckEnteredText(QString str)
{
    bool txtExist = false;
    if(m_allSessions.contains(str.simplified()))
    {
        txtExist = true;
    }

    if(str.isEmpty())
    {
        m_pAddSession->setEnabled(false);
        return;
    }

    m_pAddSession->setEnabled(!txtExist);
}

void SaveSessionWidget::slotAddNewSession()
{
    QString str = m_pLineEdit->text().simplified();
    if(!str.isEmpty())
    {
        m_pCbSessions->insertItem(m_pCbSessions->count(),str + "*");
        m_freeSessions.append(str);
        m_allSessions.append(str);
    }

    m_pLineEdit->clear();
}

QString SaveSessionWidget::getSelectedSession()
{
    return m_pCbSessions->currentText();
}
