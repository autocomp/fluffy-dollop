#ifndef ABSTRACTDATAPROVIDER_H
#define ABSTRACTDATAPROVIDER_H

#include <QObject>

#include <QUrl>
#include <QRect>
#include <QMutex>
#include <QMap>
#include <QVariant>
#include <QIcon>

#include <QStringList>

#define DEFAULT_URL "fakepath"

namespace data_system
{

struct InParam
{
    enum InWidgetType
    {
        TrackBar, ComboBox, CheckBox
    };

    enum InParamType
    {
        Int, Double, String, List
    };

    InWidgetType typeWidget;
    InParamType type;
    QString tag_name;
    QString human_read_name;
    QString human_read_name_tr;
    QList<QString> list_val;
    double min_val;
    double max_val;
};

class MetaData : public QObject
{
QMap<QString, QVariant> m_properties;

public:
    /**
     * @brief Конструктор
     */
    MetaData();

    /**
     * @brief Метод для установки свойства
     * @param rewrite параметр, указывающий необходимость перезаписи свойства, если такое уже имеется
     */
    void setProperty(const QString &, const QVariant &, bool rewrite = true);

    /**
     * @brief Метод для получения значения свойства
     * В случае если запрашиваемое свойство существует
     * @return
     */
    bool property(QString, QVariant &) const;

    /**
     * @brief properties Метод, для получения карты свойств
     * @return карту свойств
     */
    const QMap<QString, QVariant> &properties() const
    {
        return m_properties;
    }

    void clear()
    {
        m_properties.clear();
    }
};

class AbstractDataProvider : public QObject
{
    friend class DataProviderFactory;

Q_OBJECT

QIODevice *m_device;

public:
    /**
     * @brief The StoragePolicy enum Определяет политику кеширования данных прочитанных провайдером
     */
    enum StoragePolicy
    {
        /// отсутствует - заправшиваемые данные не кешируются@n
        SP_None,
        /// присутствует по запросу - кешируются только запрашиваемые данные,@nданные кешируются в локальной памяти
        SP_OnRequestInMem,
        /// присутствует по запросу - кешируются только запрашиваемые данные,@nданные кешируются в локальном файле
        SP_OnRequestInFile,
        /// присутствует полностью - данные кешируются сразу целиком при первом обращении,@nданные кешируются в локальной памяти (принимается по умолчанию)
        SP_FullInMem,
        /// присутствует полностью - данные кешируются сразу целиком при первом обращении,@nданные кешируются в локальном файле
        SP_FullInFile
    };

    /**
     * @brief The ProviderType enum Определяет порядок работы провайдера
     */
    enum ProviderType
    {
        /// синхронный режим
        Sync,
        /// асинхронный режим
        Async
    };

    /**
     * @brief The ProviderOpen enum Определяет порядок открытия провайдера
     */
    enum ProviderOpen
    {
        /// Синхронный режим
        SyncOpen,
        /// Aсинхронный режим
        AsyncOpen
    };

    /**
     * @brief The WorkMode enum Определяет режим работы провайдера
     */
    enum WorkMode
    {
        /// провайдер читает данные
        Read,
        /// провайдер пишет данные
        Write,
        /// провайдер пишет и читает данные
        Both
    };

    /**
     * @brief Конструктор
     */
    AbstractDataProvider();

    /**
     * @brief Деструктор
     */
    virtual ~AbstractDataProvider();

    virtual QMap<QString, QStringList> outKeys() const = 0;

    virtual QList<InParam> outParameters(const QString &) const = 0;

    virtual void setMetaData(const QString &key, const QVariant &value);
    /**
     * @brief  Метод, предназначенный для получения класса свойств прочитанных данных
     * @return объект класса совйств
     */
    virtual const MetaData &metaData();

    /**
     * @brief metaData функция выполняет поиск метаданных  провайдере по ключу и
     * возвращает значение, если есть или пустой QVariant(), если таких данных нет
     * @param key
     * @return
     */
    QVariant getMetaData(const QString &key);

    /**
     * @brief  Метод для получения значения политики кеширования
     * см. StoragePolicy
     * @return значение StoragePolicy
     */
    virtual StoragePolicy storagePolicy() const = 0;

    /**
     * @brief  Метод для установки значения политики кеширования
     * После выставления политики кеширования данным методом, политика кеширования будет применена
     * @return true - если новую политику кеширования удалось применить, false - если нет
     */
    virtual bool setStoragePolicy(StoragePolicy) = 0;

    /**
     * @brief  Метод для получения порядка работы провайдера
     * @return значение, определяющее порядок работы провайдера
     */
    virtual ProviderType providerType() const = 0;

    /**
     * @brief  Метод для получения режима работы провайдера
     * @return значение, определяющее режим работы провайдера
     */
    virtual WorkMode workMode() const = 0;

    /**
     * @brief  Метод, предназначенный для получения методов доступа к данным
     * Метод, предназначен для возвращения методов доступа к данным, которые рализует данный провайдер.
     * Под методом доступа понимается левая часть URL доступа к данным
     * пр. file, http, tcp, shmem, etc
     * @return список поддерживаемых методов доступа к данным
     */
    virtual QStringList accessType() const = 0;

    /**
     * @brief  Метод, предназначенный для получения списка поддерживаемых форматов данных
     * Например это bmp, byt, jpeg, jpg etc
     * @return список поддерживаемых форматов
     */
    virtual QStringList resourceType() const = 0;

    /**
     * @brief Метод, предназначенный для открытия провайдера в синхронном режиме
     * В реализации этого метода необходимо получать от источника начальные мета-данные
     * необходимые для получения данных в методе open(const QUrl& url, WorkMode mode)
     * пр. тип изображения, заголовки HTTP, FTP etc
     *
     * @param url URL определяющий ресурс
     * @param type тип доступа к ресурсу
     * Метод предназначен проверки url на валидность, доступности при подключении etc
     * Синхронный режим применяется, например, к локальному файлу
     * @return true - если доступ к ресурсу может быть установлен, false - если нет
     */
    virtual bool open(const QUrl &url, WorkMode mode = Both) = 0;

    //! только если провайдер уже был открыт !
    virtual bool reOpen(const QUrl &url, WorkMode mode = Both) = 0;

    //! только если провайдер уже был открыт !
    virtual bool saveAs(const QUrl &url, WorkMode mode = Both) = 0;

    /**
     * @brief Метод, предназначенный для открытия провайдера в асинхронном режиме
     * @param url URL определяющий ресурс
     * @param type тип доступа к ресурсу
     * Метод предназначен проверки url на валидность, доступности при подключении etc @n
     * Синхронный режим применяется, например, к сетевому ресурсу @n
     * При успешном подключении к ресурсу отправляется сигнал ready() @n
     * @return true - если доступ к ресурсу может быть установлен, false - если нет
     */
    //    virtual bool asyncOpen(const QUrl& url, WorkMode mode = Both) = 0;

    /**
     * @brief isFinished Метод, предназначенный.
     * @return
     */
    virtual bool isFinished() = 0;

    /**
     * @brief Метод, предназначенный для получения указателя на устройство,
     * обеспечивающее доступ к данным на основе метода доступа в URL. Может возвращать 0
     * @return указатель на устройство
     */
    QIODevice *device()
    {
        return m_device;
    }

    /**
     * @brief Метод предназначенный для установки указателя на устройство,
     * ассоциированное с методом доступа к данным полученного из URL
     * Используется при создании DataProvider в DataProviderFactory::getProvider(const QUrl& url)
     * При разработке совершено не обязательно использовать этот метод...
     * @param dev указатель на устройство
     */
    void setDevice(QIODevice *dev)
    {
        m_device = dev;
    }

    //    virtual void flush() = 0;

    typedef AbstractDataProvider *(*createFucntion)();

    /**
     * Метод возвращает URL источника, с которым работает провайдер
     * если провайдер только создан и не проинициализирован путем к источнику,
     * возвращается file://fakepath
     * @brief getProviderUrl
     * @return
     */
    const QUrl &getProviderUrl() const;

    /**
     * @brief setProviderUrl  - метод устанавливает ресурс работы провайдера
     * @param url
     */
    void setProviderUrl(const QUrl &url);

    uint getProviderId() const;

protected:
    /// адрес ресурса, на который ссылается объект
    QUrl m_providerURL;

    /// метаданные ресурса
    data_system::MetaData m_metadata;

private:
    uint m_id;

signals:
    /**
     * @brief  Сигнал, осуществляющий оповещение о том, что провайдер готов работать с данными
     */
    void ready();

    /**
     * @brief Сигнал, осуществляющий опевещение о том, что провайдер завершил работу с данными
     */
    void finished();

    /**
     * @brief signalProviderDestroyed - испускается из деструктора, уведомляет об удалении провайдера (фабрике необходимо актуализаровать структуры свои данных).
     */
    void signalProviderDestroyed();

    /**
     * @brief signalProviderSaved - уведомление о сохранении \ пересохранении провайдера (фабрике необходимо актуализаровать структуры свои данных).
     */
    void signalProviderSaved();

    /**
     * @brief signalProviderChanged - уведомление об изменении провайдера (матрицы и\или метаданных(привязки)).
     */
    void signalProviderChanged();

};
}

#endif // ABSTRACTDATAPROVIDER_H
