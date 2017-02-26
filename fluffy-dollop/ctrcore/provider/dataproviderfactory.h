#ifndef DATAPROVIDERFACTORY_H
#define DATAPROVIDERFACTORY_H

#include "abstractdataprovider.h"
#include "abstractproviderdevice.h"

#include <QSharedPointer>
#include <QWeakPointer>
#include <QObject>

namespace data_system
{
struct ProviderData
{
    AbstractDataProvider::createFucntion create;
    AbstractDataProvider::WorkMode mode;
};

class DataProviderFactory : public QObject
{
    Q_OBJECT

typedef QMultiMap<QString, ProviderData> ProviderMap;
typedef QMap<QString, AbstractProviderDevice::createFunction> DeviceMap;

public:
    static DataProviderFactory *instance()
    {
        if(!m_fact)
        {
            m_fact = new DataProviderFactory();
        }

        return m_fact;
    }

    void registerProvider(const AbstractDataProvider *adp, AbstractDataProvider::createFucntion);
    void registerDevice(const AbstractProviderDevice *adp, AbstractProviderDevice::createFunction);

    /**
     * @brief Метод, предназначенный для создания провайдера для соответствующего ресурса
     * @param url URL ресурса
     * @param source тип источника
     * @param type тип доступа требуемого провайдера
     *
     * Если параметр source указан по умолчанию (пустой), то тип источника определяется из url
     *
     * @return требуемый провайдер, невалидный - если провайдер для данного ресурса не найден
     */
    QSharedPointer<AbstractDataProvider> createProvider(const QUrl &url, QString source = QString(), AbstractDataProvider::WorkMode mode = AbstractDataProvider::Both);


    /**
     * @brief Метод, предназначенный для получения провайдера по идентификатору
     * @param uint id идентификатор провайдера
     *
     * @return требуемый провайдер, невалидный - если провайдер для данного ресурса не найден
     */
    QSharedPointer<AbstractDataProvider> getProvider(uint id);

    /**
     * @brief Метод, предназначенный для удаления провайдера (метод open == false)
     * @param uint id идентификатор
     *
     * @return успешность удаления
     */
//    bool removeProvider(uint id);

    /**
     * @brief Метод, предназначенный для получения указателя на устройство, необходимое для получения данных
     * Формирование необходимого экземпляра происходит на основе QUrl::scheme()
     * @param url
     * @return
     */
    QIODevice *getDevice(const QUrl &url);

    /**
     * @brief getSourceFromUrl Метод, предназначенный для определения типа исто
     * @param url URL, из которого необходимо получить тип
     * @return строку со значением полученного типа ресурса
     */
    QString getSourceFromUrl(const QUrl &url);

    /**
     * @brief resourceTypes Метод, предназначенный для получения списка доступных типов провайдеров
     * @return список с доступными типами ресурсов
     */
    QStringList resourceType()
    {
        return m_resourceType;
    }

    /**
     * @brief accessTypes Метод, предназначенный для получения списка метод доступа к данным
     * @return список с методами доступа к данным
     */
    QStringList accessTypes()
    {
        return m_accessType;
    }

    /**
     * @brief Метод, предназначенный для получение короткого и полного имени драйвера по формату изображения
     * @param ext (jpg, tif, png, etc)
     * @return QStringList -> [Имя драйвера формата GDAL], [Полное имя драйвера], лиюо NULL если не найден
     */
    QStringList driverByExt(const QString &ext);

    QList<InParam> getParamByDriver(const QString &driver);

signals:

    /**
     * @brief providerDestroyed сигнал испускается при разрушении провайдера
     * @param visualSystemRasterId идентификатор растра в системе визуализации
     * @param objreprRasterId идентификатор растра в обжрепре, с которым связан разрушаемый провайдер
     */
    void providerDestroyed(uint visualSystemRasterId, uint64_t objreprRasterId);

    /**
     * @brief providerSaved сигнал испускается при сохранении \ пересохранении провайдера
     * @param id идентификатор растра в системе визуализации
     */
    void providerSaved(uint id);

    /**
     * @brief providerChanged сигнал испускается при изменении провайдера (матрицы и\или метаданных(привязки))
     * @param id идентификатор растра в системе визуализации
     */
    void providerChanged(uint id);

private slots:
    void slotProviderDestroyed();
    void slotProviderSaved();
    void slotProviderChanged();

private:
    DataProviderFactory();
    uint getNextId();

    static DataProviderFactory *m_fact;

    QMultiMap<QString, ProviderMap> m_providers1;

    DeviceMap m_devices;
    ProviderMap m_providers;

    QMap<QString, QStringList> m_ext_to_driver;
    QMap<QString, QList<InParam> > m_driver_to_param;

    QMap<QUrl, uint> m_url_to_id;
    QMap<uint, QWeakPointer<AbstractDataProvider> > m_id_to_provider;
    uint m_id_counter;

    QStringList m_resourceType;
    QStringList m_accessType;
};
}


#endif // DATAPROVIDERFACTORY_H
