#ifndef EMBEDDEDBORDER_H
#define EMBEDDEDBORDER_H

#include <QWidget>

namespace ew{
class EmbeddedBorder : public QWidget
{
    Q_OBJECT
public:
    enum EmbeddedBorderType
    {
        EBT_LEFT,
        EBT_LEFT_TOP,
        EBT_TOP,
        EBT_RIGHT_TOP,
        EBT_RIGHT,
        EBT_RIGHT_BOTTOM,
        EBT_BOTTOM,
        EBT_LEFT_BOTTOM,
    };
    explicit EmbeddedBorder(QWidget *parent = 0);
    EmbeddedBorder(EmbeddedBorderType ebt, QWidget *parent = 0);
    void settType();
signals:

public slots:
    void checkPosition(QSize size);
private:
    EmbeddedBorderType _borderType;
    quint16 _borderWidth;
    Qt::CursorShape _shape;
    void setCursors();


    bool _resizeModeOn;
    void mouseMoveEvent ( QMouseEvent *event ) ;
    void mousePressEvent ( QMouseEvent *event ) ;
    void mouseReleaseEvent ( QMouseEvent * event );

signals:
    void signalMouseMove ( QMouseEvent *event );
    void signalMousePress ( QMouseEvent *event );
    void signalMouseRelease ( QMouseEvent *event );


//#define DEBUG_MODE_SAW
#ifdef DEBUG_MODE_SAW
    void paintEvent ( QPaintEvent * event );
#endif

};
}

#endif // EMBEDDEDBORDER_H
