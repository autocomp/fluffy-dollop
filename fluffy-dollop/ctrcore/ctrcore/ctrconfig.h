#ifndef CTRCONFIG_H
#define CTRCONFIG_H

#include <QMap>
#include "xml_settings.h"

#include <QFileInfoList>
#include <QStringList>

#include "params_observer.h"

//#include <unilog/unilog.h>

#define DEFAULT_APP_NAME "default"
#define PARAMETERS_OBSERVE "show_params"
#define PARAMETERS_OBSERVE_DELAY "show_params_delay"
#define PARAMETERS_OBSERVE_DELAY_DEFAULT 5000

//!
//! \brief The CtrConfig class менеджер конфигурации
//! Класс обеспечивает работу с конфигурацией. Имеетсможность доступа к параметрам
//! командной строки, а также доступа к конфигурационным файлам.
//! Системные файлы конфигурации открываются только для чтения.
//! Пользовательские файлы индивидуальны для пользователей.
//! Пользовательские файлы содержат только часть настроек, индивидульных.
//! В случае наличия совпадающих параметров в системных и пользовательских файлах настроек
//! приоритет отдается пользовательским.
//!

class CtrConfig
{
    //! Макросы переопределяют вызовы функций,
    //! для использования макросов имени функции, файла и номера строки
#define getCmdLineValue( x, y ) \
    getCmdLineValueDef( x, y, __FUNCTION__, __FILE__, __LINE__ )
#define isCmdLineArgumentPresent( x ) \
    isCmdLineArgumentPresentDef( x, __FUNCTION__, __FILE__, __LINE__ )
#define getValueByName( ... ) \
    getValueByNameDef( __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ )
#define getStringListByName( ... ) \
    getStringListByNameDef( __FUNCTION__, __FILE__, __LINE__, __VA_ARGS__ )

    //! Формы вызова без пространства CtrConfig
#define GET_CMD_LINE_VALUE( ... ) \
    CtrConfig::getCmdLineValue( __VA_ARGS__ )
#define IS_CMD_ARGUMENT_PRESENT( x ) \
    CtrConfig::isCmdLineArgumentPresent( __VA_ARGS__ )
#define GET_VALUE_BY_NAME( ... ) \
    CtrConfig::getValueByName( __VA_ARGS__ )
#define GET_STRINGLIST_BY_NAME( ... ) \
    CtrConfig::getStringListByName( __VA_ARGS__ )

public:
    CtrConfig();
    ~CtrConfig();

    /**
     * @brief setValueByName устанавливает значение конфигурации по ключу
     *
     * @param key ключ конфигурации
     * пример: "config_file.section.value"
     * изменит значение файла config_file.xml
     * следующим образом:
     * @code{.xml}
     * <root>
     *  <section>
     *   <value>$$VALUE</value>
     *  </section>
     * </root>
     * @endcode
     *
     * @param val значение, которое будет записано
     * может иметь простой тип, а также в качестве
     * аргумента можно передать QStringList
     * в таком случае файл расширит секцию по числу
     * элементов листа строк:
     * @code{.xml}
     * <root>
     *  <section>
     *   <value>$$VALUE_1</value>
     *   <value>$$VALUE_2</value>
     *   <value>$$VALUE_3</value>
     *  </section>
     * </root>
     * @endcode
     */
    static void setValueByName(QString key, QVariant val);

    /**
     * @brief getValueByName возвращает значение по ключу
     *
     * @param key ключ конфигурации (см. setValueByName)
     *
     * @param default_val значение по умолчанию
     * может иметь простой тип, или QStringList
     *
     * @return в качестве ожидаемого значения выступают
     * простые типы или QStringList (см. setValueByName)
     */
    static QVariant getValueByNameDef(QString func, QString file_name, int line,
                                      QString key, QVariant default_val = QVariant(),
                                      bool force_write = false );

    /**
     * @brief getStringListByNameDef возвращает значение по ключу,
     * преобразованное к виду QStringList, если это возможно
     *
     * @param key ключ конфигурации (см. setValueByName)
     *
     * @param default_val значение по умолчанию
     *
     * @param force_write необходимо ли записать
     * значение по умолчанию в файл, если параметра нет
     *
     * @return QStringList, если параметр существует,
     * значение по умолчанию в противном случае
     */
    static QStringList getStringListByNameDef( QString func, QString file_name, int line,
                                               QString key, QVariant default_val = QVariant() );

    /**
     * @brief isValueByNamePresent
     * проерка, есть ли параметр в конфигурационном файле
     * @param key ключ конфигурации
     * @return true - параметр присутствует
     */
    static bool isValueByNamePresent( QString key );

    /**
     * @brief checkConfigs метод для верификации состояния конф. файлов
     * @return true файлы найдены ( :) )
     */
    static bool checkConfigs();

    /**
     * @brief setCustomData
     * установка данных для внутреннего использования
     */
    static void setCustomData(QString,
                              QVariant);

    /**
     * @brief getCustomData
     * извлечение данных для внутреннего использования
     */
    static QVariant getCustomData(QString,
                              QVariant default_value = QVariant());

    /**
     * @brief isCmdLineFlagPresent метод проверки наличия заданного флага командной строки
     * @param flag - проверяемый флаг
     * @return TRUE если флаг есть
     */
    static bool isCmdLineFlagPresent(QChar flag);

    /**
     * @brief isCmdLineArgumentPresent метод проверки наличия заданного аргумента командной строки
     * @param name имя проверяемого аргумента
     * @return
     */
    static bool isCmdLineArgumentPresentDef(const QString& name, QString func, QString file, int line);

    /**
     * @brief getCmdLineValue метод получения значения аргумента командной строки по его имени
     * @param name - имя агрумента
     * @param flag - проверка успешного выполнения метода
     * @return Значение аргумента
     */
    static QString getCmdLineValueDef(const QString& name, bool& flag, QString func, QString file, int line);

    /**
     * @brief getCmdLineFlags - метод возвращает список флагов, заданных в командной строке,
     * в виде одной строки, флаги записаны подряд без разделителя
     * @return строка с флагами
     */
    static QString getCmdLineFlags();

    /**
     * @brief parseCmdLine
     * парсинг аргументов командной строки
     */
    static void parseCmdLine();

    /**
     * @brief getConfig создание объекта доступа к конфигурации
     * @return указатель на менеджер конфигурации
     */
    static CtrConfig* getConfig();

    /**
     * @brief getUserConfigPath метод возвращает путь к каталогу с конфигурационными
     * файлами приложения ~/.<application_name>
     */
    static QString getUserConfigPath();

    static QString getAppName(){return _application_name;}
private:

    /// Смартпоинтер на логгер
//    static unilog::LoggerPtr _logger;

    /**
     * @brief initialize Метод инициализации менеджера конфигураций
     * @param confFileList - список файлов с конфигурацией
     * @param is_common_mode выбор режима, системный или пользовательский
     */
    bool initialize(const QFileInfoList &confFileList, bool is_common_mode);

    /**
     * @brief addNewFile Метод добавляет в карту данные с файла конфигурации
     * @param name имя параметра
     * @param fname путь к файлу
     * @param is_common_mode выбор режима, системный или пользовательский
     */
    void addNewFile(QString name, QString fname, bool is_common_mode);

    /**
     * @brief getFileByName
     * @param name имя параметра
     * @param is_common_mode выбор режима, системный или пользовательский
     * @param is_set устновка или чтение значения
     * @return
     */
    XmlSettings* getFileByName(QString name, bool is_common_mode, bool is_set);


    /**
     * @brief initParametersObserver
     * инициализация обозревателя параметров
     */
    static void initParametersObserver();

    static void setAppName(QString name){_application_name = name;}

    void setInited(bool flag){_is_init = flag;}

    typedef QMap<QString, XmlSettings*> SettinsMap;

    SettinsMap _settings_user;
    SettinsMap _settings_common;

    static QMap <QString, QVariant> m_custom_data;
    static QMap<QString, QString> m_argumentsList;

    static QString m_flagList;
    bool _is_init;

    static QString _application_name;

    static ParamsObserver _params_observer;
    static int _params_observer_verbose;
    static int _params_observer_delay;
};

#endif // CTRCONFIG_H
