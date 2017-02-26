#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include <QWidget>
#include <QPushButton>
#include "rowscale.h"
#include "coordbar.h"

#include "buttonboxwidget.h"
#include "minimapcontainerwidget.h"
//#include "menuopenbuttonwidget.h"
#include "visiblelistwidget.h"
#include "mainconteinerwidget.h"
#include "navigation.h"
#include "datetimewidget.h"
#include "scrollpanel.h"
#include "collapsedwidget.h"
#include <QtOpenGL/QGLWidget>
#include <QScrollArea>
#include <QSettings>
#include "rowscalecontrol.h"
#include "waitwidget.h"
#include "reachzone.h"
#include <ctrcore/visual/abstractscenewidget.h>

namespace sw {

enum BBW_BUTTON_ID
{
    /*общая панель кнопок*/
    BTN_CALC_ANGLES = 1,
    BTN_CALC_LENGHT = 2,
    BTN_CALC_REACH = 3,             // досягаемость
    BTN_CALC_SQUARE = 4,
    BTN_CALC_ROUTE = 5,
    BTN_SHOW_INVISIBLE_AREA = 6,    // отобразить области невидимости
    BTN_ANALIZE_EARTH = 7,          // анализ рельефа
    BTN_DETECT_LINE_RESOLUTION = 8,
    /*только для 3D*/
    BTN_3D_CALC_ROUTE = 9,
    BTN_3D_CALC_SUN_FROM_EARTH = 10,
    BTN_3D_CALC_REACH_ZONE = 11,

    /*только для 2D*/
    //BTN_2D_CALC_ANGLES,
    //BTN_2D_CALC_LENGTH,
    //BTN_2D_CALC_MAX_LENGTH,
    //BTN_2D_CALC_SQUARE,
    //BTN_2D_CALC_ROUTE,
    //BTN_2D_SELECT_SEARCH_AREA,

    /*только для 3D*/
    BTN_3D_ATMO,
    BTN_3D_CLOUDS,
    BTN_3D_COORDGRID,
    BTN_3D_MOON,
    BTN_3D_RELIEF,
    BTN_3D_STARS,
    BTN_3D_SUNLIGHT,
    BTN_3D_WATER,
    BTN_3D_WATER_DENSITY,
    BTN_3D_VIEW_SUBTERRANEAN,

    /*three planet for 3D*/
    BTN_3D_NAVIGATION_EARTH,
    BTN_3D_NAVIGATION_MOON,
    BTN_3D_NAVIGATION_MARS,

    BTN_VIS_OBJ_BROWSER,

    BTN_2D_PAINTING,
    BTN_2D_CAPTURE_WINDOW,

    BTN_VI_PICTURE_SETTINGS_MANAGER,
    BTN_VI_DETECT_INTERESTING_ZONE,
    BTN_PROPERTY,

    /* остальные кнопки(для всех)*/
    BTN_WHAT_THIS,
    BTN_SAVE,
    BTN_PRINT,
    BTN_VIDEO_RECORD,
    BTN_VIDEO_SETTINGS,

    BTN_2D_PIX_FORM_RADAR_PORTRAIT
};

enum OpennedDataType
{
    ODT_SIMLE_PICTURE,              // непривязанное изображение
    ODT_GEO_PICTURE,                // привязанное изображение
    ODT_GEO_SCENE,                  // тайловое покрытие
    ODT_3D,
    ODT_VIDEO
};



enum EmbendedWidgetAlign
{
    EWA_LEFT_OUTER_BOX,             /*добавление виджета в левое ухо*/
    EWA_RIGHT_OUTER_BOX,            /*добавление виджета в правое ухо*/
    EWA_RIGHT_INNER_BOX,            /*добавление виджета в правый внутренний бокс*/
    EWA_LEFT_TOP_CORNER_OFFSET,     /*привязка виджета относительно левого верхнего угла с фиксированным смещением*/
    EWA_LEFT_BOTTOM_CORNER_OFFSET,  /*привязка виджета относительно левого нижнего угла с фиксированным смещением*/
    EWA_RIGHT_TOP_CORNER_OFFSET,    /*привязка виджета относительно правого верхнего угла с фиксированным смещением*/
    EWA_RIGHT_BOTTOM_CORNER_OFFSET, /*привязка виджета относительно правого нижнего угла с фиксированным смещением*/
    EWA_PERCENTAGE_ALIGN            /*привязка виджета с динамическим смещением(процент от ширины/высоты виджета)*/
};

enum EmbendedWidgetType
{
    EWT_WIDGET,
    EWT_UI_PLUGIN,
    EWT_CONTROL_PLUGIN,
    EWT_EDIT_PLUGIN
};

struct EmbendedWidgetStruct;
class VideoSettings;
class SearchOsmObjectsWidget;
class SceneWidget : public AbstractSceneWidget
{
    Q_OBJECT

public:



protected:
    bool m_loadSettingsComplete ;
    QTimer * m_pGetTimeTimer;
    bool m_contextPlaying;
    QSizePolicy m_policyBeforeRecord;
    Qt::WindowFlags m_defaultFlags;
    bool m_denyClose, m_denyGeometryChange, m_denyMinimize;
    QRect m_geom;
    QSize m_minSizeBeforeRecord;

    CollapsedWidget * m_pWaitCollapsedWidget;
    WaitWidget * m_pWaitWidget;

    CollapsedWidget * m_pOsmParamsContainerWidget;
    QString m_settingsPath;
    SearchOsmObjectsWidget * m_pSearchOsmObjectsWidget;

    VisibleListWidget * m_menuListWidget;
    ScrollPanel * m_pInnerRightScrollArea;
    ScrollPanel *m_pOuterRightScrollArea, *m_pOuterLeftScrollArea;

    CheckBoxActionWidget* listItem;
    bool m_needGrabScreen;
    QGLWidget * m_main3dWidget;
    //    MiniMapViewer* m_miniView;
    RowScale * m_rowScale = 0;
    //MenuBar *m_topMenuBar;
    CoordBar * m_coordBar = 0;
    //CollapsedWidget * m_pBookmarkCollapseWidget;
    RowScaleControl * m_pRowScaleControl;
    ReachZone * m_reachZone;
    DateTimeWidget * m_pTimeBar;

    CheckBoxActionWidget * m_kompasVisibleCB;
    CheckBoxActionWidget * m_zoomBarVisibleCB;
    CheckBoxActionWidget * m_minimapVisibleCB;
    CheckBoxActionWidget * m_coordBarVisibleCB;
    CheckBoxActionWidget * m_scaleBarVisibleCB;
    //CheckBoxActionWidget * m_bookmarkVisibleCB;
    CheckBoxActionWidget * m_timebarVisisbleCB;

    MenuButtonItem *m_pBtnPrint, *m_pBtnSave, *m_pBtnSaveSession, *m_pBtnLoadSession, *m_pBtnReloadClicked,
    *m_pBtnRecordVideo, *m_pBtnVideoSettings;

    //SearchWidget * m_searchWidget;

    MainConteinerWidget * m_mainConteinerWidget;

    ButtonBoxWidget * m_embendedBoxWidgets;
    ButtonBoxWidget * m_buttonBoxWidget1;
    ButtonBoxWidget * m_buttonBoxWidget2;
    ButtonBoxWidget * m_buttonBoxWidget3;
    ButtonBoxWidget * m_buttonBoxWidget4;


    MiniMapContainerWidget * m_minimapContainer;
    //MenuOpenButtonWidget * m_openMenuButton;
    int m_buttonSize;

    void resizeEvent(QResizeEvent *);
    QImage getScreenFrom3D(int x, int y, int w, int h);
    Navigation * m_navigation;
    QTimer * m_grabMainViewTimer;

    OpennedDataType m_opennedDataType;
    QWidget * m_mainViewWidget;


    quint64 m_embendedWidgetIdCounter;
    QMap<quint64 , sw::EmbendedWidgetStruct*> m_embeddedWidgets;
    int menuBarFixedWidth;

public:
    SceneWidget(QWidget *parent = 0);
    virtual ~SceneWidget();


    CheckBoxActionWidget * getKompasVisibleCB();
    CheckBoxActionWidget * getZoomBarVisibleCB();
    CheckBoxActionWidget * getMinimapVisibleCB();
    CheckBoxActionWidget * getCoordBarVisibleCB();
    CheckBoxActionWidget * getScaleBarVisibleCB();
    //CheckBoxActionWidget * getBookmarkVisibleCB();
    CheckBoxActionWidget * getTimebarVisibleCB();


    void setSettingsPath(QString str);


    /** См описание sw::EmbendedWidgetStruct

     * @brief addEmbeddedWidget - добавляет внешений виджет в окно.
     * @param str
     */
    void addEmbeddedWidget(EmbendedWidgetStruct str);

    void setDenyRotateKompas(bool denyRotate);
    /**
     * @brief Возвращает false, когда окно нельзя закрывать(напр при записи видео)
     * @return
     */
    bool isAllowClose();
    /**
     * @brief Устанавливает строчку в виджет ожидания, если он активен
     * @param str - выводимая строка
     * @param isFinalState - если true, то у виджета появляется кнопка,
     *при помощи которой его можно закрыть
     */
    void setWaitState(QString str, bool isFinalState);

    /**
     * @brief Устанавливает тип данных, открытые в визуализаторе.
     * @param dt тип данных, отображаемые в scenewidget
     */
    virtual void setOpennedDataType(OpennedDataType dt);


    /**
     * @brief Добавляет виджет к правой области скроллинга
     */
    void addWidgetToRightBar(CollapsedWidget *wdg);


    /**
     * @brief Возвращает указатель на текущую закладку
     * @return
     */

    /**
     * @brief Возвращает указатель на корневую папку из панели закладок
     * @return

    /**
     * @brief Возвращает виджет карты(глобуса), установленный в качестве основного
     * @return
     */
    QWidget * getMainViewWidget();

    /**
     * @brief Возвращает указатель на второй(различающийся в 2D и 3D) виджет кнопок панели задач
     * @return
     */
    ButtonBoxWidget * getTopBarButtonBoxWidget1();
    ButtonBoxWidget * getTopBarButtonBoxWidget2();
    ButtonBoxWidget * getTopBarButtonBoxWidget3();
    ButtonBoxWidget * getTopBarButtonBoxWidget4();
    ButtonBoxWidget * getTopBarButtonBoxWidget();



    /**
     * @brief Возвращает контейнер minimap для рисование поверх него 3d minimap- временное решение.
     * @return
     */
    QWidget * getMinimapContainerWidget();


    /**
     * @brief Функция установки размеров minimap
     */
    void setMinimapSize(QSize);

    MenuButtonItem * getBtnSave();
    MenuButtonItem * getBtnShowSearch();
    MenuButtonItem * getBtnPropertyObject();

    /**
     * @brief Возвращает указатель на ухо верхнего меню
     * @return
     */
    //MenuOpenButtonWidget * getOpenMenuButton();

    RowScale * getRowScale();
    RowScaleControl * getRowScaleControl();

    /**
     * @brief Возвращает указатель на виджет панели инструментов
     * @return
     */
//    MenuBar * getMenuBar();
//    void hideMenuBar();
//    void showMenuBar();

    /**
     * @brief Возвращает указатель на виджет управления переключением системы координат
     * @return
     */
    CoordBar * getCoordBar();

    Navigation * getNavigation();

    ReachZone * getReachZone();
    /**
     * @brief Функция установки главного виджета(виджет карты для 2D или виджет глобуса для 3D)
     * @param mainViewWidget
     */
    virtual void setMainViewWidget(QWidget * _mainViewWidget, bool isOpenGlContent = false);

    /**
     * @brief Функция установки виджета миникарты
     * @param mainViewWidget
     */
    virtual QWidget *setMinimapViewWidget(QWidget * minimapViewWidget);

    void setKompasVisible(bool visible);


    void emitSignalButtonClicked(int buttonId, bool isPressed);

    void setMenuBarFixedWidth(int mbfw);
    void setMenuOpenHeight(int h);

protected slots:
    void slotEmbendedVisibleChangedInternal(bool isVisisble);
    void slotEmbendedVisibleChanged(bool isToogled);
    void slotEmbeddedWidgetDeleted();

    /**
     * @brief DEPRECATED
     */
    void slotGrabMainViewWidget();
    void slotResendEventsFromKompas(QEvent*);
    void slotResendEventsFromZoom(QEvent*);
    void slotResendEventsFromNavigation(QEvent*);
    void slotResendEventsFromScrollWidget(QEvent* e);
    void slotResendEventsFromReachZone(QEvent* e);

    void slotLockCollapsedWidget(bool);
    void slotPinCollapsedWidget(bool);
    //void slotSearchTypeChanged(int reqType);


    /**
     * @brief Программная блокировка вращение компаса
     * @param denyRotate  - true - запрет вращения пользователем компаса
     */

    /**
     * @brief Нажата кнопка выделения зоны поиска
     */
    void slotWhatThisClicked();

    /**
     * @brief Нажата кнопка записи видео
     */
    void slotBtnVideoRecordChecked(bool);

    /**
     * @brief Нажата кнопка настроек сохранения видео
     */
    void slotBtnVideoSettingsClicked();

    void setVisible(bool visible);
protected:

    virtual void hideEvent(QHideEvent *e);
    virtual void moveEvent(QMoveEvent *e);
    virtual void initKompas();
    virtual void initZoomScroll();
    virtual void initCoordBar();
    virtual void initRowScale();
    virtual void initMenuBar();
    virtual void initTimeBar();
    virtual void initNavigation();
    virtual void initReachZone();
    /**
     * @brief Функция переинициализации элементов(компаса, баров и тд) sw::SceneWidget
     * Должен быть вызван после создания sw::SceneWidget
     * Тянется из-за невозможности(?) наследования scenewidget в 3D
     */
    virtual void reInit();

    void closeEvent(QCloseEvent *e);

private:
    void saveSettingsPrivate(QSettings& sett);
    void loadSettingsPrivate(QSettings& sett);

public slots:


    /**
     * @brief Сохранение умолчательных настроек видимости элементов управления
     */
    void saveSettings();

    /**
     * @brief Загрузка умолчательных настроек видимости элементов управления
     */
    void loadSettings();

    void slotSetOSMSearchCompetePercent(int percent);

    void slotOSMSearchResult(QString searchErrorString,
                             QString renderErrorString,
                             int objectFoundedCount,
                             int objectCreatedCount,
                             bool isSearchFinished);

    /**
     * @brief Показывает строку поиска в верхнем меню
     * @param vis - Показать/скрыть
     */
    //void slotShowSearchMenu(bool vis);


    void slotOSMObjectSelected(quint64 id,QPoint p,QVariantMap mp);
    /**
     * @brief Слот для выключения кнопки
     * @param buttonId - ID кнопки
     */

    void slotSwitchOffButton(int buttonId);
    /**
     * @brief Слот показывает модальное окно поиска OSM объектов
     * @param wgsRect - прямоугольная область в WGS координатах, в которой осуществляется поиск
     */
    //void slotShowSearchOSMWidget(QRectF wgsRect, bool isPoint = false);


    void slotAngleMeasureClicked();
    void slotDistanceMeasureClicked();
    void slotReachMeasureClicked();
    void slotAreaMeasureClicked();
    void slotRouteMeasureClicked();
    void slotShowInvisibleAreaClicked();
    void slotReloadButtonClicked();

    virtual void slotCoordSystem();

    void slottomeout();
    /*********************************************/
    /*************** панель управления зумом *****/
    /**********************************************/

    /**
     * @brief Сигнал оповещает подписчика об изменении положения бегунка зума.
     * @param текущее значение зума (от min до max)
     */
    virtual void slotZoomChanged(int){}

    /**
     * @brief Слот вызывается, когда пользователь отпустил кнопку мыши от элемента управления зумом
     */

    virtual void slotMouseReleaseFromZoom();

    /*************************************************************/
    /*************** панель переключения системы координат *******/
    /*************************************************************/

    /**
     * @brief Получение индекса текущей системы координат
     * @return
     */
    int getCurCoordSystem();


    /****************************************************/
    /****************Компас*******************************/
    /***************************************************/

    /**
     * @brief Слот для компаса: cрабатывает, когда пользователь повернул компас и отпустил левую кнопку мыши.
     * @param angle - угол повората. 0 - направление на север
     */
    virtual void slotOneStepRotate(qreal angle);
    /**
     * @brief Слот для компаса: cрабатывает, когда пользователь зажал левую кнопку мыши на компасе и начал вращение.
     * @param angle - угол повората. 0 - направление на север
     */
    virtual void slotContinuousRotate(qreal angle);

    /**
     * @brief Слот для компаса: cрабатывает, когда пользователь кликнул левой кнопкой мыши на одной из кнопок перемещения
     * @param orient - определяет, какая из кнопок нажата
     */
//    virtual void slotOneStepMove(Kompas::KompasOrientation orient);
    /**
     * @brief Слот для компаса: cрабатывает, когда пользователь отпустил левую кнопку мыши после удержания.
     */
    virtual void slotStopMove();

    /**
     * @brief Слот для компаса: cрабатывает, когда пользователь зажал и удерживает в течении некоторого времени(200мс) левую кнопку мыши на одной из кнопок перемещения
     * @param orient - определяет, какая из кнопок нажата
     */
//    virtual void slotStartContinuousMove(Kompas::KompasOrientation orient);

    /**
     * @brief Слот для компаса: cрабатывает, когда изменилось состояние блокировки компаса. При блокировке,
     * компас повернется на север, будет выпущен сигнал sigOneStepRotate(qreal)
     * @param blocked : true - заблокировн, false - блокировка снята
     */
    virtual void slotBlockToNorth(bool blocked);



    /*Список управления видимостью*/

    /**
     * @brief Слот для изменения видимости bookmark. Соединяет кнопки верхнего тулбара(выпадающий список видимых элементов окна).
     *Вызывается при установки/снятии галки в выпадающем списке вид
     * @param visible
     */
    // virtual void slotSetBookmarkVisible(bool visible);

    /**
     * @brief Слот для изменения видимости компаса. Соединяет кнопки верхнего тулбара(выпадающий список видимых элементов окна).
     *Вызывается при установки/снятии галки в выпадающем списке вид
     * @param visible
     */
    virtual void slotSetKompasVisible(bool visible);
    /**
     * @brief Слот для изменения видимости панели управления зумом. Соединяет кнопки верхнего тулбара(выпадающий список видимых элементов окна).
     *Вызывается при установки/снятии галки в выпадающем списке вид
     * @param visible
     */
    virtual void slotSetZoomBarVisible(bool visible);
    /**
     * @brief Слот для изменения видимости миникарты. Соединяет кнопки верхнего тулбара(выпадающий список видимых элементов окна).
     *Вызывается при установки/снятии галки в выпадающем списке вид
     * @param visible
     */
    virtual void slotSetMinimapVisible(bool visible);
    /**
     * @brief Слот для изменения видимости панели координат. Соединяет кнопки верхнего тулбара(выпадающий список видимых элементов окна).
     *Вызывается при установки/снятии галки в выпадающем списке вид
     * @param visible
     */
    virtual void slotSetCoordinateBarVisible(bool visible);
    /**
     * @brief Слот для изменения видимости виджета линейки. Соединяет кнопки верхнего тулбара(выпадающий список видимых элементов окна).
     *Вызывается при установки/снятии галки в выпадающем списке вид
     * @param visible
     */
    virtual void slotSetRowScaleVisible(bool visible);


    /**
     * @brief Слот для изменения видимости виджета времени.
     * @param visisble
     */
    virtual void slotSetTimeBarVisible(bool visible);

    /**
     * @brief Слот для установки времени и даты в виджет
     * @param timeZoneGmt - часовой пояс в формате GMT
     * @param hh - часы
     * @param mm - минуты
     * @param dd - день
     * @param min - месяц
     * @param yy - год
     */
//    void slotSetDateTime(QDateTime tm, bool isPlaying);
    void slotGetTimeTimeout();

    void removeEmbendedWidget(EmbendedWidgetStruct str);


    /************кнопки меню*************/
    /**
     * @brief Слот вызывается при клике на панели задач по иконке сохранения
     */
    virtual void slotBtnSaveClicked();
    /**
     * @brief Слот вызывается при клике на панели задач по иконке печати
     */
    virtual void slotBtnPrintClicked();

    /******************кнопки первой группы(общей)***********/

    void slotEmbendedButtonClicked(int);
    void slotBBW1ButtonClicked(int);
    void slotBBW2ButtonClicked(int);
    void slotBBW3ButtonClicked(int);
    void slotBBW4ButtonClicked(int);

    /*Слоты панели управления переключением системы координат*/

    /**
     * @brief Слот вызывается при смене пользователем отображаемой системы координат из виджета управления переключением
     * Слот должен произвести конвертацию координат и отправить их обратной в CoordBar
     * @param lattitude - широта
     * @param longtitude - долгота
     * @param height - высота
     * @param curCoordSystem - id текущей системы координат
     */
    virtual void slotCurCoordSystChanged(int curCoordSystem) ;

    virtual void slotSetCurrentNavigation(int);

    virtual void slotButtonToggle(bool);

    virtual void slotAddBookmark();

    virtual void slotSetBookmark();

    virtual void slotEditBookmark(){}

    virtual void slotEditFolder(){}

    virtual void slotDeleteBookmark(){}

    //virtual void slotCloseBookmark();

    /**
     * @brief Слот для установки текущего масштаба
     * @param meters
     */
    virtual void slotSetCurrentScaleMeters(qreal meters);

    /**
     * @brief Слот вызывается при изменении положения окна встроенного виджета
     */
    void slotColWidgetPosChanged(quint64 id, QPoint p);

    virtual void slotUpdateChildsGeometry();

signals:

    void signalEmbWidgetSettingsChanged(sw::EmbendedWidgetStruct *str);

    /**
     * @brief Сигнал для менежера конфигураций визуализаторов на загрузку сессии
     */
    void signalBtnLoadSessionClicked();


    /**
     * @brief Сигнал для менежера конфигураций визуализаторов на сохранение сессии
     */
    void signalBtnSaveSessionClicked();
    /**
     * @brief Сигнал для старта/окончания записи видео
     * @param onOff - старт/окончание записи
     * @param fileDestination - путь к директории, куда пользователь сохраняет видео (напр /home/user )
     * @param filePrefix - префикс имени видеофайла, напр videoFile.
     * @param fileFormat - формат файла, по умолчанию Auto
     * @param wdg - указатель на виджет, с которого необходимо писать видео
     */
    void signalStartVideoClicked(bool onOff, QString fileDestination, QString filePrefix, QString fileFormat, QWidget * wdg);
    void signalSaveButtonClicked();
    void signalPrintButtonClicked();
    void signalSetCurrentInterpixelDistance(qreal iPixelDist);
    void signalDeleteOSMObjects();
    void signalGetModelTime();
    /**
     * @brief Сигнал эмитится, когда одна из кнопок была нажата. Необходимо ввести сквозную нумерацию для всех кнопок sw::SceneWidget
     * @param buttonId - ID кнопки. Сквозная нумерация для всех кнопок
     * @param isPressed - Состояние тригерной кнопки. Если кнопка не триггерная, то значение isPressed не оговаривается.
     */
    void signalButtonClicked(int buttonId, bool isPressed);
    void signalPropertyObject(int buttonId, bool isPressed);
    void signalSearchOSMObjects(QMap<QString, QVariant> data);
    void sigSearchTextChanged(QString, int);

    void signalClearFindRequest();

    void signalCenterOnObjectWithZoom(quint64 id);

    void testChangeWindow();
    void testVisWindow();

    void buttonSwitchOff(int);

};


/**
 * @brief sw::EmbendedWidgetStruct  - структура для передачи в качестве параметра в addEmbeddedWidget
 */
class EmbendedWidgetStruct : public QObject
{
    Q_OBJECT

    friend class SceneWidget;
    friend class Scene2DWidget;
    friend class VisualizerConfigManager;

private:
    quint64 id;                             // заполняется системой
    MenuBarButtonBoxItem * m_btn;           // заполняется системой
    CheckBoxActionWidget * chBox;           // заполняется системой
    CollapsedWidget * collapsedWidgetPtr;   // заполняется системой



public:
    EmbendedWidgetStruct()
    {
        id = 0 ;
        embendedWidgetPtr = 0;
        collapsedWidgetPtr = 0;
        alignPoint = QPoint(0,0);
        alignType = EWA_LEFT_BOTTOM_CORNER_OFFSET;
        allowMoving = false;
        hasHideButton = false;
        hasVisibilityButton = false;
        hasVisibilityCheckbox = false;
        hasCollapseButton = false;
        hasPinButton = false;
        m_btn = 0;
        chBox = 0;
        checkedIcon = QIcon();
        uncheckedIcon = QIcon();
        tooltip.clear();
        visibleCbText.clear();
        showWindowTitle = true;
        size = QSize(0,0);
        addHided = false;
        inbuilt = true;
        visId = -1;
        wt = EWT_WIDGET;
        pluginTag = "";
        buttonBoxWidgetNum = 0;
        windowTitle = "";
        descr = "";
        hasLockButton = false;
        locked = false;
        object_id = 0;
        hasDescrButton = false;
        collapsed = false;
        hasWhatButton = false;
    }



    sw::EmbendedWidgetStruct& operator*() const
    //sw::EmbendedWidgetStruct& operator=(const sw::EmbendedWidgetStruct& prev)
    {
        sw::EmbendedWidgetStruct th = *this;/*
        id = prev.id ;
        embendedWidgetPtr = prev.embendedWidgetPtr;
        collapsedWidgetPtr = prev.collapsedWidgetPtr;
        alignPoint = prev.alignPoint;
        alignType = prev.alignType;
        allowMoving = prev.allowMoving;
        hasHideButton = prev.hasHideButton;
        hasVisibilityButton = prev.hasVisibilityButton;
        hasVisibilityCheckbox = prev.hasVisibilityCheckbox;
        hasCollapseButton = prev.hasCollapseButton;
        m_btn = prev.m_btn;
        chBox = prev.chBox;
        checkedIcon = prev.checkedIcon;
        uncheckedIcon = prev.uncheckedIcon;
        tooltip = prev.tooltip;
        visibleCbText = prev.visibleCbText;
        showWindowTitle = prev.showWindowTitle;
        addHided = prev.addHided;
        windowTitle = prev.windowTitle;
        size = prev.size;
        inbuilt = prev.inbuilt;
        visId = prev.visId;
        wt = prev.wt;
        pluginTag = prev.pluginTag;
        buttonBoxWidgetNum = prev.buttonBoxWidgetNum;
        descr = prev.descr;
        hasLockButton = prev.hasLockButton;
        locked = prev.locked;
        object_id = prev.object_id;
        hasDescrButton = prev.hasDescrButton;
        collapsed = prev.collapsed;
        hasWhatButton = prev.hasWhatButton;*/
        return th;
    }


    sw::EmbendedWidgetStruct& operator*()
    //sw::EmbendedWidgetStruct& operator=(const sw::EmbendedWidgetStruct& prev)
    {
        sw::EmbendedWidgetStruct th;
        th.id = id ;
        th.embendedWidgetPtr = embendedWidgetPtr;
        th.collapsedWidgetPtr = collapsedWidgetPtr;
        th.alignPoint = alignPoint;
        th.alignType = alignType;
        th.allowMoving = allowMoving;
        th.hasPinButton = hasPinButton;
        th.hasHideButton = hasHideButton;
        th.hasVisibilityButton = hasVisibilityButton;
        th.hasVisibilityCheckbox = hasVisibilityCheckbox;
        th.hasCollapseButton = hasCollapseButton;
        th.m_btn = m_btn;
        th.chBox = chBox;
        th.checkedIcon = checkedIcon;
        th.uncheckedIcon = uncheckedIcon;
        th.tooltip = tooltip;
        th.visibleCbText = visibleCbText;
        th.showWindowTitle = showWindowTitle;
        th.addHided = addHided;
        th.windowTitle = windowTitle;
        th.size = size;
        th.inbuilt = inbuilt;
        th.visId = visId;
        th.wt = wt;
        th.pluginTag = pluginTag;
        th.buttonBoxWidgetNum = buttonBoxWidgetNum;
        th.descr = descr;
        th.hasLockButton = hasLockButton;
        th.locked = locked;
        th.object_id = object_id;
        th.hasDescrButton = hasDescrButton;
        th.collapsed = collapsed;
        th.hasWhatButton = hasWhatButton;
        return th;
    }


    sw::EmbendedWidgetStruct& operator=(const sw::EmbendedWidgetStruct& prev)
    {
        id = prev.id ;
        embendedWidgetPtr = prev.embendedWidgetPtr;
        collapsedWidgetPtr = prev.collapsedWidgetPtr;
        alignPoint = prev.alignPoint;
        alignType = prev.alignType;
        allowMoving = prev.allowMoving;
        hasPinButton = prev.hasPinButton;
        hasHideButton = prev.hasHideButton;
        hasVisibilityButton = prev.hasVisibilityButton;
        hasVisibilityCheckbox = prev.hasVisibilityCheckbox;
        hasCollapseButton = prev.hasCollapseButton;
        m_btn = prev.m_btn;
        chBox = prev.chBox;
        checkedIcon = prev.checkedIcon;
        uncheckedIcon = prev.uncheckedIcon;
        tooltip = prev.tooltip;
        visibleCbText = prev.visibleCbText;
        showWindowTitle = prev.showWindowTitle;
        addHided = prev.addHided;
        windowTitle = prev.windowTitle;
        size = prev.size;
        inbuilt = prev.inbuilt;
        visId = prev.visId;
        wt = prev.wt;
        pluginTag = prev.pluginTag;
        buttonBoxWidgetNum = prev.buttonBoxWidgetNum;
        descr = prev.descr;
        hasLockButton = prev.hasLockButton;
        locked = prev.locked;
        object_id = prev.object_id;
        hasDescrButton = prev.hasDescrButton;
        collapsed = prev.collapsed;
        hasWhatButton = prev.hasWhatButton;
        return *this;
    }


    bool operator!= ( const EmbendedWidgetStruct & prev ) const
    {
        if(id != prev.id  ||
                embendedWidgetPtr != prev.embendedWidgetPtr ||
                collapsedWidgetPtr != prev.collapsedWidgetPtr ||
                alignPoint != prev.alignPoint ||
                alignType != prev.alignType ||
                allowMoving != prev.allowMoving ||
                hasPinButton != prev.hasPinButton ||
                hasHideButton != prev.hasHideButton ||
                hasVisibilityButton != prev.hasVisibilityButton ||
                hasVisibilityCheckbox != prev.hasVisibilityCheckbox ||
                hasCollapseButton != prev.hasCollapseButton ||
                m_btn != prev.m_btn  ||
                chBox != prev.chBox  ||
//                checkedIcon != prev.checkedIcon ||
//                uncheckedIcon != prev.uncheckedIcon ||
                tooltip != prev.tooltip ||
                visibleCbText != prev.visibleCbText ||
                showWindowTitle != prev.showWindowTitle ||
                addHided != prev.addHided ||
                windowTitle != prev.windowTitle ||
                size != prev.size ||
                inbuilt != prev.inbuilt ||
                visId != prev.visId ||
                wt != prev.wt ||
                pluginTag != prev.pluginTag ||
                buttonBoxWidgetNum != prev.buttonBoxWidgetNum ||
                descr != prev.descr ||
                hasLockButton != prev.hasLockButton ||
                locked != prev.locked ||
                object_id != prev.object_id ||
                hasDescrButton != prev.hasDescrButton ||
                collapsed != prev.collapsed ||
                hasWhatButton != prev.hasWhatButton
                )
        {
            return true;
        }

        return false;
    }


    bool equal( const EmbendedWidgetStruct & prev ) const
    {
        if(//id == prev.id &&
                //embendedWidgetPtr == prev.embendedWidgetPtr&&
                //collapsedWidgetPtr == prev.collapsedWidgetPtr&&
                alignPoint == prev.alignPoint&&
                alignType == prev.alignType&&
                allowMoving == prev.allowMoving&&
                hasPinButton == prev.hasPinButton &&
                hasHideButton == prev.hasHideButton&&
                hasVisibilityButton == prev.hasVisibilityButton&&
                hasVisibilityCheckbox == prev.hasVisibilityCheckbox&&
                hasCollapseButton == prev.hasCollapseButton&&
                //m_btn == prev.m_btn &&
                //chBox == prev.chBox &&
//                checkedIcon == prev.checkedIcon&&
//                uncheckedIcon == prev.uncheckedIcon&&
                tooltip == prev.tooltip&&
                visibleCbText == prev.visibleCbText&&
                showWindowTitle == prev.showWindowTitle&&
                addHided == prev.addHided&&
                windowTitle == prev.windowTitle&&
                size == prev.size&&
                inbuilt == prev.inbuilt&&
                visId == prev.visId&&
                wt == prev.wt&&
                pluginTag == prev.pluginTag&&
                buttonBoxWidgetNum == prev.buttonBoxWidgetNum&&
                descr == prev.descr&&
                hasLockButton == prev.hasLockButton&&
                locked == prev.locked&&
                object_id == prev.object_id&&
                hasDescrButton == prev.hasDescrButton&&
                collapsed == prev.collapsed&&
                hasWhatButton == prev.hasWhatButton
                )
        {
            return true;
        }

        return false;
    }

    bool operator == ( const EmbendedWidgetStruct & prev ) const
    {
        if(id == prev.id &&
                embendedWidgetPtr == prev.embendedWidgetPtr&&
                collapsedWidgetPtr == prev.collapsedWidgetPtr&&
                alignPoint == prev.alignPoint&&
                alignType == prev.alignType&&
                allowMoving == prev.allowMoving&&
                hasPinButton == prev.hasPinButton &&
                hasHideButton == prev.hasHideButton&&
                hasVisibilityButton == prev.hasVisibilityButton&&
                hasVisibilityCheckbox == prev.hasVisibilityCheckbox&&
                hasCollapseButton == prev.hasCollapseButton&&
                m_btn == prev.m_btn &&
                chBox == prev.chBox &&
//                checkedIcon == prev.checkedIcon&&
//                uncheckedIcon == prev.uncheckedIcon&&
                tooltip == prev.tooltip&&
                visibleCbText == prev.visibleCbText&&
                showWindowTitle == prev.showWindowTitle&&
                addHided == prev.addHided&&
                windowTitle == prev.windowTitle&&
                size == prev.size&&
                inbuilt == prev.inbuilt&&
                visId == prev.visId&&
                wt == prev.wt&&
                pluginTag == prev.pluginTag&&
                buttonBoxWidgetNum == prev.buttonBoxWidgetNum&&
                descr == prev.descr&&
                hasLockButton == prev.hasLockButton&&
                locked == prev.locked&&
                object_id == prev.object_id&&
                hasDescrButton == prev.hasDescrButton&&
                collapsed == prev.collapsed&&
                hasWhatButton == prev.hasWhatButton
                )
        {
            return true;
        }

        return false;
    }


    EmbendedWidgetStruct(const EmbendedWidgetStruct& prev)
    {
        id = prev.id ;
        embendedWidgetPtr = prev.embendedWidgetPtr;
        collapsedWidgetPtr = prev.collapsedWidgetPtr;
        alignPoint = prev.alignPoint;
        alignType = prev.alignType;
        allowMoving = prev.allowMoving;
        hasPinButton = prev.hasPinButton;
        hasHideButton = prev.hasHideButton;
        hasVisibilityButton = prev.hasVisibilityButton;
        hasVisibilityCheckbox = prev.hasVisibilityCheckbox;
        hasCollapseButton = prev.hasCollapseButton;
        m_btn = prev.m_btn;
        chBox = prev.chBox;
        checkedIcon = prev.checkedIcon;
        uncheckedIcon = prev.uncheckedIcon;
        tooltip = prev.tooltip;
        visibleCbText = prev.visibleCbText;
        showWindowTitle = prev.showWindowTitle;
        addHided = prev.addHided;
        windowTitle = prev.windowTitle;
        size = prev.size;
        inbuilt = prev.inbuilt;
        visId = prev.visId;
        wt = prev.wt;
        pluginTag = prev.pluginTag;
        buttonBoxWidgetNum = prev.buttonBoxWidgetNum;
        descr = prev.descr;
        hasLockButton = prev.hasLockButton;
        locked = prev.locked;
        object_id = prev.object_id;
        hasDescrButton = prev.hasDescrButton;
        collapsed = prev.collapsed;
        hasWhatButton = prev.hasWhatButton;
    }


    quint64 visId;
    QWidget * embendedWidgetPtr;    // указатель на встраиваемый виджет
    EmbendedWidgetAlign alignType;  // точка выравнивания/расстояния по x и y. Значения трактуются в зависимости от
    QPoint alignPoint;              // точка привязки угла EmbendedWidgetAlign
    bool allowMoving;               // разрешить захватывать окно. При установлении флага в заголовке окна появляется кнопка "скрепка".


    bool hasHideButton;             // окно имеет кнопку "Закрыть" при нажатии на которую происходит скрытие окна(не удаление)
    bool hasCollapseButton;         // окно имеет кнопку "Свернуть" при нажатии на которую происходит сворачивание окна(остается виден только заголовок)

    bool hasVisibilityCheckbox;     // окно имеет checkbox с подписью visibleCbText в панели "видимость".
    QString visibleCbText;          // подпись к checkbox  в панели "видимость"

    bool hasVisibilityButton;     // окно имеет кнопку "Скрыть", которая находится в указанной панели при нажатии на которую происходит скрытие окна
    int buttonBoxWidgetNum;       // номер группы кнопок, в которую будет добавлена (при установленном флаге hasVisibilityButton) кнопка
    QIcon checkedIcon;            // иконка для нажатой кнопки
    QIcon uncheckedIcon;          // иконка для отжатой кнопкиhad_hide_button

    QString tooltip;              //tooltip к кнопкам
    bool showWindowTitle;
    QString windowTitle;            // текст в заголовке окна.
    bool collapsed;                 // виджет свернут

    bool addHided;                  // добавлять скрытым
    QSize size;                     // размеры виджета
    bool inbuilt;                   // виджет появляется окном внутри визуализатора. В противном случае - внешнее немодальное окно
    QString descr;                  // справка об окне
    bool hasDescrButton;            // Кнопка со справкой
    EmbendedWidgetType wt;          //тип встраиваемого окна
    QString pluginTag;             // tag встраиваемого плагина
    bool hasLockButton;             // показывать кнопку запрещения скрытия виджета
    bool locked;                    // виджет заблокирован / отображается всегда и не учавствует в удалении
    quint64 object_id;              // id привязанного объекта(актуально только для модулей управления и редактирования)
    bool hasWhatButton;             // кнопка со справкой по модулю
    bool hasPinButton;              // имеет кнопку pin
};

}

#endif // SCENEWIDGET_H
