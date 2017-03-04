#include "embeddedapp.h"
#include "embeddedgroupwidget.h"
#include "embeddedpanel.h"
#include "embeddedutils.h"

using namespace ew;

EmbeddedGroupWidget::EmbeddedGroupWidget(QWidget *parent) :
    EmbeddedWidget(parent)
{
    m_pHeaderWidget = 0;
    m_pViewPanel = new EmbeddedPanel(this);
    m_pMainViewWidgetLayout->addWidget(m_pViewPanel);
}


void EmbeddedGroupWidget::addWidget(ew::EmbeddedWidgetBaseStructPrivate *embStrPriv, QPoint insertedPos)
{
    m_pViewPanel->insertEmbeddedWidgetForce(embStrPriv, insertedPos);
}


void EmbeddedGroupWidget::slotHeaderWindowEvents(EmbeddedHeader::WindowHeaderEvents ev, bool *acceptFlag)
{
    EmbeddedWidget::slotHeaderWindowEvents(ev, acceptFlag);

    switch(ev)
    {
      case EmbeddedHeader::WHE_OPTIMIZE_SPACE:
      {
          m_pViewPanel->optimizeSpace();
      } break;
      case EmbeddedHeader::WHE_OPTIMIZE_SIZES:
      {
          m_pViewPanel->optimizeWidgetSizes();
      } break;
      default:
      {
          EmbeddedWidget::slotHeaderWindowEvents(ev, acceptFlag);
      }
    }

    updateGeometry();
}


ew::EmbeddedGroupType EmbeddedGroupWidget::groupType()
{
    return EGT_WIDGET_GROUP;
}


EmbeddedWidgetType EmbeddedGroupWidget::widgetType()
{
    return EWT_GROUP;
}


void EmbeddedGroupWidget::setHeader(const EmbeddedHeaderStruct &headerStruct)
{
    bool needEnableUpdates = true;

    if(updatesEnabled())
    {
        needEnableUpdates = true;
        setUpdatesEnabled(false);
    }

    delete m_pHeaderWidget;
    m_pHeaderWidget = 0;

    m_pHeaderWidget = new ew::EmbeddedWidgetHeader(this);

    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_STYLE, headerStruct.hasStyleButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_SETTINGS, headerStruct.hasSettingsButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_PINUNPIN, headerStruct.hasPinButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_MIDGET, headerStruct.hasMidgetButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_LOCKUNLOCK, headerStruct.hasLockButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_HIDE_HEADER, headerStruct.hasHideHeaderButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_GLOBAL_ONPARENT_MOVE, headerStruct.hasGlobalMoveButton);

    /// FIXME добавить в структуру
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_OPTIMIZE_SIZES, true);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_OPTIMIZE_SPACE, true);



    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_COLLAPSE, headerStruct.hasCollapseButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_FULLNORMSCREEN, headerStruct.hasMinMaxButton);
    m_pHeaderWidget->setVisisbleButton(EmbeddedHeader::WHB_CLOSE, headerStruct.hasCloseButton);

    m_pHeaderWidget->setToolTip(headerStruct.tooltip);
    m_pHeaderWidget->setText(headerStruct.windowTitle);
    m_pHeaderWidget->setDescription(headerStruct.descr);
    m_pHeaderWidget->setIcon(headerStruct.headerPixmap);

    setWindowTitleText(headerStruct.windowTitle);

    m_pMainLayout->insertWidget(0, m_pHeaderWidget);

    connect(m_pHeaderWidget, SIGNAL(signalWindowEvent(EmbeddedHeader::WindowHeaderEvents,bool *))
            , this, SLOT(slotHeaderWindowEvents(EmbeddedHeader::WindowHeaderEvents,bool *)));


    connect(m_pHeaderWidget, SIGNAL(signalMoveWindowRequest(int,int,bool *))
            , this, SLOT(slotMoveWindowRequest(int,int,bool *)));

    connect(m_pHeaderWidget, SIGNAL(signalWindowDropped()),
            SLOT(onSignalWindowDropped()));

    if(needEnableUpdates)
    {
        setUpdatesEnabled(true);
    }

    m_pHeaderWidget->installEventFilter(this);
}


QSize EmbeddedGroupWidget::sizeHint() const
{
    const auto strPtr = ewApp()->getWidgetStructById(id());

    if(strPtr)
    {
        if(strPtr->size.isEmpty() || (!strPtr->size.isValid())
           || strPtr->size.isNull())
        {
            QRect r = m_pViewPanel->getContentRect();
            QSize res = QSize(r.right(), r.bottom());
            return res;
        }
        QSize sz = EmbeddedUtils::getCorrectSize(strPtr->size, strPtr->minSize, strPtr->maxSize);
        return sz;
    }

    return QWidget::sizeHint();

}


void EmbeddedGroupWidget::setExternalParent(QWidget *widget)
{
    m_externalParentWidget = widget;
    connect(m_externalParentWidget, SIGNAL(destroyed(QObject *)), this, SLOT(slotExternalParentDestroyed(QObject *)));
}


void EmbeddedGroupWidget::slotExternalParentDestroyed(QObject *)
{
    m_externalParentWidget = 0;
}
