#include "rowscalecontrol.h"
#include <QPainter>
#include <QDebug>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QApplication>

RowScaleControl::RowScaleControl(QWidget *parent) :
    QWidget(parent)
{
//    setStyleSheet("ScrollPanel { background-color:rgba(130,130,130,0); }");
    setFixedSize(200,40);

    QWidget * wdg = new QWidget(this);
    wdg->setObjectName("RowScaleControl_Background");
    m_pSpinBox = new  QSpinBox(this);
    m_pSpinBox->setMinimum(0);
    m_pSpinBox->setMaximum(10000);
    m_pSpinBox->setObjectName("RowScaleControl_SpinBox");
    //installEventFilter(m_pSpinBox);
    connect(m_pSpinBox, SIGNAL(editingFinished()), this, SLOT(slotEditScaleFinished()));
    // m_pSpinBox->setFocusProxy(this);

    m_pOkButton =  new QPushButton(tr("OK"));
    m_pOkButton->setObjectName("RowScaleControl_OkButton");
    m_pComboBox = new QComboBox(this);
    m_pComboBox->setObjectName("RowScaleControl_ComboBox");

    m_pComboBox->addItem(tr("cm"), (int)SN_CM);
    m_pComboBox->addItem(tr("dm"), (int)SN_DM);
    m_pComboBox->addItem(tr("m"), (int)SN_M);
    m_pComboBox->addItem(tr("km"), (int)SN_KM);
    connect(m_pComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(setFocus()));
    //installEventFilter(m_pComboBox);
    //m_pComboBox->setFocusProxy(this);


    QHBoxLayout * lt = new QHBoxLayout(this);
    lt->setMargin(0);
    lt->setSpacing(0);
    lt->addWidget(wdg);


    m_pLayout = new QHBoxLayout(wdg);
    m_pLayout->setContentsMargins(3,3,3,3);
    m_pLayout->setSpacing(3);
    //   m_pLayout->setMargin(2);
    //   m_pLayout->setSpacing(2);
    m_pLayout->addWidget(m_pSpinBox);
    m_pLayout->addWidget(m_pComboBox);
//    m_pLayout->addSpacing(5);
    m_pLayout->addWidget(m_pOkButton);
    m_pOkButton->setFixedWidth(32);


    connect(m_pOkButton, SIGNAL(clicked()), this, SLOT( slotOkButtonClicked()  ));
}


void RowScaleControl::slotEditScaleFinished()
{
    QWidget * wdg = QApplication::focusWidget();
    if(this != wdg && m_pComboBox != wdg && m_pSpinBox != wdg && m_pOkButton != wdg)
    {
        hide();
    }
}

bool RowScaleControl::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::FocusOut)
    {
        QWidget * wdg = QApplication::focusWidget();
        if(this == wdg || m_pComboBox == wdg || m_pSpinBox == wdg)
        {
            return QObject::eventFilter(obj, event);
        }

        while(0 != wdg)
        {
            wdg = wdg->parentWidget();
            if(this == wdg )
            {
                return QObject::eventFilter(obj, event);
            }
        }

        hide();
        return QObject::eventFilter(obj, event);
    } else
    {
        // standard event processing
        return QObject::eventFilter(obj, event);
    }
}

void RowScaleControl::showEvent(QShowEvent *e)
{
    setFocus();
    QWidget::showEvent(e);

}
void RowScaleControl::slotOkButtonClicked()
{
    hide();
    int value = m_pSpinBox->value();
    ScaleNums num = (ScaleNums)(m_pComboBox->itemData(m_pComboBox->currentIndex()).toInt());
    qreal meters = 0;

    switch (num)
    {
    case SN_CM:
    {
        meters = value*0.01;
    }break;
    case SN_DM:
    {
        meters = value*0.1;
    }break;
    case SN_M:
    {
        meters = value;
    }break;
    case SN_KM:
    {
        meters = value*1000;
    }break;

    default:
    {
        qDebug() << "RowScaleControl::slotOkButtonClicked(): ERROR! Unsupported SN value!";
        return;
    }
        break;
    }

    emit signalSetCurrentScaleMeters(meters);
}

void RowScaleControl::focusOutEvent(QFocusEvent *e)
{
    QWidget * wdg = QApplication::focusWidget();
    while(0 != wdg)
    {
        wdg = wdg->parentWidget();
        if(this == wdg )
        {
            QWidget::focusOutEvent(e);
            return;
        }
    }

    QWidget::focusOutEvent(e);
    hide();
}

void RowScaleControl::mousePressEvent(QMouseEvent *e)
{
    setFocus();
    QWidget::mousePressEvent(e);
}

void RowScaleControl::keyPressEvent(QKeyEvent *k)
{
    if(Qt::Key_Escape == k->key() )
    {
        hide();
    }
    else
    {
        QWidget::keyPressEvent(k);
    }

}
