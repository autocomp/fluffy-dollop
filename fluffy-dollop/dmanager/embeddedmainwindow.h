#ifndef EMBEDDEDMAINWINDOW_H
#define EMBEDDEDMAINWINDOW_H

#include "embeddedstruct.h"
#include "embeddedwindow.h"

namespace ew {

class EmbeddedApp;
class EmbeddedMainWindow : public EmbeddedWindow
{
    Q_OBJECT

    friend class EmbeddedApp;

protected:
    explicit EmbeddedMainWindow(QWidget * parent = 0);
    ~EmbeddedMainWindow();
    QList<EmbeddedWidget*> childs;
public:

    virtual EmbeddedWidgetType widgetType();

    virtual void setWidget(EmbeddedWidgetBaseStructPrivate *embStrPriv);
    virtual void addWidget(ew::EmbeddedWidgetBaseStructPrivate * embStrPriv, QPoint insertedPos = QPoint(0,0));
    virtual void removeWidget(ew::EmbeddedWidgetBaseStructPrivate * embStrPriv);

//    virtual void setWidget(QWidget *mainWidget);
//    virtual void addWidget(QWidget * wdg){};
//    virtual void addWidget(EmbeddedWindow * embWindow){};
//    void addChild(EmbeddedWidget*cld);
//    void

protected:
    /*virtual*/ QRect getParkingRect();
    void closeEvent(QCloseEvent *e);
private slots:
//    void deleteChild(QObject*);
//    void checkChildBorders(QRect rect);
//    void checkChildGroups(QRect rect);
//    void slotDragGlobPosChanged(QRect,QPoint,QMouseEvent::Type);
};
}

#endif // EMBEDDEDMAINWINDOW_H
