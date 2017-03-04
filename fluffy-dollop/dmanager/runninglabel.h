#ifndef RUNNINGLABEL_H
#define RUNNINGLABEL_H

#include <QLabel>
#include <QWidget>
#include <QTimer>


#include <QQueue>
#include <QTextDocument>
#include <QMessageBox>

namespace ew {


class RunningLabel : public QLabel
{
    Q_OBJECT


public:
    enum TextDisplayMode
    {
        SCROLLING,
        CUT_OFF,
    };

    ///
    /// \brief RunningLabel QLabel с бегущей строкой
    /// \param parent
    ///
    explicit RunningLabel(QWidget * parent = 0, Qt::WindowFlags f = 0 );

    explicit RunningLabel(const QString & text, QWidget * parent = 0, Qt::WindowFlags f = 0);

    ///
    /// \brief setScrollingSpeed Установка скорости прокрутки в пикселях
    /// \param pixelPerSecond  скорость в пикселях в секунду
    ///
    void setScrollingSpeed(float pixelPerSecond)
    {
        _delta = pixelPerSecond*_updateFrequency;
    }

    ///
    /// \brief setText Установка текста ярлыка
    /// \param text
    ///
    void    setText        (QString text);

    ///
    /// \brief setTextSpacing Установка промежутка между текстом и его повтором при прокрутке
    /// \param ts Расстояние между текстом и его повтором в пикселах
    ///
    void    setTextSpacing (qint32 ts) ;
    qint32  textSpacing ()          ;
    QString text        ()          ;
    int textWidth(){return _textw;}

    TextDisplayMode textDisplayMode() const;
    void setTextDisplayMode(const TextDisplayMode &textDisplayMode);

signals:

public slots:
private:
    void moveText();
    void paintEvent ( QPaintEvent * event );
    qint32  _updateFrequency;
    qint32  _textSpacing;
    qreal   _offset;
    qreal   _delta;
    QTextDocument _doc;
    qint32  _textw;
    QTimer  _updateTimer;

    virtual void resizeEvent ( QResizeEvent * event );
    virtual void mousePressEvent(QMouseEvent *e);
    virtual void mouseMoveEvent(QMouseEvent *e);
    virtual void mouseReleaseEvent(QMouseEvent *e);


    QMessageBox box;
    TextDisplayMode m_textDisplayMode;



protected slots:
    void on_updateTimer_timeout();


};
}
#endif // RUNNINGLABEL_H
