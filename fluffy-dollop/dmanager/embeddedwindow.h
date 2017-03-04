#ifndef EMBEDDEDWINDOW_H
#define EMBEDDEDWINDOW_H

#include <QMenuBar>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QWidget>


#include "embeddedapp.h"
#include "embeddedstruct.h"
#include "embeddedwidget.h"


namespace ew {
class EmbeddedApp;
class EmbeddedMainWidget;
class EmbeddedGroupWidget;

class EmbeddedWindow : public EmbeddedWidget
{
    Q_OBJECT

    friend class EmbeddedMainWidget;
    friend class EmbeddedApp;

    enum MoveOrientation
    {
        MO_LEFT = 0,
        MO_RIGHT
    };

protected:
    /*--переменные по resize и move окна*/

    bool m_denyGeometryChange, m_denyMinimize, m_denyClose;
    QString m_embendedStylesheet;
    QRect m_geom;
    QSize m_minSizeBeforeRecord;
    QString m_settingsPath;

private:
    bool  moveMidgetsRecursive(QRect wantedRect, quint64 midgetId, const QList<int> & sortedWeightList);
    QRect convertWeightToGeom(int weight, QSize sz);
    int   convertGeomToWeight(QRect r);
    void  moveMidgets(QRect wantedRect, quint64 midgetId);
    void  moveMidgetsRecursive(QRect wantedRect, quint64 midgetId, QList<int> sortedWeightList);
    void  checkMidgetPositions(QRect wantedRect, quint64 midgetId);

protected:
    explicit EmbeddedWindow(QWidget *parent = 0);
    virtual ~EmbeddedWindow();
    void hideEvent(QHideEvent *e);
    void resizeEvent(QResizeEvent *);
    void closeEvent(QCloseEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

    //void paintEvent(QPaintEvent*);

public:
    void addEmbeddedGroup(quint64 embId, ew::EmbeddedGroupWidget *wdg, ew::EmbeddedGroupStruct *eStruct);

    /** См описание sw::EmbendedWidgetStruct

     * @brief addEmbeddedWidget - добавляет внешений виджет в окно.
     * @param str
     */
    void addEmbeddedWidget(quint64 embId, ew::EmbeddedWidget *wdg, EmbeddedWidgetBaseStruct *eStruct);
    void setMenuBar(QMenuBar *menuBar);

    /**
     * @brief Устанавливает строчку в виджет ожидания, если он активен
     * @param str - выводимая строка
     * @param isFinalState - если true, то у виджета появляется кнопка,
     *при помощи которой его можно закрыть
     */
    void setWaitState(QString str, bool isFinalState);

    /**
     * @brief Функция установки главного виджета(виджет карты для 2D или виджет глобуса для 3D)
     * @param mainViewWidget
     */
    virtual void               setWidget(EmbeddedWidgetBaseStructPrivate *embStrPriv);
    virtual void               setBottomBar(QWidget *bottomBar);
    virtual void               setHeader(const EmbeddedHeaderStruct &headerStruct);
    virtual EmbeddedWidgetType widgetType();

    /*virtual*/ QRect getParkingRect();

protected slots:
    void slotEmbendedVisibleChangedInternal(bool isVisisble);
    void slotEmbendedVisibleChanged(bool isToogled);

    /// DEPRECATED
    void slotEmbeddedWidgetDeleted();
    void slotEmbendedTitleChanged(QString titleText);
    void slotWidgetMovePolicyChanged(quint64 id, bool isMoveOnParent);
    void slotEmbendedWidgetClosed();

    //void slotColWidgetInsInScrollPanel(EmbeddedWidget*);
    void slotLockCollapsedWidget(bool);
    void slotPinCollapsedWidget(bool);
    void slotUpdateChildsGeometry();

    /**
     * @brief Слот вызывается при изменении положения окна встроенного виджета
     */
    void slotColWidgetPosChanged(quint64 wId, QPoint p, QSize sz);
    void slotEmbendedCollapseChanged(quint64 wId, bool res);
    void setVisible(bool visible);
    void slotPlacedMidget(QRect rect);
    void slotMidgetMoveRequest(QRect midgetRect);

private:
public slots:
    void slotReloadButtonClicked();

signals:
    void signalEmbWidgetSettingsChanged(ew::EmbeddedWidgetBaseStruct *str);

    /**
     * @brief Сигнал для менежера конфигураций визуализаторов на загрузку сессии
     */
    void signalBtnLoadSessionClicked();

    /**
     * @brief Сигнал для менежера конфигураций визуализаторов на сохранение сессии
     */
    void signalBtnSaveSessionClicked();
};
}

#endif // EmbeddedWindow_H
