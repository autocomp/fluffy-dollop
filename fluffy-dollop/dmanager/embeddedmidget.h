#ifndef EMBEDDEDMIDGET_H
#define EMBEDDEDMIDGET_H

#include <QDialog>
#include <QWidget>
#include <QLabel>
namespace ew {


class EmbeddedMidget : public QWidget
{
    Q_OBJECT

    friend class EmbeddedApp;
    friend class EmbeddedMainWindow;
    friend class EmbeddedWindow;

    quint64 m_id;

    int showIconSize;
    QPoint m_iconLabelPos;
    QLabel *m_pTextLabel, *m_pInnerWidget, *m_pIconLabel;
    QPixmap m_rightArrow, m_leftArrow, m_topArrow, m_bottomArrow;
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY nameChanged)
    QFont m_font;
    QString m_text;
    bool dragModeOn;
    QPoint delta;
    QRect oldGeometry;
    Qt::AnchorPoint m_anchorPoint;

public:

    QRect getSavedGeometry(){return oldGeometry;}
    void setSavedGeometry(QRect r){oldGeometry = r;}

private:
    void paintEvent       ( QPaintEvent * event );
    void mousePressEvent  ( QMouseEvent * event );
    void mouseReleaseEvent( QMouseEvent * event );
    void mouseMoveEvent   ( QMouseEvent * event );


    void recalcIconPos(int w, int h);
protected:
    explicit EmbeddedMidget(QWidget *parent = 0);
    EmbeddedMidget(QString name,QWidget *parent = 0);
    ~EmbeddedMidget();
    void setId(quint64 mId){m_id = mId;}
    quint64 id(){return m_id;}
    QString text();
    void setText(QString text);
    void setAnchorPoint(Qt::AnchorPoint p);


    void commitGeometry();
    void revertGeometry();

    void setIcon(const QPixmap &pm);
    void resizeEvent(QResizeEvent *event);
signals:
    void nameChanged();
    void signalMidgetMoveRequest (QRect geometry);
    void signalMidgetPlaced(QRect geometry);
public:
    bool isDrag(){ return dragModeOn; }

private slots:
    void slotCheckStartMove();
};
}
#endif // EMBEDDEDMIDGET_H
