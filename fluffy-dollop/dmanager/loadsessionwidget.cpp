#include "loadsessionwidget.h"

LoadSessionWidget::LoadSessionWidget(QStringList sessions) :
    QDialog(0)
{
    setModal(true);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnBottomHint);
    m_pCbSessions = new QComboBox(this);
    m_pCbSessions->insertItems(0,sessions);

    m_pBtnOk = new QPushButton(QString::fromLocal8Bit("Загрузить"),this);
    connect(m_pBtnOk, SIGNAL(clicked(bool)), this, SLOT(accept()));
    m_pBtnCancel = new QPushButton(QString::fromLocal8Bit("Отмена"),this);
    connect(m_pBtnCancel, SIGNAL(clicked(bool)), this, SLOT(reject()));

    QVBoxLayout * lt = new QVBoxLayout(this);
    QHBoxLayout * cbLt = new QHBoxLayout();
    QHBoxLayout * btnLt = new QHBoxLayout();


    lt->addLayout(cbLt);
    lt->addLayout(btnLt);

    cbLt->addWidget(m_pCbSessions);
    btnLt->addWidget(m_pBtnOk);
    btnLt->addWidget(m_pBtnCancel);
}



QString LoadSessionWidget::getSelectedSession()
{
    return m_pCbSessions->currentText();
}
