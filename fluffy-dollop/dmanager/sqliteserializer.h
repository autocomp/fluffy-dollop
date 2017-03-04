#ifndef SQLITESERIALIZER_H
#define SQLITESERIALIZER_H

#include <QSqlDatabase>
#include <QSqlQuery>

#include "serializeriface.h"



class SQLiteSerializerConfig : public SerializerConfigBase
{
    Q_OBJECT

public:
    QString configDir;
    SQLiteSerializerConfig(QObject *parent);
};

/**
 * @brief Класс-синглтон для сохранения и загрузки БД плагинов.
 */
class SQLiteSerializer : public SerializerIFace
{
    Q_OBJECT

    bool m_inited;
    friend class gManager;
    friend class VisConfigManagerTest;
    const QString m_dbFileName;
    const int m_dbVersion;
    QSqlDatabase _db;
    QSqlQuery _query;

public:
    explicit SQLiteSerializer(QObject *parent = 0);
    ~SQLiteSerializer();
    bool inited();

    /**
     * @brief Метод для инициализация VisualizerConfigSerializer
     * @param settingsDirPath - путь к папке с настройками комплекса
     */
    bool init(SerializerConfigBase *cfg, bool *isFirstRun);

    /**
     * @brief Сохранение информации о плагине в базу
     * @param emb - структура с информацией о плагине
     * @param hasLocalId - true: visId, указанный в структуре является локальным id визуализатора.
     *  flase: visId, указанный в структуре является глобальным id визуализатора
     */
    virtual void saveWidget(EmbeddedSerializationData emb);

    /**
     * @brief Сохранение информации о плагине в базу
     * @param emb - структура с информацией о плагине
     * @param hasLocalId - true: visId, указанный в структуре является локальным id визуализатора.
     *  flase: visId, указанный в структуре является глобальным id визуализатора
     */
    virtual void saveWidgets(QList<EmbeddedSerializationData> embList);

    /**
     * @brief Удаление виджета из базы по его локальному id
     * @param inf
     */
    virtual void removeWidget(quint64 widgetId);

    /**
     * @brief Удаление виджета из базы по его локальному id
     * @param inf
     */
    virtual void removeWidgets(QList<quint64> idList);

    /**
     * @brief Получение списка сессий
     * @param vt - тип визуализатора
     * @return Список имен сессий
     */
    virtual QStringList getSessions();

    /**
     * @brief Получение списка свободных(незалоченных) сессий
     * @return Список имен сессий
     */
    virtual QStringList getFreeSessions();

    /**
     * @brief Сохранение сессии. Имя сессии должно быть уникальным.
     * @param sessionName - имя сессии
     * @return true в случае успешного сохранения
     */
    virtual void saveSession(QString sessionName, QList<EmbeddedSerializationData> strList, QMap<quint64, QMap<QString, QVariant> > propMap);

    /**
     * @brief Загрузка сохраненной сессии по её имени
     * @return список плагинов сессии
     */
    virtual bool loadSession(const QString sessionName
                             , QMap<quint64, EmbeddedSerializationData> & widgList,
                             QMap<quint64, QMap<QString, QVariant> > & propMap);


    /**
     * @brief Функция для сохранения состояния плагинов визуализатора в сессии с дописыванием последующих изменений.
     * Пока не вызвана detachSession(или приложение не завершится), сессии будет заблокирована.
     * @param sessionName - имя сессии
     * @param inf - информация о визуализаторе
     * @return - true - если удалось заблокировать сессию данным визуализаторов, false, в ином случае
     */

    virtual bool attachSession(QString sessionName){Q_UNUSED(sessionName); return false; }


    /**
     * @brief Разблокировка сессии указанным визуализатором
     * @param sessionName - имя сессии
     * @param inf - информация о визуализаторе
     * @return - true - если удалось разблокировать сессию данным визуализаторов, false, в ином случае
     */

    virtual bool detachSession(QString sessionName){Q_UNUSED(sessionName); return false; }

    /**
     * @brief Удаление сессии
     * @param sessionName - имя сессии
     * @return true в случае успешного сохранения
     */
    virtual void removeSession(QString sessionName);

    /**
     * @brief Функция очищает базу: все записи визуализаторов(не сохраненные в сессии),
     *  и привязанные к ним плагины удаляются из базы.
     */
    virtual void clearProcedure();
    void         saveProperties(QMap<quint64, QMap<QString, QVariant> > propMap);
    void         saveProperty(quint64 wId, QString propName, QVariant propValue);

private:
    /**
     * @brief Функция инициализации базы: создает/пересоздает(в случае разницы в версиях кода и самой базы) БД. Производит первичное заполнение
     *  и очистку
     * @param dbDirPath - путь к папке в БД.
     * @return true - в случае успешной инициализации. True так же возвращается в случае, если класс уже проинициализирован
     */
    bool initDatabase(QString dbDirPath, bool *isFirstRun);

    /**
     * @brief Конвертация из локального id визуализатора в глобальный
     * @param globalId
     * @return
     */
    quint64 localVisIdToGlobal(quint64 localId);

    /**
     * @brief Генерация уникального глобального id визуализатора
     * @return глобальный id визуализатора
     */
    quint64 getUniqueGlobalId();

    /**
     * @brief Функция создает клон визуализатора в базе со всеми плагинами
     * @param globalVisId - глобальный id визуализатора
     * @param url - url визуализатора
     * @param vt - тип визуализатора
     * @return глобальный id визуализатора
     */
    quint64 forkVisualizer(quint64 session);

    /**
     * @brief Конвертация типа виджета в строковое представление
     * @param pType - тип виджета
     * @return строковое представление типа виджета
     */
    static inline QString widgetTypeToString(ew::EmbeddedWidgetType wType);

    /**
     * @brief Конвертация типа виджета из строкового представления
     * @param str - тип виджета(строка)
     * @return типа виджета(enum)
     */
    static ew::EmbeddedWidgetType widgetTypeFromString(QString str);

    /**
     * @brief Конвертация типа выравнивания виджета в строковое представление
     * @param aType - тип выравнивания виджета
     * @return строковое представление  выравнивания типа виджета
     */
    static inline QString alignTypeToString(ew::EmbeddedWidgetAlign aType);

    /**
     * @brief Конвертация типа выравнивания виджета из строкового представления
     * @param str - тип выравнивания виджета(строка)
     * @return тип выравнивания виджета
     */
    static inline ew::EmbeddedWidgetAlign alignTypeFromString(QString str);
};

#endif // VISUALIZERCONFIGSERIALIZER_H
