#ifndef ROWSCALEINNER_H
#define ROWSCALEINNER_H

#include <QWidget>

class RowScaleInner : public QWidget
{
    Q_OBJECT
    bool m_showDistValue;
    qreal m_upScalePixelLen, m_downScalePixelLen;
public:
    explicit RowScaleInner(QWidget *parent = 0);
    void drawScale();
    void switchToMeasureDistance();
    void switchToScale();
    void setScaleLen(qreal upScalePixelLen, qreal downScalePixelLen);
protected:
    void paintEvent(QPaintEvent * e);

signals:

public slots:
};

#endif // ROWSCALEINNER_H
