#ifndef EMBEDDEDAPP_H
#define EMBEDDEDAPP_H

#include "embeddedheader.h"
#include "embeddednetworkmanager.h"
#include "embeddedprivate.h"
#include "embeddedsubiface.h"
#include "serializeriface.h"
#include <QDir>
#include <QFileDialog>
#include <QMap>
#include <QObject>
#include <QVariantMap>


#define ewApp (ew::EmbeddedApp::instance)

class SerializerConfigBase;
namespace ew {
class EmbeddedSessionManager;
class EmbeddedMidget;
class EmbeddedApp;
class EmbeddedGroupStruct;
class EmbeddedGroupWidget;
class EmbeddedMainWindow;
class EmbeddedWidget;
class EmbeddedWindow;
class StylesheetDialog;

namespace network {
class StyleDebugger;
class DocDebugger;
//class DocViewerWidget;
}

/// \defgroup EmbeddedApp

/// \brief The EmbeddedApp class
/// \todo Document me

class EmbeddedApp : public QObject
{
    Q_OBJECT

    friend class EmbeddedWindowStruct;
    friend class EmbeddedWidgetStruct;
    friend class EmbeddedGroupStruct;
    friend class EmbeddedMainWindowStruct;
    friend class EmbeddedWidget;
    friend class EmbeddedWindow;
    friend class EmbeddedGroupWidget;
    friend class EmbeddedMainWindow;
    friend class EmbeddedPanel;
    friend class WaitController;
    friend class EmbeddedSessionManager;
    friend class ew::network::StyleDebugger;
    friend class ew::network::DocDebugger;
    //friend class ew::network::DocViewerWidget;

    EmbeddedMainWindow *m_pMainWindow;

    QMap<quint64, EmbeddedWidgetBaseStructPrivate *> m_widgetMap;
    QMap<QString, EmbeddedSerializationData> m_loadedMap;
    QMap<quint64, QMap<QString, QVariant> > m_propertyMap;

    QList<quint64> m_loadedMapIds;
    bool m_denySave;
    StylesheetDialog *m_pStyleDialog;
    QString m_mainStylesheetFile;
    quint64 m_activeWidgetId;
    quint64 m_mainWindowId;
    bool m_closeAppDenied;

    explicit EmbeddedApp(QObject *parent = 0);

public:
    static ew::EmbeddedApp *instance();

    /**
     * @brief Получение списка дочерних id по родительскому
     * @param parentId - id родительского виджета
     * @return список дочерних виджетов
     */
    QList<quint64> getIdsByParent(quint64 parentId);

    /**
     * @brief Окно информации.
     * @param parentId - id окна родителя
     * @param title - title окна
     * @param text - текст сообщения
     */
    void about(quint64 parentId, const QString & title, const QString & text );


//!    @brief Сообщение о критической ошибке.
//!    @param parentId - id окна родителя. В случае заполнения, окно появится над окном родителя, в центре
//!    @param title - заголовок окна
//!    @param text - текст сообщения
//!    @param buttons - кнопки
//!    @param defaultButton - кнопка, нажатие на которую эмулируется при закрытии окна через заголовок.
//!    @return код кнопки, на которую было призведено нажатие.

    QMessageBox::StandardButton critical(quint64 parentId, const QString & title, const QString & text
                                         , QMessageBox::StandardButtons buttons = QMessageBox::Ok
                                         , QMessageBox::StandardButton defaultButton = QMessageBox::NoButton
                                         , const QString &detailedText = "");

    /**
     * @brief Сообщение с информацией.
     * @param parentId - id окна родителя. В случае заполнения, окно появится над окном родителя, в центре
     * @param title - заголовок окна
     * @param text - текст сообщения
     * @param buttons - кнопки
     * @param defaultButton - кнопка, нажатие на которую эмулируется при закрытии окна через заголовок.
     * @return код кнопки, на которую было призведено нажатие.
     */
    QMessageBox::StandardButton information(quint64 parentId, const QString & title, const QString & text
                                            , QMessageBox::StandardButtons buttons = QMessageBox::Ok
                                            , QMessageBox::StandardButton defaultButton = QMessageBox::NoButton
                                            , QString detailText = QString(""));

    /**
     * @brief Сообщение с вопросом.
     * @param parentId - id окна родителя. В случае заполнения, окно появится над окном родителя, в центре
     * @param title - заголовок окна
     * @param text - текст сообщения
     * @param buttons - кнопки
     * @param defaultButton - кнопка, нажатие на которую эмулируется при закрытии окна через заголовок.
     * @return код кнопки, на которую было призведено нажатие.
     */
    QMessageBox::StandardButton question(quint64 parentId, const QString & title
                                         , const QString & text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton, const QString &detailedText = "");

    /**
     * @brief Сообщение с предупреждением.
     * @param parentId - id окна родителя. В случае заполнения, окно появится над окном родителя, в центре
     * @param title - заголовок окна
     * @param text - текст сообщения
     * @param buttons - кнопки
     * @param defaultButton - кнопка, нажатие на которую эмулируется при закрытии окна через заголовок.
     * @return код кнопки, на которую было призведено нажатие.
     */
    QMessageBox::StandardButton warning(quint64 parentId, const QString & title
                                        , const QString & text, QMessageBox::StandardButtons buttons = QMessageBox::Ok, QMessageBox::StandardButton defaultButton = QMessageBox::NoButton );

    /**
       @brief Функция создания главного окна оконной подсистемы
       @param str - структура с информацией о главном окне
       @return id отличный от нуля в случае успешного создания или 0 - при ошибке.
       \n<h2>Примеры: </h2>
       \link createMainWindow_example Пример создания главного окна \endlink
     */
    quint64 createMainWindow(const ew::EmbeddedMainWindowStruct & str);

    /**
     * @brief Функция создания окна оконной подсистемы
     * @param str - структура с информацией об окне
     * @param parentId - id окна родительского объекта
     * @return id отличный от нуля в случае успешного восстановления или 0 - при ошибке/отсутсвии окна в базе.
     */
    quint64 createWindow(const ew::EmbeddedWindowStruct & str, quint64 parentId = 0);

    /**
     * @brief Функция создания группы виджетов оконной подсистемы
     * @param str - структура с информацией о группе
     * @param parentId - id окна родительского объекта
     * @return id отличный от нуля в случае успешного восстановления или 0 - при ошибке/отсутсвии окна в базе.
     */
    quint64 createGroup(const ew::EmbeddedGroupStruct & str, quint64 parentId = 0);

    /**
     * @brief Функция создания группы виджетов оконной подсистемы
     * @param str - структура с информацией о группе
     * @param parentWidget - указатель на родительский виджет
     * @return - id созданной группы
     */
    quint64 createGroup(const ew::EmbeddedGroupStruct & str, QWidget *parentWidget);

    /**
     * @brief Функция создания виджета оконной подсистемы
     * @param str - структура с информацией о виджете
     * @param parentId - id окна родительского объекта
     * @return id отличный от нуля в случае успешного восстановления или 0 - при ошибке/отсутсвии виджета в базе.
     */
    quint64 createWidget(const ew::EmbeddedWidgetStruct & str, quint64 parentId = 0);

    /**
     * @brief Функция восстановления виджета приложения из базы.
     * @param tag - tag окна.
     * @param iFase - указатель на интерфейс встраиваемого виджета.
     * @return id отличный от нуля в случае успешного восстановления или 0 - при ошибке/отсутсвии окна в базе.
     */
    quint64 restoreWidget(QString tag, ew::EmbeddedSubIFace *iFase);

    /**
     * @brief Функция восстановления главного окна приложения из базы.
     * @param tag - tag окна.
     * @param menuBar - меню
     * @param bottomPanel - нижняя панель окна.
     * @return id отличный от нуля в случае успешного восстановления или 0 - при ошибке/отсутсвии окна в базе.
     */
    quint64 restoreMainWidget(QString tag, QMenuBar *menuBar, QWidget *bottomPanel );

    /**
     * @brief Функция восстановления группы из базы.
     * @param tag - tag группы.
     * @param iFase - указатель на интерфейс встраиваемого окна.
     * @return id отличный от нуля в случае успешного восстановления или 0 - при ошибке/отсутсвии окна в базе.
     */
    quint64 restoreGroup(QString tag, QWidget *parentWidget = 0);

    /**
     * @brief Функция восстановления окна приложения из базы.
     * @param tag - tag окна.
     * @param iFase - указатель на интерфейс встраиваемого окна.
     * @return id отличный от нуля в случае успешного восстановления или 0 - при ошибке/отсутсвии окна в базе.
     */
    quint64 restoreWindow(QString tag, ew::EmbeddedSubIFace *iFase);

    /**
     * @brief Удаляет отображение виджета и запись из БД. При повторном открытии - берутся настройки по умолчанию.
     * @param widgetId - id виджета
     * @return true - в случае успеха.
     */
    bool removeWidgetFromSession(quint64 widgetId);

    /**
     * @brief Удаляет отображение виджета. Запись в БД остается. При повторном открытии - берутся настройки из БД
     * @param widgetId - id виджета
     * @return true - в случае успеха.
     */
    bool removeWidget(quint64 widgetId);

    /**
     * @brief Функция осуществляет корректное завершение приложения.
     * При вызове до фактического закрытия испускается сигнал signalAppCloseRequest(), далее проверяется значения
     * флага закрытия приложения( см setCloseAppDenied(...) и isCloseDenied()): если !isCloseDenied(), то приложение
     *  закрывается.
     */
    void close();

    /**
     * @brief Функция проверяет флаг запрета закрытия приложения.
     * @return true - в случае, если выставлен флаг запрета закрытия приложения
     */
    bool isCloseDenied();

    /**
     * @brief Функция изменяет флаг закрытия приложения.
     * @param denyCloseFlag
     */
    void setCloseAppDenied(bool denyCloseFlag);

    /**
     * @brief Изменение видимости виджета
     * @param id - id виджета
     * @param visibility - флаг видимости
     */
    void setVisible(quint64 id, bool visibility);

    /**
     * @brief Проверка видимости виджета
     * @param id - id виджета
     * @return возвращает true в случае, если виджет не скрыт.
     */
    bool isVisible(quint64 id);

    /**
     * @brief Функция реализет загрузку сохраненной сессии. Загрузка асинхронная. При обнаружении нового сохраненного виджета,
     *  формируется сигнал signalSavedWidgetLoaded(...). Если параметр сигнала widget не заполняется, то окно не создается.
     */
    void loadLastSession();

    /**
     * @brief Установка title для виджета
     * @param id
     * @param titleStr
     */
    void setWidgetTitle(quint64 id, QString titleStr);

    /**
     * @brief Возвращает id связанного виджета по указателю на EmbeddedSubIFace
     * @param iFacePtr - указатель на EmbeddedSubIFace
     * @return
     */
    quint64 getWidgetIdBySubIFacePtr(const EmbeddedSubIFace *iFacePtr);

    /**
     * @brief Возвращает id виджета по его указателю
     * @param wdgPtr - указатель на встроенный виджет
     * @return
     */
    quint64 getWidgetIdByWidgetPtr(QWidget *wdgPtr);

    /**
     * @brief Инициализация оконной подсистемы (Устарело)
     * @param st - тип сериализатора
     * @param config - указатель на конфигурацию сериализатора
     * @param wm - тип окружения. WM_SERVER - сервер, целевое приложение. WM_CLIENT
     *  - приложение для отладки(qss-editor, doc-editor)
     * @param debugPort - порт для отладки(должен быть свободен)
     * @param docPath - путь к корневой папке документации. Актуально только для окружения WM_SERVER.
     * @return
     * \deprecated use 'bool init(EmbeddedInitStruct initStruct)'
     */
    bool init(ew::SerializerType st, SerializerConfigBase *config
              , ew::network::EmbeddedNetworkManager::WorkMode wm = ew::network::EmbeddedNetworkManager::WM_SERVER
              , quint16 debugPort = 20342, QString docPath = "./doc");

    //!
    //! \brief The EmbeddedInitStruct struct
    //!
    struct EmbeddedInitStruct
    {
        //! Тип сериализатора
        ew::SerializerType serializerType;
        //! указатель на конфигурацию сериализатора
        SerializerConfigBase *serializerConfigBase;
        //! тип окружения. WM_SERVER - сервер, целевое приложение. WM_CLIENT - приложение для отладки(qss-editor, doc-editor)
        ew::network::EmbeddedNetworkManager::WorkMode workMode = ew::network::EmbeddedNetworkManager::WM_SERVER;
        //! - порт для отладки(должен быть свободен)
        quint16 debugPort = 20342;
        //! путь к корневой папке документации. Актуально только для окружения WM_SERVER.
        QString docPath = "./doc";
        QString defaultStylesheet = "artStyle.qss";
//        //! Флаг "Производить запись действий пользователя"
//        bool logUserActions;
        //! Функция вывода отладочной информации
        void (*loggerFuncDebug)(QString);
        //! Функция вывода предупреждений
        void (*loggerFuncWarning)(QString);
        //! Функция сообщений о критических ошибках
        void (*loggerFuncFatal)(QString);
        //! Функция фиксации действий пользователя
        void (*loggerFuncUserActions)(QString);


        EmbeddedInitStruct ( const EmbeddedInitStruct &other );
        EmbeddedInitStruct();
    };
    bool init(ew::EmbeddedApp::EmbeddedInitStruct initStruct);

    /**
     * @brief Получение имени файла стиля главного окна приложения
     * @return имя файла в формате name.qss
     */
    QString getMainStylesheetFile();

    /**
     * @brief Получение стиля главного окна приложения
     * @return содержимое qss файла
     */
    QString getMainStylesheet();

    /**
     * @brief Получение списка досупных файлов стилей
     * @return список досупных файлов стилей
     */
    QStringList getAvailableStylesheets();

    /**
     * @brief Установка стиля приложения
     * @param str - фаил стиля приложения
     */
    void setStylesheet(QString str);

    /**
     * @brief Получение title виджета
     * @param id - id виджета
     * @return строка с title
     */
    QString getWidgetTitle(quint64 id);

    /**
     * @brief Получение сохраненных свойств окна
     * @param wId - id окна.
     * @param propName - имя свойства
     * @return значение свойства
     */
    QVariant getWidgetProperty(quint64 wId, QString propName);

    /**
     * @brief Добавление/установка свойства окну
     * @param wId - id свойства
     * @param propName
     * @param propValue
     * @return
     */
    bool setWidgetProperty(quint64 wId, QString propName, QVariant propValue);

    /**
     * @brief Функция выполняет подстройку размеров до оптимальных(минимальные размеры виджета
     * для вмещения всего содержимого с учетом установленных ограничений).
     * @param widgetId
     */
    void adjustWidgetSize(quint64 widgetId);


    /***************************Функции окна ожидания******************************/

    /**
     * @brief Функция установка состояния
     * @param txt - текст состояния
     * @return - id состояния
     */
    quint64 setWaitState(QString txt);

    /**
     * @brief Удаление состояния из стека
     * @param stateId - id удаляемого состояния
     */
    void removeWaitState(quint64 stateId);

    /**
     * @brief Возврат текста состояния
     * @param id - id состояния
     * @return текст состояния
     */
    QString getWaitText(quint64 stateId);

    /**
     * @brief Возврат активного состояния
     * @return id активного состояния
     */
    quint64 getWaitCurrentState();

    /*************************** -- Функции окна ожидания******************************/

    /**
     * @brief Смена иконки окна
     * @param id - id окна
     * @param iconPath - путь к файлу иконки
     */
    void setWidgetIcon(quint64 id, QString iconPath);

    /**
     * @brief Вовзращает id главного окна приложения
     * @return id
     */
    quint64 getMainWindowId();

    /**
     * @brief Возвращает id активного окна(только окно вехнего уровня системы ew)
     * @return id окна
     */
    quint64 getActiveWidgetId();

    /**
     * @brief Функция отображает файловый диалог на получение существующей директории
     * @param parentId - id окна родителя
     * @param caption - заголовок
     * @param dir - стартовая директория
     * @param options - набор доп флагов
     * @return выбранная директория
     */
    QString getExistingDirectory( quint64 parentId = 0, const QString & caption = QString()
                                  , const QString & dir = QString()
                                  , QFileDialog::Options options = QFileDialog::ShowDirsOnly );

    /**
     * @brief Функция отображает файловый диалог на выбор файла
     * @param parentId - id окна родителя
     * @param caption - заголовок
     * @param dir - стартовая директория
     * @param filter - фильтер отображаемых имен
     * @param selectedFilter - фильт выбираемых файлов
     * @param options - набор доп флагов
     * @return выбранный фаил
     */
    QString getOpenFileName(  quint64 parentId = 0, const QString & caption = QString()
                              , const QString & dir = QString(), const QString & filter = QString()
                              , QString *selectedFilter = 0, QFileDialog::Options options = 0 );

    /**
     * @brief Функция отображает файловый диалог на выбор файла
     * @param parentId - id окна родителя
     * @param caption - заголовок
     * @param dir - стартовая директория
     * @param filter - фильтер отображаемых имен
     * @param selectedFilter - фильт выбираемых файлов
     * @param options - набор доп флагов
     * @return выбранные фаилы
     */
    QStringList getOpenFileNames(  quint64 parentId = 0, const QString & caption = QString()
                                   , const QString & dir = QString(), const QString & filter = QString()
                                   , QString *selectedFilter = 0, QFileDialog::Options options = 0 );

    /**
     * @brief Функция отображает файловый диалог на сохранение в файл
     * @param parentId - id окна родителя
     * @param caption - заголовок
     * @param dir - стартовая директория
     * @param filter - фильтер отображаемых имен
     * @param selectedFilter - фильт выбираемых файлов
     * @param options - набор доп флагов
     * @return выбранный фаил
     */
    QString getSaveFileName(  quint64 parentId = 0, const QString & caption = QString()
                              , const QString & dir = QString(), const QString & filter = QString()
                              , QString *selectedFilter = 0, QFileDialog::Options options = 0 );

    /**
     * @brief Функция получения id родительского embedded виджета/окна по указателю на встроенный виджет
     * @return wdg - id родительского окна
     */
    quint64 getTopWidgetId(QWidget *wdg);

    /**
     * @brief Меняет видимость окна справки
     * @param vis - видимость
     */
    void setVisibleDocViewer(bool vis);

    /**
     * @brief Получение id родителя по id объекта
     * @param id объекта
     * @return id родителя
     */
    quint64 getParentId(quint64 id);
    bool    insertWidgetInGroup(quint64 groupId, quint64 moduleId, QPoint localPos = QPoint(0, 0));

    /**
     * @brief Получение rect виджета относительно родителя
     * @param id - id виджета
     * @return rect виджета в относительных координатах
     */
    QRectF getWidgetRect(quint64 id);
protected:
    /**
     * @brief Функция создания сервисного окна
     * @param str - структура
     * @return id созданного окна
     */
    quint64 createServiceWidget(const ew::EmbeddedWidgetStruct & str);


    /// -----------------Функции оповещения-----------------

    /**
     * @brief Функция вызывается при любом изменении окна.
     * @param wId - id окна
     */
    void embWidgetChanged(quint64 wId, bool forceSave = false);

    /**
     * @brief Показывает/скрывает Midget - miniview для осного виджета.
     * @param widgetId - id окна. ID окна и midget совпадает.
     * @param visisble - флаг видимости окна
     */
    void showMidget(quint64 widgetId = 0, bool visible = true);

    /**
     * @brief Показывает окно выбора стиля приложения
     * @param widgetId - id окна, для которого вызвана процедура смены stylesheet. Если id совпадает
     *  с id EmbeddedMainWindow, то смена стиля происходит для всего приложения
     */
    void showStyleWidget(quint64 widgetId = 0);

    /**
     * @brief Показывает окно настройки виджета.
     * @param widgetId - id окна, для которого требуется настройка
     */
    void showSettingsWidget(quint64 widgetId = 0);

    /** Функции поиска */

    ew::EmbeddedWidget *getWidget(quint64 id);
    ew::EmbeddedMidget *getMidget(quint64 id);
    QRect               getParentViewRect(quint64 id);

    QList<EmbeddedMidget *> getMidgetsByParent(quint64 parentId);


    QList<EmbeddedWidget *> getWidgetsByParent(quint64 parentId);

    QList<EmbeddedWidgetBaseStructPrivate *> getPrivateStructByType(ew::EmbeddedWidgetType tp);

    QList<EmbeddedWidgetBaseStructPrivate *> getPrivateStructByParent(quint64 parentId, ew::EmbeddedWidgetType tp);

    QList<EmbeddedWidgetBaseStructPrivate *> getPrivateStructByParent(quint64 parentId);
    EmbeddedWidgetBaseStructPrivate         *getPrivateStruct(quint64 id);
    EmbeddedWidgetBaseStruct                *getEmbeddedStruct(quint64 id, ew::EmbeddedWidgetType tp );
    EmbeddedWidgetBaseStruct                *getEmbeddedStruct(quint64 id);
    EmbeddedWidgetBaseStruct                *getWidgetStructById(quint64 id) const;
    EmbeddedWidgetBaseStruct                *getWidgetStructByWidgetPtr(QWidget *wdgPtr);
    EmbeddedSubIFace                        *getSubIFaceById(quint64 id);

    /**
     * @brief Получение информации о виджете, который был загружен, но не создан(или закрыт). Экземпляра такого виджета не существует,
     * однако существует соответсвующая запись в хранилище.
     * @param id - id виджета.
     * @param data - ссылка на структуру, в которую будут записаны найденные данные.
     * @return true - в случае успешного поиска.
     */
    bool getLoadedStruct(quint64 id, EmbeddedSerializationData &data);

    /**
     * @brief Функция изменяет геометрию окна/виджета на запрошенные.
     * @param id - id виджета
     * @param alignPos - относительные(относительно точки привязки) координаты виджета
     * @param widgetSize - размеры окна
     */
    void changeWidgetAlignGeometry(quint64 id, QPointF alignPos, QSize widgetSize);
    bool eventFilter(QObject *obj, QEvent *ev);

    /**
     * @brief Получение ближайшего по иерархии id EmbeddedWidget по указателю на встроенный виджета
     * @param wdg - указатель на встроенный виджет
     * @return id EmbeddedWidget или 0.
     */
    quint64 getEmbeddedWidgetByChildWidget(QWidget *wdg);

    ~EmbeddedApp();

private:
    void    changeWidgetGeometry(quint64 id, QPointF widgetPos, QSize widgetSize);
    void    changeWidgetSizeInGroup(quint64 id, const QSize &sz);
    quint64 getUniquieId();
    void    checkChildWidgets(quint64 parentId);
    bool    checkGroupAction(quint64 id, EmbeddedHeader::MovingState ms);
    bool    checkGroupCreation(quint64 id, EmbeddedHeader::MovingState ms);
    void    resizeGroup(EmbeddedWidgetBaseStructPrivate *group);
    quint64 createMainWindowPrivate(const ew::EmbeddedMainWindowStruct & str, quint64 id, bool restore = false);
    quint64 createWindowPrivate(const ew::EmbeddedWindowStruct & str, quint64 id, bool restore = false, quint64 parentId = 0);
    quint64 createGroupPrivate(const ew::EmbeddedGroupStruct & str, quint64 id, QWidget *parentWidget = nullptr, bool restore = false, quint64 parentId = 0);
    quint64 createWidgetPrivate(const ew::EmbeddedWidgetStruct & str, quint64 id, bool restore = false, quint64 parentId = 0);
    quint64 createServiceWidgetPrivate(const EmbeddedWidgetStruct &str, quint64 id, bool restore = false);

signals:
    /**
     * @brief Сигнал информирует о закрытии приложения. Испускается до закрытия.
     */
    void signalAppCloseRequest();

    /**
     * @brief Сигнал формируется при загрузке сохраненной сессии(после вызова loadLastSession(...)).
     * @param widgetTag - тег сохраненного виджета
     * @param savedMetaInfo - нетипизированная сохраненная информация по виджету
     * @param embeddedWidgetType - тип сохраненного виджета
     * @param widget - указатель на виджет. Заполняется внешней системой. Если остается нулевым, окно не восстанавливается.
     */
    void signalSavedWidgetLoaded(QString widgetTag, QVariantMap savedMetaInfo, ew::EmbeddedWidgetType embeddedWidgetType, QWidget *widget);

    /**
     * @brief Сигнал оповещает об изменениях настроек виджета
     * @param widgetId - id виджета
     * @param str - указатель на структуру
     */
    void signalWidgetSettingsChanged(quint64 widgetId, const ew::EmbeddedWidgetBaseStruct *str);

protected slots:
    /**
     * @brief Слот срабатывает при удалении/закрытии/окончании exec встроенного окна.
     * @param id - id закрываемого окна
     * @param acceptFlag - флаг разрешения действия
     */
    void slotWidgetClosed();
    void slotSessionLoaded(QString session, QMap<quint64, EmbeddedSerializationData> serialMap, QMap<quint64, QMap<QString, QVariant> > propMap);
    void slotWidgetPosChanged(quint64 id, EmbeddedHeader::MovingState ms);

private slots:
    void slotAppFocusChanged(QWidget *oldWdg, QWidget *newWdg);
};
}

/*!
   \example createMainWindow_example
   <h1>Пример создания главного окна</h1>
   \code

   quint64 mainWindowId = ewApp()->restoreMainWidget("sdm_MainWindow", 0, panelWidget);
   if(0 == mainWindowId)
   {
      ew::EmbeddedMainWindowStruct mainStr;
      mainStr.menuBar = 0;
      mainStr.minSize = QSize(300,200);
      mainStr.size = QSize(600,500);
      mainStr.widgetTag = "sdm_MainWindow";
      mainStr.bottomWidget = panelWidget;
      ew::EmbeddedHeaderStruct mainHeadStr;
      mainHeadStr.hasCloseButton = true;
      mainHeadStr.hasMinMaxButton = true;
      mainHeadStr.hasCollapseButton = true;
      mainHeadStr.hasHideHeaderButton = false;
      mainHeadStr.hasSettingsButton  = false;
      mainHeadStr.tooltip = QString(tr("SPO GDM"));
      mainHeadStr.windowTitle = QString(tr("SPO GDM"));
      mainHeadStr.headerPixmap = QString("://pic/icon_logo.png");
      mainStr.header = mainHeadStr;
      mainWindowId = ewApp()->createMainWindow(mainStr);        // Создали окно
   }


   ew::EmbeddedFace *mainFace = new ew::EmbeddedFace(m_pMainWidget);
   quint64 windowId = ewApp()->restoreWindow("sdm_MainWidget", mainFace);
   if(0 == windowId)
   {
      ew::EmbeddedWindowStruct str;
      str.iface = mainFace;
      str.widgetTag = "sdm_MainWidget";
      str.allowMoving = true;
      ew::EmbeddedHeaderStruct headStr;
      headStr.hasCloseButton = true;
      headStr.hasMinMaxButton = true;
      str.header = headStr;
      windowId = ewApp()->createWindow(str,mainWindowId);
   }
   \endcode
 */


#endif // EMBEDDEDAPP_H
