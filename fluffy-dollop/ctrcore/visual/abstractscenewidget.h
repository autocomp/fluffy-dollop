#ifndef ABSTRACTSCENEWIDGET_H
#define ABSTRACTSCENEWIDGET_H

#include <QWidget>
#include <QResizeEvent>

namespace sw {

class AbstractSceneWidget : public QWidget
{
    Q_OBJECT
public:
    AbstractSceneWidget(QWidget *pParent = 0);
    virtual void resizeEvent(QResizeEvent *e);

signals:
    void newSize(int, int);
};
}
#endif // ABSTRACTSCENEWIDGET_H
