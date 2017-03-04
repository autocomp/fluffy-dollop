#ifndef VISUALCONFIGSERIALIZERIFACE_H
#define VISUALCONFIGSERIALIZERIFACE_H

#include <QMap>
#include <QObject>

#include "embeddedstruct.h"

/**
 * @brief Структура для загрузки/выгрузки данных
 */
struct EmbeddedSerializationData
{
    ew::EmbeddedWidgetBaseStruct *data;
    quint64 parent;
    quint64 id;
    ew::EmbeddedWidgetType tp;

    EmbeddedSerializationData();
    EmbeddedSerializationData(const EmbeddedSerializationData & prev);
    EmbeddedSerializationData & operator=(const EmbeddedSerializationData & prev);
    bool operator                       !=( const EmbeddedSerializationData & prev ) const;
    bool operator                       ==( const EmbeddedSerializationData & prev ) const;
};

/**
 * @brief Базовый класс для инициалиационных настроек сериализатора
 */
class SerializerConfigBase : public QObject
{
    Q_OBJECT

public:
    SerializerConfigBase(QObject *parent);
    virtual ~SerializerConfigBase(){}
};


/**
 * @brief Абстрактный интерфейс для сериализаторов
 */

class SerializerIFace : public QObject
{
    Q_OBJECT

protected:
    QString m_attachedSession;

public:
    SerializerIFace(QObject *parent = 0);
    virtual ~SerializerIFace(){}


    /**
     * @brief inited
     * @return
     */

    virtual bool inited(){return false; }

    /**
     * @brief Метод для инициализации
     * @param settingsDirPath - путь к папке с настройками комплекса
     */
    virtual bool init(SerializerConfigBase *cfg, bool *isFirstRun) = 0;

    /**
     * @brief Сохранение информации о плагине в базу
     * @param emb - структура с информацией о плагине
     * @param hasLocalId - true: visId, указанный в структуре является локальным id визуализатора.
     *  flase: visId, указанный в структуре является глобальным id визуализатора
     */
    virtual void saveWidget(EmbeddedSerializationData emb) = 0;

    /**
     * @brief Сохранение информации о плагине в базу
     * @param emb - структура с информацией о плагине
     * @param hasLocalId - true: visId, указанный в структуре является локальным id визуализатора.
     *  flase: visId, указанный в структуре является глобальным id визуализатора
     */
    virtual void saveWidgets(QList<EmbeddedSerializationData> embList) = 0;


    /**
     * @brief Удаление виджета из базы по его локальному id
     * @param inf
     */
    virtual void removeWidget(quint64) = 0;

    /**
     * @brief Удаление виджета из базы по его локальному id
     * @param inf
     */
    virtual void removeWidgets(QList<quint64> embList) = 0;

    /**
     * @brief Получение списка сессий
     * @param vt - тип визуализатора
     * @return Список имен сессий
     */
    virtual QStringList getSessions() = 0;

    /**
     * @brief Получение списка свободных(незалоченных) сессий
     * @return Список имен сессий
     */
    virtual QStringList getFreeSessions() = 0;

    /**
     * @brief Сохранение сессии. Имя сессии должно быть уникальным.
     * @param sessionName - имя сессии
     * @return true в случае успешного сохранения
     */
    virtual void saveSession(QString sessionName, QList<EmbeddedSerializationData> strList, QMap<quint64, QMap<QString, QVariant> > propMap) = 0;

    /**
     * @brief Удаление сессии
     * @param sessionName - имя сессии
     * @return true в случае успешного сохранения
     */
    virtual void removeSession(QString sessionName) = 0;


    /**
     * @brief Загрузка сохраненной сессии по её имени
     * @return список плагинов сессии
     */
    virtual bool loadSession(const QString, QMap<quint64, EmbeddedSerializationData> &, QMap<quint64, QMap<QString, QVariant> > & ) = 0;

    /**
     * @brief Функция для сохранения состояния плагинов визуализатора в сессии с дописыванием последующих изменений.
     * Пока не вызвана detachSession(или приложение не завершится), сессии будет заблокирована.
     * @param sessionName - имя сессии
     * @param inf - информация о визуализаторе
     * @return - true - если удалось заблокировать сессию данным визуализаторов, false, в ином случае
     */
    virtual bool attachSession(QString sessionName) = 0;

    /**
     * @brief Разблокировка сессии указанным визуализатором
     * @param sessionName - имя сессии
     * @param inf - информация о визуализаторе
     * @return - true - если удалось разблокировать сессию данным визуализаторов, false, в ином случае
     */
    virtual bool detachSession(QString sessionName) = 0;

    /**
     * @brief Функция очищает базу: все записи визуализаторов(не сохраненные в сессии),
     *  и привязанные к ним плагины удаляются из базы.
     */
    virtual void clearProcedure() = 0;
    virtual void saveProperties(QMap<quint64, QMap<QString, QVariant> > propMap) = 0;
    virtual void saveProperty(quint64 wId, QString propName, QVariant propValue) = 0;

signals:
public slots:
};


#endif // VISUALCONFIGSERIALIZERIFACE_H
