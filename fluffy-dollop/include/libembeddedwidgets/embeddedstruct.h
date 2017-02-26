#ifndef EMBEDDEDSTRUCT_H
#define EMBEDDEDSTRUCT_H

#include <QList>
#include <QPointF>
#include <QSize>
#include <QString>

class QWidget;
class QMenuBar;

namespace ew {
class EmbeddedSubIFace;
class EmbeddedApp;
class EmbeddedWidget;


/**
 * @brief Тип сериализатора
 */
enum SerializerType
{
    ST_SQLITE   /// SQLITE сериализатор
};

/**
 * @brief Секция подсветки виджета.
 */
enum HighlightSection
{
    HS_NO_HIGHLIGHT = 0,
    HS_TOP_LEFT,
    HS_TOP_RIGHT,
    HS_BOTTOM_RIGHT,
    HS_BOTTOM_LEFT,
    HS_LEFT,
    HS_TOP,
    HS_RIGHT,
    HS_BOTTOM,
    HS_CENTER
};
enum EmbeddedWidgetAlign
{
    EWA_IN_LEFT_TOP_CORNER_OFFSET = 0,      /*привязка виджета относительно левого верхнего угла с фиксированным смещением*/
    EWA_IN_LEFT_BOTTOM_CORNER_OFFSET,   /*привязка виджета относительно левого нижнего угла с фиксированным смещением*/
    EWA_IN_RIGHT_TOP_CORNER_OFFSET,     /*привязка виджета относительно правого верхнего угла с фиксированным смещением*/
    EWA_IN_RIGHT_BOTTOM_CORNER_OFFSET,  /*привязка виджета относительно правого нижнего угла с фиксированным смещением*/
    EWA_IN_PERCENTAGE_ALIGN,            /*привязка виджета с динамическим смещением(процент от ширины/высоты виджета, привязка по центру)*/
    EWA_OUT_FREE_WIDGET,                /*виджет свободно перемещется по экрану*/
    EWA_OUT_LEFT_TOP_CORNER_OFFSET,     /*привязка виджета относительно верхнего левого верхнего угла окна родителя и правого верхнего угла виджета с фиксированным смещением*/
    EWA_OUT_LEFT_BOTTOM_CORNER_OFFSET,  /*привязка виджета относительно левого нижнего угла окна и правого нижнего угла виджета с фиксированным смещением*/
    EWA_OUT_RIGHT_TOP_CORNER_OFFSET,    /*привязка виджета относительно правого верхнего угла окна и левого верхнего угла виджета с фиксированным смещением*/
    EWA_OUT_RIGHT_BOTTOM_CORNER_OFFSET  /*привязка виджета относительно правого нижнего угла окна и левого нижнего угла виджета с фиксированным смещением*/
};
enum EmbeddedHeaderAlign
{
    EHA_LEFT,
    EHA_RIGHT,
    EHA_TOP,
    EHA_BOTTOM
};
enum EmbeddedWidgetType
{
    EWT_WIDGET = 0,
    EWT_SERVICE_WIDGET,
    EWT_WINDOW,
    EWT_MAINWINDOW,
    EWT_GROUP
};
enum EmbeddedGroupType
{
    EGT_WIDGET_GROUP = 0,
    EGT_WINDOW_GROUP
};

/**
 * @brief Политика изменениния размеров виджета, встроенного в EmbeddedWidget
 */
enum EmbeddedSizePolicy
{
    ESP_EXPAND_FREE = 0, ///< виджет свободно изменяется, занимает всё предоставленное пространство
    ESP_FIXED, ///< виджет не меняет своих размеров
    ESP_EXPAND_PROPORTIONAL ///< виджет занимает всё предоставленное пространство, пропорции виджета не меняются
};



class EmbeddedHeaderStruct
{
    friend class EmbeddedWidgetStruct;

public:
    bool hasCloseButton;
    //bool hasVisibilityButton;
    bool hasGlobalMoveButton;
    //bool hasVisibilityCheckbox;
    bool hasCollapseButton;
    bool hasPinButton;
    bool hasWhatButton;
    bool hasLockButton;
    bool hasMinMaxButton;
    bool hasSettingsButton;
    bool hasStyleButton;
    bool hasMidgetButton;
    bool hasHideHeaderButton;
    //bool hideHeader;
    QString descr;
    QString windowTitle;
    QString tooltip;
    QString headerPixmap;
    QString visibleCbText;              ///< подпись к checkbox  в панели "видимость"
    EmbeddedHeaderAlign align;


    EmbeddedHeaderStruct();
    EmbeddedHeaderStruct(const EmbeddedHeaderStruct & prev);
    EmbeddedHeaderStruct & operator=(const EmbeddedHeaderStruct & prev);
    bool operator                  !=( const EmbeddedHeaderStruct & prev ) const;
    bool operator                  ==( const EmbeddedHeaderStruct & prev ) const;
    bool                           equal( const EmbeddedHeaderStruct & prev ) const;
};



/**
 * @brief ew::EmbeddedStruct  - структура для передачи в качестве параметра в addEmbeddedWidget
 */
class EmbeddedWidgetBaseStruct
{
public:
    EmbeddedWidgetBaseStruct();
    ew::EmbeddedWidgetBaseStruct & operator=(const ew::EmbeddedWidgetBaseStruct & prev);
    bool operator                          !=( const EmbeddedWidgetBaseStruct & prev ) const;
    virtual bool                           equal( const EmbeddedWidgetBaseStruct & prev ) const;
    bool operator                          ==( const EmbeddedWidgetBaseStruct & prev ) const;

    EmbeddedWidgetBaseStruct(const EmbeddedWidgetBaseStruct & prev);
    virtual ~EmbeddedWidgetBaseStruct(){}

    EmbeddedWidgetAlign alignType; ///<  точка выравнивания/расстояния по x и y. Значения трактуются в зависимости от
    QPointF alignPoint;             ///<  точка привязки угла EmbendedWidgetAlign
    QString widgetTag;              ///<  tag встраиваемого плагина
    bool allowMoving;               ///<  разрешить перемещать окно.
    bool allowChangeParent;         ///<  разрешить менять родителя окну
    bool collapsed;                 ///<  виджет свернут
    bool addHided;                  ///<  добавлять скрытым
    QSize size;                     ///<  размеры виджета
    QSize minSize;                  ///<  минимальный размер окна
    QSize maxSize;                  ///<  максимальный размер окна
    bool autoAdjustSize;            ///<  при установке флага виджет будет подстраивать свои размеры, в зависимости от контента(в тч при изменении контента).
    bool locked;                    ///<  виджет заблокирован / отображается всегда и не учавствует в удалении
    bool isModal;                   ///<  установка модальности окну. У модальных окон parent - фиктивный. Используется parent только при первом запуске,
                                    ///<  для размещения виджета в центре родителя. При дальнейших запусках, берутся x и y, сохраненные в БД.
    bool isModalBlock;              ///<  если окно не назначено модальным,то флаг силы не имеет. true - визуализация окна блокирует выполнение кода(по умолчанию)
                                    ///<  false - блокировки выполнения не происходит
    bool headerVisible;             ///<  видимость заголовка окна
    bool topOnHint;                 ///<  реализация флага Qt::WindowStaysOnTopHint
    bool allowGrouping;          ///<  флаг разрешающий группировку виджетов между собой с созданием новой панели
    bool maximized;                 /* флаг, при котором окно принимает размеры экрана. alignPoint
                                       и size при этом не меняются*/
    QString stylesheetFile;         ///<  имя файла qss в виде name.qss
    EmbeddedSizePolicy szPolicy;
    EmbeddedHeaderStruct header;
};

/**
 * @brief ew::EmbeddedStruct  - структура для передачи в качестве параметра в addEmbeddedWidget
 */
class EmbeddedWidgetStruct : public EmbeddedWidgetBaseStruct
{
public:
    EmbeddedWidgetStruct();
    ew::EmbeddedWidgetStruct & operator=(const ew::EmbeddedWidgetStruct & prev);
    bool operator                      !=( const EmbeddedWidgetStruct & prev ) const;
    bool                               equal( const EmbeddedWidgetStruct & prev ) const;
    bool operator                      ==( const EmbeddedWidgetStruct & prev ) const;

    EmbeddedWidgetStruct(const EmbeddedWidgetStruct & prev);

    EmbeddedSubIFace *iface;
    //QWidget * pluginWidget;                       // виджет
    //quint64 object_id;              // id привязанного объекта(актуально только для модулей управления и редактирования)
};



class EmbeddedGroupStruct : public EmbeddedWidgetBaseStruct
{
    friend class EmbeddedApp;
    friend class EmbeddedMainWindow;
    friend class EmbeddedWindow;
    friend class EmbeddedWidgetStructPrivate;

public:
    EmbeddedGroupStruct();
    ew::EmbeddedGroupStruct & operator=(const ew::EmbeddedGroupStruct & prev);
    bool operator                     !=( const EmbeddedGroupStruct & prev ) const;
    bool                              equal( const EmbeddedGroupStruct & prev ) const;
    bool operator                     ==( const EmbeddedGroupStruct & prev ) const;

    EmbeddedGroupStruct(const EmbeddedGroupStruct & prev);

    QList<EmbeddedSubIFace *> m_widgets;
};


class EmbeddedWindowStruct : public EmbeddedGroupStruct
{
    friend class EmbeddedApp;
    friend class EmbeddedMainWindow;
    friend class EmbeddedWindow;
    friend class EmbeddedWidgetStructPrivate;

protected:
public:
    EmbeddedWindowStruct();
    ew::EmbeddedWindowStruct & operator=(const ew::EmbeddedWindowStruct & prev);
    bool operator                      !=( const EmbeddedWindowStruct & prev ) const;
    bool                               equal( const EmbeddedWindowStruct & prev ) const;
    bool operator                      ==( const EmbeddedWindowStruct & prev ) const;

    EmbeddedWindowStruct(const EmbeddedWindowStruct & prev);

    EmbeddedSubIFace *iface;
};



class EmbeddedMainWindowStruct : public EmbeddedGroupStruct
{
    friend class EmbeddedApp;
    friend class EmbeddedMainWindow;
    friend class EmbeddedWindow;
    friend class EmbeddedWidgetStructPrivate;

public:
    EmbeddedMainWindowStruct();
    ew::EmbeddedMainWindowStruct & operator=(const ew::EmbeddedMainWindowStruct & prev);
    bool operator                          !=( const EmbeddedMainWindowStruct & prev ) const;
    bool                                   equal( const EmbeddedMainWindowStruct & prev ) const;
    bool operator                          ==( const EmbeddedMainWindowStruct & prev ) const;

    EmbeddedMainWindowStruct(const EmbeddedMainWindowStruct & prev);

    QMenuBar *menuBar;
    QWidget *bottomWidget;
};
}

#endif // EMBEDDEDSTRUCT_H
