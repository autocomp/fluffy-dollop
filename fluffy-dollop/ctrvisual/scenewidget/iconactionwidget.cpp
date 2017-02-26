#include "iconactionwidget.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QPalette>

IconActionWidget::IconActionWidget(QIcon icon, QString &text, QWidget *parent) :
    QWidgetAction(parent)
{
    m_icon = icon;
    m_clickedCount = 0;
    m_text = text;
    m_isEnabled = true;
    isVisisble = true;
}


QWidget* IconActionWidget::createWidget(QWidget *parent) {

    m_wdg = new QWidget(parent);
    //m_icon = new QIcon(QString(":/images/calcAngles3D.png"));
    QHBoxLayout * lt = new QHBoxLayout(m_wdg);

    m_lblIcon = new QLabel(m_wdg);
    m_lblIcon ->setPixmap((m_icon.pixmap(QSize(25, 25) )));
    lt->addWidget(m_lblIcon );



    //QWidgetAction::setIcon(QIcon(m_icon));

    QLabel * lbl = new QLabel(m_text, m_wdg);

    if(!m_isEnabled)
    {
        QPalette pal = lbl->palette();
        pal.setColor(QPalette::WindowText, Qt::gray);
        lbl->setPalette(pal);
    }

    lt->addWidget(lbl,10, Qt::AlignLeft);

    m_wdg->setLayout(lt);



    return m_wdg;
}

void IconActionWidget::setVisible(bool isVis)
{
    isVisisble = isVis;
}

void IconActionWidget::setEnabled(bool isEnabled)
{
    m_isEnabled = isEnabled;
}

void IconActionWidget::setIcon(const QIcon& icon)
{
    m_icon = icon;
    QPixmap  pm = (m_icon.pixmap(QSize(25, 25) ));

    QWidgetAction::setIcon(icon);
}

