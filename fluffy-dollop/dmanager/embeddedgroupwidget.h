#ifndef EMBEDDEDGROUPWIDGET_H
#define EMBEDDEDGROUPWIDGET_H

#include "embeddedgroupheader.h"
#include "embeddedprivate.h"
#include "embeddedstruct.h"
#include "embeddedwidget.h"
//#include "embeddedapp.h"

namespace ew {
class EmbeddedApp;
class EmbeddedGroupWidget : public EmbeddedWidget
{
    Q_OBJECT

    friend class EmbeddedApp;
    QWidget *m_externalParentWidget;

protected:
    explicit EmbeddedGroupWidget(QWidget *parent = 0);
    virtual void setHeader(const EmbeddedHeaderStruct & headerStruct);
    QSize        sizeHint() const;

public:
    void                          setExternalParent(QWidget *widget);
    virtual ew::EmbeddedGroupType groupType();
    ew::EmbeddedWidgetType        widgetType();
    virtual void                  addWidget(ew::EmbeddedWidgetBaseStructPrivate *embStrPriv, QPoint insertedPos = QPoint(0, 0));

public slots:
    virtual void slotHeaderWindowEvents(EmbeddedHeader::WindowHeaderEvents ev, bool *acceptFlag);//NOTE: Добавил слот обработки событий хедера

protected slots:
    void slotExternalParentDestroyed(QObject *);
};
}

#endif // EMBEDDEDGROUPWIDGET_H
