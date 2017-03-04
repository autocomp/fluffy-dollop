#include "curtaillabel.h"
#include <QBrush>
#include <QPen>


CurtailLabel::CurtailLabel(QWidget *parent) : QLabel(parent)
{
//    this setSizePolicy();
}

QString CurtailLabel::text() const
{
    return m_text;
}

void CurtailLabel::setText(const QString &text)
{
    QLabel::setText(this->fontMetrics().elidedText(text,Qt::ElideRight,width()));
    m_text = text;
}

void CurtailLabel::resizeEvent(QResizeEvent *)
{
    QLabel::setText(this->fontMetrics().elidedText(m_text,Qt::ElideRight,width()));
}

