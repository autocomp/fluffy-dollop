#ifndef TRANSPARENTSCROLLAREA_H
#define TRANSPARENTSCROLLAREA_H

#include <QScrollArea>
#include <QEvent>
#include <QResizeEvent>

class TransparentScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    explicit TransparentScrollArea(QWidget *parent = 0);
    
protected:
    void resizeEvent(QResizeEvent *e);
    void showEvent(QShowEvent *e);
    void hideEvent(QHideEvent *e);
signals:
    void signalSizeUpdated();
public slots:

    
};

#endif // TRANSPARENTSCROLLAREA_H
