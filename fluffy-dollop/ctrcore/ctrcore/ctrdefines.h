#ifndef CTRDEFINES_H
#define CTRDEFINES_H

#include <QString>
#include <QDir>
#include <stdlib.h>


/// Определения для классов менеджера конфигураций
namespace ctrconfig
{
    /// Аргумент командной строки для задания пути к конфигу билиотеки objrepr
    #define ARG_OBJREPR_CONFIG "objrepr-config-path"


    /// Аргумент командной строки для задания имени сценария ГДМ
    #define ARG_OBJREPR_CONTEXT "objrepr-context"

    /// Аргумент командной строки для задания умолчального пути к TMS серверу
    #define ARG_TMS_SERVER "tms-server-address"

    /// Аргумент командной строки для задания пути к конфигу билиотеки unilog
    #define ARG_UNILOG_CONFIG "unilog-config-path"

    /// Значение переменной окружения для опеределения пути к конфигу билиотеки unilog
    #define ENV_UNILOG_CONFIG_PATH getenv("UNILOG_CONFIG_PATH")

    /// Значение переменной окружения для опеределения пути к домашнему каталогу пользователя
    #define ENV_USER_HOME_DIR getenv("HOME")

    #define COMMON_CONFIG_ROOT_DIR "/etc/"

    /// Имя конфигурационного файла приложения, в котором хранятся стартовые настройки
    /// пути к плагинам приложения
    /// путь к конфигурационному файлу соединенния с ГДМ
    #define CTR_SYSTEM_FILE_NAME "ctrsystem"

    /// Тег для подсистемы журналирования для определения компонента, отправляющего сообщение
    #define CTRCORE "CtrCore"

/**
 * @brief getCommonConfigPath функция возвращает путь с умолчальными файлами конфигурации приложения
 * @param applicationName
 * @return
 */
inline QString  getCommonConfigPath(const QString& applicationName)
{
    return QString().append(COMMON_CONFIG_ROOT_DIR).append(applicationName);
}

/**
 * @brief getUserConfigPath функция возвращает путь с файлами конфигурации приложения
 * @param applicationName
 * @return
 */
inline QString  getUserConfigPath(const QString& applicationName)
{
    return QString().append(ENV_USER_HOME_DIR).append("/." + applicationName);
}

/**
 * @brief CreateUserConfigPath функция создает каталог для хранения файлов конфигурации приложения
 * @param applicationName
 * @return
 */
inline bool CreateUserConfigPath(const QString& applicationName)
{
    QDir dir(getUserConfigPath(applicationName));
    if(!dir.exists())
        if(!dir.mkdir(dir.path()))
            return false;
    return true;
}
}

namespace objreprtypes
{
    /**
     * @brief Тип выделенной сущности(строка дерева).
     */
    enum InfoType
    {
        IT_NO_TYPE,
        IT_OBJECT,
        IT_CLASS,
        IT_CATEGORY,
        IT_LAYER,
        IT_CLASSIFICATOR,
        IT_GROUP
    };
}
#endif // CTRDEFINES_H
