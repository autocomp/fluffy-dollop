#include "runninglabel.h"
#include <QFontMetrics>
#include <QMetaObject>
#include <QPainter>
#include <qdebug.h>
#include <QApplication>
#include <QTextDocument>
#include <QStyleOption>
#include <QRgb>
#include <QMouseEvent>
#include <QMessageBox>
#include <QFlags>
#include <QUrl>


ew::RunningLabel::RunningLabel(QWidget * parent , Qt::WindowFlags f)
    : QLabel(parent,f)
    , _updateFrequency(20)
    , _textSpacing(30)
    , _offset(0)
    , _delta (1)
    , _doc()
{
    setTextFormat(Qt::RichText);
    setMinimumHeight(fontMetrics().height()+margin()*2);
//    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
    _updateTimer.setInterval(_updateFrequency);
    _updateTimer.setSingleShot(false);
    setMargin(0);
    _textw = fontMetrics().width(_doc.toPlainText());
    connect(&_updateTimer,SIGNAL(timeout()),SLOT(on_updateTimer_timeout()));

    _updateTimer.start();
}

ew::RunningLabel::RunningLabel(const QString &text, QWidget *parent, Qt::WindowFlags f):
    QLabel(text,parent,f)
  , _updateFrequency(20)
  , _textSpacing(30)
  , _offset(0)
  , _delta (1)
  , _doc()
{
    setTextFormat(Qt::RichText);
    setMinimumHeight(fontMetrics().height()+margin()*2);
//    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Minimum);
    _updateTimer.setInterval(_updateFrequency);
    _updateTimer.setSingleShot(false);
    setMargin(0);
    _textw = fontMetrics().width(_doc.toPlainText());
    connect(&_updateTimer,SIGNAL(timeout()),SLOT(on_updateTimer_timeout()));
    setText(text);

    _updateTimer.start();
}

void ew::RunningLabel::setText(QString text)
{
    _doc.setHtml("<span style=\" &stylesheet;\">"+text.replace(' ',"&nbsp;")+"</span>");
    _textw = fontMetrics().width(_doc.toPlainText());
//    setMaximumWidth(fontMetrics().width(_doc.toPlainText())+margin()*2);
}

void ew::RunningLabel::setTextSpacing(qint32 ts)
{
    _textSpacing = ts;
}

qint32 ew::RunningLabel::textSpacing()
{
    return _textSpacing;
}

QString ew::RunningLabel::text()
{
    return _doc.toPlainText();
}



void ew::RunningLabel::moveText()
{
    if(width()-margin()*2 > _textw-1)
    {
        _offset = 0;
        _updateTimer.stop();
        return;
    }
    if(_offset > _textw+textSpacing()-1)
    {
        _offset = 0;
    }
    else
    {
        if(_textw > width() )
        {
            _offset +=_delta;
        }
        else
        {
            _offset = 0;
        }
    }
}

void ew::RunningLabel::paintEvent(QPaintEvent *event)
{
Q_UNUSED(event);
    QPainter p(this);
    QBrush br(Qt::darkGray);

    p.setBrush(br);

    QStyleOption opt;
    opt.init(this);
    QPalette palette = opt.palette;

    int textHeight(fontMetrics().height());
    enum
    {
        ALIGN_RIGHT,
        ALIGN_LEFT,
        ALIGN_H_CENTER,
    } labelHAlign;
    enum
    {
        ALIGN_TOP,
        ALIGN_BOTTOM,
        ALIGN_V_CENTER,
    } labelVAlign;

    if(alignment().testFlag(Qt::AlignLeft))
    {
        labelHAlign = ALIGN_LEFT;
    }
    else if (alignment().testFlag(Qt::AlignRight))
    {
        labelHAlign = ALIGN_RIGHT;
    }
    else
    {
        labelHAlign = ALIGN_H_CENTER;
    }

    if(alignment().testFlag(Qt::AlignTop))
    {
        labelVAlign = ALIGN_TOP;
    }
        else if(alignment().testFlag(Qt::AlignBottom))
    {
        labelVAlign = ALIGN_BOTTOM;
    }
    else
    {
        labelVAlign = ALIGN_V_CENTER;
    }

    QString html = _doc.toHtml();
    if(html.indexOf("color:") == -1)
    {
        if(html.indexOf("<span style=\"") != -1)
        {
            html.replace("<span style=\"",QString("<span style=\"color:%1; ").arg(palette.color(QPalette::WindowText     ).name()));
        }
        else if (html.indexOf("<p style=\"") != -1)
        {
            html.replace("<p style=\"",QString("<p style=\"color:%1; ").arg(palette.color(QPalette::WindowText     ).name()));
        }
    }
    else
    {
        html.replace(QRegExp("color:#[0-9,a-f,A-F]*;"),QString("color:%1;").arg(palette.color(QPalette::WindowText     ).name()));
    }
    if(_doc.toHtml()!=html)
    {
        _doc.setHtml(html);
    }


    qreal hAlignOffset=0;
    qreal vAlignOffset=0;

    switch (labelVAlign) {
    case ALIGN_TOP:
        vAlignOffset = 0;
        break;
    case ALIGN_BOTTOM:
        vAlignOffset = height() - textHeight;
        break;
    default:
        vAlignOffset = (height() - textHeight)/2;
        break;
    }

    if((_textw)<=(width()))
    {

        switch (labelHAlign) {
        case ALIGN_LEFT:
            hAlignOffset = 0;
            break;
        case ALIGN_RIGHT:
            hAlignOffset = width() -_textw;
            break;
        default:
            hAlignOffset = (width() - _textw)/2;
            break;
        }
    }

    p.translate(-_offset+hAlignOffset-4,vAlignOffset-4);
    _doc.drawContents(&p);

    if((_textw)>(width()))
    {
        QPainter p1(this);
        p1.setBrush(br);
        p1.translate(-_offset+_textw+_textSpacing-4,vAlignOffset-4);
        _doc.drawContents(&p1);
    }

}

void ew::RunningLabel::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    if(width() > _textw)
    {
        _updateTimer.stop();
        _offset = 0;
    }
    _updateTimer.start();
    update();

}

void ew::RunningLabel::mousePressEvent(QMouseEvent *e)
{
    e->ignore();
}

void ew::RunningLabel::mouseMoveEvent(QMouseEvent *e)
{
//    QLabel::mouseMoveEvent(e);
    e->ignore();
    //    parent()->eventFilter(parent(),e);
}

void ew::RunningLabel::mouseReleaseEvent(QMouseEvent *e)
{
    e->ignore();
}


void ew::RunningLabel::on_updateTimer_timeout()
{
    moveText();
    update();
}
