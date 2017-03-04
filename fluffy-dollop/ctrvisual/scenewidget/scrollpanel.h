#ifndef INNERSCROLLPANEL_H
#define INNERSCROLLPANEL_H

#include <QScrollArea>
#include <QWidget>
#include <QVBoxLayout>
#include <QPen>
#include <QBrush>
#include "transparentscrollarea.h"

#include "collapsedwidget.h"
class ScrollPanel : public QFrame
{
    Q_OBJECT
    QPixmap m_backgroundPixmap;
    bool m_needUpdateBackground;
    QWidget *m_pInnerWidget, *m_pInnerEmptyWidget, * m_pInnerTopWidget;
    QVBoxLayout * m_pMainLayout;
    QList<CollapsedWidget*> m_widgets;
    QList<CollapsedWidget*> m_visisbleListWidget;
    TransparentScrollArea * m_pScrollArea;
    qreal m_painterBackgroundOpacity;
    QPen m_pen, m_borderPen;
    QBrush m_brush/*, m_borderBrush*/;
    bool m_isInnerPanel;
    QList<CollapsedWidget*> m_widgetSet;
    bool m_showFrameBorder, m_showFullFrame;
    bool m_isVisible;
public:
    explicit ScrollPanel(bool isInnerPanel, QWidget *parent = 0);
    void setBackgroundImage(QPixmap pm);
    void appendWidget(CollapsedWidget * widget, bool visible = true);
    void removeWidget(CollapsedWidget * widget);
    int insertedWidgetCount(){return m_widgets.count();}
    int visibleWidgetCount(){return m_visisbleListWidget.count();}
    QList<CollapsedWidget*> takeAllWidgets();

    /**
     * @brief Добавляет виджет в данный бокс. До момента добавления должна быть вызвана функция addWidgetToSet
     * @param widgets
     */
    void appendWidgets(const QList<CollapsedWidget*> &widgets);
    void setWidgetSet(QList<CollapsedWidget*> widgetSet);    
    /**
     * @brief Функция добавления виджета в набор. Означает, что встраиваемый виджет при перемещении может быть засунут в данный Box.
     *      Если виджет не добавлен к набору, то он не может быть встроен в данный Box
     * @param widget
     */
    void addWidgetToSet(CollapsedWidget* widget);

    void removeWidgetFromSet(CollapsedWidget* widget);
    void hide();
    void show();
    bool isVisible();
    int prefferHeight();
    bool isNeedShowFull();
    void setMinimumWidth(int minw);
    void setFixedWidth(int minw);
protected:
    void paintEvent(QPaintEvent *ev);
signals:
    void signalResendUnusedEvents(QEvent * event);
    void signalGeomChanged();
    
public slots:
    void setVisible(bool visible);
    void slotVisisbleNeedChange(bool);
private slots:
    void slotWidgetPositionFromSetChanged(QRect widgetGeom, QPoint pressedMousePos, QMouseEvent::Type eventType);
    void slotInsertedWidgetSizeChanged(QSize size);
    void slotRemoveWidget( );
};

#endif // INNERSCROLLPANEL_H
